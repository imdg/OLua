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
#ifdef PLATFORM_WIN    
    wprintf(L"%s", T2W(InBuffer));
#elif defined(PLATFORM_MAC) || defined(PLATFORM_LINUX)
    printf("%s", TS2U(OLString(InBuffer)));
#else
    printf("$s", T2A(InBuffer));
#endif
}

void ConsoleStream::Flush()
{
    

}

}