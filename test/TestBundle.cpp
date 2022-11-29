#include "Common.h"
#include "Action.h"
#include "Logger.h"
#include "BundleBuilder.h"
#include "Env.h"
#include "CmdConfig.h"

namespace OL
{

class TestBundle : public Action
{
public:
    virtual int Run()
    {
        OLString SrcPath = Env::GetDataPath() + T("/TestBundle/");
        OLString SrcName = Cmd().GetFirstParam(T("--src"), T("DefaultTest.txt"));
        SrcPath += SrcName;

        BuildSetting Setting;
        Env::MakeSavedDir(SrcName.CStr());
        Setting.BundleOutputPath = Env::GetSavedDir(SrcName.CStr());
        Setting.EntryPath = Env::PrepareSavedFilePath(SrcName.CStr(), T("__entry.lua"));

        BundleBuilder Builder(Setting);
        
        Env::IterateFileInDir(SrcPath, true
            , [&](OLString ItemPath, bool IsDir)
        {
            if(ItemPath.ExtFromPath().ToLower() == T(".olu"))
            {
                
                if(ItemPath.Sub(0, SrcPath.Len()) != SrcPath)
                {
                    ERROR(LogTester, T("Path error: ItemPath: %s, SrcPath: %d"), ItemPath.CStr(), SrcPath.CStr());
                }
                OLString SubPath = ItemPath.Sub(SrcPath.Len() + 1, ItemPath.Len() - 1 - SrcPath.Len());
                OLString DstPath = Setting.BundleOutputPath;
                DstPath.AppendF(T("/%s.lua"), SubPath.CStr());

                OLString Identity;
                Identity.Printf(T("%s.lua"), SubPath.CStr());
                Builder.AddFile(ItemPath, DstPath, Identity, DstPath+T(".oli"));

                VERBOSE(LogTester, T("Adding file to project: "));
                VERBOSE(LogTester, T("        Source: %s"), ItemPath.CStr());
                VERBOSE(LogTester, T("        Target: %s"), DstPath.CStr());
                VERBOSE(LogTester, T("        Identity: %s"), Identity.CStr());
            }
        });
        
        if(Cmd().HasKey(T("--debug")))
            Builder.DebugMode = true;
        
        Builder.Run();

        Builder.DumpDebugFiles();
        return 0;
    };

};

REGISTER_ACTION(TestBundle)
}