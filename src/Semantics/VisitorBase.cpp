/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "Common.h"
#include "VisitorBase.h"
#include "ExpressionInclude.h"
#include "StatementInclude.h"
#include "ScopeInclude.h"

namespace OL 
{
EVisitStatus Visitor::Visit(SPtr<ABase> Node)
{
    return VS_Continue;
}

EVisitStatus Visitor::Visit(SPtr<AExpr> Node)
{
    return Visit(Node.PtrAs<ABase>());
}

EVisitStatus Visitor::Visit(SPtr<APrimaryExpr> Node)
{
    return Visit(Node.PtrAs<AExpr>());
}

EVisitStatus Visitor::Visit(SPtr<AParentheses> Node)
{
    return Visit(Node.PtrAs<APrimaryExpr>());
}

EVisitStatus Visitor::Visit(SPtr<ASelf> Node)
{
    return Visit(Node.PtrAs<APrimaryExpr>());
}

EVisitStatus Visitor::Visit(SPtr<ASuper> Node)
{
    return Visit(Node.PtrAs<APrimaryExpr>());
}

EVisitStatus Visitor::Visit(SPtr<AVarRef> Node)
{
    return Visit(Node.PtrAs<APrimaryExpr>());
}

EVisitStatus Visitor::Visit(SPtr<AVariableParamRef> Node)
{
    return Visit(Node.PtrAs<APrimaryExpr>());
}

EVisitStatus Visitor::Visit(SPtr<ASimpleExpr> Node)
{
    return Visit(Node.PtrAs<AExpr>());
}

EVisitStatus Visitor::Visit(SPtr<AConstExpr> Node)
{
    return Visit(Node.PtrAs<ASimpleExpr>());
}

EVisitStatus Visitor::Visit(SPtr<AConstructor> Node)
{
    return Visit(Node.PtrAs<ASimpleExpr>());
}

EVisitStatus Visitor::Visit(SPtr<AFuncExpr> Node)
{
    return Visit(Node.PtrAs<ASimpleExpr>());
}

EVisitStatus Visitor::Visit(SPtr<ASuffixedExpr> Node)
{
    return Visit(Node.PtrAs<ASimpleExpr>());
}

EVisitStatus Visitor::Visit(SPtr<ACall> Node)
{
    return Visit(Node.PtrAs<ASuffixedExpr>());
}

EVisitStatus Visitor::Visit(SPtr<AColonCall> Node)
{
    return Visit(Node.PtrAs<ACall>());
}

EVisitStatus Visitor::Visit(SPtr<ANormalCall> Node)
{
    return Visit(Node.PtrAs<ACall>());
}

EVisitStatus Visitor::Visit(SPtr<ADereferance> Node)
{
    return Visit(Node.PtrAs<ASuffixedExpr>());
}

EVisitStatus Visitor::Visit(SPtr<ABracketMember> Node)
{
    return Visit(Node.PtrAs<ADereferance>());
}

EVisitStatus Visitor::Visit(SPtr<ADotMember> Node)
{
    return Visit(Node.PtrAs<ADereferance>());
}

EVisitStatus Visitor::Visit(SPtr<ATypeCast> Node)
{
    return Visit(Node.PtrAs<ASimpleExpr>());
}

EVisitStatus Visitor::Visit(SPtr<ASubexpr> Node)
{
    return Visit(Node.PtrAs<AExpr>());
}

EVisitStatus Visitor::Visit(SPtr<ATypeIdentity> Node)
{
    return Visit(Node.PtrAs<ABase>());
}

EVisitStatus Visitor::Visit(SPtr<AArrayType> Node)
{
    return Visit(Node.PtrAs<ATypeIdentity>());
}

EVisitStatus Visitor::Visit(SPtr<AFuncType> Node)
{
    return Visit(Node.PtrAs<ATypeIdentity>());
}

EVisitStatus Visitor::Visit(SPtr<AIntrinsicType> Node)
{
    return Visit(Node.PtrAs<ATypeIdentity>());
}

EVisitStatus Visitor::Visit(SPtr<AMapType> Node)
{
    return Visit(Node.PtrAs<ATypeIdentity>());
}

EVisitStatus Visitor::Visit(SPtr<ANamedType> Node)
{
    return Visit(Node.PtrAs<ATypeIdentity>());
}

EVisitStatus Visitor::Visit(SPtr<AClassMember> Node)
{
    return Visit(Node.PtrAs<ABase>());
}

EVisitStatus Visitor::Visit(SPtr<AClassVar> Node)
{
    return Visit(Node.PtrAs<AClassMember>());
}

EVisitStatus Visitor::Visit(SPtr<AMethod> Node)
{
    return Visit(Node.PtrAs<AClassMember>());
}

EVisitStatus Visitor::Visit(SPtr<AClassContructor> Node)
{
    return Visit(Node.PtrAs<AMethod>());
}

EVisitStatus Visitor::Visit(SPtr<AClassDestructor> Node)
{
    return Visit(Node.PtrAs<AMethod>());
}

EVisitStatus Visitor::Visit(SPtr<ANormalMethod> Node)
{
    return Visit(Node.PtrAs<AMethod>());
}

EVisitStatus Visitor::Visit(SPtr<AEnumItem> Node)
{
    return Visit(Node.PtrAs<ABase>());
}

EVisitStatus Visitor::Visit(SPtr<AModifier> Node)
{
    return Visit(Node.PtrAs<ABase>());
}

EVisitStatus Visitor::Visit(SPtr<ARoot> Node)
{
    return Visit(Node.PtrAs<ABase>());
}

EVisitStatus Visitor::Visit(SPtr<AAttribute> Node)
{
    return Visit(Node.PtrAs<ABase>());
}

EVisitStatus Visitor::Visit(SPtr<ABlock> Node)
{
    return Visit(Node.PtrAs<ABase>());
}

EVisitStatus Visitor::Visit(SPtr<AFuncBody> Node)
{
    return Visit(Node.PtrAs<ABase>());
}

EVisitStatus Visitor::Visit(SPtr<AStat> Node)
{
    return Visit(Node.PtrAs<ABase>());
}

EVisitStatus Visitor::Visit(SPtr<AClass> Node)
{
    return Visit(Node.PtrAs<AStat>());
}

EVisitStatus Visitor::Visit(SPtr<AEnum> Node)
{
    return Visit(Node.PtrAs<AStat>());
}

EVisitStatus Visitor::Visit(SPtr<AInterface> Node)
{
    return Visit(Node.PtrAs<AStat>());
}

EVisitStatus Visitor::Visit(SPtr<AAssignment> Node)
{
    return Visit(Node.PtrAs<AStat>());
}

EVisitStatus Visitor::Visit(SPtr<ABlockStat> Node)
{
    return Visit(Node.PtrAs<AStat>());
}

EVisitStatus Visitor::Visit(SPtr<ADoWhile> Node)
{
    return Visit(Node.PtrAs<ABlockStat>());
}

EVisitStatus Visitor::Visit(SPtr<AForList> Node)
{
    return Visit(Node.PtrAs<ABlockStat>());
}

EVisitStatus Visitor::Visit(SPtr<AForNum> Node)
{
    return Visit(Node.PtrAs<ABlockStat>());
}

EVisitStatus Visitor::Visit(SPtr<AIfStat> Node)
{
    return Visit(Node.PtrAs<ABlockStat>());
}

EVisitStatus Visitor::Visit(SPtr<ARepeat> Node)
{
    return Visit(Node.PtrAs<ABlockStat>());
}

EVisitStatus Visitor::Visit(SPtr<AWhile> Node)
{
    return Visit(Node.PtrAs<ABlockStat>());
}

EVisitStatus Visitor::Visit(SPtr<ABreak> Node)
{
    return Visit(Node.PtrAs<AStat>());
}

EVisitStatus Visitor::Visit(SPtr<ACallStat> Node)
{
    return Visit(Node.PtrAs<AStat>());
}

EVisitStatus Visitor::Visit(SPtr<AFuncDef> Node)
{
    return Visit(Node.PtrAs<AStat>());
}

EVisitStatus Visitor::Visit(SPtr<AGlobal> Node)
{
    return Visit(Node.PtrAs<AStat>());
}

EVisitStatus Visitor::Visit(SPtr<AGoto> Node)
{
    return Visit(Node.PtrAs<AStat>());
}

EVisitStatus Visitor::Visit(SPtr<ALabel> Node)
{
    return Visit(Node.PtrAs<AStat>());
}

EVisitStatus Visitor::Visit(SPtr<ALocal> Node)
{
    return Visit(Node.PtrAs<AStat>());
}

EVisitStatus Visitor::Visit(SPtr<AReturn> Node)
{
    return Visit(Node.PtrAs<AStat>());
}

EVisitStatus Visitor::Visit(SPtr<AVarDecl> Node)
{
    return Visit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<ABase> Node)
{
    return VS_Continue;
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<ABase> Node)
{
    return VS_Continue;
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<ABase> Node)
{
    return VS_Continue;
}

EVisitStatus RecursiveVisitor::Visit(SPtr<AExpr> Node)
{
    return Visit(Node.PtrAs<ABase>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<AExpr> Node)
{
    return BeginVisit(Node.PtrAs<ABase>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<AExpr> Node)
{
    return EndVisit(Node.PtrAs<ABase>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<APrimaryExpr> Node)
{
    return Visit(Node.PtrAs<AExpr>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<APrimaryExpr> Node)
{
    return BeginVisit(Node.PtrAs<AExpr>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<APrimaryExpr> Node)
{
    return EndVisit(Node.PtrAs<AExpr>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<AParentheses> Node)
{
    return Visit(Node.PtrAs<APrimaryExpr>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<AParentheses> Node)
{
    return BeginVisit(Node.PtrAs<APrimaryExpr>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<AParentheses> Node)
{
    return EndVisit(Node.PtrAs<APrimaryExpr>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<ASelf> Node)
{
    return Visit(Node.PtrAs<APrimaryExpr>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<ASelf> Node)
{
    return BeginVisit(Node.PtrAs<APrimaryExpr>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<ASelf> Node)
{
    return EndVisit(Node.PtrAs<APrimaryExpr>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<ASuper> Node)
{
    return Visit(Node.PtrAs<APrimaryExpr>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<ASuper> Node)
{
    return BeginVisit(Node.PtrAs<APrimaryExpr>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<ASuper> Node)
{
    return EndVisit(Node.PtrAs<APrimaryExpr>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<AVarRef> Node)
{
    return Visit(Node.PtrAs<APrimaryExpr>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<AVarRef> Node)
{
    return BeginVisit(Node.PtrAs<APrimaryExpr>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<AVarRef> Node)
{
    return EndVisit(Node.PtrAs<APrimaryExpr>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<AVariableParamRef> Node)
{
    return Visit(Node.PtrAs<APrimaryExpr>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<AVariableParamRef> Node)
{
    return BeginVisit(Node.PtrAs<APrimaryExpr>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<AVariableParamRef> Node)
{
    return EndVisit(Node.PtrAs<APrimaryExpr>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<ASimpleExpr> Node)
{
    return Visit(Node.PtrAs<AExpr>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<ASimpleExpr> Node)
{
    return BeginVisit(Node.PtrAs<AExpr>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<ASimpleExpr> Node)
{
    return EndVisit(Node.PtrAs<AExpr>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<AConstExpr> Node)
{
    return Visit(Node.PtrAs<ASimpleExpr>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<AConstExpr> Node)
{
    return BeginVisit(Node.PtrAs<ASimpleExpr>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<AConstExpr> Node)
{
    return EndVisit(Node.PtrAs<ASimpleExpr>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<AConstructor> Node)
{
    return Visit(Node.PtrAs<ASimpleExpr>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<AConstructor> Node)
{
    return BeginVisit(Node.PtrAs<ASimpleExpr>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<AConstructor> Node)
{
    return EndVisit(Node.PtrAs<ASimpleExpr>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<AFuncExpr> Node)
{
    return Visit(Node.PtrAs<ASimpleExpr>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<AFuncExpr> Node)
{
    return BeginVisit(Node.PtrAs<ASimpleExpr>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<AFuncExpr> Node)
{
    return EndVisit(Node.PtrAs<ASimpleExpr>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<ASuffixedExpr> Node)
{
    return Visit(Node.PtrAs<ASimpleExpr>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<ASuffixedExpr> Node)
{
    return BeginVisit(Node.PtrAs<ASimpleExpr>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<ASuffixedExpr> Node)
{
    return EndVisit(Node.PtrAs<ASimpleExpr>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<ACall> Node)
{
    return Visit(Node.PtrAs<ASuffixedExpr>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<ACall> Node)
{
    return BeginVisit(Node.PtrAs<ASuffixedExpr>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<ACall> Node)
{
    return EndVisit(Node.PtrAs<ASuffixedExpr>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<AColonCall> Node)
{
    return Visit(Node.PtrAs<ACall>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<AColonCall> Node)
{
    return BeginVisit(Node.PtrAs<ACall>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<AColonCall> Node)
{
    return EndVisit(Node.PtrAs<ACall>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<ANormalCall> Node)
{
    return Visit(Node.PtrAs<ACall>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<ANormalCall> Node)
{
    return BeginVisit(Node.PtrAs<ACall>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<ANormalCall> Node)
{
    return EndVisit(Node.PtrAs<ACall>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<ADereferance> Node)
{
    return Visit(Node.PtrAs<ASuffixedExpr>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<ADereferance> Node)
{
    return BeginVisit(Node.PtrAs<ASuffixedExpr>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<ADereferance> Node)
{
    return EndVisit(Node.PtrAs<ASuffixedExpr>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<ABracketMember> Node)
{
    return Visit(Node.PtrAs<ADereferance>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<ABracketMember> Node)
{
    return BeginVisit(Node.PtrAs<ADereferance>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<ABracketMember> Node)
{
    return EndVisit(Node.PtrAs<ADereferance>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<ADotMember> Node)
{
    return Visit(Node.PtrAs<ADereferance>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<ADotMember> Node)
{
    return BeginVisit(Node.PtrAs<ADereferance>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<ADotMember> Node)
{
    return EndVisit(Node.PtrAs<ADereferance>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<ATypeCast> Node)
{
    return Visit(Node.PtrAs<ASimpleExpr>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<ATypeCast> Node)
{
    return BeginVisit(Node.PtrAs<ASimpleExpr>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<ATypeCast> Node)
{
    return EndVisit(Node.PtrAs<ASimpleExpr>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<ASubexpr> Node)
{
    return Visit(Node.PtrAs<AExpr>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<ASubexpr> Node)
{
    return BeginVisit(Node.PtrAs<AExpr>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<ASubexpr> Node)
{
    return EndVisit(Node.PtrAs<AExpr>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<ATypeIdentity> Node)
{
    return Visit(Node.PtrAs<ABase>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<ATypeIdentity> Node)
{
    return BeginVisit(Node.PtrAs<ABase>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<ATypeIdentity> Node)
{
    return EndVisit(Node.PtrAs<ABase>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<AArrayType> Node)
{
    return Visit(Node.PtrAs<ATypeIdentity>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<AArrayType> Node)
{
    return BeginVisit(Node.PtrAs<ATypeIdentity>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<AArrayType> Node)
{
    return EndVisit(Node.PtrAs<ATypeIdentity>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<AFuncType> Node)
{
    return Visit(Node.PtrAs<ATypeIdentity>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<AFuncType> Node)
{
    return BeginVisit(Node.PtrAs<ATypeIdentity>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<AFuncType> Node)
{
    return EndVisit(Node.PtrAs<ATypeIdentity>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<AIntrinsicType> Node)
{
    return Visit(Node.PtrAs<ATypeIdentity>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<AIntrinsicType> Node)
{
    return BeginVisit(Node.PtrAs<ATypeIdentity>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<AIntrinsicType> Node)
{
    return EndVisit(Node.PtrAs<ATypeIdentity>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<AMapType> Node)
{
    return Visit(Node.PtrAs<ATypeIdentity>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<AMapType> Node)
{
    return BeginVisit(Node.PtrAs<ATypeIdentity>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<AMapType> Node)
{
    return EndVisit(Node.PtrAs<ATypeIdentity>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<ANamedType> Node)
{
    return Visit(Node.PtrAs<ATypeIdentity>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<ANamedType> Node)
{
    return BeginVisit(Node.PtrAs<ATypeIdentity>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<ANamedType> Node)
{
    return EndVisit(Node.PtrAs<ATypeIdentity>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<AClassMember> Node)
{
    return Visit(Node.PtrAs<ABase>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<AClassMember> Node)
{
    return BeginVisit(Node.PtrAs<ABase>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<AClassMember> Node)
{
    return EndVisit(Node.PtrAs<ABase>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<AClassVar> Node)
{
    return Visit(Node.PtrAs<AClassMember>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<AClassVar> Node)
{
    return BeginVisit(Node.PtrAs<AClassMember>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<AClassVar> Node)
{
    return EndVisit(Node.PtrAs<AClassMember>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<AMethod> Node)
{
    return Visit(Node.PtrAs<AClassMember>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<AMethod> Node)
{
    return BeginVisit(Node.PtrAs<AClassMember>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<AMethod> Node)
{
    return EndVisit(Node.PtrAs<AClassMember>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<AClassContructor> Node)
{
    return Visit(Node.PtrAs<AMethod>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<AClassContructor> Node)
{
    return BeginVisit(Node.PtrAs<AMethod>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<AClassContructor> Node)
{
    return EndVisit(Node.PtrAs<AMethod>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<AClassDestructor> Node)
{
    return Visit(Node.PtrAs<AMethod>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<AClassDestructor> Node)
{
    return BeginVisit(Node.PtrAs<AMethod>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<AClassDestructor> Node)
{
    return EndVisit(Node.PtrAs<AMethod>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<ANormalMethod> Node)
{
    return Visit(Node.PtrAs<AMethod>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<ANormalMethod> Node)
{
    return BeginVisit(Node.PtrAs<AMethod>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<ANormalMethod> Node)
{
    return EndVisit(Node.PtrAs<AMethod>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<AEnumItem> Node)
{
    return Visit(Node.PtrAs<ABase>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<AEnumItem> Node)
{
    return BeginVisit(Node.PtrAs<ABase>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<AEnumItem> Node)
{
    return EndVisit(Node.PtrAs<ABase>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<AModifier> Node)
{
    return Visit(Node.PtrAs<ABase>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<AModifier> Node)
{
    return BeginVisit(Node.PtrAs<ABase>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<AModifier> Node)
{
    return EndVisit(Node.PtrAs<ABase>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<ARoot> Node)
{
    return Visit(Node.PtrAs<ABase>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<ARoot> Node)
{
    return BeginVisit(Node.PtrAs<ABase>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<ARoot> Node)
{
    return EndVisit(Node.PtrAs<ABase>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<AAttribute> Node)
{
    return Visit(Node.PtrAs<ABase>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<AAttribute> Node)
{
    return BeginVisit(Node.PtrAs<ABase>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<AAttribute> Node)
{
    return EndVisit(Node.PtrAs<ABase>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<ABlock> Node)
{
    return Visit(Node.PtrAs<ABase>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<ABlock> Node)
{
    return BeginVisit(Node.PtrAs<ABase>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<ABlock> Node)
{
    return EndVisit(Node.PtrAs<ABase>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<AFuncBody> Node)
{
    return Visit(Node.PtrAs<ABase>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<AFuncBody> Node)
{
    return BeginVisit(Node.PtrAs<ABase>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<AFuncBody> Node)
{
    return EndVisit(Node.PtrAs<ABase>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<AStat> Node)
{
    return Visit(Node.PtrAs<ABase>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<AStat> Node)
{
    return BeginVisit(Node.PtrAs<ABase>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<AStat> Node)
{
    return EndVisit(Node.PtrAs<ABase>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<AClass> Node)
{
    return Visit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<AClass> Node)
{
    return BeginVisit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<AClass> Node)
{
    return EndVisit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<AEnum> Node)
{
    return Visit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<AEnum> Node)
{
    return BeginVisit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<AEnum> Node)
{
    return EndVisit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<AInterface> Node)
{
    return Visit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<AInterface> Node)
{
    return BeginVisit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<AInterface> Node)
{
    return EndVisit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<AAssignment> Node)
{
    return Visit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<AAssignment> Node)
{
    return BeginVisit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<AAssignment> Node)
{
    return EndVisit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<ABlockStat> Node)
{
    return Visit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<ABlockStat> Node)
{
    return BeginVisit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<ABlockStat> Node)
{
    return EndVisit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<ADoWhile> Node)
{
    return Visit(Node.PtrAs<ABlockStat>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<ADoWhile> Node)
{
    return BeginVisit(Node.PtrAs<ABlockStat>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<ADoWhile> Node)
{
    return EndVisit(Node.PtrAs<ABlockStat>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<AForList> Node)
{
    return Visit(Node.PtrAs<ABlockStat>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<AForList> Node)
{
    return BeginVisit(Node.PtrAs<ABlockStat>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<AForList> Node)
{
    return EndVisit(Node.PtrAs<ABlockStat>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<AForNum> Node)
{
    return Visit(Node.PtrAs<ABlockStat>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<AForNum> Node)
{
    return BeginVisit(Node.PtrAs<ABlockStat>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<AForNum> Node)
{
    return EndVisit(Node.PtrAs<ABlockStat>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<AIfStat> Node)
{
    return Visit(Node.PtrAs<ABlockStat>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<AIfStat> Node)
{
    return BeginVisit(Node.PtrAs<ABlockStat>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<AIfStat> Node)
{
    return EndVisit(Node.PtrAs<ABlockStat>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<ARepeat> Node)
{
    return Visit(Node.PtrAs<ABlockStat>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<ARepeat> Node)
{
    return BeginVisit(Node.PtrAs<ABlockStat>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<ARepeat> Node)
{
    return EndVisit(Node.PtrAs<ABlockStat>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<AWhile> Node)
{
    return Visit(Node.PtrAs<ABlockStat>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<AWhile> Node)
{
    return BeginVisit(Node.PtrAs<ABlockStat>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<AWhile> Node)
{
    return EndVisit(Node.PtrAs<ABlockStat>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<ABreak> Node)
{
    return Visit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<ABreak> Node)
{
    return BeginVisit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<ABreak> Node)
{
    return EndVisit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<ACallStat> Node)
{
    return Visit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<ACallStat> Node)
{
    return BeginVisit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<ACallStat> Node)
{
    return EndVisit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<AFuncDef> Node)
{
    return Visit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<AFuncDef> Node)
{
    return BeginVisit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<AFuncDef> Node)
{
    return EndVisit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<AGlobal> Node)
{
    return Visit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<AGlobal> Node)
{
    return BeginVisit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<AGlobal> Node)
{
    return EndVisit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<AGoto> Node)
{
    return Visit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<AGoto> Node)
{
    return BeginVisit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<AGoto> Node)
{
    return EndVisit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<ALabel> Node)
{
    return Visit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<ALabel> Node)
{
    return BeginVisit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<ALabel> Node)
{
    return EndVisit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<ALocal> Node)
{
    return Visit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<ALocal> Node)
{
    return BeginVisit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<ALocal> Node)
{
    return EndVisit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<AReturn> Node)
{
    return Visit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<AReturn> Node)
{
    return BeginVisit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<AReturn> Node)
{
    return EndVisit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::Visit(SPtr<AVarDecl> Node)
{
    return Visit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::BeginVisit(SPtr<AVarDecl> Node)
{
    return BeginVisit(Node.PtrAs<AStat>());
}

EVisitStatus RecursiveVisitor::EndVisit(SPtr<AVarDecl> Node)
{
    return EndVisit(Node.PtrAs<AStat>());
}


void VisitorWrapper::OnVisit(SPtr<ABase> Node) {} 
void VisitorWrapper::OnBeginVisit(SPtr<ABase> Node) {} 
void VisitorWrapper::OnEndVisit(SPtr<ABase> Node) {} 
void VisitorWrapper::OnVisitFinish(SPtr<ABase> Node, EVisitStatus Status) {} 
void VisitorWrapper::OnBeginVisitFinish(SPtr<ABase> Node, EVisitStatus Status) {} 
void VisitorWrapper::OnEndVisitFinish(SPtr<ABase> Node, EVisitStatus Status) {} 
EVisitStatus VisitorWrapper::Visit(SPtr<ABase> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<ABase> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<ABase> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<AExpr> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<AExpr> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<AExpr> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<APrimaryExpr> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<APrimaryExpr> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<APrimaryExpr> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<AParentheses> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<AParentheses> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<AParentheses> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<ASelf> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<ASelf> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<ASelf> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<ASuper> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<ASuper> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<ASuper> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<AVarRef> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<AVarRef> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<AVarRef> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<AVariableParamRef> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<AVariableParamRef> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<AVariableParamRef> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<ASimpleExpr> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<ASimpleExpr> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<ASimpleExpr> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<AConstExpr> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<AConstExpr> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<AConstExpr> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<AConstructor> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<AConstructor> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<AConstructor> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<AFuncExpr> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<AFuncExpr> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<AFuncExpr> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<ASuffixedExpr> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<ASuffixedExpr> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<ASuffixedExpr> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<ACall> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<ACall> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<ACall> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<AColonCall> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<AColonCall> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<AColonCall> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<ANormalCall> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<ANormalCall> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<ANormalCall> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<ADereferance> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<ADereferance> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<ADereferance> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<ABracketMember> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<ABracketMember> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<ABracketMember> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<ADotMember> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<ADotMember> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<ADotMember> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<ATypeCast> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<ATypeCast> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<ATypeCast> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<ASubexpr> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<ASubexpr> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<ASubexpr> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<ATypeIdentity> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<ATypeIdentity> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<ATypeIdentity> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<AArrayType> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<AArrayType> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<AArrayType> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<AFuncType> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<AFuncType> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<AFuncType> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<AIntrinsicType> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<AIntrinsicType> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<AIntrinsicType> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<AMapType> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<AMapType> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<AMapType> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<ANamedType> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<ANamedType> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<ANamedType> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<AClassMember> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<AClassMember> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<AClassMember> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<AClassVar> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<AClassVar> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<AClassVar> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<AMethod> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<AMethod> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<AMethod> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<AClassContructor> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<AClassContructor> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<AClassContructor> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<AClassDestructor> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<AClassDestructor> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<AClassDestructor> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<ANormalMethod> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<ANormalMethod> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<ANormalMethod> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<AEnumItem> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<AEnumItem> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<AEnumItem> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<AModifier> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<AModifier> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<AModifier> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<ARoot> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<ARoot> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<ARoot> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<AAttribute> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<AAttribute> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<AAttribute> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<ABlock> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<ABlock> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<ABlock> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<AFuncBody> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<AFuncBody> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<AFuncBody> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<AStat> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<AStat> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<AStat> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<AClass> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<AClass> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<AClass> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<AEnum> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<AEnum> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<AEnum> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<AInterface> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<AInterface> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<AInterface> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<AAssignment> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<AAssignment> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<AAssignment> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<ABlockStat> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<ABlockStat> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<ABlockStat> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<ADoWhile> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<ADoWhile> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<ADoWhile> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<AForList> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<AForList> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<AForList> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<AForNum> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<AForNum> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<AForNum> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<AIfStat> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<AIfStat> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<AIfStat> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<ARepeat> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<ARepeat> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<ARepeat> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<AWhile> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<AWhile> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<AWhile> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<ABreak> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<ABreak> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<ABreak> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<ACallStat> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<ACallStat> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<ACallStat> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<AFuncDef> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<AFuncDef> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<AFuncDef> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<AGlobal> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<AGlobal> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<AGlobal> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<AGoto> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<AGoto> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<AGoto> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<ALabel> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<ALabel> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<ALabel> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<ALocal> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<ALocal> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<ALocal> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<AReturn> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<AReturn> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<AReturn> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::Visit(SPtr<AVarDecl> Node)
{
    OnVisit(Node);
    EVisitStatus Ret = WrappedVisitor->Visit(Node);
    OnVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::BeginVisit(SPtr<AVarDecl> Node)
{
    OnBeginVisit(Node);
    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);
    OnBeginVisitFinish(Node, Ret);
    return Ret;
}

EVisitStatus VisitorWrapper::EndVisit(SPtr<AVarDecl> Node)
{
    OnEndVisit(Node);
    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);
    OnEndVisitFinish(Node, Ret);
    return Ret;
}

}
