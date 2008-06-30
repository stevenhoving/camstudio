// CBITMAPEX.CPP
// CBitmap extention
//
// (c) Vadim Gorbatenko, 1997-99 
// gvv@mail.tomsknet.ru
// All rights reserved
//
//___________________________________________________________________________________________

#include "stdafx.h"
#include "CBitmapEx.h"

#define WIDTHBYTES(bits)        (((bits) + 31) / 32 * 4)
#define PALETTESIZE(lpbi)		(_DIBNumColors((LPBITMAPINFOHEADER) lpbi)* sizeof (RGBQUAD))
#define DIBCOLORS(lpbi)         ((LPRGBQUAD)((LPBYTE)(lpbi) + (int)(lpbi)->biSize))
#define DIBPTR(lpbi)            (LPBYTE)(DIBCOLORS((LPBITMAPINFOHEADER)lpbi) + (UINT)((LPBITMAPINFOHEADER)lpbi)->biClrUsed)

HANDLE	_dibFromBitmap(HBITMAP hBitmap);
BOOL	_writeDib(HANDLE hdib, LPCSTR filename);
INT		_DIBNumColors (LPBITMAPINFOHEADER  lpbi);


CBitmapEx::CBitmapEx():CBitmap()
{
	_modBMP= NULL;

}


CBitmapEx::~CBitmapEx()
{
	EndModify();//just in case:)
}

HANDLE	CBitmapEx::DibFromBitmap()
	{return _dibFromBitmap((HBITMAP)GetSafeHandle());}

HANDLE	CBitmapEx::DibFromBitmap(HBITMAP hb)	
	{return _dibFromBitmap(hb);}

BOOL	CBitmapEx::CreateFromDib(LPBITMAPINFO lpBi)
{
	if(!lpBi || _modBMP)	return FALSE;
	if(((LPBITMAPINFOHEADER)lpBi)->biCompression != BI_RGB)
			return FALSE;
	
	if(GetSafeHandle( ))
	{
		
		//check existing size
		BITMAP bmp;
		GetObject(sizeof BITMAP, &bmp);
		CSize sz = GetSize();

		if(bmp.bmWidth == ((LPBITMAPINFOHEADER)lpBi)->biWidth &&
			bmp.bmHeight == ((LPBITMAPINFOHEADER)lpBi)->biHeight)
		{
			// special case: we don't  need to destroy existing
			// DDB, just rewrite bits.
			// Note: we must be sure, the color resolution is
			// not changed, so, let's test it:
			HDC    hdc = ::GetDC(NULL);
			int    hdc_bits = GetDeviceCaps(hdc,BITSPIXEL);
			if(hdc_bits == bmp.bmBitsPixel)
			{
			//ok to set new bits
			BOOL ret = ::SetDIBits(
						hdc,					    // handle to device context
						(HBITMAP)GetSafeHandle( ), // handle to bitmap
						0,				   // starting scan line
						bmp.bmHeight,    // number of scan lines
					    DIBPTR(lpBi),   // array of bitmap bits
						lpBi,			// address of structure with bitmap data
						DIB_RGB_COLORS // type of color indexes to use
						) == bmp.bmHeight;
 
			::ReleaseDC(NULL,hdc);
			return ret;
			}
			else
				::ReleaseDC(NULL,hdc);
		}
		//delete existing bitmap:(
		DeleteObject();
	}
	//create a new DDB
    HDC         hdc;
    HBITMAP     hbm;

	//Create DDB
    hdc = ::GetDC(NULL);
    hbm = CreateDIBitmap(hdc, (LPBITMAPINFOHEADER)lpBi,(LONG)CBM_INIT,
                	DIBPTR(lpBi), lpBi, DIB_RGB_COLORS	);

    ::ReleaseDC(NULL,hdc);
	if(!hbm)	return FALSE;

	DeleteObject();//delete attached bitmap

	if(!Attach( (HGDIOBJ) hbm ))
		{::DeleteObject((HGDIOBJ)hbm); return FALSE;}

	return TRUE;

}

#define sfiltr	"Image files (*.BMP)|*.BMP||"
#define sext	"BMP"
#define smask	NULL

BOOL CBitmapEx::Open(LPCSTR filename, LPCSTR DialogTitle)
{

	if(GetSafeHandle( ))
		return FALSE;
	CString Path;

	if(!filename)
	{
	CFileDialog openAs( TRUE,sext,
						smask, 	OFN_NOCHANGEDIR|OFN_FILEMUSTEXIST|
						OFN_PATHMUSTEXIST,
						sfiltr); 
	
	//substitude dialog title
	if(DialogTitle)
		openAs.m_ofn.lpstrTitle=DialogTitle;
	
		if(openAs.DoModal()==IDOK)
		  
			 Path=openAs.GetPathName( );
		  	
		else return FALSE;
	}
	else Path=	filename;

	CFile file;
     
	if(!file.Open(Path,CFile::modeRead|CFile::typeBinary))
		return FALSE;

   BITMAPFILEHEADER   bmfHeader;
   LPBITMAPINFOHEADER lpbi;
   DWORD              dwBitsSize;
   //ULONGLONG dwBitsSize;
   HANDLE             hDIB;


   // get length of DIB in bytes for use when reading
	dwBitsSize = file.GetLength();

	BOOL ret=TRUE;
	TRY
	{
		
	  if( file.Read(&bmfHeader, sizeof (BITMAPFILEHEADER))!=
	  	sizeof (BITMAPFILEHEADER)||
	  	bmfHeader.bfType!=((WORD) ('M' << 8) | 'B')) ret=FALSE;

	}
	CATCH (CFileException, e)
	{
		ret=FALSE;		
		
	}
	END_CATCH

	if(!ret)	return FALSE;

   // Allocate memory for DIB
	dwBitsSize -= sizeof(BITMAPFILEHEADER);
   	
   	hDIB = GlobalAlloc (GMEM_MOVEABLE | GMEM_ZEROINIT, dwBitsSize);

	if(!hDIB)	return FALSE;
	
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDIB);
	TRY
	{
		//if(file.ReadHuge((LPVOID)lpbi, dwBitsSize)!=dwBitsSize)
		if(file.Read((LPVOID)lpbi, dwBitsSize)!=dwBitsSize)
			ret=FALSE;
	}
	CATCH (CFileException, e)
	{
		ret=FALSE;		
		
	}
	END_CATCH
      file.Close();
      

	if(!ret)
		{GlobalUnlock (hDIB); GlobalFree (hDIB); return FALSE;}

    

    HDC         hdc;
    HBITMAP     hbm;

	//Create DDB
    hdc = ::GetDC(NULL);
    hbm = CreateDIBitmap(hdc,(LPBITMAPINFOHEADER)lpbi,(LONG)CBM_INIT,
                	DIBPTR(lpbi),(LPBITMAPINFO)lpbi,DIB_RGB_COLORS	);

    ::ReleaseDC(NULL,hdc);

	GlobalUnlock (hDIB);
	GlobalFree (hDIB); 

	if(!hbm)
	       {return FALSE;}

	if(GetSafeHandle( ))
		DeleteObject();

	if(!Attach( (HGDIOBJ) hbm ))
		{::DeleteObject((HGDIOBJ)hbm); return FALSE;}
	

	return ret;
}

BOOL CBitmapEx::Save(LPCSTR filename, LPCSTR DialogTitle)
{
	
	if(!GetSafeHandle( ))
		return FALSE;
	CString Path;
	
	if(!filename)
	{

	CFileDialog saveAs( FALSE,sext,
				smask,	OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
				sfiltr); 
	
	if(DialogTitle)
		saveAs.m_ofn.lpstrTitle=DialogTitle;

	if(saveAs.DoModal()==IDOK)
		Path=saveAs.GetPathName( );
		  	
	else return FALSE;
	}
	else	Path=filename;

	CFile file;
    
	if(!file.Open((LPCSTR)Path,CFile::modeCreate|CFile::modeWrite|CFile::typeBinary))
		return FALSE;

	HANDLE hdib=_dibFromBitmap((HBITMAP)GetSafeHandle( ));

	if(!hdib)	return FALSE;

    BITMAPFILEHEADER	hdr;
    LPBITMAPINFOHEADER  lpbi;

    lpbi = (LPBITMAPINFOHEADER)GlobalLock (hdib);

    // Fill in the fields of the file header 
    hdr.bfType		= ((WORD) ('M' << 8) | 'B');  // "BM"
    hdr.bfSize		= GlobalSize (hdib) + sizeof (BITMAPFILEHEADER);
    hdr.bfReserved1     = 0;
    hdr.bfReserved2     = 0;
    hdr.bfOffBits       = (DWORD)sizeof(BITMAPFILEHEADER) + lpbi->biSize +
                          PALETTESIZE((LPSTR)lpbi);

	BOOL ret=TRUE;
	TRY
	{   
		file.Write((LPSTR)&hdr, sizeof (BITMAPFILEHEADER));
		//file.WriteHuge((LPSTR)lpbi, GlobalSize (hdib));
		file.Write((LPSTR)lpbi, GlobalSize (hdib));
	}
	CATCH (CFileException, e)
	{
		ret=FALSE;		
		
	}
	END_CATCH
   
   file.Close();
    GlobalUnlock (hdib);
     GlobalFree (hdib);
   
	 return ret;
}


// Get DC for "in-memory" drawing
CDC	*CBitmapEx::BegingModify()
{
	CWnd* dtw= CWnd::GetDesktopWindow( );
	CDC *dc=dtw->GetDC();
	
	if(_modDC.m_hDC)	
		_modDC.DeleteDC( );
	
	_modDC.CreateCompatibleDC(dc);
	
	_modBMP = _modDC.SelectObject(this);
	
	dtw->ReleaseDC(dc);
	return &_modDC;

}
// Create color bitmap
BOOL 	CBitmapEx::CreateColor(int dx, int dy)
{
	if(GetSafeHandle())	return FALSE;
	HDC hScrDC	= ::GetDC(NULL);
	HDC hMemDC	= ::CreateCompatibleDC(hScrDC);

	BOOL r = CreateCompatibleBitmap(CDC::FromHandle(hScrDC), dx, dy);
  	
	::DeleteDC(hMemDC);
	::ReleaseDC(NULL,hScrDC);
	return r;
}

// Create monocolor bitmap
BOOL 	CBitmapEx::CreateMono(int dx, int dy)
{
	if(GetSafeHandle())	return FALSE;
	CDC mDC;
	mDC.CreateCompatibleDC(NULL); //for mono!

	BOOL r = CreateCompatibleBitmap( &mDC, dx, dy );
	
	mDC.DeleteDC( );
	return r;
}

// This was the first extention i've done! :)))
CSize CBitmapEx::GetSize()
{
	BITMAP bmp;
	if(!GetSafeHandle())	return CSize(0,0);
	GetObject(sizeof BITMAP, &bmp);
	return CSize(bmp.bmWidth,bmp.bmHeight);
}

void 	CBitmapEx::BitBlt(CDC *dc_to, POINT at, DWORD rop)
{
	CSize sz=GetSize();

	dc_to->BitBlt(at.x,at.y, sz.cx, sz.cy, 
					BegingModify(),//source context
					0,0,  rop);
	EndModify();
}

void 	CBitmapEx::StretchBlt(CDC *dc_to, CRect to, DWORD rop)
{
	CSize sz=GetSize();
	dc_to->StretchBlt(to.left, to.top, 
			to.Width(), to.Height(),
			BegingModify(),//source context
			0,0,
			sz.cx, sz.cy,  
			rop);
	EndModify();
}

void	CBitmapEx::EndModify()
{
    if(_modDC.m_hDC && _modBMP)
		_modDC.SelectObject(_modBMP);
	if(_modDC.m_hDC)	
		_modDC.DeleteDC( );
	_modBMP = NULL;
}

//Copy the other bitmap to this 
BOOL CBitmapEx::CopyRect(CBitmap& bmp, CRect& rc)
{
	
	CWnd* dtw= CWnd::GetDesktopWindow( );
	CDC *dc=dtw->GetDC();
	

	CDC cdc;
	CDC cdc2;
	
	cdc2.CreateCompatibleDC(dc);
	cdc.CreateCompatibleDC(dc);
	
	bmp.CreateCompatibleBitmap( dc, rc.Width(), rc.Height());
	
	CBitmap *ob2=cdc2.SelectObject(&bmp);
	
	CBitmap *ob=cdc.SelectObject(this);

	cdc2.BitBlt( 0, 0, rc.Width(), rc.Height(), 
			&cdc, rc.left, rc.top, SRCCOPY );
	
	cdc2.SelectObject(ob2);
	
	cdc.SelectObject(ob);
	
	dtw->ReleaseDC(dc);
	return TRUE;
}

DWORD	CBitmapEx::DibImageSize(HANDLE hDIB)
{
	if(!hDIB)	return 0;
	LPBITMAPINFOHEADER lpbmInfoHdr=(LPBITMAPINFOHEADER) GlobalLock (hDIB);
	DWORD sz=sizeof (BITMAPINFOHEADER)+PALETTESIZE ((LPSTR) lpbmInfoHdr) + lpbmInfoHdr->biSizeImage;
	GlobalUnlock(hDIB);
	return sz;
}

/////////////////////////////////////////////////
// DDB->DIB
HANDLE _dibFromBitmap(HBITMAP hBitmap)
{
   HDC                  hDC;
   WORD			wBits;
   HPALETTE 		hPal=NULL;
   BITMAP               bm;
   BITMAPINFOHEADER     bi;
   BITMAPINFOHEADER     *lpbi;
   DWORD                dwLen;
   HANDLE               hDIB;

   if (!hBitmap)
      return NULL;

   ::GetObject (hBitmap, sizeof (bm), (LPSTR)&bm);

    wBits =  bm.bmBitsPixel;

   bi.biSize               = sizeof (BITMAPINFOHEADER);
   bi.biWidth              = bm.bmWidth;
   bi.biHeight             = bm.bmHeight;
   bi.biPlanes             = 1;
   bi.biBitCount           = wBits;
   bi.biCompression        = BI_RGB;
   bi.biSizeImage          = 0;
   bi.biXPelsPerMeter      = 0;
   bi.biYPelsPerMeter      = 0;
   bi.biClrUsed            = 0;
   bi.biClrImportant       = 0;

   dwLen  = bi.biSize + PALETTESIZE ((LPSTR) &bi);
   hDIB	  = GlobalAlloc(GMEM_DDESHARE|GMEM_MOVEABLE,dwLen);

   if (!hDIB)
      return NULL;

   lpbi   = (LPBITMAPINFOHEADER)GlobalLock(hDIB);
   *lpbi  = bi;
   
   HWND hWnd= GetFocus();
   
   if(!hWnd)
   {//load default system palette
   	hPal   = (HPALETTE)GetStockObject (DEFAULT_PALETTE);
   	hDC    = GetDC (NULL);
   	hPal   = SelectPalette (hDC, hPal, FALSE);
   	RealizePalette(hDC);
	}
   else
   	{
	//or get palette from focused window
	HDC hDCw    = GetDC (hWnd);
	hPal   = SelectPalette (hDCw, (HPALETTE)GetStockObject (DEFAULT_PALETTE), FALSE);
   	hDC    = GetDC (NULL);
   	hPal   = SelectPalette (hDC, hPal, FALSE);
	RealizePalette(hDC);
	SelectPalette (hDCw, hPal, FALSE);
	ReleaseDC(hWnd,hDCw);
	}

   // calculate the biSizeImage
   GetDIBits (hDC,
              hBitmap,
              0,
              (WORD) bi.biHeight,
              NULL,
              (LPBITMAPINFO) lpbi,
              DIB_RGB_COLORS);

   bi = *lpbi;
   GlobalUnlock(hDIB);

   // If the driver did not fill in the biSizeImage field, 
   // make one up

   if (bi.biSizeImage == 0)
      {
      bi.biSizeImage = WIDTHBYTES((DWORD)bm.bmWidth * wBits) * bm.bmHeight;

      if (bi.biCompression != BI_RGB)
         bi.biSizeImage = (bi.biSizeImage * 3) / 2;
      }


   DWORD sl =dwLen;
   HGLOBAL hPtr =hDIB;
   dwLen = bi.biSize + PALETTESIZE((LPSTR)&bi) + bi.biSizeImage;

	// if you have plans to use DDE or clipboard, you have
    // to allocate memory with GMEM_DDESHARE flag
   if(!(hDIB = GlobalAlloc(GMEM_DDESHARE|GMEM_MOVEABLE,dwLen))) 
   {
      SelectPalette(hDC,hPal,FALSE);
      ReleaseDC(NULL,hDC);
	  GlobalFree(hPtr);
	  return NULL;}

   lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDIB);
	LPBITMAPINFOHEADER lpS=(LPBITMAPINFOHEADER)GlobalLock(hPtr);
	
	CopyMemory(lpbi,lpS,sl);
	GlobalUnlock(hPtr);
	GlobalFree(hPtr);


    // actually fill lpBits
   if (GetDIBits( hDC,
         hBitmap,
         0,
         (WORD) bi.biHeight,
         (LPSTR) lpbi + (WORD) lpbi->biSize + PALETTESIZE((LPSTR) lpbi),
         (LPBITMAPINFO) lpbi, DIB_RGB_COLORS) == 0)
      {
      GlobalUnlock (hDIB);
      hDIB = NULL;
      SelectPalette (hDC, hPal, FALSE);
      ReleaseDC (NULL, hDC);
      return NULL;
      }

   bi = *lpbi;
   GlobalUnlock (hDIB);
   SelectPalette (hDC, hPal, FALSE);
   ReleaseDC (NULL, hDC);
   //OK
   return hDIB;
}



int _DIBNumColors (LPBITMAPINFOHEADER lpbi)
{
      if (lpbi->biClrUsed)
         return (int)lpbi->biClrUsed;
   switch (lpbi->biBitCount)
      {
      case 1:
         return 2;
      case 4:
         return 16;
      case 8:
         return 256;
      default:
         return 0;	//16,24,32 bits bitmap has no color table
      }
}
