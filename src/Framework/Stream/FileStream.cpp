#include "FileStream.h"
#include <stdio.h>

namespace OL
{
FileStream::FileStream() : FilePtr(nullptr)
{

}
void FileStream::Write(byte* InBuffer, int InSize)
{
    for(int i = 0; i < InSize; i++)
    {
        fprintf(FilePtr, "%x ", (int)InBuffer[i]);
    }
}

void FileStream::WriteText(const TCHAR* InBuffer)
{
    fprintf(FilePtr, "%s", T2A(InBuffer));
}

void FileStream::OpenWrite(OLString FileName)
{
    FilePtr = fopen(T2A(FileName.CStr()), "wb");
}
void FileStream::Close()
{
    fclose(FilePtr);
}
}