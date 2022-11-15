#pragma once
#include "Common.h"
#include "AstCommon.h"
#include "ABase.h"

namespace OL
{
struct AttributeItem
{
    OLString Name;
    EIntrinsicType Type;
    int64      IntVal;
    double     FltVal;
    bool       BoolVal;
    OLString   StrVal;
};
DECLEAR_STRUCT_RTTI(AttributeItem)


class AAttribute : public ABase
{
    DECLEAR_RTTI()
public:
    AAttribute();
    virtual ~AAttribute();

    virtual EVisitStatus Accept(Visitor* Vis);
    virtual EVisitStatus Accept(RecursiveVisitor * Vis);

    void AddString(OLString Name, OLString Val);
    void AddInt(OLString Name, int Val);
    void AddFloat(OLString Name, float Val);
    void AddBool(OLString Name, bool Val);
    void AddNil(OLString Name);

    OLList<AttributeItem> Items;

};

}
