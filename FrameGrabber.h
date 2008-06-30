#if !defined(AFX_FRAMEGRABBER_H__8145B633_9700_11D3_B4B8_E9E5ECC8F23B__INCLUDED_)
#define AFX_FRAMEGRABBER_H__8145B633_9700_11D3_B4B8_E9E5ECC8F23B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FrameGrabber.h : header file
//

#if !defined(_INC_VFW)
    #define	NODRAWDIB
    #define	NOAVIFMT
    #define	NOMMREG
    #define	NOAVIFILE
    #define	NOMCIWND
    #define	NOMSACM
    #include "vfw.h"
#endif

/////////////////////////////////////////////////////////////////////////////
// CFrameGrabber window

class CFrameGrabber : public CWnd
{
// Construction
public:
	CFrameGrabber();

// Attributes
public:
	
	LPBITMAPINFO	GetDIB();
	LPBYTE			GetImageBitsBuffer();

	CSize		GetImageSize();
	DWORD		GetImageBitsResolution();
	
	BOOL		VideoFormatDialog();
	BOOL		VideoSourceDialog();
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFrameGrabber)
	public:
	virtual BOOL Create(int at_x, int at_y, CWnd *pParentWnd);
	//}}AFX_VIRTUAL

// Implementation
public:
	//must be a public method (for use with callbac)
	void	SetImageData(LPVOID data);
	virtual ~CFrameGrabber();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFrameGrabber)
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void	update_buffer_size();
	LPVOID	imageData;
	DWORD	dwLastCallback;
	int		vfs;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FRAMEGRABBER_H__8145B633_9700_11D3_B4B8_E9E5ECC8F23B__INCLUDED_)
