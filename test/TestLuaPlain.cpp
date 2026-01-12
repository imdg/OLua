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
#include "SymbolTable.h"
#include "SymbolWalker.h"
#include "DebugVisitor.h"
#include "TypeInfoVisitor.h"
#include "TypeDerivationVisitor.h"
#include "TextBuilder.h"
#include "LuaPlainInterp.h"
#include "MemTextStream.h"
#include "ContextValidator.h"
#include "LPClassLib.h"

namespace OL
{

class TestLuaPlain : public Action
{
public:

    void ApplyTypeDeriDebugInfo(DebugVisitor& Vis, TypeDerivationVisitor& TypeDeri)
    {
        Vis.OnBeginVisitCallbacks.Add([&TypeDeri](SPtr<ABase> Node)
        { 
            VERBOSE(LogTester, T("BeginVisit: %s (%d, %d)\n %s"), Node->GetType().Name, Node->SrcRange.Start.Line, Node->SrcRange.Start.Col, TypeDeri.GetDump().CStr()); 
        });
        Vis.OnEndVisitCallbacks.Add([&TypeDeri](SPtr<ABase> Node)
        { 
            VERBOSE(LogTester, T("EndVisit: %s (%d, %d)\n %s"), Node->GetType().Name, Node->SrcRange.Start.Line, Node->SrcRange.Start.Col, TypeDeri.GetDump().CStr()); 
        });
        Vis.OnVisitCallbacks.Add([&TypeDeri](SPtr<ABase> Node)
        { 
            VERBOSE(LogTester, T("Visit: %s (%d, %d)\n %s"), Node->GetType().Name, Node->SrcRange.Start.Line, Node->SrcRange.Start.Col, TypeDeri.GetDump().CStr()); 
        });
    };

    void ApplyLuaPlainDebugInfo(DebugVisitor& Vis, LuaPlainInterp& LuaPlain)
    {
        Vis.OnEndVisitFinishCallbacks.Add([&LuaPlain](SPtr<ABase> Node, EVisitStatus Status)
        {
            MemTextStream Stream;
            Stream.WriteFormat(T("---OnEndVisitFinish Node:%s (%d,%d)\n"), Node->GetType().Name, Node->SrcRange.Start.Line, Node->SrcRange.Start.Col);
            for(int i = 0; i < LuaPlain.ContentStack.Count(); i++)
            {
                Stream.WriteFormat(T("    %d: %s(%d,%d) :"), i, LuaPlain.ContentStack[i].Node->GetType().Name
                    , LuaPlain.ContentStack[i].Node->SrcRange.Start.Line, LuaPlain.ContentStack[i].Node->SrcRange.Start.Col);
                LuaPlain.OutText.WriteStreamFrom(Stream, LuaPlain.ContentStack[i].CurrText);
                Stream.WriteText(T("\n"));
            }

            VERBOSE(LogTester, Stream.Str.CStr());
        });
    };

    void LogAst(SPtr<ABase> root, OLString AstFile)
    {
        FileStream fsAst;
        fsAst.OpenWrite(AstFile);
        TextSerializer tsAst(&fsAst);
        tsAst.WriteRTTI(root.Get(), root->GetTypePtr());
        fsAst.Close();
    }

    void LogSymbol(SymbolWalker& symbolWalker, OLString SymbolFile)
    {
        FileStream fsSymbol;
        fsSymbol.OpenWrite(SymbolFile);
        for (int i = 0; i < symbolWalker.CurrSymbolTable.Scopes.Count(); i++)
        {
            fsSymbol.WriteFormat(T("\n--------Scope %d : (0x%llx)-----------\n"), i, (uint64)symbolWalker.CurrSymbolTable.Scopes[i].Get());
            TextSerializer ts(&fsSymbol);
            ts.WriteRTTI(symbolWalker.CurrSymbolTable.Scopes[i].Get(), symbolWalker.CurrSymbolTable.Scopes[i]->GetTypePtr());
        }
        fsSymbol.Close();
    }

    void LogLua(TextBuilder& builder, OLString LuaFile)
    {
        FileStream fsLua;
        fsLua.OpenWrite(LuaFile);
        builder.WriteStream(fsLua);
        fsLua.Close();
    }

    void RunFile(OLString SrcFile, OLString SymbolFile, OLString AstFile, OLString LuaFile)
    {
        TokenReader tr;
        tr.LoadFromFile(SrcFile.CStr());

        // Lex and Parse
        Lexer lex(tr);
        CompileMsg &CM = lex.CM;
        CM.SetFile(SrcFile.NameFromPath().CStr());
        CM.SetAsWarning(CMT_NoType);
        CM.SetAsWarning(CMT_UnresolvedVar);
        CM.SetAsWarning(CMT_NoMember);       
        Parser par(lex);

        SPtr<ARoot> root = par.Parse_Root(SrcFile);
        if(root == nullptr)
        {
            VERBOSE(LogTester, T("Failed to parse: %s"), SrcFile.CStr());
            return;
        }
        //LogAst(root, AstFile);
        CompleteAst complete;
        root->Accept(&complete);

        SymbolTable symboTable;
        SymbolWalker symbolWalker(symboTable, CM);
        DebugVisitor debugSymbolWalker(&symbolWalker, false);
        root->Accept(&debugSymbolWalker);

        if(CM.ErrorCount > 0)
        {
            LogAst(root, AstFile);
            VERBOSE(LogTester, T("Failed to parse symbols: %s"), SrcFile.CStr());
            VERBOSE(LogTester, T("     Ast file: %s"), AstFile.CStr());
            return;
        }




        TypeInfoVisitor typeInfoVisitor(symboTable, BuildSetting::DefaultBuildSetting, CM);
        DebugVisitor debugTypeInfo(&typeInfoVisitor, false);
        root->Accept(&debugTypeInfo);
        if(CM.ErrorCount > 0)
        {
            LogAst(root, AstFile);
            LogSymbol(symbolWalker, SymbolFile);
            VERBOSE(LogTester, T("Failed to parse type info: %s"), SrcFile.CStr());
            VERBOSE(LogTester, T("     Ast file: %s"), AstFile.CStr());
            VERBOSE(LogTester, T("     Symbol file: %s"), SymbolFile.CStr());
            return;
        }
        
            
        symboTable.ResolveSymbolsAndTypes(CM, SRP_Standalone);
        if(CM.ErrorCount > 0)
        {
            LogAst(root, AstFile);
            LogSymbol(symbolWalker, SymbolFile);
            VERBOSE(LogTester, T("Failed to resolve symbol: %s"), SrcFile.CStr());
            VERBOSE(LogTester, T("     Ast file: %s"), AstFile.CStr());
            VERBOSE(LogTester, T("     Symbol file: %s"), SymbolFile.CStr());
            return;
        }

        ContextValidator contextVali(symboTable, CM);
        DebugVisitor debugContextVali(&contextVali, false);
        root->Accept(&debugContextVali);
        if(CM.ErrorCount > 0)
        {
            VERBOSE(LogTester, T("Failed to validate context: %s"), SrcFile.CStr());
            VERBOSE(LogTester, T("     Ast file: %s"), AstFile.CStr());
            VERBOSE(LogTester, T("     Symbol file: %s"), SymbolFile.CStr());
            return;
        }


        TypeDerivationVisitor typeDeri(symboTable, CM);
        DebugVisitor debugTypeDeri(&typeDeri, true);
        if (Cmd().HasKey(T("--log_type_deri")))
        {
            ApplyTypeDeriDebugInfo(debugTypeDeri, typeDeri);
        }
        root->Accept(&debugTypeDeri);
        if(CM.ErrorCount > 0)
        {
            VERBOSE(LogTester, T("Failed to derive types: %s"), SrcFile.CStr());
            VERBOSE(LogTester, T("     Ast file: %s"), AstFile.CStr());
            VERBOSE(LogTester, T("     Symbol file: %s"), SymbolFile.CStr());
            return;
        }

        bool HasLua = false;
        TextBuilder builder;
        LPClassLib classLib;
        classLib.BeginFile(T("standalone"));
        LuaPlainInterp luaPlain(symboTable, builder, classLib);
        DebugVisitor debugLuaPlain(&luaPlain, true);
        if (Cmd().HasKey(T("--log_lua_plain")))
        {
            ApplyLuaPlainDebugInfo(debugLuaPlain, luaPlain);
        }
        
        root->Accept(&debugLuaPlain);
        classLib.EndFile();
        luaPlain.MergeStaticBlock();
        
        classLib.ApplyClassToStandaloneFile(luaPlain.OutText.Root, luaPlain.OutText.Root);
        
        if(Cmd().HasKey(T("--entry")))
        {
            OLString Entry = Cmd().GetFirstParam(T("--entry"));
            if(Entry != T(""))
            {
                builder.Root->NewLine().Indent().AppendF(T("%s()"), Entry.CStr()).NewLine();
            }
        }

        LogAst(root, AstFile);
        LogSymbol(symbolWalker, SymbolFile);
        LogLua(builder, LuaFile);


        VERBOSE(LogTester, T("Successfully compiled %s"), SrcFile.CStr());
        VERBOSE(LogTester, T("     Ast file: %s"), AstFile.CStr());
        VERBOSE(LogTester, T("     Symbol file: %s"), SymbolFile.CStr());
        VERBOSE(LogTester, T("     Lua file: %s"), LuaFile.CStr());

    };

    virtual int Run()
    {
        OLString SrcPath = Env::GetDataPath() + T("/TestLuaPlain/");
        SrcPath += Cmd().GetFirstParam(T("--src"), T("DefaultTest.txt"));

        OLString SymboFileName = SrcPath.NameFromPath();
        SymboFileName.Append(T(".symbol"));
        OLString SymbolPath = Env::PrepareSavedFilePath(T("TestLuaPlain"), SymboFileName.CStr());

        OLString AstFileName = SrcPath.NameFromPath();
        AstFileName.Append(T(".ast"));
        OLString AstPath = Env::PrepareSavedFilePath(T("TestLuaPlain"), AstFileName.CStr());

        OLString LuaFileName = SrcPath.NameFromPath();
        LuaFileName.Append(T(".lua"));
        OLString LuaPath = Env::PrepareSavedFilePath(T("TestLuaPlain"), LuaFileName.CStr());

        RunFile(SrcPath, SymbolPath, AstPath, LuaPath);
        return 0;
    };
};

REGISTER_ACTION(TestLuaPlain)
}