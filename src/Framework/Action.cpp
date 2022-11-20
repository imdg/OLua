/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "Action.h"
#include "Logger.h"
#include "CmdConfig.h"
#include "Env.h"
namespace OL
{

ActionMgr* ActionMgr::Inst = nullptr;
ActionMgr* ActionMgr::GetInst()
{
    if(Inst == nullptr)
        Inst = new ActionMgr();
    return Inst;
}

void ActionMgr::RegisterAction(Action* Act)
{
    for(int i = 0; i < ActionList.Count(); i++)
    {
        if(ActionList[i]->Name == Act->Name)
        {
            return;
        }
    }
    ActionList.Add(Act);
}

int ActionMgr::RunAction(const TCHAR* Name)
{
    for(int i = 0; i < ActionList.Count(); i++)
    {
        if(ActionList[i]->Name == Name)
        {
            VERBOSE(LogAction, T("Run action: %s"), Name);
            ActionList[i]->WorkingDirectory = Env::GetSysCurrDir();
            return ActionList[i]->Run();
        }
    }
    ERROR(LogAction, T("Cannot find action: %s"), Name);
    return -1;
}

int ActionMgr::RunActionWithLocalCmd(const TCHAR* Name, CmdConfig* LocalCmd, OLString WorkingDir)
{
    for(int i = 0; i < ActionList.Count(); i++)
    {
        if(ActionList[i]->Name == Name)
        {
            VERBOSE(LogAction, T("Run action with local command: %s"), Name);
            ActionList[i]->LocalCmd = LocalCmd;
            ActionList[i]->WorkingDirectory = WorkingDir;
            int Ret = ActionList[i]->Run();
            ActionList[i]->LocalCmd = nullptr;
            
            return Ret;
        }
    }
    ERROR(LogAction, T("Cannot find action: %s"), Name);
    return -1;
}

ActionRegister::ActionRegister(Action* NewAct, const TCHAR* Name)
{
    NewAct->Name = Name;
    ActionMgr::GetInst()->RegisterAction(NewAct);
}


CmdConfig& Action::Cmd()
{
    if(LocalCmd == nullptr)
        return *(CmdConfig::GetInst());
    else
        return *LocalCmd;
}

Action::Action()
{
    LocalCmd = nullptr;
}




}