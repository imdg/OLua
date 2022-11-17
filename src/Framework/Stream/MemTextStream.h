#pragma once

#include "Common.h"
#include "BaseStream.h"
#include "OLString.h"

namespace OL
{

class MemTextStream : public BaseStream
{
public:
    MemTextStream();
    virtual void Write(byte* InBuffer, int InSize) override;
    virtual void WriteText(const TCHAR* InBuffer) override;
    virtual void Flush() override;

    OLString Str;
};

}