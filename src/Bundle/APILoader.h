/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#pragma once
#include "Common.h"
#include "SourceFile.h"

namespace OL
{
class BuildSetting;
struct RawLuaAPIInfo
{
    OLString FileName;
    OLString Content;
};
class ClassType;
class APILoader
{
public:
    bool LoadAPI(OLString APIPath, BuildSetting& Setting);
    OLString FindAPIPath(OLString UserSpecifiedPath);

    void ApplyAPIToSource(SPtr<SourceFile> Source);

    SPtr<ClassType> FindAPIClass(OLString Name);

    OLList<SPtr<SourceFile>> APIFiles;

    OLList<SPtr<RawLuaAPIInfo>>  LuaAPIFiles;

};

}