/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "LPClassLib.h"
#include "SourceFile.h"
namespace OL
{

void LPClassLib::BeginFile(OLString File)
{
    SPtr<FileClassInfo> NewFileInfo = new FileClassInfo();
    NewFileInfo->DependentCount = 0;
    NewFileInfo->FileName = File;
    
    Files.Add(File, NewFileInfo);
    CurrFile = NewFileInfo;
    CurrFileHasDependency = false;
}

void LPClassLib::EndFile()
{
    for(int i = 0; i < PendingBreak.Count(); i++)
    {
        BreakChild(PendingBreak[i]);
    }
    if(CurrFileHasDependency == false)
    {
        FinishedFile.Add(CurrFile);
    }
}

void LPClassLib::SaveClass(SPtr<LPClassHelper> ClassHelper, OLString UniqueName, OLString DependentName, bool SeperateStaticBlock)
{
    SPtr<ClassInfo> NewInfo = new ClassInfo();
    NewInfo->DependentClass = DependentName;
    NewInfo->UniqueName = UniqueName;
    NewInfo->SrcFile = CurrFile->FileName;
    NewInfo->Helper = ClassHelper;
    NewInfo->SeperateStaticBlock = SeperateStaticBlock;

    OL_ASSERT(Classes.Exists(UniqueName) == false);

    Classes.Add(UniqueName, NewInfo);
    CurrFile->InFileClasses.Add(NewInfo);

    // Self as a base class
    if(Dependency.Exists(UniqueName))
    {
        Dependency[UniqueName]->BaseClass = NewInfo;
    }
    else
    {
        SPtr<ClassDependency> NewCD = new ClassDependency();
        NewCD->BaseClassUniqueName = UniqueName;
        NewCD->BaseClass = NewInfo;
        Dependency.Add(UniqueName, NewCD);
    }
    PendingBreak.Add(NewInfo);

    if(DependentName != T(""))
    {
        CurrFileHasDependency = true;
        if(Dependency.Exists(DependentName))
        {
            Dependency[DependentName]->ChildClasses.Add(NewInfo);
            if(Dependency[DependentName]->BaseClass != nullptr)
                PendingBreak.Add(Dependency[DependentName]->BaseClass);
        }
        else
        {
            SPtr<ClassDependency> NewCD = new ClassDependency();
            NewCD->BaseClassUniqueName = DependentName;
            NewCD->ChildClasses.Add(NewInfo);
            Dependency.Add(DependentName, NewCD);

        }
        CurrFile->DependentCount++;
    }
}


void LPClassLib::BreakChild(SPtr<ClassInfo> BaseClass)
{
    // BaseClass has its own base class, cannot break
    if(BaseClass->DependentClass != T(""))
        return;

    // No one depend on this base class
    if(Dependency.Exists(BaseClass->UniqueName) == false)
        return;

    SPtr<ClassDependency> DepInfo = Dependency[BaseClass->UniqueName];

    OLList<SPtr<ClassInfo> > Childs;
    for(int i = 0; i < DepInfo->ChildClasses.Count(); i++)
    {
        DepInfo->ChildClasses[i]->Helper->ReplaceBaseDefinition(BaseClass->Helper);
        DepInfo->ChildClasses[i]->DependentClass = T("");
        Childs.Add(DepInfo->ChildClasses[i]);

        OLString ChildSrcFile = DepInfo->ChildClasses[i]->SrcFile;
        Files[ChildSrcFile]->DependentCount--;

        if(Files[ChildSrcFile]->DependentCount <= 0)
        {
            FinishedFile.Add(Files[ChildSrcFile]);
        }
    }

    DepInfo->ChildClasses.Clear();

    for(int i = 0; i < Childs.Count(); i++)
    {
        BreakChild(Childs[i]);
    }
}

void LPClassLib::ResetFinishedFile()
{
    FinishedFile.Clear();
}

void LPClassLib::ApplyClassToFile(SPtr<SourceFile> File)
{
    SPtr<FileClassInfo> FileInfo = Files[File->FileName];

    for(int i = 0; i < FileInfo->InFileClasses.Count(); i++)
    {
        SPtr<ClassInfo> CurrClass = FileInfo->InFileClasses[i];
        OLString HolderName;
        HolderName.Printf(T("__class_content_%s"), CurrClass->UniqueName.CStr());
        File->MainBlock->ReplaceHolder(HolderName, CurrClass->Helper->MainBlock, true);


        HolderName.Printf(T("__class_static_content_%s"), CurrClass->UniqueName.CStr());
        if(CurrClass->SeperateStaticBlock == true)
        {
            File->StaticBlock->ReplaceHolder(HolderName, CurrClass->Helper->StaticBlock, true);
        }
        else
        {
            File->MainBlock->ReplaceHolder(HolderName, CurrClass->Helper->StaticBlock, true);
        }
    }
}

void LPClassLib::ApplyClassToStandaloneFile(SPtr<TextParagraph> MainBlock, SPtr<TextParagraph> StaticBlock)
{
    auto ItFile = Files.BeginIter();
    while(ItFile.IsEnd() == false)
    {
        SPtr<FileClassInfo> FileInfo = ItFile.GetValue();
        for(int i = 0; i < FileInfo->InFileClasses.Count(); i++)
        {
            SPtr<ClassInfo> CurrClass = FileInfo->InFileClasses[i];
            OLString HolderName;
            HolderName.Printf(T("__class_content_%s"), CurrClass->UniqueName.CStr());
            MainBlock->ReplaceHolder(HolderName, CurrClass->Helper->MainBlock, true);


            HolderName.Printf(T("__class_static_content_%s"), CurrClass->UniqueName.CStr());
            if(CurrClass->SeperateStaticBlock == true)
            {
                StaticBlock->ReplaceHolder(HolderName, CurrClass->Helper->StaticBlock, true);
            }
            else
            {
                MainBlock->ReplaceHolder(HolderName, CurrClass->Helper->StaticBlock, true);
            }
        }
        ItFile.Next();
    }
}

}