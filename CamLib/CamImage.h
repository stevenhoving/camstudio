// CamImage.h	- include file for CamStudio Library image api's
/////////////////////////////////////////////////////////////////////////////
#ifndef CAMIMAGE_H
#define CAMIMAGE_H

#pragma once

HANDLE Bitmap2Dib(HBITMAP hbitmap, UINT bits);
void SaveBitmapCopy(HBITMAP& hBitmap, HDC hdc, HDC hdcbits, const RECT& rect);
void SaveBitmapCopy(HBITMAP& hBitmap, HDC hdc, HDC hdcbits, int x, int y, int sx, int sy);
void RestoreBitmapCopy(HBITMAP& hBitmap, HDC hdc, HDC hdcbits, const RECT& rect);
void RestoreBitmapCopy(HBITMAP& hBitmap, HDC hdc, HDC hdcbits, int x, int y, int sx, int sy);

#endif	// CAMIMAGE_H
