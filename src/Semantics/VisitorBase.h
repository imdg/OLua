#pragma once
#include "Common.h"
#include "ExpressionDecl.h"
#include "StatementDecl.h"
#include "ScopeDecl.h"

namespace OL 
{
enum EVisitStatus
{
    VS_Continue,
    VS_ContinueParent,
    VS_Stop
};

class ABase;
class Visitor
{
public:
    virtual EVisitStatus Visit(SPtr<ABase> Node);
    virtual EVisitStatus Visit(SPtr<AExpr> Node);
    virtual EVisitStatus Visit(SPtr<APrimaryExpr> Node);
    virtual EVisitStatus Visit(SPtr<AParentheses> Node);
    virtual EVisitStatus Visit(SPtr<ASelf> Node);
    virtual EVisitStatus Visit(SPtr<ASuper> Node);
    virtual EVisitStatus Visit(SPtr<AVarRef> Node);
    virtual EVisitStatus Visit(SPtr<AVariableParamRef> Node);
    virtual EVisitStatus Visit(SPtr<ASimpleExpr> Node);
    virtual EVisitStatus Visit(SPtr<AConstExpr> Node);
    virtual EVisitStatus Visit(SPtr<AConstructor> Node);
    virtual EVisitStatus Visit(SPtr<AFuncExpr> Node);
    virtual EVisitStatus Visit(SPtr<ASuffixedExpr> Node);
    virtual EVisitStatus Visit(SPtr<ACall> Node);
    virtual EVisitStatus Visit(SPtr<AColonCall> Node);
    virtual EVisitStatus Visit(SPtr<ANormalCall> Node);
    virtual EVisitStatus Visit(SPtr<ADereferance> Node);
    virtual EVisitStatus Visit(SPtr<ABracketMember> Node);
    virtual EVisitStatus Visit(SPtr<ADotMember> Node);
    virtual EVisitStatus Visit(SPtr<ATypeCast> Node);
    virtual EVisitStatus Visit(SPtr<ASubexpr> Node);
    virtual EVisitStatus Visit(SPtr<ATypeIdentity> Node);
    virtual EVisitStatus Visit(SPtr<AArrayType> Node);
    virtual EVisitStatus Visit(SPtr<AFuncType> Node);
    virtual EVisitStatus Visit(SPtr<AIntrinsicType> Node);
    virtual EVisitStatus Visit(SPtr<AMapType> Node);
    virtual EVisitStatus Visit(SPtr<ANamedType> Node);
    virtual EVisitStatus Visit(SPtr<AClassMember> Node);
    virtual EVisitStatus Visit(SPtr<AClassVar> Node);
    virtual EVisitStatus Visit(SPtr<AMethod> Node);
    virtual EVisitStatus Visit(SPtr<AClassContructor> Node);
    virtual EVisitStatus Visit(SPtr<AClassDestructor> Node);
    virtual EVisitStatus Visit(SPtr<ANormalMethod> Node);
    virtual EVisitStatus Visit(SPtr<AEnumItem> Node);
    virtual EVisitStatus Visit(SPtr<AModifier> Node);
    virtual EVisitStatus Visit(SPtr<ARoot> Node);
    virtual EVisitStatus Visit(SPtr<AAttribute> Node);
    virtual EVisitStatus Visit(SPtr<ABlock> Node);
    virtual EVisitStatus Visit(SPtr<AFuncBody> Node);
    virtual EVisitStatus Visit(SPtr<AStat> Node);
    virtual EVisitStatus Visit(SPtr<AClass> Node);
    virtual EVisitStatus Visit(SPtr<AEnum> Node);
    virtual EVisitStatus Visit(SPtr<AInterface> Node);
    virtual EVisitStatus Visit(SPtr<AAssignment> Node);
    virtual EVisitStatus Visit(SPtr<ABlockStat> Node);
    virtual EVisitStatus Visit(SPtr<ADoWhile> Node);
    virtual EVisitStatus Visit(SPtr<AForList> Node);
    virtual EVisitStatus Visit(SPtr<AForNum> Node);
    virtual EVisitStatus Visit(SPtr<AIfStat> Node);
    virtual EVisitStatus Visit(SPtr<ARepeat> Node);
    virtual EVisitStatus Visit(SPtr<AWhile> Node);
    virtual EVisitStatus Visit(SPtr<ABreak> Node);
    virtual EVisitStatus Visit(SPtr<ACallStat> Node);
    virtual EVisitStatus Visit(SPtr<AFuncDef> Node);
    virtual EVisitStatus Visit(SPtr<AGlobal> Node);
    virtual EVisitStatus Visit(SPtr<AGoto> Node);
    virtual EVisitStatus Visit(SPtr<ALabel> Node);
    virtual EVisitStatus Visit(SPtr<ALocal> Node);
    virtual EVisitStatus Visit(SPtr<AReturn> Node);
    virtual EVisitStatus Visit(SPtr<AVarDecl> Node);
};

class RecursiveVisitor : public Visitor
{
public:
    virtual EVisitStatus Visit(SPtr<ABase> Node);
    virtual EVisitStatus BeginVisit(SPtr<ABase> Node);
    virtual EVisitStatus EndVisit(SPtr<ABase> Node);

    virtual EVisitStatus Visit(SPtr<AExpr> Node);
    virtual EVisitStatus BeginVisit(SPtr<AExpr> Node);
    virtual EVisitStatus EndVisit(SPtr<AExpr> Node);

    virtual EVisitStatus Visit(SPtr<APrimaryExpr> Node);
    virtual EVisitStatus BeginVisit(SPtr<APrimaryExpr> Node);
    virtual EVisitStatus EndVisit(SPtr<APrimaryExpr> Node);

    virtual EVisitStatus Visit(SPtr<AParentheses> Node);
    virtual EVisitStatus BeginVisit(SPtr<AParentheses> Node);
    virtual EVisitStatus EndVisit(SPtr<AParentheses> Node);

    virtual EVisitStatus Visit(SPtr<ASelf> Node);
    virtual EVisitStatus BeginVisit(SPtr<ASelf> Node);
    virtual EVisitStatus EndVisit(SPtr<ASelf> Node);

    virtual EVisitStatus Visit(SPtr<ASuper> Node);
    virtual EVisitStatus BeginVisit(SPtr<ASuper> Node);
    virtual EVisitStatus EndVisit(SPtr<ASuper> Node);

    virtual EVisitStatus Visit(SPtr<AVarRef> Node);
    virtual EVisitStatus BeginVisit(SPtr<AVarRef> Node);
    virtual EVisitStatus EndVisit(SPtr<AVarRef> Node);

    virtual EVisitStatus Visit(SPtr<AVariableParamRef> Node);
    virtual EVisitStatus BeginVisit(SPtr<AVariableParamRef> Node);
    virtual EVisitStatus EndVisit(SPtr<AVariableParamRef> Node);

    virtual EVisitStatus Visit(SPtr<ASimpleExpr> Node);
    virtual EVisitStatus BeginVisit(SPtr<ASimpleExpr> Node);
    virtual EVisitStatus EndVisit(SPtr<ASimpleExpr> Node);

    virtual EVisitStatus Visit(SPtr<AConstExpr> Node);
    virtual EVisitStatus BeginVisit(SPtr<AConstExpr> Node);
    virtual EVisitStatus EndVisit(SPtr<AConstExpr> Node);

    virtual EVisitStatus Visit(SPtr<AConstructor> Node);
    virtual EVisitStatus BeginVisit(SPtr<AConstructor> Node);
    virtual EVisitStatus EndVisit(SPtr<AConstructor> Node);

    virtual EVisitStatus Visit(SPtr<AFuncExpr> Node);
    virtual EVisitStatus BeginVisit(SPtr<AFuncExpr> Node);
    virtual EVisitStatus EndVisit(SPtr<AFuncExpr> Node);

    virtual EVisitStatus Visit(SPtr<ASuffixedExpr> Node);
    virtual EVisitStatus BeginVisit(SPtr<ASuffixedExpr> Node);
    virtual EVisitStatus EndVisit(SPtr<ASuffixedExpr> Node);

    virtual EVisitStatus Visit(SPtr<ACall> Node);
    virtual EVisitStatus BeginVisit(SPtr<ACall> Node);
    virtual EVisitStatus EndVisit(SPtr<ACall> Node);

    virtual EVisitStatus Visit(SPtr<AColonCall> Node);
    virtual EVisitStatus BeginVisit(SPtr<AColonCall> Node);
    virtual EVisitStatus EndVisit(SPtr<AColonCall> Node);

    virtual EVisitStatus Visit(SPtr<ANormalCall> Node);
    virtual EVisitStatus BeginVisit(SPtr<ANormalCall> Node);
    virtual EVisitStatus EndVisit(SPtr<ANormalCall> Node);

    virtual EVisitStatus Visit(SPtr<ADereferance> Node);
    virtual EVisitStatus BeginVisit(SPtr<ADereferance> Node);
    virtual EVisitStatus EndVisit(SPtr<ADereferance> Node);

    virtual EVisitStatus Visit(SPtr<ABracketMember> Node);
    virtual EVisitStatus BeginVisit(SPtr<ABracketMember> Node);
    virtual EVisitStatus EndVisit(SPtr<ABracketMember> Node);

    virtual EVisitStatus Visit(SPtr<ADotMember> Node);
    virtual EVisitStatus BeginVisit(SPtr<ADotMember> Node);
    virtual EVisitStatus EndVisit(SPtr<ADotMember> Node);

    virtual EVisitStatus Visit(SPtr<ATypeCast> Node);
    virtual EVisitStatus BeginVisit(SPtr<ATypeCast> Node);
    virtual EVisitStatus EndVisit(SPtr<ATypeCast> Node);

    virtual EVisitStatus Visit(SPtr<ASubexpr> Node);
    virtual EVisitStatus BeginVisit(SPtr<ASubexpr> Node);
    virtual EVisitStatus EndVisit(SPtr<ASubexpr> Node);

    virtual EVisitStatus Visit(SPtr<ATypeIdentity> Node);
    virtual EVisitStatus BeginVisit(SPtr<ATypeIdentity> Node);
    virtual EVisitStatus EndVisit(SPtr<ATypeIdentity> Node);

    virtual EVisitStatus Visit(SPtr<AArrayType> Node);
    virtual EVisitStatus BeginVisit(SPtr<AArrayType> Node);
    virtual EVisitStatus EndVisit(SPtr<AArrayType> Node);

    virtual EVisitStatus Visit(SPtr<AFuncType> Node);
    virtual EVisitStatus BeginVisit(SPtr<AFuncType> Node);
    virtual EVisitStatus EndVisit(SPtr<AFuncType> Node);

    virtual EVisitStatus Visit(SPtr<AIntrinsicType> Node);
    virtual EVisitStatus BeginVisit(SPtr<AIntrinsicType> Node);
    virtual EVisitStatus EndVisit(SPtr<AIntrinsicType> Node);

    virtual EVisitStatus Visit(SPtr<AMapType> Node);
    virtual EVisitStatus BeginVisit(SPtr<AMapType> Node);
    virtual EVisitStatus EndVisit(SPtr<AMapType> Node);

    virtual EVisitStatus Visit(SPtr<ANamedType> Node);
    virtual EVisitStatus BeginVisit(SPtr<ANamedType> Node);
    virtual EVisitStatus EndVisit(SPtr<ANamedType> Node);

    virtual EVisitStatus Visit(SPtr<AClassMember> Node);
    virtual EVisitStatus BeginVisit(SPtr<AClassMember> Node);
    virtual EVisitStatus EndVisit(SPtr<AClassMember> Node);

    virtual EVisitStatus Visit(SPtr<AClassVar> Node);
    virtual EVisitStatus BeginVisit(SPtr<AClassVar> Node);
    virtual EVisitStatus EndVisit(SPtr<AClassVar> Node);

    virtual EVisitStatus Visit(SPtr<AMethod> Node);
    virtual EVisitStatus BeginVisit(SPtr<AMethod> Node);
    virtual EVisitStatus EndVisit(SPtr<AMethod> Node);

    virtual EVisitStatus Visit(SPtr<AClassContructor> Node);
    virtual EVisitStatus BeginVisit(SPtr<AClassContructor> Node);
    virtual EVisitStatus EndVisit(SPtr<AClassContructor> Node);

    virtual EVisitStatus Visit(SPtr<AClassDestructor> Node);
    virtual EVisitStatus BeginVisit(SPtr<AClassDestructor> Node);
    virtual EVisitStatus EndVisit(SPtr<AClassDestructor> Node);

    virtual EVisitStatus Visit(SPtr<ANormalMethod> Node);
    virtual EVisitStatus BeginVisit(SPtr<ANormalMethod> Node);
    virtual EVisitStatus EndVisit(SPtr<ANormalMethod> Node);

    virtual EVisitStatus Visit(SPtr<AEnumItem> Node);
    virtual EVisitStatus BeginVisit(SPtr<AEnumItem> Node);
    virtual EVisitStatus EndVisit(SPtr<AEnumItem> Node);

    virtual EVisitStatus Visit(SPtr<AModifier> Node);
    virtual EVisitStatus BeginVisit(SPtr<AModifier> Node);
    virtual EVisitStatus EndVisit(SPtr<AModifier> Node);

    virtual EVisitStatus Visit(SPtr<ARoot> Node);
    virtual EVisitStatus BeginVisit(SPtr<ARoot> Node);
    virtual EVisitStatus EndVisit(SPtr<ARoot> Node);

    virtual EVisitStatus Visit(SPtr<AAttribute> Node);
    virtual EVisitStatus BeginVisit(SPtr<AAttribute> Node);
    virtual EVisitStatus EndVisit(SPtr<AAttribute> Node);

    virtual EVisitStatus Visit(SPtr<ABlock> Node);
    virtual EVisitStatus BeginVisit(SPtr<ABlock> Node);
    virtual EVisitStatus EndVisit(SPtr<ABlock> Node);

    virtual EVisitStatus Visit(SPtr<AFuncBody> Node);
    virtual EVisitStatus BeginVisit(SPtr<AFuncBody> Node);
    virtual EVisitStatus EndVisit(SPtr<AFuncBody> Node);

    virtual EVisitStatus Visit(SPtr<AStat> Node);
    virtual EVisitStatus BeginVisit(SPtr<AStat> Node);
    virtual EVisitStatus EndVisit(SPtr<AStat> Node);

    virtual EVisitStatus Visit(SPtr<AClass> Node);
    virtual EVisitStatus BeginVisit(SPtr<AClass> Node);
    virtual EVisitStatus EndVisit(SPtr<AClass> Node);

    virtual EVisitStatus Visit(SPtr<AEnum> Node);
    virtual EVisitStatus BeginVisit(SPtr<AEnum> Node);
    virtual EVisitStatus EndVisit(SPtr<AEnum> Node);

    virtual EVisitStatus Visit(SPtr<AInterface> Node);
    virtual EVisitStatus BeginVisit(SPtr<AInterface> Node);
    virtual EVisitStatus EndVisit(SPtr<AInterface> Node);

    virtual EVisitStatus Visit(SPtr<AAssignment> Node);
    virtual EVisitStatus BeginVisit(SPtr<AAssignment> Node);
    virtual EVisitStatus EndVisit(SPtr<AAssignment> Node);

    virtual EVisitStatus Visit(SPtr<ABlockStat> Node);
    virtual EVisitStatus BeginVisit(SPtr<ABlockStat> Node);
    virtual EVisitStatus EndVisit(SPtr<ABlockStat> Node);

    virtual EVisitStatus Visit(SPtr<ADoWhile> Node);
    virtual EVisitStatus BeginVisit(SPtr<ADoWhile> Node);
    virtual EVisitStatus EndVisit(SPtr<ADoWhile> Node);

    virtual EVisitStatus Visit(SPtr<AForList> Node);
    virtual EVisitStatus BeginVisit(SPtr<AForList> Node);
    virtual EVisitStatus EndVisit(SPtr<AForList> Node);

    virtual EVisitStatus Visit(SPtr<AForNum> Node);
    virtual EVisitStatus BeginVisit(SPtr<AForNum> Node);
    virtual EVisitStatus EndVisit(SPtr<AForNum> Node);

    virtual EVisitStatus Visit(SPtr<AIfStat> Node);
    virtual EVisitStatus BeginVisit(SPtr<AIfStat> Node);
    virtual EVisitStatus EndVisit(SPtr<AIfStat> Node);

    virtual EVisitStatus Visit(SPtr<ARepeat> Node);
    virtual EVisitStatus BeginVisit(SPtr<ARepeat> Node);
    virtual EVisitStatus EndVisit(SPtr<ARepeat> Node);

    virtual EVisitStatus Visit(SPtr<AWhile> Node);
    virtual EVisitStatus BeginVisit(SPtr<AWhile> Node);
    virtual EVisitStatus EndVisit(SPtr<AWhile> Node);

    virtual EVisitStatus Visit(SPtr<ABreak> Node);
    virtual EVisitStatus BeginVisit(SPtr<ABreak> Node);
    virtual EVisitStatus EndVisit(SPtr<ABreak> Node);

    virtual EVisitStatus Visit(SPtr<ACallStat> Node);
    virtual EVisitStatus BeginVisit(SPtr<ACallStat> Node);
    virtual EVisitStatus EndVisit(SPtr<ACallStat> Node);

    virtual EVisitStatus Visit(SPtr<AFuncDef> Node);
    virtual EVisitStatus BeginVisit(SPtr<AFuncDef> Node);
    virtual EVisitStatus EndVisit(SPtr<AFuncDef> Node);

    virtual EVisitStatus Visit(SPtr<AGlobal> Node);
    virtual EVisitStatus BeginVisit(SPtr<AGlobal> Node);
    virtual EVisitStatus EndVisit(SPtr<AGlobal> Node);

    virtual EVisitStatus Visit(SPtr<AGoto> Node);
    virtual EVisitStatus BeginVisit(SPtr<AGoto> Node);
    virtual EVisitStatus EndVisit(SPtr<AGoto> Node);

    virtual EVisitStatus Visit(SPtr<ALabel> Node);
    virtual EVisitStatus BeginVisit(SPtr<ALabel> Node);
    virtual EVisitStatus EndVisit(SPtr<ALabel> Node);

    virtual EVisitStatus Visit(SPtr<ALocal> Node);
    virtual EVisitStatus BeginVisit(SPtr<ALocal> Node);
    virtual EVisitStatus EndVisit(SPtr<ALocal> Node);

    virtual EVisitStatus Visit(SPtr<AReturn> Node);
    virtual EVisitStatus BeginVisit(SPtr<AReturn> Node);
    virtual EVisitStatus EndVisit(SPtr<AReturn> Node);

    virtual EVisitStatus Visit(SPtr<AVarDecl> Node);
    virtual EVisitStatus BeginVisit(SPtr<AVarDecl> Node);
    virtual EVisitStatus EndVisit(SPtr<AVarDecl> Node);


};
class VisitorWrapper : public RecursiveVisitor
{
public:
    RecursiveVisitor* WrappedVisitor;
    virtual void OnVisit(SPtr<ABase> Node);
    virtual void OnBeginVisit(SPtr<ABase> Node);
    virtual void OnEndVisit(SPtr<ABase> Node);
    virtual void OnVisitFinish(SPtr<ABase> Node, EVisitStatus Status);
    virtual void OnBeginVisitFinish(SPtr<ABase> Node, EVisitStatus Status);
    virtual void OnEndVisitFinish(SPtr<ABase> Node, EVisitStatus Status);
    virtual EVisitStatus Visit(SPtr<ABase> Node);
    virtual EVisitStatus BeginVisit(SPtr<ABase> Node);
    virtual EVisitStatus EndVisit(SPtr<ABase> Node);

    virtual EVisitStatus Visit(SPtr<AExpr> Node);
    virtual EVisitStatus BeginVisit(SPtr<AExpr> Node);
    virtual EVisitStatus EndVisit(SPtr<AExpr> Node);

    virtual EVisitStatus Visit(SPtr<APrimaryExpr> Node);
    virtual EVisitStatus BeginVisit(SPtr<APrimaryExpr> Node);
    virtual EVisitStatus EndVisit(SPtr<APrimaryExpr> Node);

    virtual EVisitStatus Visit(SPtr<AParentheses> Node);
    virtual EVisitStatus BeginVisit(SPtr<AParentheses> Node);
    virtual EVisitStatus EndVisit(SPtr<AParentheses> Node);

    virtual EVisitStatus Visit(SPtr<ASelf> Node);
    virtual EVisitStatus BeginVisit(SPtr<ASelf> Node);
    virtual EVisitStatus EndVisit(SPtr<ASelf> Node);

    virtual EVisitStatus Visit(SPtr<ASuper> Node);
    virtual EVisitStatus BeginVisit(SPtr<ASuper> Node);
    virtual EVisitStatus EndVisit(SPtr<ASuper> Node);

    virtual EVisitStatus Visit(SPtr<AVarRef> Node);
    virtual EVisitStatus BeginVisit(SPtr<AVarRef> Node);
    virtual EVisitStatus EndVisit(SPtr<AVarRef> Node);

    virtual EVisitStatus Visit(SPtr<AVariableParamRef> Node);
    virtual EVisitStatus BeginVisit(SPtr<AVariableParamRef> Node);
    virtual EVisitStatus EndVisit(SPtr<AVariableParamRef> Node);

    virtual EVisitStatus Visit(SPtr<ASimpleExpr> Node);
    virtual EVisitStatus BeginVisit(SPtr<ASimpleExpr> Node);
    virtual EVisitStatus EndVisit(SPtr<ASimpleExpr> Node);

    virtual EVisitStatus Visit(SPtr<AConstExpr> Node);
    virtual EVisitStatus BeginVisit(SPtr<AConstExpr> Node);
    virtual EVisitStatus EndVisit(SPtr<AConstExpr> Node);

    virtual EVisitStatus Visit(SPtr<AConstructor> Node);
    virtual EVisitStatus BeginVisit(SPtr<AConstructor> Node);
    virtual EVisitStatus EndVisit(SPtr<AConstructor> Node);

    virtual EVisitStatus Visit(SPtr<AFuncExpr> Node);
    virtual EVisitStatus BeginVisit(SPtr<AFuncExpr> Node);
    virtual EVisitStatus EndVisit(SPtr<AFuncExpr> Node);

    virtual EVisitStatus Visit(SPtr<ASuffixedExpr> Node);
    virtual EVisitStatus BeginVisit(SPtr<ASuffixedExpr> Node);
    virtual EVisitStatus EndVisit(SPtr<ASuffixedExpr> Node);

    virtual EVisitStatus Visit(SPtr<ACall> Node);
    virtual EVisitStatus BeginVisit(SPtr<ACall> Node);
    virtual EVisitStatus EndVisit(SPtr<ACall> Node);

    virtual EVisitStatus Visit(SPtr<AColonCall> Node);
    virtual EVisitStatus BeginVisit(SPtr<AColonCall> Node);
    virtual EVisitStatus EndVisit(SPtr<AColonCall> Node);

    virtual EVisitStatus Visit(SPtr<ANormalCall> Node);
    virtual EVisitStatus BeginVisit(SPtr<ANormalCall> Node);
    virtual EVisitStatus EndVisit(SPtr<ANormalCall> Node);

    virtual EVisitStatus Visit(SPtr<ADereferance> Node);
    virtual EVisitStatus BeginVisit(SPtr<ADereferance> Node);
    virtual EVisitStatus EndVisit(SPtr<ADereferance> Node);

    virtual EVisitStatus Visit(SPtr<ABracketMember> Node);
    virtual EVisitStatus BeginVisit(SPtr<ABracketMember> Node);
    virtual EVisitStatus EndVisit(SPtr<ABracketMember> Node);

    virtual EVisitStatus Visit(SPtr<ADotMember> Node);
    virtual EVisitStatus BeginVisit(SPtr<ADotMember> Node);
    virtual EVisitStatus EndVisit(SPtr<ADotMember> Node);

    virtual EVisitStatus Visit(SPtr<ATypeCast> Node);
    virtual EVisitStatus BeginVisit(SPtr<ATypeCast> Node);
    virtual EVisitStatus EndVisit(SPtr<ATypeCast> Node);

    virtual EVisitStatus Visit(SPtr<ASubexpr> Node);
    virtual EVisitStatus BeginVisit(SPtr<ASubexpr> Node);
    virtual EVisitStatus EndVisit(SPtr<ASubexpr> Node);

    virtual EVisitStatus Visit(SPtr<ATypeIdentity> Node);
    virtual EVisitStatus BeginVisit(SPtr<ATypeIdentity> Node);
    virtual EVisitStatus EndVisit(SPtr<ATypeIdentity> Node);

    virtual EVisitStatus Visit(SPtr<AArrayType> Node);
    virtual EVisitStatus BeginVisit(SPtr<AArrayType> Node);
    virtual EVisitStatus EndVisit(SPtr<AArrayType> Node);

    virtual EVisitStatus Visit(SPtr<AFuncType> Node);
    virtual EVisitStatus BeginVisit(SPtr<AFuncType> Node);
    virtual EVisitStatus EndVisit(SPtr<AFuncType> Node);

    virtual EVisitStatus Visit(SPtr<AIntrinsicType> Node);
    virtual EVisitStatus BeginVisit(SPtr<AIntrinsicType> Node);
    virtual EVisitStatus EndVisit(SPtr<AIntrinsicType> Node);

    virtual EVisitStatus Visit(SPtr<AMapType> Node);
    virtual EVisitStatus BeginVisit(SPtr<AMapType> Node);
    virtual EVisitStatus EndVisit(SPtr<AMapType> Node);

    virtual EVisitStatus Visit(SPtr<ANamedType> Node);
    virtual EVisitStatus BeginVisit(SPtr<ANamedType> Node);
    virtual EVisitStatus EndVisit(SPtr<ANamedType> Node);

    virtual EVisitStatus Visit(SPtr<AClassMember> Node);
    virtual EVisitStatus BeginVisit(SPtr<AClassMember> Node);
    virtual EVisitStatus EndVisit(SPtr<AClassMember> Node);

    virtual EVisitStatus Visit(SPtr<AClassVar> Node);
    virtual EVisitStatus BeginVisit(SPtr<AClassVar> Node);
    virtual EVisitStatus EndVisit(SPtr<AClassVar> Node);

    virtual EVisitStatus Visit(SPtr<AMethod> Node);
    virtual EVisitStatus BeginVisit(SPtr<AMethod> Node);
    virtual EVisitStatus EndVisit(SPtr<AMethod> Node);

    virtual EVisitStatus Visit(SPtr<AClassContructor> Node);
    virtual EVisitStatus BeginVisit(SPtr<AClassContructor> Node);
    virtual EVisitStatus EndVisit(SPtr<AClassContructor> Node);

    virtual EVisitStatus Visit(SPtr<AClassDestructor> Node);
    virtual EVisitStatus BeginVisit(SPtr<AClassDestructor> Node);
    virtual EVisitStatus EndVisit(SPtr<AClassDestructor> Node);

    virtual EVisitStatus Visit(SPtr<ANormalMethod> Node);
    virtual EVisitStatus BeginVisit(SPtr<ANormalMethod> Node);
    virtual EVisitStatus EndVisit(SPtr<ANormalMethod> Node);

    virtual EVisitStatus Visit(SPtr<AEnumItem> Node);
    virtual EVisitStatus BeginVisit(SPtr<AEnumItem> Node);
    virtual EVisitStatus EndVisit(SPtr<AEnumItem> Node);

    virtual EVisitStatus Visit(SPtr<AModifier> Node);
    virtual EVisitStatus BeginVisit(SPtr<AModifier> Node);
    virtual EVisitStatus EndVisit(SPtr<AModifier> Node);

    virtual EVisitStatus Visit(SPtr<ARoot> Node);
    virtual EVisitStatus BeginVisit(SPtr<ARoot> Node);
    virtual EVisitStatus EndVisit(SPtr<ARoot> Node);

    virtual EVisitStatus Visit(SPtr<AAttribute> Node);
    virtual EVisitStatus BeginVisit(SPtr<AAttribute> Node);
    virtual EVisitStatus EndVisit(SPtr<AAttribute> Node);

    virtual EVisitStatus Visit(SPtr<ABlock> Node);
    virtual EVisitStatus BeginVisit(SPtr<ABlock> Node);
    virtual EVisitStatus EndVisit(SPtr<ABlock> Node);

    virtual EVisitStatus Visit(SPtr<AFuncBody> Node);
    virtual EVisitStatus BeginVisit(SPtr<AFuncBody> Node);
    virtual EVisitStatus EndVisit(SPtr<AFuncBody> Node);

    virtual EVisitStatus Visit(SPtr<AStat> Node);
    virtual EVisitStatus BeginVisit(SPtr<AStat> Node);
    virtual EVisitStatus EndVisit(SPtr<AStat> Node);

    virtual EVisitStatus Visit(SPtr<AClass> Node);
    virtual EVisitStatus BeginVisit(SPtr<AClass> Node);
    virtual EVisitStatus EndVisit(SPtr<AClass> Node);

    virtual EVisitStatus Visit(SPtr<AEnum> Node);
    virtual EVisitStatus BeginVisit(SPtr<AEnum> Node);
    virtual EVisitStatus EndVisit(SPtr<AEnum> Node);

    virtual EVisitStatus Visit(SPtr<AInterface> Node);
    virtual EVisitStatus BeginVisit(SPtr<AInterface> Node);
    virtual EVisitStatus EndVisit(SPtr<AInterface> Node);

    virtual EVisitStatus Visit(SPtr<AAssignment> Node);
    virtual EVisitStatus BeginVisit(SPtr<AAssignment> Node);
    virtual EVisitStatus EndVisit(SPtr<AAssignment> Node);

    virtual EVisitStatus Visit(SPtr<ABlockStat> Node);
    virtual EVisitStatus BeginVisit(SPtr<ABlockStat> Node);
    virtual EVisitStatus EndVisit(SPtr<ABlockStat> Node);

    virtual EVisitStatus Visit(SPtr<ADoWhile> Node);
    virtual EVisitStatus BeginVisit(SPtr<ADoWhile> Node);
    virtual EVisitStatus EndVisit(SPtr<ADoWhile> Node);

    virtual EVisitStatus Visit(SPtr<AForList> Node);
    virtual EVisitStatus BeginVisit(SPtr<AForList> Node);
    virtual EVisitStatus EndVisit(SPtr<AForList> Node);

    virtual EVisitStatus Visit(SPtr<AForNum> Node);
    virtual EVisitStatus BeginVisit(SPtr<AForNum> Node);
    virtual EVisitStatus EndVisit(SPtr<AForNum> Node);

    virtual EVisitStatus Visit(SPtr<AIfStat> Node);
    virtual EVisitStatus BeginVisit(SPtr<AIfStat> Node);
    virtual EVisitStatus EndVisit(SPtr<AIfStat> Node);

    virtual EVisitStatus Visit(SPtr<ARepeat> Node);
    virtual EVisitStatus BeginVisit(SPtr<ARepeat> Node);
    virtual EVisitStatus EndVisit(SPtr<ARepeat> Node);

    virtual EVisitStatus Visit(SPtr<AWhile> Node);
    virtual EVisitStatus BeginVisit(SPtr<AWhile> Node);
    virtual EVisitStatus EndVisit(SPtr<AWhile> Node);

    virtual EVisitStatus Visit(SPtr<ABreak> Node);
    virtual EVisitStatus BeginVisit(SPtr<ABreak> Node);
    virtual EVisitStatus EndVisit(SPtr<ABreak> Node);

    virtual EVisitStatus Visit(SPtr<ACallStat> Node);
    virtual EVisitStatus BeginVisit(SPtr<ACallStat> Node);
    virtual EVisitStatus EndVisit(SPtr<ACallStat> Node);

    virtual EVisitStatus Visit(SPtr<AFuncDef> Node);
    virtual EVisitStatus BeginVisit(SPtr<AFuncDef> Node);
    virtual EVisitStatus EndVisit(SPtr<AFuncDef> Node);

    virtual EVisitStatus Visit(SPtr<AGlobal> Node);
    virtual EVisitStatus BeginVisit(SPtr<AGlobal> Node);
    virtual EVisitStatus EndVisit(SPtr<AGlobal> Node);

    virtual EVisitStatus Visit(SPtr<AGoto> Node);
    virtual EVisitStatus BeginVisit(SPtr<AGoto> Node);
    virtual EVisitStatus EndVisit(SPtr<AGoto> Node);

    virtual EVisitStatus Visit(SPtr<ALabel> Node);
    virtual EVisitStatus BeginVisit(SPtr<ALabel> Node);
    virtual EVisitStatus EndVisit(SPtr<ALabel> Node);

    virtual EVisitStatus Visit(SPtr<ALocal> Node);
    virtual EVisitStatus BeginVisit(SPtr<ALocal> Node);
    virtual EVisitStatus EndVisit(SPtr<ALocal> Node);

    virtual EVisitStatus Visit(SPtr<AReturn> Node);
    virtual EVisitStatus BeginVisit(SPtr<AReturn> Node);
    virtual EVisitStatus EndVisit(SPtr<AReturn> Node);

    virtual EVisitStatus Visit(SPtr<AVarDecl> Node);
    virtual EVisitStatus BeginVisit(SPtr<AVarDecl> Node);
    virtual EVisitStatus EndVisit(SPtr<AVarDecl> Node);

};
}
