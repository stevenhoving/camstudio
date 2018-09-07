#include "stdafx.h"
#include "BasicMessageDlg.h"

IMPLEMENT_DYNAMIC(CBasicMessageDlg, CDialog)

CBasicMessageDlg::CBasicMessageDlg(CWnd *pParent /*=nullptr*/)
    : CDialog(CBasicMessageDlg::IDD, pParent)
    , m_bCancelled(false)
{
}

CBasicMessageDlg::~CBasicMessageDlg()
{
}

void CBasicMessageDlg::DoDataExchange(CDataExchange *pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_MESSAGE, m_MessageText);
}

BEGIN_MESSAGE_MAP(CBasicMessageDlg, CDialog)
ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

// CBasicMessageDlg message handlers
void CBasicMessageDlg::SetText(LPCTSTR lpString)
{
    strText = lpString;
}

void CBasicMessageDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CDialog::OnShowWindow(bShow, nStatus);

    m_MessageText.SetWindowText(strText);
    m_MessageText.HideCaret();
}

void CBasicMessageDlg::OnCancel()
{
    m_bCancelled = true;
    ReleaseCapture();
    this->ShowWindow(SW_HIDE);
}
bool CBasicMessageDlg::Cancelled()
{
    return m_bCancelled;
}
