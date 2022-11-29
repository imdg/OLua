/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#pragma once
#include "Common.h"
#include "ASubexpr.h"
#include "CompileCommon.h"
#include "AAttribute.h"

namespace OL
{

RTTI_ENUM(ETypeFamily,
    TF_Intrinsic,
    TF_Class,
    TF_Interface,
    TF_Function
)

class ABase;
class SymbolScope;

enum ClassMemberFlags
{
    CMF_Const       = 1,
    CMF_Static      = 1 << 1,
    CMF_Virtual     = 1 << 2,
    CMF_Abstract    = 1 << 3,
    CMF_Override    = 1 << 4,
    CMF_Constructor = 1 << 5,
    CMF_Public      = 1 << 6,
    CMF_Protected   = 1 << 7,
};

RTTI_ENUM(ETypeValidation,
    TCR_OK,
    TCR_DataLose,
    TCR_Unsafe,
    TCR_NoWay

);


struct OperatorResult
{
    SPtr<TypeDescBase> Type;
    bool               IsNilable;
};

class CompileMsg;
class TypeDescBase : public SThisOwner<TypeDescBase>
{
    DECLEAR_RTTI();

public:
    WPtr<ABase> DeclNode;
    TypeDescBase();
    virtual ~TypeDescBase();
    virtual void ResolveReferredType(SymbolScope* Scope, CompileMsg& CM, ESymbolResolvePhase Phase);

    virtual ETypeValidation ValidateConvert(SPtr<TypeDescBase> Target, bool IsExplict) = 0;
    virtual bool EqualsTo(SPtr<TypeDescBase> Target) = 0;
    virtual OLString ToString(bool IsNilable) = 0;

    virtual bool IsInt();
    virtual bool IsFloat();
    virtual bool IsString();
    virtual bool IsAny();
    virtual bool IsBool();
    virtual bool IsNil();
    virtual bool IsImplicitAny();

    virtual bool IsRefType();

    //OperatorResult AcceptNilCoalesing(SPtr<TypeDescBase> Target, bool TargetNilable);

    virtual OperatorResult AcceptBinOp(EBinOp Op, SPtr<TypeDescBase> Target, bool TargetNilable);
    virtual SPtr<TypeDescBase> AcceptUniOp(EUniOp Op);

    virtual SPtr<TypeDescBase> DeduceLValueType(SPtr<SymbolScope> Scope);

    virtual OLString DefaultValueText();

    template<typename Ty>
    SPtr<Ty> ActuallyAs() { return GetActualType().PtrAs<Ty>(); };

    template<typename Ty>
    bool ActuallyIs() { return GetActualType().PtrIs<Ty>(); };

    virtual SPtr<TypeDescBase> GetActualType();

    // Sometimes a type will be deduced to another, which is a new TypeDescBase with same DeclNode. 
    // In this case the original TypeDescBase should be 'muted'
    bool IsValid();
    bool Valid;
};

}