#pragma once
#include <assert.h>
namespace OL
{

typedef char byte;
typedef unsigned int uint;
typedef long long int64;
typedef unsigned long long uint64;
}




#ifndef LOG_WITH_FILE
    #define LOG_WITH_FILE 0
#endif

#define OL_ASSERT(x) assert(x)