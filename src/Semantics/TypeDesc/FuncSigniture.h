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
#include "CompileCommon.h"
namespace OL
{
class AVarDecl;
class ATypeIdentity;
class TupleType;
class ArrayType;
class MapType;

enum FunctionParamFlag
{
    FPF_This = 1,
    FPF_Const = 1 << 1,
    FPF_HasDefault = 1 << 2
};

struct FuncParamDesc
{
    WPtr<TypeDescBase> Type;
    uint Flags;
    OLString UnresolvedTypeName;
    bool IsResolved;
    bool IsVariableParam;
    bool IsOptional;
    CodeLineInfo Line;
};
DECLEAR_STRUCT_RTTI(FuncParamDesc)

struct FuncReturnDesc
{
    WPtr<TypeDescBase> Type;
    OLString UnresolvedTypeName;
    bool IsResolved;
    CodeLineInfo Line;
};
DECLEAR_STRUCT_RTTI(FuncReturnDesc)


class SymbolScope;

class FuncSigniture : public TypeDescBase
{
    DECLEAR_RTTI()
public:
    OLList<FuncParamDesc> Params;
    OLList<FuncReturnDesc>  Returns;

    WPtr<TypeDescBase>  UniqueReturnType;  // TupleType when multiple return, or the only return type, or VoidHolder

    WPtr<TypeDescBase>  CtorOwnerType; // This indicates  that the function is a constructor when it is not null

    bool HasThis;
    OLString UnresolvedThisType;
    WPtr<TypeDescBase> ThisType;

    FuncSigniture();
    virtual ~FuncSigniture();

    void SetThis(SPtr<TypeDescBase> InThisType);
    void SetUnresolvedThis(OLString InThisTypeName);
    void SetCtorOwner(SPtr<TypeDescBase> InCtorOwner);

    void AddParam(OLString& UnresolvedName, bool IsConst, bool IsVariableParam, bool IsOptional, CodeLineInfo& Line);
    void AddParam(EIntrinsicType Type, bool IsConst, bool IsVariableParam, bool IsOptional,  CodeLineInfo& Line);
    void AddParam(SPtr<TypeDescBase> Type, bool IsConst, bool IsVariableParam, bool IsOptional,  CodeLineInfo& Line);

    void AddReturn(SPtr<TypeDescBase> Type, CodeLineInfo& Line);
    void AddReturn(OLString& UnresolvedName, CodeLineInfo& Line);
    void AddReturn(EIntrinsicType Type, CodeLineInfo& Line);

    void MakeUniqueReturn(SPtr<TupleType> EmptyTuple);

    bool IsParamDecl(AVarDecl* Node);
    bool IsReturnDecl(ATypeIdentity* Node);

    virtual ETypeValidation ValidateConvert(SPtr<TypeDescBase> Target, bool IsExplict);
    virtual bool EqualsTo(SPtr<TypeDescBase> Target);
    virtual OLString ToString();
    virtual bool IsNilable();
    virtual SPtr<TypeDescBase> AcceptBinOp(EBinOp Op, SPtr<TypeDescBase> Target);
    virtual SPtr<TypeDescBase> AcceptUniOp(EUniOp Op);

    virtual void ResolveReferredType(SymbolScope* Scope, CompileMsg& CM, ESymbolResolvePhase Phase);
    bool HasVariableParam();

    int   MaxParamRequire();
    int   MinParamRequire();
    
};

}