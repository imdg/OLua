#include "Common.h"
#include "Action.h"
#include "Logger.h"
#include "OLMap.h"
#include "OLString.h"
#include <memory>
#include "CompileMsg.h"

namespace OL
{

bool TestWildcard(const TCHAR* InStr, const TCHAR* InFilter)
{
    TCHAR* Str = (TCHAR*)InStr;
    TCHAR* Filter = (TCHAR*)InFilter;
    if(*Filter == 0 && *Str == 0)
        return true;
    else if(*Filter == 0 || *Str == 0)
        return false;

    if(*Filter == C('?'))
    {
        return TestWildcard(Str + 1, Filter + 1);
    }
    if(*Filter == C('*'))
    {
        return (TestWildcard(Str + 1, Filter)) || (TestWildcard(Str + 1, Filter + 1));
    }

    if(*Filter != *Str)
        return false;

    return TestWildcard(Str + 1, Filter + 1);
    
}

class TestMisc : public Action
{
public:
    void Match(const TCHAR* Str, const TCHAR* Filter)
    {
        bool Result = TestWildcard(Str, Filter);
        VERBOSE(LogTester, T("Test\"%s\" by \"%s\""), Str, Filter);
        VERBOSE(LogTester, T("    %s"), Result ? T("Matched") : T("Failed"));
    };
    virtual int Run()
    {
        Match(T("MyNameIsGavin.lua"), T("*.lua"));
        Match(T("MyNameIsGavin.lua"), T("*.*"));
        Match(T("MyNameIsGavin.lua"), T("*Name*G*.lua"));
        Match(T("MyNameIsGavin.lua"), T("*Name*FF.lua"));
        Match(T("MyNameIsGavin.lua"), T("*.exe"));
        return 0;
    };
};

REGISTER_ACTION(TestMisc)

}