#include "Common.h"
#include "Rtti.h"
#include "TextExpand.h"
namespace OL
{
    TypeInfo* TypeInfo::GlobalTypeList = nullptr;

    void LinkTypeInfo(TypeInfo* Info)
    {
        Info->GlobalNext = TypeInfo::GlobalTypeList;
        TypeInfo::GlobalTypeList = Info;
    }
    TypeInfo::TypeInfo(const TCHAR* InName): Name(InName), BaseType(nullptr), RTTIGetter(nullptr) 
    {  
        LinkTypeInfo(this);
    };

    TypeInfo::TypeInfo(const TCHAR* InName, const TypeInfo* InBaseType): Name(InName), BaseType(InBaseType), RTTIGetter(nullptr) 
    {
        LinkTypeInfo(this);
    };



    TypeMember::TypeMember(const TCHAR* InName, bool InIsBasicType
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
    , int InFlags)
         : Name(InName)
        , IsBasicType(InIsBasicType)
        , IsPointer(InIsPointer)
        , IsElementPointer(InIsElementPointer)
        , IsKeyPointer(InIsKeyPointer)
        , Type(InType)
        , ElemPtrGetter(InElemPtrGetter)
        , KeyPtrGetter(InKeyPtrGetter)
        , KeyType(InKeyType)
        , Offset(InOffset)
        , Flags(InFlags)
    {
        if(IsArray)
            ContainerType = CT_Array;
        else if(IsOLList)
            ContainerType = CT_OLList;
        else if(IsMap)
            ContainerType = CT_OLMap;
        else if(IsLinkedList)
            ContainerType = CT_OLLinkedList;
        else
            ContainerType = CT_None;
    }

    TypeMember::TypeMember()
        : Name(nullptr)
        , IsBasicType(true)
        , IsPointer(false)
        , IsElementPointer(false)
        , Type(RT_None)
        , KeyType(RT_None)
        , Offset(0)
        , ElemPtrGetter(nullptr)
        , KeyPtrGetter(nullptr)
        , Flags(0)
    {
    }


    bool TypeInfo::IsKindOf(TypeInfo& Father)
    {
        TypeInfo const * Curr = this;
        while (Curr != nullptr)
        {
            if(Curr == &Father)
                return true;
            Curr = Curr->BaseType;
        }
        return false;
    }
    TypeInfo* TypeInfo::GetRealRTTI(void* Ptr)
    {
        if(RTTIGetter == nullptr)
            return this;
        return RTTIGetter->GetTypePtr(Ptr);
    }


    const TCHAR* TypeID::Name()
    {
        if(TypeCat == TypeCategory::BaseType)
        {
            switch (BasicType)
            {
                case RT_bool:       return T("bool");
                case RT_int:        return T("int");
                case RT_uint:       return T("uint");
                case RT_int64:      return T("int64");
                case RT_uint64:     return T("uint64");
                case RT_float:      return T("float");
                case RT_double:     return T("double");
                case RT_tchar:      return T("tchar");
                case RT_OLString:   return T("OLString");
                case RT_UnknownRTTI:   return T("unknown_rtti");
            }
            return T("unknown");
        }
        else if(TypeCat == TypeCategory::RTTIEnum)
        {
            return EnumGetter(0, EGF_EnumName);
        }
        else 
        {
            return RTTI->Name;
        }

    };
}