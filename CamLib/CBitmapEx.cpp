// CBITMAPEX.CPP
// CBitmap extention
//
// (c) Vadim Gorbatenko, 1997-99
// gvv@mail.tomsknet.ru
// All rights reserved

#include "stdafx.h"
#include "CamLib/CBitmapEx.h"
#include <afxdlgs.h>

#define WIDTHBYTES(iBits) (((iBits) + 31) / 32 * 4)
#define PALETTESIZE(lpbi) (_DIBNumColors((LPBITMAPINFOHEADER)lpbi) * sizeof(RGBQUAD))
#define DIBCOLORS(lpbi) ((LPRGBQUAD)((LPBYTE)(lpbi) + (int)(lpbi)->biSize))
#define DIBPTR(lpbi) (LPBYTE)(DIBCOLORS((LPBITMAPINFOHEADER)lpbi) + (UINT)((LPBITMAPINFOHEADER)lpbi)->biClrUsed)

HANDLE _dibFromBitmap(HBITMAP hBitmap);
INT _DIBNumColors(LPBITMAPINFOHEADER lpbi);

CBitmapEx::~CBitmapEx()
{
    EndModify(); // just in case:)
}

HANDLE CBitmapEx::DibFromBitmap()
{
    return _dibFromBitmap(static_cast<HBITMAP>(GetSafeHandle()));
}

HANDLE CBitmapEx::DibFromBitmap(HBITMAP hb)
{
    return _dibFromBitmap(hb);
}

BOOL CBitmapEx::CreateFromDib(LPBITMAPINFO lpBi)
{
    if (!lpBi || _modBMP)
    {
        return false;
    }
    if ((reinterpret_cast<LPBITMAPINFOHEADER>(lpBi))->biCompression != BI_RGB)
    {
        return false;
    }

    if (GetSafeHandle())
    {

        // check existing size
        BITMAP bmp;
        GetObject(sizeof(BITMAP), &bmp);
        CSize sz = GetSize();

        if (bmp.bmWidth == (reinterpret_cast<LPBITMAPINFOHEADER>(lpBi))->biWidth &&
            bmp.bmHeight == (reinterpret_cast<LPBITMAPINFOHEADER>(lpBi))->biHeight)
        {
            // special case: we don't need to destroy existing
            // DDB, just rewrite iBits.
            // Note: we must be sure, the color resolution is
            // not changed, so, let's test it:
            HDC hdc = ::GetDC(nullptr);
            int hdc_bits = GetDeviceCaps(hdc, BITSPIXEL);
            if (hdc_bits == bmp.bmBitsPixel)
            {
                // ok to set new iBits
                BOOL ret = ::SetDIBits(hdc,                                   // handle to device context
                                       static_cast<HBITMAP>(GetSafeHandle()), // handle to bitmap
                                       0,                                     // starting scan line
                                       bmp.bmHeight,                          // number of scan lines
                                       DIBPTR(lpBi),                          // array of bitmap iBits
                                       lpBi,                                  // address of structure with bitmap data
                                       DIB_RGB_COLORS                         // type of color indexes to use
                                       ) == bmp.bmHeight;

                ::ReleaseDC(nullptr, hdc);
                return ret;
            }
            {
                ::ReleaseDC(nullptr, hdc);
            }
        }
        // delete existing bitmap:(
        DeleteObject();
    }
    // create a new DDB
    HDC hdc;
    HBITMAP hbm;

    // Create DDB
    hdc = ::GetDC(nullptr);
    hbm = CreateDIBitmap(hdc, reinterpret_cast<LPBITMAPINFOHEADER>(lpBi), static_cast<LONG>(CBM_INIT), DIBPTR(lpBi),
                         lpBi, DIB_RGB_COLORS);

    ::ReleaseDC(nullptr, hdc);
    if (!hbm)
    {
        return false;
    }

    DeleteObject(); // delete attached bitmap

    if (!Attach((HGDIOBJ)hbm))
    {
        ::DeleteObject((HGDIOBJ)hbm);
        return false;
    }

    return TRUE;
}

#define sfiltr _T("Image files (*.BMP)|*.BMP||")
#define sext _T("BMP")
#define smask nullptr

BOOL CBitmapEx::Open(const TCHAR * filename, const TCHAR *DialogTitle)
{
    if (GetSafeHandle())
    {
        return false;
    }

    CString Path(filename);
    if (!filename)
    {
        CFileDialog openAs(TRUE, sext, smask, OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, sfiltr);

        // substitude dialog title
        if (DialogTitle)
        {
            openAs.m_ofn.lpstrTitle = DialogTitle;
        }

        if (IDOK != openAs.DoModal())
        {
            return false;
        }
        Path = openAs.GetPathName();
    }

    CFile file;
    if (!file.Open(Path, CFile::modeRead | CFile::typeBinary))
    {
        return false;
    }

    // get length of DIB in bytes for use when reading
    // DWORD dwBitsSize = file.GetLength();      -> Cause C4244 warning

    // Prevent dwBitsSize C4244 warning and take some predictions for unwanted truncations
    ULONGLONG tmpUlongSize = file.GetLength();
    if (tmpUlongSize != static_cast<WORD>(tmpUlongSize))
    {
        TRACE("Error: dwBitsSize, Casted value [%i] is not the same as original [%l] ..!\n",
              static_cast<WORD>(tmpUlongSize), tmpUlongSize);
    }
    // Now assigned tested casted value to dest value (Functional nothing is changed but we have a marker in the
    // tracelog if truncated)
    DWORD dwBitsSize = static_cast<WORD>(tmpUlongSize);

    BITMAPFILEHEADER bmfHeader;
    BOOL ret = TRUE;
    TRY
    {
        if (file.Read(&bmfHeader, sizeof(BITMAPFILEHEADER)) != sizeof(BITMAPFILEHEADER) ||
            bmfHeader.bfType != (static_cast<WORD>('M' << 8) | 'B'))
        {
            ret = FALSE;
        }
    }
    CATCH(CFileException, e)
    {
        ret = FALSE;
    }
    END_CATCH

    if (!ret)
    {
        return false;
    }

    // Allocate memory for DIB
    dwBitsSize -= sizeof(BITMAPFILEHEADER);

    HANDLE hDIB = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, dwBitsSize);
    if (!hDIB)
    {
        return false;
    }

    auto lpbi = static_cast<LPBITMAPINFOHEADER>(GlobalLock(hDIB));
    TRY
    {
        // if (file.ReadHuge((LPVOID)lpbi, dwBitsSize)!=dwBitsSize)
        if (file.Read((LPVOID)lpbi, dwBitsSize) != dwBitsSize)
        {
            ret = FALSE;
        }
    }
    CATCH(CFileException, e)
    {
        ret = FALSE;
    }
    END_CATCH

    file.Close();

    if (!ret)
    {
        GlobalUnlock(hDIB);
        GlobalFree(hDIB);
        return false;
    }

    // Create DDB
    HDC hdc = ::GetDC(nullptr);
    HBITMAP hbm = CreateDIBitmap(hdc, lpbi, static_cast<LONG>(CBM_INIT), DIBPTR(lpbi),
                                 reinterpret_cast<LPBITMAPINFO>(lpbi), DIB_RGB_COLORS);
    ::ReleaseDC(nullptr, hdc);

    GlobalUnlock(hDIB);
    GlobalFree(hDIB);

    if (!hbm)
    {
        return false;
    }

    if (GetSafeHandle())
    {
        DeleteObject();
    }

    if (!Attach((HGDIOBJ)hbm))
    {
        ::DeleteObject((HGDIOBJ)hbm);
        return false;
    }

    return ret;
}

BOOL CBitmapEx::Save(const TCHAR *filename, const TCHAR *DialogTitle)
{
    if (!GetSafeHandle())
    {
        return false;
    }

    CString Path(filename);
    if (!filename)
    {
        CFileDialog saveAs(FALSE, sext, smask, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, sfiltr);
        if (DialogTitle)
        {
            saveAs.m_ofn.lpstrTitle = DialogTitle;
        }

        if (IDOK != saveAs.DoModal())
        {
            return false;
        }

        Path = saveAs.GetPathName();
    }

    CFile file;
    if (!file.Open(Path, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
    {
        return false;
    }

    HANDLE hdib = _dibFromBitmap(static_cast<HBITMAP>(GetSafeHandle()));
    if (!hdib)
    {
        return false;
    }

    auto lpbi = static_cast<LPBITMAPINFOHEADER>(::GlobalLock(hdib));

    // Fill in the fields of the file header
    BITMAPFILEHEADER hdr;
    hdr.bfType = (static_cast<WORD>('M' << 8) | 'B'); // "BM"
    hdr.bfSize = GlobalSize(hdib) + sizeof(BITMAPFILEHEADER);
    hdr.bfReserved1 = 0;
    hdr.bfReserved2 = 0;
    hdr.bfOffBits = static_cast<DWORD>(sizeof(BITMAPFILEHEADER)) + lpbi->biSize + PALETTESIZE((LPSTR)lpbi);

    BOOL ret = TRUE;
    TRY
    {
        file.Write(reinterpret_cast<LPSTR>(&hdr), sizeof(BITMAPFILEHEADER));
        file.Write(reinterpret_cast<LPSTR>(lpbi), GlobalSize(hdib));
    }
    CATCH(CFileException, e)
    {
        ret = FALSE;
    }
    END_CATCH

    file.Close();
    GlobalUnlock(hdib);
    GlobalFree(hdib);

    return ret;
}

// Get DC for "in-memory" drawing
CDC *CBitmapEx::BegingModify()
{
    CWnd *dtw = CWnd::GetDesktopWindow();
    CDC *dc = dtw->GetDC();

    if (_modDC.m_hDC)
    {
        _modDC.DeleteDC();
    }

    _modDC.CreateCompatibleDC(dc);

    _modBMP = _modDC.SelectObject(this);

    dtw->ReleaseDC(dc);
    return &_modDC;
}

void CBitmapEx::EndModify()
{
    if (_modDC.m_hDC && _modBMP)
    {
        _modDC.SelectObject(_modBMP);
    }
    if (_modDC.m_hDC)
    {
        _modDC.DeleteDC();
    }
    _modBMP = nullptr;
}

// Create color bitmap
BOOL CBitmapEx::CreateColor(int dx, int dy)
{
    if (GetSafeHandle())
    {
        return false;
    }
    HDC hScrDC = ::GetDC(nullptr);
    HDC hMemDC = ::CreateCompatibleDC(hScrDC);

    BOOL r = CreateCompatibleBitmap(CDC::FromHandle(hScrDC), dx, dy);

    ::DeleteDC(hMemDC);
    ::ReleaseDC(nullptr, hScrDC);
    return r;
}

// Create monocolor bitmap
BOOL CBitmapEx::CreateMono(int width, int height)
{
    if (GetSafeHandle())
    {
        return false;
    }
    CDC mDC;
    mDC.CreateCompatibleDC(nullptr); // for mono!

    BOOL r = CreateCompatibleBitmap(&mDC, width, height);

    mDC.DeleteDC();
    return r;
}

CSize CBitmapEx::GetSize()
{
    BITMAP bmp;
    if (!GetSafeHandle())
    {
        return {0, 0};
    }
    GetObject(sizeof(BITMAP), &bmp);
    return CSize(bmp.bmWidth, bmp.bmHeight);
}

void CBitmapEx::BitBlt(CDC *dc_to, POINT at, DWORD rop)
{
    CSize sz = GetSize();

    dc_to->BitBlt(at.x, at.y, sz.cx, sz.cy,
                  BegingModify(), // source context
                  0, 0, rop);
    EndModify();
}

void CBitmapEx::StretchBlt(CDC *dc_to, CRect to, DWORD rop)
{
    CSize sz = GetSize();
    dc_to->StretchBlt(to.left, to.top, to.Width(), to.Height(), BegingModify(), 0, 0, sz.cx, sz.cy, rop);
    EndModify();
}

// Copy the other bitmap to this
BOOL CBitmapEx::CopyRect(CBitmap &bmp, CRect &rc)
{
    CWnd *dtw = CWnd::GetDesktopWindow();
    CDC *dc = dtw->GetDC();

    CDC cdc;
    CDC cdc2;
    cdc2.CreateCompatibleDC(dc);
    cdc.CreateCompatibleDC(dc);

    bmp.CreateCompatibleBitmap(dc, rc.Width(), rc.Height());

    CBitmap *ob2 = cdc2.SelectObject(&bmp);
    CBitmap *ob = cdc.SelectObject(this);

    cdc2.BitBlt(0, 0, rc.Width(), rc.Height(), &cdc, rc.left, rc.top, SRCCOPY);
    cdc2.SelectObject(ob2);
    cdc.SelectObject(ob);

    dtw->ReleaseDC(dc);
    return TRUE;
}

DWORD CBitmapEx::DibImageSize(HANDLE hDIB)
{
    if (!hDIB)
    {
        return 0;
    }
    auto lpbmInfoHdr = static_cast<LPBITMAPINFOHEADER>(GlobalLock(hDIB));
    DWORD sz = sizeof(BITMAPINFOHEADER) + PALETTESIZE((LPSTR)lpbmInfoHdr) + lpbmInfoHdr->biSizeImage;
    GlobalUnlock(hDIB);
    return sz;
}

/////////////////////////////////////////////////
// DDB->DIB
HANDLE _dibFromBitmap(HBITMAP hBitmap)
{
    if (!hBitmap)
    {
        return nullptr;
    }

    BITMAP bm;
    ::GetObject(hBitmap, sizeof(bm), reinterpret_cast<LPSTR>(&bm));

    WORD wBits = bm.bmBitsPixel;

    BITMAPINFOHEADER bi;
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bm.bmWidth;
    bi.biHeight = bm.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = wBits;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    DWORD dwLen = bi.biSize + PALETTESIZE((LPSTR)&bi);
    HANDLE hDIB = ::GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE, dwLen);
    if (!hDIB)
    {
        return nullptr;
    }

    auto *lpbi = static_cast<LPBITMAPINFOHEADER>(::GlobalLock(hDIB));
    *lpbi = bi;

    HDC hDC = nullptr;
    HPALETTE hPal = nullptr;
    HWND hWnd = GetFocus();
    if (!hWnd)
    {
        // load default system palette
        hPal = static_cast<HPALETTE>(GetStockObject(DEFAULT_PALETTE));
        hDC = GetDC(nullptr);
        hPal = SelectPalette(hDC, hPal, FALSE);
        RealizePalette(hDC);
    }
    else
    {
        // or get palette from focused window
        HDC hDCw = GetDC(hWnd);
        hPal = SelectPalette(hDCw, static_cast<HPALETTE>(GetStockObject(DEFAULT_PALETTE)), FALSE);
        hDC = GetDC(nullptr);
        hPal = SelectPalette(hDC, hPal, FALSE);
        RealizePalette(hDC);
        SelectPalette(hDCw, hPal, FALSE);
        ReleaseDC(hWnd, hDCw);
    }

    // calculate the biSizeImage
    GetDIBits(hDC, hBitmap, 0, static_cast<WORD>(bi.biHeight), nullptr, reinterpret_cast<LPBITMAPINFO>(lpbi),
              DIB_RGB_COLORS);
    bi = *lpbi;
    GlobalUnlock(hDIB);

    // If the driver did not fill in the biSizeImage field,
    // make one up

    if (bi.biSizeImage == 0)
    {
        bi.biSizeImage = WIDTHBYTES((DWORD)bm.bmWidth * wBits) * bm.bmHeight;

        if (bi.biCompression != BI_RGB)
        {
            bi.biSizeImage = (bi.biSizeImage * 3) / 2;
        }
    }

    DWORD sl = dwLen;
    HGLOBAL hPtr = hDIB;
    dwLen = bi.biSize + PALETTESIZE((LPSTR)&bi) + bi.biSizeImage;

    // if you have plans to use DDE or clipboard, you have
    // to allocate memory with GMEM_DDESHARE flag
    hDIB = GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE, dwLen);
    if (!hDIB)
    {
        SelectPalette(hDC, hPal, FALSE);
        ReleaseDC(nullptr, hDC);
        GlobalFree(hPtr);
        return nullptr;
    }

    lpbi = static_cast<LPBITMAPINFOHEADER>(GlobalLock(hDIB));
    auto lpS = static_cast<LPBITMAPINFOHEADER>(GlobalLock(hPtr));

    CopyMemory(lpbi, lpS, sl);
    GlobalUnlock(hPtr);
    GlobalFree(hPtr);

    // actually fill lpBits
    if (GetDIBits(hDC, hBitmap, 0, static_cast<WORD>(bi.biHeight),
                  reinterpret_cast<LPSTR>(lpbi) + static_cast<WORD>(lpbi->biSize) + PALETTESIZE((LPSTR)lpbi),
                  reinterpret_cast<LPBITMAPINFO>(lpbi), DIB_RGB_COLORS) == 0)
    {
        GlobalUnlock(hDIB);
        GlobalFree(hDIB);
        hDIB = nullptr;
        SelectPalette(hDC, hPal, FALSE);
        ReleaseDC(nullptr, hDC);
        return nullptr;
    }

    bi = *lpbi;
    GlobalUnlock(hDIB);
    SelectPalette(hDC, hPal, FALSE);
    ReleaseDC(nullptr, hDC);
    // OK
    return hDIB;
}

int _DIBNumColors(LPBITMAPINFOHEADER lpbi)
{
    if (lpbi->biClrUsed)
    {
        return static_cast<int>(lpbi->biClrUsed);
    }

    switch (lpbi->biBitCount)
    {
        case 1:
            return 2;
        case 4:
            return 16;
        case 8:
            return 256;
        default:
            return 0; // 16,24,32 iBits bitmap has no color table
    }
}
