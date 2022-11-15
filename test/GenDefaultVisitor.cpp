#include "Common.h"
#include "Action.h"
#include "Logger.h"
#include "ABase.h"
#include <stdio.h>


namespace OL
{

struct TypeNodeInfo
{
    TypeInfo* Type;
    bool IsLeaf;
};

bool IsLeafType(const TypeInfo* Type)
{
    TypeMember* Member = Type->Members;
    while (Member->Type.UniqVal != 0)
    {
        if(Member->Type.IsRTTI())
        {
            if(Member->Type.RTTI->IsKindOf(ABase::RTTI) )
                return false;
            if(false == IsLeafType(Member->Type.RTTI))
                return false;
        }

        if(Member->KeyType.IsRTTI())
        {
            if(Member->KeyType.RTTI->IsKindOf(ABase::RTTI) )
                return false;
            if(false == IsLeafType(Member->KeyType.RTTI))
                return false;
        }
        Member++;
    }
    return true;
}

bool TranversSubClass(TypeInfo* BaseType, OLList<TypeNodeInfo>& OutTypes, bool ForceNonLeaf)
{
    bool IsLeaf = IsLeafType(BaseType);
    if(ForceNonLeaf)
        IsLeaf = false;
    TypeInfo* Node = TypeInfo::GlobalTypeList;

    while(Node != nullptr)
    {
        if(Node->BaseType == BaseType)
        {
            bool SubclassIsLeaf = TranversSubClass(Node, OutTypes, IsLeaf == false ? true:false);
            if(SubclassIsLeaf == false)
                IsLeaf = false;
        }
        Node = Node->GlobalNext;
    }

    //CallBack(BaseType, IsLeaf);
    TypeNodeInfo Item;
    Item.Type = BaseType;
    Item.IsLeaf = IsLeaf;
    OutTypes.Add(Item);

    return IsLeaf;
}



class GenDefaultVisitor : public Action
{
public:

    void WriteVisitorSPtr(OLString& HeaderContent, OLString& CPPContent)
    {
        HeaderContent.Append(T("#pragma once\n"));
        HeaderContent.Append(T("#include \"Common.h\"\n"));
        HeaderContent.Append(T("#include \"ExpressionDecl.h\"\n"));
        HeaderContent.Append(T("#include \"StatementDecl.h\"\n"));
        HeaderContent.Append(T("#include \"ScopeDecl.h\"\n\n"));
        HeaderContent.Append(T("namespace OL \n{\n"));

        HeaderContent.Append(T("enum EVisitStatus\n"));
        HeaderContent.Append(T("{\n"));
        HeaderContent.Append(T("    VS_Continue,\n"));
        HeaderContent.Append(T("    VS_ContinueParent,\n"));
        HeaderContent.Append(T("    VS_Stop\n"));
        HeaderContent.Append(T("};\n\n"));

        HeaderContent.AppendF(T("class ABase;\n"));
        HeaderContent.AppendF(T("class Visitor\n"));
        HeaderContent.Append (T("{\n"));
        HeaderContent.Append (T("public:\n"));


        CPPContent.AppendF(T("#include \"Common.h\"\n"));
        CPPContent.AppendF(T("#include \"VisitorBase.h\"\n"));
        CPPContent.Append(T("#include \"ExpressionInclude.h\"\n"));
        CPPContent.Append(T("#include \"StatementInclude.h\"\n"));
        CPPContent.Append(T("#include \"ScopeInclude.h\"\n\n"));

        CPPContent.Append(T("namespace OL \n{\n"));

        OLString Header2;
        OLString CPP2;

        OLString Header2_NonLeaf = T("\n");
        OLString CPP2_NonLeaf = T("\n");;

        Header2.AppendF(T("class RecursiveVisitor : public Visitor\n"));
        Header2.Append (T("{\n"));
        Header2.Append (T("public:\n"));


        OLString HeaderDebugger;
        OLString CPPDebugger;

        HeaderDebugger.Append(T("class VisitorWrapper : public RecursiveVisitor\n"));
        HeaderDebugger.Append(T("{\n"));
        HeaderDebugger.Append(T("public:\n"));
        HeaderDebugger.Append(T("    RecursiveVisitor* WrappedVisitor;\n"));
        HeaderDebugger.Append(T("    virtual void OnVisit(SPtr<ABase> Node);\n"));
        HeaderDebugger.Append(T("    virtual void OnBeginVisit(SPtr<ABase> Node);\n"));
        HeaderDebugger.Append(T("    virtual void OnEndVisit(SPtr<ABase> Node);\n"));

        HeaderDebugger.Append(T("    virtual void OnVisitFinish(SPtr<ABase> Node, EVisitStatus Status);\n"));
        HeaderDebugger.Append(T("    virtual void OnBeginVisitFinish(SPtr<ABase> Node, EVisitStatus Status);\n"));
        HeaderDebugger.Append(T("    virtual void OnEndVisitFinish(SPtr<ABase> Node, EVisitStatus Status);\n"));

        CPPDebugger.Append(T("void VisitorWrapper::OnVisit(SPtr<ABase> Node) {} \n"));
        CPPDebugger.Append(T("void VisitorWrapper::OnBeginVisit(SPtr<ABase> Node) {} \n"));
        CPPDebugger.Append(T("void VisitorWrapper::OnEndVisit(SPtr<ABase> Node) {} \n"));
        
        CPPDebugger.Append(T("void VisitorWrapper::OnVisitFinish(SPtr<ABase> Node, EVisitStatus Status) {} \n"));
        CPPDebugger.Append(T("void VisitorWrapper::OnBeginVisitFinish(SPtr<ABase> Node, EVisitStatus Status) {} \n"));
        CPPDebugger.Append(T("void VisitorWrapper::OnEndVisitFinish(SPtr<ABase> Node, EVisitStatus Status) {} \n"));
        OLList<TypeNodeInfo> AllTypes;
        TranversSubClass(&(ABase::RTTI), AllTypes, false);
        //[&HeaderContent, &CPPContent, &Header2, &CPP2] (const TypeInfo* Curr, bool IsLeaf)
        for(int i = AllTypes.Count() - 1; i >= 0; i--)
        {
            TypeInfo* Curr = AllTypes[i].Type;
            bool IsLeaf = AllTypes[i].IsLeaf;

            //bool NeedCallBase  = Curr->BaseType == &ABase::RTTI || Curr == &ABase::RTTI;
            bool IsRoot  = ( Curr == &ABase::RTTI);

            HeaderContent.AppendF(T("    virtual EVisitStatus Visit(SPtr<%s> Node);\n"), Curr->Name);
            CPPContent.AppendF(T("EVisitStatus Visitor::Visit(SPtr<%s> Node)\n"), Curr->Name);
            CPPContent.AppendF(T("{\n"));
            if(IsRoot)
                CPPContent.AppendF(T("    return VS_Continue;\n"));
            else
                CPPContent.AppendF(T("    return Visit(Node.PtrAs<%s>());\n"), Curr->BaseType->Name);
            CPPContent.AppendF(T("}\n\n"));

            Header2.AppendF(T("    virtual EVisitStatus Visit(SPtr<%s> Node);\n"), Curr->Name);
            Header2.AppendF(T("    virtual EVisitStatus BeginVisit(SPtr<%s> Node);\n"), Curr->Name);
            Header2.AppendF(T("    virtual EVisitStatus EndVisit(SPtr<%s> Node);\n\n"), Curr->Name);

            CPP2.AppendF(T("EVisitStatus RecursiveVisitor::Visit(SPtr<%s> Node)\n"), Curr->Name);
            CPP2.AppendF(T("{\n"));
            if (IsRoot)
                CPP2.AppendF(T("    return VS_Continue;\n"));
            else
                CPP2.AppendF(T("    return Visit(Node.PtrAs<%s>());\n"), Curr->BaseType->Name);
            CPP2.AppendF(T("}\n\n"));

            CPP2.AppendF(T("EVisitStatus RecursiveVisitor::BeginVisit(SPtr<%s> Node)\n"), Curr->Name);
            CPP2.AppendF(T("{\n"));
            if (IsRoot)
                CPP2.AppendF(T("    return VS_Continue;\n"));
            else
                CPP2.AppendF(T("    return BeginVisit(Node.PtrAs<%s>());\n"), Curr->BaseType->Name);
            CPP2.AppendF(T("}\n\n"));

            CPP2.AppendF(T("EVisitStatus RecursiveVisitor::EndVisit(SPtr<%s> Node)\n"), Curr->Name);
            CPP2.AppendF(T("{\n"));
            if (IsRoot)
                CPP2.AppendF(T("    return VS_Continue;\n"));
            else
                CPP2.AppendF(T("    return EndVisit(Node.PtrAs<%s>());\n"), Curr->BaseType->Name);
            CPP2.AppendF(T("}\n\n"));



            //Debugger
            HeaderDebugger.AppendF(T("    virtual EVisitStatus Visit(SPtr<%s> Node);\n"), Curr->Name);
            HeaderDebugger.AppendF(T("    virtual EVisitStatus BeginVisit(SPtr<%s> Node);\n"), Curr->Name);
            HeaderDebugger.AppendF(T("    virtual EVisitStatus EndVisit(SPtr<%s> Node);\n\n"), Curr->Name);

            CPPDebugger.AppendF(T("EVisitStatus VisitorWrapper::Visit(SPtr<%s> Node)\n"), Curr->Name);
            CPPDebugger.AppendF(T("{\n"));
            CPPDebugger.AppendF(T("    OnVisit(Node);\n"));
            CPPDebugger.AppendF(T("    EVisitStatus Ret = WrappedVisitor->Visit(Node);\n"));
            CPPDebugger.AppendF(T("    OnVisitFinish(Node, Ret);\n"));
            CPPDebugger.AppendF(T("    return Ret;\n"));
            CPPDebugger.AppendF(T("}\n\n"));

            CPPDebugger.AppendF(T("EVisitStatus VisitorWrapper::BeginVisit(SPtr<%s> Node)\n"), Curr->Name);
            CPPDebugger.AppendF(T("{\n"));
            CPPDebugger.AppendF(T("    OnBeginVisit(Node);\n"));
            CPPDebugger.AppendF(T("    EVisitStatus Ret = WrappedVisitor->BeginVisit(Node);\n"));
            CPPDebugger.AppendF(T("    OnBeginVisitFinish(Node, Ret);\n"));
            CPPDebugger.AppendF(T("    return Ret;\n"));
            CPPDebugger.AppendF(T("}\n\n"));

            CPPDebugger.AppendF(T("EVisitStatus VisitorWrapper::EndVisit(SPtr<%s> Node)\n"), Curr->Name);
            CPPDebugger.AppendF(T("{\n"));
            CPPDebugger.AppendF(T("    OnEndVisit(Node);\n"));
            CPPDebugger.AppendF(T("    EVisitStatus Ret = WrappedVisitor->EndVisit(Node);\n"));
            CPPDebugger.AppendF(T("    OnEndVisitFinish(Node, Ret);\n"));
            CPPDebugger.AppendF(T("    return Ret;\n"));
            CPPDebugger.AppendF(T("}\n\n"));
        }


        HeaderContent.Append (T("};\n\n"));
        Header2.Append(Header2_NonLeaf.CStr());
        Header2.Append (T("};\n"));

        HeaderDebugger.Append(T("};\n"));
        

        HeaderContent.Append(Header2.CStr());
        HeaderContent.Append(HeaderDebugger);
        HeaderContent.Append (T("}\n"));

        CPP2.Append(CPP2_NonLeaf.CStr());
        CPPContent.Append(CPP2.CStr());



        CPPContent.Append(CPPDebugger);

        CPPContent.AppendF(T("}\n"));

    }


    void WriteVisitor(OLString& HeaderContent, OLString& CPPContent)
    {
        HeaderContent.Append(T("#pragma once\n"));
        HeaderContent.Append(T("#include \"Common.h\"\n"));
        HeaderContent.Append(T("#include \"ExpressionDecl.h\"\n"));
        HeaderContent.Append(T("#include \"StatementDecl.h\"\n"));
        HeaderContent.Append(T("#include \"ScopeDecl.h\"\n\n"));
        HeaderContent.Append(T("namespace OL \n{\n"));

        HeaderContent.Append(T("enum EVisitStatus\n"));
        HeaderContent.Append(T("{\n"));
        HeaderContent.Append(T("    VS_Continue,\n"));
        HeaderContent.Append(T("    VS_ContinueParent,\n"));
        HeaderContent.Append(T("    VS_Stop\n"));
        HeaderContent.Append(T("};\n\n"));

        HeaderContent.AppendF(T("class ABase;\n"));
        HeaderContent.AppendF(T("class Visitor\n"));
        HeaderContent.Append (T("{\n"));
        HeaderContent.Append (T("public:\n"));


        CPPContent.AppendF(T("#include \"Common.h\"\n"));
        CPPContent.AppendF(T("#include \"VisitorBase.h\"\n"));
        CPPContent.Append(T("#include \"ExpressionInclude.h\"\n"));
        CPPContent.Append(T("#include \"StatementInclude.h\"\n"));
        CPPContent.Append(T("#include \"ScopeInclude.h\"\n\n"));

        CPPContent.Append(T("namespace OL \n{\n"));

        OLString Header2;
        OLString CPP2;

        OLString Header2_NonLeaf = T("\n");
        OLString CPP2_NonLeaf = T("\n");;

        Header2.AppendF(T("class RecursiveVisitor : public Visitor\n"));
        Header2.Append (T("{\n"));
        Header2.Append (T("public:\n"));


        OLString HeaderDebugger;
        OLString CPPDebugger;

        HeaderDebugger.Append(T("class VisitorWrapper : public RecursiveVisitor\n"));
        HeaderDebugger.Append(T("{\n"));
        HeaderDebugger.Append(T("public:\n"));
        HeaderDebugger.Append(T("    RecursiveVisitor* WrappedVisitor;\n"));
        HeaderDebugger.Append(T("    virtual void OnVisit(ABase* Node);\n"));
        HeaderDebugger.Append(T("    virtual void OnBeginVisit(ABase* Node);\n"));
        HeaderDebugger.Append(T("    virtual void OnEndVisit(ABase* Node);\n"));

        CPPDebugger.Append(T("void VisitorWrapper::OnVisit(ABase* Node) {} \n"));
        CPPDebugger.Append(T("void VisitorWrapper::OnBeginVisit(ABase* Node) {} \n"));
        CPPDebugger.Append(T("void VisitorWrapper::OnEndVisit(ABase* Node) {} \n"));

        OLList<TypeNodeInfo> AllTypes;
        TranversSubClass(&(ABase::RTTI), AllTypes, false);
        //[&HeaderContent, &CPPContent, &Header2, &CPP2] (const TypeInfo* Curr, bool IsLeaf)
        for(int i = AllTypes.Count() - 1; i >= 0; i--)
        {
            TypeInfo* Curr = AllTypes[i].Type;
            bool IsLeaf = AllTypes[i].IsLeaf;

            //bool NeedCallBase  = Curr->BaseType == &ABase::RTTI || Curr == &ABase::RTTI;
            bool IsRoot  = ( Curr == &ABase::RTTI);

            HeaderContent.AppendF(T("    virtual EVisitStatus Visit(%s* Node);\n"), Curr->Name);
            CPPContent.AppendF(T("EVisitStatus Visitor::Visit(%s* Node)\n"), Curr->Name);
            CPPContent.AppendF(T("{\n"));
            if(IsRoot)
                CPPContent.AppendF(T("    return VS_Continue;\n"));
            else
                CPPContent.AppendF(T("    return Visit(Node->As<%s>());\n"), Curr->BaseType->Name);
            CPPContent.AppendF(T("}\n\n"));

            Header2.AppendF(T("    virtual EVisitStatus Visit(%s* Node);\n"), Curr->Name);
            Header2.AppendF(T("    virtual EVisitStatus BeginVisit(%s* Node);\n"), Curr->Name);
            Header2.AppendF(T("    virtual EVisitStatus EndVisit(%s* Node);\n\n"), Curr->Name);

            CPP2.AppendF(T("EVisitStatus RecursiveVisitor::Visit(%s* Node)\n"), Curr->Name);
            CPP2.AppendF(T("{\n"));
            if (IsRoot)
                CPP2.AppendF(T("    return VS_Continue;\n"));
            else
                CPP2.AppendF(T("    return Visit(Node->As<%s>());\n"), Curr->BaseType->Name);
            CPP2.AppendF(T("}\n\n"));

            CPP2.AppendF(T("EVisitStatus RecursiveVisitor::BeginVisit(%s* Node)\n"), Curr->Name);
            CPP2.AppendF(T("{\n"));
            if (IsRoot)
                CPP2.AppendF(T("    return VS_Continue;\n"));
            else
                CPP2.AppendF(T("    return BeginVisit(Node->As<%s>());\n"), Curr->BaseType->Name);
            CPP2.AppendF(T("}\n\n"));

            CPP2.AppendF(T("EVisitStatus RecursiveVisitor::EndVisit(%s* Node)\n"), Curr->Name);
            CPP2.AppendF(T("{\n"));
            if (IsRoot)
                CPP2.AppendF(T("    return VS_Continue;\n"));
            else
                CPP2.AppendF(T("    return EndVisit(Node->As<%s>());\n"), Curr->BaseType->Name);
            CPP2.AppendF(T("}\n\n"));



            //Debugger
            HeaderDebugger.AppendF(T("    virtual EVisitStatus Visit(%s* Node);\n"), Curr->Name);
            HeaderDebugger.AppendF(T("    virtual EVisitStatus BeginVisit(%s* Node);\n"), Curr->Name);
            HeaderDebugger.AppendF(T("    virtual EVisitStatus EndVisit(%s* Node);\n\n"), Curr->Name);

            CPPDebugger.AppendF(T("EVisitStatus VisitorWrapper::Visit(%s* Node)\n"), Curr->Name);
            CPPDebugger.AppendF(T("{\n"));
            CPPDebugger.AppendF(T("    OnVisit(Node);\n"));
            CPPDebugger.AppendF(T("    return WrappedVisitor->Visit(Node);\n"));
            CPPDebugger.AppendF(T("}\n\n"));

            CPPDebugger.AppendF(T("EVisitStatus VisitorWrapper::BeginVisit(%s* Node)\n"), Curr->Name);
            CPPDebugger.AppendF(T("{\n"));
            CPPDebugger.AppendF(T("    OnBeginVisit(Node);\n"));
            CPPDebugger.AppendF(T("    return WrappedVisitor->BeginVisit(Node);\n"));
            CPPDebugger.AppendF(T("}\n\n"));

            CPPDebugger.AppendF(T("EVisitStatus VisitorWrapper::EndVisit(%s* Node)\n"), Curr->Name);
            CPPDebugger.AppendF(T("{\n"));
            CPPDebugger.AppendF(T("    OnEndVisit(Node);\n"));
            CPPDebugger.AppendF(T("    return WrappedVisitor->EndVisit(Node);\n"));
            CPPDebugger.AppendF(T("}\n\n"));
        }


        HeaderContent.Append (T("};\n\n"));
        Header2.Append(Header2_NonLeaf.CStr());
        Header2.Append (T("};\n"));

        HeaderDebugger.Append(T("};\n"));
        

        HeaderContent.Append(Header2.CStr());
        HeaderContent.Append(HeaderDebugger);
        HeaderContent.Append (T("}\n"));

        CPP2.Append(CPP2_NonLeaf.CStr());
        CPPContent.Append(CPP2.CStr());



        CPPContent.Append(CPPDebugger);

        CPPContent.AppendF(T("}\n"));

    }

// #define MFSLIDER(p1, p2, ...) p1
// #define MFFLAG(...) MFSLIDER( __VA_ARGS__, 0, ...)

    template<int X=0> struct TestX { enum { Value = X}; };
    //template<> struct TestX { enum { Value = 0}; };
    virtual int Run()
    {
        // int a = MFFLAG();
        // int b = MFFLAG(11);
        // int a = TestX<12>::Value;
        // int b = TestX<>::Value;
        // return;
        OLString VisitorHeader(T("F:/OtherProj/OLua/src/Semantics/VisitorBase.h"));
        OLString VisitorCPP(T("F:/OtherProj/OLua/src/Semantics/VisitorBase.cpp"));

        OLString HeaderContent;
        OLString CPPContent;

        WriteVisitorSPtr(HeaderContent, CPPContent);

        FILE* HeaderFile = fopen(T2A(VisitorHeader.CStr()), "wb");
        fwrite(HeaderContent.CStr(), HeaderContent.Len(), 1, HeaderFile);
        fclose(HeaderFile);

        FILE* CPPFile = fopen(T2A(VisitorCPP.CStr()), "wb");
        fwrite(CPPContent.CStr(), CPPContent.Len(), 1, CPPFile);
        fclose(CPPFile);

        VERBOSE(LogMisc, T("Write success!\n"));
        return 0;
    };
};

REGISTER_ACTION(GenDefaultVisitor)

}