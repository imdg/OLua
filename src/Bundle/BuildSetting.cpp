/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "BuildSetting.h"

namespace OL
{
BuildSetting BuildSetting::DefaultBuildSetting;


void BuildSetting::InitDefaultDefaultBuildSetting()
{
    DefaultBuildSetting.AllowImplicitAny = true;
    DefaultBuildSetting.AllowUnresolvedType = true;

}
}