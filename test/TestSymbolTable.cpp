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
#include "ARoot.h"
#include "SymbolWalker.h"
#include "CompleteAst.h"
#include "TypeInfoVisitor.h"
#include "SymbolScope.h"
#include "DebugVisitor.h"

namespace OL
{


class TestSymbolTable : public Action
{
public:
    virtual int Run()
    {
        OLString SrcPath = Env::GetDataPath() + T("/TestSemantics/");
        SrcPath += Cmd().GetFirstParam(T("--symboltest"), T("DefaultTest.txt") );

        TokenReader tr;
        tr.LoadFromFile(SrcPath.CStr());

        Lexer lex(tr);

        lex.CM.SetFile(T("Short"));
        Parser par(lex);
        SPtr<ARoot> root = par.Parse_Root(SrcPath);

        CompleteAst complete;
        root->Accept(&complete);

        SymbolTable symboTable;
        SymbolWalker symbolWalker(symboTable, lex.CM);
        DebugVisitor debugSymbolWalker(&symbolWalker, false);
        root->Accept(&debugSymbolWalker);


        TypeInfoVisitor typeInfoVisitor(symboTable, lex.CM);
        DebugVisitor debugTypeInfo(&typeInfoVisitor, false);
        root->Accept(&debugTypeInfo);

        symboTable.ResolveSymbolsAndTypes(lex.CM, SRP_Standalone);
        if(root != nullptr)
        {
            OLString DstPath = Env::PrepareSavedFilePath(T("TestSymbolTable"), T("TestSymbolTable.log"));
            FileStream fs;
            fs.OpenWrite(DstPath);
            for(int i = 0; i < symbolWalker.CurrSymbolTable.Scopes.Count(); i++)
            {
                fs.WriteFormat(T("\n--------Scope %d : (0x%llx)-----------\n"), i,  (uint64)symbolWalker.CurrSymbolTable.Scopes[i].Get());
                TextSerializer ts(&fs);
                ts.WriteRTTI(symbolWalker.CurrSymbolTable.Scopes[i].Get(), symbolWalker.CurrSymbolTable.Scopes[i]->GetTypePtr());
            }
            fs.Close();


            OLString AstPath = Env::PrepareSavedFilePath(T("TestSymbolTable"), T("TestSymbolTableAst.log"));

            FileStream fs2;
            fs2.OpenWrite(AstPath);
            TextSerializer ts2(&fs);
            ts2.WriteRTTI(root.Get(), root->GetTypePtr());
            fs2.Close();
        }
        return 0;

    };
};


REGISTER_ACTION(TestSymbolTable)
}