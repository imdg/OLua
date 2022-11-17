#include "TextEncoding.h"
#include "OLString.h"
namespace OL
{


int Utf8ToWchar(const char* Src, int SrcSize, wchar_t* Dst, int DstSize)
{

    int i = 0;
    int UsedDstSize = 0;

    while (i < SrcSize)
    {
        unsigned long uni;
        int todo;
        bool error = false;
        unsigned char ch = Src[i++];
        if(ch == 0)
            break;
        if (ch <= 0x7F)
        {
            uni = ch;
            todo = 0;
        }
        else if (ch <= 0xBF)
        {
            return -1;
        }
        else if (ch <= 0xDF)
        {
            uni = ch&0x1F;
            todo = 1;
        }
        else if (ch <= 0xEF)
        {
            uni = ch&0x0F;
            todo = 2;
        }
        else if (ch <= 0xF7)
        {
            uni = ch&0x07;
            todo = 3;
        }
        else
        {
            return -1;
        }
        for (int j = 0; j < todo; ++j)
        {
            if (i == SrcSize)
                return -1;
            unsigned char ch = Src[i++];
            if (ch < 0x80 || ch > 0xBF)
                return -1;
            uni <<= 6;
            uni += ch & 0x3F;
        }
        if (uni >= 0xD800 && uni <= 0xDFFF)
            return -1;
        if (uni > 0x10FFFF)
            return -1;

        if(Dst == nullptr)
        {
            UsedDstSize++;
        }
        else
        {
            if(UsedDstSize >= DstSize)
                break;
            Dst[UsedDstSize++] = (wchar_t)uni;
        }

    }
    if(UsedDstSize < DstSize && Dst != nullptr)
    {
        Dst[UsedDstSize] = L'\0';
    }
    return (UsedDstSize + 1)  * sizeof(wchar_t);
}






ToUtf8Helper::ToUtf8Helper(const OLString& Src)
{
    Src.ToUTF8(Buffer);
}


char* ToUtf8Helper::Get()
{
    return Buffer.Data();
}



}