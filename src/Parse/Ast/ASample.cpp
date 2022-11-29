#include "ASample.h"

namespace OL
{

RTTI_BEGIN(ASample)

RTTI_END(ASample)


ASample::ASample()
{
}
    
ASample::~ASample()
{
    
}

EVisitStatus ASample::Accept(Visitor* Vis)
{
    return VS_Continue; //Vis->Visit(SThis);
}

EVisitStatus ASample::Accept(RecursiveVisitor* Vis)
{
    return VS_Continue; // Vis->Visit(SThis);
}

}