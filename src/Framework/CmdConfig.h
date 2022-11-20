/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#pragma once
#include "Defs.h"
#include "OLMap.h"
#include "OLString.h"
#include "OLList.h"

namespace OL
{

class CmdConfig
{
public:
    static CmdConfig* GetInst();

    CmdConfig();
    CmdConfig(bool InIsGlobal);
    void ParseCommandline(int argc, const char** argv);
    void ParseCommandlineT(int argc, const TCHAR** argv);
    OLList<OLString>& GetParam(const TCHAR* Key);
    const OLString& GetFirstParam(const TCHAR* Key, const OLString& Default);
    const OLString& GetFirstParam(const TCHAR* Key);

    void AddParam(OLString& Key, OLString& Val);
    bool HasParam(const TCHAR* Key);
    void AddKey(OLString& Key);
    bool HasKey(const TCHAR*Key);
    bool IsGlobal;
    
    OLString Act;

private:
    static CmdConfig* Inst;
    OLMap<OLString, OLList<OLString> > Params;
    
};



}