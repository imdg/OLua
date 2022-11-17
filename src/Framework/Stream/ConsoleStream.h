#pragma once

#include "Common.h"
#include "BaseStream.h"

namespace OL
{

class ConsoleStream : public BaseStream
{
public:
    virtual void Write(byte* InBuffer, int InSize) override;
    virtual void WriteText(const TCHAR* InBuffer) override;
    virtual void Flush() override;
};






}