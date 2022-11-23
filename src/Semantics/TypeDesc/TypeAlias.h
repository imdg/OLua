/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#pragma once
#include "Common.h"
#include "TypeDescBase.h"

namespace OL
{

class TypeAlias : public TypeDescBase
{
    DECLEAR_RTTI();
public:
    TypeAlias();
    virtual ~TypeAlias();

    virtual void ResolveReferredType(SymbolScope* Scope, CompileMsg& CM, ESymbolResolvePhase Phase);

    virtual ETypeValidation ValidateConvert(SPtr<TypeDescBase> Target, bool IsExplict);
    virtual bool EqualsTo(SPtr<TypeDescBase> Target);
    virtual OLString ToString();

    virtual bool IsInt();
    virtual bool IsFloat();
    virtual bool IsString();
    virtual bool IsAny();
    virtual bool IsBool();
    virtual bool IsNil();
    virtual bool IsImplicitAny();

    virtual bool IsNilable();
    virtual bool IsRefType();

    virtual SPtr<TypeDescBase> AcceptBinOp(EBinOp Op, SPtr<TypeDescBase> Target);
    virtual SPtr<TypeDescBase> AcceptUniOp(EUniOp Op);

    virtual SPtr<TypeDescBase> DeduceLValueType(SPtr<SymbolScope> Scope);

    virtual OLString DefaultValueText();

    void SetActualType(SPtr<TypeDescBase> Type);
    void SetActualTypeName(OLString TypeName);

    virtual SPtr<TypeDescBase> GetActualType();
public:

    WPtr<TypeDescBase> ActualType;
    OLString           UnresolvedTypeName;
    OLString           Name;

};



}