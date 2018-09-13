#include "stdafx.h"
#include "AboutDlg.h"

#include "buildinfo.h"
#include <fmt/format.h>

CAboutDlg::CAboutDlg()
    : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STATIC_VERSION, version_copyright_label_);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CAboutDlg, CDialog)
END_EVENTSINK_MAP()

BOOL CAboutDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    const auto about_text = fmt::format(
        "Version: v{}\n"
        "Copyright \xA9 2001 RenderSoft Software and Web Publishing\n"
        "Copyright \xA9 2008 CamStudio Group && Contributors\n"
        "Copyright \xA9 2018 Steven Hoving",
        buildinfo::full_version);

    version_copyright_label_.SetWindowText(CString(about_text.c_str()));

    return TRUE;
}
