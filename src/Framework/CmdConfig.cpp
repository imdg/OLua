#include "CmdConfig.h"
#include "OLString.h"
namespace OL
{

CmdConfig* CmdConfig::GetInst()
{
    if(Inst == nullptr)
        Inst = new CmdConfig();
    return Inst;
}
CmdConfig* CmdConfig::Inst = nullptr;


CmdConfig::CmdConfig()
{
    Act = T("PrintHelp");
}
CmdConfig::CmdConfig(bool InIsGlobal) : IsGlobal(InIsGlobal)
{
    Act = T("PrintHelp");
}

void CmdConfig::ParseCommandline(int argc, const char** argv)
{
    if(argc < 2)
        return;
    Act = argv[1];

    OLString CurrKey(T(""));

    for(int i = 2; i < argc; i++)
    {
        OLString Cmd(A2T(argv[i]));
        if(Cmd.FindSubstr(T("--")) == 0)
        {
            CurrKey = Cmd;
            AddKey(CurrKey);
        }
        else
        {
            if(CurrKey == T(""))
                continue;

            AddParam(CurrKey, Cmd);
        }
    }

}

void CmdConfig::AddParam(OLString& Key, OLString& Val)
{
    if(Params.Exists(Key) == false)
    {
        Params.Add(Key, OLList<OLString>());
    }

    Params[Key].Add(Val);
}

void CmdConfig::AddKey(OLString &Key)
{
    if (Params.Exists(Key) == false)
    {
        Params.Add(Key, OLList<OLString>());
    }
}


OLList<OLString>& CmdConfig::GetParam(const TCHAR* Key)
{
    if(IsGlobal == false && this != GetInst())
    {
        if(Params.Exists(Key) == false)
            return GetInst()->GetParam(Key);
    }
    return Params[Key];
}

const OLString& CmdConfig::GetFirstParam(const TCHAR* Key)
{
    if(IsGlobal == false && this != GetInst())
    {
        if(Params.Exists(Key) == false || Params[Key].Count() == 0)
            return GetInst()->GetFirstParam(Key);
    }
    return Params[Key][0];
}

const OLString& CmdConfig::GetFirstParam(const TCHAR* Key, const OLString& Default)
{
    if(Params.Exists(Key) == false)
    {
        if(IsGlobal == false && this != GetInst())
            return GetInst()->GetFirstParam(Key, Default);
        return Default;
    }
    if(Params[Key].Count() == 0)
    {
        if(IsGlobal == false && this != GetInst())
            return GetInst()->GetFirstParam(Key, Default);
        return Default;
    }
    return Params[Key][0];
}
    

bool CmdConfig::HasParam(const TCHAR* Key)
{
    if(Params.Exists(Key) == false)
    {
        if(IsGlobal == false && this != GetInst())
            return GetInst()->HasParam(Key);
        return false;
    }
    if(Params[Key].Count() == 0)
    {
        if(IsGlobal == false && this != GetInst())
            return GetInst()->HasParam(Key);
        return false;
    }
    return true;
}

bool CmdConfig::HasKey(const TCHAR* Key)
{
    if(Params.Exists(Key) == false)
    {
        if(IsGlobal == false && this != GetInst())
            return GetInst()->HasKey(Key);
        return false;
    }

    return true;
}






 }