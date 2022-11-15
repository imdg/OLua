#pragma once
#include "Common.h"
#include "ScopeVistorBase.h"
#include "TextBuilder.h"
#include "SymbolTable.h"
#include "LPClassLib.h"
namespace OL
{


class LPClassHelper;
class LuaPlainInterp : public ScopeVisitorBase
{
public:

    struct NodeGen
    {
        NodeGen(SPtr<ABase> InNode, SPtr<TextParagraph> InCurrText);
        SPtr<ABase> Node;
        SPtr<TextParagraph> CurrText;
        SPtr<TextParagraph> ExtraText;
    };


    struct StaticTableInfo
    {
        SPtr<TextParagraph> StaticTableText;
        SPtr<ClassType>     OwnerClass;
    };


    
    OLString CurrSelfName;
    OLList<OLString > SelfNameStack;

    TextBuilder& OutText;

    OLList<NodeGen> ContentStack;
    OLList<int>     IndexStack;

    OLList<SPtr<LPClassHelper> > ClassHelper;

    //Class static blocks that left to be in the global init section
    OLList<StaticTableInfo> PendingStaticTable; 
    OLMap<ClassType*, SPtr<LPClassHelper> > FinishedClass;

    LPClassLib& ClassLib;

    LuaPlainInterp(SymbolTable& InSymbolTable, TextBuilder& InOutText, LPClassLib& InClassLib);
    //OLString MakeMemberName(SPtr<ClassType> Class, OLString MemberName, OLString Suffix);
    //OLString MakeConstructorName(SPtr<ClassType> Class, OLString MemberName, OLString Suffix);
    //OLString MakeVTableName(SPtr<ClassType> Class, OLString Suffix);
    //OLString MakeSelfName(SPtr<ClassType> Class);
    //OLString MakeStaticTableName(SPtr<ClassType> Class, OLString Suffix);

    SPtr<TextParagraph> FromTop(int CurrIndex, int TopIndex);
    NodeGen&            NodeFromTop(int CurrIndex, int TopIndex);
    int                 StackCount(int CurrIndex);
    NodeGen*            FindStack(int CurrIndex, SPtr<ABase> Node);

    void                UpdateStaticTableWrite(SPtr<ClassType> LastClass, SPtr<TextParagraph> CurrText);


    void                MergeStaticBlock();
    SPtr<TextParagraph> MakeStandaloneStaticBlock();

    virtual EVisitStatus BeginVisit(SPtr<AParentheses> Node);
    virtual EVisitStatus EndVisit(SPtr<AParentheses> Node);

    virtual EVisitStatus Visit(SPtr<ASelf> Node);

    virtual EVisitStatus Visit(SPtr<ASuper> Node);


    virtual EVisitStatus Visit(SPtr<AVarRef> Node);


    virtual EVisitStatus Visit(SPtr<AConstExpr> Node);

    virtual EVisitStatus BeginVisit(SPtr<AConstructor> Node);
    virtual EVisitStatus EndVisit(SPtr<AConstructor> Node);





    // to do
    virtual EVisitStatus BeginVisit(SPtr<AFuncExpr> Node);
    virtual EVisitStatus EndVisit(SPtr<AFuncExpr> Node);

    virtual EVisitStatus BeginVisit(SPtr<AColonCall> Node);
    virtual EVisitStatus EndVisit(SPtr<AColonCall> Node);


    virtual EVisitStatus BeginVisit(SPtr<ANormalCall> Node);
    virtual EVisitStatus EndVisit(SPtr<ANormalCall> Node);

    virtual EVisitStatus BeginVisit(SPtr<ABracketMember> Node);
    virtual EVisitStatus EndVisit(SPtr<ABracketMember> Node);

    virtual EVisitStatus BeginVisit(SPtr<ADotMember> Node);
    virtual EVisitStatus EndVisit(SPtr<ADotMember> Node);

    virtual EVisitStatus BeginVisit(SPtr<ATypeCast> Node);
    virtual EVisitStatus EndVisit(SPtr<ATypeCast> Node);


    virtual EVisitStatus BeginVisit(SPtr<ASubexpr> Node);
    virtual EVisitStatus EndVisit(SPtr<ASubexpr> Node);

    virtual EVisitStatus Visit(SPtr<ATypeIdentity> Node);
    virtual EVisitStatus BeginVisit(SPtr<ATypeIdentity> Node);
    virtual EVisitStatus EndVisit(SPtr<ATypeIdentity> Node);


    // virtual EVisitStatus Visit(SPtr<ANamedType> Node);
    // virtual EVisitStatus BeginVisit(SPtr<ANamedType> Node);
    // virtual EVisitStatus EndVisit(SPtr<ANamedType> Node);

    virtual EVisitStatus BeginVisit(SPtr<AClass> Node);
    virtual EVisitStatus EndVisit(SPtr<AClass> Node);


    // virtual EVisitStatus Visit(SPtr<AClassMember> Node);
    // virtual EVisitStatus BeginVisit(SPtr<AClassMember> Node);
    // virtual EVisitStatus EndVisit(SPtr<AClassMember> Node);







    virtual EVisitStatus BeginVisit(SPtr<AClassVar> Node);
    virtual EVisitStatus EndVisit(SPtr<AClassVar> Node);

    virtual EVisitStatus BeginVisit(SPtr<AVarDecl> Node);
    virtual EVisitStatus EndVisit(SPtr<AVarDecl> Node);

    virtual EVisitStatus BeginVisit(SPtr<ARoot> Node);
    virtual EVisitStatus EndVisit(SPtr<ARoot> Node);

    virtual EVisitStatus BeginVisit(SPtr<ABlock> Node);
    virtual EVisitStatus EndVisit(SPtr<ABlock> Node);
    
    virtual EVisitStatus BeginVisit(SPtr<AClassContructor> Node);
    virtual EVisitStatus EndVisit(SPtr<AClassContructor> Node);

    virtual EVisitStatus BeginVisit(SPtr<ANormalMethod> Node);
    virtual EVisitStatus EndVisit(SPtr<ANormalMethod> Node);


    virtual EVisitStatus BeginVisit(SPtr<AFuncBody> Node);
    virtual EVisitStatus EndVisit(SPtr<AFuncBody> Node);

    
    virtual EVisitStatus BeginVisit(SPtr<ALocal> Node);
    virtual EVisitStatus EndVisit(SPtr<ALocal> Node);

    virtual EVisitStatus BeginVisit(SPtr<AGlobal> Node);
    virtual EVisitStatus EndVisit(SPtr<AGlobal> Node);


    virtual EVisitStatus BeginVisit(SPtr<AFuncDef> Node);
    virtual EVisitStatus EndVisit(SPtr<AFuncDef> Node);


    virtual EVisitStatus BeginVisit(SPtr<AAssignment> Node);
    virtual EVisitStatus EndVisit(SPtr<AAssignment> Node);

    
    virtual EVisitStatus BeginVisit(SPtr<ACallStat> Node);
    virtual EVisitStatus EndVisit(SPtr<ACallStat> Node);

    
    virtual EVisitStatus BeginVisit(SPtr<AForList> Node);
    virtual EVisitStatus EndVisit(SPtr<AForList> Node);

    
    virtual EVisitStatus BeginVisit(SPtr<AForNum> Node);
    virtual EVisitStatus EndVisit(SPtr<AForNum> Node);


    virtual EVisitStatus BeginVisit(SPtr<AIfStat> Node);
    virtual EVisitStatus EndVisit(SPtr<AIfStat> Node);


    virtual EVisitStatus BeginVisit(SPtr<ARepeat> Node);
    virtual EVisitStatus EndVisit(SPtr<ARepeat> Node);

        
    virtual EVisitStatus BeginVisit(SPtr<AWhile> Node);
    virtual EVisitStatus EndVisit(SPtr<AWhile> Node);

    virtual EVisitStatus Visit(SPtr<ABreak> Node);

    virtual EVisitStatus Visit(SPtr<AGoto> Node);

    virtual EVisitStatus Visit(SPtr<ALabel> Node);

    virtual EVisitStatus BeginVisit(SPtr<AReturn> Node);
    virtual EVisitStatus EndVisit(SPtr<AReturn> Node);

    virtual EVisitStatus Visit(SPtr<AVariableParamRef> Node);


    //--------- partially to do ------------------------
    virtual EVisitStatus BeginVisit(SPtr<AInterface> Node);
    virtual EVisitStatus EndVisit(SPtr<AInterface> Node);


    //-------------- to do ------------------------

    // virtual EVisitStatus Visit(SPtr<AMethod> Node);
    // virtual EVisitStatus BeginVisit(SPtr<AMethod> Node);
    // virtual EVisitStatus EndVisit(SPtr<AMethod> Node);


    // virtual EVisitStatus Visit(SPtr<AClassDestructor> Node);
    // virtual EVisitStatus BeginVisit(SPtr<AClassDestructor> Node);
    // virtual EVisitStatus EndVisit(SPtr<AClassDestructor> Node);



    // virtual EVisitStatus Visit(SPtr<AEnumItem> Node);
    // virtual EVisitStatus BeginVisit(SPtr<AEnumItem> Node);
    // virtual EVisitStatus EndVisit(SPtr<AEnumItem> Node);

    // virtual EVisitStatus Visit(SPtr<AModifier> Node);
    // virtual EVisitStatus BeginVisit(SPtr<AModifier> Node);
    // virtual EVisitStatus EndVisit(SPtr<AModifier> Node);


    // virtual EVisitStatus Visit(SPtr<AStat> Node);
    // virtual EVisitStatus BeginVisit(SPtr<AStat> Node);
    // virtual EVisitStatus EndVisit(SPtr<AStat> Node);


    // virtual EVisitStatus Visit(SPtr<AEnum> Node);
    // virtual EVisitStatus BeginVisit(SPtr<AEnum> Node);
    // virtual EVisitStatus EndVisit(SPtr<AEnum> Node);



    // virtual EVisitStatus Visit(SPtr<ABlockStat> Node);
    // virtual EVisitStatus BeginVisit(SPtr<ABlockStat> Node);
    // virtual EVisitStatus EndVisit(SPtr<ABlockStat> Node);






};
}