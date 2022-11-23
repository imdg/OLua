/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#pragma once
#include "Common.h"
#include "CompileMsg.h"
#include "AstCommon.h"
#include "ScopeDecl.h"
#include "ExpressionDecl.h"
#include "StatementDecl.h"

namespace OL
{

RTTI_ENUM(EParseStatus,
    PS_Success,
    PS_Fail
)

class Lexer;
class ABase;
class CompileMsg;

class Parser
{
public:
    Parser(Lexer& InLex);
    Parser(Lexer& InLex, CompileMsg& InCM);
    //EParseStatus Parse_Root();
    
    // Expresion
    AExpr*          Parse_Expr();
    AExpr*          Parse_BinOpSubexpr();
    AExpr*          Parse_UniOpSubexpr();
    AExpr*          Parse_SimpleExpr();
    AConstExpr*     Parse_ConstExpr();
    AConstructor*   Parse_Constructor();
    AExpr*          Parse_TypeExp();
    AExpr*          Parse_SuffixedExpr();
    AExpr*          Parse_PrimaryExpr();
    bool            Helper_ParseExprList(OLList<SPtr<AExpr>>& OutList);
    AExpr*          Parse_FuncExpr();
    ATypeIdentity*  Parse_TypeIdentity();
    AFuncType*      Parse_FuncType();
    AMapType*       Parse_MapType();
    
    //bool            Helper_ParseSuffixedExprList(OLList<SPtr<ASuffixedExpr>>& OutList);

    // Statement
    AStat*          Parse_Extern();
    AStat*          Parse_Stat(bool AcceptGlobal);
    AStat*          Parse_While();
    AStat*          Parse_FuncDef(bool IsExtern);
    AFuncBody*      Parse_FuncBody(bool IsAbstract);
    AStat*          Parse_If();
    ABlock*         Parse_Block(bool AcceptGlobal);
    AStat*          Parse_ForNum(AVarDecl* FirstDecl);
    AStat*          Parse_ForList(AVarDecl* FirstDecl);
    AStat*          Parse_For();
    AStat*          Parse_Repeat();

    AStat*          Parse_ExprStat();
    AStat*          Parse_Assignment(AExpr* FirstExpr);
    AStat*          Parse_Return();
    AStat*          Parse_Lable();
    AStat*          Parse_Break();
    AStat*          Parse_Goto();
    AStat*          Parse_Local();
    AStat*          Parse_Global();
    AVarDecl*       Parse_VarDecl(bool AcceptVariableParam);
    AAttribute *    Parse_Attribute();
    AAlias*         Parse_Alias();

    bool            Helper_ParseBlock(OLList< SPtr<AStat> >& Block, bool AcceptGlobal);
    bool            Helper_ParseVarDeclList(OLList < SPtr<AVarDecl> >& DeclList, bool AcceptVariableParam);
    bool            Helper_ParseTypeList(OLList<SPtr<ATypeIdentity> >& TypeList);
    bool            Helper_ParseFuncParam(OLList<SPtr<AVarDecl>>& ParamList);
    bool            Helper_ParseFuncRet(OLList<SPtr<ATypeIdentity>>& ReturnType);

    // Scope
    AClass*         Parse_Class(bool IsExternal);
    AModifier*      Parse_Modifier();
    AClassMember*   Parse_ClassMember(bool IsExternal);
    AMethod*        Parse_Method(bool IsAbstract);
    AClassVar*       Parse_ClassVar(bool IsExternal);
    AClassContructor* Parse_ClassConstructor();
    ARoot*          Parse_Root(OLString Name);
    AEnum*          Parse_Enum();
    AEnumItem*      Parse_EnumItem();
    AInterface*     Parse_Interface();

    bool            Helper_ParseClassTitle(AClass* Owner, bool IsExternal);
    bool            Helper_ParseClassBody(AClass* Owner, bool IsExternal);
    bool            Helper_ParseInterfaceTitle(AInterface* Owner);
    bool            Helper_ParseInterfaceBody(AInterface* Owner);

    

private:
    Lexer& Lex;
    SPtr<ARoot> Root;
    CompileMsg& CM;
};


}

