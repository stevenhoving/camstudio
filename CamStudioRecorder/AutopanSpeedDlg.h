#pragma once

class CAutopanSpeedDlg : public CDialog
{
public:
    explicit CAutopanSpeedDlg(CWnd *pParent = nullptr);

    // Dialog Data
    //{{AFX_DATA(CAutopanSpeedDlg)
    enum
    {
        IDD = IDD_AUTOPANSPEED
    };
    // NOTE: the ClassWizard will add data members here
    //}}AFX_DATA

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAutopanSpeedDlg)
protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(CAutopanSpeedDlg)
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
private:
    CSliderCtrl m_ctrlSliderPanSpeed;
    CStatic m_ctrlStaticMaxSpeed;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

