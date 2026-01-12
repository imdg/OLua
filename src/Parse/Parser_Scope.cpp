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
// Class -> class {Attrib} ClassTitle ClassBody
AClass* Parser::Parse_Class(bool IsExternal)
{
    OL_ASSERT(Lex.GetCurrent().Tk == TKK_class);
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

    AClass* Ret = AstPool::New<AClass>(Lex.GetCurrent().SrcRange);
    Ret->Attrib = Attrib;
    
    if(Helper_ParseClassTitle(Ret, IsExternal) == false)
    {
        AstPool::Delete(Ret);
        return nullptr;
    }

    if(Helper_ParseClassBody(Ret, IsExternal) == false)
    {
        AstPool::Delete(Ret);
        return nullptr;
    }
    Ret->IsExternal = IsExternal;
    

    return Ret;
}

// ClassTitle -> Name {extends Name {, Name} }
bool Parser::Helper_ParseClassTitle(AClass* Owner, bool IsExternal)
{
    if(Lex.GetCurrent().Tk != TK_name)
    {
        CM.Log(CMT_ClassNeedsName, Lex.GetCurrent().SrcRange);
        return false;
    }
    Owner->ClassName = Lex.GetCurrent().StrOrNameVal;
    Lex.Next();

    if(Lex.GetCurrent().Tk == TKK_extends)
    {
        Lex.Next();
        while(true)
        {
            Token& Curr = Lex.GetCurrent();
            if(Curr.Tk != TK_name)
            {
                CM.Log(CMT_NeedBaseName, Curr.SrcRange);
                return false;
            }
            ANamedType* BaseType = AstPool::New<ANamedType>(Curr.SrcRange);
            BaseType->TypeName = Curr.StrOrNameVal;
            Owner->BaseClasses.Add(BaseType);

            Lex.Next();
            if(Lex.GetCurrent().Tk != TKS_comma)
                break;
            Lex.Next();
        }
    }

    return true;
}

// ClassBody -> ClassMember { {;} ClassMember } end
bool Parser::Helper_ParseClassBody(AClass* Owner, bool IsExternal)
{
    while (true)
    {
        while(Lex.GetCurrent().Tk == TKS_semicolon)
            Lex.Next();

        if(Lex.GetCurrent().Tk == TKK_end)
        {
            Lex.Next();
            break;
        }

        AClassMember* Member = Parse_ClassMember(IsExternal);
        if(Member == nullptr)
            return false;
        
        Owner->ClassMember.Add(Member);
    }
    
    return true;
}

// Modifier -> SingleModifier { SingleModifier }
// SingleModifier -> public | protected | private | abstract | virtual | override | static | const
AModifier* Parser::Parse_Modifier() 
{
    AModifier* Ret = AstPool::New<AModifier>(Lex.GetCurrent().SrcRange);

    while(true)
    {
        bool IsValidModifier = true;
        switch(Lex.GetCurrent().Tk)
        {
            case TKK_public:    Ret->Access = AT_Public;    break;
            case TKK_protected:    Ret->Access = AT_Protected;    break;
            case TKK_private:    Ret->Access = AT_Private;    break;
            case TKK_abstract:  Ret->IsAbstract = true;     break;
            case TKK_virtual:   Ret->IsVirtual = true;     break;
            case TKK_override:  Ret->IsOverride = true;     break;
            case TKK_static:    Ret->IsStatic = true;     break;
            case TKK_const:     Ret->IsConst = true;     break;

            default:
                IsValidModifier = false;
                break;
        }

        if(IsValidModifier == false)
            break;
        else
            Lex.Next();
    }
    return Ret;
}

// ClassMember -> Modifier Method | ClassVar | ClassConstructor
AClassMember* Parser::Parse_ClassMember(bool IsExternal)
{
    AModifier* Modifier = Parse_Modifier();

    if(Lex.GetCurrent().Tk == TKK_function)
    {
        AMethod* Method = Parse_Method(Modifier->IsAbstract || IsExternal);
        if(Method == nullptr)
        {
            AstPool::Delete(Modifier);
            return nullptr;
        }
        Method->Modifier = Modifier;
        return Method;
    }
    else if(Lex.GetCurrent().Tk == TKK_local)
    {
        AClassVar* Var = Parse_ClassVar(IsExternal);
        if(Var == nullptr)
        {
            AstPool::Delete(Modifier);
            return nullptr;
        }
        Var->Modifier = Modifier;
        return Var;
    }
    else if(Lex.GetCurrent().Tk == TKK_constructor)
    {
        if(IsExternal)
        {
            CM.Log(CMT_CtorNotAllowOnExternal, Lex.GetCurrent().SrcRange);
        }
        AClassContructor* Constr = Parse_ClassConstructor();
        if(Constr == nullptr)
        {
            AstPool::Delete(Modifier);
            return nullptr;
        }
        
        Constr->Modifier = Modifier;
        return Constr;
    }

    CM.Log(CMT_MemberDeclWrong, Lex.GetCurrent().SrcRange);
    return nullptr;
}

// Method -> func { Attrib } Name  FuncBody 
AMethod* Parser::Parse_Method(bool IsAbstract)
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


    if(Lex.GetCurrent().Tk != TK_name)
    {
        CM.Log(CMT_MethodNeedsName, Lex.GetCurrent().SrcRange);
        if(Attrib)
            AstPool::Delete(Attrib);
        return nullptr;
    }
    OLString Name = Lex.GetCurrent().StrOrNameVal;

    Lex.Next();
    AFuncBody* Body = Parse_FuncBody(IsAbstract);
    if(Body == nullptr)
    {
        if(Attrib)
            AstPool::Delete(Attrib);
        return nullptr;
    }

    ANormalMethod* Ret = AstPool::New<ANormalMethod>(Lex.GetCurrent().SrcRange);
    Ret->Body = Body;
    Ret->Name = Name;
    Ret->Attrib = Attrib;
    return Ret;
}

// ClassConstructor -> ctor Name FuncBody
AClassContructor* Parser::Parse_ClassConstructor()
{
    OL_ASSERT(Lex.GetCurrent().Tk == TKK_constructor);
    Lex.Next();

    if(Lex.GetCurrent().Tk != TK_name)
    {
        CM.Log(CMT_MethodNeedsName, Lex.GetCurrent().SrcRange);
        return nullptr;
    }
    OLString Name = Lex.GetCurrent().StrOrNameVal;
    Lex.Next();

    AFuncBody* Body = Parse_FuncBody(false);
    if(Body == nullptr)
    {
        return nullptr;
    }

    AClassContructor* Ret = AstPool::New<AClassContructor>(Lex.GetCurrent().SrcRange);
    Ret->Body = Body;
    Ret->Name = Name;
    return Ret;
}

// ClassVar -> local VarDecl {, VarDecl} { =  Expr {, Expr} }
AClassVar* Parser::Parse_ClassVar(bool IsExternal)
{
    OL_ASSERT(Lex.GetCurrent().Tk == TKK_local);
    Lex.Next();

    // Parse the variant declearation
    AClassVar* Ret = AstPool::New<AClassVar>(Lex.GetCurrent().SrcRange);
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

// Enum -> enum EnumItem {, EnumItem} end
AEnum* Parser::Parse_Enum()
{
    OL_ASSERT(Lex.GetCurrent().Tk == TKK_enum);
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


    if(Lex.GetCurrent().Tk != TK_name)
    {
        CM.Log(CMT_EnumNeedsName, Lex.GetCurrent().SrcRange);
        AstPool::Delete(Attrib);
        return nullptr;
    }

    AEnum* Enum = AstPool::New<AEnum>(Lex.GetCurrent().SrcRange);
    Enum->Name = Lex.GetCurrent().StrOrNameVal;
    Enum->Attrib = Attrib;

    Lex.Next();

    while(Lex.GetCurrent().Tk != TKK_end)
    {
        AEnumItem* Item = Parse_EnumItem();
        if(Item == nullptr)
        {
            AstPool::Delete(Enum);
            return nullptr;
        }
        Enum->Items.Add(Item);

        if(Lex.GetCurrent().Tk == TKS_comma)
            Lex.Next();
        else
        {
            if(Lex.GetCurrent().Tk != TKK_end)
            {
                CM.Log(CMT_EnumItemSytexError, Lex.GetCurrent().SrcRange);
                AstPool::Delete(Enum);
                return nullptr;
            }
        }


    }

    Lex.Next();

    return Enum;
}

// EnumItem -> Name { = Expr(must be const)} 
AEnumItem* Parser::Parse_EnumItem()
{
    if(Lex.GetCurrent().Tk != TK_name)
    {
        CM.Log(CMT_ErrorInEnumItem, Lex.GetCurrent().SrcRange);
        return nullptr;
    }

    AEnumItem* EnumItem = AstPool::New<AEnumItem>(Lex.GetCurrent().SrcRange);
    EnumItem->Name = Lex.GetCurrent().StrOrNameVal;

    if(Lex.GetLookAhead().Tk == TKS_assign)
    {
        Lex.Next();
        Lex.Next(); 
        AExpr* ValueExpr = Parse_Expr();
        if(ValueExpr == nullptr)
        {
            AstPool::Delete(EnumItem);
            return nullptr;
        }
        EnumItem->ValueExpr = ValueExpr;

    }
    else
        Lex.Next();

    return EnumItem;
}

// Interface = interface {Attrib} InterfaceTitle InterfaceBody
AInterface* Parser::Parse_Interface()
{
    OL_ASSERT(Lex.GetCurrent().Tk == TKK_interface);
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


    AInterface* Ret = AstPool::New<AInterface>(Lex.GetCurrent().SrcRange);
    Ret->Attrib = Attrib;

    if(Helper_ParseInterfaceTitle(Ret) == false)
    {
        AstPool::Delete(Ret);
        return nullptr;
    }

    if(Helper_ParseInterfaceBody(Ret) == false)
    {
        AstPool::Delete(Ret);
        return nullptr;
    }

    return Ret;
}

// InterfaceTitle -> Name {extends Name {, Name}} 
bool Parser::Helper_ParseInterfaceTitle(AInterface* Owner)
{
    if(Lex.GetCurrent().Tk != TK_name)
    {
        CM.Log(CMT_InterfaceNeedsName, Lex.GetCurrent().SrcRange);
        return false;
    }
    Owner->InterfaceName = Lex.GetCurrent().StrOrNameVal;
    Lex.Next();

    if(Lex.GetCurrent().Tk == TKK_extends)
    {
        Lex.Next();
        while(true)
        {
            Token& Curr = Lex.GetCurrent();
            if(Curr.Tk != TK_name)
            {
                CM.Log(CMT_NeedBaseName, Curr.SrcRange);
                return false;
            }
            ANamedType* BaseType = AstPool::New<ANamedType>(Curr.SrcRange);
            BaseType->TypeName = Curr.StrOrNameVal;
            Owner->BaseInterfaces.Add(BaseType);

            Lex.Next();
            if(Lex.GetCurrent().Tk != TKS_comma)
                break;
            Lex.Next();
        }
    }

    return true;
}


// InterfaceBody -> {InterfaceMember {, InterfaceMember} } end
// InterfaceMember -> Modifier Method
bool Parser::Helper_ParseInterfaceBody(AInterface* Owner)
{
    while (true)
    {
        while(Lex.GetCurrent().Tk == TKS_semicolon)
            Lex.Next();

        if(Lex.GetCurrent().Tk == TKK_end)
        {
            Lex.Next();
            break;
        }

        AModifier *Modifier = Parse_Modifier();
        if (Lex.GetCurrent().Tk == TKK_function)
        {
            AMethod *Method = Parse_Method(true);
            if (Method == nullptr)
            {
                AstPool::Delete(Modifier);
                return false;
            }
            if(Method->Is<ANormalMethod>() == false)
            {
                CM.Log(CMT_InterfaceMemberError, Lex.GetCurrent().SrcRange);
                AstPool::Delete(Modifier);
                AstPool::Delete(Method);
                return false;
            }
            Method->Modifier = Modifier;
            Owner->InterfaceMembers.Add(Method->As<ANormalMethod>());
        }
        else
        {
            CM.Log(CMT_InterfaceMemberError, Lex.GetCurrent().SrcRange);
            return false;
        }
    }
    
    return true;
}


ARoot* Parser::Parse_Root(OLString Name)
{
    //OL_ASSERT(Lex.GetCurrent().Tk != TK_none );
    ARoot* Root = AstPool::New<ARoot>(Lex.Next().SrcRange);
    ABlock* Block = Parse_Block(true);
    if(Block == nullptr)
    {
        AstPool::Delete(Root);
        return nullptr;
    }
    Root->Block = Block;
    Root->Name = Name;
    return Root;
}

}