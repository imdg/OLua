/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "SymbolLib.h"
#include "CompileMsg.h"
#include "SourceFile.h"

namespace OL
{
STRUCT_RTTI_BEGIN(PendingImport)
    RTTI_MEMBER(Name)
    RTTI_MEMBER(RefFile, MF_External)
STRUCT_RTTI_END(PendingImport)

STRUCT_RTTI_BEGIN(ExportDecl)
    RTTI_MEMBER(Name)
    RTTI_STRUCT_MEMBER(Line, CodeLineInfo)
    RTTI_MEMBER(DeclFile, MF_External)
STRUCT_RTTI_END(ExportDecl)


RTTI_BEGIN(SymbolLib)
    RTTI_STRUCT_MEMBER_V(ExportVarLib, ExportDecl)
    RTTI_STRUCT_MEMBER_V(ImportVarLib, PendingImport)

    RTTI_STRUCT_MEMBER_V(ExportTypeLib, ExportDecl)
    RTTI_STRUCT_MEMBER_V(ImportTypeLib, PendingImport)
RTTI_END(SymbolLib)

void SymbolLib::MatchFileTypes(SPtr<SourceFile> InputFile, OLList<SPtr<SourceFile> >& MatchedFile)
{
    TypeMatchStatus.Add(InputFile, false);
    auto ItExp = InputFile->Exports.BeginIter();
    while(ItExp.IsEnd() == false)
    {
        SymbolExport& Curr = ItExp.GetValue();
        if(Curr.IsType == false)
        {
            ItExp.Next();
            continue;
        }
        if(ExportTypeLib.Exists(Curr.Name))
        {
            InputFile->CM.Log(CMT_SymbolRedefinition, Curr.Line, Curr.Name.CStr(), ExportTypeLib[Curr.Name].DeclFile->FileName.CStr()
                , ExportTypeLib[Curr.Name].Line.Line, ExportTypeLib[Curr.Name].Line.Col);
        }
        else
        {

            ExportTypeLib.Add(Curr.Name, ExportDecl{Curr.Name, Curr.Line, InputFile});
            if(ImportTypeLib.Exists(Curr.Name))
            {
                SPtr<PendingImport> CurrImport = ImportTypeLib[Curr.Name];
                while(CurrImport != nullptr)
                {
                    SPtr<SourceFile> RefFile = CurrImport->RefFile.Lock();
                    if(RefFile->BindType(Curr.Name, InputFile) && RefFile->GetUnbindTypeCount() == 0)
                    {
                        TypeMatchStatus[RefFile] = true; 
                        MatchedFile.Add(RefFile);
                    }
                    CurrImport = CurrImport->Next;
                }
            }
        }
        ItExp.Next();
    }


    auto ItImp = InputFile->ImportTypes.BeginIter();
    while(ItImp.IsEnd() == false)
    {
        SymbolImport& Curr = ItImp.GetValue();

        if(ImportTypeLib.Exists(Curr.Name))
        {
            SPtr<PendingImport> Head = ImportTypeLib[Curr.Name];
            SPtr<PendingImport > NewItem = new PendingImport{Curr.Name, InputFile, Head};
            ImportTypeLib[Curr.Name] = NewItem;
        }
        else
        {
            SPtr<PendingImport > NewItem = new PendingImport{Curr.Name, InputFile, nullptr};
            ImportTypeLib.Add(Curr.Name, NewItem);
        }

        if(ExportTypeLib.Exists(Curr.Name))
        {
            InputFile->BindType(Curr.Name, ExportTypeLib[Curr.Name].DeclFile.Lock());


        }
        ItImp.Next();
    }
    if(InputFile->GetUnbindTypeCount() == 0)
    {
        TypeMatchStatus[InputFile] = true; 
        MatchedFile.Add(InputFile);
    }


}

void SymbolLib::MatchFileVars(SPtr<SourceFile> InputFile, OLList<SPtr<SourceFile> >& MatchedFile)
{

    // a var ref could actually be referring to a type
    // check both var lib and type lib
    // when new file comes for importing, check existing export var and type
    VarMatchStatus.Add(InputFile, false);
    auto ItExp = InputFile->Exports.BeginIter();
    
    while(ItExp.IsEnd() == false)
    {
        SymbolExport& Curr = ItExp.GetValue();
        if(Curr.IsType == true)
        {
            ItExp.Next();
            continue;
        }

        if(ExportVarLib.Exists(Curr.Name))
        {
            InputFile->CM.Log(CMT_SymbolRedefinition, Curr.Line, Curr.Name.CStr()
                , ExportTypeLib[Curr.Name].Line.Line, ExportVarLib[Curr.Name].Line.Col);
        }

        else
        {
            if(Curr.IsType == false)
                ExportVarLib.Add(Curr.Name, ExportDecl{Curr.Name, Curr.Line, InputFile});

            if(ImportVarLib.Exists(Curr.Name))
            {
                SPtr<PendingImport> CurrImport = ImportVarLib[Curr.Name];
                while(CurrImport != nullptr)
                {
                    SPtr<SourceFile> RefFile = CurrImport->RefFile.Lock();
                    if(RefFile->BindVar(Curr.Name, InputFile) && RefFile->GetUnbindVarCount() == 0)
                    {
                        VarMatchStatus[RefFile] = true;
                        MatchedFile.Add(RefFile);
                    }
                    CurrImport = CurrImport->Next;
                }
            }
        }
        ItExp.Next();
    }


    auto ItImp = InputFile->ImportVars.BeginIter();
    while(ItImp.IsEnd() == false)
    {
        SymbolImport& Curr = ItImp.GetValue();
        if(ImportVarLib.Exists(Curr.Name))
        {
            SPtr<PendingImport> Head = ImportVarLib[Curr.Name];
            SPtr<PendingImport > NewItem = new PendingImport{Curr.Name, InputFile, Head};
            ImportVarLib[Curr.Name] = NewItem;
        }
        else
        {
            SPtr<PendingImport > NewItem = new PendingImport{Curr.Name, InputFile, nullptr};
            ImportVarLib.Add(Curr.Name, NewItem);
        }

        if(ExportVarLib.Exists(Curr.Name))
        {
            InputFile->BindVar(Curr.Name, ExportVarLib[Curr.Name].DeclFile.Lock());
        }
        if(ExportTypeLib.Exists(Curr.Name))
        {
            // here use BindVar because even if it is actually a type, the file still see it as a var
            // Binding only make relations between files, seeing it as a var, bind it in a var way
            // Final resolvation will make it correct
            InputFile->BindVar(Curr.Name, ExportTypeLib[Curr.Name].DeclFile.Lock());
        }

        ItImp.Next();
    }
    if(InputFile->GetUnbindVarCount() == 0)
    {
        VarMatchStatus[InputFile] = true;
        MatchedFile.Add(InputFile);
    }


}


void SymbolLib::GetTypeUnmatched( OLList<SPtr<SourceFile> >& Files)
{
    auto It = TypeMatchStatus.BeginIter();
    while(It.IsEnd() == false)
    {
        if(It.GetValue() == false)
        {
            Files.Add(It.GetKey());
        }
        It.Next();
    }

}
void SymbolLib::GetVarUnmatched( OLList<SPtr<SourceFile> >& Files)
{
    auto It = VarMatchStatus.BeginIter();
    while(It.IsEnd() == false)
    {
        if(It.GetValue() == false)
        {
            Files.Add(It.GetKey());
        }
        It.Next();
    }
}

}