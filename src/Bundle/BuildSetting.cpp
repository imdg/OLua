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