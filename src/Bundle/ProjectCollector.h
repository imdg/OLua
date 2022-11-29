/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#pragma once
#include "Common.h"

namespace OL
{


class BundleBuilder;

class ProjectCollector
{
public:
    struct DirInfo
    {
        OLString DirPath;
        OLString FileFilter;
        bool     IncludeSubDir;

        bool     IncludeExt;  
        OLString PathSplitter;
        OLString Prefix;
        OLString Suffix;
        OLString OutExt;
        OLString OutPath;
        OLString IntermediatePath;
    };

    struct FileInfo
    {
        OLString FilePath;
        OLString OutPath;
        OLString FileIdentity;
        OLString IntermediateFile;
    };

    OLList<DirInfo>  Dirs;
    OLList<FileInfo> Files;

    void Reset();
    void ApplyBuilder(BundleBuilder& Builder);

    void ApplyDir(BundleBuilder& Builder, DirInfo& Dir );

};


}