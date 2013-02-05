//
// Huffyuv v1.3.1, by Ben Rudiak-Gould.
// http://www.math.berkeley.edu/~benrg/huffyuv.html
//
// This file is copyright 2000 Ben Rudiak-Gould, and distributed under
// the terms of the GNU General Public License, v2 or later.  See
// http://www.gnu.org/copyleft/gpl.html.
//
// I edit these files in 10-point Verdana, a proportionally-spaced font.
// You may notice formatting oddities if you use a monospaced font.
//

#include <windows.h>
#include <vfw.h>
#pragma hdrstop

static const DWORD FOURCC_CSCD = mmioFOURCC('C','S','C','D');   // our compressed format
static const DWORD FOURCC_YUY2 = mmioFOURCC('Y','U','Y','2');   // uncompressed YUY2
static const DWORD FOURCC_UYVY = mmioFOURCC('U','Y','V','Y');   // uncompressed UYVY
static const DWORD FOURCC_VYUY = mmioFOURCC('V','Y','U','Y');   // an alias for YUY2 used by ATI cards

extern HMODULE hmoduleCamcodec;

struct CodecInst {
  

  // methods

  unsigned char* prevFrame;
  unsigned char* diffFrame;
  unsigned char* diffinput; 
  
  int m_algorithm;
  int m_gzip_level;
  
  int m_autokeyframe;
  int m_autokeyframe_rate;
  int m_currentFrame;

  int m_compressionHasBegun;
  int m_decompressionHasBegun;
  
  CodecInst();  

  BOOL QueryAbout();
  DWORD About(HWND hwnd);

  BOOL QueryConfigure();
  DWORD Configure(HWND hwnd);

  DWORD GetState(LPVOID pv, DWORD dwSize);
  DWORD SetState(LPVOID pv, DWORD dwSize);

  DWORD GetInfo(ICINFO* icinfo, DWORD dwSize);

  DWORD CompressQuery(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
  DWORD CompressGetFormat(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
  DWORD CompressBegin(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
  DWORD CompressGetSize(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
  DWORD Compress(ICCOMPRESS* icinfo, DWORD dwSize);
  DWORD CompressEnd();

  DWORD CompressRGB(ICCOMPRESS* icinfo);
    
  DWORD	DecompressRGB(ICDECOMPRESS* icinfo);
  DWORD DecompressQuery(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
  DWORD DecompressGetFormat(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
  DWORD DecompressBegin(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
  DWORD Decompress(ICDECOMPRESS* icinfo, DWORD dwSize);
  DWORD DecompressGetPalette(LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut);
  DWORD DecompressEnd();  

  DWORD FreeResources();
  

};

CodecInst* Open(ICOPEN* icinfo);
DWORD Close(CodecInst* pinst);

