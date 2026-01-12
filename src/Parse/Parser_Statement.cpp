/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "Common.h"
#include "Lexer.h"
#include "Parser.h"
#include "ScopeInclude.h"
#include "ExpressionInclude.h"
#include "StatementInclude.h"
#include "AstPool.h"
#include "CompileMsg.h"

namespace OL
{

// stat -> whilestat
//          | ifstat
//          | forstat
//          | repeatstat
//          | returnstat
//          | breakstat
//          | gotostat
//          | localstat
//          | funcdef
//          | lable
//          | exprstat
//          | aliasstat
AStat* Parser::Parse_Stat(bool AcceptGlobal)
{
    switch (Lex.GetCurrent().Tk)
    {
    case TKK_while:
        return Parse_While();
    case TKK_if:
        return Parse_If();
    case TKK_for:
        return Parse_For();
    case TKK_repeat:
        return Parse_Repeat();
    case TKK_return:
        return Parse_Return();
    case TKK_break:
        return Parse_Break();
    case TKK_goto:
        return Parse_Goto();
    case TKK_local:
        return Parse_Local();
    case TKK_function:
        return Parse_FuncDef(false);
    case TKK_extern:
        return Parse_Extern();
    case TKS_colon2:
        return Parse_Lable();
    case TKK_class:
        return Parse_Class(false);
    case TKK_enum:
        return Parse_Enum();
    case TKK_interface:
        return Parse_Interface();
    case TKK_global:
        if(AcceptGlobal)
        {
            return Parse_Global();
        }
        else
        {
            CM.Log(CMT_CannotDefineGlobal, Lex.GetCurrent().SrcRange);
            return nullptr;
        }
    case TKK_alias:
        return Parse_Alias();
    default:
        // Includs function call and assignment
        return Parse_ExprStat();
        break;
    }
}

// whilestat -> while cond do block end
AStat* Parser::Parse_While()
{

    // Validate 'while'
    OL_ASSERT(Lex.GetCurrent().Tk == TKK_while);

    Lex.Next();

    // Parse condition
    SourceRange CondBegin = Lex.GetCurrent().SrcRange;
    AExpr* Cond = Parse_Expr();
    if(Cond == nullptr)
    {
        CM.Log(CMT_FailWhileCondition, CondBegin);
        return nullptr;
    }

    // Validate 'do'
    if(Lex.GetCurrent().Tk != TKK_do)
    {
        CM.Log(CMT_WhileMissingDo, Lex.GetCurrent().SrcRange);
        AstPool::Delete(Cond);
        return nullptr;
    }
    Lex.Next();

    // Save the beginning line for error reporting
    SourceRange BeginLine = Lex.GetCurrent().SrcRange;

    // Parse inside statements of while-loop
    AWhile* WhileStat = AstPool::New<AWhile>(Lex.GetCurrent().SrcRange);

    SourceRange BlockBegin = Lex.GetCurrent().SrcRange;
    ABlock* Block = Parse_Block(false);
    if(Block == nullptr)
    {
        AstPool::Delete(Cond);
        AstPool::Delete(WhileStat);
        CM.Log(CMT_FailBlock, BlockBegin);
        return nullptr;
    }
    WhileStat->MainBlock = Block;

    // Validate the final 'end'
    if(Lex.GetCurrent().Tk != TKK_end)
    {
        AstPool::Delete(Cond);
        AstPool::Delete(WhileStat);
        CM.Log(CMT_WhileMissingEnd, BeginLine);
    }
    Lex.Next();

    WhileStat->Condition = Cond;
    return WhileStat;
}

ABlock* Parser::Parse_Block(bool AcceptGlobal)
{
    SourceRange BlockBegin = Lex.GetCurrent().SrcRange;
    ABlock* Block = AstPool::New<ABlock>(Lex.GetCurrent().SrcRange);
    if(false == Helper_ParseBlock(Block->Stats, AcceptGlobal))
    {
        AstPool::Delete(Block);
        CM.Log(CMT_FailBlock, BlockBegin);
        return nullptr;
    }
    return Block;
}


// ifstat -> if cond then block {elseif cond then block} [else block] end
AStat* Parser::Parse_If()
{
    // Validate 'if'
    OL_ASSERT(Lex.GetCurrent().Tk == TKK_if);
    
    // Save beginning line for error reporting
    SourceRange BeginLine = Lex.GetCurrent().SrcRange;
    AIfStat* Ret = AstPool::New<AIfStat>(Lex.GetCurrent().SrcRange);

    Lex.Next();

    // Parse the first condition in 'if'
    AExpr* Cond = Parse_Expr();
    if(Cond == nullptr)
    {
        CM.Log(CMT_FailIfCondition, BeginLine);
        AstPool::Delete(Ret);
        return nullptr;
    }

    // Validate 'then'
    if(Lex.GetCurrent().Tk != TKK_then)
    {
        AstPool::Delete(Cond);
        AstPool::Delete(Ret);
        CM.Log(CMT_IfMissingThen, Lex.GetCurrent().SrcRange);
        return nullptr;
    }
    Lex.Next();
    SourceRange BlockBegin = Lex.GetCurrent().SrcRange;
    ABlock* MainBlock = Parse_Block(false);
    if(MainBlock == nullptr)
    {
        AstPool::Delete(Cond);
        AstPool::Delete(Ret);
        return nullptr;
    }
    Ret->MainCondition = Cond;
    Ret->MainIfBlock = MainBlock;

    ETokenType Tk = Lex.GetCurrent().Tk;
    // Finish statement if 'end'
    if(Tk == TKK_end)
    {
        Lex.Next();
        return Ret;
    }

    // Must be 'elseif' or 'else'
    if(Tk != TKK_elseif && Tk != TKK_else)
    {
        AstPool::Delete(Ret);
        CM.Log(CMT_IfMissingEnding, BeginLine);
        return nullptr;
    }

    // Parse all 'elseif' blocks
    while(Lex.GetCurrent().Tk == TKK_elseif)
    {
        BeginLine = Lex.GetCurrent().SrcRange;

        Lex.Next();

        // Parse condition in elseif
        AExpr* ElseIfCond = Parse_Expr();
        if(ElseIfCond == nullptr)
        {
            CM.Log(CMT_FailIfCondition, BeginLine);
            AstPool::Delete(Ret);
            return nullptr;
        }


        // Validate following 'then'
        if(Lex.GetCurrent().Tk != TKK_then)
        {
            AstPool::Delete(ElseIfCond);
            AstPool::Delete(Ret);
            CM.Log(CMT_ElseIfMissingThen, Lex.GetCurrent().SrcRange);
            return nullptr;
        }
        Lex.Next();

        ElseIfBlockInfo& EI = Ret->ElseIfBlocks.AddConstructed();
        EI.Condition = ElseIfCond;
        BlockBegin = Lex.GetCurrent().SrcRange;
        // Parse statements in this 'elseif'

        ABlock* ElseIfBlock = Parse_Block(false);
        if(ElseIfBlock == nullptr)
        {
            AstPool::Delete(Ret);
            //CM.Log(CMT_FailBlock, BlockBegin);
            return nullptr;
        }
        EI.Block = ElseIfBlock;

        // Finish if 'end'
        if (Lex.GetCurrent().Tk == TKK_end)
        {
            Lex.Next();
            return Ret;
        }

        // Must be 'elseif' or 'else'
        if (Lex.GetCurrent().Tk != TKK_elseif && Lex.GetCurrent().Tk != TKK_else)
        {
            AstPool::Delete(Ret);
            CM.Log(CMT_ElseIfMissingEnding, BeginLine);
            return nullptr;
        }
    }

    // Have not return yet. Must be a final 'else' block
    BeginLine = Lex.GetCurrent().SrcRange;
    Lex.Next();
    ABlock* ElseBlock = Parse_Block(false);
    if(ElseBlock == nullptr)
    {
        AstPool::Delete(Ret);
        //CM.Log(CMT_FailBlock, BeginLine);
        return nullptr;
    }
    Ret->ElseBlock = ElseBlock;

    // Validate the final 'end'
    if (Lex.GetCurrent().Tk != TKK_end)
    {
        AstPool::Delete(Ret);
        CM.Log(CMT_ElseMissingEnding, BeginLine);
        return nullptr;
    }

    Lex.Next();
    return Ret;
}


// forstat -> for fornum | forlist
AStat* Parser::Parse_For()
{
    // Validate 'for'
    OL_ASSERT(Lex.GetCurrent().Tk == TKK_for);
    Lex.Next();

    // Parse the first loop variant
    SourceRange BeginLine = Lex.GetCurrent().SrcRange;
    AVarDecl* FirstDecl = Parse_VarDecl(false);
    if(FirstDecl == nullptr)
    {
        CM.Log(CMT_FailForDecl, BeginLine);
        return nullptr;
    }

    // ForNum has only one variant, and followed by '='
    if(Lex.GetCurrent().Tk == TKS_assign)
    {
        return Parse_ForNum(FirstDecl);
    }
    else
    {
        return Parse_ForList(FirstDecl);
    }

    
}

// fornum -> vardecl = expr, expr [,expr] do block end
AStat* Parser::Parse_ForNum(AVarDecl* FirstDecl)
{
    OL_ASSERT(Lex.GetCurrent().Tk == TKS_assign);
    Lex.Next();

    AForNum* ForNum = AstPool::New<AForNum>(Lex.GetCurrent().SrcRange);
    ForNum->VarDecl = FirstDecl;

    // Parse the initial value
    SourceRange BeginLine = Lex.GetCurrent().SrcRange;
    AExpr* From = Parse_Expr();
    if(From == nullptr)
    {
        CM.Log(CMT_FailForRange, BeginLine);
        AstPool::Delete(ForNum);
        return nullptr;
    }
    ForNum->Begin = From;

    // Check ','
    if(Lex.GetCurrent().Tk != TKS_comma)
    {
        AstPool::Delete(ForNum);
        CM.Log(CMT_ForNeedsComma, Lex.GetCurrent().SrcRange);
        return nullptr;
    }
    Lex.Next();

    // Parse the end value
    AExpr* To = Parse_Expr();
    if(To == nullptr)
    {
        CM.Log(CMT_FailForRange, BeginLine);
        AstPool::Delete(ForNum);
        return nullptr;
    }
    ForNum->End = To;

    // if a ',' is seen, the step value should be parsed
    if(Lex.GetCurrent().Tk == TKS_comma)
    {
        Lex.Next();
        AExpr* Step = Parse_Expr();
        if(Step == nullptr)
        {
            CM.Log(CMT_FailForRange, BeginLine);
            AstPool::Delete(ForNum);
            return nullptr;
        }
        ForNum->Step = Step;
    }

    // Check 'do' keyword
    if(Lex.GetCurrent().Tk != TKK_do)
    {
        AstPool::Delete(ForNum);
        CM.Log(CMT_ForNeedsDo, Lex.GetCurrent().SrcRange);
        return nullptr;
    }

    Lex.Next();

    // Parse the loop body
    BeginLine = Lex.GetCurrent().SrcRange;
    ABlock* Block = Parse_Block(false);
    if(Block == nullptr)
    {
        AstPool::Delete(ForNum);
        return nullptr;
    }
    ForNum->MainBlock = Block;
    // Validate the final 'end'
    if(Lex.GetCurrent().Tk != TKK_end)
    {
        AstPool::Delete(ForNum);
        CM.Log(CMT_ForNeedsEnd, Lex.GetCurrent().SrcRange);
        return nullptr;
    }
    Lex.Next();
    return ForNum;
}

// forlist -> vardecl{, vardecl} in exprlist do block end
AStat* Parser::Parse_ForList(AVarDecl* FirstDecl)
{
    AForList* ForList = AstPool::New<AForList>(Lex.GetCurrent().SrcRange);
    ForList->VarList.Add(FirstDecl);

    SourceRange BeginLine = Lex.GetCurrent().SrcRange;
    // Parse the variant declearations, sep by ','
    while(Lex.GetCurrent().Tk == TKS_comma)
    {
        Lex.Next();
        AVarDecl* NextDecl = Parse_VarDecl(false);
        if(NextDecl == nullptr)
        {
            CM.Log(CMT_FailForDecl, BeginLine);
            AstPool::Delete(ForList);
            return nullptr;
        }
        ForList->VarList.Add(NextDecl);
    }

    // Check the  'in' keyword
    if(Lex.GetCurrent().Tk != TKK_in)
    {
        AstPool::Delete(ForList);
        CM.Log(CMT_ForNeedsIn, Lex.GetCurrent().SrcRange);
        return nullptr;
    }

    Lex.Next();

    // Parse the expr that returns the iterator
    AExpr* Iterator = Parse_Expr();
    if(Iterator == nullptr)
    {
        CM.Log(CMT_FailForIter, BeginLine);
        AstPool::Delete(ForList);
        return nullptr;
    }

    ForList->Iterator = Iterator;

    // Check the 'do' keyword
    if(Lex.GetCurrent().Tk != TKK_do)
    {
        AstPool::Delete(ForList);
        CM.Log(CMT_ForNeedsDo, Lex.GetCurrent().SrcRange);
        return nullptr;
    }
    Lex.Next();

    // Parse the loop body
    BeginLine = Lex.GetCurrent().SrcRange;
    ABlock* Block = Parse_Block(false);
    if(Block == nullptr)
    {
        AstPool::Delete(ForList);
        return nullptr;
    }
    ForList->MainBlock = Block;

    // Validate the final 'end'
    if(Lex.GetCurrent().Tk != TKK_end)
    {
        AstPool::Delete(ForList);
        CM.Log(CMT_ForNeedsEnd, Lex.GetCurrent().SrcRange);
        return nullptr;
    }
    Lex.Next();
    return ForList;
}

// repeatstat -> repeat block until cond end
AStat* Parser::Parse_Repeat()
{
    OL_ASSERT(Lex.GetCurrent().Tk == TKK_repeat);
    Lex.Next();

    SourceRange BeginLine = Lex.GetCurrent().SrcRange;
    ARepeat* Repeat = AstPool::New<ARepeat>(Lex.GetCurrent().SrcRange);

    // Parse the loop body first
    ABlock* Block = Parse_Block(false);
    if(Block == nullptr)
    {
        CM.Log(CMT_FailBlock, BeginLine);
        AstPool::Delete(Repeat);
        return nullptr;
    }
    Repeat->MainBlock = Block;

    // Validate keyword 'until' 
    if(Lex.GetCurrent().Tk != TKK_until)
    {
        AstPool::Delete(Repeat);
        CM.Log(CMT_RepeatNeedsUntil, Lex.GetCurrent().SrcRange);
        return nullptr;
    }

    Lex.Next();

    // Parse loop condition 
    AExpr* Cond = Parse_Expr();
    if(Cond == nullptr)
    {
        CM.Log(CMT_FailRepeatCondition, BeginLine);
        AstPool::Delete(Repeat);
        return nullptr;
    }

    Repeat->Condition = Cond;
    return Repeat;
}

// vardecl -> name [as type]
AVarDecl* Parser::Parse_VarDecl(bool AcceptVariableParam)
{
    // Variant name
    Token& Curr = Lex.GetCurrent();
    
    if(Curr.Tk != TK_name && !(AcceptVariableParam && Curr.Tk == TKS_dot3 ))
    {
        CM.Log(CMT_NeedVarName, Curr.SrcRange);
        return nullptr;
    }
    AVarDecl* Decl = AstPool::New<AVarDecl>(Curr.SrcRange);
    if(Curr.Tk == TK_name)
        Decl->VarName = Curr.StrOrNameVal;
    else
    {
        Decl->VarName = T("...");
        Decl->IsVariableParam = true;
    }

    // if 'as' is seen, parse the following type
    if(Lex.Next().Tk == TKK_as)
    {
        Token TypeToken = Lex.Next();
        if(TypeToken.Tk == TKK_const)
        {
            Decl->IsConst = true;
            TypeToken = Lex.Next();
        }

        Decl->VarType = Parse_TypeIdentity(true);
        if(Decl->VarType == nullptr)
        {
            AstPool::Delete(Decl);
            CM.Log(CMT_NeedTypeName, TypeToken.SrcRange);
            return nullptr;
        }
        //Lex.Next();
    }
    else
    {
        // No type specified, 
        AIntrinsicType* AnyType = AstPool::New<AIntrinsicType>(Lex.GetCurrent().SrcRange);
        AnyType->Type = IT_any;
        AnyType->IsImplicitAny = true;
        Decl->VarType = AnyType;

    }
    return Decl;


}

AStat* Parser::Parse_Extern()
{
    Lex.Next();
    if(Lex.GetCurrent().Tk == TKK_function)
    {
        AStat* Func = Parse_FuncDef(true);
        if(Func != nullptr)
            return Func;
    }
    else if(Lex.GetCurrent().Tk == TKK_class)
    {
        AClass* Class = Parse_Class(true);
        if(Class != nullptr)
        {
            Class->IsExternal = true;
            return Class;
        }
    }
    CM.Log(CMT_ExternDefError, Lex.GetCurrent().SrcRange);
    return nullptr;
}

// funcdef -> function name{.name}[:name] funcbody
AStat* Parser::Parse_FuncDef(bool IsExtern)
{
    OL_ASSERT(Lex.GetCurrent().Tk == TKK_function);
    Lex.Next();

    AAttribute* Attrib = nullptr;
    if(Lex.GetCurrent().Tk == TKS_lbracket)
    {
        Attrib = Parse_Attribute();
        if(Attrib == nullptr)
        {
            return nullptr;
        }
    }


    bool NeedSelf = false;
    OLList<OLString> OwnerName;
    OLString FuncName;

    // Parse funtion owner table, only '.' is allowed to derefrence
    while(true)
    {
        Token& Curr = Lex.GetCurrent();
        if(Lex.GetCurrent().Tk != TK_name)
        {
            CM.Log(CMT_FuncNeedName, Lex.GetCurrent().SrcRange);
            if(Attrib)
                AstPool::Delete(Attrib);
            return nullptr;
        }

        if(FuncName != T(""))
        {
            OwnerName.Add(FuncName);
        }

        // Always the last part of the sequence
        FuncName = Lex.GetCurrent().StrOrNameVal;
        
        if(Lex.Next().Tk != TKS_dot)
            break;
        Lex.Next();
    }

    // If ':' is seen, make the name after it the new FuncName, and the function need pass the owner as 'self'
    if(Lex.GetCurrent().Tk == TKS_colon)
    {
        NeedSelf = true;
        OwnerName.Add(FuncName);
        Token& FinalName = Lex.Next();
        if(FinalName.Tk != TK_name)
        {
            CM.Log(CMT_FuncNeedName, Lex.GetCurrent().SrcRange);
            if(Attrib)
                AstPool::Delete(Attrib);
            return nullptr;
        }
        FuncName = FinalName.StrOrNameVal;
        Lex.Next();
    }

    // Convert the name sequence to AST
    AExpr* Owner = nullptr;
    for(int i = 0; i < OwnerName.Count(); i++)
    {
        if(i == 0)
        {
            Owner = AstPool::New<AVarRef>(Lex.GetCurrent().SrcRange);
            Owner->As<AVarRef>()->VarName = OwnerName[i];
        }
        else
        {
            ADotMember* NewOwner = AstPool::New<ADotMember>(Lex.GetCurrent().SrcRange);
            NewOwner->Target = Owner;
            NewOwner->Field = OwnerName[i];
            Owner = NewOwner;
        }
    }

    AFuncDef* Ret = AstPool::New<AFuncDef>(Lex.GetCurrent().SrcRange);
    Ret->Owner = Owner;
    Ret->NeedSelf = NeedSelf;
    Ret->Name = FuncName;
    Ret->IsExtern = IsExtern;
    Ret->Attrib = Attrib;
    
    // Parse the function body
    SourceRange BeginLine = Lex.GetCurrent().SrcRange;
    
    AFuncBody* Body = Parse_FuncBody(IsExtern);
    if(Body == nullptr)
    {
        CM.Log(CMT_FailFuncBody, BeginLine);
        AstPool::Delete(Ret);
        return nullptr;
    }
    Ret->Body = Body;
    return Ret;

}

// funcbody -> '(' vardef_list ')' [as typelist]  {block end }
// vardef_list -> vardef {, vardef ...}
// typelist -> type | '(' type {, type} ')'
AFuncBody* Parser::Parse_FuncBody(bool IsAbstract)
{
    SourceRange BeginLine = Lex.GetCurrent().SrcRange;
    AFuncBody *Ret = AstPool::New<AFuncBody>(Lex.GetCurrent().SrcRange);

    if(false == Helper_ParseFuncParam(Ret->Params))
    {
        AstPool::Delete(Ret);
        return nullptr;
    }

    // Parse return type if 'as' is seen
    if(Lex.Next().Tk == TKK_as)
    {
        Lex.Next();
        if (false == Helper_ParseFuncRet(Ret->ReturnType))
        {
            AstPool::Delete(Ret);
            return nullptr;
        }
    };

    Ret->IsDeclearOnly = IsAbstract;
    BeginLine = Lex.GetCurrent().SrcRange;
    // Parse the function body
    if (IsAbstract == false)
    {
        ABlock* Block = Parse_Block(false);
        if (Block == nullptr)
        {
            CM.Log(CMT_FailBlock, BeginLine);
            AstPool::Delete(Ret);
            return nullptr;
        }
        Ret->MainBlock = Block;


        // Validate the final 'end'
        if (Lex.GetCurrent().Tk != TKK_end)
        {
            CM.Log(CMT_FuncMissingEnd, Lex.GetCurrent().SrcRange);
            AstPool::Delete(Ret);
            return nullptr;
        }
        Lex.Next();
    }
    return Ret;
}

ATypeIdentity*      Parser::Parse_FuncType(bool AcceptNilable)
{
    OL_ASSERT(Lex.GetCurrent().Tk == TKK_function);
    Lex.Next();
    SourceRange BeginLine = Lex.GetCurrent().SrcRange;

    bool IsArray = false;
    if(Lex.GetCurrent().Tk == TKS_lbracket)
    {
        Lex.Next();
        if(Lex.GetCurrent().Tk != TKS_rbracket)
        {
            CM.Log(CMT_IncompleteBracket, Lex.GetCurrent().SrcRange);
            return nullptr;
        }
        Lex.Next();
        IsArray = true;
    }


    bool IsNilable = false;
    if(Lex.GetCurrent().Tk == TKS_question)
    {
        if(AcceptNilable == false)
            CM.Log(CMT_NotAcceptedNilable, Lex.GetCurrent().SrcRange);
        else
            IsNilable = true;
        Lex.Next();
    }

    AFuncType *Ret = AstPool::New<AFuncType>(Lex.GetCurrent().SrcRange);
    Ret->IsNilable = IsNilable;
    if(false  == Helper_ParseFuncParam(Ret->Params))
    {
        AstPool::Delete(Ret);
        return nullptr;
    }

    // Parse return type if 'as' is seen
    if(Lex.Next().Tk == TKK_as)
    {
        Lex.Next();

        if(false == Helper_ParseFuncRet(Ret->ReturnType))
        {
            AstPool::Delete(Ret);
            return nullptr;
        }

    };

    if(IsArray)
    {
        AArrayType* Arr = AstPool::New<AArrayType>(BeginLine);
        Arr->ElemType = Ret;
        return Arr;
    }
    else
    {
        return Ret;
    }
}


bool Parser::Helper_ParseFuncParam(OLList<SPtr<AVarDecl>>& ParamList)
{
    // To parse the param list, validate '(' first
    if(Lex.GetCurrent().Tk != TKS_lparenthese)
    {
        CM.Log(CMT_FuncNeedsParam, Lex.GetCurrent().SrcRange);
        return false;
    }
    Lex.Next();

    // Parse the param list
    if(Lex.GetCurrent().Tk != TKS_rparenthese)
    {
        if (false == Helper_ParseVarDeclList(ParamList, true))
        {
            CM.Log(CMT_FailFuncParamDecl, Lex.GetCurrent().SrcRange);
            return false;
        }
    }

    // Finish param list, validate the ')'
    if (Lex.GetCurrent().Tk != TKS_rparenthese)
    {
        CM.Log(CMT_FuncParamMissingRP, Lex.GetCurrent().SrcRange);
        return false;
    }
    return true;
}

bool Parser::Helper_ParseFuncRet(OLList<SPtr<ATypeIdentity>>& ReturnType)
{
        // Types of mulitiple return values should be in a parenthese, which is optional for single return value
    if(Lex.GetCurrent().Tk == TKS_lparenthese)
    {
        Lex.Next();
        if(false == Helper_ParseTypeList(ReturnType))
        {
            CM.Log(CMT_FailFuncRetType, Lex.GetCurrent().SrcRange);
            return false;
        }
        if(Lex.GetCurrent().Tk != TKS_rparenthese)
        {
            CM.Log(CMT_FuncTypeMissingRB, Lex.GetCurrent().SrcRange);
            return false;
        }
        Lex.Next();
    }
    else
    {
        // Single return value
        ATypeIdentity* RetType = Parse_TypeIdentity(true);
        if(RetType == nullptr)
        {
            CM.Log(CMT_NeedTypeName, Lex.GetCurrent().SrcRange);
            return false;
        }

        ReturnType.Add(RetType);

    }
    return true;
}

// exprstat -> call | assignment
AStat* Parser::Parse_ExprStat()
{
    // Parse a suffixed expr, because there is no reason an upper level "SubExpr" makes sense here for neither assignment nor call
    AExpr* First = Parse_SuffixedExpr();
    if(First == nullptr)
        return nullptr;
    
    // Ensure this expr is an L-value
    // PS: Function call is an L-value that returns references can be assigned to
    if(First->IsLValue() == false)
    {
        CM.Log(CMT_NeedLValue, First->SrcRange);
        AstPool::Delete(First);
        return nullptr;
    }

    // Check if there is following '=' or ',', which confirms assignment
    if(Lex.GetCurrent().Tk == TKS_assign || Lex.GetCurrent().Tk == TKS_comma)
    {
        return Parse_Assignment(First);
    }

    // Sole ACall is an expr, should be wrapped in a ACallStat which is a AStat
    if(First->Is<ACall>())
    {
        ACallStat* CallStat = AstPool::New<ACallStat>(First->SrcRange);
        CallStat->CallExpr = First->As<ACall>();
        return CallStat;
    }

    // Other types of expr should not be seen here
    CM.Log(CMT_UnknownStat, First->SrcRange);
    return nullptr;
}

// assignment -> lvalue {, lvalue} = exprlist
// lvalue -> suffixedexpr   (not realy, more varifications have been done)
AStat* Parser::Parse_Assignment(AExpr* FirstExpr)
{
    // Parse all left expr
    AAssignment* Ret = AstPool::New<AAssignment>(Lex.GetCurrent().SrcRange);
    Ret->LeftExprs.Add(SPtr<AExpr>(FirstExpr));
    while(Lex.GetCurrent().Tk == TKS_comma)
    {
        Lex.Next();
        AExpr* NextLValue = Parse_SuffixedExpr();
        if(NextLValue == nullptr)
        {
            AstPool::Delete(Ret);
            return nullptr;
        }
    }

    // Validate '='
    if(Lex.GetCurrent().Tk != TKS_assign)
    {
        AstPool::Delete(Ret);
        CM.Log(CMT_AssignNeedsEq, Lex.GetCurrent().SrcRange);
        return nullptr;
    }
    Lex.Next();

    // Parse the right side expr list
    if(false == Helper_ParseExprList(Ret->RightExprs))
    {
        AstPool::Delete(Ret);
        CM.Log(CMT_AssignNeedsExpr, Lex.GetCurrent().SrcRange);
        return nullptr;
    }
    return Ret;
    
}

// returnstat -> return exprllist
AStat* Parser::Parse_Return()
{
    OL_ASSERT(Lex.GetCurrent().Tk == TKK_return);
    Lex.Next();
    AReturn* Ret = AstPool::New<AReturn>(Lex.GetCurrent().SrcRange);
    Token Curr = Lex.GetCurrent();
    
    // return stat only exist at the end of a block
    if(Curr.Tk == TKK_end || Curr.Tk == TKK_until || Curr.Tk == TKK_else || Curr.Tk == TKK_elseif ||  Curr.Tk == TK_eof)
        return Ret;

    if(false == Helper_ParseExprList(Ret->Ret))
    {
        AstPool::Delete(Ret);
        return nullptr;
    }
    Curr = Lex.GetCurrent();
    if(Curr.Tk == TKK_end || Curr.Tk == TKK_until || Curr.Tk == TKK_else || Curr.Tk == TKK_elseif ||  Curr.Tk == TK_eof)
        return Ret;
    else
    {
        CM.Log(CMT_LastReturn, Curr.SrcRange);
        AstPool::Delete(Ret);
        return nullptr;
    }
}


// lable -> ::name
AStat* Parser::Parse_Lable()
{
    OL_ASSERT(Lex.GetCurrent().Tk == TKS_colon2);
    Lex.Next();
    if(Lex.GetCurrent().Tk != TK_name)
    {
        CM.Log(CMT_LableNeedName, Lex.GetCurrent().SrcRange);
        return nullptr;
    }
    ALabel* Ret = AstPool::New<ALabel>(Lex.GetCurrent().SrcRange);
    Ret->Name = Lex.GetCurrent().StrOrNameVal;
    Lex.Next();
    return Ret;
}

// breakstat -> break
AStat* Parser::Parse_Break()
{
    OL_ASSERT(Lex.GetCurrent().Tk == TKK_break);
    ABreak* Ret = AstPool::New<ABreak>(Lex.GetCurrent().SrcRange);
    Lex.Next();
    return Ret;
}

// gotostat -> goto name
AStat* Parser::Parse_Goto()
{
    OL_ASSERT(Lex.GetCurrent().Tk == TKK_goto);
    Lex.Next();
    if(Lex.GetCurrent().Tk != TK_name)
    {
        CM.Log(CMT_GotoNeedName, Lex.GetCurrent().SrcRange);
        return nullptr;
    }
    Lex.Next();
    AGoto* Ret = AstPool::New<AGoto>(Lex.GetCurrent().SrcRange);
    Ret->Label = Lex.GetCurrent().StrOrNameVal;
    return Ret;
}

// localstat -> local vardef {, vardef} [= exprlist]
AStat* Parser::Parse_Local()
{
    OL_ASSERT(Lex.GetCurrent().Tk == TKK_local);
    Lex.Next();

    // Parse the variant declearation
    ALocal* Ret = AstPool::New<ALocal>(Lex.GetCurrent().SrcRange);
    while(true)
    {
        AVarDecl* Decl = Parse_VarDecl(false);

        if(Decl == nullptr)
        {
            AstPool::Delete(Ret);
            return nullptr;
        }

        Ret->Decls.Add(SPtr<AVarDecl>(Decl));

        ETokenType Tk = Lex.GetCurrent().Tk;
        if(Tk == TKS_comma)
        {
            Lex.Next();
        }
        else
        {
            break;
        }
    }

    // Without initial value, return
    if(Lex.GetCurrent().Tk != TKS_assign)
        return Ret;
    
    Lex.Next();

    // Parse the initial value
    while(true)
    {
        AExpr* Exp = Parse_Expr();
        if(Exp == nullptr)
        {
            AstPool::Delete(Ret);
            return nullptr;
        }
        Ret->Inits.Add(SPtr<AExpr>(Exp));

        if(Lex.GetCurrent().Tk == TKS_comma)
        {
            Lex.Next();
        }
        else
        {
            break;
        }
    }
    return Ret;
}


AStat* Parser::Parse_Global()
{
    OL_ASSERT(Lex.GetCurrent().Tk == TKK_global);
    Lex.Next();

    // Parse the variant declearation
    AGlobal* Ret = AstPool::New<AGlobal>(Lex.GetCurrent().SrcRange);
    while(true)
    {
        AVarDecl* Decl = Parse_VarDecl(false);

        if(Decl == nullptr)
        {
            AstPool::Delete(Ret);
            return nullptr;
        }

        Ret->Decls.Add(SPtr<AVarDecl>(Decl));

        ETokenType Tk = Lex.GetCurrent().Tk;
        if(Tk == TKS_comma)
        {
            Lex.Next();
        }
        else
        {
            break;
        }
    }

    // Without initial value, return
    if(Lex.GetCurrent().Tk != TKS_assign)
        return Ret;
    
    Lex.Next();

    // Parse the initial value
    while(true)
    {
        AExpr* Exp = Parse_Expr();
        if(Exp == nullptr)
        {
            AstPool::Delete(Ret);
            return nullptr;
        }
        Ret->Inits.Add(SPtr<AExpr>(Exp));

        if(Lex.GetCurrent().Tk == TKS_comma)
        {
            Lex.Next();
        }
        else
        {
            break;
        }
    }
    return Ret;
}

AAttribute* Parser::Parse_Attribute()
{
    OL_ASSERT(Lex.GetCurrent().Tk == TKS_lbracket);
    Lex.Next();

    AAttribute* Attrib = AstPool::New<AAttribute>(Lex.GetCurrent().SrcRange);
    if(Lex.GetCurrent().Tk == TKS_rbracket)
        return Attrib;

    while(true)
    {
        if(Lex.GetCurrent().Tk != TK_name)
        {
            CM.Log(CMT_ExpectAttribName, Lex.GetCurrent().SrcRange);
            AstPool::Delete(Attrib);
            return nullptr;
        }

        OLString Name = Lex.GetCurrent().StrOrNameVal;
        Lex.Next();

        if(Lex.GetCurrent().Tk != TKS_assign)
        {
            CM.Log(CMT_AttribSytexErr, Lex.GetCurrent().SrcRange);
            AstPool::Delete(Attrib);
            return nullptr;
        }

        Lex.Next();
        switch (Lex.GetCurrent().Tk)
        {
        case TK_stringVal:
            Attrib->AddString(Name, Lex.GetCurrent().StrOrNameVal);
            break;
        case TK_intVal:
            Attrib->AddInt(Name, Lex.GetCurrent().IntVal);
            break;
        case TK_floatVal:
            Attrib->AddFloat(Name, Lex.GetCurrent().FltVal);
            break;
        case TKK_true:
            Attrib->AddBool(Name, true);
            break;
        case TKK_false:
            Attrib->AddBool(Name, false);
            break;            
        case TKK_nil:
            Attrib->AddNil(Name);
            break;                                    
        default:
            {
                CM.Log(CMT_AttribValueErr, Lex.GetCurrent().SrcRange);
                AstPool::Delete(Attrib);
                return nullptr;
            }
            break;
        }

        Lex.Next();
        if(Lex.GetCurrent().Tk == TKS_rbracket)
        {
            Lex.Next();
            break;
        }
        else if(Lex.GetCurrent().Tk == TKS_comma)
        {
            Lex.Next();
            continue;
        }
        else
        {
            CM.Log(CMT_AttribSytexErr, Lex.GetCurrent().SrcRange);
            AstPool::Delete(Attrib);
            return nullptr;
        }

    }

    return Attrib;

}

//aliasstat -> 'alias' Name 'as' TypeIdentifier
AAlias* Parser::Parse_Alias()
{
    OL_ASSERT(Lex.GetCurrent().Tk == TKK_alias);
    SourceRange BegiLine = Lex.GetCurrent().SrcRange;
    Lex.Next();

    if(Lex.GetCurrent().Tk != TK_name)
    {
        CM.Log(CMT_NeedAliasName, Lex.GetCurrent().SrcRange);
        return nullptr;
    }

    AAlias* Ret = AstPool::New<AAlias>(BegiLine);
    Ret->Name = Lex.GetCurrent().StrOrNameVal;

    Lex.Next();
    if(Lex.GetCurrent().Tk != TKK_as)
    {
        CM.Log(CMT_AliasTypeNeeded, Lex.GetCurrent().SrcRange);
        AstPool::Delete(Ret);
        return nullptr;
    }

    Lex.Next();
    ATypeIdentity* TargetType = Parse_TypeIdentity(true);
    if(TargetType == nullptr)
    {
        AstPool::Delete(Ret);
        return nullptr;
    }

    Ret->TargetType = TargetType;
    return Ret;
}


// Parse a statement block of a function, loop, or 'if' stat
bool Parser::Helper_ParseBlock(OLList< SPtr<AStat> >& Block, bool AcceptGlobal)
{
    while(true)
    {
        ETokenType CurrTk = Lex.GetCurrent().Tk;
        // Ignore ';'
        while(CurrTk == TKS_semicolon)
        {
            CurrTk = Lex.Next().Tk;
            if(CurrTk == TK_eof)
                break;
        }

        // Tokens indicating the end of a code block
        if( CurrTk == TKK_end || CurrTk == TKK_else || CurrTk == TKK_elseif || CurrTk == TK_eof || CurrTk == TKK_until)
        {
            break;
        }



        AStat* NewStat = Parse_Stat(AcceptGlobal);
        if(NewStat == nullptr)
        {
            return false;
        }

        Block.Add(SPtr<AStat>(NewStat));
    }

    return true;
}


bool Parser::Helper_ParseVarDeclList(OLList < SPtr<AVarDecl> >& DeclList, bool AcceptVariableParam)
{
    bool VariableParamSeen = false;
    bool BeginOptionalParam = false;
    while(true)
    {
        if(Lex.GetCurrent().Tk == TKS_lbracket)
        {
            BeginOptionalParam = true;
            Lex.Next();
        }
        AVarDecl* Decl = Parse_VarDecl(AcceptVariableParam);
        if(Decl == nullptr)
        {
            return false;
        }
        if(Decl->IsVariableParam)
        {
            if(VariableParamSeen)
            {
                CM.Log(CMT_VariableParamOrder, Decl->SrcRange);
                AstPool::Delete(Decl);
                return false;
            }
            VariableParamSeen = true;
        }
        Decl->IsOptionalParam = BeginOptionalParam;

        DeclList.Add(Decl);

        ETokenType Tk = Lex.GetCurrent().Tk;

        if(Tk != TKS_comma)
        {
            break;
        }
        Lex.Next();
    }

    if(BeginOptionalParam)
    {
        if(Lex.GetCurrent().Tk != TKS_rbracket)
        {
            CM.Log(CMT_IncompleteBracket, Lex.GetCurrent().SrcRange);
            return false;
        }
        else
        {
            Lex.Next();
        }
    }

    return true;
}

bool Parser::Helper_ParseTypeList(OLList<SPtr<ATypeIdentity> >& TypeList)
{
    while(true)
    {
        ATypeIdentity* NewType = Parse_TypeIdentity(true);
        if(NewType == nullptr)
        {
            CM.Log(CMT_NeedTypeName, Lex.GetCurrent().SrcRange);
            return false;
        }
        TypeList.Add(NewType);

        if(Lex.GetCurrent().Tk != TKS_comma)
            break;
        else
            Lex.Next();
    }
    return true;
}


}