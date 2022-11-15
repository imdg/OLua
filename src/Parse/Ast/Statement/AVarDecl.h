#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "AStat.h"

namespace OL
{

class ATypeIdentity;

class AVarDecl : public AStat
{
    DECLEAR_RTTI()
public:
    AVarDecl();
    virtual ~AVarDecl();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    OLString VarName;
    bool IsVariableParam;
    SPtr<ATypeIdentity> VarType;
    bool IsConst;
    bool IsOptionalParam;
};

}
