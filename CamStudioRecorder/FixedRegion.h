#pragma once

class CFixedRegionDlg : public CDialog
{
    DECLARE_DYNAMIC(CFixedRegionDlg)
public:
    explicit CFixedRegionDlg(CWnd *pParent = nullptr);

    // Dialog Data
    //{{AFX_DATA(CFixedRegionDlg)
    enum
    {
        IDD = IDD_FIXEDREGION
    };
    // NOTE: the ClassWizard will add data members here
    //}}AFX_DATA

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CFixedRegionDlg)
protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(CFixedRegionDlg)
    virtual void OnOK();
    afx_msg void OnSelect();
    virtual BOOL OnInitDialog();
    afx_msg void OnFixedtopleft();
    //}}AFX_MSG
    afx_msg LRESULT OnRegionUpdate(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnDisplayChange(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()
private:
    CStatic m_ctrlStaticMsg;
    CEdit m_ctrlEditWidth;
    CEdit m_ctrlEditHeight;
    CEdit m_ctrlEditPosX;
    CEdit m_ctrlEditPosY;
    CButton m_ctrlButtonMouseDrag;
    CButton m_ctrlButtonFixTopLeft;
    // CButton m_ctrlButtonRoundDown;
    int m_iLeft;
    int m_iTop;
    int m_iWidth;
    int m_iHeight;
    int m_iRNDWidth;
    int m_nRNDHeight;
    void RoundDownWidth();
    void RoundDownHeight();

public:
    afx_msg void OnEnChangeY();
    afx_msg void OnEnChangeHeight();
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedSupportrounddown();
    afx_msg void OnEnKillfocusWidth();
    afx_msg void OnEnKillfocusHeight();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.


