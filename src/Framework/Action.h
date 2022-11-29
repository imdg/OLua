/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#pragma once

#include "Common.h"
#include "OLList.h"
#include "OLString.h"

namespace OL
{
class CmdConfig;
class Action
{
public:
    Action();
    virtual int Run() = 0;
    virtual CmdConfig& Cmd();
    CmdConfig* LocalCmd;
    OLString Name;
    OLString WorkingDirectory;
};

class ActionMgr
{
public:
    void RegisterAction(Action* Act);
    int RunAction(const TCHAR* Name);
    int RunActionWithLocalCmd(const TCHAR* Name, CmdConfig* LocalCmd, OLString WorkingDir);

    static ActionMgr* GetInst();
private:
    OLList<Action*> ActionList;
    static ActionMgr* Inst;
};

class ActionRegister
{
public:
    ActionRegister(Action* NewAction, const TCHAR* Name);
};

#define REGISTER_ACTION(Act) \
static Act InstOf##Act; \
static ActionRegister RegisterOf##Act(&InstOf##Act, T(#Act));


#define REGISTER_NAMED_ACTION(Act, Name) \
static Act InstOf##Act; \
static ActionRegister RegisterOf##Act(&InstOf##Act, Name);

}