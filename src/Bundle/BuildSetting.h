#pragma once
#include "Common.h"


namespace OL
{

class BuildSetting
{
public:
    OLString BundleOutputPath;
    OLString EntryPath;
    OLString APIPath;

    OLString MainFunc;
    bool AllowImplicitAny;
    bool AllowUnresolvedType;
    
    static BuildSetting DefaultBuildSetting;
    static void InitDefaultDefaultBuildSetting();
};



}