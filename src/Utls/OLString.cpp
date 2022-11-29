/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include <utility>
#include <stdarg.h>
#include <stdlib.h>
#include <codecvt>
#include <locale> 
#include <string.h>

#include "OLString.h"
#include "OLList.h"
namespace OL
{
    extern int OL_vswprintf(wchar_t* const Dst, size_t const DstSize, wchar_t const* const Fmt, va_list Args);
    
    OLString::OLString()
    {
    }

    OLString::OLString(const TCHAR* InStr) : InnerStr(InStr)
    {

    }
    OLString::OLString(const TCHAR* InStr, int Len) : InnerStr(InStr, (size_t)Len)
    {

    }
    OLString::OLString(const OLString& InSrc) : InnerStr(InSrc.InnerStr)
    {

    }
    OLString::OLString(const OLString&& InSrc)
    {
        InnerStr = std::move(InSrc.InnerStr);
    }

    int OLString::Len() const
    {
        return (int)InnerStr.length();
    }

    OLString& OLString::Append(const TCHAR* InStr)
    {
        InnerStr.append(InStr);
        return *this;
    }

    OLString& OLString::Append(const OLString& InStr)
    {
        InnerStr.append(InStr.InnerStr);
        return *this;
    }

    OLString& OLString::AppendCh(const TCHAR Ch)
    {
        InnerStr.push_back(Ch);
        return *this;
    }

    int OLString::StrCmp(const TCHAR* Str1, const TCHAR* Str2)
    {
        return t_strcmp(Str1, Str2);
    }

#define BASE_SIZE 512

    void OLString::Printf(const TCHAR* Format, ...)
    {
        TCHAR* Buffer = (TCHAR*)malloc(BASE_SIZE * sizeof(TCHAR));
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

        while(Written >= CurrSize - 1)
        {
            CurrSize *= 2;
            Buffer = (TCHAR*)realloc(Buffer, CurrSize);
            va_list ap2;
            va_start(ap2, Format);
            Written = t_vsnprintf(Buffer, CurrSize, RealFormat, ap2);
            va_end(ap2);
        }

        InnerStr = Buffer;

        free(Buffer);
    }

    OLString& OLString::AppendF(const TCHAR* Format, ...)
    {
        TCHAR* Buffer = (TCHAR*)malloc(BASE_SIZE * sizeof(TCHAR));
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

        while(Written >= CurrSize - 1)
        {
            CurrSize *= 2;
            Buffer = (TCHAR*)realloc(Buffer, CurrSize);
            va_list ap2;
            va_start(ap2, Format);
            Written = t_vsnprintf(Buffer, CurrSize, RealFormat, ap2);
            va_end(ap2);
        }

        InnerStr.append(Buffer);

        free(Buffer);
        return *this;
    }

    const TCHAR* OLString::CStr() const
    {
        return InnerStr.c_str();
    }


    OLString OLString::operator + (const OLString& InSrc) const
    {
        OLString NewStr;
        NewStr.InnerStr = InnerStr + InSrc.InnerStr;
        return NewStr;
    }

    OLString OLString::operator + (const TCHAR* InSrc) const
    {
        OLString NewStr;
        NewStr.InnerStr = InnerStr + InSrc;
        return NewStr;
    }

    const OLString& OLString::operator += (const OLString& InSrc)
    {
        InnerStr += InSrc.InnerStr;
        return *this;
    }

    const OLString& OLString::operator += (const TCHAR* InSrc)
    {
        InnerStr += InSrc;
        return *this;
    }

    const OLString& OLString::operator = (const OLString& InSrc)
    {
        InnerStr = InSrc.InnerStr;
        return *this;
    }

    const OLString& OLString::operator = (const TCHAR* InSrc)
    {
        InnerStr = InSrc;
        return *this;
    }

    bool OLString::operator == (const OLString& InSrc) const
    {
        return InnerStr == InSrc.InnerStr;
    }

    bool OLString::operator == (const TCHAR* InSrc) const
    {
        return InnerStr == InSrc;
    }

    bool OLString::operator != (const OLString& InSrc) const
    {
        return InnerStr != InSrc.InnerStr;
    }

    bool OLString::operator != (const TCHAR* InSrc) const
    {
        return InnerStr != InSrc;
    }

    void OLString::Strcpy(TCHAR* Buffer, int Len)
    {
        t_strncpy(Buffer, InnerStr.c_str(), Len);
    }

    int OLString::FindSubstr(const TCHAR* Str) const
    {
        size_t pos = InnerStr.find_first_of(Str);
        if(pos == std::string::npos)
            return -1;
        return (int)pos;
    }

    OLString OLString::Sub(int Start, int Count) const
    {
        OLString ret;
        ret.InnerStr = InnerStr.substr(Start, Count);
        return ret;
    }

    TCHAR& OLString::operator[](int Index) const
    {
        return *(&(((TCHAR*)InnerStr.data())[Index]));
        
    }

    bool MatchStr(const TCHAR* MainStr, const TCHAR* SubStr)
    {
        TCHAR* P1 = (TCHAR*)MainStr;
        TCHAR* P2 = (TCHAR*)SubStr;

        while(*P1 != 0 && *P2 != 0)
        {
            if(*P1 != *P2)
                return false;
            P1++;
            P2++;

        }
        if(*P2 == 0)
            return true;
        return false;
    }

    OLString& OLString::Replace(const TCHAR* From, const TCHAR* To)
    {
        int FromLen = t_strlen(From);
        int ToLen = t_strlen(To);
        StdStringType Str(From);
        size_t pos = 0;
        size_t next_begin = 0;
        int SrcLen = Len();

        OLList<TCHAR> Out;
        int SrcPos = 0;
        const TCHAR* SrcBuf = CStr();
        while(SrcPos < SrcLen && SrcBuf[SrcPos] != 0)
        {
            bool IsMatch = true;
            if(MatchStr(SrcBuf + SrcPos, From))
            {
                for(int DstIdx = 0; DstIdx < ToLen; DstIdx++)
                    Out.Add(To[DstIdx]);
                SrcPos += FromLen;
            }
            else
            {
                Out.Add(SrcBuf[SrcPos]);
                SrcPos++;
            }
        }
        Out.Add(0);
        InnerStr = Out.Data();


        // while((pos = InnerStr.find_first_of(Str, next_begin)) != std::string::npos)
        // {
        //     InnerStr.replace(pos, FromLen, To);
        //     next_begin = pos + ToLen;
        // }
        return *this;
    }

    OLString OLString::NameFromPath()
    {
        for(int i = InnerStr.length() - 1; i >= 0; i--)
        {
            if(InnerStr[i] == '/' || InnerStr[i] == '\\')
            {
                return Sub(i + 1, InnerStr.length() - 1 - i );
            }
        }
        return T("");
    }

    OLString OLString::ParentPath()
    {
        for(int i = InnerStr.length() - 1; i >= 0; i--)
        {
            if(InnerStr[i] == '/' || InnerStr[i] == '\\')
            {
                return Sub(0, i);
            }
        }
        return T("");
    }


    OLString OLString::ExtFromPath()
    {
        for(int i = InnerStr.length() - 1; i >= 0; i--)
        {
            if(InnerStr[i] == '.')
            {
                return Sub(i, InnerStr.length() - i );
            }
             if(InnerStr[i] == '/' || InnerStr[i] == '\\')
            {
                break;
            }
        }
        return T("");  
    }


    OLString OLString::PureNameFromPath()
    {
        int Start = -1;
        int End = -1;

        for(int i = InnerStr.length() - 1; i >= 0; i--)
        {
            if(InnerStr[i] == '.')
            {
                End = i;
            }
            if(InnerStr[i] == '/' || InnerStr[i] == '\\')
            {
                Start = i;
                break;
            }
        }

        if(End == -1)
            return Sub(Start + 1, InnerStr.length() - Start - 1);
        else
            return Sub(Start + 1, End - Start - 1);

    }

    bool MatchWildcardImpl(const TCHAR* Src, const TCHAR* Wildcard)
    {
        if(*Wildcard == 0 && *Src == 0)
            return true;
        else if(*Wildcard == 0 || *Src == 0)
            return false;

        if(*Wildcard == C('?'))
        {
            return MatchWildcardImpl(Src + 1, Wildcard + 1);
        }
        if(*Wildcard == C('*'))
        {
            return (MatchWildcardImpl(Src + 1, Wildcard)) || (MatchWildcardImpl(Src + 1, Wildcard + 1));
        }

        if(*Wildcard != *Src)
            return false;

        return MatchWildcardImpl(Src + 1, Wildcard + 1);
    }
    bool OLString::MatchWildcard(const TCHAR* Wildcard)
    {
        return MatchWildcardImpl(CStr(), Wildcard);
    }

    OLString& OLString::ToUpper()
    {
        int Len = InnerStr.length();
        for(int i = 0; i < Len; i++)
        {
            if(InnerStr[i]>= 'a' && InnerStr[i] <= 'z')
                InnerStr[i] = 'A' + InnerStr[i] - 'a';
        }
        return *this;
    }
    OLString& OLString::ToLower()
    {
        int Len = InnerStr.length();
        for(int i = 0; i < Len; i++)
        {
            if(InnerStr[i]>= 'A' && InnerStr[i] <= 'Z')
                InnerStr[i] = 'a' + InnerStr[i] - 'A';
        }
        return *this;
    }

    bool OLString::IsRelativePath()
    {
        int Len = InnerStr.length();
#ifdef PLATFORM_WIN        
        for(int i = 0; i < Len; i++)
        {
            if(InnerStr[i] >= C('A') && InnerStr[i] <= C('Z'))
                continue;
            if(InnerStr[i] >= C('a') && InnerStr[i] <= C('z'))
                continue;
            
            if(InnerStr[i] == C(':'))
            {
                if( (i + 1 < Len) && (InnerStr[i + 1] == C('/') || InnerStr[i + 1] == C('\\')))
                    return false;

            }
        }
        return true;
#elif defined(PLATFORM_MAC) || defined(PLATFORM_LINUX) 
    if(InnerStr.length() > 0 && InnerStr[0] == C('/'))
        return false;
    return true;
#else
        OL_ASSERT(0 && "to be implemented on this platform");
#endif
    }


    OLString OLString::FromUTF8(const char* Src)
    {
#ifdef USE_WCHAR
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> Converter;
        OLString Ret;
        Ret.InnerStr = Converter.from_bytes(Src);
        return Ret;
#else
        return OLString(Src);
#endif

    }

    void OLString::ToUTF8(OLList<char>& Output) const
    {
#ifdef USE_WCHAR        
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> Converter;
        std::string Utf8Str = Converter.to_bytes(InnerStr);
        int Len = (int)Utf8Str.length();
        Output.Resize(Len + 1);
        memcpy(Output.Data(), Utf8Str.data(), Len + 1);
#else
        return OLString(*this);
#endif
    }



}