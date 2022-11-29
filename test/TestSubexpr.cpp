#include "Common.h"
#include "Action.h"
#include "Env.h"
#include "Lexer.h"
#include "Logger.h"

#include "CmdConfig.h"
#include "TokenReader.h"
#include "Parser.h"
#include "TextSerializer.h"
#include "FileStream.h"
#include "AExpr.h"
namespace OL
{

class TestSubexpr : public Action
{
public:
    virtual int Run()
    {
        OLString SrcPath = Env::GetDataPath() + T("/TestParser/");
        SrcPath += Cmd().GetFirstParam(T("--parsetest"), T("DefaultTest.txt") );

        TokenReader tr;
        tr.LoadFromFile(SrcPath.CStr());

        Lexer lex(tr);
        lex.Next();
        Parser par(lex);
        Token curr;
        int counter = 0;

        AExpr* exp = par.Parse_Expr();
        
        if(exp != nullptr)
        {
            //VERBOSE(LogTester, T("Success\n"));

            OLString DstPath = Env::PrepareSavedFilePath(T("TestParser"), T("AllExp.log"));
            FileStream fs;
            fs.OpenWrite(DstPath);
            TextSerializer ts(&fs);
            ts.WriteRTTI(exp, &(exp->GetType()) );
            fs.Close();
        }
        
        return 0;
    };
};


REGISTER_ACTION(TestSubexpr)
}