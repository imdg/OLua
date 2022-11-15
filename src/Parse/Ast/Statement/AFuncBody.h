#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "ABlockStat.h"

namespace OL
{
class AVarDecl;
class ATypeIdentity;

class ABlock;


class AFuncBody : public ABase
{
    DECLEAR_RTTI()
public:
    AFuncBody();
    virtual ~AFuncBody();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    SPtr<ABlock> MainBlock;
    OLList<SPtr<AVarDecl> > Params;
    OLList<SPtr<ATypeIdentity> > ReturnType;
    bool            IsDeclearOnly;

};

}
