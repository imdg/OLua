/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "SourceFile.h"
#include "Env.h"
#include "Lexer.h"
#include "Logger.h"

#include "CmdConfig.h"
#include "TokenReader.h"
#include "Parser.h"
#include "TextSerializer.h"
#include "FileStream.h"

#include "ARoot.h"
#include "CompleteAst.h"
#include "SymbolTable.h"
#include "SymbolWalker.h"
#include "DebugVisitor.h"
#include "TypeInfoVisitor.h"
#include "TypeDerivationVisitor.h"
#include "TextBuilder.h"
#include "LuaPlainInterp.h"
#include "MemTextStream.h"
#include "ContextValidator.h"
#include "DebugVisitor.h"
#include "ImportedScopeGroup.h"
#include "LuaPlainInterp.h"
#include "LPClassLib.h"
#include "BuildSetting.h"
namespace OL
{

STRUCT_RTTI_BEGIN(SymbolImport)
    RTTI_MEMBER(Name)
    RTTI_MEMBER(DeclFile)
    RTTI_MEMBER(IsType)
STRUCT_RTTI_END(SymbolImport)

STRUCT_RTTI_BEGIN(SymbolExport)
    RTTI_MEMBER(Name)
    RTTI_STRUCT_MEMBER(Line, CodeLineInfo)
    RTTI_MEMBER(IsType)
STRUCT_RTTI_END(SymbolExport)



RTTI_BEGIN(SourceFile)
    RTTI_MEMBER(FileName)
    RTTI_MEMBER(AstRoot, MF_External)
    RTTI_MEMBER(Symbols, MF_External)
    RTTI_MEMBER(UnbindVarCount)
    RTTI_MEMBER(UnbindTypeCount)
    RTTI_STRUCT_MEMBER_V(ImportVars, SymbolImport)
    RTTI_STRUCT_MEMBER_V(ImportTypes, SymbolImport)
    RTTI_STRUCT_MEMBER_V(Exports, SymbolExport)
RTTI_END(SourceFile)
    

SourceFile::SourceFile(OLString InFileName) 
    : FileName(InFileName), UnbindVarCount(0), UnbindTypeCount(0), Settings(nullptr)
{
    CM.SetFile(InFileName.CStr());

    ImportedScope = new ImportedScopeGroup();
}

void SourceFile::ApplyBuildSetting(BuildSetting& Setting)
{
    if(Setting.AllowUnresolvedType)
        CM.SetAsWarning(CMT_UnresolvedVar);
    
    static ECompileMsgType NilSafetyMsg[] = 
    {
        CMT_UnwrapNonnilable,
        CMT_NonnilableCoalescing,
        CMT_NonnilableMember,
        CMT_NotAcceptedNilable
    };
    static ECompileMsgType NilSafetyErrors[] = 
    {
        CMT_NilableAsIndex,
        CMT_NonnilableNotInit,
        CMT_NilableIter,
        CMT_NilableConvert,
        CMT_AssignNilToNonnilable,
        CMT_NilableCoalescingNilable,
        CMT_DerefNilable
    };

    if(Setting.NilSafety == VL_None)
    {
        for(int i = 0; i < sizeof(NilSafetyMsg) / sizeof(ECompileMsgType); i++)
        {
            CM.SetAsMute(NilSafetyMsg[i]);
        }
        for(int i = 0; i < sizeof(NilSafetyErrors) / sizeof(ECompileMsgType); i++)
        {
            CM.SetAsMute(NilSafetyErrors[i]);
        }
    }
    else if(Setting.NilSafety == VL_Warning)
    {
        for(int i = 0; i < sizeof(NilSafetyErrors) / sizeof(ECompileMsgType); i++)
        {
            CM.SetAsWarning(NilSafetyErrors[i]);
        }
    }
    Settings = &Setting;


}

void SourceFile::RunVisitor(SPtr<ARoot> Ast, RecursiveVisitor& Vis, bool EnableBreak, OLFunc<void(DebugVisitor&)> LoggerSetter)
{
    DebugVisitor Wrapper(&Vis, EnableBreak );
    LoggerSetter(Wrapper);

    Ast->Accept(&Wrapper);
}

void SourceFile::RunVisitor(SPtr<ARoot> Ast, RecursiveVisitor& Vis, bool EnableBreak)
{
    if(EnableBreak == false)
    {
        Ast->Accept(&Vis);
    }
    else
    {
        DebugVisitor Wrapper(&Vis, EnableBreak );
        Ast->Accept(&Wrapper);
    }
}

void SourceFile::AddAPISource(SPtr<SourceFile> APIFile)
{
    ImportedScope->RegisterImportedScope(APIFile);
}

bool SourceFile::DoLocalCompile()
{
    BuildSetting* CurrSetting = Settings;
    if(Settings == nullptr)
        CurrSetting = &BuildSetting::DefaultBuildSetting;
    TokenReader Tr;
    Tr.LoadFromFile(FileName.CStr());

    Lexer Lex(Tr, CM);

    Parser Par(Lex, CM);
    AstRoot = Par.Parse_Root(FileName);
    if(AstRoot == nullptr)
        return false;

    CompleteAst VisComplete;
    AstRoot->Accept(&VisComplete);

    Symbols = new SymbolTable();
    SymbolWalker VisSymbolWalker(*Symbols, CM);
    RunVisitor(AstRoot, VisSymbolWalker, false);
    if(CM.ErrorCount > 0)
        return false;

    TypeInfoVisitor VisTypeInfo(*Symbols, *CurrSetting, CM);
    RunVisitor(AstRoot, VisTypeInfo, false);
    if(CM.ErrorCount > 0)
        return false;

    ContextValidator VisContext(*Symbols, CM);
    RunVisitor(AstRoot, VisContext, false);
    if(CM.ErrorCount > 0)
        return false;   

    Symbols->Scopes[0]->BindImport(ImportedScope);
    
    Symbols->ResolveSymbolsAndTypes(CM, SRP_Local);
    if(CM.ErrorCount > 0)
        return false;

    

    // build exports and imports
    OL_ASSERT(Symbols->Scopes.Count() > 0 && Symbols->Scopes[0]->Type == ST_Global);

    SPtr<SymbolScope> RootScope = Symbols->Scopes[0];
    for(int i = 0; i < RootScope->Decls.Count(); i++)
    {
        SymbolExport ExportInfo;
        ExportInfo.Name = RootScope->Decls[i]->Name;
        ExportInfo.Line = RootScope->Decls[i]->DeclNode->Line;
        if(RootScope->Decls[i]->DeclType == DT_Class 
            || RootScope->Decls[i]->DeclType == DT_Enum 
            || RootScope->Decls[i]->DeclType == DT_Interface 
            || RootScope->Decls[i]->DeclType == DT_Alias )
            ExportInfo.IsType = true;
        else
            ExportInfo.IsType = false;

        //VERBOSE(LogMisc, T("    Export %s: %s from: %s"), ExportInfo.IsType? T("Type"):T("Variant"), RootScope->Decls[i]->Name.CStr(), FileName.CStr());
        Exports.Add(RootScope->Decls[i]->Name, ExportInfo);
    }

    
    // Add possible unresolved type name
    UnbindTypeCount = 0;
    for(int i = 0; i < RootScope->Refs.Count(); i++)
    {
        SPtr<Reference> CurrRef = RootScope->Refs[i];
        if(CurrRef->Type == Ref_UnresolvedType)
        {
            if(ImportTypes.Exists(CurrRef->Name) == false)
            {
                UnbindTypeCount++;

                SymbolImport ImportInfo;
                ImportInfo.DeclFile = nullptr;
                ImportInfo.IsType = true;
                ImportInfo.Name = CurrRef->Name;

                ImportTypes.Add(CurrRef->Name, ImportInfo);
                //VERBOSE(LogMisc, T("    Import Type: %s from: %s"), CurrRef->Name.CStr(), FileName.CStr());
            }
        }
    }
    return true;

}

bool SourceFile::DoTypeResolve()
{
    Symbols->ResolveSymbolsAndTypes(CM, SRP_GlobalType);

    // Type resolved. Now add possible unresolved variant name
    SPtr<SymbolScope> RootScope = Symbols->Scopes[0];

    UnbindVarCount = 0;
    for(int i = 0; i < RootScope->Refs.Count(); i++)
    {
        SPtr<Reference> CurrRef = RootScope->Refs[i];
        if(CurrRef->Type == Ref_Unresolved)
        {
            if(ImportVars.Exists(CurrRef->Name) == false)
            {
                UnbindVarCount++;

                SymbolImport ImportInfo;
                ImportInfo.DeclFile = nullptr;
                ImportInfo.IsType = false;
                ImportInfo.Name = CurrRef->Name;

                ImportVars.Add(CurrRef->Name, ImportInfo);

                //VERBOSE(LogMisc, T("    Import Variant: %s from: %s"), CurrRef->Name.CStr(), FileName.CStr());
            }
        }
    }

    return true;
}

void SourceFile::DoResolveAsAPI()
{
    Symbols->ResolveSymbolsAndTypes(CM, SRP_GlobalVar);
}

bool SourceFile::DoCodeGen(LPClassLib& ClassLib)
{
    Symbols->ResolveSymbolsAndTypes(CM, SRP_GlobalVar);
    //return false;
    
    if(CM.ErrorCount > 0)
        return false;

    TypeDerivationVisitor VisTypeDeri(*Symbols, CM);
    RunVisitor(AstRoot, VisTypeDeri, true);
    if(CM.ErrorCount > 0)
        return false;   

    LuaPlainInterp VisLuaPlain(*Symbols, OutText, ClassLib);
    RunVisitor(AstRoot, VisLuaPlain, false);

    MainBlock = OutText.Root;
    StaticBlock = VisLuaPlain.MakeStandaloneStaticBlock();


    return true;
}

int SourceFile::GetUnbindTypeCount()
{
    return UnbindTypeCount;
}
int SourceFile::GetUnbindVarCount()
{
    return UnbindVarCount;
}

bool SourceFile::BindType(OLString Name, SPtr<SourceFile> DeclFile)
{
    SymbolImport* Found = ImportTypes.TryFind(Name);
    if(Found == nullptr)
        return false;

    if(Found->DeclFile == nullptr)
    {
        Found->DeclFile = DeclFile;
        ImportedScope->RegisterImportedScope(DeclFile);

        UnbindTypeCount--;
        return true;
    }
    return false;

}

bool SourceFile::BindVar(OLString Name, SPtr<SourceFile> DeclFile)
{
    SymbolImport* Found = ImportVars.TryFind(Name);
    if(Found == nullptr)
        return false;

    if(Found->DeclFile == nullptr)
    {
        Found->DeclFile = DeclFile;
        ImportedScope->RegisterImportedScope(DeclFile);

        UnbindVarCount--;
        return true;
    }
    return false;

}

void SourceFile::PrintUnbindError()
{
    SPtr<SymbolScope> RootScope = Symbols->Scopes[0];
    auto ItImpTypes = ImportTypes.BeginIter();
    while(ItImpTypes.IsEnd() == false)
    {
        if(ItImpTypes.GetValue().DeclFile != nullptr)
        {
            ItImpTypes.Next();
            continue;
        }
        
        
        for(int i = 0; i < RootScope->Refs.Count(); i++)
        {
            if(RootScope->Refs[i]->Name !=  ItImpTypes.GetValue().Name)
                continue;
            
            SPtr<ABase> RefNode = RootScope->Refs[i]->RefNode.Lock();
            if(RootScope->Refs[i]->RootRef != nullptr)
                RefNode = RootScope->Refs[i]->RootRef->RefNode.Lock();
            CM.Log(CMT_UnresolvedVar, RefNode->Line, RootScope->Refs[i]->Name.CStr());
            
        }
        ItImpTypes.Next();
        
    }

    auto ItImpVars = ImportVars.BeginIter();
    while(ItImpVars.IsEnd() == false)
    {
        if(ItImpVars.GetValue().DeclFile != nullptr)
        {
            ItImpVars.Next();
            continue;
        }
        for(int i = 0; i < RootScope->Refs.Count(); i++)
        {
            if(RootScope->Refs[i]->Name !=  ItImpVars.GetValue().Name)
                continue;
            
            SPtr<ABase> RefNode = RootScope->Refs[i]->RefNode.Lock();
            if(RootScope->Refs[i]->RootRef != nullptr)
                RefNode = RootScope->Refs[i]->RootRef->RefNode.Lock();
            CM.Log(CMT_UnresolvedVar, RefNode->Line, RootScope->Refs[i]->Name.CStr());
            
        }
        ItImpVars.Next();
        
    }
}

}