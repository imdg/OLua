/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#pragma once
#include "Common.h"
#include "TypeDescBase.h"
#include "AstCommon.h"

namespace OL
{


class VariableParamHolder : public TypeDescBase
{
    DECLEAR_RTTI();
public:
    VariableParamHolder(SPtr<ABase> Node, SPtr<TypeDescBase> InParamType);
    VariableParamHolder(SPtr<ABase> Node, OLString ParamTypeName);
    virtual ETypeValidation ValidateConvert(SPtr<TypeDescBase> Target, bool IsExplict);
    virtual bool EqualsTo(SPtr<TypeDescBase> Target);
    virtual OLString ToString(bool IsNilable);

    virtual OperatorResult AcceptBinOp(EBinOp Op, SPtr<TypeDescBase> Target, bool TargetNilable);
    virtual SPtr<TypeDescBase> AcceptUniOp(EUniOp Op);

    virtual void ResolveReferredType(SymbolScope* Scope, CompileMsg& CM, ESymbolResolvePhase Phase);

    WPtr<TypeDescBase> ParamType;
    OLString UnresolvedTypeName;
    bool IsResolved;
    bool IsNilable;

};
}