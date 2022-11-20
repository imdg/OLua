/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#pragma once
#include "Common.h"
#include "Rtti.h"
#include "BaseStream.h"


namespace OL
{


class TextSerializer
{
public:
    TextSerializer();
    TextSerializer(BaseStream* InStream);
    virtual ~TextSerializer();

    void WriteRTTI(void* Data, TypeInfo* Type);
    void WriteBaseType(void* Data, TypeID InType);
    void WriteShortRTTI(void* Data, TypeInfo* Type);
    bool IsAlwaysShort(TypeInfo* Type);
    void WriteMember(void* Data, TypeInfo* Type, TypeMember* Member);

    void WriteOLList(void* Data, TypeID Type, bool IsElementPointer, PtrGetterBase* ElemGetter, bool IsExternal);
    void WriteOLLinkedList(void* Data, TypeID Type, bool IsElementPointer, PtrGetterBase* ElemGetter, bool IsExternal);
    void WriteOLMap(void* Data, TypeID KeyType, TypeID ValueType,  bool IsKeyPointer, bool IsElementPointer, PtrGetterBase* KeyGetter, PtrGetterBase* ElemGetter, bool IsKeyExternal, bool IsValueExternal);
    void AddIndent();
    void DecreaseIndent();
    const TCHAR* GetIndent();
private:
    BaseStream* Output;
    bool NeedDeleteStream;
    int Indent;

};



}