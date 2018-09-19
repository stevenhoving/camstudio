#pragma once

#include <memory>

#include <CamCapture/cam_rect.h>

class settings_model;
class mouse_capture_ui;

static cam::rect<int> from_rect(const CRect &rect)
{
    return {rect.left, rect.top, rect.right, rect.bottom};
}

static CRect from_rect(const cam::rect<int> &rect)
{
    return {rect.left(), rect.top(), rect.right(), rect.bottom()};
}

class CFixedRegionDlg : public CDialog
{
    DECLARE_DYNAMIC(CFixedRegionDlg)
public:
    explicit CFixedRegionDlg(CWnd *pParent, settings_model &settings);

    enum
    {
        IDD = IDD_FIXEDREGION
    };

protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    afx_msg void OnRegionSelect();
    afx_msg void OnFixedtopleft();
    DECLARE_MESSAGE_MAP()
private:
    CEdit width_text_edit_ctrl_;
    CEdit height_text_edit_ctrl_;
    CEdit left_text_edit_ctrl_;
    CEdit top_text_edit_ctrl_;
    CButton m_ctrlButtonMouseDrag;
    CButton m_ctrlButtonFixTopLeft;
    cam::rect<int> capture_rect_;

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
