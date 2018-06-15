// RenderSoft CamStudio
//
// Copyright 2001 RenderSoft Software & Web Publishing
//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "FixedRegionDlg.h"
#include "settings_model.h"

#include "MouseCaptureWnd.h" // for g_hFixedRegionWnd and g_hMouseCaptureWnd
#include "Recorder.h" // only for WM_APP_REGIONUPDATE
#include "MainFrm.h" // only for g_maxx_screen and alike
#include <CamLib/CamError.h>
#include <fmt/printf.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int g_iDefineMode;

IMPLEMENT_DYNAMIC(CFixedRegionDlg, CDialog)

CFixedRegionDlg::CFixedRegionDlg(CWnd *pParent, settings_model &settings)
    : CDialog(CFixedRegionDlg::IDD, pParent)
    , m_iLeft(1)
    , m_iTop(1)
    , m_iWidth(1)
    , m_iHeight(1)
    , m_iRNDWidth(0)
    , m_nRNDHeight(0)
    , settings_(settings)
{
    //capture_ = std::make_unique<mouse_capture_wnd>(settings, GetSafeHwnd());
}

void CFixedRegionDlg::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);

    fmt::print("CFixedRegionDlg::DoDataExchange\n");

    DDX_Control(pDX, IDC_MSG, m_ctrlStaticMsg);
    DDX_Control(pDX, IDC_WIDTH, m_ctrlEditWidth);
    DDX_Control(pDX, IDC_HEIGHT, m_ctrlEditHeight);
    DDX_Control(pDX, IDC_X, m_ctrlEditPosX);
    DDX_Control(pDX, IDC_Y, m_ctrlEditPosY);
    DDX_Control(pDX, IDC_SUPPORTMOUSEDRAG, m_ctrlButtonMouseDrag);
    DDX_Control(pDX, IDC_FIXEDTOPLEFT, m_ctrlButtonFixTopLeft);

    DDX_Text(pDX, IDC_X, m_iLeft);
    DDV_MinMaxInt(pDX, m_iLeft, g_minx_screen, g_maxx_screen);
    DDX_Text(pDX, IDC_Y, m_iTop);
    DDV_MinMaxInt(pDX, m_iTop, g_miny_screen, g_maxy_screen);
    DDX_Text(pDX, IDC_WIDTH, m_iWidth);
    DDV_MinMaxInt(pDX, m_iWidth, 0, abs(g_maxx_screen - g_minx_screen));
    DDX_Text(pDX, IDC_HEIGHT, m_iHeight);
    DDV_MinMaxInt(pDX, m_iHeight, 0, abs(g_maxy_screen - g_miny_screen));
}

BEGIN_MESSAGE_MAP(CFixedRegionDlg, CDialog)
ON_BN_CLICKED(IDSELECT, OnSelect)
ON_BN_CLICKED(IDC_FIXEDTOPLEFT, OnFixedtopleft)
ON_MESSAGE(WM_APP_REGIONUPDATE, OnRegionUpdate)
ON_MESSAGE(WM_DISPLAYCHANGE, OnDisplayChange)
ON_EN_CHANGE(IDC_Y, &CFixedRegionDlg::OnEnChangeY)
ON_EN_CHANGE(IDC_HEIGHT, &CFixedRegionDlg::OnEnChangeHeight)
ON_BN_CLICKED(IDOK, &CFixedRegionDlg::OnBnClickedOk)
ON_EN_KILLFOCUS(IDC_WIDTH, &CFixedRegionDlg::OnEnKillfocusWidth)
ON_EN_KILLFOCUS(IDC_HEIGHT, &CFixedRegionDlg::OnEnKillfocusHeight)
END_MESSAGE_MAP()

void CFixedRegionDlg::OnOK()
{
    if (!UpdateData())
    {
        return;
    }

    // Not Correct, one to high because MinXY is zero an MaxXY is width/height no of pixels.
    int maxWidth = abs(g_maxx_screen - g_minx_screen); // Assuming number first pixel is one not zero.
    int maxHeight = abs(g_maxy_screen - g_miny_screen);
    // TRACE(_T("## CFixedRegionDlg::OnOK / maxWidth=[%d], maxHeight=[%d]\n"), maxWidth, maxHeight );

    if (m_iWidth < 0)
    {
        MessageOut(m_hWnd, IDS_STRING_WIDTHGREATER, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
        return;
    }

    if (maxWidth < m_iWidth)
    {
        MessageOut(m_hWnd, IDS_STRING_WIDTHSMALLER, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION, g_maxx_screen);
        return;
    }

    if (m_iHeight < 0)
    {
        MessageOut(m_hWnd, IDS_STRING_HEIGHTGREATER, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
        return;
    }

    if (maxHeight < m_iHeight)
    {
        MessageOut(m_hWnd, IDS_STRING_HEIGHTSMALLER, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION, g_maxy_screen);
        return;
    }

    // version 1.5
    int fval = m_ctrlButtonFixTopLeft.GetCheck();
    if (fval)
    {
        if (m_iLeft < g_minx_screen)
        {
            MessageOut(m_hWnd, IDS_STRING_LEFTGREATER, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
            return;
        }

        if (g_maxx_screen < m_iLeft)
        {
            MessageOut(this->m_hWnd, IDS_STRING_LEFTSMALLER, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION, g_maxx_screen);
            return;
        }

        if (m_iTop < g_miny_screen)
        {
            MessageOut(m_hWnd, IDS_STRING_TOPGREATER, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION);
            return;
        }

        if (g_maxy_screen < m_iTop)
        {
            MessageOut(m_hWnd, IDS_STRING_TOPSMALLER, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION, g_maxy_screen);
            return;
        }

        if (maxWidth < (m_iLeft + m_iWidth))
        {
            m_iWidth = g_maxx_screen - m_iLeft;
            if (m_iWidth <= 0)
            {
                // TODO -- where did these constants came from? Get rid of 'em, put 'em in an ini or #define them
                // somewhere Answer: See struct sRegionOpts in Profile.h. An area of 240x320 is defined there.
                m_iLeft = g_minx_screen + 100;
                m_iWidth = 320;
            }
            MessageOut(m_hWnd, IDS_STRING_VALUEEXCEEDWIDTH, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION, m_iWidth);
        }

        if (maxHeight < (m_iTop + m_iHeight))
        {
            m_iHeight = g_maxy_screen - m_iTop;
            if (m_iHeight <= 0)
            {
                // TODO -- where did these constants come from? Get rid of 'em, put 'em in an ini or #define them
                // somewhere Answer: See struct sRegionOpts in Profile.h. An area of 240x320 is defined there.
                m_iTop = g_miny_screen + 100;
                m_iHeight = 240;
            }
            MessageOut(m_hWnd, IDS_STRING_VALUEEXCEEDHEIGHT, IDS_STRING_NOTE, MB_OK | MB_ICONEXCLAMATION, m_iHeight);
        }
    }

    // Technical pixel coordinates are applicable.  Top-Left = 0:0 not 1:1 What user often think it is.
    // TRACE(_T("## CFixedRegionDlg::OnOK / L=%d, T=%d, W=%d, H=%d\n"), m_iLeft, m_iTop, m_iWidth, m_iHeight );
    settings_.set_capture_rect({
        m_iLeft,
        m_iTop,
        m_iLeft + m_iWidth,
        m_iTop + m_iHeight
    });

    settings_.set_mouse_drag(m_ctrlButtonMouseDrag.GetCheck() ? true : false);

    //cRegionOpts.m_bMouseDrag = ;
    CDialog::OnOK();
}

BOOL CFixedRegionDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    const auto captur_rect = settings_.get_capture_rect();
    m_iLeft = captur_rect.left();
    m_iTop = captur_rect.top();
    m_iWidth = captur_rect.width() + 1; // \todo make sure that fixing the rect width function does not have unwanted side effects
    m_iHeight = captur_rect.height() + 1;
    UpdateData(FALSE);

    const auto capture_fixed = settings_.get_capture_fixed();
    m_ctrlEditPosX.EnableWindow(TRUE);
    m_ctrlEditPosY.EnableWindow(TRUE);
    m_ctrlButtonFixTopLeft.SetCheck(capture_fixed);
    m_ctrlEditPosX.EnableWindow(capture_fixed);
    m_ctrlEditPosY.EnableWindow(capture_fixed);

    const auto capture_mouse_drag = settings_.get_mouse_drag();
    m_ctrlButtonMouseDrag.SetCheck(capture_mouse_drag);

    return TRUE;
}

void CFixedRegionDlg::OnSelect()
{
    m_ctrlStaticMsg.SetWindowText(_T("Click and drag to define a rectangle"));

    //cRegionOpts.m_iCaptureMode = CAPTURE_VARIABLE; // set temporarily to variable region
    g_iDefineMode = 1;
    g_hFixedRegionWnd = m_hWnd;
    //::ShowWindow(g_hMouseCaptureWnd, SW_SHOW);
    //::UpdateWindow(g_hMouseCaptureWnd);

    capture_ = std::make_unique<mouse_capture_wnd>(settings_, GetSafeHwnd());
    capture_->show();
    m_ctrlStaticMsg.SetWindowText(_T(""));
}

// OnRegionUpdate
// message handler for WM_APP_REGIONUPDATE
// TODO: why doesn't this message send position values?
LRESULT CFixedRegionDlg::OnRegionUpdate(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    fmt::print("CFixedRegionDlg::OnRegionUpdate\n");

    const auto captur_rect = settings_.get_capture_rect();

    if (!m_ctrlButtonFixTopLeft.GetCheck())
    {
        m_iLeft = captur_rect.left();
        m_iTop = captur_rect.top();
    }

    m_iWidth = captur_rect.width() + 1; // \todo make sure that fixing the rect width function does not have unwanted side effects
    m_iHeight = captur_rect.height() + 1;
    UpdateData(FALSE);

    //CRect rectUse(g_rcUse);
    //if (!m_ctrlButtonFixTopLeft.GetCheck())
    //{
    //    m_iLeft = rectUse.left;
    //    m_iTop = rectUse.top;
    //}
    //m_iWidth = rectUse.Width();
    //m_iHeight = rectUse.Height();
    //if (cVideoOpts.m_bRoundDown)
    {
        if (m_iWidth % 2 != 0)
            m_iWidth += 1;
        if (m_iHeight % 2 != 0)
            m_iHeight += 1;
    }

    // TRACE(_T("## CFixedRegionDlg::OnRegionUpdate / L=%d, T=%d, W=%d, H=%d\n"), m_iLeft, m_iTop, m_iWidth, m_iHeight
    // );

    UpdateData(FALSE);

    return 0;
}

void CFixedRegionDlg::OnFixedtopleft()
{
    int fixtl = m_ctrlButtonFixTopLeft.GetCheck();
    m_ctrlEditPosX.EnableWindow(fixtl);
    m_ctrlEditPosY.EnableWindow(fixtl);
}

LRESULT CFixedRegionDlg::OnDisplayChange(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    return 0;
}

void CFixedRegionDlg::OnEnChangeY()
{
}

void CFixedRegionDlg::OnEnChangeHeight()
{
    //if (cVideoOpts.m_bRoundDown)
    {
        RoundDownWidth();
    }
}

void CFixedRegionDlg::OnBnClickedOk()
{
    OnOK();
}

void CFixedRegionDlg::OnBnClickedSupportrounddown()
{
    //if (cVideoOpts.m_bRoundDown)
    {
        RoundDownHeight();
        RoundDownWidth();
    }
}

void CFixedRegionDlg::OnEnKillfocusWidth()
{
    //if (cVideoOpts.m_bRoundDown)
    {
        RoundDownWidth();
    }
}
void CFixedRegionDlg::OnEnKillfocusHeight()
{
    //if (cVideoOpts.m_bRoundDown)
    {
        RoundDownHeight();
    }
}

void CFixedRegionDlg::RoundDownHeight()
{
    CString sVal;
    m_ctrlEditHeight.GetWindowText(sVal);
    int nHeight = std::stoi(sVal.GetString());
    if ((nHeight % 2) != 0)
    {
        nHeight = nHeight - 1;
        sVal.Format(_T("%d"), nHeight);
        m_ctrlEditHeight.SetWindowText(sVal);
    }
}
void CFixedRegionDlg::RoundDownWidth()
{
    CString sVal;
    m_ctrlEditWidth.GetWindowText(sVal);
    int nWidth = std::stoi(sVal.GetString());
    if ((nWidth % 2) != 0)
    {
        nWidth = nWidth - 1;
        sVal.Format(_T("%d"), nWidth);
        m_ctrlEditWidth.SetWindowText(sVal);
    }
}
