#include "Common.h"
#include "Action.h"
#include "Lexer.h"
#include "Logger.h"
namespace OL
{

class GenCharTab : public Action
{
public:
    virtual int Run()
    {
        OLString Text = T("\n");
        for(int i = 0; i < 255; i++)
        {
            TCHAR ch = (TCHAR)i;
            uint val = 0;

            if(ch >= C('0') && ch <= C('9'))
                val |= CH_DIGIT;
            
            if( (ch >= C('a') && ch <= C('z'))
                || (ch >= C('A') && ch <= C('Z')))
                val |= CH_ABC;

            if((ch >= C('0') && ch <= C('9')) 
                || (ch >= C('a') && ch <= C('z'))
                || (ch >= C('A') && ch <= C('Z'))
                || (ch == C('_')))
                val |= CH_NAME;
            
            if((ch >= C('a') && ch <= C('z'))
                || (ch >= C('A') && ch <= C('Z'))
                || (ch == C('_')))
                val |= CH_NAME_START;
            
            if((ch >= C('a') && ch <= C('f'))
                || (ch >= C('A') && ch <= C('F'))
                || (ch >= C('0') && ch <= C('9')))
                val |= CH_HEX_DIGIT;

            if(i % 16 == 0)
                Text.Append(T("\n"));
            Text.AppendF(T("0x%x, "), val);

        }

        ERROR(LogMisc, T("%s\n"), Text.CStr());
        ERROR(LogMisc, T("----------\n"));
        return 0;
    };
};

REGISTER_ACTION(GenCharTab)

}