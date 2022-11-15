#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "ACall.h"

namespace OL
{
class ANormalCall : public ACall
{
    DECLEAR_RTTI()
public:
    ANormalCall();
    virtual ~ANormalCall();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);
};

}
