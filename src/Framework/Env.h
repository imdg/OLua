#include "Common.h"
#include "OLFunc.h"

namespace OL
{

class Env
{
public:
    static void InitEnv();
    static OLString GetSavedPath();
    static OLString GetBinPath();
    static OLString GetDataPath();
    static OLString GetSysCurrDir();

    static OLString PrepareSavedFilePath(const TCHAR* DirUnderSave, const TCHAR* FileName);
    static void MakeDir(const OLString& AbsPath);
    static void MakeSavedDir(const TCHAR* SubPath);

    static OLString GetSavedDir(const TCHAR* SubPath);

    static OLString NormalizePath(OLString Src, bool RemoveLast);
    

    static OLString FileNameFromPath(OLString Path);
    static OLString MakeReletivePath(OLString Root, OLString Path);

    static void BreakPath(OLString Path, OLList<OLString> OutPath);

    static void IterateFileInDir(OLString DirPath, bool Recursive, OLFunc<void(OLString Path, bool IsDir)> Callback);
    static OLString FullPath(OLString RelativePath);

    static OLString ToAbsPath(OLString AnyPath);
    
private:
    static OLString SavedPath;
    static OLString BinPath;
    static OLString DataPath;
    static OLString SysCurrDir;
};

} // namespace OL
