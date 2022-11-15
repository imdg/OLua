#pragma once
#include "Common.h"

namespace OL
{

struct DateTime
{
    int Year;
    int Month;
    int Day;
    int Hour;
    int Minute;
    int Second;
    int Millisecond;
    uint64 Timestamp;
};

class Timer
{
public:
    static void Init();
    static DateTime GetCurrTime();
};
}