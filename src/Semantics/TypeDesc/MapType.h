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

class MapType : public TypeDescBase
{
    DECLEAR_RTTI();
public:

    WPtr<TypeDescBase> KeyType;
    WPtr<TypeDescBase> ValueType;

    OLString UnresolvedKeyType;
    OLString UnresolvedValueType;

    //bool IsKeyNilable;
    bool IsValueNilable;

    virtual void ResolveReferredType(SymbolScope* Scope, CompileMsg& CM, ESymbolResolvePhase Phase);

    void SetKeyType(SPtr<TypeDescBase> InType);
    void SetKeyType(OLString UnresolvedName);

    void SetValueType(SPtr<TypeDescBase> InType, bool IsNilable);
    void SetValueType(OLString UnresolvedName, bool IsNilable);

    bool IsKeyTypeDecl(ATypeIdentity* Node);
    bool IsValueTypeDecl(ATypeIdentity* Node);

    virtual ETypeValidation ValidateConvert(SPtr<TypeDescBase> Target, bool IsExplict);
    virtual bool EqualsTo(SPtr<TypeDescBase> Target);
    virtual OLString ToString(bool IsNilable);

    virtual OperatorResult AcceptBinOp(EBinOp Op, SPtr<TypeDescBase> Target, bool TargetNilable);
    virtual SPtr<TypeDescBase> AcceptUniOp(EUniOp Op);
};

}