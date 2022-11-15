#pragma once
#include "Common.h"
#include "SourceFile.h"

namespace OL
{

class APILoader
{
public:
    bool LoadAPI(OLString APIPath);
    OLString FindAPIPath(OLString UserSpecifiedPath);

    void ApplyAPIToSource(SPtr<SourceFile> Source);

    OLList<SPtr<SourceFile>> APIFiles;

};

}