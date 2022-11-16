#include "Defs.h"
#include "TextEncoding.h"
namespace OL
{

#define LogMisc             1
#define LogSerializer       (1 << 1)
#define LogStream           (1 << 2)
#define LogCompile           (1 << 3)
#define LogAction           (1 << 4)
#define LogTester           (1 << 5)
#define LogLuaPlain           (1 << 6)
#define LogProjecct         (1 << 7)

// #if LOG_WITH_FILE
//     #define VERBOSE(mod, fmt, ...)    Logger::Log(ELogLevel::LogLevelVerbose, mod, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)
//     #define INFO(mod, fmt, ...)    Logger::Log(ELogLevel::LogLevelInfo, mod, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)
//     #define WARNING(mod, fmt, ...)    Logger::Log(ELogLevel::LogLevelWarning, mod, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)
//     #define ERROR(mod, fmt, ...)    Logger::Log(ELogLevel::LogLevelError, mod, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)
//     #define FATAL(mod, fmt, ...)    Logger::Log(ELogLevel::LogLevelFatal, mod, __FILENAME__, __LINE__, fmt, ##__VA_ARGS__)
// #else
//     #define VERBOSE(mod, fmt, ...)    Logger::Log(ELogLevel::LogLevelVerbose, mod, "", 0, fmt, ##__VA_ARGS__)
//     #define INFO(mod, fmt, ...)    Logger::Log(ELogLevel::LogLevelInfo, mod, "", 0, fmt, ##__VA_ARGS__)
//     #define WARNING(mod, fmt, ...)    Logger::Log(ELogLevel::LogLevelWarning, mod, "", 0, fmt, ##__VA_ARGS__)
//     #define ERROR(mod, fmt, ...)    Logger::Log(ELogLevel::LogLevelError, mod, "", 0, fmt, ##__VA_ARGS__)
//     #define FATAL(mod, fmt, ...)    Logger::Log(ELogLevel::LogLevelFatal, mod, "", 0, fmt, ##__VA_ARGS__)
// #endif

#if LOG_WITH_FILE
    #define VERBOSE(mod, ...)    Logger::Log(ELogLevel::LogLevelVerbose, mod, __FILENAME__, __LINE__,  ##__VA_ARGS__)
    #define INFO(mod, ...)    Logger::Log(ELogLevel::LogLevelInfo, mod, __FILENAME__, __LINE__, ##__VA_ARGS__)
    #define WARNING(mod, ...)    Logger::Log(ELogLevel::LogLevelWarning, mod, __FILENAME__, __LINE__, ##__VA_ARGS__)
    #define ERROR(mod, ...)    Logger::Log(ELogLevel::LogLevelError, mod, __FILENAME__, __LINE__, ##__VA_ARGS__)
    #define FATAL(mod, ...)    Logger::Log(ELogLevel::LogLevelFatal, mod, __FILENAME__, __LINE__, ##__VA_ARGS__)
#else
    #define VERBOSE(mod, ...)    Logger::Log(ELogLevel::LogLevelVerbose, mod, "", 0, ##__VA_ARGS__)
    #define INFO(mod, ...)    Logger::Log(ELogLevel::LogLevelInfo, mod, "", 0, ##__VA_ARGS__)
    #define WARNING(mod, ...)    Logger::Log(ELogLevel::LogLevelWarning, mod, "", 0, ##__VA_ARGS__)
    #define ERROR(mod, ...)    Logger::Log(ELogLevel::LogLevelError, mod, "", 0, ##__VA_ARGS__)
    #define FATAL(mod, ...)    Logger::Log(ELogLevel::LogLevelFatal, mod, "", 0, ##__VA_ARGS__)
#endif




enum ELogLevel
{
    LogLevelVerbose,
    LogLevelInfo,
    LogLevelWarning,
    LogLevelError,
    LogLevelFatal
};


class Logger
{
public:
    static void Init();

    static void Uninit();

    static void Log(ELogLevel Level, int Module, const char* File, int Line, const TCHAR* Fmt, ... );
    static void LogRaw(int Module, const TCHAR* Msg);
    static uint64 ModuleFilter;
    static ELogLevel MinLevel;
};



}