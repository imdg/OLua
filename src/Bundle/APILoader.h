#pragma once
#include "Common.h"
#include "SourceFile.h"

namespace OL
{
class BuildSetting;
class APILoader
{
public:
    bool LoadAPI(OLString APIPath, BuildSetting& Setting);
    OLString FindAPIPath(OLString UserSpecifiedPath);

    void ApplyAPIToSource(SPtr<SourceFile> Source);

    OLList<SPtr<SourceFile>> APIFiles;

};

}