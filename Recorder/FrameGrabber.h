#pragma once

// FrameGrabber.h : header file
//

#if !defined(_INC_VFW)
#define NODRAWDIB
#define NOAVIFMT
#define NOMMREG
#define NOAVIFILE
#define NOMCIWND
#define NOMSACM
#include "vfw.h"
#endif

/////////////////////////////////////////////////////////////////////////////
// CFrameGrabber window

class CFrameGrabber : public CWnd
{

public:
    CFrameGrabber();

    // Attributes
public:
    LPBITMAPINFO GetDIB();
    LPBYTE GetImageBitsBuffer();

    CSize GetImageSize();
    DWORD GetImageBitsResolution();

    BOOL VideoFormatDialog();
    BOOL VideoSourceDialog();
    // Operations
public:
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CFrameGrabber)
public:
#if 0
    virtual BOOL Create(LPCTSTR lpszClassName,
        LPCTSTR lpszWindowName, DWORD dwStyle,
        const RECT& rect,
        CWnd* pParentWnd, UINT nID,
        CCreateContext* pContext = nullptr);
#endif

    virtual BOOL Create(int x, int y, CWnd *pParentWnd);
    //}}AFX_VIRTUAL

    // Implementation
public:
    ~CFrameGrabber() override;

    // must be a public method (for use with callbac)
    void SetImageData(LPVOID data);

    // Generated message map functions
protected:
    //{{AFX_MSG(CFrameGrabber)
    afx_msg void OnDestroy();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
    void update_buffer_size();
    LPVOID imageData;
    DWORD dwLastCallback;
    int vfs;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

