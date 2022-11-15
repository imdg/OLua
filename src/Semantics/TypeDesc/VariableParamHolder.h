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
    virtual bool EqualsTo(TypeDescBase* Target);
    virtual OLString ToString();
    virtual bool IsNilable();
    virtual SPtr<TypeDescBase> AcceptBinOp(EBinOp Op, SPtr<TypeDescBase> Target);
    virtual SPtr<TypeDescBase> AcceptUniOp(EUniOp Op);

    virtual void ResolveReferredType(SymbolScope* Scope, CompileMsg& CM, ESymbolResolvePhase Phase);

    WPtr<TypeDescBase> ParamType;
    OLString UnresolvedTypeName;
    bool IsResolved;

};
}