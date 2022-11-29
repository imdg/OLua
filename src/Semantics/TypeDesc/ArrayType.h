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

class ATypeIdentity;

class ArrayType : public TypeDescBase
{
    DECLEAR_RTTI();
public:

    WPtr<TypeDescBase> ElemType;
    OLString   UnresolvedElemType;
    bool       IsElemNilable;

    virtual void ResolveReferredType(SymbolScope* Scope, CompileMsg& CM, ESymbolResolvePhase Phase);

    void SetElemType(SPtr<TypeDescBase> InElemType, bool IsNilable);
    void SetElemType(OLString UnresolvedName, bool IsNilable);
    bool IsElemTypeDecl(ATypeIdentity* Node);
    virtual ETypeValidation ValidateConvert(SPtr<TypeDescBase> Target, bool IsExplict);
    virtual bool EqualsTo(SPtr<TypeDescBase> Target);
    virtual OLString ToString(bool IsNilable);


    virtual OperatorResult AcceptBinOp(EBinOp Op, SPtr<TypeDescBase> Target, bool TargetNilable);
    virtual SPtr<TypeDescBase> AcceptUniOp(EUniOp Op);

    virtual ~ArrayType();
};
}