#pragma once

#include "VideoWnd.h"

class CTransRateDlg : public CDialog
{
public:
    explicit CTransRateDlg(CWnd *pParent = nullptr);

    // Dialog Data
    //{{AFX_DATA(CTransRateDlg)
    enum
    {
        IDD = IDD_TRANSRATE
    };
    // NOTE: the ClassWizard will add data members here
    //}}AFX_DATA

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CTransRateDlg)
protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(CTransRateDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
    virtual void OnOK();
    virtual void OnCancel();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
private:
    CVideoWnd *m_myparent;
    CSliderCtrl m_ctrlSliderTransRate;
    CStatic m_ctrlStaticFrameRate;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

