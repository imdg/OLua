/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#pragma once
#include "Common.h"
#include "FuncSigniture.h"
#include "IntrinsicType.h"
#include "AstCommon.h"

namespace OL
{

class APILoader;
struct IntrinsicMethodInfo
{
    EIntrinsicType Type;
    OLMap<OLString, SPtr<FuncSigniture> > Methods;
};

struct BuildInTypeDesc
{
    SPtr<TypeDescBase> Type;
};
class ClassType;
class TypeLibHelper;
class BuiltinLib
{
public: 
    IntrinsicMethodInfo Instrinsics[IT_max];
    void Init();

    void LoadAPIClasses(APILoader& API);

    OLList<SPtr<TypeDescBase>> AuxTypes;

    SPtr<ClassType>  IntClass;
    SPtr<ClassType>  FloatClass;
    SPtr<ClassType>  StringClass;
    SPtr<ClassType>  BoolClass;

    SPtr<ClassType>  EnumBaseClass;
    SPtr<ClassType>  ClassBaseClass;
    SPtr<ClassType>  InterfaceBaseClass;

    SPtr<ClassType>  ArrayClass;
    SPtr<ClassType>  MapClass;

    SPtr<ClassType>  TypeInfoClass;
 
    static BuiltinLib* Inst;
    static BuiltinLib& GetInst();

    SPtr<ClassType>   GetIntrinsicClass(EIntrinsicType Type);

    void SaveMethodType(TypeLibHelper& Helper);
    void AddAuxType(SPtr<TypeDescBase> NewType);

    SPtr<FuncSigniture> GetIntrinsicMethod(EIntrinsicType Type, OLString Name);
    SPtr<FuncSigniture> GetEnumBaseMethod(OLString Name);
    SPtr<FuncSigniture> GetClassBaseMethod(OLString Name);
    SPtr<FuncSigniture> GetInterfaceBaseMethod(OLString Name);
    SPtr<FuncSigniture> GetArrayMethod(OLString Name);
    SPtr<FuncSigniture> GetMapMethod(OLString Name);

    SPtr<FuncSigniture> GetMethodInClass(SPtr<ClassType> Class, OLString Name);

    SPtr<ClassType>     GetTypeInfoClass();

};

}