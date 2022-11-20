/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#pragma once
#include "Common.h"

namespace OL
{

class BaseStream
{
public:

    virtual void Write(byte* InBuffer, int InSize) = 0;
    virtual void WriteText(const TCHAR* InBuffer) = 0;

    virtual void WriteFormat(const TCHAR* Format, ...);
    virtual void Flush() = 0;
    virtual void Close() {};
    virtual ~BaseStream();
};




}