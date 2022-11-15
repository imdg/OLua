#pragma once
#include <assert.h>
namespace OL
{
typedef char TCHAR;
typedef char byte;
typedef unsigned int uint;
typedef long long int64;
typedef unsigned long long uint64;
}

// Wrapper for all hardcode string
#define T(x) x
#define C(x) x

#define T2A(x) x
#define A2T(x) x


#ifndef LOG_WITH_FILE
    #define LOG_WITH_FILE 0
#endif

#define OL_ASSERT(x) assert(x)