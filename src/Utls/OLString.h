#pragma once

#include <string>
#include "Defs.h"
#include "OLHash.h"
namespace OL
{
    class OLString;
    template<> 
    struct OLHash<OLString>;

    class OLString
    {
    public:
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
    private:
        std::string InnerStr;
        friend OLHash<OLString>;
        
    };

    template<> 
    struct OLHash<OLString>
    {
        inline uint64 operator() (const OLString& _Keyval) const
        {
            return (uint64)std::hash<std::string>()(_Keyval.InnerStr);
        }
    };

}
