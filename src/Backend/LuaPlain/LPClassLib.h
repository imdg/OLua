/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#pragma once
#include "Common.h"
#include "LPClassHelper.h"
#include "OLMap.h"

namespace OL
{


class SourceFile;
class TextParagraph;
class LPClassLib
{
public:

    struct ClassInfo
    {
        OLString UniqueName;
        OLString DependentClass;
        OLString SrcFile;
        SPtr<LPClassHelper> Helper;
        bool    SeperateStaticBlock;
        
    };

    struct ClassDependency
    {
        OLString BaseClassUniqueName;
        SPtr<ClassInfo> BaseClass;
        OLList<SPtr<ClassInfo>> ChildClasses;
    };

    struct FileClassInfo
    {
        OLString FileName;
        OLList<SPtr<ClassInfo>> InFileClasses;
        int DependentCount;
    };
    
    void BeginFile(OLString File);
    void EndFile();

    void SaveClass(SPtr<LPClassHelper> ClassHelper, OLString UniqueName, OLString DependentName, bool SeperateStaticBlock);
    
    void BreakChild(SPtr<ClassInfo> BaseClass);
    void ResetFinishedFile();

    void ApplyClassToFile(SPtr<SourceFile> File);
    void ApplyClassToStandaloneFile(SPtr<TextParagraph> MainBlock, SPtr<TextParagraph> StaticBlock);

    OLList<SPtr<ClassInfo> > PendingBreak;
    OLMap<OLString, SPtr<ClassInfo> > Classes;
    OLMap<OLString, SPtr<ClassDependency> > Dependency;
    OLMap<OLString, SPtr<FileClassInfo>> Files;

    OLList<SPtr<FileClassInfo>> FinishedFile;
    SPtr<FileClassInfo>        CurrFile;
    bool            CurrFileHasDependency;

};

}