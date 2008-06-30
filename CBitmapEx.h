// CBITMAPEX.H
// CBitmap extention
//
//
//___________________________________________________________________________________________

#ifndef _CBITMAPEX_INCL
#define	_CBITMAPEX_INCL

class CBitmapEx : public CBitmap
{
public:
		CBitmapEx();	
		~CBitmapEx();
		//load&save DDB
		BOOL	Open(LPCSTR filename, LPCSTR DialogTitle=NULL);
		BOOL	Save(LPCSTR filename, LPCSTR DialogTitle=NULL);

		void	BitBlt(CDC *dc_to, POINT at, DWORD rop=SRCCOPY);
		void	StretchBlt(CDC *dc_to, CRect to, DWORD rop=SRCCOPY);
		CSize 	GetSize();
		
		BOOL 	CreateMono(int dx, int dy);
		BOOL 	CreateColor(int dx, int dy);
		BOOL	CreateFromDib(LPBITMAPINFO lpBi);


		BOOL	CopyRect(CBitmap& bmp, CRect& rc);
		HANDLE	DibFromBitmap();

		CDC		*BegingModify();
		void	EndModify();
	
static	HANDLE	DibFromBitmap(HBITMAP);
static	DWORD	DibImageSize(HANDLE hDIB);

protected:
		CDC		_modDC;
		CBitmap *_modBMP;
};

#endif //_RBITMAP_INCL
