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
enum EValiLevel
{
    VL_None,
    VL_Warning,
    VL_Error
};

enum ECompileSwitch
{
    CS_On,
    CS_Off,
    CS_NotSet
};

class BuildSetting
{
public:
    OLString BundleOutputPath;
    OLString EntryPath;
    OLString APIPath;

    OLString MainFunc;
    bool AllowImplicitAny;
    bool AllowUnresolvedType;
    EValiLevel NilSafety;

    bool DumpDebugFile;

    ECompileSwitch  EnableReflection;

    static BuildSetting DefaultBuildSetting;
    static void InitDefaultDefaultBuildSetting();
};



}