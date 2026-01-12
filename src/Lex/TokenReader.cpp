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
#include "Env.h"
namespace OL
{

TokenReader::TokenReader() 
    : Start(0), End(0), Buffer(nullptr), Len(0)
    , Line(1), LastLineEnd(-1)
{

}

TokenReader::~TokenReader()
{
    //delete[] Buffer;
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

    //VERBOSE(LogMisc, T("NextChar, %d line: %d, col: %d"), (int)(Buffer[End]), Line, End-LastLineEnd );
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
    while(Buffer[End] == C('\t') || Buffer[End] == C('\v') || Buffer[End] == C('\f') || Buffer[End] == C('\r') || Buffer[End] == C(' ') || Buffer[End] == C('\n'))
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
        //VERBOSE(LogMisc, T("NextCharUntil 1, %d line: %d, col: %d"), (int)(Buffer[End]), Line, End-LastLineEnd );
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
        //VERBOSE(LogMisc, T("NextCharUntil 2, %d line: %d, col: %d"), (int)(Buffer[End]), Line, End-LastLineEnd );
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
        //VERBOSE(LogMisc, T("NextCharUntil 3, %d line: %d, col: %d"), (int)(Buffer[End]), Line, End-LastLineEnd );
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
        //VERBOSE(LogMisc, T("NextCharUntil 4, %d line: %d, col: %d"), (int)(Buffer[End]), Line, End-LastLineEnd );
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

int TokenReader::GetPos()
{
    return End;
}

void TokenReader::LoadFromMemory(const TCHAR* SrcBuffer, int SrcLen)
{
    BufferString = SrcBuffer;
    Buffer = (TCHAR*)BufferString.CStr();
    Len = BufferString.Len();

    SourceFileName = T("Memory code");
}

void TokenReader::LoadFromFile(const TCHAR* FileName)
{
    FILE* file = t_fopen(FileName, "rb");
    if(file == nullptr)
    {
        ERROR(LogStream, T("Cannot open file %s"), FileName);
        return;
    }

    fseek(file, 0, SEEK_END);
    long SrcLen = ftell(file);
    
    char* TmpBuffer = new char[SrcLen + 1];

    fseek(file, 0, SEEK_SET);
    fread(TmpBuffer, 1, SrcLen, file);
    fclose(file);

    TmpBuffer[SrcLen] = 0;

    BufferString = OLString::FromUTF8(TmpBuffer);
    delete[] TmpBuffer;
    Buffer = (TCHAR*)BufferString.CStr();
    Len = BufferString.Len();
    SourceFileName = FileName;

    //VERBOSE(LogMisc, T("Load File: %s"), SourceFileName.CStr());
}

OLString& TokenReader::GetSourceFileName()
{
    return SourceFileName;
}


}
