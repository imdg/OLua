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