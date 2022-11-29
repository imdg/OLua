/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "BaseStream.h"
#include "stdarg.h"
#include "OLString.h"
namespace OL
{
#define BASE_SIZE 512

void BaseStream::WriteFormat(const TCHAR* Format, ...)
{
    TCHAR *Buffer = (TCHAR *)malloc(BASE_SIZE * sizeof(TCHAR));
    int CurrSize = BASE_SIZE;
    int Written = -1;

    TCHAR* RealFormat = (TCHAR*)Format;
#if (defined(PLATFORM_MAC) || defined(PLATFORM_LINUX)) && USE_WCHAR
    OLString TempFormat = Format;
    TempFormat.Replace(T("%s"), T("%ls"));
    RealFormat = (TCHAR*)TempFormat.CStr();
#endif  

    va_list ap;
    va_start(ap, Format);
    Written = t_vsnprintf(Buffer, CurrSize, RealFormat, ap);
    va_end(ap);

    while (Written >= CurrSize - 1)
    {
        CurrSize *= 2;
        Buffer = (TCHAR *)realloc(Buffer, CurrSize);
        va_list ap2;
        va_start(ap2, Format);
        Written = t_vsnprintf(Buffer, CurrSize, RealFormat, ap2);
        va_end(ap2);
    }

    WriteText(Buffer);
}

BaseStream::~BaseStream()
{

}



}