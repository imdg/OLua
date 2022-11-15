#include <stdio.h>
#include "Common.h"
#include "OLString.h"
#include "OLList.h"
#include "Rtti.h"
#include "Action.h"
#include "CmdConfig.h"
#include "Logger.h"
#include "Env.h"
#include "Lexer.h"
#include "Timer.h"
#include "BuildSetting.h"

int main(int argc, char** argv)
{
    OL::CmdConfig::GetInst()->ParseCommandline(argc, (const char**)argv);
    OL::Timer::Init();
    OL::Env::InitEnv();
    OL::Logger::Init();
    OL::BuildSetting::InitDefaultDefaultBuildSetting();
    OL::Lexer::InitKeywordsTable();

    OL::ActionMgr::GetInst()->RunAction(OL::CmdConfig::GetInst()->Act.CStr());

    OL::Logger::Uninit();
    return 0;
}