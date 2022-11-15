
#ifdef PLATFORM_WIN
#include <windows.h>
#endif

#include "Env.h"
#include "CmdConfig.h"
#include <stdlib.h>
namespace OL
{

OLString Env::SavedPath;
OLString Env::BinPath;
OLString Env::DataPath;
OLString Env::SysCurrDir;

void Env::BreakPath(OLString Path, OLList<OLString> OutPath)
{
    int SrcLen = Path.Len();
    int Start = 0;
    for(int i = 0; i < SrcLen + 1; i++)
    {
        if(i == SrcLen || Path[i] == C('\\') || Path[i] == C('/'))
        {
            if(i != Start)
            {
                OLString Seg = Path.Sub(Start, i - Start);
                if(Seg == T(".."))
                {
                    OutPath.RemoveAt(OutPath.Count() - 1);
                }
                else if(Seg != T("."))
                {
                    OutPath.Add(Seg);
                }
            }
            Start = i + 1;
        }
    }
}
OLString Env::NormalizePath(OLString Src, bool RemoveLast)
{
    OLList<OLString> PathStack;
    int SrcLen = Src.Len();
    int Start = 0;
    for(int i = 0; i < SrcLen + 1; i++)
    {
        if(i == SrcLen || Src[i] == C('\\') || Src[i] == C('/'))
        {
            if(i != Start)
            {
                OLString Seg = Src.Sub(Start, i - Start);
                if(Seg == T(".."))
                {
                    PathStack.RemoveAt(PathStack.Count() - 1);
                }
                else if(Seg != T("."))
                {
                    PathStack.Add(Seg);
                }
            }
            Start = i + 1;
        }
    }

    int SegLen = PathStack.Count();
    if(RemoveLast)
        SegLen--;

    OLString Ret;
    for(int i = 0; i < SegLen; i++)
    {
        Ret.Append(PathStack[i]);
        if(i != SegLen - 1)
            Ret.Append(T("/"));
    }

    return Ret;
}

OLString Env::FileNameFromPath(OLString Path)
{
    int Len = Path.Len();
    int First = Len - 1;
    while(First > 0 && Path[First] != C('/') && Path[First] != C('\\'))
    {
        First--;
    }

    return Path.Sub(First + 1, Path.Len() - First);
}

void Env::InitEnv()
{
#ifdef PLATFORM_WIN
    char path[MAX_PATH];
    ::GetModuleFileNameA(nullptr, path, MAX_PATH);
    BinPath = path;
    BinPath = NormalizePath(BinPath, true);

    if(CmdConfig::GetInst()->HasParam(T("--saved")))
    {
        OLString TempPath;
        TempPath.Printf(T("%s/%s"), BinPath.CStr(), CmdConfig::GetInst()->GetFirstParam(T("--saved")).CStr());

        SavedPath = NormalizePath(TempPath, false);
    }
    else
    {
        SavedPath.Printf(T("%s/saved"), BinPath.CStr());
    }

    if(CmdConfig::GetInst()->HasParam(T("--data")))
    {
        OLString TempPath;
        TempPath.Printf(T("%s/%s"), BinPath.CStr(), CmdConfig::GetInst()->GetFirstParam(T("--data")).CStr());

        DataPath = NormalizePath(TempPath, false);
    }
    else
    {
        DataPath.Printf(T("%s/data"), BinPath.CStr());
    }

    
    ::GetCurrentDirectoryA(MAX_PATH, path);
    SysCurrDir = path;
    
#else
    // to do
    OL_ASSERT(0 && "to be implemented on this platform");
#endif    
}

OLString Env::FullPath(OLString RelativePath)
{

#ifdef PLATFORM_WIN
    char path[MAX_PATH];
    _fullpath(path, RelativePath.CStr(), MAX_PATH);
    return NormalizePath(path, false);
#else
    OL_ASSERT(0 && "to be implemented on this platform");
#endif
}
void Env::MakeDir(const OLString& AbsPath)
{
#ifdef PLATFORM_WIN
    
    int Pos = 0;
    int Len = AbsPath.Len();
    for(int Pos = 0; Pos < Len; Pos++)
    {
        if(AbsPath[Pos] == C('\\') || AbsPath[Pos] == C('/') )
        {
            OLString Partial = AbsPath.Sub(0, Pos);
            CreateDirectoryA(T2A(Partial.CStr()), nullptr);
        }
    }
    CreateDirectoryA(T2A(AbsPath.CStr()), nullptr);

#else
// to do
    OL_ASSERT(0 && "to be implemented on this platform");
#endif
}

void Env::MakeSavedDir(const TCHAR* SubPath)
{
#ifdef PLATFORM_WIN
    OLString FullPath;
    FullPath.Printf(T("%s/%s"), GetSavedPath().CStr(), SubPath );
    MakeDir(FullPath);
#else
// to do
    OL_ASSERT(0 && "to be implemented on this platform");
#endif
}

OLString Env::GetSavedDir(const TCHAR* SubPath)
{
    OLString FullPath;
    FullPath.Printf(T("%s/%s"), GetSavedPath().CStr(), SubPath );
    return FullPath;
}

OLString Env::GetSavedPath()
{
    return SavedPath;
}

OLString Env::PrepareSavedFilePath(const TCHAR* DirUnderSave, const TCHAR* FileName)
{
    MakeSavedDir(DirUnderSave);
    OLString FullPath = GetSavedDir(DirUnderSave);
    FullPath.AppendF(T("/%s"), FileName);
    return FullPath;
}

OLString Env::GetBinPath()
{
    return BinPath;
}

OLString Env::GetDataPath()
{
    return DataPath;
}

OLString Env::MakeReletivePath(OLString Root, OLString Path)
{
    OLString NorRoot = NormalizePath(Root, false);
    OLString NorPath = NormalizePath(Path, false);

    if(NorPath.FindSubstr(NorRoot.CStr()) != 0)
        return T("");

    return NorPath.Sub(NorRoot.Len() + 1, NorPath.Len() - NorRoot.Len() - 1);
}


void Env::IterateFileInDir(OLString DirPath, bool Recursive, OLFunc<void(OLString Path, bool IsDir)> Callback)
{
#ifdef PLATFORM_WIN
    WIN32_FIND_DATAA FindFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    OLString SearchPathName = DirPath;
    for(int i = 0; i < DirPath.Len(); i++)
    {
        if(SearchPathName[i] == C('/'))
            SearchPathName[i] = C('\\');
    }
    SearchPathName.Append(T("\\*"));
    hFind = FindFirstFileA(SearchPathName.CStr(), &FindFileData); 

    if (hFind == INVALID_HANDLE_VALUE)
    {
        FindClose(hFind);
        return;
    }
    else
    {
        while (FindNextFileA(hFind, &FindFileData) != 0)
        {
            OLString FileLocalName = FindFileData.cFileName;
            bool IsDir = (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
            if ( IsDir && (FileLocalName == T(".") || FileLocalName == T("..")) )
                continue;

            OLString FullPath = DirPath;
            FullPath.AppendF(T("/%s"), FileLocalName.CStr());
            Callback(FullPath, IsDir);

            if(IsDir && Recursive)
            {
                IterateFileInDir(FullPath, Recursive, Callback);
            }
        }
        FindClose(hFind);
    }
#else
// to do
    OL_ASSERT(0 && "to be implemented on this platform");
#endif
}


OLString Env::GetSysCurrDir()
{
    return SysCurrDir;
}

OLString Env::ToAbsPath(OLString AnyPath)
{
    if(AnyPath.IsRelativePath())
    {
        return SysCurrDir + "/" + AnyPath;
    }
    else
    {
        return AnyPath;
    }
}

}