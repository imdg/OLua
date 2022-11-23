/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#pragma once
#include "Common.h"
#include "TokenReader.h"
//#include "CompileMsg.h"
#include "CompileCommon.h"
#include "OLLinkedList.h"

namespace OL
{
#define CH_DIGIT    (1)
#define CH_ABC      (1 << 1)
#define CH_NAME_START (1 << 2)
#define CH_NAME     (1 << 3)
#define CH_HEX_DIGIT (1 << 4)

RTTI_ENUM(ETokenType, 

    TK_intVal,
    TK_floatVal,
    TK_stringVal,
    TK_name,

    TKT_int32,
    TKT_int,
    TKT_int64,
    TKT_uint32,
    TKT_uint,
    TKT_uint64,
    TKT_byte,    
    TKT_char,
    TKT_any,
    TKT_float,
    TKT_double,
    TKT_bool,
    TKT_string,

    TKS_add,
    TKS_sub,
    TKS_mul,
    TKS_div,
    TKS_idiv,
    TKS_assign,
    TKS_eq,
    TKS_g,
    TKS_l,
    TKS_ge,
    TKS_le,
    TKS_ne,
    TKS_colon,
    TKS_colon2,
    TKS_semicolon,
    TKS_comma,
    TKS_dot,
    TKS_dot2,
    TKS_dot3,
    TKS_lshift,
    TKS_rshift,
    TKS_lparenthese,
    TKS_rparenthese,
    TKS_lbracket,
    TKS_rbracket,
    TKS_lbrace,
    TKS_rbrace,
    TKS_sharp,
    TKS_bnot,
    TKS_power,

    TKS_dollor,

    TKK_function,
    TKK_static,
    TKK_end,
    TKK_if,
    TKK_then,
    TKK_else,
    TKK_elseif,
    TKK_while,
    TKK_do,
    TKK_until,
    TKK_repeat,
    TKK_for,
    TKK_in,
    TKK_local,
    TKK_return,
    TKK_and,
    TKK_or,
    TKK_break,
    TKK_goto,
    TKK_nil,
    TKK_true,
    TKK_false,
    TKK_not,
    TKK_global,
    TKK_alias,

    TKK_class,
    TKK_interface,
    TKK_enum,
    TKK_extends,
    TKK_template,
    TKK_as,
    TKK_is,
    TKK_of,


    TKK_public,
    TKK_protected,
    TKK_private,
    TKK_const,
    TKK_abstract,
    TKK_virtual,
    TKK_override,
    TKK_constructor,
    TKK_self,
    TKK_super,
    TKK_extern,
    

    TK_eof,
    TK_eol,
    TK_none,
    TK_error

)



struct Token
{
    ETokenType Tk;
    int64 IntVal;
    double FltVal;
    OLString StrOrNameVal;
    CodeLineInfo LineInfo;

    void SetCurrLine(TokenReader& Reader);
    void SetCurrLine(int Line, int Col);
    void Reset();

    OLString ToString();
};

DECLEAR_STRUCT_RTTI(Token)


struct MacroDefine
{
    OLString Name;
    OLMap<OLString, OLString> Params;
    OLList<Token> TokenList;
};
DECLEAR_STRUCT_RTTI(MacroDefine)

struct MacroExpand
{
    OLString Name;
    OLList<Token> Params;
};
DECLEAR_STRUCT_RTTI(MacroDefine)

class CompileMsg;
class Lexer
{
public:
    Lexer(TokenReader& SrcReader);

    Lexer(TokenReader& SrcReader, CompileMsg& InCM);

    Token& Next();

    inline Token& GetCurrent()
    {
        return Curr;
    }
    Token& GetLookAhead();

    void AddBreakPoints(int Line);

    static void InitKeywordsTable();
    CompileMsg& CM;
private:
    void ReadNext(Token& tk);
    void ReadSingleToken(Token& tk, bool ForMacro, bool NoKeywords);

    bool IsDigit(TCHAR Ch);
    bool ReadString(Token& tk, const TCHAR Quoater);
    bool ReadNumber(Token& tk);
    bool ReadName(Token& tk, bool NoKeywords);
    int ReadHexChar();

    bool ReadPreprocesser();
    bool ReadDefine(MacroDefine& DefineInfo);
    bool ExpandMacro(OLString Name, CodeLineInfo Line, int Level);
    bool ReadMacroCallParam(OLLinkedList<Token>& TokenList, Token& Last, ETokenType EndMark1, ETokenType EndMark2, bool IncludeEndMark);
    bool CheckChFeature(TCHAR Ch, uint Feature);
    struct CondiCompileState
    {
        enum EState
        {
            Enable,
            Disable,
            ParentDisable,
            SiblingEnable
        };
        EState State;
    };
    OLList<CondiCompileState> CondiCompile;

    Token   Curr;
    Token   LookAhead;

    TokenReader& Reader;
    bool HasLookAhead;

    bool EnableForceEOF;

    static OLMap<OLString, ETokenType> KeywordsTable;
    static CompileMsg DefaultCM;

    OLList<int> BreakPoints;
    bool BreakOnNextToken;

    OLLinkedList<Token> FowardTokens;
    OLMap<OLString, MacroDefine> MacroDef;
};

}