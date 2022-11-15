#include "TextBuilder.h"
#include <stdarg.h>

namespace OL
{

TextSeg::TextSeg()
{
    Prev = nullptr;
    Next = nullptr;
    PtrData = nullptr;
    IntData = 0;
    SegType = TST_Text;
}




TextParagraph::TextParagraph(TextBuilder* InOwner): Owner(InOwner)
{
    Head = nullptr;
    Tail = nullptr;
}

TextParagraph::TextParagraph(const TextParagraph& Src)
: Head(Src.Head), Tail(Src.Tail), Owner(Src.Owner)
{
}

TextParagraph& TextParagraph::Append(TextSeg& Src)
{
    if(Head == nullptr)
    {
        Head = &Src;
        Tail = &Src;
        Src.Prev = nullptr;
        Src.Next = nullptr;

    }
    else
    {
        Tail->Next = &Src;
        Src.Prev = Tail;
        Src.Next = nullptr;

        Tail = Tail->Next;
    }
    Owner->TotalLen += Src.Text.Len();
    return *this;
}

TextParagraph& TextParagraph::Append(const OLString& InText)
{
    return Append(Owner->NewSeg(InText));
}
TextParagraph& TextParagraph::Append(const TCHAR* InText)
{
    return Append(Owner->NewSeg(InText));
}

TextParagraph& TextParagraph::Merge(SPtr<TextParagraph> Src)
{
    if(Src->Head == nullptr)
        return *this;
    if(Head == nullptr)
    {
        Head = Src->Head;
        Tail = Src->Tail;
        Src->Reset();
    }
    else
    {
        Tail->Next = Src->Head;
        Src->Head->Prev = Tail;
        Tail = Src->Tail;

        Src->Reset();
    }
    return *this;
}

void TextParagraph::Reset()
{
    Head = nullptr;
    Tail = nullptr;
    Owner = nullptr;
}


TextParagraph& TextParagraph::Indent()
{
    TextSeg& Seg = Owner->NewSeg();
    Seg.IntData = 0;
    Seg.SegType = TST_Indent;

    return Append(Seg);
}
TextParagraph& TextParagraph::IndentInc()
{
    TextSeg& Seg = Owner->NewSeg();
    Seg.IntData = 0;
    Seg.SegType = TST_IndentInc;

    return Append(Seg);
}
TextParagraph& TextParagraph::IndentDec()
{
    TextSeg& Seg = Owner->NewSeg();
    Seg.IntData = 0;
    Seg.SegType = TST_IndentDec;

    return Append(Seg);
}

TextParagraph& TextParagraph::NewLine()
{
    return Append("\n");
}

TextParagraph& TextParagraph::Hold(void* PtrData, int IntData)
{
    TextSeg& Seg = Owner->NewSeg(T(""));
    Seg.SegType = TST_PlaceHolderByData;
    Seg.PtrData = PtrData;
    Seg.IntData = IntData;
    
    return Append(Seg);
}

TextParagraph& TextParagraph::Hold(OLString Name)
{
    TextSeg& Seg = Owner->NewSeg(T(""));
    Seg.SegType = TST_PlaceHolderByName;
    Seg.Text = Name;

    return Append(Seg);
}

void TextParagraph::ReplaceImpl(TextSeg& Holder, SPtr<TextParagraph> Src)
{
    if(Src->Head == nullptr)
    {
        if (Holder.Prev != nullptr)
            Holder.Prev->Next = Holder.Next;
        if (Holder.Next != nullptr)
            Holder.Next->Prev = Holder.Prev;
        
    }
    else
    {
        Src->Head->Prev = Holder.Prev;
        if (Holder.Prev != nullptr)
            Holder.Prev->Next = Src->Head;
        else
            Head = Src->Head;

        Src->Tail->Next = Holder.Next;
        if (Holder.Next != nullptr)
            Holder.Next->Prev = Src->Tail;   
        else
            Tail = Src->Tail;
    }
}

int TextParagraph::ReplaceHolder(void* PtrData, int IntData, SPtr<TextParagraph> Src, bool Multi)
{
    int ReplaceCount = 0;
    TextSeg* Curr = Head;

    while(Curr != nullptr)
    {
        if(Curr->SegType == TST_PlaceHolderByData)
        {
            if(Curr->PtrData == PtrData && Curr->IntData == IntData)
            {
                ReplaceCount++;
                
                if(Multi)
                {
                    SPtr<TextParagraph> NewText = Src->Duplicate();
                    ReplaceImpl(*Curr, NewText);
                }
                else
                {
                    ReplaceImpl(*Curr, Src);
                    return 1;
                }
            }
            
        }
        Curr = Curr->Next;
    }
    return ReplaceCount;
}

int TextParagraph::ReplaceHolder(OLString Name, SPtr<TextParagraph> Src, bool Multi)
{
       int ReplaceCount = 0;
    TextSeg* Curr = Head;

    while(Curr != nullptr)
    {
        if(Curr->SegType == TST_PlaceHolderByName)
        {
            if(Curr->Text == Name)
            {
                ReplaceCount++;
                
                if(Multi)
                {
                    SPtr<TextParagraph> NewText = Src->Duplicate();
                    ReplaceImpl(*Curr, NewText);
                }
                else
                {
                    ReplaceImpl(*Curr, Src);
                    return 1;
                }
            }
            
        }
        Curr = Curr->Next;
    }

    return ReplaceCount;
}

SPtr<TextParagraph> TextParagraph::Duplicate()
{
    SPtr<TextParagraph> NewText = Owner->NewParagraph();
    TextSeg* Curr = Head;
    while(Curr != nullptr)
    {
        TextSeg& NewSeg = Owner->NewSeg();
        NewSeg.IntData = Curr->IntData;
        NewSeg.PtrData = Curr->PtrData;
        NewSeg.SegType = Curr->SegType;
        NewSeg.Text = Curr->Text;

        NewText->Append(NewSeg);
        Curr = Curr->Next;
    }

    return NewText;
}


#define BASE_SIZE 512

TextParagraph& TextParagraph::AppendF(const TCHAR* Fmt, ...)
{
    TCHAR* Buffer = (TCHAR*)malloc(BASE_SIZE * sizeof(TCHAR));
    int CurrSize = BASE_SIZE;
    int Written = -1;

    va_list ap;
    va_start(ap, Fmt);
    Written = vsnprintf(Buffer, CurrSize, Fmt, ap);
    va_end(ap);

    while(Written >= CurrSize - 1)
    {
        CurrSize *= 2;
        Buffer = (TCHAR*)realloc(Buffer, CurrSize);
        va_list ap2;
        va_start(ap2, Fmt);
        Written = vsnprintf(Buffer, CurrSize, Fmt, ap2);
        va_end(ap2);
    }

    TextSeg& Ret = Owner->NewSeg(Buffer);
    free(Buffer);
    return Append(Ret);
    
}


TextSegPool::TextSegPool(int InMax) : Max(InMax), Used(0)
{
    Mem = new byte[Max * sizeof(TextSeg)];
}

TextSegPool::TextSegPool(const TextSegPool& Src)
    : Mem(Src.Mem), Max(Src.Max), Used(Src.Used)
{

}


TextSeg* TextSegPool::NewSeg()
{
    if(Used >= Max)
        return nullptr;
    else
    {
        return new(Mem + sizeof(TextSeg) * (Used++)) TextSeg();
    }
}

TextBuilder::TextBuilder() 
{
    ActivePool = nullptr;
    NewPool();
    Root = NewParagraph();
    TotalLen = 0;
    DebugMode = false;
}

void TextBuilder::NewPool()
{
    TextSegPool& NewPool = Pools.AddConstructed(512);
    ActivePool = &NewPool;

}

TextSeg& TextBuilder::NewSeg()
{
    TextSeg* Ret = ActivePool->NewSeg();
    if(Ret == nullptr)
    {
        NewPool();
        Ret = ActivePool ->NewSeg();
    }
    return *Ret;
}

TextSeg&    TextBuilder::NewSeg(const OLString& Text)
{
    TextSeg& Ret = NewSeg();
    Ret.Text = Text;
    return Ret;
}
TextSeg&    TextBuilder::NewSeg(const TCHAR* Text)
{
    TextSeg& Ret = NewSeg();
    Ret.Text = Text;
    return Ret;
}

SPtr<TextParagraph> TextBuilder::NewParagraph()
{
    return new TextParagraph(this);
}

static const TCHAR* StaticIndent[] =
{
    T(""),
    T("    "),
    T("        "),
    T("            "),
    T("                "),
    T("                    "),
    T("                        "),
    T("                            "),
    T("                                "),
    T("                                    "),
    T("                                        "),
    T("                                            "),
    T("                                                "),
    T("                                                    "),
    T("                                                        "),
    T("                                                            "),
    T("                                                                "),
    T("                                                                    "),
    T("                                                                        "),
    T("                                                                            "),
    T("                                                                                "),
    T("                                                                                    "),
};

static int StaticIndentLen = 22;
const TCHAR* TextBuilder::GetIndentText(int Indent)
{
    if(Indent < StaticIndentLen)
        return StaticIndent[Indent];
    else
    {
        OLString IndentText;
        for(int i = 0; i < Indent; i++)
        {
            IndentText += "    ";
        }
        static TCHAR Buffer[512];

        IndentText.Strcpy(Buffer, 512);
        return Buffer;
    }
    
}


void TextBuilder::WriteStream(BaseStream& Stream)
{
    WriteStreamFrom(Stream, Root);
}

void TextBuilder::WriteStreamFrom(BaseStream& Stream, SPtr<TextParagraph> From)
{
    TextSeg* Curr = From->Head;
    int Indent = 0;
    while(Curr != nullptr)
    {
        switch (Curr->SegType)
        {
        case TST_Text:
            Stream.WriteText(Curr->Text.CStr());
            break;
        case TST_Indent:
            if(DebugMode)
                Stream.WriteFormat(T("%d"), Indent);
            Stream.WriteText(GetIndentText((Indent - 1) < 0 ? 0 : (Indent - 1) ));
            break;
        case TST_IndentInc:
            Indent++;
            break;
        case TST_IndentDec:
            Indent--;
            if(Indent  < 0)
                Indent = 0;
            break;
        case TST_PlaceHolderByName:
            if(DebugMode)
                Stream.WriteFormat(T("${%s}"), Curr->Text.CStr());
            break;
        default:
            break;
        }
        Curr = Curr->Next;
    }
}

}