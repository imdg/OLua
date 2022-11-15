#include "TextSerializer.h"
#include "ConsoleStream.h"
#include "OLString.h"
#include "OLMap.h"
#include "AstCommon.h"
#include "Rtti.h"
#include "TypeDescBase.h"
#include "OLLinkedList.h"

namespace OL
{

#define IDTCall(x) {AddIndent(); x; DecreaseIndent(); }
#define ValOffset(ptr, off) ((byte*)ptr + off)

bool TextSerializer::IsAlwaysShort(TypeInfo* Type)
{
    // if(OLString::StrCmp(Type->Name, T("OLTypeInfo")) == 0)
    //     return true;
    return false;
}

void TextSerializer::WriteShortRTTI(void* Data, TypeInfo* Type)
{
    // if(OLString::StrCmp(Type->Name, T("OLTypeInfo")) == 0 )
    // {
    //     OLTypeInfo* RealData = (OLTypeInfo*)Data;
    //     Output->WriteFormat(T("%s"), ENUM_VALUE_TEXT(ETypeCat, RealData->TypeCat));
    //     if(RealData->TypeCat == TC_Class || RealData->TypeCat == TC_Interface || RealData->TypeCat == TC_Enum)
    //         Output->WriteFormat(T(" (%s)"), RealData->TypeName.CStr());
    //     else if(RealData->TypeCat == TC_IntrinsicType)
    //         Output->WriteFormat(T(" (%s)"), ENUM_VALUE_TEXT(EIntrinsicType, RealData->IntrType));

    // }

    if(Type->IsKindOf(TypeDescBase::RTTI))
    {
        TypeDescBase* Obj = (TypeDescBase*)Data;
        Output->WriteText(Obj->ToString().CStr());
    }
    else if(Type->IsKindOf(ABase::RTTI))
    {
        ABase* Obj = (ABase*)Data;
        Output->WriteFormat(T("%s(%d, %d)"), Obj->GetType().Name, Obj->Line.Line, Obj->Line.Col);
    }
}

TextSerializer::TextSerializer() : NeedDeleteStream(true),  Indent(0)
{
    Output = new ConsoleStream();

}

TextSerializer::TextSerializer(BaseStream* InStream): NeedDeleteStream(false),  Indent(0)
{
    Output = InStream;
}

TextSerializer::~TextSerializer()
{
    if(NeedDeleteStream)
        delete Output;
}


void TextSerializer::WriteRTTI(void* Data, TypeInfo* Type)
{
    Output->WriteFormat(T("%s\n"), Type->Name);
    while(Type != nullptr)
    {
        TypeMember *Member = Type->Members;
        while (Member->Type.UniqVal != 0)
        {
            IDTCall(WriteMember(Data, Type, Member))
                Member++;
        }
        Type = (TypeInfo*)Type->BaseType;
    }
}
void TextSerializer::WriteBaseType(void* Data, TypeID InType)
{
    if(InType.IsBaseType())
    {
        switch (InType.BasicType)
        {
            case RT_bool:       Output->WriteFormat(T("%s"),     *(bool*)Data == true ? T("true") : T("false")); break;
            case RT_int:        Output->WriteFormat(T("%d"),    *(int*)Data); break;
            case RT_uint:       Output->WriteFormat(T("%ud"),   *(uint*)Data); break;
            case RT_int64:      Output->WriteFormat(T("%lld"),  *(int64*)Data); break;
            case RT_uint64:     Output->WriteFormat(T("%ulld"),    *(uint64*)Data); break;
            case RT_float:      Output->WriteFormat(T("%f"), *(float*)Data); break;
            case RT_double:     Output->WriteFormat(T("%lf"), *(double*)Data); break;
            case RT_tchar:      Output->WriteFormat(T("%c(%d)"), *(TCHAR*)Data, int(*(TCHAR*)Data)); break;
            case RT_OLString:   Output->WriteFormat(T("%s"), ((OLString*)Data)->CStr()); break;
            default:
                Output->WriteFormat(T("Unknown(%d)"), (int)InType.BasicType);
        }
    }
    else if(InType.IsRTTIEnum())
    {
        Output->WriteFormat(T("%s"), InType.EnumGetter(*(int*)Data, EGF_ValueText));
    }
}


void TextSerializer::WriteMember(void* Data, TypeInfo* Type, TypeMember* Member)
{
    void* MemberData = nullptr;
    if(Member->IsPointer)
    {   
        const TCHAR* TypeName = Member->Type.Name();
        //MemberData = *((void**)(ValOffset(Data, Member->Offset)));
        MemberData = Member->ElemPtrGetter->Get(ValOffset(Data, Member->Offset));
        Output->WriteFormat(T("%s%s* %s = (0x%llx) : "), GetIndent(),  Member->Type.Name(), Member->Name, (uint64)MemberData );

        if(MemberData == nullptr)
        {
            Output->WriteFormat(T("(nullptr)\n"));
            return;
        }
    }
    else
    {
        const TCHAR* TypeName = Member->Type.Name();
        Output->WriteFormat(T("%s%s %s = "), GetIndent(), Member->Type.Name(), Member->Name);
        MemberData = ValOffset(Data, Member->Offset);
    }
    bool NeedNewLine = true;
    if(Member->ContainerType == TypeMember::EContainerType::CT_None)
    {   
        if(Member->Flags & MF_External)
        {
            Output->WriteFormat(T("(external) "));
            if(Member->Type.IsRTTI())
            {
                TypeInfo* RealTypeInfo = Member->Type.RTTI->GetRealRTTI(MemberData);
                WriteShortRTTI(MemberData, RealTypeInfo);
            }
        }
        else
        {
            if (!Member->Type.IsRTTI())
            {
                WriteBaseType(MemberData, Member->Type);
            }
            else
            {
                TypeInfo* RealTypeInfo = Member->Type.RTTI->GetRealRTTI(MemberData);
                if(IsAlwaysShort(RealTypeInfo))
                {
                    WriteShortRTTI(MemberData, RealTypeInfo);
                }
                else
                {
                    WriteRTTI(MemberData, RealTypeInfo);
                    NeedNewLine = false;
                }
            }
        }
    }
    else if(Member->ContainerType == TypeMember::EContainerType::CT_OLList)
    {
        WriteOLList(MemberData, Member->Type, Member->IsElementPointer, Member->ElemPtrGetter, (Member->Flags & MF_External)? true:false);
        if(Member->Type.IsRTTI())
            NeedNewLine = false;
    }
    else if(Member->ContainerType == TypeMember::EContainerType::CT_OLMap)
    {
        WriteOLMap(MemberData
            , Member->KeyType
            , Member->Type
            , Member->IsKeyPointer
            , Member->IsElementPointer
            , Member->KeyPtrGetter
            , Member->ElemPtrGetter
            , (Member->Flags & MF_KeyExternal)? true:false
            , (Member->Flags & MF_External)? true:false);

        if(Member->Type.IsRTTI())
            NeedNewLine = false;
    }

    if(NeedNewLine)
        Output->WriteText(T("\n"));

}

void TextSerializer::WriteOLList(void* Data, TypeID Type, bool IsElementPointer, PtrGetterBase* ElemGetter, bool IsExternal)
{
    OLListBase* BaseList = (OLListBase*)Data;
    if(!Type.IsRTTI())
    {
        int Count = BaseList->GetDataCount();
        int Stride = BaseList->GetDataStride();
        const void* ListData = BaseList->GetBuffer();
        Output->WriteText(T("{"));
        for(int i = 0; i < Count; i++)
        {
            if(IsElementPointer)
            {
                //void* ElementData = *(void**)((byte*)ListData + Stride * i);
                void* ElementData = ElemGetter->Get((void*)((byte*)ListData + Stride * i));

                if(ElementData == nullptr)
                {
                    Output->WriteText(T("(nullptr)"));
                }
                else
                {
                    WriteBaseType(ElementData, Type);
                }
            }
            else
            {
                void* ElementData = (byte*)ListData + Stride * i;
                WriteBaseType(ElementData, Type);
            }
            if(i != Count - 1)
                Output->WriteText(T(", "));
        }
        Output->WriteText(T("}"));
    }
    else
    {
        int Count = BaseList->GetDataCount();
        int Stride = BaseList->GetDataStride();
        const void* ListData = BaseList->GetBuffer();
        Output->WriteFormat(T("\n%s{\n"), GetIndent());
        AddIndent();
        for(int i = 0; i < Count; i++)
        {
            Output->WriteFormat(T("%s[%d] = "), GetIndent(), i);
            if(IsElementPointer)
            {
                //void* ElementData = *(void**)((byte*)ListData + Stride * i);
                void* ElementData = ElemGetter->Get((void*)((byte*)ListData + Stride * i));
                if(ElementData == nullptr)
                    Output->WriteText(T("(nullptr)\n"));
                else
                {
                    Output->WriteFormat(T("(0x%llx) : "), (uint64)ElementData);
                    TypeInfo* RealRTTI = Type.RTTI->GetRealRTTI(ElementData);
                    if(IsExternal)
                    {
                        Output->WriteFormat(T("(external) "));
                        WriteShortRTTI(ElementData, RealRTTI);
                        Output->WriteText(T("\n"));
                    }
                    else
                    {
                        if(IsAlwaysShort(RealRTTI))
                            WriteShortRTTI(ElementData, RealRTTI);
                        else
                            WriteRTTI(ElementData, RealRTTI);
                    }
                }
            }
            else
            {
                void* ElementData = (byte*)ListData + Stride * i;
                Output->WriteFormat(T("(0x%llx) : "), (uint64)ElementData);
                TypeInfo* RealRTTI = Type.RTTI->GetRealRTTI(ElementData);
                if(IsExternal)
                {
                    Output->WriteFormat(T("(external)\n"));
                    WriteShortRTTI(ElementData, RealRTTI);
                }
                else
                {
                    if(IsAlwaysShort(RealRTTI))
                        WriteShortRTTI(ElementData, RealRTTI);
                    else
                        WriteRTTI(ElementData, RealRTTI);
                }
            }
        }
        DecreaseIndent();
        Output->WriteFormat(T("%s}\n"), GetIndent());
    }
}

void TextSerializer::WriteOLLinkedList(void* Data, TypeID Type, bool IsElementPointer, PtrGetterBase* ElemGetter, bool IsExternal)
{
    OLLinkedListBase* BaseList = (OLLinkedListBase*)Data;
    if(!Type.IsRTTI())
    {
        OLLinkedListBase::BaseIterator* It = BaseList->BeginBaseIter();
        Output->WriteText(T("{"));
        while(It->IsValid())
        {
            if(IsElementPointer)
            {
                
                void* ElementData = ElemGetter->Get(It->GetCurrPtr());

                if(ElementData == nullptr)
                {
                    Output->WriteText(T("(nullptr)"));
                }
                else
                {
                    WriteBaseType(ElementData, Type);
                }
            }
            else
            {
                void* ElementData = It->GetCurrPtr();
                WriteBaseType(ElementData, Type);
            }
            if(It->Next())
                Output->WriteText(T(", "));
        }
        Output->WriteText(T("}"));
    }
    else
    {
        OLLinkedListBase::BaseIterator* It = BaseList->BeginBaseIter();
        Output->WriteFormat(T("\n%s{\n"), GetIndent());
        AddIndent();
        int Index = 0;
        while(It->IsValid())
        {
            Output->WriteFormat(T("%s[%d] = "), GetIndent(), Index++);
            if(IsElementPointer)
            {
                //void* ElementData = *(void**)((byte*)ListData + Stride * i);
                void* ElementData = ElemGetter->Get(It->GetCurrPtr());
                if(ElementData == nullptr)
                    Output->WriteText(T("(nullptr)\n"));
                else
                {
                    Output->WriteFormat(T("(0x%llx) : "), (uint64)ElementData);
                    TypeInfo* RealRTTI = Type.RTTI->GetRealRTTI(ElementData);
                    if(IsExternal)
                    {
                        Output->WriteFormat(T("(external) "));
                        WriteShortRTTI(ElementData, RealRTTI);
                        Output->WriteText(T("\n"));
                    }
                    else
                    {
                        if(IsAlwaysShort(RealRTTI))
                            WriteShortRTTI(ElementData, RealRTTI);
                        else
                            WriteRTTI(ElementData, RealRTTI);
                    }
                }
            }
            else
            {
                void* ElementData = It->GetCurrPtr();
                Output->WriteFormat(T("(0x%llx) : "), (uint64)ElementData);
                TypeInfo* RealRTTI = Type.RTTI->GetRealRTTI(ElementData);
                if(IsExternal)
                {
                    Output->WriteFormat(T("(external)\n"));
                    WriteShortRTTI(ElementData, RealRTTI);
                }
                else
                {
                    if(IsAlwaysShort(RealRTTI))
                        WriteShortRTTI(ElementData, RealRTTI);
                    else
                        WriteRTTI(ElementData, RealRTTI);
                }
            }

            It->Next();

        }
        DecreaseIndent();
        Output->WriteFormat(T("%s}\n"), GetIndent());
    }
}


void TextSerializer::WriteOLMap(void* Data, TypeID KeyType, TypeID ValueType,  bool IsKeyPointer, bool IsElementPointer, PtrGetterBase* KeyGetter, PtrGetterBase* ElemGetter, bool IsKeyExternal, bool IsValueExternal)
{
    auto WriteElement = [this](void* ElemData, TypeID ElemType, bool IsPointer, PtrGetterBase* Getter, bool IsExternal)
    {
        void* RealData = ElemData;
        if(IsPointer)
        {

            //RealData = *(void**)(ElemData);
            RealData = Getter->Get(RealData);

            if(RealData == nullptr)
            {
                Output->WriteText(T("(nullptr)\n"));
                return;
            }
        }

        if(! ElemType.IsRTTI())
        {
            WriteBaseType(RealData, ElemType);
            Output->WriteText("\n");
        }
        else
        {
            Output->WriteFormat(T("(%llx) : "), (uint64)RealData);
            TypeInfo* RealRTTI = ElemType.RTTI->GetRealRTTI(RealData);
            if(IsExternal)
            {
                Output->WriteText(T("(external)\n"));
                WriteShortRTTI(RealData, RealRTTI);
            }
            else
            {  
                if(IsAlwaysShort(RealRTTI))
                    WriteShortRTTI(RealData, RealRTTI);
                else
                    WriteRTTI(RealData, RealRTTI);
            }
            //Output->WriteText("\n");
        }
    };

    OLMapBase* BaseMap = (OLMapBase*)Data;
    OLMapBase::BaseIterator* It = BaseMap->BeginBaseIter();
    Output->WriteFormat(T("\n%s{\n"), GetIndent());
    AddIndent();
    while(!It->BaseIsEnd())
    {
        Output->WriteFormat(T("%sKey = "), GetIndent());
        WriteElement(It->GetKeyPtr(), KeyType, IsKeyPointer, KeyGetter, IsKeyExternal);

        Output->WriteFormat(T("%sValue = "), GetIndent());
        WriteElement(It->GetValuePtr(), ValueType, IsElementPointer, ElemGetter, IsValueExternal);
        It->BaseNext();
    }

    DecreaseIndent();
    BaseMap->FinishBaseIter(It);
    Output->WriteFormat(T("%s}\n"), GetIndent());

}

void TextSerializer::AddIndent()
{
    Indent++;
}
void TextSerializer::DecreaseIndent()
{
    Indent--;
}

static const TCHAR* StaticIndent[] =
{
    "",
    "    ",
    "        ",
    "            ",
    "                ",
    "                    ",
    "                        ",
    "                            ",
    "                                ",
    "                                    ",
    "                                        ",
    "                                            ",
    "                                                ",
    "                                                    ",
    "                                                        ",
    "                                                            ",
    "                                                                ",
    "                                                                    ",
    "                                                                        ",
    "                                                                            ",
    "                                                                                ",
    "                                                                                    ",
};

static int StaticIndentLen = 22;
const TCHAR* TextSerializer::GetIndent()
{
    if(Indent < StaticIndentLen)
        return StaticIndent[Indent];
    else
    {
        OLString IndentText;
        for(int i = 0; i < Indent; i++)
        {
            IndentText += "    ";
        }
        static TCHAR Buffer[512];

        IndentText.Strcpy(Buffer, 512);
        return Buffer;
    }
    
}


}