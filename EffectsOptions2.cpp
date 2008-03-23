// EffectsOptions2.cpp : implementation file
//

#include "stdafx.h"
#include "vscap.h"
#include "EffectsOptions2.h"


// CEffectsOptions2 dialog

IMPLEMENT_DYNAMIC(CEffectsOptions2, CDialog)
CEffectsOptions2::CEffectsOptions2(CWnd* pParent /*=NULL*/)
	: CDialog(CEffectsOptions2::IDD, pParent)
{
}

CEffectsOptions2::~CEffectsOptions2()
{
}

void CEffectsOptions2::DoDataExchange(CDataExchange* pDX)
{
	int* pPos = (int*)(&m_params.position);
	int& pos = *pPos;
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_TOP_LEFT, pos);
}


BEGIN_MESSAGE_MAP(CEffectsOptions2, CDialog)
END_MESSAGE_MAP()


// CEffectsOptions2 message handlers
