#include "AstCommon.h"
#include "Lexer.h"
namespace OL
{

// STRUCT_RTTI_BEGIN(OLTypeInfo)
//     RTTI_MEMBER(TypeCat)
//     RTTI_MEMBER(IntrType)
//     RTTI_MEMBER(TypeName)
// STRUCT_RTTI_END(OLTypeInfo)

//  OLTypeInfo OLTypeInfo::FromToken(Token& Tk)
// {
//     OLTypeInfo Ret;
//     Ret.TypeCat = TC_IntrinsicType;
//     switch(Tk.Tk)
//     {
//         case TKT_int32:
//         case TKT_int:
//         case TKT_int64:
//         case TKT_uint32:
//         case TKT_uint:
//         case TKT_uint64:
//         case TKT_byte:
//         case TKT_char:
//             Ret.IntrType = IT_int;
//             return Ret;
//         case TKT_float:
//         case TKT_double:
//             Ret.IntrType = IT_float;
//             return Ret;
//         case TKT_bool:
//             Ret.IntrType = IT_bool;
//             return Ret;
//         case TKT_any:
//             Ret.TypeCat = TC_Any;
//             return Ret;
//         case TK_name:
//             Ret.TypeCat = TC_Class;
//             Ret.TypeName = Tk.StrOrNameVal;
//             return Ret;
//     }
//     Ret.TypeCat = TC_None;
//     return Ret;
// }

}