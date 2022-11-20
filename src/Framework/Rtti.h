/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#pragma once
#include "OLString.h"
#include "OLList.h"
#include "OLMap.h"
#include "Pointer.h"
#include "TextExpand.h"
#include "OLLinkedList.h"

#include <functional>

#ifndef ENABLE_RTTI
#define ENABLE_RTTI 0
#endif

#ifndef ENABLE_ENUM_TEXT
#define ENABLE_ENUM_TEXT 0
#endif


namespace OL
{

enum EEnumGetterField
{
    EGF_ValueText,
    EGF_EnumName
};
#if ENABLE_RTTI

typedef const TCHAR* (*EnumTextGetter)(int Val, EEnumGetterField Field);



#if ENABLE_ENUM_TEXT

template< typename T>
class TRTTIEnumInfo
{
public: 
    enum { IsRTTIEnum = false };
    static const TCHAR* GetEnumText(int Val, EEnumGetterField Field) { return T("N/A"); };
};


#define RTTI_ENUM(name, ...) \
enum name { __VA_ARGS__ }; \
template <> \
class TRTTIEnumInfo<name> \
{ \
public: \
    enum { IsRTTIEnum = true }; \
    static const TCHAR* GetEnumText(int Val, EEnumGetterField Field) \
    { \
        if(Field == EGF_EnumName) return T(#name); \
        static const TCHAR* Texts[] = {EXPAND_TEXT(__VA_ARGS__) };\
        int MaxEnumNum = EXPAND_TEXT_NUM(__VA_ARGS__);\
        if(Val >= 0 && Val < MaxEnumNum) \
            return Texts[Val] ; \
        return T("N/A"); \
    }; \
}; 

    // /    static TCHAR* Texts = {EXPAND_TEXT(__VA_ARGS__) };\
// #else


// #define RTTI_ENUM(name, ...) \
// enum name { __VA_ARGS__ };

#define ENUM_VALUE_TEXT(enumtype, value) TRTTIEnumInfo<enumtype>::GetEnumText((int)value, EGF_ValueText)
#endif



enum EBasicType
{
    RT_None,
    RT_bool,
    RT_int,
    RT_uint,
    RT_int64,
    RT_uint64,
    RT_float,
    RT_double,
    RT_tchar,
    RT_OLString,
    RT_UnknownRTTI,
    RT_Max
    
};
class TypeInfo;

struct TypeID
{
    enum TypeCategory
    {
        BaseType,
        RTTIType,
        RTTIEnum
    };
    union 
    {
        union
        {
            EBasicType BasicType;
            TypeInfo* RTTI;
            EnumTextGetter EnumGetter;
        };
        uint64 UniqVal;
    };
    TypeCategory TypeCat;

    TypeID(EBasicType InBasicType)
    {
        UniqVal = 0;
        BasicType = InBasicType;
        TypeCat = TypeCategory::BaseType;
    };

    TypeID(TypeInfo* InRTTI) 
    {
        UniqVal = 0;
        if(InRTTI != nullptr)
        {
            RTTI = InRTTI;
            TypeCat = TypeCategory::RTTIType;
        }
        else
        {
            TypeCat = TypeCategory::BaseType;
            BasicType = RT_UnknownRTTI;
        }
    };

    TypeID(EnumTextGetter InEnumGetter) 
    {
        UniqVal = 0;
        EnumGetter = InEnumGetter;
        TypeCat = TypeCategory::RTTIEnum;
    };


    TypeID() : UniqVal(0)
    {
    }

    const TCHAR* Name();

    bool IsBaseType()
    {
        if(UniqVal < (uint64)RT_Max)
        {
            return true;
        }
        return false;
    };

    bool IsRTTIEnum()
    {
        if(TypeCat == TypeCategory::RTTIEnum)
            return true;
        return false;
    };

    bool IsRTTI()
    {
        return TypeCat == TypeCategory::RTTIType;
    }
};


// ------------ Ptr check
template <typename T> struct TIsPtr { enum {Value = false } ; };
template <typename T> struct TIsPtr< T* > { enum { Value = true };  };
template <typename T> struct TIsPtr< const T* > { enum { Value = true }; };
template <typename T> struct TIsPtr< SPtr<T> > { enum { Value = true };  };
template <typename T> struct TIsPtr< WPtr<T> > { enum { Value = true };  };
//template <typename T> struct TIsPtr< OLList<T> > { enum { Value = TIsPtr<T>::Value }; typedef T* PtType; };


// ---------------- RTTI type check
template <typename T>
struct TIsRTTI
{
    template<typename U> static void Test(decltype(&U::RTTI));
    template<typename U> static int Test(...);
    
    enum { Value = std::is_void<decltype(Test<T>(0))>::value };
};

template <typename T>   struct TIsRTTI<T*>      { enum { Value = TIsRTTI<T>::Value }; };
template <typename T>   struct TIsRTTI<const T*>      { enum { Value = TIsRTTI<T>::Value }; };
template <typename T>   struct TIsRTTI<OLList<T> >      { enum { Value = TIsRTTI<T>::Value }; };
template <typename T>   struct TIsRTTI<OLLinkedList<T> >      { enum { Value = TIsRTTI<T>::Value }; };

template<bool a, bool b> struct TAnd { enum { Value = a&&b }; };


template <typename T> struct TTypeID                { enum { Value = 0}; };
template <typename T> struct TTypeID<T*>            { enum { Value = TTypeID<T>::Value }; };
template <typename T> struct TTypeID<const T*>      { enum { Value = TTypeID<T>::Value }; };
template <typename T> struct TTypeID<OLList<T> >      { enum { Value = TTypeID<T>::Value }; };
template <typename T> struct TTypeID<OLLinkedList<T> >      { enum { Value = TTypeID<T>::Value }; };

template <> struct TTypeID<bool>                     { enum { Value = RT_bool};  };
template <> struct TTypeID<int>                     { enum { Value = RT_int};  };
template <> struct TTypeID<unsigned int>            { enum { Value = RT_uint};  };
template <> struct TTypeID<long long>               { enum { Value = RT_int64};  };
template <> struct TTypeID<unsigned long long>      { enum { Value = RT_uint64};  };
template <> struct TTypeID<float>                   { enum { Value = RT_float};};
template <> struct TTypeID<double>                  { enum { Value = RT_double};  };
template <> struct TTypeID<TCHAR>                   { enum { Value = RT_tchar}; };
template <> struct TTypeID<OLString>                { enum { Value = RT_OLString};  };


template <typename T>
std::enable_if_t< TRTTIEnumInfo<T>::IsRTTIEnum, TypeID > CreateTypeIDImpl()
{
    return TypeID((EnumTextGetter) &TRTTIEnumInfo<T>::GetEnumText);
}


template <typename T>
std::enable_if_t< TAnd<TIsRTTI<T>::Value,  !TRTTIEnumInfo<T>::IsRTTIEnum>::Value, TypeID > CreateTypeIDImpl()
{
    return TypeID(&T::RTTI);
}

template <typename T>
std::enable_if_t< TAnd<!TIsRTTI<T>::Value, !TRTTIEnumInfo<T>::IsRTTIEnum>::Value, TypeID > CreateTypeIDImpl()
{
    static_assert(TTypeID<T>::Value != 0, "This type is neither a base type nor an RTTI type. Unable to create TypeID. If this is a struct with RTTI defined, make sure use RTTI_STRUCT_MEMBER instead of RTTI_MEMBER");
    return TypeID((EBasicType)TTypeID<T>::Value);
}


template <typename T >
struct CreateTypeID
{
    static TypeID Do() { return CreateTypeIDImpl<T>(); }
};

template <typename T>
struct CreateTypeID<T*>
{
    static TypeID Do() { return CreateTypeID<T>::Do() ; }
};

template <typename T>
struct CreateTypeID<const T*>
{
    static TypeID Do() { return CreateTypeID<T>::Do() ; }
};

template <typename T>
struct CreateTypeID<SPtr<T> >
{
    static TypeID Do() { return CreateTypeID<T>::Do() ; }
};

template <typename T>
struct CreateTypeID<const SPtr<T>>
{
    static TypeID Do() { return CreateTypeID<T>::Do() ; }
};

template <typename T>
struct CreateTypeID<WPtr<T> >
{
    static TypeID Do() { return CreateTypeID<T>::Do() ; }
};

template <typename T>
struct CreateTypeID<const WPtr<T>>
{
    static TypeID Do() { return CreateTypeID<T>::Do() ; }
};

template <typename T>
struct CreateTypeID<OLList<T>>
{
    static TypeID Do() { return CreateTypeID<T>::Do() ; }
};

template <typename T>
struct CreateTypeID<OLLinkedList<T>>
{
    static TypeID Do() { return CreateTypeID<T>::Do() ; }
};

template <typename ...T>
struct CreateTypeID<OLMap<T...>>
{
    static TypeID Do() { return CreateTypeID<typename OLMap<T...>::ValueType>::Do() ; }
};

template <typename ...T>
struct CreateKeyTypeID
{
    static TypeID Do() { return TypeID(RT_None) ; }
};
template <typename ...T>
struct CreateKeyTypeID<OLMap<T...>>
{
    static TypeID Do() { return CreateTypeID<typename OLMap<T...>::KeyType>::Do() ; }
};

// -------------Container check
template <typename T> struct TIsArray           { enum { Value = false}; };
template <typename T> struct TIsArray< T[] >    { enum { Value = true}; };

template <typename T> struct TIsList                { enum { Value = false}; };
template <typename T> struct TIsList< OLList<T> >   { enum { Value = true }; };

template <typename T> struct TIsLinkedList                { enum { Value = false}; };
template <typename T> struct TIsLinkedList< OLLinkedList<T> >   { enum { Value = true }; };

template <typename ...T> struct TIsMap               { enum { Value = false}; };
template <typename ...T> struct TIsMap< OLMap<T...> >   { enum { Value = true }; };


template <typename T> struct TIsElementPtr              { enum {Value = false} ;};
template <typename T> struct TIsElementPtr<T[]>         { enum {Value = TIsPtr<T>::Value } ;};
template <typename T> struct TIsElementPtr< OLList<T> > { enum {Value = TIsPtr<T>::Value } ;};
template <typename ...T> struct TIsElementPtr< OLMap<T...> > { enum {Value = TIsPtr<typename OLMap<T...>::ValueType >::Value } ;};

template <typename T> struct TIsKeyPointer                      { enum {Value = false }; };
template <typename ...T> struct TIsKeyPointer< OLMap<T...> >    { enum {Value = TIsPtr<typename OLMap<T...>::KeyType >::Value }; };




class PtrGetterBase
{
public:
    virtual void* Get(void* Src) = 0;
};

template < typename T > 
class PtrGetter : public PtrGetterBase
{
public:
    virtual void* Get(void* Src) { return nullptr; };
};

template < typename T > 
class PtrGetter<T*> : public PtrGetterBase
{
public:
    virtual void* Get(void* Src) { return (void*)(*(T**)Src); };
};

template < typename T > 
class PtrGetter<SPtr<T>> : public PtrGetterBase
{
public:
    virtual void* Get(void* Src) { return (void*)( ((SPtr<T>*)Src)->Get() ); };
};

template < typename T > 
class PtrGetter<WPtr<T>> : public PtrGetterBase
{
public:
    virtual void* Get(void* Src) { return (void*)( ((WPtr<T>*)Src)->Get() ); };
};

template <typename T>
struct GetPtrGetter
{
    static PtrGetterBase* Do()     {  static PtrGetter<T> Getter;   return &Getter;    }
};


template <typename T>
struct GetPtrGetter< OLList<T> >    
{  
    static PtrGetterBase* Do()    {   static PtrGetter<T> Getter;   return &Getter;    }
};

template <typename T>
struct GetPtrGetter< OLLinkedList<T> >    
{  
    static PtrGetterBase* Do()    {   static PtrGetter<T> Getter;   return &Getter;    }
};

template <typename ...T>
struct GetPtrGetter< OLMap<T...> >
{
    static PtrGetterBase* Do()    {   static PtrGetter<typename OLMap<T...>::ValueType > Getter;      return &Getter;   }
};


template <typename T>
struct GetKeyPtrGetter
{
    static PtrGetterBase* Do()     {    return nullptr;    }
};

template <typename ...T>
struct GetKeyPtrGetter<OLMap<T...>>
{
    static PtrGetterBase* Do()     {    static PtrGetter<typename OLMap<T...>::KeyType > Getter;  return &Getter;   }
};


enum EMemberFlag
{
    MF_None = 0,
    MF_External = 1,
    MF_KeyExternal = 2
};

template<int Flag=0> struct MemberFlagger { enum { Value = Flag}; };
class TypeMember
{
public:
    enum EContainerType
    {
        CT_None,
        CT_Array,
        CT_OLList,
        CT_OLLinkedList,
        CT_OLMap
    };
    const TCHAR* Name;
    bool IsBasicType;
    bool IsPointer;
    bool IsElementPointer;
    bool IsKeyPointer;
    int Offset;
    EContainerType ContainerType;
    TypeID Type;
    TypeID KeyType;

    int Flags;
    PtrGetterBase* ElemPtrGetter;
    PtrGetterBase* KeyPtrGetter;

    TypeMember(const TCHAR* InName
    , bool InIsBasicType
    , bool InIsPointer
    , bool InIsElementPointer
    , bool InIsKeyPointer
    , TypeID InType
    , TypeID InKeyType
    , bool IsArray
    , bool IsOLList
    , bool IsMap
    , bool IsLinkedList
    , PtrGetterBase* InElemPtrGetter
    , PtrGetterBase* InKeyPtrGetter
    , int InOffset
    , int InFlags);

    TypeMember();

    void* GetRealAddress(void* Owner);
    void* GetKeyRealAddress(void* Owner);
};

class TypeInfo;
class RealRTTIGetter
{
public:
    virtual TypeInfo* GetTypePtr(void* Obj) =0 ;
};


class TypeInfo
{
public:
    const TCHAR* Name;

    TypeInfo(const TCHAR* InName);
    TypeInfo(const TCHAR* InName, const TypeInfo* InBaseType);

    TypeMember* Members;
    const TypeInfo* BaseType;
    RealRTTIGetter* RTTIGetter;
    bool IsKindOf(TypeInfo& Father);
    TypeInfo* GetRealRTTI(void* Ptr);

    TypeInfo* GlobalNext;
    static TypeInfo* GlobalTypeList;
};




#define DECLEAR_RTTI() \
public:\
    static TypeInfo RTTI;\
    virtual TypeInfo& GetType() { return RTTI; }; \
    virtual TypeInfo* GetTypePtr() { return &RTTI; }; \
    template < typename CheckType > bool IsExactly() { return GetTypePtr() == &CheckType::RTTI; }; \
    template < typename CheckType > bool Is() { return GetType().IsKindOf(CheckType::RTTI); }; \
    template < typename CheckType > CheckType* As() \
    { \
        if(Is<CheckType>()) return static_cast<CheckType*>(this); \
        else return nullptr; \
    }; \

#define RTTI_BEGIN_INHERITED(ClassName, BaseClassName) \
TypeInfo ClassName::RTTI(T(#ClassName), &BaseClassName::RTTI); \
class ReadRTTIGetter##ClassName : public RealRTTIGetter \
{\
public: virtual TypeInfo* GetTypePtr(void* Obj) { return ((ClassName*)Obj)->GetTypePtr(); } \
};\
struct TypeMember##ClassName \
{  \
    typedef ClassName OwnerType;    \
    TypeMember##ClassName() \
    { \
        static TypeMember StaticMembers[] = {


#define RTTI_BEGIN(ClassName) \
TypeInfo ClassName::RTTI(T(#ClassName)); \
class ReadRTTIGetter##ClassName : public RealRTTIGetter \
{\
public: virtual TypeInfo* GetTypePtr(void* Obj) { return ((ClassName*)Obj)->GetTypePtr(); } \
};\
struct TypeMember##ClassName \
{  \
    typedef ClassName OwnerType;    \
    TypeMember##ClassName() \
    { \
        static TypeMember StaticMembers[] = {

#define RTTI_MEMBER(name, ...) \
            TypeMember(T(#name) \
                , !TIsRTTI<decltype(OwnerType::name)>::Value \
                , TIsPtr<decltype(OwnerType::name)>::Value \
                , TIsElementPtr<decltype(OwnerType::name)>::Value \
                , TIsKeyPointer<decltype(OwnerType::name)>::Value \
                , CreateTypeID<decltype(OwnerType::name)>::Do() \
                , CreateKeyTypeID<decltype(OwnerType::name)>::Do() \
                , TIsArray<decltype(OwnerType::name)>::Value \
                , TIsList<decltype(OwnerType::name)>::Value \
                , TIsMap<decltype(OwnerType::name)>::Value \
                , TIsLinkedList<decltype(OwnerType::name)>::Value \
                , GetPtrGetter<decltype(OwnerType::name)>::Do() \
                , GetKeyPtrGetter<decltype(OwnerType::name)>::Do() \
                , (int)(int64)(&((OwnerType*)0)->name) \
                , MemberFlagger<__VA_ARGS__>::Value),



#define RTTI_END(ClassName) \
            TypeMember() };  \
         ClassName::RTTI.Members = StaticMembers; \
         static ReadRTTIGetter##ClassName RTTIGetter; \
         ClassName::RTTI.RTTIGetter = &RTTIGetter; \
     }; \
 }; \
TypeMember##ClassName TypeMember##ClassName##Inst;


#define RTTI_STRUCT_MEMBER_KV(name, struct_name_k, struct_name_v, ... ) \
            TypeMember(T(#name) \
                , !TIsRTTI<decltype(OwnerType::name)>::Value \
                , TIsPtr<decltype(OwnerType::name)>::Value \
                , TIsElementPtr<decltype(OwnerType::name)>::Value \
                , TIsKeyPointer<decltype(OwnerType::name)>::Value \
                , TypeID(&StructTypeRTTI##struct_name_v) \
                , TypeID(&StructTypeRTTI##struct_name_k) \
                , TIsArray<decltype(OwnerType::name)>::Value \
                , TIsList<decltype(OwnerType::name)>::Value \
                , TIsMap<decltype(OwnerType::name)>::Value \
                , TIsLinkedList<decltype(OwnerType::name)>::Value \
                , GetPtrGetter<decltype(OwnerType::name)>::Do() \
                , GetKeyPtrGetter<decltype(OwnerType::name)>::Do() \
                , (int)(int64)(&((OwnerType*)0)->name) \
                , MemberFlagger<__VA_ARGS__>::Value),

#define RTTI_STRUCT_MEMBER_K(name, struct_name_k, ... ) \
            TypeMember(T(#name) \
                , !TIsRTTI<decltype(OwnerType::name)>::Value \
                , TIsPtr<decltype(OwnerType::name)>::Value \
                , TIsElementPtr<decltype(OwnerType::name)>::Value \
                , TIsKeyPointer<decltype(OwnerType::name)>::Value \
                , CreateTypeID<decltype(OwnerType::name)>::Do() \
                , TypeID(&StructTypeRTTI##struct_name_k) \
                , TIsArray<decltype(OwnerType::name)>::Value \
                , TIsList<decltype(OwnerType::name)>::Value \
                , TIsMap<decltype(OwnerType::name)>::Value \
                , TIsLinkedList<decltype(OwnerType::name)>::Value \
                , GetPtrGetter<decltype(OwnerType::name)>::Do() \
                , GetKeyPtrGetter<decltype(OwnerType::name)>::Do() \
                , (int)(int64)(&((OwnerType*)0)->name) \
                , MemberFlagger<__VA_ARGS__>::Value),


#define RTTI_STRUCT_MEMBER_V(name, struct_name_v, ... ) \
            TypeMember(T(#name) \
                , !TIsRTTI<decltype(OwnerType::name)>::Value \
                , TIsPtr<decltype(OwnerType::name)>::Value \
                , TIsElementPtr<decltype(OwnerType::name)>::Value \
                , TIsKeyPointer<decltype(OwnerType::name)>::Value \
                , TypeID(&StructTypeRTTI##struct_name_v) \
                , CreateKeyTypeID<decltype(OwnerType::name)>::Do() \
                , TIsArray<decltype(OwnerType::name)>::Value \
                , TIsList<decltype(OwnerType::name)>::Value \
                , TIsMap<decltype(OwnerType::name)>::Value \
                , TIsLinkedList<decltype(OwnerType::name)>::Value \
                , GetPtrGetter<decltype(OwnerType::name)>::Do() \
                , GetKeyPtrGetter<decltype(OwnerType::name)>::Do() \
                , (int)(int64)(&((OwnerType*)0)->name) \
                , MemberFlagger<__VA_ARGS__>::Value),                


#define RTTI_STRUCT_MEMBER(name, struct_name, ... ) RTTI_STRUCT_MEMBER_V(name, struct_name, ##__VA_ARGS__)


// Struct RTTI
#define DECLEAR_STRUCT_RTTI(ClassName) \
extern TypeInfo StructTypeRTTI##ClassName;

#define STRUCT_RTTI_BEGIN(ClassName) \
TypeInfo StructTypeRTTI##ClassName(T(#ClassName)); \
struct StructTypeMember##ClassName \
{  \
    typedef ClassName OwnerType;    \
    StructTypeMember##ClassName() \
    { \
        static TypeMember StaticMembers[] = {



#define STRUCT_RTTI_END(ClassName) \
            TypeMember() };  \
         StructTypeRTTI##ClassName.Members = StaticMembers; \
     }; \
 }; \
StructTypeMember##ClassName StructTypeMember##ClassName##Inst;




template <typename CheckType>
bool Is(TypeInfo* ObjTypeInfo)
{
    return ObjTypeInfo == &CheckType::RTTI;
}





#else


#define DECLEAR_RTTI() 
#define RTTI_BEGIN_INHERITED(ClassName, BaseClassName) 
#define RTTI_BEGIN(ClassName) 
#define RTTI_MEMBER(name) 
#define RTTI_END(ClassName)
#define RTTI_STRUCT_MEMBER_KV(name, struct_name_k, struct_name_v )
#define RTTI_STRUCT_MEMBER_K(name, struct_name_k )
#define RTTI_STRUCT_MEMBER_V(name, struct_name_v ) 
#define RTTI_STRUCT_MEMBER(name, struct_name )
#define DECLEAR_STRUCT_RTTI(ClassName)
#define STRUCT_RTTI_BEGIN(ClassName)
#define STRUCT_RTTI_END(ClassName)



#endif // ENABLE_RTTI

}
#include "Pointer.inl"