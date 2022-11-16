#include "BaseStream.h"
#include "stdarg.h"

namespace OL
{
#define BASE_SIZE 512

void BaseStream::WriteFormat(const TCHAR* Format, ...)
{
    TCHAR *Buffer = (TCHAR *)malloc(BASE_SIZE * sizeof(TCHAR));
    int CurrSize = BASE_SIZE;
    int Written = -1;

    va_list ap;
    va_start(ap, Format);
    Written = t_vsnprintf(Buffer, CurrSize, Format, ap);
    va_end(ap);

    while (Written >= CurrSize - 1)
    {
        CurrSize *= 2;
        Buffer = (TCHAR *)realloc(Buffer, CurrSize);
        va_list ap2;
        va_start(ap2, Format);
        Written = t_vsnprintf(Buffer, CurrSize, Format, ap2);
        va_end(ap2);
    }

    WriteText(Buffer);
}

BaseStream::~BaseStream()
{

}



}