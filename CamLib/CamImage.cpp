#include "stdafx.h"
#include "CamLib/CamImage.h"

HANDLE Bitmap2Dib(HBITMAP hbitmap, UINT bits)
{
    BITMAP bitmap;
    ::GetObject(hbitmap, sizeof(BITMAP), &bitmap);

    // DWORD align the width of the DIB
    // Figure out the size of the colour table
    // Calculate the size of the DIB
    UINT wLineLen = (bitmap.bmWidth * bits + 31) / 32 * 4;
    DWORD wColSize = sizeof(RGBQUAD) * ((bits <= 8) ? 1 << bits : 0);
    DWORD dwSize = sizeof(BITMAPINFOHEADER) + wColSize +
                   static_cast<DWORD>(wLineLen) * static_cast<DWORD>(static_cast<UINT>(bitmap.bmHeight));

    // Allocate room for a DIB and set the LPBI fields
    HANDLE hdib = ::GlobalAlloc(GHND, dwSize);
    if (!hdib)
    {
        return hdib;
    }

    auto lpbi = static_cast<LPBITMAPINFOHEADER>(GlobalLock(hdib));
    lpbi->biSize = sizeof(BITMAPINFOHEADER);
    lpbi->biWidth = bitmap.bmWidth;
    lpbi->biHeight = bitmap.bmHeight;
    lpbi->biPlanes = 1;
    lpbi->biBitCount = static_cast<WORD>(bits);
    lpbi->biCompression = BI_RGB;
    lpbi->biSizeImage = dwSize - sizeof(BITMAPINFOHEADER) - wColSize;
    lpbi->biXPelsPerMeter = 0;
    lpbi->biYPelsPerMeter = 0;
    lpbi->biClrUsed = (bits <= 8) ? 1 << bits : 0;
    lpbi->biClrImportant = 0;

    // Get the bits from the bitmap and stuff them after the LPBI
    LPBYTE lpBits = reinterpret_cast<LPBYTE>(lpbi + 1) + wColSize;

    HDC hdc = ::CreateCompatibleDC(nullptr);

    ::GetDIBits(hdc, hbitmap, 0, bitmap.bmHeight, lpBits, reinterpret_cast<LPBITMAPINFO>(lpbi), DIB_RGB_COLORS);
    lpbi->biClrUsed = (bits <= 8) ? (1 << bits) : 0;

    ::DeleteDC(hdc);
    ::GlobalUnlock(hdib);

    return hdib;
}

void SaveBitmapCopy(HBITMAP &hBitmap, HDC hdc, HDC hdcbits, int x, int y, int sx, int sy)
{
    if (hBitmap)
    {
        ::DeleteObject(hBitmap);
    }
    hBitmap = ::CreateCompatibleBitmap(hdc, sx, sy);
    auto oldbitmap = static_cast<HBITMAP>(::SelectObject(hdcbits, hBitmap));
    ::BitBlt(hdcbits, 0, 0, sx, sy, hdc, x, y, SRCCOPY);

    ::SelectObject(hdcbits, oldbitmap);
}

void SaveBitmapCopy(HBITMAP &hBitmap, HDC hdc, HDC hdcbits, const RECT &rect)
{
    SaveBitmapCopy(hBitmap, hdc, hdcbits, rect.left, rect.top, (rect.right - rect.left), (rect.bottom - rect.top));
}

void RestoreBitmapCopy(HBITMAP &hBitmap, HDC hdc, HDC hdcbits, int x, int y, int sx, int sy)
{
    if (!hBitmap)
    {
        return;
    }
    auto oldbitmap = static_cast<HBITMAP>(::SelectObject(hdcbits, hBitmap));
    ::BitBlt(hdc, x, y, sx, sy, hdcbits, 0, 0, SRCCOPY);
    ::SelectObject(hdcbits, oldbitmap);
    ::DeleteObject(hBitmap);
    hBitmap = nullptr;
}

void RestoreBitmapCopy(HBITMAP &hBitmap, HDC hdc, HDC hdcbits, const RECT &rect)
{
    RestoreBitmapCopy(hBitmap, hdc, hdcbits, rect.left, rect.top, (rect.right - rect.left), (rect.bottom - rect.top));
}
