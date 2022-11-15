/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "Lexer.h"
#include "Common.h"
#include "Logger.h"
#include "CompileMsg.h"

namespace OL
{




STRUCT_RTTI_BEGIN(Token)
    RTTI_MEMBER(Tk)
    RTTI_MEMBER(IntVal)
    RTTI_MEMBER(FltVal)
    RTTI_MEMBER(StrOrNameVal)
    RTTI_STRUCT_MEMBER(LineInfo, CodeLineInfo)
STRUCT_RTTI_END(Token)


int CharFeature[] = {
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x19, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 
0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0x0, 0x0, 0x0, 0x0, 0xc, 
0x0, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 
0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0xe, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 
0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0


};



void Token::SetCurrLine(TokenReader& Reader)
{
    LineInfo.Col = Reader.GetCol();
    LineInfo.Line = Reader.GetLine();
}

void Token::SetCurrLine(int Line, int Col)
{
    LineInfo.Line = Line;
    LineInfo.Col = Col;
}

void Token::Reset()
{
    Tk = TK_none;
    IntVal = 0;
    FltVal = 0;
    StrOrNameVal = "";
    LineInfo.Line = 0;
    LineInfo.Col = 0;
}

OLString Token::ToString()
{
    OLString Ret;
    Ret.AppendF(T("[%d:%d]"), LineInfo.Line, LineInfo.Col);
    Ret.Append( ENUM_VALUE_TEXT(ETokenType, Tk));
    if(Tk == TK_intVal)
        Ret.AppendF(T("(%d)"), IntVal);
    else if(Tk == TK_floatVal)
        Ret.AppendF(T("(%f)"), FltVal);
    else if(Tk == TK_name || Tk == TK_stringVal)
        Ret.AppendF(T("(\"%s\")"), StrOrNameVal.CStr());
    
    return Ret;
}


CompileMsg Lexer::DefaultCM;

OLMap<OLString, ETokenType> Lexer::KeywordsTable;
void Lexer::InitKeywordsTable()
{
    KeywordsTable.Add(T("function"), TKK_function);
    KeywordsTable.Add(T("func"), TKK_function);
    KeywordsTable.Add(T("static"), TKK_static);
    KeywordsTable.Add(T("end"), TKK_end);
    KeywordsTable.Add(T("if"), TKK_if);
    KeywordsTable.Add(T("then"), TKK_then);
    KeywordsTable.Add(T("else"), TKK_else);
    KeywordsTable.Add(T("elseif"), TKK_elseif);
    KeywordsTable.Add(T("while"), TKK_while);
    KeywordsTable.Add(T("do"), TKK_do);
    KeywordsTable.Add(T("until"), TKK_until);
    KeywordsTable.Add(T("repeat"), TKK_repeat);
    KeywordsTable.Add(T("local"), TKK_local);
    KeywordsTable.Add(T("and"), TKK_and);
    KeywordsTable.Add(T("or"), TKK_or);
    KeywordsTable.Add(T("not"), TKK_not);
    KeywordsTable.Add(T("break"), TKK_break);
    KeywordsTable.Add(T("goto"), TKK_goto);
    KeywordsTable.Add(T("in"), TKK_in);
    KeywordsTable.Add(T("nil"), TKK_nil);
    KeywordsTable.Add(T("true"), TKK_true);
    KeywordsTable.Add(T("false"), TKK_false);
    KeywordsTable.Add(T("for"), TKK_for);
    KeywordsTable.Add(T("return"), TKK_return);

    KeywordsTable.Add(T("int32"), TKT_int32);
    KeywordsTable.Add(T("int"), TKT_int);
    KeywordsTable.Add(T("int64"), TKT_int64);
    KeywordsTable.Add(T("uint32"), TKT_uint32);
    KeywordsTable.Add(T("uint"), TKT_uint);
    KeywordsTable.Add(T("uint64"), TKT_uint64);
    KeywordsTable.Add(T("byte"), TKT_byte);
    KeywordsTable.Add(T("char"), TKT_char);
    KeywordsTable.Add(T("any"), TKT_any);
    KeywordsTable.Add(T("float"), TKT_float);
    KeywordsTable.Add(T("double"), TKT_double);
    KeywordsTable.Add(T("bool"), TKT_bool);
    KeywordsTable.Add(T("string"), TKT_string);

    KeywordsTable.Add(T("global"), TKK_global);

    KeywordsTable.Add(T("class"), TKK_class);
    KeywordsTable.Add(T("interface"), TKK_interface);
    KeywordsTable.Add(T("enum"), TKK_enum);

    KeywordsTable.Add(T("extends"), TKK_extends);
    KeywordsTable.Add(T("template"), TKK_template);
    KeywordsTable.Add(T("as"), TKK_as);
    KeywordsTable.Add(T("is"), TKK_is);
    KeywordsTable.Add(T("of"), TKK_of);

    KeywordsTable.Add(T("public"), TKK_public);
    KeywordsTable.Add(T("protected"), TKK_protected);
    KeywordsTable.Add(T("private"), TKK_private);
    KeywordsTable.Add(T("const"), TKK_const);
    KeywordsTable.Add(T("abstract"), TKK_abstract);
    KeywordsTable.Add(T("virtual"), TKK_virtual);
    KeywordsTable.Add(T("override"), TKK_override);
    KeywordsTable.Add(T("constructor"), TKK_constructor);
    KeywordsTable.Add(T("ctor"), TKK_constructor);

    KeywordsTable.Add(T("extern"), TKK_extern);

    KeywordsTable.Add(T("self"), TKK_self);
    KeywordsTable.Add(T("super"), TKK_super);

    // Only used for test
    KeywordsTable.Add(T("eof"), TK_eof);

}

Lexer::Lexer(TokenReader& SrcReader): Reader(SrcReader), HasLookAhead(false), CM(DefaultCM)
{
    CM.SetFile(SrcReader.GetSourceFileName().CStr());
    EnableForceEOF = false;
    BreakOnNextToken = false;
    CondiCompile.Add(CondiCompileState{CondiCompileState::Enable});
}

Lexer::Lexer(TokenReader& SrcReader, CompileMsg& InCM)
    : Reader(SrcReader), HasLookAhead(false), CM(InCM)
{
    EnableForceEOF = false;
    BreakOnNextToken = false;
    CondiCompile.Add(CondiCompileState{CondiCompileState::Enable});
}

Token& Lexer::Next()
{
    if(HasLookAhead)
    {
        Curr = LookAhead;
        HasLookAhead = false;
    }
    else
    {
        ReadNext(Curr);
    }

    for(int i = 0; i < BreakPoints.Count(); i++)
    {
        if(Curr.LineInfo.Line == BreakPoints[i])
        {
            // Break point hit
            // Set the debugger break point here
            VERBOSE(LogMisc, T("Break point hit\n"));
            break;
        }
    }

    return Curr;
}

// Token& Lexer::GetCurrent()
// {
//     return Curr;
// }

Token& Lexer::GetLookAhead()
{
    if(HasLookAhead)
    {
        return LookAhead;
    }
    else
    {
        ReadNext(LookAhead);
        HasLookAhead = true;
        return LookAhead;
    }
}

void Lexer::ReadNext(Token& tk)
{
    bool KeepReading = false;
    do
    {
        KeepReading = false;
        if(FowardTokens.Count() > 0)
            tk = FowardTokens.PopHead();
        else
            ReadSingleToken(tk, false, false);

        if(tk.Tk == TK_none)
        {
            KeepReading = true;
            continue;
        }

        if(tk.Tk == TKS_dollor)
        {
            ReadPreprocesser();
            KeepReading = true;
            continue;
        }

        if(CondiCompile.Top().State != CondiCompileState::Enable)
        {
            KeepReading = true;
            continue;
        }

        if(tk.Tk == TK_name && MacroDef.Exists(tk.StrOrNameVal))
        {
            ExpandMacro(tk.StrOrNameVal, tk.LineInfo, 0);
            KeepReading = true;
            continue;;
        }

    } while (KeepReading);




    if(BreakOnNextToken)
    {
        BreakOnNextToken = false;
        tk.LineInfo.Flag = LF_Break;
    }
    else
    {
        tk.LineInfo.Flag = 0;
    }
    
}

bool Lexer::ReadDefine(MacroDefine& DefineInfo)
{
    Token MacroTk;
    CodeLineInfo BeginLine;
    ReadSingleToken(MacroTk, true, false);
    BeginLine = MacroTk.LineInfo;

    if(MacroTk.Tk != TK_name)
    {
        CM.Log(CMT_NeedMacroName, MacroTk.LineInfo);
        return false;
    }

    DefineInfo.Name = MacroTk.StrOrNameVal;
    int SpaceCount = Reader.SkipSpace(false);
    if(Reader.CurrChar() == C('\n'))
        return true;
        
    ReadSingleToken(MacroTk, true, false);

    if(MacroTk.Tk == TK_eol)
        return true;
    
    // Params
    if(SpaceCount == 0 && MacroTk.Tk == TKS_lparenthese)
    {
        do
        {
            ReadSingleToken(MacroTk, true, false);
            if(MacroTk.Tk == TK_eol || MacroTk.Tk == TK_eof)
            {
                CM.Log(CMT_IncompleteParenthese, BeginLine, BeginLine.Line, BeginLine.Col);
                return false;
            }
            else if(MacroTk.Tk != TK_name)
            {
                CM.Log(CMT_MacroParamNeedName, MacroTk.LineInfo);
                return false;
            }

            DefineInfo.Params.Add(MacroTk.StrOrNameVal, MacroTk.StrOrNameVal);

            ReadSingleToken(MacroTk, true, false);
            if(MacroTk.Tk == TKS_comma)
                continue;
            else if(MacroTk.Tk == TKS_rparenthese)
                break;
            else
            {
                CM.Log(CMT_MacroSytexError, BeginLine);
                return false;
            }
        }
        while(true);
    }
    else
    {
        DefineInfo.TokenList.Add(MacroTk);
    }

    // Define body
    while(true)
    {
        ReadSingleToken(MacroTk, true, false);
        if(MacroTk.Tk != TK_eof && MacroTk.Tk != TK_eol)
            DefineInfo.TokenList.Add(MacroTk);
        else
            break;
    }

    return true;

}

bool Lexer::ReadPreprocesser()
{
    Token PreprocesserTk;
    ReadSingleToken(PreprocesserTk, true, true);
    if(PreprocesserTk.Tk != TK_name)
    {
        CM.Log(CMT_NeedMacroOpt, PreprocesserTk.LineInfo);
        return false;
    }

    if(PreprocesserTk.StrOrNameVal == T("define"))
    {
        if(CondiCompile.Top().State != CondiCompileState::Enable)
            return true;

        MacroDefine NewDefine;
        
        if(false == ReadDefine(NewDefine))
        {
            return false;
        }

        MacroDef.Add(NewDefine.Name, NewDefine);
    }
    else if(PreprocesserTk.StrOrNameVal == T("ifdef"))
    {
        Token Macro;
        ReadSingleToken(Macro, true, false);
        if(Macro.Tk != TK_name)
        {
            CM.Log(CM_NeedMacroNameForIf, Macro.LineInfo);
            return false;
        }

        if(MacroDef.Exists(Macro.StrOrNameVal))
        {
            if(CondiCompile.Top().State != CondiCompileState::Enable)
                CondiCompile.Add(CondiCompileState{CondiCompileState::ParentDisable});
            else    
                CondiCompile.Add(CondiCompileState{CondiCompileState::Enable});
        }
        else
        {
            if(CondiCompile.Top().State != CondiCompileState::Enable)
                CondiCompile.Add(CondiCompileState{CondiCompileState::ParentDisable});
            else    
                CondiCompile.Add(CondiCompileState{CondiCompileState::Disable});
        }
    }
    else if(PreprocesserTk.StrOrNameVal == T("ifndef"))
    {
        Token Macro;
        ReadSingleToken(Macro, true, false);
        if(Macro.Tk != TK_name)
        {
            CM.Log(CM_NeedMacroNameForIf, Macro.LineInfo);
            return false;
        }

        if(MacroDef.Exists(Macro.StrOrNameVal) == false)
        {
            if(CondiCompile.Top().State != CondiCompileState::Enable)
                CondiCompile.Add(CondiCompileState{CondiCompileState::ParentDisable});
            else    
                CondiCompile.Add(CondiCompileState{CondiCompileState::Enable});
        }
        else
        {
            if(CondiCompile.Top().State != CondiCompileState::Enable)
                CondiCompile.Add(CondiCompileState{CondiCompileState::ParentDisable});
            else    
                CondiCompile.Add(CondiCompileState{CondiCompileState::Disable});
        }
    }
    else if(PreprocesserTk.StrOrNameVal == T("elifdef"))
    {
        Token Macro;
        ReadSingleToken(Macro, true, false);
        if(Macro.Tk != TK_name)
        {
            CM.Log(CM_NeedMacroNameForIf, Macro.LineInfo);
            return false;
        }
        if(CondiCompile.Count() == 1)
        {
            CM.Log(CMT_CondiCompileNotMatch, PreprocesserTk.LineInfo);
            return false;
        }
        CondiCompileState Sibling = CondiCompile.PickPop();
        if(CondiCompile.Top().State != CondiCompileState::Enable)
            CondiCompile.Add(CondiCompileState{CondiCompileState::ParentDisable});
        else
        {
            if(Sibling.State == CondiCompileState::Enable)
                CondiCompile.Add(CondiCompileState{CondiCompileState::SiblingEnable});
            else
            {
                if(MacroDef.Exists(Macro.StrOrNameVal))
                    CondiCompile.Add(CondiCompileState{CondiCompileState::Enable});
                else
                    CondiCompile.Add(CondiCompileState{CondiCompileState::Disable});
            }
        }
    }
    else if(PreprocesserTk.StrOrNameVal == T("elifndef"))
    {
        Token Macro;
        ReadSingleToken(Macro, true, false);
        if(Macro.Tk != TK_name)
        {
            CM.Log(CM_NeedMacroNameForIf, Macro.LineInfo);
            return false;
        }
        if(CondiCompile.Count() == 1)
        {
            CM.Log(CMT_CondiCompileNotMatch, PreprocesserTk.LineInfo);
            return false;
        }

        CondiCompileState Sibling = CondiCompile.PickPop();
        if(CondiCompile.Top().State != CondiCompileState::Enable)
            CondiCompile.Add(CondiCompileState{CondiCompileState::ParentDisable});
        else
        {
            if(Sibling.State == CondiCompileState::Enable)
                CondiCompile.Add(CondiCompileState{CondiCompileState::SiblingEnable});
            else
            {
                if(MacroDef.Exists(Macro.StrOrNameVal) == false)
                    CondiCompile.Add(CondiCompileState{CondiCompileState::Enable});
                else
                    CondiCompile.Add(CondiCompileState{CondiCompileState::Disable});
            }
        }
    }
    else if(PreprocesserTk.StrOrNameVal == T("else"))
    {
        if(CondiCompile.Count() == 1)
        {
            CM.Log(CMT_CondiCompileNotMatch, PreprocesserTk.LineInfo);
            return false;
        }

        CondiCompileState Sibling = CondiCompile.PickPop();
        if(CondiCompile.Top().State != CondiCompileState::Enable)
            CondiCompile.Add(CondiCompileState{CondiCompileState::ParentDisable});
        else
        {
            if(Sibling.State == CondiCompileState::Enable || Sibling.State == CondiCompileState::SiblingEnable )
                CondiCompile.Add(CondiCompileState{CondiCompileState::Disable});
            else
            {
                CondiCompile.Add(CondiCompileState{CondiCompileState::Enable});
            }
        }
    }
    else if(PreprocesserTk.StrOrNameVal == T("endif"))
    {
        if(CondiCompile.Count() == 1)
        {
            CM.Log(CMT_CondiCompileNotMatch, PreprocesserTk.LineInfo);
            return false;
        }
        CondiCompile.Pop();
    }
    return true;
}

bool Lexer::ReadMacroCallParam(OLLinkedList<Token>& TokenList,  Token& Last, ETokenType EndMark1, ETokenType EndMark2, bool IncludeEndMark)
{
    Token CurrTk;
    
    do
    {
        ReadSingleToken(CurrTk, false, false);
        

        if(CurrTk.Tk == TKS_lparenthese)
        {
            Token Dummy;
            if(false == ReadMacroCallParam(TokenList, Dummy, TKS_rparenthese, TK_none, true))
                return false;
        }

        if(CurrTk.Tk == EndMark1 || CurrTk.Tk == EndMark2)
        {
            Last = CurrTk;
            if(IncludeEndMark)
                TokenList.AddTail(CurrTk);
            return true;
        }
        
        if(CurrTk.Tk == TK_eof)
            return false;

        TokenList.AddTail(CurrTk);

    } while (true);
    
    return false;
}


bool Lexer::ExpandMacro(OLString Name, CodeLineInfo Line, int Level)
{
    if(Level >= 100)
    {
        CM.Log(CMT_MacroTooManyLevels, Line);
        return false;
    }
    MacroDefine& Def = MacroDef.Find(Name); // Existance is ensured

    OLMap<OLString, OLLinkedList<Token> > ReplaceArgs;

    if(Def.Params.Count() > 0)
    {
        Token Tmp;
        ReadSingleToken(Tmp, false, false);
        if(Tmp.Tk != TKS_lparenthese)
        {
            CM.Log(CMT_MacroExpandError, Line);
            return false;
        }

        auto It = Def.Params.BeginIter();
        while(It.IsEnd() == false)
        {
            ReplaceArgs.Add(It.GetValue(), OLLinkedList<Token>());
            OLLinkedList<Token>& CurrArg = ReplaceArgs.Find(It.GetValue());
            Token EndToken;
  
            if(false == ReadMacroCallParam(CurrArg, EndToken, TKS_comma, TKS_rparenthese, false ))
            {
                CM.Log(CMT_MacroExpandError, Line);
                return false;
            };

            if(EndToken.Tk == TKS_rparenthese)
                break;
            It.Next();
        }

        if(Def.Params.Count() != ReplaceArgs.Count())
        {
            return false;
        }

    }

    for(int i = 0; i < Def.TokenList.Count(); i++)
    {
        Token& Curr = Def.TokenList[i];
        bool IsParam = false;
        if(Curr.Tk == TK_name)
        {
            if(Def.Params.Exists(Curr.StrOrNameVal) )
            {
                IsParam = true;
            }
        }



        if(IsParam)
        {
            if(ReplaceArgs.Exists(Curr.StrOrNameVal))
            {
                OLLinkedList<Token>& Args = ReplaceArgs.Find(Curr.StrOrNameVal);
                auto It = Args.BeginIterator();
                while(It.IsValid())
                {
                    if(MacroDef.Exists(It.Value().StrOrNameVal))
                    {
                        if(false == ExpandMacro(It.Value().StrOrNameVal, Line, Level + 1))
                            return false;
                    }
                    else
                        FowardTokens.AddTail(It.Value());
                    It.Next();
                }
            }
            
        }
        else if(MacroDef.Exists(Curr.StrOrNameVal))
        {
            if(false == ExpandMacro(Curr.StrOrNameVal, Line,  Level + 1))
                return false;
        }
        else
        {
            FowardTokens.AddTail(Curr);
        }
    }

    return true;
}


void Lexer::ReadSingleToken(Token& tk, bool ForMacro, bool NoKeywords)
{
    tk.Reset();
    
    Reader.SkipSpace(ForMacro ? false : true);
    int Ch = Reader.CurrChar();
    tk.SetCurrLine(Reader);


    if(Ch == TR_EOF)
    {
        tk.Tk = TK_eof;
        return;
    }

    if(Ch == C('\n'))
    {
        tk.Tk = TK_eol;
        return;
    }

    switch (Ch)
    {
    case C('+'):     
        Reader.NextChar();
        tk.Tk = TKS_add;
        break;
    case C('-'):
        {
            Reader.NextChar();
            if(Reader.NextCharIfCurr(C('-')))
            {
                bool BlockComment = false;
                if(Reader.NextCharIfCurr(C('[')) && Reader.NextCharIfCurr(C('[')))
                {
                    do
                    {
                        Reader.NextCharUntil(C(']'));
                        if(Reader.NextCharIfCurr(C(']')))
                            break;
                    } 
                    while (Reader.CurrChar() != TR_EOF);

                }
                else
                {
                    Reader.NextCharUntil(C('\n'));
                }


                Reader.NextChar();
                tk.Tk = TK_none;
            }
            else
                tk.Tk = TKS_sub;
        }
        break;
    case C('*'):
        Reader.NextChar();
        tk.Tk = TKS_mul;
        break;
    case C('/'):
        {
            Reader.NextChar();
            if(Reader.NextCharIfCurr(C('/')))
                tk.Tk = TKS_idiv;
            else
                tk.Tk = TKS_div;           
        }
        break;
    case C('='):
        {
            Reader.NextChar();
            if(Reader.NextCharIfCurr(C('=')))
                tk.Tk = TKS_eq;
            else
                tk.Tk = TKS_assign;           
        }
        break;
    case C('>'):
        {
            Reader.NextChar();
            if(Reader.NextCharIfCurr(C('=')))
                tk.Tk = TKS_ge;
            else if (Reader.NextCharIfCurr(C('>')))
                tk.Tk = TKS_rshift;
            else
                tk.Tk = TKS_g;           
        }
        break;    
    case C('<'):
        {
            Reader.NextChar();
            if(Reader.NextCharIfCurr(C('=')))
                tk.Tk = TKS_le;
            else if (Reader.NextCharIfCurr(C('<')))
                tk.Tk = TKS_lshift;
            else
                tk.Tk = TKS_l;           
        }
        break;
    case C('~'):
        {
            Reader.NextChar();
            if(Reader.NextCharIfCurr(C('=')))
                tk.Tk = TKS_ne;
            else
                tk.Tk = TKS_bnot;
        }
        break;
    case C(':'):

        Reader.NextChar();
        if(Reader.NextCharIfCurr(T(':')))
            tk.Tk = TKS_colon2;
        else
            tk.Tk = TKS_colon;
        break;
    case C(';'):
        Reader.NextChar();
        tk.Tk = TKS_semicolon;
        break;
    case C(','):
        Reader.NextChar();
        tk.Tk = TKS_comma;
        break;        
    case C('.'):
        {
            Reader.NextChar();
            if(Reader.NextCharIfCurr(C('.')))
            {
                if(Reader.NextCharIfCurr(C('.')))
                {
                    tk.Tk = TKS_dot3;
                }
                else
                {
                    tk.Tk = TKS_dot2;
                }
            }
            else
                tk.Tk = TKS_dot;
        }
        break;
    
    case C('('):
        Reader.NextChar();
        tk.Tk = TKS_lparenthese;
        break; 
    case C(')'):
        Reader.NextChar();
        tk.Tk = TKS_rparenthese;
        break; 
    case C('['):
        Reader.NextChar();
        tk.Tk = TKS_lbracket;
        break; 
    case C(']'):
        Reader.NextChar();
        tk.Tk = TKS_rbracket;
        break; 
    case C('{'):
        Reader.NextChar();
        tk.Tk = TKS_lbrace;
        break; 
    case C('}'):
        Reader.NextChar();
        tk.Tk = TKS_rbrace;
        break; 
    case C('#'):
        Reader.NextChar();
        tk.Tk = TKS_sharp;
        break;
    case C('^'):
        Reader.NextChar();
        tk.Tk = TKS_power;
        break;        
    case C('\"'):
        if(ReadString(tk, C('\"')) == false)
            tk.Tk = TK_error;
        break;
    case C('\''):
        if(ReadString(tk, C('\'')) == false)
            tk.Tk = TK_error;
        break;
    case C('%'):
        VERBOSE(LogMisc, T("Break point symbol hit\n"));
        Reader.NextChar();
        BreakOnNextToken = true;
        break;
    case C('$'):
        Reader.NextChar();
        tk.Tk = TKS_dollor;
        break;
    default:
        if (IsDigit(Ch))
        {
            if(ReadNumber(tk) == false)
                tk.Tk = TK_error;
        }
        else if((CharFeature[Ch] & CH_NAME_START) != 0 )
        {
            if(ReadName(tk, NoKeywords) == false)
                tk.Tk = TK_error;
        }
        else
        {
            CM.Log(CMT_UnknownCh,  Reader.GetLine(), Reader.GetCol(), Ch);
            tk.Tk = TK_error;
        }

        break;
    }
}


bool Lexer::IsDigit(TCHAR Ch)
{
    return (CharFeature[Ch] & CH_DIGIT) != 0;
}

int Lexer::ReadHexChar()
{
    auto HexToNum = [](TCHAR InCh) -> int
    {
        if((CharFeature[InCh] & CH_DIGIT) != 0)
            return InCh - C('0');
        else if(InCh >= 'a' && InCh <= 'f')
            return InCh - C('a') + 10;
        else // Valid hex char garanteed
            return InCh - C('A') + 10;
    };
    int Ch1 = Reader.NextChar();
    if(Ch1 == TR_EOF)
        return -1;
    if((CharFeature[Ch1] & CH_HEX_DIGIT) == 0)
    {
        CM.Log(CMT_HexExpected, Reader.GetLine(), Reader.GetCol());
        return -1;
    }

    int Ch2 = Reader.NextChar();
    if(Ch2 == TR_EOF)
        return -1;
    if((CharFeature[Ch2] & CH_HEX_DIGIT) == 0)
    {
        CM.Log(CMT_HexExpected, Reader.GetLine(), Reader.GetCol());
        return -1;
    }

    return (Ch1 << 4) + Ch2;



}
bool Lexer::ReadString(Token& tk, const TCHAR Quoater)
{
    int StartLine = Reader.GetLine();
    int StartCol = Reader.GetCol();

    tk.Tk = TK_stringVal;
    OLString& Str = tk.StrOrNameVal;
    Str = "";

    int Ch = Reader.NextChar();
    Reader.BeginNewToken();
    
    while(Ch != Quoater)
    {
        if(Ch == C('\\'))
        {
            Ch = Reader.NextChar();
            switch (Ch)
            {
            case C('a'): Str.AppendCh(C('\a')); break;
            case C('b'): Str.AppendCh(C('\b')); break;
            case C('f'): Str.AppendCh(C('\f')); break;
            case C('n'): Str.AppendCh(C('\n')); break;
            case C('r'): Str.AppendCh(C('\r')); break;
            case C('t'): Str.AppendCh(C('\t')); break;
            case C('v'): Str.AppendCh(C('\v')); break;
            case C('\"'): Str.AppendCh(C('\"')); break;
            case C('\''): Str.AppendCh(C('\'')); break;
            case C('x'): 
                Ch = ReadHexChar();
                if(Ch == -1)
                {
                    return false;
                }
                Str.AppendCh(Ch);
                break;
            case C('u'): break; // to do 
            case C('z'): break; // to do
            default:
                CM.Log(CMT_UnknowEscape, Reader.GetLine(), Reader.GetCol(), Ch);
                return false;
            }

            continue;
        }

        if(Ch == TR_EOF)
        {
            CM.Log(CMT_UnfinishedString, StartLine, StartCol);
            return false;
        }

        Str.AppendCh(Ch);
        Ch = Reader.NextChar();
    }
    Reader.NextChar();
    return true;
    

}

bool Lexer::ReadNumber(Token& tk)
{
    Reader.BeginNewToken();
    int Ch = Reader.CurrChar();
    int FirstCh = Ch;

    int Index = 0;
    bool IsHex = false;
    bool IsDecimal = false;
    bool IsExponent = false;
    while(Ch != TR_EOF)
    {
        Index++;
        if(IsHex == false)
        {
            if(CharFeature[Ch] & CH_DIGIT)
            {
                Ch = Reader.NextChar();
                continue;
            }
        }
        else
        {
            if(CharFeature[Ch] & CH_HEX_DIGIT)
            {
                Ch = Reader.NextChar();
                continue;
            }
        }
        
        if(Index == 2)
        {
            if(FirstCh == C('0') && Reader.NextCharIfCurr(C('x'), C('X')))
            {
                IsHex = true;
                Ch = Reader.CurrChar();
                continue;
            }
        }

        if(IsDecimal == false && IsHex == false)
        {
            if (Reader.NextCharIfCurr(C('e'), C('E')))
            {
                IsExponent = true;
                Reader.NextCharIfCurr(C('+'), C('-'));
                Ch = Reader.CurrChar();
                if((CharFeature[Ch] & CH_DIGIT) == false)
                {
                    CM.Log(CMT_ExponentNumErr, Reader.GetLine(), Reader.GetCol(), Reader.Pick().CStr());
                    return false;
                }

                continue;
            }

        }

        if(IsExponent == false)
        {
            if (Reader.NextCharIfCurr(C('.')))
            {
                IsDecimal = true;
                Ch = Reader.CurrChar();
                if((CharFeature[Ch] & CH_DIGIT) == false)
                {
                    CM.Log(CMT_DecimalNumErr, Reader.GetLine(), Reader.GetCol(), Reader.Pick().CStr());
                    return false;
                }
                continue;
            }
        }

        break;
    }

    OLString str = Reader.PickExcludeCurr();

    if(IsDecimal || IsExponent)
    {
        tk.Tk = TK_floatVal;
        tk.FltVal = strtod(T2A(str.CStr()), nullptr);
        tk.StrOrNameVal = str;
    }
    else
    {
        tk.Tk = TK_intVal;
        tk.IntVal = strtol(T2A(str.CStr()), nullptr, 0);
        tk.StrOrNameVal = str;
    }
    return true;

}

bool Lexer::ReadName(Token& tk, bool NoKeywords)
{
    Reader.BeginNewToken();

    TCHAR Ch = Reader.NextChar();
    while(Ch != TR_EOF && (CharFeature[Ch] & CH_NAME) != 0)
    {
        Ch = Reader.NextChar();
    }

    OLString Text = Reader.PickExcludeCurr();

    ETokenType KeywordToken = KeywordsTable.Find(Text, TK_none);
    if(NoKeywords || KeywordToken == TK_none || (EnableForceEOF == false && KeywordToken == TK_eof))
    {
        tk.Tk = TK_name;
        tk.StrOrNameVal = Text;
    }
    else
    {
        tk.Tk = KeywordToken;
    }

    return true;
}
    

void Lexer::AddBreakPoints(int Line)
{
    BreakPoints.Add(Line);
}

}