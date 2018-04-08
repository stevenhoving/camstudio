#pragma once

#include "TransparentWnd.h"
#include "FrameGrabber.h"
#include <CamLib/CBitmapEx.h>

/////////////////////////////////////////////////////////////////////////////
// CVideoWnd window

class CVideoWnd : public CTransparentWnd
{

public:
    CVideoWnd();
    virtual ~CVideoWnd();

    // Attributes
public:
    // Operations
public:
    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CVideoWnd)
    //}}AFX_VIRTUAL

    // Implementation
private:
public:
    UINT m_ProcessorMode;
    CFrameGrabber m_FrameGrabber;
    CBitmapEx m_ImageBitmap;
    int m_iRefreshRate;
    int m_iStatus;

    virtual void CreateTransparent(LPCTSTR pTitle, RECT rect, HBITMAP BitmapID);
    virtual void OnUpdateContextMenu();
    void OnUpdateSize();
    void AdjustRefreshRate(int rate);

    // Generated message map functions
protected:
    //{{AFX_MSG(CVideoWnd)
    afx_msg void OnContextvideoSourceformat();
    afx_msg void OnContextvideoVideosource();
    afx_msg void OnPaint();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnContextMenu(CWnd *pWnd, CPoint point);
    afx_msg void OnContextvideoEdittransparencyrefreshrate();
    afx_msg void OnContextvideoEdittransparency();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.