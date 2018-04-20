#pragma once

class CTransparentWnd;

class CEditTransparencyDlg : public CDialog
{
    explicit CEditTransparencyDlg(CWnd *pParent = nullptr);
public:
    CEditTransparencyDlg(int &bEnable, int &iLevel, CTransparentWnd *pParent);

    // Dialog Data
    //{{AFX_DATA(CEditTransparencyDlg)
    enum
    {
        IDD = IDD_TRANSPARENCY
    };
    // NOTE: the ClassWizard will add data members here
    //}}AFX_DATA

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CEditTransparencyDlg)
protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(CEditTransparencyDlg)
    afx_msg void OnCheck1();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
    virtual BOOL OnInitDialog();
    virtual void OnCancel();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
public:
private:
    int &m_rbEnableTrans; // TODO: should be bool&
    int &m_riLevel;
    int m_bEnableTransOld; // TODO: should be bool
    int m_iLevelOld;
    CTransparentWnd *m_pTransparentWnd;

    CButton m_ctrlButtonEnableTranparency;
    CSliderCtrl m_ctrlSliderTransparency;
    CStatic m_ctrlStaticTransparency;

protected:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

