/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "BatchAction.h"
#include "CmdConfig.h"
#include "Env.h"
#include "Logger.h"
#include <stdio.h>
#include <string.h>

namespace OL
{

bool IsSpace(TCHAR Ch)
{
    if(Ch == C('\t') || Ch == C('\v') || Ch == C('\f') || Ch == C(' ') )
        return true;
    return false;
}

bool IsNewLine(TCHAR Ch)
{
    if(Ch == C('\r') || Ch == C('\n'))
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
    
    OLString BathFileFullPath = BatchFileName;
    if(BatchFileName.IsRelativePath())
    {
        BathFileFullPath = Env::GetSysCurrDir() + T("/") + BatchFileName;
    }
    OLString SubActionWorkingDir = BathFileFullPath.ParentPath();

    FILE* BatchFile = t_fopen(BathFileFullPath.CStr(), "rb");
    if(BatchFile == NULL)
    {
        ERROR(LogAction, T("Cannot load batch file: %s"), BathFileFullPath.CStr());
        return -1;
    }
    fseek(BatchFile, 0, SEEK_END);
    long RawLen = ftell(BatchFile);
    fseek(BatchFile, 0, SEEK_SET);

    char* RawContent = new char[RawLen + 1];
    fread(RawContent, 1, RawLen, BatchFile);
    fclose(BatchFile);
    RawContent[RawLen] = 0;
    OLString ContentStr = OLString::FromUTF8(RawContent);

    long Len = ContentStr.Len();
    TCHAR* Content = new TCHAR[Len + 1];
    memcpy(Content, ContentStr.CStr(), Len * sizeof(TCHAR));
    Content[Len] = 0;

    long Curr = 0;
    TCHAR LastCh = ' ';
    OLList<const TCHAR*> CurrCmdLine;
    CurrCmdLine.Add(T("OLua"));
    
    for(int Curr = 0; Curr < Len+1; Curr++)
    {
        TCHAR Ch = Content[Curr];
        if(IsNewLine(Ch) || Curr == Len)
        {
            Content[Curr] = 0;
            if(CurrCmdLine.Count() > 1 && CurrCmdLine[1][0] != '#')
            {
                Content[Curr] = 0;
                CmdConfig NewCmd(false);
                NewCmd.ParseCommandlineT(CurrCmdLine.Count(), CurrCmdLine.Data());

                
                OLString Info;
                Info.AppendF(T("Running batch action step: %s: "), NewCmd.Act.CStr());
                for(int i = 1; i < CurrCmdLine.Count(); i++)
                {
                    Info.AppendF(T("%s "), CurrCmdLine[i]);
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
            CurrCmdLine.Add(T("OLua"));
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