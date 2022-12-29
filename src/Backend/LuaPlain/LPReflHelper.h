/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/
#pragma once
#include "Common.h"
#include "TextBuilder.h"

namespace OL
{
class ClassType;
class FuncSigniture;
class ArrayType;
class MapType;
class TypeDescBase;
struct ClassMemberDesc;
class EnumType;

class LPReflHelper
{
public:
    static SPtr<TextParagraph> BuildClassRefl(SPtr<ClassType> Class,  bool NamedTypeByRef, TextBuilder& TextOwner);

    static SPtr<TextParagraph> BuildClassMemberType(ClassMemberDesc& Member,  bool NamedTypeByRef, TextBuilder& TextOwner);
    static SPtr<TextParagraph> BuildFunctionType(SPtr<FuncSigniture> Func,  bool NamedTypeByRef, TextBuilder& TextOwner);
    static SPtr<TextParagraph> BuildArrayType(SPtr<ArrayType> Array,  bool NamedTypeByRef, TextBuilder& TextOwner);
    static SPtr<TextParagraph> BuildMapType(SPtr<MapType> Map,  bool NamedTypeByRef, TextBuilder& TextOwner);

    static SPtr<TextParagraph> BuildTypeRefl(SPtr<TypeDescBase> Type,  bool NamedTypeByRef, TextBuilder& TextOwner);
    static SPtr<TextParagraph> BuildEnumType(SPtr<EnumType> Enum, bool NamedTypeByRef, TextBuilder& TextOwner );
};


}