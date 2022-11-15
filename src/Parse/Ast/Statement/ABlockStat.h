#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "AStat.h"

namespace OL
{
class ABlock;
class ABlockStat : public AStat
{
    DECLEAR_RTTI()
public:
    ABlockStat();
    virtual ~ABlockStat();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    //virtual int GetBlockCount();
    //virtual OLList< SPtr<AStat> >& GetBlock(int Index);

    //EVisitStatus VisitMainBlock(RecursiveVisitor* Vis);
    static EVisitStatus VisitBlock(OLList<SPtr<AStat> >& Block, RecursiveVisitor* Vis);


    SPtr<ABlock> MainBlock;
};

}
