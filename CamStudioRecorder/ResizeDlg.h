#pragma once

#include "TransparentWnd.h"

class CResizeDlg : public CDialog
{
public:
    explicit CResizeDlg(CWnd *pParent = nullptr);
    CTransparentWnd *m_transWnd;
    void PreModal(CTransparentWnd *transWnd);

    // Dialog Data
    //{{AFX_DATA(CResizeDlg)
    enum
    {
        IDD = IDD_RESIZE
    };
    // NOTE: the ClassWizard will add data members here
    //}}AFX_DATA

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CResizeDlg)
protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(CResizeDlg)
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    afx_msg void OnReset();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
private:
    CSliderCtrl m_ctrlSliderWidth;
    CSliderCtrl m_ctrlSliderHeight;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

