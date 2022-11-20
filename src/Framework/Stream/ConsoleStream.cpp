/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

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