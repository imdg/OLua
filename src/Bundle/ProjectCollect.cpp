/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "ProjectCollector.h"
#include "Action.h"
#include "CmdConfig.h"
#include "Logger.h"
#include "Env.h"
#include "BundleBuilder.h"

namespace OL
{

ProjectCollector GProject;

void ProjectCollector::Reset()
{
    Dirs.Clear();
    Files.Clear();
}

void ProjectCollector::ApplyBuilder(BundleBuilder& Builder)
{
    for(int i = 0; i < Dirs.Count(); i++)
    {
        ApplyDir(Builder, Dirs[i]);
    }

    for(int i = 0; i < Files.Count(); i++)
    {
        Builder.AddFile(Files[i].FilePath, Files[i].OutPath, Files[i].FileIdentity, Files[i].IntermediateFile);
    }
}

void ProjectCollector::ApplyDir(BundleBuilder& Builder, DirInfo& Dir )
{
    int DirPathLen = Dir.DirPath.Len();
    VERBOSE(LogTester, T("Applying directory: %s"), Dir.DirPath.CStr());

    Env::IterateFileInDir(Dir.DirPath, Dir.IncludeSubDir, [&](OLString Path, bool IsDir)
    {
        if(IsDir == false)
        {
            OLString RealPath = Env::NormalizePath(Path, false);
            OLString Name = RealPath.NameFromPath();

            if(Name.MatchWildcard(Dir.FileFilter.CStr()) == false)
                return;
            
            OLString ReletivePath = Env::MakeReletivePath(Dir.DirPath, RealPath);
            OLString ReletiveParentPath = ReletivePath.ParentPath();
            OLString PureName = RealPath.PureNameFromPath();

            OLString OutPathParent;
            OutPathParent.Printf(T("%s/%s"), Dir.OutPath.CStr(), ReletiveParentPath.CStr());
            Env::MakeDir(OutPathParent);
            
            OLString IntermediatePathParent;
            IntermediatePathParent.Printf(T("%s/%s"), Dir.IntermediatePath.CStr(), ReletiveParentPath.CStr());
            Env::MakeDir(IntermediatePathParent);

             if(ReletiveParentPath != T(""))
                ReletiveParentPath.Append(T("/"));

            OLString OutPath;
            OutPath.Printf(T("%s/%s%s%s"), Dir.OutPath.CStr(), ReletiveParentPath.CStr(), PureName.CStr(), Dir.OutExt.CStr());

            OLString IntermediatePath;
            IntermediatePath.Printf(T("%s/%s%s.oli"), Dir.IntermediatePath.CStr(), ReletiveParentPath.CStr(), PureName.CStr());

            OLString Identity = ReletiveParentPath;
            if(Dir.IncludeExt)
                Identity.Append(PureName + T(".lua"));
            else
                Identity.Append(PureName);

            Identity.Replace(T("/"), Dir.PathSplitter.CStr());
            Identity = Dir.Prefix + Identity + Dir.Suffix;

            // VERBOSE(LogTester, T("    Source file: %s"), RealPath.CStr());
            // VERBOSE(LogTester, T("        Output       : %s"), OutPath.CStr());
            // VERBOSE(LogTester, T("              In     : %s"), OutPathParent.CStr());
            // VERBOSE(LogTester, T("        Intermediate : %s"), IntermediatePath.CStr());
            // VERBOSE(LogTester, T("              In     : %s"), IntermediatePathParent.CStr());
            // VERBOSE(LogTester, T("        Identity     : %s"), Identity.CStr());
            
            Builder.AddFile(RealPath, OutPath, Identity, IntermediatePath);
        }

        
    });
}
class ActBeginProject : public Action
{
public:
    virtual int Run()
    {
        GProject.Reset();
        return 0;
    };
};
REGISTER_NAMED_ACTION(ActBeginProject, T("begin"));


OLString GetFullPathForAction(OLString WorkingDir, OLString Path)
{
    if (Path.IsRelativePath())
        return Env::NormalizePath(WorkingDir + T("/") + Path, false);
    else
        return Env::NormalizePath(Path, false);
}

class ActCollectDirectory : public Action
{
public:
    virtual int Run()
    {
        ProjectCollector::DirInfo& NewDir = GProject.Dirs.AddConstructed();
        if(Cmd().HasKey(T("--path")) == false)
        {
            ERROR(LogProjecct, T("'--path' is required"));
            return -1;
        }
        NewDir.DirPath = GetFullPathForAction(WorkingDirectory, Cmd().GetFirstParam(T("--path")));

        NewDir.OutPath = NewDir.DirPath + T("/output");
        NewDir.IntermediatePath = NewDir.DirPath + T("/intermediate");

        if(Cmd().HasKey(T("--out")) != false)
        {
            NewDir.OutPath = GetFullPathForAction(WorkingDirectory, Cmd().GetFirstParam(T("--out")));
            NewDir.IntermediatePath = NewDir.OutPath + T("/intermediate");
        }

        if(Cmd().HasKey(T("--intermediate")) != false)
            NewDir.IntermediatePath = GetFullPathForAction(WorkingDirectory, Cmd().GetFirstParam(T("--intermediate")));


        if(Cmd().HasKey(T("--filter")) != false)
            NewDir.FileFilter = Cmd().GetFirstParam(T("--filter"));
        else
            NewDir.FileFilter = T("*.olu");

        if(Cmd().HasKey(T("--no_subdir")))
            NewDir.IncludeSubDir = false;
        else
            NewDir.IncludeSubDir = true;

        if(Cmd().HasKey(T("--id_ext")))
            NewDir.IncludeExt = true;
        else
            NewDir.IncludeExt = false;

        if(Cmd().HasKey(T("--id_splitter")))
            NewDir.PathSplitter = Cmd().GetFirstParam(T("--id_splitter"));
        else
            NewDir.PathSplitter = T("/");

        if(Cmd().HasKey(T("--id_prefix")))
            NewDir.Prefix = Cmd().GetFirstParam(T("--id_prefix"));
        else
            NewDir.Prefix = T("");

        if(Cmd().HasKey(T("--id_suffix")))
            NewDir.Prefix = Cmd().GetFirstParam(T("--id_suffix"));
        else
            NewDir.Prefix = T("");

        if(Cmd().HasKey(T("--out_ext")))
            NewDir.OutExt = Cmd().GetFirstParam(T("--out_ext"));
        else
            NewDir.OutExt = T(".lua");
        
        return 0;
    };
};
REGISTER_NAMED_ACTION(ActCollectDirectory, T("dir"));

class ActCollectFile : public Action
{
public:
    virtual int Run()
    {
        ProjectCollector::FileInfo& NewFile = GProject.Files.AddConstructed();
        if(Cmd().HasKey(T("--path")) == false)
        {
            ERROR(LogProjecct, T("'--path' is required"));
            return -1;
        }
        else
        {
            NewFile.FilePath = GetFullPathForAction(WorkingDirectory, Cmd().GetFirstParam(T("--path")));

            
            NewFile.OutPath = NewFile.FilePath + T(".lua");
            NewFile.IntermediateFile = NewFile.FilePath + T(".oli");
        }

        if(Cmd().HasKey(T("--out")) == false)
        {
            ERROR(LogProjecct, T("'--out' is required"));
            return -1;
        }
        else
        {
            NewFile.OutPath = GetFullPathForAction(WorkingDirectory, Cmd().GetFirstParam(T("--out")));
            NewFile.IntermediateFile = NewFile.OutPath + T(".oli");
        }

        if(Cmd().HasKey(T("--id")) == false)
        {
            ERROR(LogProjecct, T("'--id' is required"));
            return -1;
        }
        else
        {
            NewFile.FileIdentity = Cmd().GetFirstParam(T("--id"));
        }

        if(Cmd().HasKey(T("--intermediate")) != false)
        {
            NewFile.IntermediateFile = GetFullPathForAction(WorkingDirectory, Cmd().GetFirstParam(T("--intermediate")));
        }
        return 0;
    };
};
REGISTER_NAMED_ACTION(ActCollectFile, T("file"));


class ActCompileProj : public Action
{
public:
    virtual int Run()
    {
        BuildSetting Setting;
        if(Cmd().HasKey(T("--entry")) == true)
        {
            Setting.EntryPath = GetFullPathForAction(WorkingDirectory, Cmd().GetFirstParam(T("--entry")));
        }
        else
        {
            if(GProject.Dirs.Count() > 0)
                Setting.EntryPath = GProject.Dirs[0].OutPath + T("/__entry.lua");
            else
                Setting.EntryPath = WorkingDirectory + T("/__entry.lua");
        }
        
        if(GProject.Dirs.Count() == 0 && GProject.Files.Count() == 0)
        {
            VERBOSE(LogProjecct, T("No files is compiled"));
            return 0;
        }

        if(Cmd().HasKey(T("--api")))
            Setting.APIPath = Cmd().GetFirstParam(T("--api"));
        else
            Setting.APIPath = T("");

        if(Cmd().HasKey(T("--main")))
            Setting.MainFunc = Cmd().GetFirstParam(T("--main"));
        else
            Setting.MainFunc = T("");

        Setting.NilSafety = VL_Warning;
        if(Cmd().HasKey(T("--nilsafety")))
        {
            OLString Level = Cmd().GetFirstParam(T("--nilsafety"));
            if(Level == T("yes") || Level == T("1") || Level == T("true") )
                Setting.NilSafety = VL_Error;
            else if(Level == T("no") || Level == T("0") || Level == T("false") )
                Setting.NilSafety = VL_Warning;
        }
       

        Setting.AllowUnresolvedType = true;
        if(Cmd().HasKey(T("--allow_unresolved")))
        {
            OLString txt = Cmd().GetFirstParam(T("--allow_unresolved"));
            if(txt == T("0") || txt == T("false") || txt == T("no"))
                Setting.AllowUnresolvedType = false;
        }

        if(Cmd().HasKey(T("--main")))
            Setting.MainFunc = Cmd().GetFirstParam(T("--main"));
        else
            Setting.MainFunc = T("");

        if(Cmd().HasKey(T("--debugfile")))
            Setting.DumpDebugFile = true;
        else
            Setting.DumpDebugFile = false;

        BundleBuilder Builder(Setting);
        GProject.ApplyBuilder(Builder);
        Builder.Run();

        return 0;
    };
};
REGISTER_NAMED_ACTION(ActCompileProj, T("compile"));

}