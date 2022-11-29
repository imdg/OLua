/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#pragma once
#include "Common.h"
#include "ScopeVistorBase.h"
#include "AstDecl.h"
#include "CompileMsg.h"
namespace OL
{
// TypeInfoVisitor :
//      1. creates TypeDesc of user defined types
//         including class, interface, enum, function signiture, array, map
//      2. some nodes implies both a variant and a type
//         like AFuncDef: a function variant and a function signiture type
//      3. every user defined type is created but they may not know others, while only 'Unresolved Names' are know
//         like a base class of a class, a class type param of a function signiture
//         these will be resolved to real TypeDesc object in further steps


class FuncSigniture;
class AFuncType;
class TypeInfoVisitor : public ScopeVisitorBase
{
public:
    TypeInfoVisitor(SymbolTable& InSymboTable, CompileMsg& InCM);

    virtual EVisitStatus BeginVisit(SPtr<AClass> Node);
    virtual EVisitStatus EndVisit(SPtr<AClass> Node);

    virtual EVisitStatus BeginVisit(SPtr<AInterface> Node);
    virtual EVisitStatus EndVisit(SPtr<AInterface> Node);

    virtual EVisitStatus BeginVisit(SPtr<AFuncBody> Node);
    virtual EVisitStatus EndVisit(SPtr<AFuncBody> Node);

    virtual EVisitStatus BeginVisit(SPtr<AFuncType> Node);
    virtual EVisitStatus EndVisit(SPtr<AFuncType> Node);

    virtual EVisitStatus BeginVisit(SPtr<ANormalMethod> Node);
    virtual EVisitStatus EndVisit(SPtr<ANormalMethod> Node);

    virtual EVisitStatus BeginVisit(SPtr<AClassContructor> Node);
    virtual EVisitStatus EndVisit(SPtr<AClassContructor> Node);

    virtual EVisitStatus BeginVisit(SPtr<AFuncDef> Node);
    virtual EVisitStatus EndVisit(SPtr<AFuncDef> Node);

    virtual EVisitStatus BeginVisit(SPtr<AFuncExpr> Node);
    virtual EVisitStatus EndVisit(SPtr<AFuncExpr> Node);

    virtual EVisitStatus BeginVisit(SPtr<AAlias> Node);
    virtual EVisitStatus EndVisit(SPtr<AAlias> Node);


    virtual EVisitStatus EndVisit(SPtr<AVarDecl> Node);

    virtual EVisitStatus BeginVisit(SPtr<AClassVar> Node);

    //virtual EVisitStatus Visit(SPtr<ATypeIdentity> Node);

    virtual EVisitStatus Visit(SPtr<ANamedType> Node);
    virtual EVisitStatus Visit(SPtr<AIntrinsicType> Node);

    virtual EVisitStatus BeginVisit(SPtr<AArrayType> Node);
    virtual EVisitStatus EndVisit(SPtr<AArrayType> Node);

    virtual EVisitStatus BeginVisit(SPtr<AMapType> Node);
    virtual EVisitStatus EndVisit(SPtr<AMapType> Node);

    virtual EVisitStatus BeginVisit(SPtr<AEnum> Node);
    virtual EVisitStatus EndVisit(SPtr<AEnum> Node);

    virtual EVisitStatus BeginVisit(SPtr<AEnumItem> Node);
    virtual EVisitStatus EndVisit(SPtr<AEnumItem> Node);

    virtual EVisitStatus BeginVisit(SPtr<AConstructor> Node);
    virtual EVisitStatus EndVisit(SPtr<AConstructor> Node);

    EVisitStatus VisitAsFuncParam(SPtr<AVarDecl> Node, SPtr<FuncSigniture> FuncSig);

    void TryReceiveComplexSubType(SPtr<TypeDescBase> Owner, SPtr<TypeDescBase> SubType, SPtr<ATypeIdentity> Node);

    void AddFuncReturn(SPtr<FuncSigniture> Func, SPtr<TypeDescBase> NewRet);
    OLString MakeClassUniquName(SPtr<ClassType> Class);


    // Push new TypeDesc into this stack when beginning parsing a new type
    // When a TypeDesc is finished (EndVisit)
    //     1. pop it
    //     2. check the remaining stack top to see if the current TypeDesc is a part of the stack-top TypeDesc
    //     3. update the stack-top TypeDesc
    // Eg. func(val as int[]) :  Param 'val' type of FuncSigniture will be updated to int[] at the EndVisit of int[]
    OLList<SPtr<TypeDescBase>> TypeStack;
    CompileMsg&     CM;
    int ClassNestLevel;
};

}
