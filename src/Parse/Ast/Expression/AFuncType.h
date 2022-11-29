#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "ATypeIdentity.h"

namespace OL
{
class AVarDecl;

class AFuncType : public ATypeIdentity
{
    DECLEAR_RTTI()
public:
    AFuncType();
    virtual ~AFuncType();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    OLList<SPtr<AVarDecl> > Params;
    OLList<SPtr<ATypeIdentity>> ReturnType;

};

}
