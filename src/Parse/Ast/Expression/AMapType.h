#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "ATypeIdentity.h"

namespace OL
{
class AMapType : public ATypeIdentity
{
    DECLEAR_RTTI()
public:
    AMapType();
    virtual ~AMapType();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    SPtr<ATypeIdentity> KeyType;
    SPtr<ATypeIdentity> ValueType;

};

}
