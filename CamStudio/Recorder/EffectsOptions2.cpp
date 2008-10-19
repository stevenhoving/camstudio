// EffectsOptions2.cpp : implementation file
//

#include "stdafx.h"
#include "Recorder.h"
#include "EffectsOptions2.h"

// CEffectsOptions2Dlg dialog

IMPLEMENT_DYNAMIC(CEffectsOptions2Dlg, CDialog)
CEffectsOptions2Dlg::CEffectsOptions2Dlg(CWnd* pParent /*=NULL*/)
: CDialog(CEffectsOptions2Dlg::IDD, pParent)
{
}

CEffectsOptions2Dlg::~CEffectsOptions2Dlg()
{
}

void CEffectsOptions2Dlg::DoDataExchange(CDataExchange* pDX)
{
	int* pPos = (int*)(&m_params.position);
	int& pos = *pPos;
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_TOP_LEFT, pos);
}

BEGIN_MESSAGE_MAP(CEffectsOptions2Dlg, CDialog)
END_MESSAGE_MAP()

// CEffectsOptions2Dlg message handlers
