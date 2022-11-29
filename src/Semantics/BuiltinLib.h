/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#pragma once
#include "Common.h"
#include "FuncSigniture.h"
#include "IntrinsicType.h"
#include "AstCommon.h"

namespace OL
{

struct IntrinsicMethodInfo
{
    EIntrinsicType Type;
    OLMap<OLString, SPtr<FuncSigniture> > Methods;
};

class BuiltinLib
{
public: 
    IntrinsicMethodInfo Instrinsics[IT_max];
    void Init();
 

};

}