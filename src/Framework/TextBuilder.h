#pragma once
#include "Common.h"
#include "BaseStream.h"

namespace OL
{
class TextBuilder;

enum ETextSegType
{
    TST_Text,
    TST_Indent,
    TST_IndentInc,
    TST_IndentDec,
    TST_PlaceHolderByData,
    TST_PlaceHolderByName,
};

class TextSeg
{
public:
    TextSeg();

    OLString Text;
    ETextSegType    SegType;
    
    void*       PtrData;
    int         IntData;

    TextSeg* Prev;
    TextSeg* Next;
};

class TextParagraph
{
public:
    TextSeg* Head;
    TextSeg* Tail;
    TextBuilder* Owner;

    TextParagraph(TextBuilder* InOwner);
    TextParagraph(const TextParagraph& Src);

    TextParagraph& Append(TextSeg& Src);
    TextParagraph& Append(const OLString& InText);
    TextParagraph& Append(const TCHAR* InText);
    TextParagraph& AppendF(const TCHAR* Fmt, ...);
    TextParagraph& Merge(SPtr<TextParagraph> Src);

    TextParagraph& Indent();
    TextParagraph& IndentInc();
    TextParagraph& IndentDec();
    TextParagraph& NewLine();

    TextParagraph& Hold(void* PtrData, int IntData);
    TextParagraph& Hold(OLString Name);


    int ReplaceHolder(void* PtrData, int IntData, SPtr<TextParagraph> Src, bool Multi);
    int ReplaceHolder(OLString Name, SPtr<TextParagraph> , bool Multi);

    void ReplaceImpl(TextSeg& Holder, SPtr<TextParagraph> Src);

    void Reset();

    SPtr<TextParagraph> Duplicate();
};

class TextSegPool
{
public:
    byte*   Mem;
    int     Max;
    int     Used;

    TextSegPool(int InMax);
    TextSegPool(const TextSegPool& Src);
    TextSeg* NewSeg();
};


class TextBuilder
{
public:
    TextBuilder();
    OLList<TextSeg*> Segments;

    OLList<TextSegPool> Pools;
    TextSegPool*    ActivePool;

    TextSeg&    NewSeg();
    TextSeg&    NewSeg(const OLString& Text);
    TextSeg&    NewSeg(const TCHAR* Text);

    SPtr<TextParagraph> NewParagraph();


    SPtr<TextParagraph>   Root;
    
    const TCHAR* GetIndentText(int Indent);

    void NewPool();

    void WriteStream(BaseStream& Stream);
    void WriteStreamFrom(BaseStream& Stream, SPtr<TextParagraph> From);
    int TotalLen;
    bool DebugMode;
};
}