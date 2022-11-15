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
#include "AStat.h"
#include "ARoot.h"
#include "CompleteAst.h"

namespace OL
{

class TestVisitor : public Action
{
public:
    virtual int Run()
    {
        OLString SrcPath = Env::GetDataPath() + T("/TestSemantics/");
        SrcPath += Cmd().GetFirstParam(T("--semtest"), T("DefaultTest.txt") );

        TokenReader tr;
        tr.LoadFromFile(SrcPath.CStr());

        Lexer lex(tr);
        Parser par(lex);

        ARoot* Root = par.Parse_Root(T("Main"));
        
        CompleteAst CompAst;
        Root->Accept(&CompAst);

        if(Root != nullptr)
        {
            OLString DstPath = Env::PrepareSavedFilePath(T("TestSemantics"), T("Visitor.log"));
            FileStream fs;
            fs.OpenWrite(DstPath);
            TextSerializer ts(&fs);
            ts.WriteRTTI(Root, Root->GetTypePtr() );


            fs.Close();
            VERBOSE(LogTester, T("TestVisiter success\n"));
        }
        else
        {
            WARNING(LogTester, T("TestVisiter FAIL !!!!!\n"));
        }
        
        return 0;
    };
};


REGISTER_ACTION(TestVisitor)
}