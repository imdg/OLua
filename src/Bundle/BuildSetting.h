/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

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