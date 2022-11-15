#include "BatchAction.h"
#include "CmdConfig.h"
#include "Env.h"
#include "Logger.h"
#include <stdio.h>

namespace OL
{

bool IsSpace(char Ch)
{
    if(Ch == '\t' || Ch == '\v' || Ch == '\f' || Ch == ' ' )
        return true;
    return false;
}

bool IsNewLine(char Ch)
{
    if(Ch == '\r' || Ch == '\n')
        return true;
    return false;
}

int BatchAction::Run()
{
    if(Cmd().HasParam(T("--batchfile")) == false)
    {
        ERROR(LogAction, T("Batch file not specified\n"));
        return -1;
    }
    OLString BatchFileName = Cmd().GetFirstParam(T("--batchfile"));
    
    OLString BathFileFullPath = Env::FullPath(BatchFileName);
    OLString SubActionWorkingDir = BathFileFullPath.ParentPath();

    FILE* BatchFile = fopen(T2A(BatchFileName.CStr()), "rb");
    fseek(BatchFile, 0, SEEK_END);
    long Len = ftell(BatchFile);
    fseek(BatchFile, 0, SEEK_SET);

    char* Content = new char[Len + 1];
    fread(Content, 1, Len, BatchFile);
    fclose(BatchFile);
    Content[Len] = 0;

    long Curr = 0;
    char LastCh = ' ';
    OLList<const char*> CurrCmdLine;
    CurrCmdLine.Add("OLua");

    for(int Curr = 0; Curr < Len+1; Curr++)
    {
        char Ch = Content[Curr];
        if(IsNewLine(Ch) || Curr == Len)
        {
            Content[Curr] = 0;
            if(CurrCmdLine.Count() > 1 && CurrCmdLine[1][0] != '#')
            {
                Content[Curr] = 0;
                CmdConfig NewCmd(false);
                NewCmd.ParseCommandline(CurrCmdLine.Count(), CurrCmdLine.Data());

                
                OLString Info;
                Info.AppendF(T("Running batch action step: %s: "), NewCmd.Act.CStr());
                for(int i = 1; i < CurrCmdLine.Count(); i++)
                {
                    Info.AppendF(T("%s "), A2T(CurrCmdLine[i]));
                }
                //Info.Append(T("\n"));
                VERBOSE(LogAction, (TCHAR*)Info.CStr());

                int Ret = OL::ActionMgr::GetInst()->RunActionWithLocalCmd(NewCmd.Act.CStr(), &NewCmd, SubActionWorkingDir);
                if(Ret != 0)
                {
                    return Ret;
                }

            }
            CurrCmdLine.Clear();
            CurrCmdLine.Add("OLua");
        }
        else
        {
            if (IsSpace(LastCh) || IsNewLine(LastCh))
            {
                if (!IsSpace(Ch) && !IsNewLine(Ch))
                {
                    CurrCmdLine.Add(Content + Curr);
                }

            }

            if (IsSpace(Ch))
            {
                Content[Curr] = 0;
            }
        }

        LastCh = Ch;
    }
    return 0;
}

REGISTER_ACTION(BatchAction)
}