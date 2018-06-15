#pragma once

#include <memory>

#include <CamCapture/cam_rect.h>

class settings_model;
class mouse_capture_ui;

static rect<int> from_crect(const CRect &rect)
{
    return {rect.left, rect.top, rect.right, rect.bottom};
}

static CRect from_rect(const rect<int> &rect)
{
    return {rect.left(), rect.top(), rect.right(), rect.bottom()};
}

class CFixedRegionDlg : public CDialog
{
    DECLARE_DYNAMIC(CFixedRegionDlg)
public:
    explicit CFixedRegionDlg(CWnd *pParent, settings_model &settings);

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

protected:
    // Generated message map functions
    //{{AFX_MSG(CFixedRegionDlg)
    virtual void OnOK();
    afx_msg void OnRegionSelect();
    virtual BOOL OnInitDialog();
    afx_msg void OnFixedtopleft();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
private:
    CEdit width_text_edit_ctrl_;
    CEdit height_text_edit_ctrl_;
    CEdit left_text_edit_ctrl_;
    CEdit top_text_edit_ctrl_;
    CButton m_ctrlButtonMouseDrag;
    CButton m_ctrlButtonFixTopLeft;
    rect<int> capture_rect_;

    settings_model &settings_;

    std::unique_ptr<mouse_capture_ui> capture_;

public:
    afx_msg void OnEnChangeX();
    afx_msg void OnEnChangeY();
    afx_msg void OnEnChangeWidth();
    afx_msg void OnEnChangeHeight();
    afx_msg void OnBnClickedOk();
    afx_msg void OnEnKillfocusWidth();
    afx_msg void OnEnKillfocusHeight();
    afx_msg void OnRegionMove();
    afx_msg void OnBnClickedSupportmousedrag();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
