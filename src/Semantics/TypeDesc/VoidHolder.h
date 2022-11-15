#pragma once
#include "Common.h"
#include "TypeDescBase.h"
#include "AstCommon.h"

namespace OL
{

class ATypeIdentity;

class VoidHolder : public TypeDescBase
{
    DECLEAR_RTTI();
public:

    virtual ETypeValidation ValidateConvert(SPtr<TypeDescBase> Target, bool IsExplict);
    virtual bool EqualsTo(TypeDescBase* Target);
    virtual OLString ToString();
    virtual bool IsNilable();
    virtual SPtr<TypeDescBase> AcceptBinOp(EBinOp Op, SPtr<TypeDescBase> Target);
    virtual SPtr<TypeDescBase> AcceptUniOp(EUniOp Op);

    static SPtr<VoidHolder> Get();

};
}