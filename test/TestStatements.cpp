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
namespace OL
{

class TestStatements : public Action
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

        int counter = 0;

        OLList<SPtr<AStat> > Stats;

        bool Succ = par.Helper_ParseBlock(Stats, true);
        
        if(Succ == true)
        {
            OLString DstPath = Env::PrepareSavedFilePath(T("TestParser"), T("Statements.log"));
            FileStream fs;
            fs.OpenWrite(DstPath);
            TextSerializer ts(&fs);
            for(int i = 0; i < Stats.Count(); i++)
            {
                fs.WriteFormat(T("\n---------- STAT: %d -------------------\n"), i);
                ts.WriteRTTI(Stats[i].Get(), &(Stats[i]->GetType()) );
            }

            fs.Close();
        }
        return 0;

    };
};


REGISTER_ACTION(TestStatements)
}