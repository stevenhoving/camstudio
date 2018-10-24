// RenderSoft CamStudio
//
// Copyright 2001 RenderSoft Software & Web Publishing
//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "FixedRegionDlg.h"

#include "settings_model.h"
#include "mouse_capture_ui.h"
#include "utility/rect_util.h"

#include "MainFrm.h" // only for g_maxx_screen and alike
#include <CamLib/CamError.h>

IMPLEMENT_DYNAMIC(CFixedRegionDlg, CDialog)

CFixedRegionDlg::CFixedRegionDlg(CWnd *pParent, cam::virtual_screen_info screen_info, settings_model &settings)
    : CDialog(CFixedRegionDlg::IDD, pParent)
    , capture_rect_(0, 0, 0, 0)
    , settings_(settings)
    , screen_info_(screen_info)
{
    capture_ = std::make_unique<mouse_capture_ui>(AfxGetInstanceHandle(), GetSafeHwnd(),
        screen_info_,
        [this](const CRect &capture_rect)
        {
            if (!m_ctrlButtonFixTopLeft.GetCheck())
            {
                capture_rect_.left(capture_rect.left);
                capture_rect_.top(capture_rect.top);
            }
            capture_rect_.width(capture_rect.Width());
            capture_rect_.height(capture_rect.Height());

            settings_.set_capture_rect(capture_rect_);

            UpdateData(FALSE);
        }
    );

    capture_rect_ = settings_.get_capture_rect();
}

void CFixedRegionDlg::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_WIDTH, width_text_edit_ctrl_);
    DDX_Control(pDX, IDC_HEIGHT, height_text_edit_ctrl_);
    DDX_Control(pDX, IDC_X, left_text_edit_ctrl_);
    DDX_Control(pDX, IDC_Y, top_text_edit_ctrl_);
    DDX_Control(pDX, IDC_SUPPORTMOUSEDRAG, m_ctrlButtonMouseDrag);
    DDX_Control(pDX, IDC_FIXEDTOPLEFT, m_ctrlButtonFixTopLeft);

    DDX_Text(pDX, IDC_X, capture_rect_.left_);
    DDV_MinMaxInt(pDX, capture_rect_.left_, screen_info_.size.left(), screen_info_.size.right());
    DDX_Text(pDX, IDC_Y, capture_rect_.top_);
    DDV_MinMaxInt(pDX, capture_rect_.top_, screen_info_.size.top(), screen_info_.size.bottom());
    int width = capture_rect_.width();
    DDX_Text(pDX, IDC_WIDTH, width);
    DDV_MinMaxInt(pDX, width, 0, abs(screen_info_.size.width()));
    int height = capture_rect_.height();
    DDX_Text(pDX, IDC_HEIGHT, height);
    DDV_MinMaxInt(pDX, height, 0, abs(screen_info_.size.height()));
}

BEGIN_MESSAGE_MAP(CFixedRegionDlg, CDialog)
ON_BN_CLICKED(IDSELECT, &CFixedRegionDlg::OnRegionSelect)
ON_BN_CLICKED(IDC_FIXEDTOPLEFT, &CFixedRegionDlg::OnFixedtopleft)
ON_EN_CHANGE(IDC_Y, &CFixedRegionDlg::OnEnChangeY)
ON_EN_CHANGE(IDC_HEIGHT, &CFixedRegionDlg::OnEnChangeHeight)
ON_BN_CLICKED(IDOK, &CFixedRegionDlg::OnBnClickedOk)
ON_EN_KILLFOCUS(IDC_WIDTH, &CFixedRegionDlg::OnEnKillfocusWidth)
ON_EN_KILLFOCUS(IDC_HEIGHT, &CFixedRegionDlg::OnEnKillfocusHeight)
ON_EN_CHANGE(IDC_X, &CFixedRegionDlg::OnEnChangeX)
ON_EN_CHANGE(IDC_WIDTH, &CFixedRegionDlg::OnEnChangeWidth)
ON_BN_CLICKED(IDC_REGION_MOVE, &CFixedRegionDlg::OnRegionMove)
ON_BN_CLICKED(IDC_SUPPORTMOUSEDRAG, &CFixedRegionDlg::OnBnClickedSupportmousedrag)
END_MESSAGE_MAP()

void CFixedRegionDlg::OnOK()
{
    if (!UpdateData())
    {
        return;
    }

    int maxWidth = abs(screen_info_.size.width());
    int maxHeight = abs(screen_info_.size.height());

    if (capture_rect_.width() < 0)
    {
        MessageOut(m_hWnd, IDS_STRING_WIDTHGREATER, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
        return;
    }

    if (maxWidth < capture_rect_.width())
    {
        MessageOut(m_hWnd, IDS_STRING_WIDTHSMALLER, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION, screen_info_.size.width());
        return;
    }

    if (capture_rect_.height() < 0)
    {
        MessageOut(m_hWnd, IDS_STRING_HEIGHTGREATER, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
        return;
    }

    if (maxHeight < capture_rect_.height())
    {
        MessageOut(m_hWnd, IDS_STRING_HEIGHTSMALLER, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION, screen_info_.size.height());
        return;
    }

    int fval = m_ctrlButtonFixTopLeft.GetCheck();
    if (fval)
    {
        if (capture_rect_.left_ < screen_info_.size.left())
        {
            MessageOut(m_hWnd, IDS_STRING_LEFTGREATER, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
            return;
        }

        if (capture_rect_.left_ >= screen_info_.size.right())
        {
            MessageOut(this->m_hWnd, IDS_STRING_LEFTSMALLER, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION, screen_info_.size.right());
            return;
        }

        if (capture_rect_.top_ < screen_info_.size.top())
        {
            MessageOut(m_hWnd, IDS_STRING_TOPGREATER, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
            return;
        }

        if (capture_rect_.top_ >= screen_info_.size.bottom())
        {
            MessageOut(m_hWnd, IDS_STRING_TOPSMALLER, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION, screen_info_.size.bottom());
            return;
        }

        if (maxWidth < (capture_rect_.left_ + capture_rect_.width()))
        {
            capture_rect_.width(screen_info_.size.width() - capture_rect_.left_);
            if (capture_rect_.width() <= 0)
            {
                capture_rect_.left_ = screen_info_.size.left() + 100;
                capture_rect_.width(320);
            }
            MessageOut(m_hWnd, IDS_STRING_VALUEEXCEEDWIDTH, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION, capture_rect_.width());
        }

        if (maxHeight < (capture_rect_.top_ + capture_rect_.height()))
        {
            capture_rect_.height(screen_info_.size.height() - capture_rect_.top_);
            if (capture_rect_.height() <= 0)
            {
                // TODO -- where did these constants come from? Get rid of 'em, put 'em in an ini or #define them
                // somewhere Answer: See struct sRegionOpts in Profile.h. An area of 240x320 is defined there.
                capture_rect_.top_ = screen_info_.size.top() + 100;
                capture_rect_.height(240);
            }
            MessageOut(m_hWnd, IDS_STRING_VALUEEXCEEDHEIGHT, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION, capture_rect_.height());
        }
    }

    settings_.set_capture_rect(capture_rect_);

    settings_.set_region_mouse_drag(m_ctrlButtonMouseDrag.GetCheck() ? true : false);

    CDialog::OnOK();
}

BOOL CFixedRegionDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    capture_rect_ = settings_.get_capture_rect();

    UpdateData(FALSE);

    const auto capture_fixed = settings_.get_region_fixed();
    left_text_edit_ctrl_.EnableWindow(TRUE);
    top_text_edit_ctrl_.EnableWindow(TRUE);
    m_ctrlButtonFixTopLeft.SetCheck(capture_fixed);
    left_text_edit_ctrl_.EnableWindow(capture_fixed);
    top_text_edit_ctrl_.EnableWindow(capture_fixed);

    const auto capture_mouse_drag = settings_.get_region_mouse_drag();
    m_ctrlButtonMouseDrag.SetCheck(capture_mouse_drag);

    return TRUE;
}

void CFixedRegionDlg::OnRegionSelect()
{
    capture_->set_modify_mode(modify_mode::select);
    capture_->show(capture_rect_, capture_type::fixed);
}

void CFixedRegionDlg::OnRegionMove()
{
    capture_->set_modify_mode(modify_mode::move);
    capture_->show(capture_rect_, capture_type::fixed);
}

void CFixedRegionDlg::OnFixedtopleft()
{
    const auto checked = m_ctrlButtonFixTopLeft.GetCheck();
    left_text_edit_ctrl_.EnableWindow(checked);
    top_text_edit_ctrl_.EnableWindow(checked);
    settings_.set_region_fixed(checked != 0);
}

void CFixedRegionDlg::OnBnClickedSupportmousedrag()
{
    const auto checked = m_ctrlButtonMouseDrag.GetCheck();
    settings_.set_region_mouse_drag(checked != 0);
}

std::wstring get_window_text(const CEdit &text_edit)
{
    wchar_t text[255 + 1]; // just limit it to 255 for now
    text_edit.GetWindowText(text, 255);
    return text;
}

void set_window_text(CEdit &text_edit, const std::wstring &text)
{
    text_edit.SetWindowText(text.c_str());
}

void CFixedRegionDlg::OnEnChangeX()
{
    const auto left_text = get_window_text(left_text_edit_ctrl_);
    if (left_text.empty())
        return;
    capture_rect_.left_ = std::stoi(left_text);
}

void CFixedRegionDlg::OnEnChangeY()
{
    const auto top_text = get_window_text(top_text_edit_ctrl_);
    if (top_text.empty())
        return;
    capture_rect_.top_ = std::stoi(top_text);
}

void CFixedRegionDlg::OnEnChangeWidth()
{
    const auto width_text = get_window_text(width_text_edit_ctrl_);
    if (width_text.empty())
        return;
    capture_rect_.width(std::stoi(width_text));
}

void CFixedRegionDlg::OnEnChangeHeight()
{
    const auto height_text = get_window_text(height_text_edit_ctrl_);
    if (height_text.empty())
        return;
    capture_rect_.height(std::stoi(height_text));
}

void CFixedRegionDlg::OnBnClickedOk()
{
    OnOK();
}

void CFixedRegionDlg::OnEnKillfocusWidth()
{
    if (capture_rect_.width() % 2 != 0)
    {
        capture_rect_.width(capture_rect_.width() + 1);
        set_window_text(width_text_edit_ctrl_, std::to_wstring(capture_rect_.width()));
    }
}

void CFixedRegionDlg::OnEnKillfocusHeight()
{
    if (capture_rect_.height() % 2 != 0)
    {
        capture_rect_.height(capture_rect_.height() + 1);
        set_window_text(height_text_edit_ctrl_, std::to_wstring(capture_rect_.height()));
    }
}
