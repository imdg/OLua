/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#pragma once
#include "Common.h"
#include "TextBuilder.h"


namespace OL
{


class ClassType;
class AClassVar;
class AExpr;
class AClassContructor;
class ANormalMethod;
class AVarDecl;
struct ClassMemberDesc;
struct InterfaceMember;
class LPClassHelper
{

public:
    struct DefinitionItem
    {
        SPtr<TextParagraph> Text;
        OLString            MemberName;
    };

    struct CtorInfo
    {
        SPtr<TextParagraph> Wrapper;
        SPtr<TextParagraph> Impl;
    };
    
    struct OperatorOverride
    {
        OLString OpMetaName;
        OLString OpFuncName;
    };
    

    LPClassHelper(SPtr<ClassType> InClass, TextBuilder& InTextOwner);


    void LoadBaseClass(SPtr<LPClassHelper> BaseClassHelper);

    void BuildDefinitionItems();
    void BuildVTableItems();

    OLString CheckOperator(OLString MetaName);
    void BuildOperatorOverride();

    void AddFieldInit(SPtr<AVarDecl> Node, SPtr<AExpr> InitNode, SPtr<TextParagraph> ExprText);
    DefinitionItem* FindDefinition(OLString Name);

    void AddConstructor(SPtr<AClassContructor> Node, SPtr<TextParagraph> ImplText);
    void AddDefaultConstructor();
    void AddNormalMethod(SPtr<ANormalMethod> Node, SPtr<TextParagraph> Text);

    void DoBuild();
    void Prepare();

    void ReplaceBaseDefinition(SPtr<LPClassHelper> Base);



    SPtr<TextParagraph> BuildInstDefBlock();
    SPtr<TextParagraph> BuildInstDefInsideBlock(OLList<DefinitionItem>& Defs);
    SPtr<TextParagraph> BuildStaticDefBlock();

    SPtr<TextParagraph> BuildCtorBlock();
    SPtr<TextParagraph> BuildHiddenCtor();
    SPtr<TextParagraph> BuildStaticCtor();

    static OLString MakeMemberName(SPtr<ClassType> Class, ClassMemberDesc* Member, OLString Suffix);
    
    static OLString MakeConstructorName(SPtr<ClassType> Class, OLString MemberName, OLString Suffix);
    static OLString MakeStaticTableName(SPtr<ClassType> Class, OLString Suffix);
    static OLString MakeSelfName(SPtr<ClassType> Class);

    static OLString InCodeMemberName(ClassMemberDesc* Member);
    static OLString InCodeMemberName(InterfaceMember* Member);
    static OLString InCodeClassName(SPtr<ClassType> Class);
    

    OLList<DefinitionItem> InstancedDefines;
    OLList<DefinitionItem> StaticDefines;
    OLList<SPtr<TextParagraph> > VTableItems;

    OLList<OperatorOverride> OpInfo;

    SPtr<TextParagraph> InstancedDefineCombined;

    OLList<SPtr<TextParagraph> > InstancedInit;
    OLList<SPtr<TextParagraph> > StaticInit;


    //OLList<SPtr<TextParagraph> > ClassInitExprs;

    // OLList<SPtr<TextParagraph> > CtorWrappers;
    // OLList<SPtr<TextParagraph> > CtorImpls;

    OLList<CtorInfo>            Ctors;

    OLList<SPtr<TextParagraph> > Methods;

    SPtr<ClassType>             Class;
    TextBuilder&                TextOwner;

    OLString                    CurrSelfName;

    // if this class does not have hidden ctor, this is the nearest hidden ctor in the inherit chain
    // if this class has hidden ctor, but only calling base ctor, this is still the base ctor name
    // if this class has hidden ctor, doing real initialization, this is its name
    OLString                    SavedHiddenCtorName;

    // hidden ctor of this class.
    // when this is not empty, hidden ctor must be called
    // this is different with SavedHiddenCtorName in case that base ctor must be called but nothing of this class need to do
    OLString                    CurrHiddenCtorName;

    // SPtr<TextParagraph>         CtorBlock;
    // SPtr<TextParagraph>         StaticCtorBlock;

    SPtr<TextParagraph>         MainBlock;
    SPtr<TextParagraph>         StaticBlock;
};


}