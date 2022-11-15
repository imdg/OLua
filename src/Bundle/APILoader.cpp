#include "APILoader.h"
#include "CmdConfig.h"
#include "Env.h"


namespace OL
{

bool APILoader::LoadAPI(OLString APIPath)
{

    Env::IterateFileInDir(APIPath, true, [this](OLString Path, bool IsDir)
    {
        if(IsDir)
            return;
        OLString Ext = Path.ExtFromPath();
        Ext.ToUpper();
        if(Ext != T(".OLU"))
            return;

        SPtr<SourceFile> APISource = new SourceFile(Path);
        APIFiles.Add(APISource);
    });


    for(int i = 0; i < APIFiles.Count(); i++)
    {
        APIFiles[i]->DoLocalCompile();
        APIFiles[i]->DoTypeResolve();
        APIFiles[i]->DoResolveAsAPI();
    }

    if(APIFiles.Count() == 0)
        return false;
    
    return true;
}


OLString APILoader::FindAPIPath(OLString UserSpecifiedPath)
{
    if(UserSpecifiedPath == T(""))
    {
        return Env::GetBinPath() + "/api";
    }
    else
    {
        return Env::ToAbsPath(UserSpecifiedPath);
    }
}

void APILoader::ApplyAPIToSource(SPtr<SourceFile> Source)
{
    for(int i = 0; i < APIFiles.Count(); i++)
    {
        Source->AddAPISource(APIFiles[i]);
    }
}

}