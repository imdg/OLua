/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/


#ifdef PLATFORM_WIN
#include <windows.h>
#elif defined(PLATFORM_MAC)
#include <mach-o/dyld.h>
#include <limits.h>
#include <sys/param.h> 
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#elif defined(PLATFORM_LINUX)
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include<sys/types.h>
#include<dirent.h>
#include<sys/stat.h>
#include <sys/param.h> 
#include <string.h>
#endif

#include "Env.h"
#include "CmdConfig.h"
#include <stdlib.h>
namespace OL
{

FILE* FOpenWin(OLString FileName, const char* Mode)
{
#ifdef PLATFORM_WIN
#if USE_WCHAR
    return _wfopen(FileName.CStr(), A2T(Mode));
#else
    return fopen(FileName.CStr(), Mode);
#endif
#else
    return nullptr;
#endif
}

FILE* FOpenLin(OLString FileName, const char* Mode)
{

    OLList<char> utf8;
    FileName.ToUTF8(utf8);
    return fopen(utf8.Data(), Mode);
}


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
#if defined(PLATFORM_MAC) || defined(PLATFORM_LINUX)
    if(SrcLen > 0 && Src[0] == C('/'))
        Ret.Append(T("/"));
#endif
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
    wchar_t path[MAX_PATH];
    ::GetModuleFileNameW(nullptr, path, MAX_PATH);
    BinPath = W2T(path);
    BinPath = NormalizePath(BinPath, true);

    ::GetCurrentDirectoryW(MAX_PATH, path);
    SysCurrDir = W2T(path);
#elif defined(PLATFORM_MAC)
 
    char RawPath[MAXPATHLEN + 1];
    unsigned int Size = MAXPATHLEN + 1;
    int Result = _NSGetExecutablePath(RawPath, &Size);
    RawPath[Size] = '\0';
    BinPath = NormalizePath(OLString::FromUTF8(RawPath), true);

    getcwd(RawPath, MAXPATHLEN);
    SysCurrDir = OLString::FromUTF8(RawPath);
#elif defined(PLATFORM_LINUX)
    char RawPath[PATH_MAX];
    readlink( "/proc/self/exe", RawPath, PATH_MAX );
    BinPath = NormalizePath(OLString::FromUTF8(RawPath), true);

    getcwd(RawPath, PATH_MAX);
    SysCurrDir = OLString::FromUTF8(RawPath);
#else
    OL_ASSERT(0 && "to be implemented on this platform");
#endif 


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
            CreateDirectoryW(T2W(Partial.CStr()), nullptr);
        }
    }
    CreateDirectoryW(T2W(AbsPath.CStr()), nullptr);
#elif defined(PLATFORM_MAC) || defined(PLATFORM_LINUX)
    int Pos = 0;
    int Len = AbsPath.Len();
    for(int Pos = 0; Pos < Len; Pos++)
    {
        if((AbsPath[Pos] == C('\\') || AbsPath[Pos] == C('/') ) && Pos != 0)
        {
            OLString Partial = AbsPath.Sub(0, Pos);

            OLList<char> utf8;
            Partial.ToUTF8(utf8);
            struct stat st = {0};
            if (stat(utf8.Data(), &st) == -1) 
                mkdir(utf8.Data(), 0700);
        }
    }
    {
        OLList<char> utf8;
        AbsPath.ToUTF8(utf8);
        struct stat st;
        if (stat(utf8.Data(), &st) == -1) 
            mkdir(utf8.Data(), 0700);
    }
#else
// to do
    OL_ASSERT(0 && "to be implemented on this platform");
#endif
}

void Env::MakeSavedDir(const TCHAR* SubPath)
{
    OLString FullPath;
    FullPath.Printf(T("%s/%s"), GetSavedPath().CStr(), SubPath );
    MakeDir(FullPath);
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
    WIN32_FIND_DATAW FindFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    OLString SearchPathName = DirPath;
    for(int i = 0; i < DirPath.Len(); i++)
    {
        if(SearchPathName[i] == C('/'))
            SearchPathName[i] = C('\\');
    }
    SearchPathName.Append(T("\\*"));
    hFind = FindFirstFileW(T2W(SearchPathName.CStr()), &FindFileData); 

    if (hFind == INVALID_HANDLE_VALUE)
    {
        FindClose(hFind);
        return;
    }
    else
    {
        while (FindNextFileW(hFind, &FindFileData) != 0)
        {
            OLString FileLocalName = W2T(FindFileData.cFileName);
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
#elif defined(PLATFORM_MAC) || defined(PLATFORM_LINUX)
        DIR* dir;
        struct dirent *ent;
        struct stat states;
        
        OLList<char> utf8;
        DirPath.ToUTF8(utf8);
        dir = opendir(utf8.Data());
        if(dir == NULL)
            return;
        while((ent=readdir(dir)) != NULL)
        {
            
            if(!strcmp(".", ent->d_name) || !strcmp("..", ent->d_name))
            {
                continue;
            }
            else
            {
                OLString CurrFullName = DirPath + T("/") + OLString::FromUTF8(ent->d_name);

                stat(TS2U(CurrFullName), &states);
                if(S_ISDIR(states.st_mode))
                {
                    Callback(CurrFullName, true);
                    if(Recursive)
                        IterateFileInDir(CurrFullName, Recursive, Callback);
                }
                else
                {
                    Callback(CurrFullName, false);
                }
            }
        }

        closedir(dir);
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
        return SysCurrDir + T("/") + AnyPath;
    }
    else
    {
        return AnyPath;
    }
}

}