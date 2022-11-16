#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "ABlockStat.h"

namespace OL
{
class AExpr;
class ABlock;

struct ElseIfBlockInfo
{
    SPtr<AExpr> Condition;
    SPtr<ABlock> Block;
};
DECLEAR_STRUCT_RTTI(ElseIfBlock)


class AIfStat : public ABlockStat
{
    DECLEAR_RTTI()
public:
    AIfStat();
    virtual ~AIfStat();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);
    
    SPtr<AExpr>             MainCondition;
    SPtr<ABlock>          MainIfBlock;
    OLList<ElseIfBlockInfo>     ElseIfBlocks;
    SPtr<ABlock>     ElseBlock;

};

}
