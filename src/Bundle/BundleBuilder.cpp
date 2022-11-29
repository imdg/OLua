/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "BundleBuilder.h"
#include "SourceFile.h"
#include "Logger.h"
#include "Env.h"
#include "FileStream.h"
#include "TextSerializer.h"
#include "FileStream.h"
#include "ARoot.h"
#include "CmdConfig.h"

namespace OL
{

BundleBuilder::BundleBuilder(BuildSetting& InSetting) : TotalError(0), TotalWarning(0), Settings(InSetting), DebugMode(false)
{

}

void BundleBuilder::PrepareBuild()
{
    OLString APIPath = APIs.FindAPIPath(Settings.APIPath);
    APIs.LoadAPI(APIPath, Settings);
}

void BundleBuilder::AddTotalError(int Count)
{
    TotalError += Count;
}

void BundleBuilder::AddTotalWarning(int Count)
{
    TotalWarning += Count;
}

void BundleBuilder::DoNextLocalBuild()
{
    if(PendingList.Count() <= 0)
        return;

    PendingFileInfo Pending = PendingList.DequeueHead();
    SPtr<SourceFile> NewSource = new SourceFile(Pending.SrcPath);
    NewSource->OutputName = Pending.DstPath;
    NewSource->FileIdenty = Pending.Identity;
    NewSource->ApplyBuildSetting(Settings);
    APIs.ApplyAPIToSource(NewSource);


    BundleFiles.Add(NewSource);
    BundleFilesIndex.Add(Pending.SrcPath, NewSource);

    OLString FileName = Env::FileNameFromPath(Pending.SrcPath);
    VERBOSE(LogCompile, T("Compiling %s"), FileName.CStr());
    
    NewSource->DoLocalCompile();

    if(NewSource->CM.ErrorCount == 0)
        LocalFinished.Add(NewSource);
    
    AddTotalError(NewSource->CM.ErrorCount);
    AddTotalWarning(NewSource->CM.WarningCount);
    NewSource->CM.Reset();

}

void BundleBuilder::DoNextTypeBind()
{
    if(LocalFinished.Count() <= 0)
        return;

    SPtr<SourceFile> CurrFile = LocalFinished.DequeueHead();
    FilesInLib.Add(CurrFile);

    OLList<SPtr<SourceFile>> MatchedList;
    GlobalSymbols.MatchFileTypes(CurrFile, MatchedList);

    AddTotalError(CurrFile->CM.ErrorCount);
    AddTotalWarning(CurrFile->CM.WarningCount);

    CurrFile->CM.Reset();

    for(int i = 0; i < MatchedList.Count(); i++)
    {
        TypeBindFinshed.Add(MatchedList[i]);
    }

}

void BundleBuilder::FlushTypeUnmatchedFile()
{
    OLList<SPtr<SourceFile>> Files;
    GlobalSymbols.GetTypeUnmatched(Files);

    for(int i = 0; i < Files.Count(); i++)
    {
        TypeBindFinshed.Add(Files[i]);
    }
}



void BundleBuilder::DoNextVarBind()
{
    if(TypeBindFinshed.Count() <= 0)
        return;

    SPtr<SourceFile> CurrFile = TypeBindFinshed.DequeueHead();
    CurrFile->DoTypeResolve();

    OLList<SPtr<SourceFile>> MatchedList;
    GlobalSymbols.MatchFileVars(CurrFile, MatchedList);

    AddTotalError(CurrFile->CM.ErrorCount);
    AddTotalWarning(CurrFile->CM.WarningCount);

    CurrFile->CM.Reset();

    for(int i = 0; i < MatchedList.Count(); i++)
    {
        VarBindFinshed.Add(MatchedList[i]);
    }

}

void BundleBuilder::FlushVarUnmatchedFile()
{
    OLList<SPtr<SourceFile>> Files;
    GlobalSymbols.GetVarUnmatched(Files);

    for(int i = 0; i < Files.Count(); i++)
    {
        VarBindFinshed.Add(Files[i]);
    }
}

void BundleBuilder::DoNextCodeGen()
{
    if(VarBindFinshed.Count() <= 0)
        return;
    
    SPtr<SourceFile> CurrFile = VarBindFinshed.DequeueHead();
    
    OLString FileName = Env::FileNameFromPath(CurrFile->FileName);
    VERBOSE(LogCompile, T("Generating %s"), FileName.CStr());

    

    ClassCodeLib.BeginFile(CurrFile->FileName);
    CurrFile->DoCodeGen(ClassCodeLib);
    ClassCodeLib.EndFile();


    if(CurrFile->CM.ErrorCount > 0)
        AddTotalError(CurrFile->CM.ErrorCount);
    else
        Completed.Add(CurrFile);

    if(CurrFile->CM.WarningCount > 0)
        AddTotalWarning(CurrFile->CM.WarningCount);

    CurrFile->CM.Reset();

    for(int i = 0; i < ClassCodeLib.FinishedFile.Count(); i++)
    {
        if(BundleFilesIndex.Exists(ClassCodeLib.FinishedFile[i]->FileName))
        {
            SPtr<SourceFile> File = BundleFilesIndex[ClassCodeLib.FinishedFile[i]->FileName];
            ClassCodeLib.ApplyClassToFile(File);
            TextFinished.Add(File);
        }
    }

    ClassCodeLib.ResetFinishedFile();


    

}

void BundleBuilder::DoNextFileWrite()
{
    if(TextFinished.Count() <= 0)
        return;

    SPtr<SourceFile> CurrFile = TextFinished.DequeueHead();
    if(CurrFile->MainBlock == nullptr)
    {
        ERROR(LogCompile, T("Fail to emitt %s"), CurrFile->OutputName.CStr());
        return;
    }
    VERBOSE(LogCompile, T("Emitting %s"), CurrFile->OutputName.CStr());
    FileStream FS;
    FS.OpenWrite(CurrFile->OutputName);

    if(DebugMode)
        CurrFile->OutText.DebugMode = true;

    
    CurrFile->OutText.WriteStreamFrom(FS, CurrFile->MainBlock);
    FS.Close();
}

void BundleBuilder::AddFile(OLString Src, OLString Dst, OLString Identity, OLString Intermediate)
{
    PendingList.Add(PendingFileInfo{Src, Dst, Identity, Intermediate});
}

void BundleBuilder::PrintFailedMsg()
{
    ERROR(LogCompile, T("Build failed. %d error(s), %d warning(s)"), TotalError, TotalWarning);
}
void BundleBuilder::PrintSuccessMsg()
{
    VERBOSE(LogCompile, T("Build finished successfully. %d error(s), %d warning(s)"), TotalError, TotalWarning);
}

void BundleBuilder::DoGenerateEntry()
{
    OLString EntryFile = Settings.EntryPath;
    

    TextBuilder EntryText;
    if(DebugMode)
        EntryText.DebugMode = true;

    EntryText.Root->IndentInc()
        .Append(T("----------------------------------------------")).NewLine()
        .Append(T("----------     Global requires    ------------")).NewLine()
        .Append(T("----------------------------------------------")).NewLine().NewLine();

    for(int i = 0; i < Completed.Count(); i++)
    {
        EntryText.Root->AppendF(T("require (\"%s\")"), Completed[i]->FileIdenty.CStr()).NewLine();
    }
    EntryText.Root->IndentDec().NewLine();
    

    EntryText.Root->IndentInc()
        .Append(T("----------------------------------------------")).NewLine()
        .Append(T("-------- Global static initializaions --------")).NewLine()
        .Append(T("----------------------------------------------")).NewLine().NewLine();
    
    FileStream FS;
    FS.OpenWrite(EntryFile);
    EntryText.WriteStream(FS);
    
    for(int i = 0; i < Completed.Count(); i++)
    {
        Completed[i]->OutText.WriteStreamFrom(FS, Completed[i]->StaticBlock);
    }
    if(Settings.MainFunc != T(""))
        FS.WriteFormat(T("\n%s()\n"), Settings.MainFunc.CStr());
    FS.Close();

    VERBOSE(LogCompile, T("Entry generated at %s"), EntryFile.CStr());
}

void BundleBuilder::Run()
{
    PrepareBuild();
    while(PendingList.Count() > 0)
    {
        DoNextLocalBuild();
        if(TotalError > 10)
        {
            PrintFailedMsg();
            return;
        }
    }

    VERBOSE(LogCompile, T("Linking ..."));
    while(LocalFinished.Count() > 0)
    {
        DoNextTypeBind();
        if(TotalError > 10)
        {
            PrintFailedMsg();
            return;
        }
    }
    FlushTypeUnmatchedFile();

    while(TypeBindFinshed.Count() > 0)
    {
        DoNextVarBind();
        if(TotalError > 10)
        {
            PrintFailedMsg();
            return;
        }
    }
    FlushVarUnmatchedFile();

    for(int i = 0; i < FilesInLib.Count(); i++)
    {
        if(FilesInLib[i]->GetUnbindTypeCount() > 0 
            || FilesInLib[i]->GetUnbindVarCount() > 0 )
        {
            FilesInLib[i]->PrintUnbindError();
            AddTotalError(FilesInLib[i]->CM.ErrorCount);
            AddTotalWarning(FilesInLib[i]->CM.WarningCount);
            FilesInLib[i]->CM.Reset();
        }
    }


    if(TotalError > 10)
    {
        PrintFailedMsg();
        return;
    }
    
    while (VarBindFinshed.Count() > 0)
    {
        DoNextCodeGen();
        if(TotalError > 10)
        {
            PrintFailedMsg();
            return;
        }
    }


    while (TextFinished.Count() > 0)
    {
        DoNextFileWrite();
    }

    DoGenerateEntry();
    
    if(TotalError > 0)
        PrintFailedMsg();
    else
        PrintSuccessMsg();

    if(Settings.DumpDebugFile)
    {
        DumpDebugFiles();
    }
}
void BundleBuilder::DumpSingleDebugFile(SPtr<SourceFile> File)
{
    OLString AstFile = File->OutputName;
    OLString SymbolFile = File->OutputName;

    AstFile.Append(T(".ast"));
    SymbolFile.Append(T(".symbol"));

    VERBOSE(LogCompile, T("          Ast: %s"), AstFile.CStr());
    VERBOSE(LogCompile, T("          Symbol: %s"), SymbolFile.CStr());

    FileStream FSSymbol;
    FSSymbol.OpenWrite(SymbolFile);
    for(int i = 0; i < File->Symbols->Scopes.Count(); i++)
    {
        FSSymbol.WriteFormat(T("\n--------Scope %d : (0x%llx)-----------\n"), i,  (uint64)File->Symbols->Scopes[i].Get());
        TextSerializer TSSymbol(&FSSymbol);
        TSSymbol.WriteRTTI(File->Symbols->Scopes[i].Get(), File->Symbols->Scopes[i]->GetTypePtr());
    }
    FSSymbol.Close();

    FileStream FSAst;
    FSAst.OpenWrite(AstFile);
    TextSerializer TSAst(&FSAst);
    TSAst.WriteRTTI(File->AstRoot.Get(), File->AstRoot->GetTypePtr());
    FSAst.Close();
}


void BundleBuilder::DumpDebugFiles()
{
    VERBOSE(LogCompile, T("Pending files:"));
    for(int i = 0; i < PendingList.Count(); i++)
    {
        VERBOSE(LogCompile, T("    %d: %s"), i+1, PendingList[i].SrcPath.CStr());
        VERBOSE(LogCompile, T("          Dst: %s"), PendingList[i].DstPath.CStr());
        VERBOSE(LogCompile, T("          Identity: %s"), PendingList[i].SrcPath.CStr());
    }

    VERBOSE(LogCompile, T("Bundle files: "));
    for(int i = 0; i < BundleFiles.Count(); i++)
    {
        VERBOSE(LogCompile, T("    %d: %s"), i+1, BundleFiles[i]->FileName.CStr());
        DumpSingleDebugFile(BundleFiles[i]);
    }

    VERBOSE(LogCompile, T("Local finished files: "));
    for(int i = 0; i < LocalFinished.Count(); i++)
    {
        VERBOSE(LogCompile, T("    %d: %s"), i+1, LocalFinished[i]->FileName.CStr());
        //DumpSingleDebugFile(LocalFinished[i]);
    }

    VERBOSE(LogCompile, T("Files in library: "));
    for(int i = 0; i < FilesInLib.Count(); i++)
    {
        VERBOSE(LogCompile, T("    %d: %s"), i+1, FilesInLib[i]->FileName.CStr());
        //DumpSingleDebugFile(FilesInLib[i]);
    }

    VERBOSE(LogCompile, T("Type Bind finished files: "));
    for(int i = 0; i < TypeBindFinshed.Count(); i++)
    {
        VERBOSE(LogCompile, T("    %d: %s"), i+1, TypeBindFinshed[i]->FileName.CStr());
        //DumpSingleDebugFile(BindFinshed[i]);
    }

    VERBOSE(LogCompile, T("Var Bind finished files: "));
    for(int i = 0; i < VarBindFinshed.Count(); i++)
    {
        VERBOSE(LogCompile, T("    %d: %s"), i+1, VarBindFinshed[i]->FileName.CStr());
        //DumpSingleDebugFile(BindFinshed[i]);
    }

    VERBOSE(LogCompile, T("Complete files: "));
    for(int i = 0; i < Completed.Count(); i++)
    {
        VERBOSE(LogCompile, T("    %d: %s"), i+1, Completed[i]->FileName.CStr());
        //DumpSingleDebugFile(Completed[i]);
    }


}

}