#pragma once

#include <string>
#include "Defs.h"
#include "TextEncoding.h"
#include "OLHash.h"
namespace OL
{
    class OLString;
    template<typename Ty>
    class OLList;
    template<> 
    struct OLHash<OLString>;

    class OLString
    {

    public:
        typedef std::basic_string<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR>> StdStringType;
        OLString();
        OLString(const TCHAR* InStr);
        OLString(const TCHAR* InStr, int Len);
        OLString(const OLString& InSrc);
        OLString(const OLString&& InSrc);

        int Len() const;
        OLString& Append(const TCHAR* InStr);
        OLString& Append(const OLString& InStr);
        OLString& AppendCh(const TCHAR Ch);
        void Printf(const TCHAR* Format, ...);
        OLString& AppendF(const TCHAR* Format, ...);
        int FindSubstr(const TCHAR* Str) const;

        OLString Sub(int Start, int Count) const;
        OLString NameFromPath();
        OLString ExtFromPath(); // '.' is included in return value
        OLString PureNameFromPath();
        OLString ParentPath();
        OLString& ToUpper();
        OLString& ToLower();

        bool IsRelativePath();
        const TCHAR* CStr() const;

        OLString& Replace(const TCHAR* From, const TCHAR* To);

        bool MatchWildcard(const TCHAR* Wildcard);
  
        OLString operator + (const OLString& InSrc) const;
        OLString operator + (const TCHAR* InSrc) const;

        const OLString& operator += (const OLString& InSrc);
        const OLString& operator += (const TCHAR* InSrc);

        const OLString& operator = (const OLString& InSrc);
        const OLString& operator = (const TCHAR* InSrc);

        bool operator == (const OLString& InSrc) const;
        bool operator == (const TCHAR* InSrc) const;

        bool operator != (const OLString& InSrc) const;
        bool operator != (const TCHAR* InSrc) const;

        TCHAR& operator[](int Index) const;

        void Strcpy(TCHAR* Buffer, int Len);
        static int StrCmp(const TCHAR* Str1, const TCHAR* Str2);

        static OLString FromUTF8(const char* Src);
        void ToUTF8(OLList<char>& Output);

        
    private:

        StdStringType InnerStr;
        friend OLHash<OLString>;
        
    };

    template<> 
    struct OLHash<OLString>
    {
        inline uint64 operator() (const OLString& _Keyval) const
        {
            return (uint64)std::hash<typename OLString::StdStringType>()(_Keyval.InnerStr);
        }
    };

}
