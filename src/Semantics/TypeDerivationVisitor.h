/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#pragma once

#include "Common.h"
#include "AstDecl.h"
#include "VisitorBase.h"
#include "SymbolTable.h"
#include "ScopeVistorBase.h"
#include "BuildSetting.h"
#include "TypeDescBase.h"
namespace OL
{

// TypeDerivationVisitor
//      1. every expression is supposed to have its TypeDesc. This step is to build these connections and store it in AST
//      2. this step does more validation than real data operation. Type convertion validation, function call validation, etc.
//      3. dereference is done in this step

class CompileMsg;
class TypeDescBase;
class FuncSigniture;
class ClassType;
class MapType;
class InterfaceType;
class EnumType;

struct TypeDeriContex
{
    TypeDeriContex(SPtr<ABase> InNode, SPtr<TypeDescBase>  InPrimaryType, bool InIsConst );
    TypeDeriContex(SPtr<ABase> InNode, SPtr<TypeDescBase>  InPrimaryType, bool InIsConst, bool InIsType );
    SPtr<ABase>         Node;
    SPtr<TypeDescBase>  PrimaryType;
    bool                IsType;
    bool                IsConst;
    bool                IsVariableParamRef;
    bool                IsNilable;
};

// enum ETypeConvResult
// {
//     TCR
// }

class TypeDerivationVisitor : public ScopeVisitorBase
{
public:
    TypeDerivationVisitor(SymbolTable& InSymboTable, CompileMsg& InCM, BuildSetting& InBuildSetting = BuildSetting::DefaultBuildSetting);

    virtual EVisitStatus BeginVisit(SPtr<ASubexpr> Node);
    virtual EVisitStatus EndVisit(SPtr<ASubexpr> Node);

    virtual EVisitStatus BeginVisit(SPtr<ATypeCast> Node);
    virtual EVisitStatus EndVisit(SPtr<ATypeCast> Node);

    virtual EVisitStatus BeginVisit(SPtr<ADotMember> Node);
    virtual EVisitStatus EndVisit(SPtr<ADotMember> Node);

    virtual EVisitStatus BeginVisit(SPtr<ABracketMember> Node);
    virtual EVisitStatus EndVisit(SPtr<ABracketMember> Node);

    virtual EVisitStatus BeginVisit(SPtr<ANormalCall> Node);
    virtual EVisitStatus EndVisit(SPtr<ANormalCall> Node);

    virtual EVisitStatus BeginVisit(SPtr<AColonCall> Node);
    virtual EVisitStatus EndVisit(SPtr<AColonCall> Node);

    virtual EVisitStatus BeginVisit(SPtr<AFuncExpr> Node);
    virtual EVisitStatus EndVisit(SPtr<AFuncExpr> Node);

    virtual EVisitStatus BeginVisit(SPtr<AConstructor> Node);
    virtual EVisitStatus EndVisit(SPtr<AConstructor> Node);

    virtual EVisitStatus Visit(SPtr<AConstExpr> Node);
    virtual EVisitStatus Visit(SPtr<AVarRef> Node);
    virtual EVisitStatus Visit(SPtr<ASuper> Node);
    virtual EVisitStatus Visit(SPtr<ASelf> Node);

    virtual EVisitStatus BeginVisit(SPtr<AParentheses> Node);
    virtual EVisitStatus EndVisit(SPtr<AParentheses> Node);

    virtual EVisitStatus BeginVisit(SPtr<ANilableUnwrap> Node);
    virtual EVisitStatus EndVisit(SPtr<ANilableUnwrap> Node);

    //-------- statements

    virtual EVisitStatus BeginVisit(SPtr<ACallStat> Node);
    virtual EVisitStatus EndVisit(SPtr<ACallStat> Node);


    virtual EVisitStatus BeginVisit(SPtr<AClassVar> Node);
    virtual EVisitStatus EndVisit(SPtr<AClassVar> Node);

    virtual EVisitStatus BeginVisit(SPtr<AEnumItem> Node);
    virtual EVisitStatus EndVisit(SPtr<AEnumItem> Node);

    virtual EVisitStatus BeginVisit(SPtr<AAssignment> Node);
    virtual EVisitStatus EndVisit(SPtr<AAssignment> Node);
    
    virtual EVisitStatus BeginVisit(SPtr<ADoWhile> Node);
    virtual EVisitStatus EndVisit(SPtr<ADoWhile> Node);
    
    virtual EVisitStatus BeginVisit(SPtr<AForList> Node);
    virtual EVisitStatus EndVisit(SPtr<AForList> Node);
    
    virtual EVisitStatus BeginVisit(SPtr<AForNum> Node);
    virtual EVisitStatus EndVisit(SPtr<AForNum> Node);
    
    virtual EVisitStatus BeginVisit(SPtr<AFuncDef> Node);
    virtual EVisitStatus EndVisit(SPtr<AFuncDef> Node);
    
    virtual EVisitStatus BeginVisit(SPtr<AGlobal> Node);
    virtual EVisitStatus EndVisit(SPtr<AGlobal> Node);
    
    virtual EVisitStatus BeginVisit(SPtr<AIfStat> Node);
    virtual EVisitStatus EndVisit(SPtr<AIfStat> Node);
    
    virtual EVisitStatus BeginVisit(SPtr<ALocal> Node);
    virtual EVisitStatus EndVisit(SPtr<ALocal> Node);
    
    virtual EVisitStatus BeginVisit(SPtr<ARepeat> Node);
    virtual EVisitStatus EndVisit(SPtr<ARepeat> Node);
    
    virtual EVisitStatus BeginVisit(SPtr<AReturn> Node);
    virtual EVisitStatus EndVisit(SPtr<AReturn> Node);
    
    virtual EVisitStatus BeginVisit(SPtr<AWhile> Node);
    virtual EVisitStatus EndVisit(SPtr<AWhile> Node);
    
    virtual EVisitStatus Visit(SPtr<AVariableParamRef> Node);

    ETypeValidation MatchType(SPtr<TypeDescBase> From, bool IsFromNilable, SPtr<TypeDescBase> To, bool IsToNilable, SPtr<ABase> Node, bool IsExplicit, bool SaveConvertInfo = true);

    //bool PopAndCheckType(SPtr<TypeDescBase> To, ABase* Node, bool IsExplicit = false);
    
    struct DerefResult
    {
        SPtr<TypeDescBase> Type;
        bool IsConst;
        bool IsNilable;
    };
    

    DerefResult DerefClass(SPtr<ClassType> Class, OLString Name, ABase* Node, bool UseTypeName, bool UseColon);
    SPtr<TypeDescBase> DerefMap(SPtr<MapType> Map, OLString Name, ABase* Node);
    DerefResult DerefInterface(SPtr<InterfaceType> Interface, OLString Name, ABase* Node);

    bool DerefEnum(SPtr<EnumType> Enum, OLString Name, ABase* Node);
    
    bool MatchFuncCall(SPtr<FuncSigniture> Func, int RetIndex, SPtr<AExpr>  Node);
    
    void ExpandStackTuple(int BaseIndex, OLList<TypeDeriContex>& OutList);

    
    OLString    GetDump();
    CompileMsg& CM;

    // For exprs
    // 1. BeginVisit: a base index and an empty TypeDeriContex is pushed to the two stack
    // 2. EndVisit: when current TypeDeriContex is complet, pops the base index, 
    //              but leave the TypeDeriContex in the stack as a 'return value' for the parent expr,
    //              so as a parent expr, it pops all 'return values' of child exprs in the stack, validates them, 
    //              and completes its own TypeDeriContex as the 'return value' of itself
    // For non-exprs
    //     They do not push TypeDeriContex, only pops 'return values' of children, to meet their own validation requirements
    //     like AIfStat, AAssignment, etc
    OLList<TypeDeriContex> DeriStack;
    OLList<int>            IndexStack;

    BuildSetting&          Setting;

};

 
}
