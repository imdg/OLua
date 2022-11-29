/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

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
    virtual void Close() override;
    virtual void Flush() override;

    FILE* FilePtr;
};

}