#include "ConsoleStream.h"
#include <stdio.h>

namespace OL
{

void ConsoleStream::Write(byte* InBuffer, int InSize)
{
    for(int i = 0; i < InSize; i++)
    {
        printf("%x ", (int)InBuffer[i]);
    }
}

void ConsoleStream::WriteText(const TCHAR* InBuffer)
{
    printf("%s", T2A(InBuffer));
}

}