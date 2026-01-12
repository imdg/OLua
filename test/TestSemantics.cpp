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
#include "TypeDerivationVisitor.h"


namespace OL
{


class TestSemantics : public Action
{
public:

    void RunFile(OLString SrcFile, OLString SymbolFile, OLString AstFile)
    {
       TokenReader tr;
        tr.LoadFromFile(SrcFile.CStr());

        Lexer lex(tr);
        CompileMsg& CM = lex.CM;
        CM.SetFile(SrcFile.NameFromPath().CStr());
        Parser par(lex);
        SPtr<ARoot> root = par.Parse_Root(SrcFile);

        if(root != nullptr)
        {
            CompleteAst complete;
            root->Accept(&complete);


            SymbolTable symboTable;
            SymbolWalker symbolWalker(symboTable, CM);
            DebugVisitor debugSymbolWalker(&symbolWalker, false);
            if(CM.ErrorCount == 0)
                root->Accept(&debugSymbolWalker);


            TypeInfoVisitor typeInfoVisitor(symboTable, BuildSetting::DefaultBuildSetting,  CM);
            DebugVisitor debugTypeInfo(&typeInfoVisitor, false);
            if(CM.ErrorCount == 0)
                root->Accept(&debugTypeInfo);
            
            if(CM.ErrorCount == 0)
                symboTable.ResolveSymbolsAndTypes(CM, SRP_Standalone);

            if(Cmd().HasKey(T("--skip_type_deri")) == false && CM.ErrorCount == 0 )
            {
                TypeDerivationVisitor typeDeri(symboTable, CM);
                DebugVisitor debugTypeDeri(&typeDeri, false);

                if(Cmd().HasKey(T("--log_type_deri")))
                {
                    debugTypeDeri.OnBeginVisitCallbacks.Add([&typeDeri](SPtr<ABase> Node)
                    {
                        VERBOSE(LogTester, T("BeginVisit: %s (%d, %d)\n %s"), Node->GetType().Name, Node->SrcRange.Start.Line, Node->SrcRange.Start.Col, typeDeri.GetDump().CStr());
                    });
                    debugTypeDeri.OnEndVisitCallbacks.Add([&typeDeri](SPtr<ABase> Node)
                    {
                        VERBOSE(LogTester, T("EndVisit: %s (%d, %d)\n %s"), Node->GetType().Name,  Node->SrcRange.Start.Line, Node->SrcRange.Start.Col, typeDeri.GetDump().CStr());
                    });
                    debugTypeDeri.OnVisitCallbacks.Add([&typeDeri](SPtr<ABase> Node)
                    {
                        VERBOSE(LogTester, T("Visit: %s (%d, %d)\n %s"),Node->GetType().Name, Node->SrcRange.Start.Line, Node->SrcRange.Start.Col, typeDeri.GetDump().CStr());
                    });
                }
                root->Accept(&debugTypeDeri);
            }
            FileStream fs;
            fs.OpenWrite(SymbolFile);
            for(int i = 0; i < symbolWalker.CurrSymbolTable.Scopes.Count(); i++)
            {
                fs.WriteFormat(T("\n--------Scope %d : (0x%llx)-----------\n"), i,  (uint64)symbolWalker.CurrSymbolTable.Scopes[i].Get());
                TextSerializer ts(&fs);
                ts.WriteRTTI(symbolWalker.CurrSymbolTable.Scopes[i].Get(), symbolWalker.CurrSymbolTable.Scopes[i]->GetTypePtr());
            }
            fs.Close();

            FileStream fs2;
            fs2.OpenWrite(AstFile);
            TextSerializer ts2(&fs);
            ts2.WriteRTTI(root.Get(), root->GetTypePtr());
            fs2.Close();

            VERBOSE(LogTester, T("Successfully compiled %s"), SrcFile.CStr());
            VERBOSE(LogTester, T("     Symbol file: %s"), SymbolFile.CStr());
            VERBOSE(LogTester, T("     Ast file: %s"), AstFile.CStr());
        }
        else
        {
            ERROR(LogTester, T("Fail to compile %s"), SrcFile.CStr() );
        }
    };

    virtual int Run()
    {
        OLString SrcPath = Env::GetDataPath() + T("/FileCases/");
        SrcPath += Cmd().GetFirstParam(T("--src"), T("DefaultTest.txt") );

        OLString SymboFileName = SrcPath.NameFromPath();
        SymboFileName.Append(T(".symbol"));
        OLString SymbolPath =  Env::PrepareSavedFilePath(T("TestSemantics"), SymboFileName.CStr() );

        OLString AstFileName = SrcPath.NameFromPath();
        AstFileName.Append(T(".ast"));
        OLString AstPath =  Env::PrepareSavedFilePath(T("TestSemantics"), AstFileName.CStr() );

        RunFile(SrcPath, SymbolPath, AstPath);
        return 0;
    };

};


REGISTER_ACTION(TestSemantics)
}