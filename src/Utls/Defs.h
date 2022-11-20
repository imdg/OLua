#pragma once
#include <assert.h>
namespace OL
{

typedef char byte;
typedef unsigned int uint;
typedef long long int64;
typedef unsigned long long uint64;

#if PLATFORM_WIN
#define t_fopen FOpenWin
#else
#define t_fopen FOpenLin
#endif

#define TS2U(x) (ToUtf8Helper(x).Get())


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
//#define t_vsnprintf vswprintf
#define t_vsnprintf OL_vswprintf


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

}




#ifndef LOG_WITH_FILE
    #define LOG_WITH_FILE 0
#endif

#define OL_ASSERT(x) assert(x)