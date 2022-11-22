/*
Copyright 2022 Dong Gen

Use of this source code is governed by an MIT-style
license that can be found in the LICENSE file or at
https://opensource.org/licenses/MIT.
*/

#include "TextEncoding.h"
#include "OLString.h"
#include <string.h>

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


int Wtoi(const wchar_t* Start)
{
    char Tmp[64] = {0};
    wchar_t* Curr = (wchar_t*) Start;
    int Idx = 0;
    for(Idx = 0; Idx < 63; Idx++)
    {
        Tmp[Idx] = Start[Idx];
        if(Start[Idx] == 0)
            break;
    }
    Tmp[Idx] = 0;

    return  atoi(Tmp);

}

// An implementation of vswprintf without awareness of locale. Refered to Unreal Engine
int OL_vswprintf(wchar_t* const DstBuffer, size_t const DstBufferSize, wchar_t const* const Fmt, va_list Args)
{

	if (Fmt == nullptr)
	{
		if ((DstBufferSize > 0) && (DstBuffer != nullptr))
		{
			*DstBuffer = 0;
		}
		return 0;
	}

	int FmtLen = wcslen(Fmt);
	const TCHAR *Src = Fmt;

	TCHAR *Dst = DstBuffer;
	TCHAR *EndDst = Dst + (DstBufferSize - 1);

	while ((*Src) && (Dst < EndDst))
	{
		if (*Src != C('%'))
		{
			*Dst = *Src;
			Dst++;
			Src++;
			continue;
		}

		const TCHAR *Percent = Src;
		int FieldLen = 0;
		int PrecisionLen = -1;

		Src++; // skip the '%' char...

		while (*Src == C(' '))
		{
			*Dst = C(' ');
			Dst++;
			Src++;
		}

		// Skip modifier flags that don't need additional processing;
		// they still get passed to snprintf() below based on the conversion.
		if (*Src == C('+'))
		{
			Src++;
		}

		// check for field width requests...
		if ((*Src == C('-')) || ((*Src >= C('0')) && (*Src <= C('9'))))
		{
			const TCHAR *Cur = Src + 1;
			while ((*Cur >= C('0')) && (*Cur <= C('9')))
			{
				Cur++;
			}

			FieldLen = Wtoi(Src);
			Src = Cur;
		}

		// check for dynamic field requests
		if (*Src == '*')
		{
			FieldLen = va_arg(Args, int);
			Src++;
		}

		if (*Src == C('.'))
		{
			const TCHAR *Cur = Src + 1;
			while ((*Cur >= C('0')) && (*Cur <= C('9')))
			{
				Cur++;
			}

			PrecisionLen = Wtoi(Src + 1);
			Src = Cur;
		}

		// Check for 'ls' field, change to 's'
		if ((Src[0] == C('l') && Src[1] == C('s')))
		{
			Src++;
		}

		switch (*Src)
		{
		case C('%'):
		{
			Src++;
			*Dst = C('%');
			Dst++;
			break;
		}

		case ('c'):
		{
			TCHAR Val = (TCHAR) va_arg(Args, int);
			Src++;
			*Dst = Val;
			Dst++;
			break;
		}

		case C('d'):
		case C('i'):
		case C('X'):
		case C('x'):
		case C('u'):
		{
			Src++;
			int Val = va_arg(Args, int);
			char AnsiNum[64];
			char FmtBuf[30] = {0};

			// limit width to the buffer size
            if(FieldLen > static_cast<int>(sizeof(AnsiNum)) - 1)
                FieldLen = static_cast<int>(sizeof(AnsiNum)) - 1;

			// Yes, this is lame.
			int CpyIdx = 0;
			while (Percent < Src && CpyIdx < sizeof(FmtBuf) / sizeof(char))
			{
				if (*Percent != C('*'))
				{
					FmtBuf[CpyIdx] = (char) *Percent;
					++CpyIdx;
				}
				else
				{
					snprintf(&FmtBuf[CpyIdx], sizeof(FmtBuf) - CpyIdx, "%d", FieldLen);
					while (CpyIdx < (sizeof(FmtBuf) / sizeof(char)) && FmtBuf[CpyIdx] != 0)
					{
						++CpyIdx;
					}
				}
				++Percent;
			}
			FmtBuf[CpyIdx] = 0;

			int RetCnt = snprintf(AnsiNum, sizeof(AnsiNum), FmtBuf, Val);
			if ((Dst + RetCnt) > EndDst)
			{
				return -1;	// Fail - the app needs to create a larger buffer and try again
			}
			for (int i = 0; i < RetCnt; i++)
			{
				*Dst = (TCHAR) AnsiNum[i];
				Dst++;
			}
			break;
		}

		case C('z'):
		case C('Z'):
		{
			Src += 2;

			size_t Val = va_arg(Args, size_t);

			char AnsiNum[64];
			char FmtBuf[30] = {0};

			// limit width to the buffer size
			//FieldLen = FMath::Min(static_cast<int>(sizeof(AnsiNum)) - 1, FieldLen);
            if(FieldLen > static_cast<int>(sizeof(AnsiNum)) - 1)
                FieldLen = static_cast<int>(sizeof(AnsiNum)) - 1;


			// Yes, this is lame.
			int CpyIdx = 0;
			while (Percent < Src && CpyIdx <(sizeof(FmtBuf) / sizeof(char)))
			{
				if (*Percent != '*')
				{
					FmtBuf[CpyIdx] = (char) *Percent;
					++CpyIdx;
				}
				else
				{
					snprintf(&FmtBuf[CpyIdx], sizeof(FmtBuf) - CpyIdx, "%d", FieldLen);
					while (CpyIdx < (sizeof(FmtBuf) / sizeof(char)) && FmtBuf[CpyIdx] != 0)
					{
						++CpyIdx;
					}
				}
				++Percent;
			}
			FmtBuf[CpyIdx] = 0;

			int RetCnt = snprintf(AnsiNum, sizeof(AnsiNum), FmtBuf, Val);
			if ((Dst + RetCnt) > EndDst)
			{
				return -1;	// Fail - the app needs to create a larger buffer and try again
			}
			for (int i = 0; i < RetCnt; i++)
			{
				*Dst = (TCHAR) AnsiNum[i];
				Dst++;
			}
			break;
		}

		case C('p'):
		{
			Src++;
			void* Val = va_arg(Args, void*);
			char AnsiNum[30];
			char FmtBuf[30];

			// Yes, this is lame.
			int CpyIdx = 0;
			while (Percent < Src && CpyIdx < (sizeof(FmtBuf) / sizeof(char)))
			{
				FmtBuf[CpyIdx] = (char) *Percent;
				Percent++;
				CpyIdx++;
			}
			FmtBuf[CpyIdx] = 0;

			int RetCnt = snprintf(AnsiNum, sizeof(AnsiNum), FmtBuf, Val);
			if ((Dst + RetCnt) > EndDst)
			{
				return -1;	// Fail - the app needs to create a larger buffer and try again
			}
			for (int i = 0; i < RetCnt; i++)
			{
				*Dst = (TCHAR) AnsiNum[i];
				Dst++;
			}
			break;
		}

		case ('l'):
		case ('I'):
		case ('h'):
		{
			int RemainingSize = wcslen(Src);

			// treat %ld as %d. Also shorts for %h will be promoted to ints
			if (RemainingSize >= 2 && ((Src[0] == C('l') && Src[1] == C('d')) || Src[0] == C('h')))
			{
				Src += 2;
				int Val = va_arg(Args, int);
				char AnsiNum[30];
				char FmtBuf[30];

				// Yes, this is lame.
				int CpyIdx = 0;
				while (Percent < Src && CpyIdx < (sizeof(FmtBuf) / sizeof(char)))
				{
					FmtBuf[CpyIdx] = (char) *Percent;
					Percent++;
					CpyIdx++;
				}
				FmtBuf[CpyIdx] = 0;

				int RetCnt = snprintf(AnsiNum, sizeof(AnsiNum), FmtBuf, Val);
				if ((Dst + RetCnt) > EndDst)
				{
					return -1;	// Fail - the app needs to create a larger buffer and try again
				}
				for (int i = 0; i < RetCnt; i++)
				{
					*Dst = (TCHAR) AnsiNum[i];
					Dst++;
				}
				break;
			}
			// Treat %lf as a %f
			else if (RemainingSize >= 2 && Src[0] == 'l' && Src[1] == 'f')
			{
				Src += 2;
				double Val = va_arg(Args, double);
				char AnsiNum[30];
				char FmtBuf[30];

				// Yes, this is lame.
				int CpyIdx = 0;
				while (Percent < Src && CpyIdx < (sizeof(FmtBuf) / sizeof(char)))
				{
					FmtBuf[CpyIdx] = (char) *Percent;
					Percent++;
					CpyIdx++;
				}
				FmtBuf[CpyIdx] = 0;

				int RetCnt = snprintf(AnsiNum, sizeof(AnsiNum), FmtBuf, Val);
				if ((Dst + RetCnt) > EndDst)
				{
					return -1;	// Fail - the app needs to create a larger buffer and try again
				}
				for (int i = 0; i < RetCnt; i++)
				{
					*Dst = (TCHAR) AnsiNum[i];
					Dst++;
				}
				break;
			}

			if (RemainingSize >= 2 && (Src[0] == C('l') && Src[1] != C('l') && Src[1] != C('u') && Src[1] != C('x')))
			{				
				Src++; 
				break;
			}
			else if (RemainingSize >= 3 && Src[0] == C('I') && (Src[1] != C('6') || Src[2] != C('4')))
			{
				Src++; 
				break;
			}

			// Yes, this is lame.
			int CpyIdx = 0;
			unsigned long long Val = va_arg(Args, unsigned long long);
			char AnsiNum[60];
			char FmtBuf[30];
			if (Src[0] == C('l'))
			{
				Src += 3;
			}
			else
			{
				Src += 4;
				strcpy(FmtBuf, "%L");
				Percent += 4;
				CpyIdx = 2;
			}

			while (Percent < Src && CpyIdx < (sizeof(FmtBuf) / sizeof(char)))
			{
				FmtBuf[CpyIdx] = (char) *Percent;
				Percent++;
				CpyIdx++;
			}
			FmtBuf[CpyIdx] = 0;

			int RetCnt = snprintf(AnsiNum, sizeof(AnsiNum), FmtBuf, Val);
			if ((Dst + RetCnt) > EndDst)
			{
				return -1;	// Fail - the app needs to create a larger buffer and try again
			}
			for (int i = 0; i < RetCnt; i++)
			{
				*Dst = (TCHAR) AnsiNum[i];
				Dst++;
			}
			break;
		}

		case C('f'):
		case C('e'):
		case C('g'):
		{
			Src++;
			double Val = va_arg(Args, double);
			// doubles in the form of 1e+9999 can get quite large, make sure we have enough room for them
			char AnsiNum[48];
			char FmtBuf[30];

			// Yes, this is lame.
			int CpyIdx = 0;
			while (Percent < Src && CpyIdx < (sizeof(FmtBuf) / sizeof(char)))
			{
				FmtBuf[CpyIdx] = (char) *Percent;
				Percent++;
				CpyIdx++;
			}
			FmtBuf[CpyIdx] = 0;

			int RetCnt = snprintf(AnsiNum, sizeof(AnsiNum), FmtBuf, Val);
			if ((Dst + RetCnt) > EndDst)
			{
				return -1;	// Fail - the app needs to create a larger buffer and try again
			}
			if (RetCnt >= (sizeof(AnsiNum) / sizeof(char)) )
			{
				// We should print what we have written into AnsiNum but ensure we null terminate before printing
				AnsiNum[sizeof(AnsiNum) / sizeof(char) - 1] = C('\0');
			}
			for (int i = 0; i < RetCnt; i++)
			{
				*Dst = (TCHAR) AnsiNum[i];
				Dst++;
			}
			break;
		}

		case 's':
		{
			Src++;
			static const TCHAR* Null = T("(null)");
			const TCHAR *Val = va_arg(Args, TCHAR *);
			if (Val == nullptr)
			{
				Val = Null;
			}

			int RetCnt = wcslen(Val);
			//int Spaces = FPlatformMath::Max(FPlatformMath::Abs(FieldLen) - RetCnt, 0);

            int Spaces = (FieldLen < 0 ? -FieldLen : FieldLen) - RetCnt;
            if(Spaces < 0)
                Spaces = 0;

			if ((Dst + RetCnt + Spaces) > EndDst)
			{
				return -1;	// Fail - the app needs to create a larger buffer and try again
			}
			if (Spaces > 0 && FieldLen > 0)
			{
				for (int i = 0; i < Spaces; i++)
				{
					*Dst = T(' ');
					Dst++;
				}
			}
			for (int i = 0; i < RetCnt; i++)
			{
				*Dst = *Val;
				Dst++;
				Val++;
			}
			if (Spaces > 0 && FieldLen < 0)
			{
				for (int i = 0; i < Spaces; i++)
				{
					*Dst = T(' ');
					Dst++;
				}
			}
			break;
		}

		default:
		
			Src++;  // skip char, I guess.
			break;
		}
	}

	// Check if we were able to finish the entire format string
	// If not, the app needs to create a larger buffer and try again
	if (*Src)
	{
		return -1;
	}

	*Dst = 0;  // null terminate the new string.
	return Dst - DstBuffer;
}


}