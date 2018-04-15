#pragma once

// SlideToolBar.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CSlideToolBar window

class CSlideToolBar : public CToolBar
{

public:
    CSlideToolBar();

    // Attributes
public:
    CSliderCtrl m_wndSliderCtrl;
    int m_slidevalue;

    // Operations
public:
    void SetPositions();

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CSlideToolBar)
    //}}AFX_VIRTUAL

    // Implementation
public:
    void AdjustTimeBar(CRect clientrect);
    void UpdateValues();
    void EnableButton(BOOL setToOn, int nIndex);
    virtual ~CSlideToolBar();

    // Generated message map functions
protected:
    //{{AFX_MSG(CSlideToolBar)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
