

#ifdef PLATFORM_WIN
#include <Windows.h>
#include <sys/timeb.h>
#include <time.h>
#elif defined(PLATFORM_MAC)
#include <CoreServices/CoreServices.h>
#include <mach/mach.h>
#include <mach/mach_time.h>

#else

#endif

#include "Timer.h"
namespace OL
{


#ifdef PLATFORM_WIN
__timeb64 StartTime;
LARGE_INTEGER StartCounter;
LARGE_INTEGER Frenquency;
#elif defined(PLATFORM_MAC)
time_t          StartRealTime;
uint64_t        StartHPTime;
#endif

void Timer::Init()
{
#ifdef PLATFORM_WIN
    _ftime64_s( &StartTime);
    QueryPerformanceCounter(&StartCounter);
    QueryPerformanceFrequency(&Frenquency);

    struct timeval tv;
    time_t t;
    
#elif defined(PLATFORM_MAC)
    time(&StartRealTime);
    StartHPTime = clock_gettime_nsec_np(CLOCK_MONOTONIC_RAW);
#endif
}

DateTime Timer::GetCurrTime()
{
#ifdef PLATFORM_WIN    
    __timeb64 Curr = StartTime;
    LARGE_INTEGER CurrTicks;
    QueryPerformanceCounter(&CurrTicks);
    uint64 Elaspsed = (CurrTicks.QuadPart - StartCounter.QuadPart) * 1000 / Frenquency.QuadPart + StartTime.millitm;
    Curr.time += Elaspsed / 1000;
    Curr.millitm = Elaspsed % 1000;

    struct tm * timeinfo = _localtime64 ( &Curr.time );

    DateTime Ret;
    Ret.Year = timeinfo->tm_year;
    Ret.Month = timeinfo->tm_mon;
    Ret.Day = timeinfo->tm_mday;
    Ret.Hour = timeinfo->tm_hour;
    Ret.Minute = timeinfo->tm_min;
    Ret.Second = timeinfo->tm_sec;

    Ret.Millisecond = Curr.millitm;
    Ret.Timestamp = Elaspsed;

    return Ret;
#elif defined(PLATFORM_MAC)
    uint64_t CurrHPTime = clock_gettime_nsec_np(CLOCK_MONOTONIC_RAW);
    uint64_t ElaspedNano = CurrHPTime - StartHPTime;

    time_t CurrRealTime = StartRealTime + ElaspedNano/NSEC_PER_SEC;

    struct tm * timeinfo;
    timeinfo = localtime ( &CurrRealTime );
    DateTime Ret;
    Ret.Year = timeinfo->tm_year;
    Ret.Month = timeinfo->tm_mon;
    Ret.Day = timeinfo->tm_mday;
    Ret.Hour = timeinfo->tm_hour;
    Ret.Minute = timeinfo->tm_min;
    Ret.Second = timeinfo->tm_sec;

    uint64_t ElaspedMilli = ElaspedNano / NSEC_PER_MSEC;
    Ret.Millisecond = ElaspedMilli % 1000;
    Ret.Timestamp = ElaspedMilli;
    return Ret;
#else
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    DateTime Ret;
    Ret.Year = timeinfo->tm_year;
    Ret.Month = timeinfo->tm_mon;
    Ret.Day = timeinfo->tm_mday;
    Ret.Hour = timeinfo->tm_hour;
    Ret.Minute = timeinfo->tm_min;
    Ret.Second = timeinfo->tm_sec;

    Ret.Millisecond = 0;
    Ret.Timestamp = 0;
    #pragma warning("Millisecond and Timestamp to be finished")
    return Ret;
#endif
}



}