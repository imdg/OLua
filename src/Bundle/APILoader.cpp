/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "APILoader.h"
#include "CmdConfig.h"
#include "Env.h"
#include "BuildSetting.h"
#include "ClassType.h"
#include "Logger.h"

namespace OL
{

bool APILoader::LoadAPI(OLString APIPath, BuildSetting& Setting)
{

    Env::IterateFileInDir(APIPath, true, [this](OLString Path, bool IsDir)
    {

        if(IsDir)
            return;
        OLString Ext = Path.ExtFromPath();
        Ext.ToUpper();
        if(Ext == T(".OLU"))
        {    
            SPtr<SourceFile> APISource = new SourceFile(Path);
            APIFiles.Add(APISource);
           // WARNING(LogMisc,  T("Load olu API %s"), Path.NameFromPath().CStr());
        }
        else if(Ext == T(".LUA"))
        {
            FILE* RawLuaFile = t_fopen(Path, "rb");
            fseek(RawLuaFile, 0, SEEK_END);
            long Len = ftell(RawLuaFile);
            fseek(RawLuaFile, 0, SEEK_SET);

            byte* Content = new byte[Len + 1];
            fread(Content, 1, Len, RawLuaFile);
            Content[Len] = 0;

            RawLuaAPIInfo* NewInfo = new RawLuaAPIInfo();
            NewInfo->FileName = Path.NameFromPath();
            NewInfo->Content = OLString::FromUTF8((const char*) Content);

            LuaAPIFiles.Add(NewInfo);
            //WARNING(LogMisc,  T("Load lua API %s"), NewInfo->FileName.CStr());
            delete[] Content;
        }
    });


    for(int i = 0; i < APIFiles.Count(); i++)
    {
        APIFiles[i]->ApplyBuildSetting(Setting);
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
        return Env::GetBinPath() + T("/api");
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

SPtr<ClassType> APILoader::FindAPIClass(OLString Name)
{
    for(int i = 0; i < APIFiles.Count(); i++)
    {
        SPtr<TypeDescBase> Found = APIFiles[i]->Symbols->Scopes[0]->FindNamedType(Name, false);
        if(Found != nullptr && Found->Is<ClassType>())
        {
            return Found.PtrAs<ClassType>();
        }
    }
    return nullptr;
}

}