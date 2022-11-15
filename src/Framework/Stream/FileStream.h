#pragma once

#include "Common.h"
#include "BaseStream.h"
#include <stdio.h>

namespace OL
{

class FileStream : public BaseStream
{
public:
    FileStream();
    virtual void Write(byte* InBuffer, int InSize) override;
    virtual void WriteText(const TCHAR* InBuffer) override;

    void OpenWrite(OLString FileName);
    void Close();

    FILE* FilePtr;
};

}