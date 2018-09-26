#pragma once

#include "virtual_screen_info.h"
#include <CamCapture/cam_rect.h>
#include <memory>

class settings_model;
class mouse_capture_ui;

class CFixedRegionDlg : public CDialog
{
    DECLARE_DYNAMIC(CFixedRegionDlg)
public:
    explicit CFixedRegionDlg(CWnd *pParent, virtual_screen_info screen_info, settings_model &settings);

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
    virtual_screen_info screen_info_;

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
