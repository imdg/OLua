#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "AStat.h"

namespace OL
{
class AStat;
class ABlock;
class ARoot : public ABase
{
    DECLEAR_RTTI()
public:
    ARoot();
    virtual ~ARoot();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    OLString Name;
    SPtr<ABlock> Block;
};

}
