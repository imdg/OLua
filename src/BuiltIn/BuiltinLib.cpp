/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "BuiltinLib.h"
#include "FuncSigniture.h"
#include "IntrinsicType.h"
#include "ClassType.h"
#include "CompileMsg.h"
#include "TupleType.h"
#include "APILoader.h"
#include "Logger.h"

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
struct NilableTag
{
    NilableTag(Ty& In):Val(In) {};
    Ty& Val;
};

template <typename Ty>
inline NilableTag<Ty> Nilable(Ty Val)
{
    return NilableTag<Ty>(Val);
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
        ParamImpl(Arg, false, false, false, false);
        Param(OtherArgs...);
        return *this;
    };

    template<typename Ty>
    FuncSigHelper& Param(Ty Arg)
    {
        ParamImpl(Arg, false, false, false, false);
        return *this;
    };

    FuncSigHelper& Param()
    {
        return *this;
    }

    template<typename Ty2>
    void ParamImpl(Ty2 Arg, bool IsConst, bool IsVariableParam, bool IsOptional, bool IsNilable)
    {
    };

    //template< >
    void ParamImpl(EIntrinsicType Arg, bool IsConst, bool IsVariableParam, bool IsOptional, bool IsNilable)
    {
        FuncSig->AddParam(Arg, IsConst, IsVariableParam,  IsOptional, IsNilable, SourceRange::Zero );
    };

    //template< >
    void ParamImpl(SPtr<TypeDescBase> Arg, bool IsConst, bool IsVariableParam, bool IsOptional, bool IsNilable)
    {
        FuncSig->AddParam(Arg, IsConst, IsVariableParam,  IsOptional, IsNilable, SourceRange::Zero );
    };

    template<typename Ty >
    void ParamImpl(OptionalTag<Ty> Arg, bool IsConst, bool IsVariableParam, bool IsOptional, bool IsNilable)
    {
        ParamImpl(Arg.Val, IsConst, IsVariableParam, true, IsNilable);
    };

    template<typename Ty >
    void ParamImpl(VariableTag<Ty> Arg, bool IsConst, bool IsVariableParam, bool IsOptional, bool IsNilable)
    {
        ParamImpl(Arg.Val, IsConst, true, IsOptional, IsNilable);
    };

    template<typename Ty >
    void ParamImpl(ConstTag<Ty> Arg, bool IsConst, bool IsVariableParam, bool IsOptional, bool IsNilable)
    {
        ParamImpl(Arg.Val, true, IsVariableParam, IsOptional, IsNilable );
    };

    template<typename Ty >
    void ParamImpl(NilableTag<Ty> Arg, bool IsConst, bool IsVariableParam, bool IsOptional, bool IsNilable)
    {
        ParamImpl(Arg.Val, IsConst, IsVariableParam, IsOptional, true );
    };





    template<typename Ty2>
    FuncSigHelper& Return(Ty2 Arg)
    {
        ReturnImpl(Arg, false);
        return *this;
    };

    template<typename Ty2, typename... OtherRetTypes>
    FuncSigHelper& Return(Ty2 Arg, OtherRetTypes... OtherRet)
    {
        ReturnImpl(Arg, false);
        Return(OtherRet...);
        return *this;
    };

    template< typename Ty2 >
    void ReturnImpl(NilableTag<Ty2> Arg, bool IsNilable)
    {
        ReturnImpl(Arg.Val, true);
    };

    template<typename Ty2>
    void ReturnImpl(EIntrinsicType Arg, bool IsNilable)
    {
        FuncSig->AddReturn(Arg, IsNilable, SourceRange::Zero );
    };

    //template< >
    void ReturnImpl(EIntrinsicType Arg, bool IsNilable)
    {
        FuncSig->AddReturn(Arg, IsNilable, SourceRange::Zero );
    };

    //template< >
    void ReturnImpl(SPtr<TypeDescBase> Arg, bool IsNilable)
    {
        FuncSig->AddReturn(Arg, IsNilable, SourceRange::Zero );
    };

};


class TypeLibHelper
{
    SPtr<TypeDescBase> OwnerType;
    SPtr<ClassType> BuiltInClass;
    OLList<SPtr<FuncSigniture>> DefinedMethods;
public:
    TypeLibHelper(EIntrinsicType InType, OLString TypeName) 
    {
        BuiltInClass = new ClassType();
        BuiltInClass->Name = TypeName;
        OwnerType = IntrinsicType::CreateFromRaw(InType);
    };

    TypeLibHelper(OLString TypeName) 
    {
        BuiltInClass = new ClassType();
        BuiltInClass->Name = TypeName;
        OwnerType = IntrinsicType::CreateFromRaw(IT_any);
    };

    OLList<SPtr<FuncSigniture>>& GetDefinedMethods()
    {
        return DefinedMethods;
    }


    SPtr<ClassType> GetClassDesc()
    {
        return BuiltInClass;
    }

    FuncSigHelper Method(OLString Name)
    {
        return MethodImpl(Name, false, false, OwnerType);
    }

    FuncSigHelper StaticMethod(OLString Name)
    {
        return MethodImpl(Name, true, false, OwnerType);
    }

    FuncSigHelper ConstMethod(OLString Name)
    {
        return MethodImpl(Name, false, true, OwnerType);
    }

    FuncSigHelper StaticConstMethod(OLString Name)
    {
        return MethodImpl(Name, true, true, OwnerType);
    }



    FuncSigHelper MethodImpl(OLString Name, bool IsStatic, bool IsConst, SPtr<TypeDescBase> ThisType)
    {
        SPtr<FuncSigniture> Func = new FuncSigniture();
        Func->HasThis = !IsStatic;
        Func->ThisType = ThisType;

        BuiltInClass->AddExtraNormalMethod(Name, Func, IsConst, IsStatic, AT_Public);
        DefinedMethods.Add(Func);
        return FuncSigHelper(Func);
    }

};

// #define BEGIN_INTRINSIC(Type) \
// { \
//     IntrinsicTypeLibHelper LibHelper(*this, Type); \

// #define FUNC(Name) LibHelper.NewMethod(T(Name)).Param
// #define AS  .Return
// #define END_INTRINSIC  }

void BuiltinLib::SaveMethodType(TypeLibHelper& Helper)
{
    OLList<SPtr<FuncSigniture>>& Methods = Helper.GetDefinedMethods();
    for(int i = 0; i < Methods.Count(); i++)
    {
        AddAuxType(Methods[i]);
    }
};

void  BuiltinLib::AddAuxType(SPtr<TypeDescBase> NewType)
{
    if(NewType->ActuallyIs<FuncSigniture>())
    {
        SPtr<FuncSigniture> Func = NewType->ActuallyAs<FuncSigniture>();
        if (Func->Returns.Count() > 1)
        {
            SPtr<TupleType> NewTuple = new TupleType();
            Func->MakeUniqueReturn(NewTuple);
            AuxTypes.Add(NewTuple);
        }
        else
        {
            Func->MakeUniqueReturn(nullptr);
        }
    }

    AuxTypes.Add(NewType);
}

void BuiltinLib::Init()
{
    // int
    TypeLibHelper IntHelper( IT_int, T("int"));
    {
        IntHelper.ConstMethod(T("tostring")).Return(IT_string);
        //IntHelper.Method(T("parse")).Param(IT_string).Return(IT_bool);
    }
    IntClass = IntHelper.GetClassDesc();
    SaveMethodType(IntHelper);

    // float
    TypeLibHelper FloatHelper( IT_float, T("float"));
    {
        FloatHelper.ConstMethod(T("tostring")).Return(IT_string);
        //FloatHelper.Method(T("parse")).Param(IT_string).Return(IT_bool);
        FloatHelper.ConstMethod(T("floor")).Return(IT_int);
        FloatHelper.ConstMethod(T("ceil")).Return(IT_int);
    }
    FloatClass = FloatHelper.GetClassDesc();
    SaveMethodType(FloatHelper);

    // bool
    TypeLibHelper BoolHelper( IT_bool, T("bool"));
    {
        BoolHelper.ConstMethod(T("tostring")).Return(IT_string);
        //BoolHelper.Method(T("parse")).Param(IT_string).Return(IT_bool);
    }
    BoolClass = BoolHelper.GetClassDesc();
    SaveMethodType(BoolHelper);

    // string
    TypeLibHelper StringHelper( IT_string, T("string"));
    {
        StringHelper.ConstMethod(T("tointeger")).Return(Nilable(IT_int));
        StringHelper.ConstMethod(T("tofloat")).Return(Nilable(IT_float));
        StringHelper.ConstMethod(T("tobool")).Return(Nilable(IT_bool));

        StringHelper.ConstMethod(T("len")).Return(IT_int);
        StringHelper.ConstMethod(T("lower")).Return(IT_string);
        StringHelper.ConstMethod(T("upper")).Return(IT_string);
        StringHelper.ConstMethod(T("sub"))
            .Param(IT_int, Optional(IT_int))
            .Return(IT_string);
        StringHelper.ConstMethod(T("getchar"))
            .Param(IT_int)
            .Return(IT_string);
        
    }
    StringClass = StringHelper.GetClassDesc();
    SaveMethodType(StringHelper);

    // enum
    TypeLibHelper EnumHelper(T("enum"));
    {
        SPtr<FuncSigniture> ItemIterFun = new FuncSigniture();
        FuncSigHelper(ItemIterFun)
            .Param(IT_any, IT_string)
            .Return(IT_string, IT_int);
        AddAuxType(ItemIterFun);
            
        EnumHelper.StaticMethod(T("items")).Return(ItemIterFun, IT_any);
        EnumHelper.ConstMethod(T("tostring")).Return(IT_int);
        EnumHelper.StaticMethod(T("parse"))
            .Param(IT_string)
            .Return(IT_int);

    }
    EnumBaseClass = EnumHelper.GetClassDesc();
    SaveMethodType(EnumHelper);

    // array
    TypeLibHelper ArrayHelper(T("array"));
    {
        ArrayHelper.Method(T("add")).Param(IT_any);
        ArrayHelper.Method(T("insert")).Param(IT_int, IT_any);
        ArrayHelper.Method(T("remove_at")).Param(IT_int);
        ArrayHelper.Method(T("clear"));
    }
    ArrayClass = ArrayHelper.GetClassDesc();
    SaveMethodType(ArrayHelper);


    // map
    TypeLibHelper MapHelper(T("map"));;
    {
        MapHelper.Method(T("add")).Param(IT_any, IT_any);
        MapHelper.Method(T("remove")).Param(IT_any);
        MapHelper.Method(T("clear"));
    }
    MapClass = MapHelper.GetClassDesc();
    SaveMethodType(MapHelper);

}

BuiltinLib* BuiltinLib::Inst = nullptr;
BuiltinLib& BuiltinLib::GetInst()
{
    if(Inst == nullptr)
    {
        Inst = new BuiltinLib();
        Inst->Init();
    }

    return *Inst;
}


void BuiltinLib::LoadAPIClasses(APILoader& API)
{
    TypeInfoClass = API.FindAPIClass(T("type_info"));
    if(TypeInfoClass == nullptr)
    {
        ERROR(LogCompile, T("Cannot find class 'type_info' in API files. Reflection will be disabled"));
    }
}

SPtr<ClassType> BuiltinLib::GetTypeInfoClass()
{
    return TypeInfoClass;
}
SPtr<ClassType>   BuiltinLib::GetIntrinsicClass(EIntrinsicType Type)
{
    switch (Type)
    {
    case IT_int:
        return IntClass;
    case IT_float:
        return FloatClass;
    case IT_string:
        return StringClass;    
    case IT_bool:
        return BoolClass; 
    default:
        break;
    }
    return nullptr;
}

SPtr<FuncSigniture> BuiltinLib::GetIntrinsicMethod(EIntrinsicType Type, OLString Name)
{
    SPtr<ClassType> Class = GetIntrinsicClass(Type);
    if(Class != nullptr)
        return GetMethodInClass(Class, Name);
    return nullptr;
}

SPtr<FuncSigniture> BuiltinLib::GetEnumBaseMethod(OLString Name)
{
    return GetMethodInClass(EnumBaseClass, Name);
}
SPtr<FuncSigniture> BuiltinLib::GetClassBaseMethod(OLString Name)
{
    return GetMethodInClass(ClassBaseClass, Name);
}

SPtr<FuncSigniture> BuiltinLib::GetInterfaceBaseMethod(OLString Name)
{
    return GetMethodInClass(InterfaceBaseClass, Name);
}

SPtr<FuncSigniture> BuiltinLib::GetArrayMethod(OLString Name)
{
    return GetMethodInClass(ArrayClass, Name);
}

SPtr<FuncSigniture> BuiltinLib::GetMapMethod(OLString Name)
{
    return GetMethodInClass(MapClass, Name);
}

SPtr<FuncSigniture> BuiltinLib::GetMethodInClass(SPtr<ClassType> Class, OLString Name)
{
    FindMemberResult Result = Class->FindMember(Name, false);
    if(Result.FromClass != nullptr )
    {
        if(Result.FromClass-> DeclTypeDesc != nullptr &&  Result.FromClass-> DeclTypeDesc->Is<FuncSigniture>())
            return Result.FromClass->DeclTypeDesc.Lock().PtrAs<FuncSigniture>();
    }
    return nullptr;
}


}