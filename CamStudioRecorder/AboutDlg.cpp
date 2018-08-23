#include "stdafx.h"
#include "AboutDlg.h"

#include "RecorderVersionReleaseInfo.h"
#include <fmt/format.h>

CAboutDlg::CAboutDlg()
    : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STATIC_VERSION, m_ctrlStaticVersion);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
    ON_STN_CLICKED(IDC_STATIC_VERSION, &CAboutDlg::OnStnClickedStaticVersion)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CAboutDlg, CDialog)
END_EVENTSINK_MAP()

BOOL CAboutDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // only needs to be done once.
    const auto about_text = fmt::format(
        L"Version: v{}\n"
        L"Copyright \xA9 2001 RenderSoft Software and Web Publishing\n"
        L"Copyright \xA9 2008 CamStudio Group && Contributors\n"
        L"Copyright \xA9 2018 Steven Hoving",
        CURRENT_VERSION_NUMBER);

    m_ctrlStaticVersion.SetWindowText(about_text.c_str());

    return TRUE;
}

void CAboutDlg::OnBnClickedButtonlink2()
{
}

void CAboutDlg::OnButtonlink()
{
}

void CAboutDlg::OnStnClickedStaticVersion()
{
    // TODO: Add your control notification handler code here
}
