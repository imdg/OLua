#pragma once
#include "Common.h"
#include "BuildSetting.h"
#include "SymbolLib.h"
#include "LPClassLib.h"
#include "APILoader.h"
namespace OL 
{
class SourceFile;

class BundleBuilder
{

public:
    BuildSetting&    Settings;

    SymbolLib       GlobalSymbols;
    LPClassLib      ClassCodeLib;
    APILoader       APIs;

    int             TotalError;
    int             TotalWarning;

    struct PendingFileInfo
    {
        OLString SrcPath;
        OLString DstPath;
        OLString Identity;
        OLString Intermediate;
    };
    bool DebugMode;

     OLList<SPtr<SourceFile>> BundleFiles;
    OLMap<OLString, SPtr<SourceFile>> BundleFilesIndex;
    
    OLList<PendingFileInfo> PendingList;

    OLList<SPtr<SourceFile>> LocalFinished;

    OLList<SPtr<SourceFile>> FilesInLib;

    OLList<SPtr<SourceFile>> TypeBindFinshed;
    OLList<SPtr<SourceFile>> VarBindFinshed;

    OLList<SPtr<SourceFile>> TextFinished;

    OLList<SPtr<SourceFile>> Completed;

    BundleBuilder(BuildSetting& InSetting);

    void PrepareBuild();

    void DoNextLocalBuild();

    void DoNextTypeBind();
    void DoNextVarBind();

    void FlushTypeUnmatchedFile();
    void FlushVarUnmatchedFile();

    void DoNextCodeGen();

    void DoNextFileWrite();

    void DoGenerateEntry();

    void AddFile(OLString Src, OLString Dst, OLString Identity, OLString Intermediate);

    void Run();

    void PrintFailedMsg();
    void PrintSuccessMsg();

    void DumpDebugFiles();
    void DumpSingleDebugFile(SPtr<SourceFile> File);

protected:
    void AddTotalError(int Count);
    void AddTotalWarning(int Count);
    
};

}