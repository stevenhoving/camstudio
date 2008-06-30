//-----------------------------------------------------------------------------
// Picture (Prototypes) Version 1.00
//
// Routins 4 Showing Picture Files... (.BMP .DIB .EMF .GIF .ICO .JPG .WMF)
//
// Author: Dr. Yovav Gad, EMail: Sources@SuperMain.com ,Web: www.SuperMain.com
//=============================================================================

#if !defined(AFX_PICTURE_H__COPYFREE_BY_YOVAV_GAD__SOURCES_AT_SUPERMAIN_DOT_COM__INCLUDED_)
#define AFX_PICTURE_H__COPYFREE_BY_YOVAV_GAD__SOURCES_AT_SUPERMAIN_DOT_COM__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPicture
{
public:
	void FreePictureData();
	BOOL Load(CString sFilePathName);
	BOOL Load(UINT ResourceName, LPCSTR ResourceType);
	BOOL LoadPictureData(BYTE* pBuffer, int nSize);
	BOOL SaveAsBitmap(CString sFilePathName);
	BOOL Show(CDC* pDC, CPoint LeftTop, CPoint WidthHeight, int MagnifyX, int MagnifyY);
	BOOL Show(CDC* pDC, CRect DrawRect);
	BOOL ShowBitmapResource(CDC* pDC, const int BMPResource, CPoint LeftTop);
	BOOL UpdateSizeOnDC(CDC* pDC);
	BOOL CopyToPicture(CPicture* dstPic, CString exchangeFile);

	BOOL SaveToFile(FILE* fptr);
	BOOL LoadFromFile(FILE* fptr);

	CPicture();
	virtual ~CPicture();

	IPicture* m_IPicture; // Same As LPPICTURE (typedef IPicture __RPC_FAR *LPPICTURE)

	LONG      m_Height; // Height (In Pixels Ignor What Current Device Context Uses)
	LONG      m_Weight; // Size Of The Image Object In Bytes (File OR Resource)
	LONG      m_Width;  // Width (In Pixels Ignor What Current Device Context Uses)
};

#endif // !defined(AFX_PICTURE_H__COPYFREE_BY_YOVAV_GAD__SOURCES_AT_SUPERMAIN_DOT_COM__INCLUDED_)
