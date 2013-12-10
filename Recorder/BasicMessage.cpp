// BasicMessage.cpp : implementation file
//

#include "stdafx.h"
//#include "Recorder.h"
#include "BasicMessage.h"

// CBasicMessage dialog

IMPLEMENT_DYNAMIC(CBasicMessage, CDialog)

CBasicMessage::CBasicMessage(CWnd* pParent /*=NULL*/)
	: CDialog(CBasicMessage::IDD, pParent)
	, m_bCancelled(false)
{
}

CBasicMessage::~CBasicMessage()
{
}

void CBasicMessage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MESSAGE, m_MessageText);
}

BEGIN_MESSAGE_MAP(CBasicMessage, CDialog)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

// CBasicMessage message handlers
void CBasicMessage::SetText(LPCTSTR lpString)
{
	strText = lpString;
}

void CBasicMessage::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	// TODO: Add your message handler code here
	m_MessageText.SetWindowText(strText);
	m_MessageText.HideCaret();
}

void CBasicMessage::OnCancel()
{
	m_bCancelled = true;
	ReleaseCapture();
	this->ShowWindow(SW_HIDE);
}
bool CBasicMessage::Cancelled()
{
	return m_bCancelled;
}

