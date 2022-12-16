/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#pragma once
#include "Common.h"
#include "VisitorBase.h"
#include "OLFunc.h"
#include "LuaPlainInterp.h"
#include "FuncGenBase.h"

namespace OL
{


class ABase;
class FuncSigniture;



class LPBuiltInFuncInterp
{

public:
    typedef bool GenCallBack(LuaPlainInterp& Interp, LuaPlainInterp::NodeGen& Result, int BaseIndex, SPtr<ABase> Node);
    void Init();

    void RegisterCodeGen(SPtr<FuncSigniture> Func, OLFunc<GenCallBack> Generator);
    static LPBuiltInFuncInterp* Inst;
    static LPBuiltInFuncInterp& GetInst();

};

class LPBuiltinFuncGen : public FuncGenBase
{
    DECLEAR_RTTI();
public:    
    
    OLFunc<LPBuiltInFuncInterp::GenCallBack> OnCall;
    LPBuiltinFuncGen(OLFunc<LPBuiltInFuncInterp::GenCallBack> InOnCall);

    bool DoCodeGen(LuaPlainInterp& Interp, LuaPlainInterp::NodeGen& Result, int BaseIndex, SPtr<ABase> Node);

};

}