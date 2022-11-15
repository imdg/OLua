#pragma once
#include "Rtti.h"
namespace OL
{

RTTI_ENUM(EIntrinsicType,
    IT_int,
    IT_float,
    IT_string,
    IT_bool,
    IT_nil,
    IT_any,
    IT_max
);

class Token;
RTTI_ENUM(ETypeCat,
    TC_IntrinsicType,
    TC_Class,
    TC_Enum,
    TC_Interface,
    TC_Any,
    TC_None
)

// struct OLTypeInfo
// {
//     ETypeCat TypeCat;
//     EIntrinsicType IntrType;
//     OLString TypeName;

//     OLTypeInfo() : TypeCat(TC_Any), IntrType(IT_nil)
//     { }

//     static OLTypeInfo FromToken(Token& Tk);
    
// };

//DECLEAR_STRUCT_RTTI(OLTypeInfo)


#define RETURN_BY_STATUS(S) { if(S == VS_ContinueParent) return VS_Continue; if(S == VS_Stop) return VS_Stop;}
#define RETURN_IF_STOP(S) { if (S == VS_Stop) return S; }
}