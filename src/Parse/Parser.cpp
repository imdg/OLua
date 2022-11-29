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


namespace OL
{

Parser::Parser(Lexer& InLex) : Lex(InLex), CM(Lex.CM)
{

}


Parser::Parser(Lexer& InLex, CompileMsg& InCM)
    : Lex(InLex), CM(InCM)
{

}


}