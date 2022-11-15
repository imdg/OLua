#pragma once
#include "Common.h"
#include "TypeDescBase.h"
#include "AstCommon.h"

namespace OL
{

class ATypeIdentity;

class ArrayType : public TypeDescBase
{
    DECLEAR_RTTI();
public:

    WPtr<TypeDescBase> ElemType;
    OLString   UnresolvedElemType;

    virtual void ResolveReferredType(SymbolScope* Scope, CompileMsg& CM, ESymbolResolvePhase Phase);

    void SetElemType(SPtr<TypeDescBase> InElemType);
    void SetElemType(OLString UnresolvedName);
    bool IsElemTypeDecl(ATypeIdentity* Node);
    virtual ETypeValidation ValidateConvert(SPtr<TypeDescBase> Target, bool IsExplict);
    virtual bool EqualsTo(TypeDescBase* Target);
    virtual OLString ToString();

    virtual bool IsNilable();

    virtual SPtr<TypeDescBase> AcceptBinOp(EBinOp Op, SPtr<TypeDescBase> Target);
    virtual SPtr<TypeDescBase> AcceptUniOp(EUniOp Op);

    virtual ~ArrayType();
};
}