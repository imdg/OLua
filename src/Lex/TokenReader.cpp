/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "Common.h"
#include "TokenReader.h"
#include <stdio.h>
#include "Logger.h"

namespace OL
{

TokenReader::TokenReader() 
    : Start(0), End(0), Buffer(nullptr), Len(0)
    , Line(1), LastLineEnd(-1)
{

}

TokenReader::~TokenReader()
{
    delete[] Buffer;
}

void TokenReader::BeginNewToken()
{
    Start = End;
}

int TokenReader::NextChar()
{
    End++;
    if(End >= Len)
        return TR_EOF;

    if(Buffer[End] == C('\n'))
    {
        LastLineEnd = End;
        Line++;
    }
    return Buffer[End];
}

bool TokenReader::NextCharIfCurr(TCHAR Ch)
{
    if(CurrChar() == Ch)
    {
        NextChar();
        return true;
    }
    return false;
}

bool TokenReader::NextCharIfCurr(TCHAR Ch1, TCHAR Ch2)
{
    int Curr = CurrChar();
    if(Curr == Ch1 || Curr == Ch2)
    {
        NextChar();
        return true;
    }
    return false;
}

int TokenReader::CurrChar()
{
    if(End >= Len)
        return TR_EOF;
    return Buffer[End];
}

OLString TokenReader::Pick()
{
    if(End >= Len)
        return OLString(Buffer + Start, End - Start);
    return OLString(Buffer + Start, End - Start + 1);
}

OLString TokenReader::PickExcludeCurr()
{
    if(End == Start)
        return T("");

    return OLString(Buffer + Start, End - Start);
}

int TokenReader::SkipSpace(bool SkipLine)
{
    int Count = 0;
    if(End >= Len)
        return 0;
    while(Buffer[End] == '\t' || Buffer[End] == '\v' || Buffer[End] == '\f' || Buffer[End] == '\r' || Buffer[End] == ' ' || Buffer[End] == '\n')
    {
        if(End < Len)
        {
            Start = ++End;
            Count++;
            if (Buffer[End] == C('\n'))
            {
                LastLineEnd = End;
                Line++;
                if(SkipLine == false)
                    return Count;
            }

        }
        else
            return Count;
    }

    return Count;
    
}
void TokenReader::NextCharUntil(TCHAR Ch)
{
    while(End < Len)
    {
        if(Buffer[End] == Ch)
            break;
        End++;
        if (Buffer[End] == C('\n'))
        {
            LastLineEnd = End;
            Line++;
        }
    }
}
void TokenReader::NextCharUntil(TCHAR Ch1, TCHAR Ch2)
{
    while(End < Len)
    {
        if(Buffer[End] == Ch1 || Buffer[End] == Ch2)
            break;
        End++;
        if (Buffer[End] == C('\n'))
        {
            LastLineEnd = End;
            Line++;
        }
    }
}
void TokenReader::NextCharUntil(TCHAR Ch1, TCHAR Ch2, TCHAR Ch3)
{
    while(End < Len)
    {
        if(Buffer[End] == Ch1 || Buffer[End] == Ch2 || Buffer[End] == Ch3)
            break;
        End++;
        if (Buffer[End] == C('\n'))
        {
            LastLineEnd = End;
            Line++;
        }
    }
}
void TokenReader::NextCharUntil(TCHAR Ch1, TCHAR Ch2, TCHAR Ch3, TCHAR Ch4)
{
    while(End < Len)
    {
        if(Buffer[End] == Ch1 || Buffer[End] == Ch2 || Buffer[End] == Ch3 || Buffer[End] == Ch4)
            break;
        End++;
        if (Buffer[End] == C('\n'))
        {
            LastLineEnd = End;
            Line++;
        }
    }
}

int TokenReader::GetLine()
{
    return Line;
}
int TokenReader::GetCol()
{
    return End - LastLineEnd;
}

void TokenReader::LoadFromMemory(const TCHAR* SrcBuffer, int SrcLen)
{
    Buffer = new TCHAR[SrcLen+1];
    memcpy(Buffer, SrcBuffer, sizeof(TCHAR) * SrcLen);
    Buffer[SrcLen] = 0;
    Len = SrcLen;
    SourceFileName = T("Memory code");
}

void TokenReader::LoadFromFile(const TCHAR* FileName)
{
    FILE* file = fopen(T2A(FileName), "rb");
    if(file == nullptr)
    {
        ERROR(LogStream, T("Cannot open file %s"), FileName);
        return;
    }

    fseek(file, 0, SEEK_END);
    long SrcLen = ftell(file);
    Buffer = new TCHAR[SrcLen + 1];

    fseek(file, 0, SEEK_SET);
    fread(Buffer, 1, SrcLen, file);
    fclose(file);

    Buffer[SrcLen] = 0;
    Len = SrcLen;
    SourceFileName = FileName;
}

OLString& TokenReader::GetSourceFileName()
{
    return SourceFileName;
}


}
