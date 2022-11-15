#include "Common.h"
#include "ABase.h"

namespace OL
{

RTTI_BEGIN(ABase)
    RTTI_STRUCT_MEMBER(Line, CodeLineInfo)
    RTTI_MEMBER(Parent, MF_External)
RTTI_END(ABase)


ABase::ABase() : Parent(nullptr)
{
}
    
ABase::~ABase()
{
    
}

EVisitStatus ABase::Accept(Visitor* Vis)
{
    return Vis->Visit(SThis);
}

EVisitStatus ABase::Accept(RecursiveVisitor* Vis)
{
    return Vis->Visit(SThis);
}

bool ABase::IsParentOf(ABase* Child)
{
    ABase* Parent = Child->Parent.Get();
    while(Parent != nullptr)
    {
        if(Parent == this)
            return true;
        else
            Parent = Parent->Parent.Get();
    }

    return false;
}
        
} // namespace OL
