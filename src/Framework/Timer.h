/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

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