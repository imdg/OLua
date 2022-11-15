#include "Common.h"
#include "Action.h"

#include "Logger.h"
#include <stdio.h>
#include "AstPool.h"
#include "Env.h"

namespace OL
{

class TestClass
{
public:
    TestClass(int x) {};
};

class GenASTBase : public Action
{
public:

    FILE* SafeOpenWrite(OLString FilePath)
    {
        FILE* ToOpen = fopen(T2A(FilePath.CStr()), "rb");
        if(ToOpen != nullptr)
        {
            ERROR(LogMisc, T("File exists: %s"), FilePath.CStr());
            return nullptr;
        }
        ToOpen = fopen(T2A(FilePath.CStr()), "wb");
        return ToOpen;

    }
    OLString GenHeader(OLString Name, OLString BaseClass)
    {
        OLString Template;
        Template.Append (T("#pragma once\n"));
        Template.Append (T("#include \"Common.h\"\n"));
        Template.Append (T("#include \"AstCommon.h\"\n"));
        Template.AppendF(T("#include \"%s.h\"\n"), BaseClass.CStr());
        Template.Append (T("\n"));
        Template.Append (T("namespace OL\n"));
        Template.Append (T("{\n"));
        Template.AppendF(T("class %s : public %s\n"), Name.CStr(), BaseClass.CStr());
        Template.Append (T("{\n"));
        Template.Append (T("    DECLEAR_RTTI()\n"));
        Template.Append (T("public:\n"));
        Template.AppendF(T("    %s();\n"), Name.CStr());
        Template.AppendF(T("    virtual ~%s();\n"), Name.CStr());
        Template.Append (T("\n"));
        Template.Append (T("    virtual EVisitStatus Accept(Visitor* Vis);\n"));
        Template.Append (T("    virtual EVisitStatus Accept(RecursiveVisitor * Vis);\n"));
        Template.Append (T("};\n"));
        Template.Append (T("\n"));
        Template.Append (T("}\n"));

        return Template;

    };

    OLString GenImpl(OLString Name, OLString BaseClass)
    {
        OLString Template;
        Template.AppendF(T("#include \"%s.h\"\n"), Name.CStr());
        Template.Append (T("\n"));
        Template.Append (T("namespace OL\n"));
        Template.Append (T("{\n"));
        Template.Append (T("\n"));
        Template.AppendF(T("RTTI_BEGIN_INHERITED(%s, %s)\n"), Name.CStr(), BaseClass.CStr());
        Template.Append (T("\n"));
        Template.AppendF(T("RTTI_END(%s)\n"), Name.CStr());
        Template.Append (T("\n"));
        Template.Append (T("\n"));
        Template.AppendF(T("%s::%s()\n"), Name.CStr(), Name.CStr());
        Template.AppendF(T("    : %s()\n"), BaseClass.CStr());
        Template.Append (T("{\n"));
        Template.Append (T("}\n"));
        Template.Append (T("\n"));
        Template.AppendF(T("%s::~%s()\n"), Name.CStr(), Name.CStr());
        Template.Append (T("{\n"));
        Template.Append (T("}\n"));
        Template.Append (T("\n"));
        Template.AppendF(T("EVisitStatus %s::Accept(Visitor* Vis)\n"), Name.CStr());
        Template.Append (T("{\n"));
        Template.Append (T("    return Vis->Visit(this);\n"));
        Template.Append (T("}\n"));
        Template.Append (T("\n"));
        Template.AppendF(T("EVisitStatus %s::Accept(RecursiveVisitor* Vis)\n"), Name.CStr());
        Template.Append (T("{\n"));
        Template.Append (T("    return Vis->Visit(this);\n"));
        Template.Append (T("}\n"));
        Template.Append (T("\n"));
        Template.Append (T("}\n"));
        Template.Append (T("\n"));

        return Template;
    }

    void WriteClass(OLString Name, OLString Base, OLString Dir)
    {
        OLString BasePath = Dir; //Env::GetSavedDir(T("TestAstGen"));
        OLString Header = BasePath;
        Header.AppendF(T("/%s.h"), Name.CStr());

        OLString Impl = BasePath;
        Impl.AppendF(T("/%s.cpp"), Name.CStr());
        
        FILE* HeaderFile = SafeOpenWrite(Header); // fopen(T2A(Header.CStr()), "wb");
        if(HeaderFile == nullptr)
            return;
        OLString HeaderContent = GenHeader(Name, Base);
        fwrite(HeaderContent.CStr(), HeaderContent.Len() * sizeof(TCHAR), 1, HeaderFile);
        fclose(HeaderFile);

        FILE* ImplFile = SafeOpenWrite(Impl) ;//fopen(T2A(Impl.CStr()), "wb");
        if(ImplFile == nullptr)
            return;

        OLString ImplContent = GenImpl(Name, Base);
        fwrite(ImplContent.CStr(), ImplContent.Len() * sizeof(TCHAR), 1, ImplFile);
        fclose(ImplFile);
    }

    struct ClassDesc
    {
        OLString ClassName;
        OLString BaseName;
    };

    void GenList(OLString Title, OLList<ClassDesc>& List)
    {
        OLString BasePath(T("F:/OtherProj/OLua/src/Parse/Ast"));

        OLString Dir = BasePath + T("/") + Title.CStr();
        Env::MakeDir(Dir);

        // Gen common include
        OLString CommonFilePath;
        CommonFilePath.Printf(T("%s/%s/%sCommon.h"), BasePath.CStr(), Title.CStr(), Title.CStr());
        FILE* CommonFile = SafeOpenWrite(CommonFilePath); //fopen(T2A(CommonFilePath.CStr()), "rb");
        if(CommonFile != nullptr)
        {
            fprintf(CommonFile, "#pragma once\n");
            for(int i = 0; i < List.Count(); i++)
            {
                fprintf(CommonFile, "#include \"%s.h\"\n", List[i].ClassName.CStr());
            }

            fclose(CommonFile);
        }

        OLString DeclFilePath;
        DeclFilePath.Printf(T("%s/%s/%sDecl.h"), BasePath.CStr(), Title.CStr(), Title.CStr());
        FILE* DeclFile = SafeOpenWrite(DeclFilePath); //fopen(T2A(CommonFilePath.CStr()), "rb");
        if(DeclFile != nullptr)
        {
            fprintf(DeclFile, "#pragma once\n");
            for(int i = 0; i < List.Count(); i++)
            {
                fprintf(DeclFile, "class %s;\n", List[i].ClassName.CStr());
            }

            fclose(DeclFile);
        }


        // Gen file
        OLString CurrPath;
        CurrPath.Printf(T("%s/%s"), BasePath.CStr(), Title.CStr());
        for(int i = 0; i < List.Count(); i++)
        {
            WriteClass(List[i].ClassName, List[i].BaseName, CurrPath);
        }
    }


    void GenExpression()
    {
        OLList<ClassDesc> ExpressionList;
        ExpressionList.Add({T("AExpr"),             T("ABase")});
        ExpressionList.Add({T("ASimpleExpr"),       T("AExpr")});
        ExpressionList.Add({T("AConstExpr"),        T("ASimpleExpr")});
        ExpressionList.Add({T("AConstructor"),      T("ASimpleExpr")});
        ExpressionList.Add({T("AFuncExpr"),         T("ASimpleExpr")});
        ExpressionList.Add({T("ATypeCast"),         T("ASimpleExpr")});

        ExpressionList.Add({T("ASuffixedExpr"),     T("ASimpleExpr")});

        ExpressionList.Add({T("ADereferance"),           T("ASuffixedExpr")});
        ExpressionList.Add({T("ADotMember"),        T("ADereferance")});
        ExpressionList.Add({T("ABracketMember"),    T("ADereferance")});  

        ExpressionList.Add({T("ACall"),             T("ASuffixedExpr")});
        ExpressionList.Add({T("ANormalCall"),       T("ACall")});
        ExpressionList.Add({T("AColonCall"),        T("ACall")});

        ExpressionList.Add({T("ASubexpr"),          T("AExpr")});

        ExpressionList.Add({T("APrimaryExpr"),      T("AExpr")});
        ExpressionList.Add({T("AVarRef"),           T("APrimaryExpr")});
        ExpressionList.Add({T("AParentheses"),      T("APrimaryExpr")});
        ExpressionList.Add({T("ATypeCast"),         T("APrimaryExpr")});

        GenList(T("Expression"), ExpressionList);
    }

    void GenStatement()
    {
        OLList<ClassDesc> StatementList;
        StatementList.Add({T("AStat"),              T("ABase")});
        StatementList.Add({T("ABlockStat"),         T("AStat")});
        StatementList.Add({T("ADoWhile"),           T("ABlockStat")});
        StatementList.Add({T("AWhile"),             T("ABlockStat")});
        StatementList.Add({T("AFuncDef"),           T("ABlockStat")});
        StatementList.Add({T("AForNum"),               T("ABlockStat")});
        StatementList.Add({T("AForList"),               T("ABlockStat")});
        StatementList.Add({T("ARepeat"),            T("ABlockStat")});
        StatementList.Add({T("AIfStat"),            T("ABlockStat")});

        StatementList.Add({T("AAssignment"),        T("AStat")});
        StatementList.Add({T("AReturn"),            T("AStat")});
        StatementList.Add({T("ACallStat"),          T("AStat")});
        StatementList.Add({T("ALabel"),             T("AStat")});
        StatementList.Add({T("ABreak"),             T("AStat")});
        StatementList.Add({T("AGoto"),              T("AStat")});
        StatementList.Add({T("ALocal"),             T("AStat")});

        GenList(T("Statement"), StatementList);
    }

    void GenScope()
    {
        OLList<ClassDesc> ScopeList;
        ScopeList.Add({T("AClass"),                 T("AStat")});
        ScopeList.Add({T("AClassMember"),           T("ABase")});
        ScopeList.Add({T("AClassVar"),              T("AClassMember")});
        ScopeList.Add({T("AMethod"),                T("AClassMember")});
        ScopeList.Add({T("AClassContructor"),       T("AMethod")});
        ScopeList.Add({T("AClassDestructor"),       T("AMethod")});
        ScopeList.Add({T("ANormalMethod"),          T("AMethod")});
        ScopeList.Add({T("AParam"),          T("ABase")});

        GenList(T("Scope"), ScopeList);
    }

    virtual int Run()
    {
        // GenExpression();  //Finished
        // GenStatement();   //Finished   
        GenScope();
        return 0;
    };
};

REGISTER_ACTION(GenASTBase)

}