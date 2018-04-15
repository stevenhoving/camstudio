#include "stdafx.h"
#include "Recorder.h"
#include "UpdateDialog.h"

// CUpdateDialog dialog

IMPLEMENT_DYNAMIC(CUpdateDialog, CDialogEx)

CUpdateDialog::CUpdateDialog(CWnd *pParent /*=nullptr*/)
    : CDialogEx(CUpdateDialog::IDD, pParent)
{
}

CUpdateDialog::~CUpdateDialog()
{
}

void CUpdateDialog::DoDataExchange(CDataExchange *pDX)
{
    CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CUpdateDialog, CDialogEx)
ON_BN_CLICKED(ID_UPDATE, OnBnClickedButtonlink)
END_MESSAGE_MAP()

void CUpdateDialog::OnBnClickedButtonlink()
{
    const auto *mode = _T("open");
    ShellExecute(GetSafeHwnd(), mode, _T("http://www.camstudio.org/"), nullptr, nullptr, SW_SHOW);
    this->ShowWindow(SW_HIDE);
}
// CUpdateDialog message handlers
