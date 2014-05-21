// ***********************************************************************
// CamStudio Codec
// http://www.rendersoftware.com
//
// Changes copyright 2003 RenderSoft Software and Web Publishing, and distributed under
// the terms of the GNU General Public License, v2 or later.  See
// http://www.gnu.org/copyleft/gpl.html.
//
// The code for this codec is based on the LZO library by Markus Franz Xaver Johannes Oberhumer,
// zlib compression library by Jean-loup Gailly and Mark Adler
// and the HUFFYUV codec  by Ben Rudiak-Gould.
//
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// ***********************************************************************

// ***********************************************************************
// LZO real-time data compression library.
// Copyright (C) 2000 Markus Franz Xaver Johannes Oberhumer

// The LZO library is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.

// The LZO library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with the LZO library; see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

// Markus F.X.J. Oberhumer
// <markus.oberhumer@jk.uni-linz.ac.at>
//  http://wildsau.idv.uni-linz.ac.at/mfx/lzo.htm
// ***********************************************************************

// ***********************************************************************
/* zlib.h -- interface of the 'zlib' general purpose compression library
version 1.1.4, March 11th, 2002

Copyright (C) 1995-2002 Jean-loup Gailly and Mark Adler

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

Jean-loup Gailly        Mark Adler
jloup@gzip.org          madler@alumni.caltech.edu

The data format used by the zlib library is described by RFCs (Request for
Comments) 1950 to 1952 in the files ftp://ds.internic.net/rfc/rfc1950.txt
(zlib format), rfc1951.txt (deflate format) and rfc1952.txt (gzip format).
*/
// ***********************************************************************

// ***********************************************************************
//
// Huffyuv v1.3.1, by Ben Rudiak-Gould.
// http://www.math.berkeley.edu/~benrg/huffyuv.html
//
// Based on MSYUV sample code, which is:
// Copyright (c) 1993 Microsoft Corporation.
// All Rights Reserved.
//
// Changes copyright 2000 Ben Rudiak-Gould, and distributed under
// the terms of the GNU General Public License, v2 or later.  See
// http://www.gnu.org/copyleft/gpl.html.
//
// I edit these files in 10-point Verdana, a proportionally-spaced font.
// You may notice formatting oddities if you use a monospaced font.
//
// ***********************************************************************

//this version is discarded because the color space does not introduce a huge amount of space saving and is difficult to implement
//left colorspace for difference...note the difference is in YUV space ...so ned to keep internal buffer when decompressing

#include "camcodec.h"
#include "resource.h"
#include "minilzo/minilzo.h"
#include "../CxImage/zlib/zlib.h"
#include <stdio.h>

TCHAR szDescription[] = TEXT("CamStudio Lossless Codec v1.5");
TCHAR szName[]        = TEXT("CamCodec");

#define HEAP_ALLOC(var,size) \
	long __LZO_MMODEL var [ ((size) + (sizeof(long) - 1)) / sizeof(long) ]

static HEAP_ALLOC(wrkmem,LZO1X_1_MEM_COMPRESS);

#define VERSION         0x00010003      // 1.3

/********************************************************************
********************************************************************/

/*void Msg(const char fmt[], ...)
{
	DWORD written;
	char buf[2000];
	va_list val;

	va_start(val, fmt);
	wvsprintf(buf, fmt, val);

	const COORD _80x50 = {80,50};
	static BOOL startup = (AllocConsole(), SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), _80x50));
	WriteConsole(GetStdHandle(STD_OUTPUT_HANDLE), buf, lstrlen(buf), &written, 0);
}*/

int AppFlags()
{
	static int flags = -1;
	if (flags < 0)
	{
		flags = 0;
		TCHAR apppath[MAX_PATH];
		if (GetModuleFileName(NULL, apppath, MAX_PATH)) {
			TCHAR* appname = strrchr(apppath, '\\');
			appname = appname ? appname+1 : apppath;
//			Msg("App name is %s; ", appname);
			if (!lstrcmpi(appname, TEXT("premiere.exe")))
				flags = 1;
			if (!lstrcmpi(appname, TEXT("veditor.exe")))
				flags = 1;
			if (!lstrcmpi(appname, TEXT("avi2mpg2_vfw.exe")))
				flags = 1;
			if (!lstrcmpi(appname, TEXT("bink.exe")))
				flags = 1;
			if (!lstrcmpi(appname, TEXT("afterfx.exe")))
				flags = 2;
//			Msg("flags=%d\n", flags);
		}
	}

	return flags;
}

/********************************************************************
********************************************************************/

// Constructor
CodecInst::CodecInst()
{
	prevFrame = NULL; 
	diffFrame = NULL; 
	diffinput = NULL; 
	m_gzip_level = 6;
	m_algorithm = 0; 

	m_autokeyframe = 0;
	m_autokeyframe_rate = 20;
	m_currentFrame = 0;

	m_compressionHasBegun = 0;
	m_decompressionHasBegun = 0;
}

CodecInst* Open(ICOPEN* icinfo)
{
	if (icinfo && icinfo->fccType != ICTYPE_VIDEO)
		return NULL;

	CodecInst* pinst = new CodecInst();

	if (icinfo) icinfo->dwError = pinst ? ICERR_OK : ICERR_MEMORY;

	return pinst;
}

DWORD Close(CodecInst* pinst)
{
	//    delete pinst;       // this caused problems when deleting at app close time
	return 1;
}

/********************************************************************
********************************************************************/

BOOL CodecInst::QueryAbout() { return TRUE; }

static BOOL CALLBACK AboutDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM /*lParam*/)
{
	if (uMsg == WM_COMMAND)
	{
		switch (LOWORD(wParam))
		{
			case IDOK:
				EndDialog(hwndDlg, 0);
				break;
			case IDC_HOMEPAGE:
				ShellExecute(NULL, NULL, "http://www.rendersoftware.com/products/camstudio/codecs.htm", NULL, NULL, SW_SHOW);
				break;
			case IDC_EMAIL:
				ShellExecute(NULL, NULL, "mailto:support@rendersoftware.com", NULL, NULL, SW_SHOW);
				break;
		}
	}

	return FALSE;
}

DWORD CodecInst::About(HWND hwnd)
{
	//VC++ 6
	//DialogBox(hmoduleCamcodec, MAKEINTRESOURCE(IDD_ABOUT), hwnd,  AboutDialogProc);  

	//VC++ 5
	//DialogBox(hmoduleCamcodec, MAKEINTRESOURCE(IDD_ABOUT), hwnd, (int (__stdcall *)(void)) AboutDialogProc);  

	return ICERR_OK;
}

#define UDM_SETRANGE            (WM_USER+101)
#define UDM_SETPOS              (WM_USER+103)
#define UDM_SETACCEL            (WM_USER+107)
typedef struct _UDACCEL
{
	UINT nSec;
	UINT nInc;
}
UDACCEL, FAR *LPUDACCEL;

static BOOL CALLBACK ConfigureDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_INITDIALOG)
	{
		int l_algorithm = GetPrivateProfileInt("GENERAL", "algorithm", 0, "camcodec100.ini"); 
		int l_gzip_level = GetPrivateProfileInt("GENERAL", "gzip_level", 6, "camcodec100.ini"); 

		//int l_autokeyframe = GetPrivateProfileInt("GENERAL", "autokeyframe", 0, "camcodec100.ini");
		//int l_autokeyframe_rate = GetPrivateProfileInt("GENERAL", "autokeyframe_rate", 20, "camcodec100.ini");

		//char gzip_levelstr[2];
		//sprintf(gzip_levelstr,"%d",l_gzip_level);
		//SetWindowText(GetDlgItem(hwndDlg, IDC_EDIT1),gzip_levelstr);

		UDACCEL acc[1];
		acc[0].nSec = 1; 
		acc[0].nInc = 1; 		

		HWND hSpinCtrlWnd = GetDlgItem(hwndDlg, IDC_SPIN1);		
		int lower = 1;
		int upper = 9;
		::SendMessage(hSpinCtrlWnd, UDM_SETRANGE, 0, MAKELPARAM(upper, lower));
		::SendMessage(hSpinCtrlWnd, UDM_SETACCEL, 1, (LPARAM)acc); 
		::SendMessage(hSpinCtrlWnd, UDM_SETPOS, 0, MAKELPARAM(l_gzip_level, 0));

		if (l_algorithm == 0)
		{
			CheckDlgButton(hwndDlg, IDC_RADIO1, BST_CHECKED);
			CheckDlgButton(hwndDlg, IDC_RADIO2, BST_UNCHECKED);

		}
		else if (l_algorithm == 1)
		{			
			CheckDlgButton(hwndDlg, IDC_RADIO1, BST_UNCHECKED);
			CheckDlgButton(hwndDlg, IDC_RADIO2, BST_CHECKED);			

		}
	} 
	else if (uMsg == WM_COMMAND)
	{
		switch (LOWORD(wParam))
		{
	case IDOK:			
		{
			char algostr[2];
			if (IsDlgButtonChecked(hwndDlg, IDC_RADIO1))
				sprintf(algostr,"0");
			else if (IsDlgButtonChecked(hwndDlg, IDC_RADIO2))
				sprintf(algostr,"1");			

			int val=6;
			char gzip_levelstr[10];
			GetWindowText(GetDlgItem(hwndDlg, IDC_EDIT1),gzip_levelstr,5);

			sscanf(gzip_levelstr,"%d",&val);

			if (val<1) val=1;
			if (val>9) val=9;

			//Msg("val %d",val);

			char gzip_level_out_str[2];
			sprintf(gzip_level_out_str,"%d",val);

			WritePrivateProfileString("GENERAL", "algorithm", algostr, "camcodec100.ini");
			WritePrivateProfileString("GENERAL", "gzip_level", gzip_level_out_str , "camcodec100.ini");
			//WritePrivateProfileString("GENERAL", "gzip_level", gzip_levelstr , "camcodec100.ini");			
		}

	case IDCANCEL:
		EndDialog(hwndDlg, 0);
		break;
	default:
		return AboutDialogProc(hwndDlg, uMsg, wParam, lParam);    // handle email and home-page buttons
		}
	}
	return FALSE;
}

BOOL CodecInst::QueryConfigure() { return TRUE; }

DWORD CodecInst::Configure(HWND hwnd)
{
	//VC++ 6
	//DialogBox(hmoduleCamcodec, MAKEINTRESOURCE(IDD_CONFIGURE), hwnd,  ConfigureDialogProc);

	//VC++ 5
	//DialogBox(hmoduleCamcodec, MAKEINTRESOURCE(IDD_CONFIGURE), hwnd, (int (__stdcall *)(void)) ConfigureDialogProc);

	return ICERR_OK;
}

// we have no state information which needs to be stored

DWORD CodecInst::GetState(LPVOID /*pv*/, DWORD /*dwSize*/) { return 0; }

DWORD CodecInst::SetState(LPVOID /*pv*/, DWORD /*dwSize*/) { return 0; }

DWORD CodecInst::GetInfo(ICINFO* icinfo, DWORD dwSize)
{
	if (icinfo == NULL)
		return sizeof(ICINFO);

	if (dwSize < sizeof(ICINFO))
		return 0;

	icinfo->dwSize            = sizeof(ICINFO);
	icinfo->fccType           = ICTYPE_VIDEO;
	icinfo->fccHandler        = FOURCC_CSCD;
	icinfo->dwFlags           = VIDCF_FASTTEMPORALC | VIDCF_TEMPORAL;

	icinfo->dwVersion         = VERSION;
	icinfo->dwVersionICM      = ICVERSION;
	MultiByteToWideChar(CP_ACP, 0, szDescription, -1, icinfo->szDescription, sizeof(icinfo->szDescription)/sizeof(WCHAR));
	MultiByteToWideChar(CP_ACP, 0, szName, -1, icinfo->szName, sizeof(icinfo->szName)/sizeof(WCHAR));

	return sizeof(ICINFO);
}

/********************************************************************
********************************************************************/

struct PrintBitmapType
{
	char s[32];
	PrintBitmapType(LPBITMAPINFOHEADER lpbi)
	{
		if (!lpbi)
			strcpy(s,  "(null)");
		else
		{
			*(DWORD*)s = lpbi->biCompression;
			s[4] = 0;
			if (!isalnum(s[0]) || !isalnum(s[1]) || !isalnum(s[2]) || !isalnum(s[3]))
				wsprintfA(s, "%x", lpbi->biCompression);
			wsprintfA(strchr(s, 0), ", bits = %d", lpbi->biBitCount);
		}
	}
};

/********************************************************************
********************************************************************/

// 0=unknown, -1=compressed YUY2, -2=compressed RGB, -3=compressed RGBA, 1=YUY2, 2=UYVY, 3=RGB 24-bit, 4=RGB 32-bit
static int GetBitmapType(LPBITMAPINFOHEADER lpbi)
{
	if (!lpbi)
		return 0;
	//if (lpbi->biCompression == FOURCC_VYUY || lpbi->biCompression == FOURCC_YUY2)
	//  return 1;
	//if (lpbi->biCompression == FOURCC_UYVY)
	//  return 2;
	//if (lpbi->biCompression == 0 || lpbi->biCompression == ' BID')
	if (lpbi->biCompression == BI_RGB)
		return (lpbi->biBitCount == 16) ? 2  : (lpbi->biBitCount == 24) ? 3 : (lpbi->biBitCount == 32) ? 4 : 0;
	if (lpbi->biCompression == FOURCC_CSCD)
		return (lpbi->biBitCount == 16) ? -2 : (lpbi->biBitCount == 24) ? -3 : (lpbi->biBitCount == 32) ? -4 : 0;
	return 0;
}

static bool CanCompress(LPBITMAPINFOHEADER lpbiIn)
{
	int intype = GetBitmapType(lpbiIn);

	//Msg("intype =  %d",intype);
	//return (intype == 3 || (intype == 4 && ((AppFlags()!=1) )));
	//return (intype == 2 || intype == 3 || (intype == 4 && ((AppFlags()!=1) )));
	return (intype == 2 || intype == 3 || intype == 4);
}

static bool CanCompress(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut) 
{
	if (!lpbiOut) return CanCompress(lpbiIn);  

	//must compress to the same bitcount ... compressed format must be biCompression == FOURCC_CSCD	
	int intype = GetBitmapType(lpbiIn);
	int outtype = GetBitmapType(lpbiOut);  

	//Msg("intype2 =  %d",intype);

	//if (intype == 3 || (intype == 4 && ((AppFlags()!=1) )))
	//if (intype == 2 || intype == 3 || (intype == 4 && ((AppFlags()!=1) )))
	if (intype == 2 || intype == 3 || intype == 4)
	{  
		if (intype == -outtype)
			return true;
	}  

	return false;
}

/********************************************************************
********************************************************************/

DWORD CodecInst::CompressQuery(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
	//Msg("CSCD CompressQuery: input = %s, output = %s\n", &PrintBitmapType(lpbiIn), &PrintBitmapType(lpbiOut));
	return CanCompress(lpbiIn, lpbiOut) ? ICERR_OK : ICERR_BADFORMAT;
}

DWORD CodecInst::CompressGetFormat(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
	if (lpbiOut == 0)
		return sizeof(BITMAPINFOHEADER);

	if (!CanCompress(lpbiIn))
		return ICERR_BADFORMAT;

	*lpbiOut = *lpbiIn;
	lpbiOut->biCompression = FOURCC_CSCD;
	int intype = GetBitmapType(lpbiIn);
	if (intype == 4)
		lpbiOut->biBitCount = 32;
	else if (intype == 3)
		lpbiOut->biBitCount = 24;
	else if (intype == 2)
		lpbiOut->biBitCount = 16;

	return ICERR_OK;
}

DWORD CodecInst::CompressGetSize(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
	// Assume 24 bpp worst-case for YUY2 input, 40 bpp for RGB.
	// The actual worst case is 43/51 bpp currently, but this is exceedingly improbable
	// (probably impossible with real captured video)

	//return lpbiIn->biWidth * lpbiIn->biHeight * ((GetBitmapType(lpbiIn) <= 2) ? 3 : 5);
	return lpbiIn->biWidth * lpbiIn->biHeight * (GetBitmapType(lpbiIn)+1) ;
}

DWORD CodecInst::Compress(ICCOMPRESS* icinfo, DWORD dwSize)
{
	//int intype = GetBitmapType(icinfo->lpbiInput);
	//int outtype = GetBitmapType(icinfo->lpbiOutput);

	return CompressRGB(icinfo);
}

/********************************************************************
********************************************************************/

static bool CanDecompress(LPBITMAPINFOHEADER lpbiIn)
{
	int intype = GetBitmapType(lpbiIn);  
	return ((intype == -2) || (intype == -3) || (intype == -4));
}

static bool CanDecompress(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
	if (!lpbiOut)
		return CanDecompress(lpbiIn);

	// must be 1:1 (no stretching)
	if (lpbiOut && (lpbiOut->biWidth != lpbiIn->biWidth || lpbiOut->biHeight != lpbiIn->biHeight))
		return false;

	int intype = GetBitmapType(lpbiIn);
	int outtype = GetBitmapType(lpbiOut);

	if ((intype == -2) || (intype == -3) || (intype == -4))
	{  
		if (intype == -outtype)
			return true;
	}

	return false;
}

/********************************************************************
********************************************************************/

DWORD CodecInst::DecompressQuery(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
	//Msg("CSCD DecompressQuery: input = %s, output = %s\n", &PrintBitmapType(lpbiIn), &PrintBitmapType(lpbiOut));
	return CanDecompress(lpbiIn, lpbiOut) ? ICERR_OK : ICERR_BADFORMAT;
}

// This function should return "the output format which preserves the most
// information."  However, I now provide the option to return RGB format
// instead, since some programs treat the default format as the ONLY format.

DWORD CodecInst::DecompressGetFormat(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
	// if lpbiOut == NULL, then return the size required to hold an output format struct
	if (lpbiOut == NULL)
		return lpbiIn->biSize;

	if (!CanDecompress(lpbiIn))
		return ICERR_BADFORMAT;

	// get default rgboutput value and allow the user to override
	//bool rgboutput = AppFlags() & 1;
	//rgboutput = !!GetPrivateProfileInt("debug", "rgboutput", rgboutput, "huffyuv.ini");

	memcpy(lpbiOut, lpbiIn, lpbiIn->biSize);

	int intype = GetBitmapType(lpbiIn);
	if (intype == -4)
	{
		lpbiOut->biBitCount = 32;   // "RGBA" (though not really)
		lpbiOut->biCompression = 0;
		lpbiOut->biSizeImage = lpbiIn->biWidth * lpbiIn->biHeight * 4;
	} 
	else if (intype == -3)
	{	  
		//(rgboutput || intype == -2 || intype == 1 || intype == 2) {
		lpbiOut->biBitCount = 24;   // RGB
		lpbiOut->biCompression = 0;
		lpbiOut->biSizeImage = lpbiIn->biWidth * lpbiIn->biHeight * 3;
	} 
	else if (intype == -2)
	{	  
		//(rgboutput || intype == -2 || intype == 1 || intype == 2) {
		lpbiOut->biBitCount = 16;   // RGB
		lpbiOut->biCompression = 0;
		lpbiOut->biSizeImage = lpbiIn->biWidth * lpbiIn->biHeight * 2;
	} 

	return ICERR_OK;
}

DWORD CodecInst::Decompress(ICDECOMPRESS* icinfo, DWORD dwSize)
{
	//int intype = GetBitmapType(icinfo->lpbiInput);
	//int outtype = GetBitmapType(icinfo->lpbiOutput);

	return DecompressRGB(icinfo);
}

// palette-mapped output only
DWORD CodecInst::DecompressGetPalette(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
	return ICERR_BADFORMAT;
}

#define ORIG_RGBIGNORE 0
#define ORIG_RGB15 1
#define ORIG_RGB24 2
#define ORIG_RGB32 3

#define CONVERTCOLORYUY2_BIT 1
#define NON_KEYFRAME_BIT 0
#define KEYFRAME_BIT 1   //0 ==> DELTA FRAME, 1==>KEY FRAME 
#define ALGORITHM_GZIP_BIT 2  //000=>LZO , 001 = >GZIP, 010==>RESERVED, 011 ==>RESERVED, etc.
// byte 1 :0000  000  0
// byte 1 :level algo frametype

// byte 2 :0000          00           00
// byte 2 :reserved      origRGBbit   convertmodebit (colorspace)   

DWORD CodecInst::CompressRGB(ICCOMPRESS* icinfo)
{
	if (m_compressionHasBegun == 0)
		CompressBegin(icinfo->lpbiInput, icinfo->lpbiOutput);

	unsigned char convertmodebit = 0;
	unsigned char originalRGBbit = 0; 
	originalRGBbit = (GetBitmapType(icinfo->lpbiInput) - 1) << 2; //1, 2, 3

	//force auto keyframe
	unsigned char* input = NULL;
	lzo_uint in_len;
	lzo_uint out_len;

	int isKey = 0;   
	if (!m_autokeyframe)
	{  
		if (*icinfo->lpdwFlags == AVIIF_KEYFRAME)
			isKey = 1;
	}
	else
	{
		if ((m_currentFrame % m_autokeyframe_rate)==0)
			isKey = 1;

		//Msg("framenum %d isKey %d",m_currentFrame,isKey);
	}

	//v 1.01 hack to solve bitmaps of width not divisible by 4
	int remainder = icinfo->lpbiInput->biWidth % 4;  
	if (remainder > 0)
	{
		int newbit = icinfo->lpbiInput->biBitCount;

		// In the original version, remainder was subtracted from newwidth.
		// This is wrong - it causes a black line to appear over the video
		// if the width's not divisible by 4, so it's been removed.
		int newwidth = icinfo->lpbiInput->biWidth;
		int newheight = icinfo->lpbiInput->biHeight;	  
		int wLineLen = (newwidth * (newbit) + 31) / 32 * 4;
		int newsize = wLineLen * newheight;	  	  

		input = (unsigned char*)icinfo->lpInput;  	  

		// strange, adding this line will allow the image to be displayed!!
		in_len = newsize;
	}
	else // normal case
	{
		input = (unsigned char*)icinfo->lpInput;  
		in_len=icinfo->lpbiInput->biSizeImage;
	}
	////////////////// end v1.01

	DWORD* output = (DWORD*)icinfo->lpOutput;
	unsigned char* outputbyte = (unsigned char*)icinfo->lpOutput;

	int r;
	unsigned char byte1, byte2;

	if ((prevFrame==NULL) || (isKey))
	{
		if (prevFrame == NULL)
			prevFrame = (unsigned char *) malloc(in_len);

		memcpy(prevFrame, input, in_len);	  

		if (m_algorithm == 0)
			r = lzo1x_1_compress(input,in_len,(unsigned char *) outputbyte+2,&out_len,wrkmem);	  
		else if (m_algorithm == 1)
		{
			unsigned long destlen;		  
			r = compress2 ((unsigned char *) outputbyte+2, &destlen, input, in_len,  m_gzip_level); 
			out_len = destlen;
		}

		unsigned char keybit = KEYFRAME_BIT;
		unsigned char algo = m_algorithm;
		unsigned char gzip_level_bits = 0;

		algo = algo << 1;	  

		if (m_algorithm == 1)
		{		  

			gzip_level_bits = m_gzip_level;
			gzip_level_bits = gzip_level_bits << 4;

		}

		byte1 = keybit | algo | gzip_level_bits;
		byte2 = convertmodebit | originalRGBbit;

		*outputbyte = byte1;
		*(outputbyte + 1) = byte2;

		// Force key frames in case this is auto key framing mode
		*icinfo->lpdwFlags = AVIIF_KEYFRAME;

		icinfo->lpbiOutput->biSizeImage = out_len+2;
	}
	else
	{
		unsigned char* diffinputptr;
		unsigned char* prevFrameptr;
		unsigned char* inputptr;

		if (diffinput == NULL)
			diffinput = (unsigned char *) malloc(in_len);	  

		diffinputptr = diffinput;
		inputptr = input;
		prevFrameptr = prevFrame;

		for (unsigned long i = 0; i < in_len; i++)
		{
			*diffinputptr = *inputptr - *prevFrameptr;

			if (*diffinputptr)
			{
				*prevFrameptr = *inputptr;
			}

			prevFrameptr++;
			inputptr++;
			diffinputptr++;
		}	

		// Msg("nonKey ");

		if (m_algorithm == 0)
			r = lzo1x_1_compress((unsigned char *) diffinput,in_len,(unsigned char *) outputbyte+2,&out_len,wrkmem);	  
		else if (m_algorithm == 1)
		{		
			unsigned long destlen;	
			r = compress2 ((unsigned char *) outputbyte+2, &destlen, diffinput, in_len,  m_gzip_level); 
			out_len = destlen;
		}

		// Force non key frames in case this is auto key framing mode
		*icinfo->lpdwFlags &= ~AVIIF_KEYFRAME;

		unsigned char keybit = NON_KEYFRAME_BIT;
		unsigned char algo = m_algorithm;
		unsigned char gzip_level_bits = 0;
		algo = algo << 1;	  

		if (m_algorithm == 1)
		{
			gzip_level_bits = m_gzip_level;
			gzip_level_bits = gzip_level_bits << 4;
		}

		byte1 = keybit | algo | gzip_level_bits;
		byte2 = convertmodebit | originalRGBbit;

		*outputbyte = byte1;
		*(outputbyte + 1) = byte2;

		icinfo->lpbiOutput->biSizeImage = out_len+2;

		// if (diffinput) free(diffinput);
		// memcpy(prevFrame,icinfo->lpInput,in_len);
	}

	if (m_algorithm == 0)
	{
		if (r != LZO_E_OK)		
		{
			/* this should NEVER happen */
			printf("internal error - compression failed: %d\n", r);
			return 2;
		}
	}	
	else if (m_algorithm==1)
	{
		if (r != Z_OK)
		{
			printf("internal error - compression failed: %d\n", r);
			return 2;        
		}

	}

	/* check for an incompressible block */
	//if (out_len >= in_len)
	//{
	//	printf("This block contains incompressible data.\n");
	//	return 0;
	//}  	

	if (icinfo->lpckid)
		*icinfo->lpckid = FOURCC_CSCD;

	//*icinfo->lpdwFlags = AVIIF_KEYFRAME;

	m_currentFrame++;
	if (m_currentFrame >= m_autokeyframe_rate*100)
		m_currentFrame = 0;

	return ICERR_OK;
}

// hereRGB
DWORD CodecInst::DecompressRGB(ICDECOMPRESS* icinfo)
{
	//int remainder = icinfo->lpbiOutput->biWidth % 4;
	//icinfo->lpbiOutput->biWidth -= remainder;

	if (!m_decompressionHasBegun)
	{
		DecompressBegin(icinfo->lpbiInput, icinfo->lpbiOutput);
	}

	int width = icinfo->lpbiOutput->biWidth;
	int height = icinfo->lpbiOutput->biHeight;

	//int size = width * height * (icinfo->lpbiOutput->biBitCount >> 3);
	int wLineLen = (width*(icinfo->lpbiOutput->biBitCount)+31)/32 * 4;
	int size = wLineLen * height;

	icinfo->lpbiOutput->biSizeImage = size;

	DWORD* in = (DWORD*)icinfo->lpInput;
	unsigned char* inbyte = (unsigned char *) icinfo->lpInput;
	unsigned char* out = (unsigned char*)icinfo->lpOutput;
	//unsigned char* const end = out + size;

	lzo_uint in_len;
	lzo_uint out_len;

	in_len=icinfo->lpbiInput->biSizeImage;

	unsigned char  byte1, byte2;
	byte1=inbyte[0];
	byte2=inbyte[1];

	int r;
	int use_algo =0;  
	int use_gzip_level = 0;
	use_algo = (byte1 >> 1) & 7; // last three bits  
	use_gzip_level = (byte1 >> 4);

	int val = byte1 & KEYFRAME_BIT;

	// Msg("Decompress %d %d",use_algo,byte1);    

	if (byte1 & KEYFRAME_BIT)
	{	  
		if (use_algo == 0)
		{

			r = lzo1x_decompress((unsigned char *) inbyte + 2, in_len - 2, out, &out_len, NULL);	  		 	  

			//we don't use the lenght as condition becuase there is a hack in compressRGB
			//tat alters the length (bImageSize) of the image to force it to display 
			//if ((r != LZO_E_OK) || (out_len != (unsigned long) size))

			if (r != LZO_E_OK)
			{
				// this should NEVER happen 
				printf("internal error - decompression failed: %d\n", r);
				return 1;
			}
		}	  
		else if (use_algo == 1)
		{
			unsigned long destLen = size;
			r =  uncompress (out, &destLen, (unsigned char *)inbyte + 2, in_len - 2); 
			out_len = destLen;

			if (r != Z_OK)        
			{
				/* this should NEVER happen */			    
				printf("internal error - decompression failed: %d\n", r);
				return 1;
			}
		}
		//diffFrame = (unsigned char *) malloc(out_len);
		//memcpy(prevFrame,out,out_len);
	}
	else
	{
		if (diffFrame == NULL)
			diffFrame = (unsigned char *) malloc(size*3/2);

		if (use_algo == 0)
		{
			r = lzo1x_decompress((unsigned char *) inbyte+2,in_len-2,diffFrame,&out_len,NULL);	
			if (r != LZO_E_OK)
			{
				// this should NEVER happen 
				printf("internal error - decompression failed: %d\n", r);
				return 1;
			}

		}	  
		else if (use_algo == 1)
		{
			unsigned long destLen = size*3/2;
			r =  uncompress (diffFrame, &destLen, (unsigned char *) inbyte+2, in_len-2); 
			out_len = destLen;

			if (r != Z_OK)        
			{
				/* this should NEVER happen */
				printf("internal error - decompression failed: %d\n", r);
				return 1;
			}
		}	  

		unsigned char* diffFrameptr;
		unsigned char* outputptr;	  

		outputptr = out; //out is the current yuv
		diffFrameptr = diffFrame; //is the difference in yuvptr

		for (unsigned long i = 0; i < out_len; i++)
		{
			*outputptr = *diffFrameptr  + *outputptr;

			diffFrameptr++;
			outputptr++;
		}

		//no need for copy, we are using the current output itself as the prev frame
		//memcpy(Frame,out,out_len);	  	  
	}

	return ICERR_OK;
}

DWORD CodecInst::CompressEnd()
{
	FreeResources();
	m_compressionHasBegun = 0;

	return ICERR_OK;
}

DWORD CodecInst::DecompressEnd()
{
	//Msg("DecompressEnd()");

	FreeResources();
	m_decompressionHasBegun = 0;
	return ICERR_OK;
}

DWORD CodecInst::FreeResources()
{
	if (prevFrame != NULL)
	{
		free(prevFrame);
		prevFrame = NULL;
	}

	if (diffFrame != NULL)
	{
		free(diffFrame);
		diffFrame = NULL;
	}

	if (diffinput != NULL)
	{
		free(diffinput);
		diffinput = NULL;
	}

	return ICERR_OK;
}

DWORD CodecInst::CompressBegin(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut) 
{
	//  total_bpp = 0;
	//  num_frames = 0;

	//Msg("CSCD CompressBegin: input = %s, output = %s\n", &PrintBitmapType(lpbiIn), &PrintBitmapType(lpbiOut));

	m_algorithm = GetPrivateProfileInt("GENERAL", "algorithm", 0, "camcodec100.ini");  
	m_gzip_level = GetPrivateProfileInt("GENERAL", "gzip_level", 6, "camcodec100.ini"); 

	m_currentFrame = 0;
	CompressEnd();  // free resources if necessary

	//this line must be after CompressEnd();
	m_compressionHasBegun = 1;

	return CanCompress(lpbiIn, lpbiOut) ? ICERR_OK : ICERR_BADFORMAT;
}

DWORD CodecInst::DecompressBegin(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut)
{
	//Msg("CSCD DecompressBegin: input = %s, output = %s\n", &PrintBitmapType(lpbiIn), &PrintBitmapType(lpbiOut));

	DecompressEnd();  // free resources if necessary

	//need to initclip
	//InitClip();  

	m_decompressionHasBegun = 1;

	return CanDecompress(lpbiIn, lpbiOut) ? ICERR_OK : ICERR_BADFORMAT;
}
