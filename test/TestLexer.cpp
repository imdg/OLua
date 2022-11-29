#include "Common.h"
#include "Action.h"
#include "Lexer.h"
#include "Logger.h"
#include "Env.h"
#include "CmdConfig.h"
#include "TokenReader.h"

namespace OL
{

class TestLexer : public Action
{
public:
    virtual int Run()
    {
        OLString SrcPath = Env::GetDataPath() + T("/TestLexer/");
        SrcPath += Cmd().GetFirstParam(T("--lextest"), T("DefaultTest.txt") );

        TokenReader tr;
        tr.LoadFromFile(SrcPath.CStr());

        Lexer lex(tr);

        Token curr;
        int counter = 0;
        do
        {
            lex.Next();
            curr = lex.GetCurrent();
            VERBOSE(LogTester, T("%s"), curr.ToString().CStr() );
            if(counter++ >= 100)
                break;
        }
        while(curr.Tk != TK_eof && curr.Tk != TK_error);
        
        return 0;
    };
};


REGISTER_ACTION(TestLexer)
}