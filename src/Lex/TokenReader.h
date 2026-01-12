/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#pragma once
#include "OLString.h"

namespace OL
{
 

#define TR_EOF  (0)


class TokenReader
{
public:

    TokenReader();
    ~TokenReader();

    void BeginNewToken();
    int NextChar();
    bool NextCharIfCurr(TCHAR Ch);
    bool NextCharIfCurr(TCHAR Ch1, TCHAR Ch2);
    int CurrChar();

    OLString Pick();
    OLString PickExcludeCurr();

    int SkipSpace(bool SkipLine);
    void NextCharUntil(TCHAR Ch);
    void NextCharUntil(TCHAR Ch1, TCHAR Ch2);
    void NextCharUntil(TCHAR Ch1, TCHAR Ch2, TCHAR Ch3);
    void NextCharUntil(TCHAR Ch1, TCHAR Ch2, TCHAR Ch3, TCHAR Ch4);


    void LoadFromMemory(const TCHAR* SrcBuffer, int Len);
    void LoadFromFile(const TCHAR* FileName);

    OLString& GetSourceFileName();
    int GetLine();
    int GetCol();
    int GetPos();

private:
    int Start;
    int End;
    TCHAR* Buffer;
    int Len;
    int Line;
    int LastLineEnd;
    OLString SourceFileName;
    OLString BufferString;
};










}