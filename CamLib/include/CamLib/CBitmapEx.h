#pragma once

class CBitmapEx : public CBitmap
{
public:
    CBitmapEx() = default;
    ~CBitmapEx() override;

    CBitmapEx(const CBitmapEx&) = delete;
    CBitmapEx& operator=(const CBitmapEx&) = delete;

    // load&save DDB
    BOOL Open(const TCHAR *filename, const TCHAR *DialogTitle = nullptr);
    BOOL Save(const TCHAR *filename, const TCHAR *DialogTitle = nullptr);

    void BitBlt(CDC *dc_to, POINT at, DWORD rop = SRCCOPY);
    void StretchBlt(CDC *dc_to, CRect to, DWORD rop = SRCCOPY);
    CSize GetSize();

    BOOL CreateMono(int dx, int dy);
    BOOL CreateColor(int dx, int dy);
    BOOL CreateFromDib(LPBITMAPINFO lpBi);

    BOOL CopyRect(CBitmap &bmp, CRect &rc);
    HANDLE DibFromBitmap();

    CDC *BegingModify();
    void EndModify();

    static HANDLE DibFromBitmap(HBITMAP /*hb*/);
    static DWORD DibImageSize(HANDLE hDIB);

protected:
    CDC _modDC;
    CBitmap *_modBMP{nullptr};
};
