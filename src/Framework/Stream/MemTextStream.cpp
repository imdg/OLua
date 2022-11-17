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