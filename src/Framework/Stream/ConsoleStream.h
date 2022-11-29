/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

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