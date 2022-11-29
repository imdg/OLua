#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "ASimpleExpr.h"

namespace OL
{
class ATypeIdentity;

class ATypeCast : public ASimpleExpr
{
    DECLEAR_RTTI()
public:
    ATypeCast();
    virtual ~ATypeCast();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);
    virtual bool IsConstant();
    
    //OLString TypeName;
    SPtr<ATypeIdentity> TargetType;

    SPtr<ASimpleExpr> CastExpr;
};

}
