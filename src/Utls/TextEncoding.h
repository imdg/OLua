#pragma once
#ifdef USE_WCHAR
#include <wchar.h>
#endif
#include "OLList.h"


namespace OL
{


int Utf8ToWchar(const char* Src, int SrcSize, wchar_t* Dst, int DstSize);

template <typename CharType>
class StringOp
{
public:
    static int Len(const CharType* Src)
    {
        int Ret = 0;
        CharType* Curr = (CharType*)Src;
        while(*Src != 0)
        {    
            Src++;
            Ret++;
        }
        return Ret;
    }
};

template <typename From, typename To>
class StringConverter
{
public:
    To* Buffer;
    StringConverter(const From* Src)
    {
        int DstSize = StringOp<From>::Len(Src) + 1;
        Buffer = new To[DstSize];
        To* CurrDst = Buffer;
        From* CurrSrc = (From*)Src;
        while(*CurrSrc != 0)
        {
            *(CurrDst++) = (To)*(CurrSrc++);
        }
        *CurrDst = 0;
    };

    ~StringConverter()
    {
        delete[] Buffer;
    };

    To* Get()
    {
        return Buffer;
    }
};

class Utf8ToWcharConverter
{
public:
    wchar_t* Buffer;
    Utf8ToWcharConverter(const char* Src)
    {
        int SrcSize = (StringOp<char>::Len(Src) + 1) ;
        int DstSize = Utf8ToWchar(Src, SrcSize, nullptr, 0);
        if(DstSize == -1)
        {
            Buffer = new wchar_t[4];
            wcscpy(Buffer, L"???");
            return;
        }
        Buffer = new wchar_t[DstSize];
        Utf8ToWchar(Src, SrcSize, Buffer, DstSize);
    };

    ~Utf8ToWcharConverter()
    {
        delete[] Buffer;
    };

    wchar_t* Get()
    {
        return Buffer;
    }
};


class OLString;
class ToUtf8Helper
{
public:
    OLList<char> Buffer;
    ToUtf8Helper(const OLString& Src);
    char* Get();

};

}