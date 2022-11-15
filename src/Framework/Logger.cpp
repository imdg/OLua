#include "stdarg.h"
#include "Logger.h"
#include "Common.h"

#include "Env.h"
#include <stdio.h>
#include "Timer.h"



namespace OL
{

FILE* GLogFile = nullptr;



void Logger::Init()
{
    Env::MakeSavedDir("logs");

    OLString Dir = Env::GetSavedDir(T("logs"));

    OLString LogFilePath = Dir + T("/OLuaLog.log");
    
    GLogFile = fopen(T2A(LogFilePath.CStr()), "wb");



}

void Logger::Uninit()
{
    fclose(GLogFile);
}

const char* LevelStr(ELogLevel Level)
{
    switch (Level)
    {
    case ELogLevel::LogLevelVerbose: return "Verbose";
    case ELogLevel::LogLevelInfo: return "Info";
    case ELogLevel::LogLevelWarning: return "Warning";
    case ELogLevel::LogLevelError: return "Error";
    case ELogLevel::LogLevelFatal: return "Fatal";
        
    
    default:
        return "";
    }
}

#define BASE_SIZE 512
void Logger::Log(ELogLevel Level, int Module, const char* File, int Line, const TCHAR* Fmt, ... )
{
    if((int)Level < (int)MinLevel)
        return;
    if((Module & (~ModuleFilter)) == 0 )
        return;

    TCHAR* Buffer = (TCHAR*)malloc(BASE_SIZE * sizeof(TCHAR));
    int CurrSize = BASE_SIZE;
    int Written = -1;

    va_list ap;
    va_start(ap, Fmt);
    Written = vsnprintf(Buffer, CurrSize, Fmt, ap);
    va_end(ap);

    while(Written >= CurrSize - 1)
    {
        CurrSize *= 2;
        Buffer = (TCHAR*)realloc(Buffer, CurrSize);
        va_list ap2;
        va_start(ap2, Fmt);
        Written = vsnprintf(Buffer, CurrSize, Fmt, ap2);
        va_end(ap2);
    }
    
    char timeStr[128];

    DateTime CurrTime = Timer::GetCurrTime();
    snprintf(timeStr, 128, "%02d:%02d:%02d.%03d", CurrTime.Hour, CurrTime.Minute, CurrTime.Second, (int)CurrTime.Millisecond  );
  
    if(File[0] != 0)
    {
        printf("[%s - %s:%d] [%s]:  %s\n", timeStr, File, Line, LevelStr(Level), T2A(Buffer));
        fprintf(GLogFile, "[%s - %s:%d] [%s]:  %s\n", timeStr, File, Line, LevelStr(Level), T2A(Buffer));
    }
    else
    {
        printf("[%s] [%s]:  %s\n", timeStr, LevelStr(Level), T2A(Buffer));
        fprintf(GLogFile, "[%s] [%s]:  %s\n", timeStr, LevelStr(Level), T2A(Buffer));
    }
    fflush(GLogFile);
    delete[] Buffer;
}

void Logger::LogRaw(int Module, const TCHAR* Msg)
{
    if((Module & (~ModuleFilter)) == 0 )
        return;
    printf("%s\n", Msg);
    fprintf(GLogFile, "%s\n", Msg);
    fflush(GLogFile);
}

uint64 Logger::ModuleFilter = 0;

ELogLevel Logger::MinLevel = ELogLevel::LogLevelVerbose;

}