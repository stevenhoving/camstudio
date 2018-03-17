#pragma once

class CNewShapeDlg : public CDialog
{
public:
    explicit CNewShapeDlg(CWnd *pParent = nullptr);

    // Dialog Data
    //{{AFX_DATA(CNewShapeDlg)
    enum
    {
        IDD = IDD_NEWSHAPE
    };
    // NOTE: the ClassWizard will add data members here
    //}}AFX_DATA

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CNewShapeDlg)
protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(CNewShapeDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnClickBlankImage();
    afx_msg void OnClickImageFile();
    virtual void OnOK();
    afx_msg void OnFindImageFile();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
private:
    CString m_imageDir;
    CEdit m_ctrlTextFileText;
    CEdit m_ctrlEditName;
    CButton m_ctrlButtoBlankImage;
    CButton m_ctrlButtoImageFile;
    CStatic m_ctrlStaticWidth;
    CStatic m_ctrlStaticHeight;
    CButton m_ctrlButtonImageFile;
    CEdit m_ctrlEditWidth;
    CEdit m_ctrlEditHeight;
    CEdit m_ctrlEditShaepText;
    UINT m_uImageWidth;
    UINT m_uImageHeight;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

