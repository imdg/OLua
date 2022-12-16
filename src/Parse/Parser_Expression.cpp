/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "Parser.h"
#include "Lexer.h"
#include "ScopeInclude.h"
#include "ExpressionInclude.h"
#include "StatementInclude.h"
#include "AstPool.h"
#include "CompileMsg.h"


namespace OL
{

// Expr -> BinOpSubExpr
AExpr* Parser::Parse_Expr()
{
    return Parse_BinOpSubexpr( );
}

bool IsUniOp(ETokenType Tk)
{
    switch (Tk)
    {
    case TKS_sub:
    case TKS_bnot:
    case TKS_sharp:
    case TKK_not:
        return true;
    }
    return false;
}



// BinOpSubExpr -> UniOpSubExpr { binop BinOpSubExpr}
AExpr* Parser::Parse_BinOpSubexpr()
{

    // For convinience, merge to expr with an operator
    auto MergeSubexpr = [](AExpr *First, AExpr *Second, EBinOp Op, CodeLineInfo &LineInfo) -> AExpr *
    { 
        if (First->Is<ASubexpr>())
        {
            int SrcPrio = First->As<ASubexpr>()->GetBinOpPrio();
            if(SrcPrio != -1)  // -1 is a single uni op subexpr, do not merge
            {
                int CurrPrio = ASubexpr::GetBinOpInfo(Op).Priority;
                
                OL_ASSERT(CurrPrio <= SrcPrio );

                if(CurrPrio == SrcPrio)
                {
                    First->As<ASubexpr>()->AddOperand(Op, Second);
                    return First;
                }
            }
        }
        ASubexpr *Subexp = AstPool::New<ASubexpr>(LineInfo);
        Subexp->AddOperand(BO_None, First);
        Subexp->AddOperand(Op, Second);
        return Subexp;
    };

    EBinOp Op = BO_None;

    OLList<AExpr*> ExpStack;
    OLList<EBinOp> OpStack;

    AExpr* Exp = Parse_UniOpSubexpr();
    if(!Exp)
    {
        CM.Log(CMT_FailUniOpSubexpr, Lex.GetCurrent().LineInfo);
        return nullptr;
    }

    // to do: first exp allows a uni operator
    
    ExpStack.Add(Exp);

    while(true)
    {
        Op = ASubexpr::ConvertBinOp(Lex.GetCurrent().Tk);
        if(Op == BO_None)
            break;
        BinOpInfo& OpInfo = ASubexpr::GetBinOpInfo(Op);
        Lex.Next();

        if(OpStack.Count() == 0)
        {
            // first time run, only pushes the operator
            OpStack.Add(Op);
        }
        else
        {
            BinOpInfo& TopOpInfo = ASubexpr::GetBinOpInfo(OpStack.Top());
            
            // Higher priority op, pushes to the stack
            if(OpInfo.Priority > TopOpInfo.Priority)
            {
                OpStack.Add(Op);
            }
            else
            {
                // Shrink the exp stack by the op stack, until current op is higher than the top one
                while(OpStack.Count() > 0)
                {
                    EBinOp CurrTop = OpStack.Top();
                    BinOpInfo& CurrTopInfo = ASubexpr::GetBinOpInfo(CurrTop);

                    if(OpInfo.Priority > CurrTopInfo.Priority)
                        break;

                    AExpr *Top1 = ExpStack.Top();
                    ExpStack.Pop();

                    AExpr *Top2 = ExpStack.Top();
                    ExpStack.Pop();

                    AExpr *NewExp = MergeSubexpr(Top2, Top1, CurrTopInfo.Op, Top1->Line);
                    ExpStack.Add(NewExp);

                    OpStack.Pop();
                }

                OpStack.Add(Op);
            }
        }

        Exp = Parse_UniOpSubexpr();
        if(Exp == nullptr)
        {
            CM.Log(CMT_FailUniOpSubexpr, Lex.GetCurrent().LineInfo);
            return nullptr;
        }
        ExpStack.Add(Exp);

    }

    // finish remaining operators and exprs
    while (OpStack.Count() > 0)
    {
        EBinOp CurrTop = OpStack.Top();

        AExpr *Top1 = ExpStack.Top();
        ExpStack.Pop();

        AExpr *Top2 = ExpStack.Top();
        ExpStack.Pop();

        AExpr *NewExp = MergeSubexpr(Top2, Top1, CurrTop, Top1->Line);
        ExpStack.Add(NewExp);

        OpStack.Pop();
    }

    return ExpStack[0];
}


// UniOpSubExpr -> { uniop } SimpleExpr
AExpr* Parser::Parse_UniOpSubexpr()
{
    EUniOp Op = ASubexpr::ConvertUniOp(Lex.GetCurrent().Tk);

    if (Op == UO_None)
    {
        return Parse_SimpleExpr();
    }
    else
    {
        // Make a new subexpr whenever getting a new uniop
        CodeLineInfo Line = Lex.GetCurrent().LineInfo;
        Lex.Next();
        
        AExpr* Exp = Parse_UniOpSubexpr();
        if(Exp != nullptr)
        {
            ASubexpr* Subexpr = AstPool::New<ASubexpr>(Line);
            
            Subexpr->FirstUniOp = Op;
            Subexpr->AddOperand(BO_None, Exp);
            return Subexpr;
        }
        
    }

    return nullptr;
  
}


// SimpleExpr -> Constructor | FUNCTION funcbody | TypeExp
AExpr*   Parser::Parse_SimpleExpr()
{
    Token& Curr = Lex.GetCurrent();
    if(Curr.Tk == TKS_lbrace)
    {
        return Parse_Constructor();
    }
    else if(Curr.Tk == TKK_function)
    {
        return Parse_FuncExpr();
    }

     return Parse_TypeExp();
}

AExpr* Parser::Parse_FuncExpr()
{
    CodeLineInfo BeginLine = Lex.GetCurrent().LineInfo;

    OL_ASSERT(Lex.GetCurrent().Tk == TKK_function);
    Lex.Next();

    AFuncBody* Body = Parse_FuncBody(false);
    if(Body == nullptr)
    {
        CM.Log(CMT_FailFuncBody, BeginLine);
        return nullptr;
    }

    AFuncExpr* Ret = AstPool::New<AFuncExpr>(Lex.GetCurrent().LineInfo);
    Ret->Body = Body;

    return Ret;

}


// TypeExp -> Const | SuffixedExpr { as Type }
AExpr*     Parser::Parse_TypeExp()
{
    Token Curr = Lex.GetCurrent();
    AExpr* MainExpr = nullptr;
    if(Curr.Tk == TK_intVal 
        || Curr.Tk == TK_floatVal 
        || Curr.Tk == TK_stringVal 
        || Curr.Tk == TKK_true 
        || Curr.Tk == TKK_false 
        || Curr.Tk == TKK_nil 
         )
    {
        MainExpr = Parse_ConstExpr();
    }
    else
    {
        MainExpr = Parse_SuffixedExpr();
    }

    if(MainExpr == nullptr)
    {
        CM.Log(CMT_FailExpr, Curr.LineInfo);
        return nullptr;
    }

    
    Curr = Lex.GetCurrent();
    // if(Curr.Tk == TKS_exclamation)
    // {
    //     ANilableUnwrap* Unwrap = AstPool::New<ANilableUnwrap>(Curr.LineInfo);
    //     Unwrap->TargetExpr = MainExpr;
    //     MainExpr = Unwrap;
    //     Curr = Lex.Next();
    // }

    if(Curr.Tk == TKK_as)
    {
        Curr = Lex.Next();

        // Type cast keeps original nilable state, so it does not accept any other nilable mark
        ATypeIdentity* TargetType = Parse_TypeIdentity(false);
        if(TargetType == nullptr)
        {
            //CM.Log(CMT_ExpectTypeIdentity, Curr.LineInfo);
            AstPool::Delete(MainExpr);
            return nullptr;
        }

        ATypeCast* Ret = AstPool::New<ATypeCast>(Curr.LineInfo);
        //Ret->TypeName = Curr.StrOrNameVal;
        Ret->TargetType = TargetType;
        Ret->CastExpr = MainExpr;
        return Ret;
        
    }
    else
    {
        return MainExpr;
    }

    return nullptr;
}

// ConstExpr -> intVal | floatVal | stringVal | true | false | nil
AConstExpr*    Parser::Parse_ConstExpr()
{
    Token& Curr = Lex.GetCurrent();
    if( Curr.Tk == TK_intVal)
    {
        AConstExpr* Ret = AConstExpr::NewInt(Curr.IntVal, Curr.LineInfo);
        Lex.Next();
        return Ret;
    }
    else if( Curr.Tk == TK_floatVal)
    {
        AConstExpr* Ret = AConstExpr::NewFlt(Curr.FltVal, Curr.LineInfo);
        Lex.Next();
        return Ret;
    }
    else if( Curr.Tk == TK_stringVal)
    {
        AConstExpr* Ret = AConstExpr::NewStr(Curr.StrOrNameVal.CStr(), Curr.LineInfo);
        Lex.Next();
        return Ret;
    }
    else if( Curr.Tk == TKK_true)
    {
        AConstExpr* Ret = AConstExpr::NewBool(true, Curr.LineInfo);
        Lex.Next();
        return Ret;
    }
    else if( Curr.Tk == TKK_false)
    {
        AConstExpr* Ret = AConstExpr::NewBool(false, Curr.LineInfo);
        Lex.Next();
        return Ret;
    }
    else if( Curr.Tk == TKK_nil)
    {
        AConstExpr* Ret = AConstExpr::NewNil(Curr.LineInfo);
        Lex.Next();
        return Ret;
    }

    CM.Log(CMT_ExpectConst, Curr.LineInfo.Line, Curr.LineInfo.Col);
    return nullptr;
}




// SuffixedExpr -> PrimaryExpr { '.' Name | '[' expr ']' | ':' Name FuncArgs | FuncArgs}
AExpr* Parser::Parse_SuffixedExpr()
{
    CodeLineInfo BeginLine = Lex.GetCurrent().LineInfo;
    // First part
    AExpr* PrimaryExp = Parse_PrimaryExpr();
    if(PrimaryExp == nullptr)
    {
        CM.Log(CMT_FailPrimaryExpr, BeginLine);
        return nullptr;
    }

    AExpr* Ret = PrimaryExp;
    while (true)
    {
        Token Curr = Lex.GetCurrent();

        // Use '.' to dereference, like VarName.Member
        // Member can only be a name
        if(Curr.Tk == TKS_dot)
        {
            Curr = Lex.Next();
            if(Curr.Tk != TK_name)
            {
                CM.Log(CMT_ExpectDotName, Curr.LineInfo);
                AstPool::Delete(Ret);
                return nullptr;
            }
           
            ADotMember* NewNode = AstPool::New<ADotMember>(Curr.LineInfo);
            NewNode->Target = Ret;
            NewNode->Field = Curr.StrOrNameVal;
            Ret = NewNode;

            Lex.Next();
        }
        // Use '[  ]' to dereference like VarName["Member"]
        // The member can be any expressions
        else if(Curr.Tk == TKS_lbracket)
        {
            CodeLineInfo BracketBegin = Curr.LineInfo;

            Lex.Next();
            AExpr* BracketExpr = Parse_Expr();
            if(BracketExpr == nullptr)
            {
                AstPool::Delete(Ret);
                return nullptr;
            }

            // Validate the enclosing ']'
            if(Lex.GetCurrent().Tk != TKS_rbracket)
            {
                CM.Log(CMT_IncompleteBracket, Lex.GetCurrent().LineInfo, BracketBegin.Line, BracketBegin.Col);
                AstPool::Delete(Ret);
                return nullptr;
            }

            ABracketMember* NewNode = AstPool::New<ABracketMember>(Curr.LineInfo);
            NewNode->Target = Ret;
            NewNode->Field = BracketExpr;
            Ret = NewNode;

            Lex.Next();
        }
        else if(Curr.Tk == TKS_exclamation)
        {
            ANilableUnwrap* Unwrap = AstPool::New<ANilableUnwrap>(Curr.LineInfo);
            Unwrap->TargetExpr = Ret;
            Ret = Unwrap;
            Curr = Lex.Next();
        }

        // Member function call using ':' to dereference as VarName:Func(a, b, c)
        // The member can be a name used in a function call
        // Parse the whole function call
        else if(Lex.GetCurrent().Tk == TKS_colon)
        {
            // Validate name
            Curr = Lex.Next();
            if(Curr.Tk != TK_name)
            {
                 CM.Log(CMT_NameAfterColon, Curr.LineInfo);
                 AstPool::Delete(Ret);
                 return nullptr;
            }
            AColonCall* NewNode = AstPool::New<AColonCall>(Curr.LineInfo);
            NewNode->NameAfter = Curr.StrOrNameVal;
            // Validate function call
            // 1. '('
            Curr = Lex.Next();
            if(Curr.Tk != TKS_lparenthese)
            {
                 CM.Log(CMT_ColonForCallOnly, Curr.LineInfo);
                 AstPool::Delete(Ret);
                 AstPool::Delete(NewNode);
                 return nullptr;
            }
            CodeLineInfo ParentheseBegin = Curr.LineInfo;

            // 2. ')' or an expression list as arugments
            Curr = Lex.Next();
            
            if(Curr.Tk != TKS_rparenthese )
            {
                if (Helper_ParseExprList(NewNode->Params) == false)
                {
                    AstPool::Delete(Ret);
                    AstPool::Delete(NewNode);
                    CM.Log(CMT_FailParamList, ParentheseBegin);
                    return nullptr;
                }
            }
            // 3. ')' ( Validate again if ')' has been seen in step 2)
            Curr = Lex.GetCurrent();
            if(Curr.Tk != TKS_rparenthese)
            {
                 CM.Log(CMT_IncompleteParenthese, Curr.LineInfo, ParentheseBegin.Line, ParentheseBegin.Col);
                 AstPool::Delete(Ret);
                 AstPool::Delete(NewNode);
                 return nullptr;
            }
            Lex.Next();

            NewNode->Func = Ret;
            Ret = NewNode;

        }

        // Function call
        // Validate arguments
        else if(Lex.GetCurrent().Tk == TKS_lparenthese)
        {
            CodeLineInfo ParentheseBegin = Curr.LineInfo;
            

            ANormalCall* NewNode = AstPool::New<ANormalCall>(Curr.LineInfo);

            // Validate ')' or expression list as arguments
            Curr = Lex.Next();
            if(Curr.Tk != TKS_rparenthese && Helper_ParseExprList(NewNode->Params) == false)
            {
                CM.Log(CMT_FailParamList, ParentheseBegin);
                AstPool::Delete(Ret);
                AstPool::Delete(NewNode);
                return nullptr;
            }

            // Validate enclosing ')'
            Curr = Lex.GetCurrent();
            if(Curr.Tk != TKS_rparenthese)
            {
                 CM.Log(CMT_IncompleteParenthese, Curr.LineInfo, ParentheseBegin.Line, ParentheseBegin.Col);
                 AstPool::Delete(Ret);
                 AstPool::Delete(NewNode);
                 return nullptr;
            }
            Lex.Next();

            NewNode->Func = Ret;
            Ret = NewNode;
        }
        else
        {
            break;
        }


    }
    return Ret;
}

// PrimaryExpr -> Name | '(' Expr ')' | self | super | ...
AExpr*  Parser::Parse_PrimaryExpr()
{
    Token Curr = Lex.GetCurrent();
    // Variant name
    if(Curr.Tk == TK_name)
    {
        AVarRef* Ret = AstPool::New<AVarRef>(Curr.LineInfo);
        Ret->VarName = Curr.StrOrNameVal;
        Lex.Next();
        return Ret;
    }
    else if(Curr.Tk == TKK_self)
    {
        ASelf* Ret = AstPool::New<ASelf>(Curr.LineInfo);
        Lex.Next();
        return Ret;
    }
        else if(Curr.Tk == TKK_super)
    {
        ASuper* Ret = AstPool::New<ASuper>(Curr.LineInfo);
        Lex.Next();
        return Ret;
    }
    else if(Curr.Tk == TKS_dot3)
    {
        AVariableParamRef* Ret = AstPool::New<AVariableParamRef>(Curr.LineInfo);
        Lex.Next();
        return Ret;
    }

    // Using '(  )'
    else if(Curr.Tk == TKS_lparenthese)
    {
        CodeLineInfo ParentheseBegin = Curr.LineInfo;
        Lex.Next();
        AParentheses* ParentheseExpr = AstPool::New<AParentheses>(ParentheseBegin);

        AExpr *InsideExpr = Parse_Expr();
        if (InsideExpr == nullptr)
        {
            CM.Log(CMT_FailExprInParenthese, ParentheseBegin);
            return nullptr;
        }

        if (Lex.GetCurrent().Tk != TKS_rparenthese)
        {
            CM.Log(CMT_IncompleteParenthese, Lex.GetCurrent().LineInfo, ParentheseBegin.Line, ParentheseBegin.Col);
            AstPool::Delete(InsideExpr);
            return nullptr;
        }
        Lex.Next();

        ParentheseExpr->Expr = InsideExpr;
        return ParentheseExpr;
    }

    CM.Log(CMT_NotPrimaryExpr, Curr.LineInfo);
    return nullptr;
}

// Constructor ( for table ) -> '{' Expr [, Expr ...] '}' | '{' Name = Expr [, Name = Expr ...]  '}' | '{' '[' Expr ']' = Expr [, '[' Expr ']' = Expr] '}'
AConstructor*  Parser::Parse_Constructor()
{
    // Ensure current token is '{'
    if(Lex.GetCurrent().Tk != TKS_lbrace)
    {
        CM.Log(CMT_ExpectConstructor, Lex.GetCurrent().LineInfo);
        return nullptr;
    }

    AConstructor* Ret = AstPool::New<AConstructor>(Lex.GetCurrent().LineInfo);
    
    bool HasKey = false;
    
    // Empty constructor
    if(Lex.Next().Tk == TKS_rbrace)
    {
        Ret->HasKey = false;
        Lex.Next();
        return Ret;
    }

    while(true)
    {
        Token Curr = Lex.GetCurrent();
        AExpr* Key = nullptr;

        // { Name = Value } Style, check ahead '=' to confirm
        if(Curr.Tk == TK_name)
        {
            Token& Ahead = Lex.GetLookAhead();
            if(Ahead.Tk == TKS_assign)
            {
                HasKey = true;
                Key = AConstExpr::NewStr(Curr.StrOrNameVal.CStr(), Curr.LineInfo);
                Lex.Next();
                Lex.Next();
            }
        }

        // { [something] = Value } Style, definitely a key-value style
        else if(Curr.Tk == TKS_lbracket)
        {
            CodeLineInfo BeginLine = Lex.GetCurrent().LineInfo;

            Lex.Next();
            Key = Parse_Expr();
            if(Key == nullptr)
            {
                CM.Log(CMT_FailExprInBracket, BeginLine);
                AstPool::Delete(Ret);
                return nullptr;
            }

            if(Lex.GetCurrent().Tk != TKS_rbracket)
            {
                CM.Log(CMT_IncompleteBracket, Lex.GetCurrent().LineInfo, BeginLine.Line, BeginLine.Col);
                AstPool::Delete(Key);
                AstPool::Delete(Ret);
                return nullptr;
            }
            Lex.Next();

            HasKey = true; 
        }

        // Ensure the '=' after key is confirmed (has already comfirmed in the first style, nothing bad to confirm again)
        if(Key != nullptr)
        {
            if(Lex.GetCurrent().Tk != TKS_assign)
            {
                CM.Log(CMT_ConstructorNeedValue, Lex.GetCurrent().LineInfo);
                AstPool::Delete(Key);
                AstPool::Delete(Ret);
                return nullptr;
            }
            Lex.Next();
        }
        
        // Parse value expression
        CodeLineInfo ExprBegin = Lex.GetCurrent().LineInfo;
        AExpr* Value = Parse_Expr();
        if(Value == nullptr)
        {
            AstPool::Delete(Key);
            AstPool::Delete(Ret);
            CM.Log(CMT_FailExpr, ExprBegin);
            return nullptr;
        }
        Ret->AddElement(Key, Value);

        //Finish one element, ',' to continue the next one, '}' to finish the constructor
        ETokenType Tk = Lex.GetCurrent().Tk;
        if(Tk == TKS_rbrace)
        {
            Lex.Next();
            Ret->HasKey = HasKey;
            if(HasKey)
            {
                for(int i = 0; i < Ret->Elems.Count(); i++ )
                {
                    if(Ret->Elems[i].Key == nullptr)
                    {
                        CM.Log(CMT_PartialKeys, Lex.GetCurrent().LineInfo);
                        AstPool::Delete(Ret);
                        return nullptr;
                    }
                }
            }
            return Ret;
        }
        else if(Tk == TKS_comma)
        {
            Lex.Next();
        }
        else
        {
            CM.Log(CMT_ConstructorMissComma, Lex.GetCurrent().LineInfo);
            AstPool::Delete(Ret);
            return nullptr;
        }


    }
    return nullptr;
}

// Type -> (intrinsics) | Name | FuncType | '(' Type ')' | MapType  { [] }
ATypeIdentity*  Parser::Parse_TypeIdentity(bool AcceptNilable)
{
    Token Curr = Lex.GetCurrent();
    ATypeIdentity* TypeIdn = nullptr;
    bool NeedToParseNilable = true;
    
    bool HasParenthess = false;
    CodeLineInfo ParenthessBegin;

    if(Curr.Tk == TKS_lparenthese)
    {
        ParenthessBegin = Curr.LineInfo;
        Curr = Lex.Next();
        HasParenthess = true;
    }

    switch(Curr.Tk)
    {
        case TKT_int32:
        case TKT_int:
        case TKT_int64:
        case TKT_uint32:
        case TKT_uint:
        case TKT_uint64:
        case TKT_byte:
        case TKT_char:
        {
            AIntrinsicType* Intr = AstPool::New<AIntrinsicType>(Curr.LineInfo);
            Intr->Type = IT_int;
            Lex.Next();
            TypeIdn =  Intr;
            break;
        }
        case TKT_float:
        case TKT_double:
        {
            AIntrinsicType* Intr = AstPool::New<AIntrinsicType>(Curr.LineInfo);
            Intr->Type = IT_float;
            Lex.Next();
            TypeIdn =  Intr;
            break;
        }
        case TKT_bool:
        {
            AIntrinsicType* Intr = AstPool::New<AIntrinsicType>(Curr.LineInfo);
            Intr->Type = IT_bool;
            Lex.Next();
            TypeIdn =  Intr;
            break;
        }
        case TKT_string:
        {
            AIntrinsicType* Intr = AstPool::New<AIntrinsicType>(Curr.LineInfo);
            Intr->Type = IT_string;
            Lex.Next();
            TypeIdn =  Intr;
            break;
        }
        case TKT_any:
        {
            AIntrinsicType* Intr = AstPool::New<AIntrinsicType>(Curr.LineInfo);
            Intr->Type = IT_any;
            Lex.Next();
            TypeIdn =  Intr;
            break;
        }    
        case TK_name:
        {
            ANamedType* Named = AstPool::New<ANamedType>(Curr.LineInfo);
            Named->TypeName = Curr.StrOrNameVal;
            Lex.Next();
            TypeIdn = Named;
            break;
        }
        case TKK_function:
        {
            TypeIdn = Parse_FuncType(AcceptNilable);
            NeedToParseNilable = false; // Nilable parse is done in Parse_FuncType

            break;
        }
        case TKS_lbracket:
        {
            TypeIdn = Parse_MapType();
            break;
        }
        default:
        {
            CM.Log(CMT_ExpectTypeIdentity, Curr.LineInfo);
            return nullptr;
        }
    }
    
    if(Lex.GetCurrent().Tk == TKS_lbracket)
    {
        CodeLineInfo BracketBegin = Lex.GetCurrent().LineInfo;
        Lex.Next();
        
        if(Lex.GetCurrent().Tk == TKS_rbracket)
        {
            AArrayType* Arr = AstPool::New<AArrayType>(Lex.GetCurrent().LineInfo);
            Arr->ElemType = TypeIdn;
            TypeIdn = Arr;
            Lex.Next();
        }
        else
        {
            CM.Log(CMT_IncompleteBracket, Lex.GetCurrent().LineInfo, BracketBegin.Line, BracketBegin.Col);
            AstPool::Delete(TypeIdn);
            return nullptr;
        }
    }

    if(HasParenthess)
    {
        if(Lex.GetCurrent().Tk != TKS_rparenthese)
        {
            CM.Log(CMT_IncompleteParenthese, Lex.GetCurrent().LineInfo, ParenthessBegin.Line, ParenthessBegin.Col);
            AstPool::Delete(TypeIdn);
            return nullptr;
        }
        Lex.Next();
    }

    
    if(NeedToParseNilable)
    {
        if(Lex.GetCurrent().Tk == TKS_question)
        {
            // Type alias, type cast does not accept nilable state
            if(AcceptNilable == false)
            {
                CM.Log(CMT_NotAcceptedNilable, Lex.GetCurrent().LineInfo);
                TypeIdn->IsNilable = false;
            }
            else
            {
                TypeIdn->IsNilable = true;
            }
            Lex.Next();
        }
    }
    return TypeIdn;
}


// MapType -> [Type, Type]
AMapType* Parser::Parse_MapType()
{
    OL_ASSERT(Lex.GetCurrent().Tk == TKS_lbracket);

    CodeLineInfo BeginLine = Lex.GetCurrent().LineInfo;

    Lex.Next();

    // Map key should not be defined as nilable
    ATypeIdentity* First = Parse_TypeIdentity(false);
    if(First == nullptr)
    {
        return nullptr;
    }
    ATypeIdentity* Second = nullptr;

    if(Lex.GetCurrent().Tk == TKS_comma)
    {
        Lex.Next();
        Second = Parse_TypeIdentity(true);
        if(Second == nullptr)
        {
            AstPool::Delete(First);
            return nullptr;
        }
    }

    if(Lex.GetCurrent().Tk != TKS_rbracket)
    {
        AstPool::Delete(First);
        if(Second != nullptr)
            AstPool::Delete(Second);
        CM.Log(CMT_IncompleteBracket, Lex.GetCurrent().LineInfo, BeginLine.Line, BeginLine.Col);
        return nullptr;
    }
    Lex.Next();
    AMapType* Ret = AstPool::New<AMapType>(BeginLine);
    if(Second != nullptr)
    {
        Ret->KeyType = First;
        Ret->ValueType = Second;
    }
    else
    {
        Ret->ValueType = First;
    }

    return Ret;
}

bool Parser::Helper_ParseExprList(OLList<SPtr<AExpr>>& OutList)
{
    Token Curr = Lex.GetCurrent();
    bool First = true;
    while(true)
    {
        AExpr* Expr = Parse_Expr();
        if(Expr == nullptr)
            return false;
        
        OutList.Add(Expr);

        if(Lex.GetCurrent().Tk != TKS_comma)
            break;
        Lex.Next();
    }
    return true;
}

// bool Parser::Helper_ParseSuffixedExprList(OLList<SPtr<ASuffixedExpr>>& OutList)
// {
//     Token Curr = Lex.GetCurrent();

//     while(true)
//     {
//         AExpr* Expr = Parse_Expr();
//         if(Expr == nullptr)
//             return false;
        
//         OutList.Add(Expr);

//         if(Lex.GetCurrent().Tk != TKS_comma)
//             break;
//         Lex.Next();
//     }
//     return true;
// }



}