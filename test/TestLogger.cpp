#include "Common.h"
#include "Action.h"
#include "Logger.h"
#include "OLMap.h"
#include "OLString.h"
#include <memory>
#include "CompileMsg.h"

namespace OL
{

class TestLogger : public Action
{
public:
    virtual int Run()
    {
        OLMap < OLString, int> Test;
        Test[T("Var1")] = 1;
        Test[T("Var2")] = 2;
        int x = 10;
        VERBOSE(LogMisc, T("This is a test log: %d\n"), x);
        VERBOSE(LogMisc, T(": %d\n"), Test[T("Var1")] );
        VERBOSE(LogMisc, T(": %d\n"), Test[T("Var2")] );


        CompileMsg cm;

        cm.SetFile(T("C:\\NotAFile.ol"));
        cm.Log(CMT_Dummy, 0, 0, -1, -1);
        cm.Log(CMT_UnknownCh, 126, 7, -1, -1, C('('));

        return 0;
    };
};

REGISTER_ACTION(TestLogger)

}