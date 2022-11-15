#include "BuiltinLib.h"
#include "FuncSigniture.h"
#include "IntrinsicType.h"
namespace OL
{



template <typename Ty>
struct OptionalTag
{
    OptionalTag(Ty& In):Val(In) {};
    Ty& Val;
};

template <typename Ty>
inline OptionalTag<Ty> Optional(Ty Val)
{
    return OptionalTag<Ty>(Val);
}

template <typename Ty>
struct VariableTag
{
    VariableTag(Ty& In):Val(In) {};
    Ty& Val;
};
template <typename Ty>
inline VariableTag<Ty> Variable(Ty Val)
{
    return VariableTag<Ty>(Val);
}



template <typename Ty>
struct ConstTag 
{
    ConstTag(Ty& In):Val(In) {};
    Ty& Val;
};

template <typename Ty>
inline ConstTag<Ty> Const(Ty Val)
{
    return ConstTag<Ty>(Val);
}



class FuncSigHelper
{
public:
    SPtr<FuncSigniture> FuncSig;

    FuncSigHelper(SPtr<FuncSigniture> InSig) : FuncSig(InSig)
    {

    };

    template<typename Ty, typename... ArgsType>
    FuncSigHelper& Param(Ty Arg, ArgsType... OtherArgs)
    {
        ParamImpl(Arg, false, false, false);
        Param(OtherArgs...);
        return *this;
    };

    template<typename Ty>
    FuncSigHelper& Param(Ty Arg)
    {
        ParamImpl(Arg, false, false, false);
        return *this;
    };

    FuncSigHelper& Param()
    {
        return *this;
    }

    template<typename Ty2>
    void ParamImpl(Ty2 Arg, bool IsConst, bool IsVariableParam, bool IsOptional)
    {
    };

    template< >
    void ParamImpl(EIntrinsicType Arg, bool IsConst, bool IsVariableParam, bool IsOptional)
    {
        FuncSig->AddParam(Arg, IsConst, IsVariableParam,  IsOptional, CodeLineInfo::Zero );
    };

    template< >
    void ParamImpl(SPtr<TypeDescBase> Arg, bool IsConst, bool IsVariableParam, bool IsOptional)
    {
        FuncSig->AddParam(Arg, IsConst, IsVariableParam,  IsOptional, CodeLineInfo::Zero );
    };

    template<typename Ty >
    void ParamImpl(OptionalTag<Ty> Arg, bool IsConst, bool IsVariableParam, bool IsOptional)
    {
        ParamImpl(Arg.Val, IsConst, IsVariableParam, true);
    };

    template<typename Ty >
    void ParamImpl(VariableTag<Ty> Arg, bool IsConst, bool IsVariableParam, bool IsOptional)
    {
        ParamImpl(Arg.Val, IsConst, true, IsOptional);
    };

    template<typename Ty >
    void ParamImpl(ConstTag<Ty> Arg, bool IsConst, bool IsVariableParam, bool IsOptional)
    {
        ParamImpl(Arg.Val, true, IsVariableParam, IsOptional );
    };





    template<typename Ty2>
    void Return(Ty2 Arg)
    {
        ReturnImpl(Arg);
    };

    template<typename Ty2, typename... OtherRetTypes>
    void Return(Ty2 Arg, OtherRetTypes... OtherRet)
    {
        ReturnImpl(Arg);
        Return(OtherRet...);
    };

    template< typename Ty2 >
    void ReturnImpl(Ty2 Arg)
    {
    };

    template< >
    void ReturnImpl(EIntrinsicType Arg)
    {
        FuncSig->AddReturn(Arg, CodeLineInfo::Zero );
    };

    template< >
    void ReturnImpl(SPtr<TypeDescBase> Arg)
    {
        FuncSig->AddReturn(Arg, CodeLineInfo::Zero );
    };

};




class IntrinsicTypeLibHelper
{
    BuiltinLib& Lib;
    EIntrinsicType Type;
public:
    IntrinsicTypeLibHelper(BuiltinLib& InLib, EIntrinsicType InType) : Lib(InLib), Type(InType)
    {
    };

    IntrinsicMethodInfo& GetMethodsInfo()
    {
        return Lib.Instrinsics[(int)Type];
    }

    FuncSigHelper NewMethod(OLString Name)
    {
        SPtr<FuncSigniture> Func = new FuncSigniture();
        Func->HasThis = true;
        Func->ThisType = IntrinsicType::CreateFromRaw(Type);

        GetMethodsInfo().Methods.Add(Name, Func);

        return FuncSigHelper(Func);
    }

};

#define BEGIN_INTRINSIC(Type) \
{ \
    IntrinsicTypeLibHelper LibHelper(*this, Type); \

#define FUNC(Name) LibHelper.NewMethod(T(Name)).Param
#define AS  .Return
#define END_INTRINSIC  }

void BuiltinLib::Init()
{

    // BEGIN_INTRINSIC(IT_string)
    //     FUNC("find") (IT_string, Optional(IT_int), Optional(IT_bool)) AS (IT_int);
    //     FUNC("len") () ;
    //     FUNC("lower") ();
    //     FUNC("pack") (IT_string, Variable(IT_any)) AS (IT_string);
    //     FUNC("rep") (IT_int, Optional(IT_string)) AS (IT_string);
    //     FUNC("reverse") () AS (IT_string);
    //     FUNC("sub") (IT_int, Optional(IT_int)) AS (IT_string);
    //     FUNC("upper") () AS (IT_string);
    // END_INTRINSIC


}

}