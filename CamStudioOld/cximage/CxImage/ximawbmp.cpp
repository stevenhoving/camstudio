/*
 * File:	ximawbmp.cpp
 * Purpose:	Platform Independent WBMP Image Class Loader and Writer
 * 12/Jul/2002 Davide Pizzolato - www.xdp.it
 * CxImage version 5.99c 17/Oct/2004
 */

#include "ximawbmp.h"

#if CXIMAGE_SUPPORT_WBMP

#include "ximaiter.h"

////////////////////////////////////////////////////////////////////////////////
bool CxImageWBMP::Decode(CxFile *hFile)
{
	if (hFile == NULL) return false;

	WBMPHEADER wbmpHead;

  try
  {
	if (hFile->Read(&wbmpHead,sizeof(wbmpHead),1)==0)
		throw "Not a WBMP";

	if (wbmpHead.Type != 0)
		throw "Unsupported WBMP type";			

	if (wbmpHead.ImageHeight==0 || wbmpHead.ImageWidth==0)
		throw "Corrupted WBMP";

	Create(wbmpHead.ImageWidth, wbmpHead.ImageHeight, 1, CXIMAGE_FORMAT_WBMP);
	if (!IsValid()) throw "WBMP Create failed";
	SetGrayPalette();

	int linewidth=(wbmpHead.ImageWidth+7)/8;
    CImageIterator iter(this);
	iter.Upset();
    for (int y=0; y < wbmpHead.ImageHeight; y++){
		hFile->Read(iter.GetRow(),linewidth,1);
		iter.PrevRow();
    }

  } catch (char *message) {
	strncpy(info.szLastError,message,255);
	return FALSE;
  }
    return true;
}
////////////////////////////////////////////////////////////////////////////////
#if CXIMAGE_SUPPORT_ENCODE
////////////////////////////////////////////////////////////////////////////////
bool CxImageWBMP::Encode(CxFile * hFile)
{
	if (EncodeSafeCheck(hFile)) return false;

	//check format limits
	if ((head.biWidth>255)||(head.biHeight>255)||(head.biBitCount!=1)){
		strcpy(info.szLastError,"Can't save this image as WBMP");
		return false;
	}

	WBMPHEADER wbmpHead;
	wbmpHead.Type=0;
	wbmpHead.FixHeader=0;
	wbmpHead.ImageWidth=(BYTE)head.biWidth;
	wbmpHead.ImageHeight=(BYTE)head.biHeight;

    // Write the file header
	hFile->Write(&wbmpHead,sizeof(wbmpHead),1);
    // Write the pixels
	int linewidth=(wbmpHead.ImageWidth+7)/8;
    CImageIterator iter(this);
	iter.Upset();
    for (int y=0; y < wbmpHead.ImageHeight; y++){
		hFile->Write(iter.GetRow(),linewidth,1);
		iter.PrevRow();
    }
	return true;
}
////////////////////////////////////////////////////////////////////////////////
#endif // CXIMAGE_SUPPORT_ENCODE
////////////////////////////////////////////////////////////////////////////////
#endif // CXIMAGE_SUPPORT_WBMP

