#pragma once
#include "Common.h"
#include "TypeDescBase.h"
#include "AstCommon.h"

namespace OL
{
class ATypeIdentity;

class ConstructorType : public TypeDescBase
{
    DECLEAR_RTTI();
public:


    virtual ETypeValidation ValidateConvert(SPtr<TypeDescBase> Target, bool IsExplict);
    virtual bool EqualsTo(TypeDescBase* Target);
    virtual OLString ToString();

    virtual bool IsNilable();

    ETypeValidation ValidateIterator(SPtr<TypeDescBase> Type1, SPtr<TypeDescBase> Type2);
    virtual SPtr<TypeDescBase> DeduceLValueType(SPtr<SymbolScope> Scope);
};

}