#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "ACall.h"

namespace OL
{
class AColonCall : public ACall
{
    DECLEAR_RTTI()
public:
    AColonCall();
    virtual ~AColonCall();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    OLString NameAfter;
};

}
