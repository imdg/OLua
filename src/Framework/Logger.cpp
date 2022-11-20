/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "stdarg.h"
#include "Logger.h"
#include "Common.h"

#include "Env.h"
#include <stdio.h>
#include "Timer.h"

#include "ConsoleStream.h"
#include "FileStream.h"


namespace OL
{

//FILE* GLogFile = nullptr;
ConsoleStream GConsole;
FileStream GLogFile;



void Logger::Init()
{
    Env::MakeSavedDir(T("logs"));

    OLString Dir = Env::GetSavedDir(T("logs"));

    OLString LogFilePath = Dir + T("/OLuaLog.log");
    GLogFile.OpenWrite(LogFilePath);

    //GLogFile = t_fopen(LogFilePath, "wb");



}

void Logger::Uninit()
{
    //fclose(GLogFile);
    GLogFile.Close();
}

const TCHAR* LevelStr(ELogLevel Level)
{
    switch (Level)
    {
    case ELogLevel::LogLevelVerbose: return T("Verbose");
    case ELogLevel::LogLevelInfo: return T("Info");
    case ELogLevel::LogLevelWarning: return T("Warning");
    case ELogLevel::LogLevelError: return T("Error");
    case ELogLevel::LogLevelFatal: return T("Fatal");
        
    
    default:
        return T("");
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

    TCHAR* RealFormat = (TCHAR*)Fmt;
#if (defined(PLATFORM_MAC) || defined(PLATFORM_LINUX)) && USE_WCHAR
    OLString TempFormat = Fmt;
    TempFormat.Replace(T("%s"), T("%ls"));
    RealFormat = (TCHAR*)TempFormat.CStr();
#endif 

    va_list ap;
    va_start(ap, Fmt);
    Written = t_vsnprintf(Buffer, CurrSize, RealFormat, ap);
    va_end(ap);

    while(Written >= CurrSize - 1)
    {
        CurrSize *= 2;
        Buffer = (TCHAR*)realloc(Buffer, CurrSize);
        va_list ap2;
        va_start(ap2, Fmt);
        Written = t_vsnprintf(Buffer, CurrSize, RealFormat, ap2);
        va_end(ap2);
    }
    

    DateTime CurrTime = Timer::GetCurrTime();
    OLString TimeStr;
    TimeStr.Printf(T("%02d:%02d:%02d.%03d"), CurrTime.Hour, CurrTime.Minute, CurrTime.Second, (int)CurrTime.Millisecond  );
  
    OLString FileName = OLString::FromUTF8(File);
    OLString OutStr;
    if(File[0] != 0)
    {
        OutStr.Printf(T("[%s - %s:%d] [%s]:  %s\n"), TimeStr.CStr(), FileName.CStr(), Line, LevelStr(Level), Buffer);
    }
    else
    {
        OutStr.Printf(T("[%s] [%s]:  %s\n"), TimeStr.CStr(), LevelStr(Level), Buffer);
    }
    GConsole.WriteText(OutStr.CStr());
    GLogFile.WriteText(OutStr.CStr());

    GLogFile.Flush();
    delete[] Buffer;
}

void Logger::LogRaw(int Module, const TCHAR* Msg)
{
    if((Module & (~ModuleFilter)) == 0 )
        return;

    OLString OutStr;

    OutStr.Printf(T("%s\n"), Msg);
    GConsole.WriteText(OutStr.CStr());
    GLogFile.WriteText(OutStr.CStr());
    GLogFile.Flush();
}

uint64 Logger::ModuleFilter = 0;

ELogLevel Logger::MinLevel = ELogLevel::LogLevelVerbose;

}