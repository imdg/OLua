#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "ABlockStat.h"

namespace OL
{

class ABlock : public ABase
{
    DECLEAR_RTTI()
public:
    ABlock();
    virtual ~ABlock();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    OLList<SPtr<AStat> >    Stats;

};

}
