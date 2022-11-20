/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "MemTextStream.h"
#include <stdio.h>

namespace OL
{
MemTextStream::MemTextStream() 
{

}
void MemTextStream::Write(byte* InBuffer, int InSize)
{

}

void MemTextStream::WriteText(const TCHAR* InBuffer)
{
    Str.Append(InBuffer);
}

void MemTextStream::Flush()
{}

}