#pragma once
#ifdef USE_WCHAR
#include <wchar.h>
#endif

namespace OL
{
#ifdef USE_WCHAR
typedef wchar_t TCHAR;
#define T(x) L ## x
#define C(x) L ## x

#define T2A(x) (StringConverter<TCHAR, char>(x).Get())
#define A2T(x) (StringConverter<char, TCHAR>(x).Get())


#define T2W(x) x
#define W2T(x) x


#define t_strcmp wcscmp
#define t_strncpy wcsncpy
#define t_strlen wcslen
#define t_vsnprintf vswprintf


#else
typedef char TCHAR;
// Wrapper for all hardcode string
#define T(x) x
#define C(x) x

#define T2A(x) x
#define A2T(x) x

#define T2W(x) (StringConverter<TCHAR, wchar_t>(x).Get())
#define W2T(x) (StringConverter<wchar_t, TCHAR>(x).Get())

#define t_strcmp strcmp
#define t_strncpy strncpy
#define t_strlen strlen
#define t_vsnprintf vsnprintf
#endif

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
            *(++CurrDst) = (To)*(++CurrSrc);
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



}