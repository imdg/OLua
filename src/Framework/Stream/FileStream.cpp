#include "FileStream.h"
#include <stdio.h>
#include "Env.h"
#include "TextEncoding.h"
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
    //Always write UTF8 to file
    fprintf(FilePtr, "%s", TS2U(OLString(InBuffer)));
}

void FileStream::OpenWrite(OLString FileName)
{
    FilePtr = t_fopen(FileName, "wb");
}
void FileStream::Close()
{
    fclose(FilePtr);
}

void FileStream::Flush()
{
    fflush(FilePtr);
}
}