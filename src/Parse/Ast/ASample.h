#pragma once
#include "Common.h"
#include "ABase.h"

namespace OL
{

class ASample : public ABase
{
    DECLEAR_RTTI()
public: 
    ASample();
    virtual ~ASample();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);
};

}