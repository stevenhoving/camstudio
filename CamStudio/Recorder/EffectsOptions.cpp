// EffectsOptions.cpp : implementation file
//

#include "stdafx.h"
#include "Recorder.h"
#include "EffectsOptions.h"
#include ".\effectsoptions.h"

// CEffectsOptionsDlg dialog

IMPLEMENT_DYNAMIC(CEffectsOptionsDlg, CDialog)
CEffectsOptionsDlg::CEffectsOptionsDlg(CWnd* pParent /*=NULL*/)
: CDialog(CEffectsOptionsDlg::IDD, pParent)
// , m_Position(TOP_LEFT)
{
}

CEffectsOptionsDlg::~CEffectsOptionsDlg()
{
}

void CEffectsOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	int* pPos = (int*)(&m_params.position);
	int& pos = *pPos;
	CDialog::DoDataExchange(pDX);
	//DDX_Radio(pDX, IDC_RADIO_TOP_LEFT, m_Position);
	DDX_Radio(pDX, IDC_RADIO_TOP_LEFT, pos);
}

BEGIN_MESSAGE_MAP(CEffectsOptionsDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_BACKGROUND_COLOR, OnBnClickedButtonBackgroundColor)
	ON_BN_CLICKED(IDC_BUTTON_TEXT_COLOR, OnBnClickedButtonTextColor)
	ON_BN_CLICKED(IDC_BUTTON_FONT, OnBnClickedButtonFont)
END_MESSAGE_MAP()

// CEffectsOptionsDlg message handlers

void CEffectsOptionsDlg::OnBnClickedButtonBackgroundColor()
{
	// TODO: Add your control notification handler code here
	CColorDialog dlg;
	dlg.m_cc.Flags |= CC_FULLOPEN | CC_RGBINIT;
	dlg.m_cc.rgbResult = m_params.backgroundColor;
	if (dlg.DoModal() == IDOK){
		m_params.backgroundColor = dlg.GetColor();
	}
}

void CEffectsOptionsDlg::OnBnClickedButtonTextColor()
{
	// TODO: Add your control notification handler code here
	CColorDialog dlg;
	dlg.m_cc.Flags |= CC_FULLOPEN | CC_RGBINIT;
	dlg.m_cc.rgbResult = m_params.textColor;
	if (dlg.DoModal() == IDOK){
		m_params.textColor = dlg.GetColor();
	}
}

void CEffectsOptionsDlg::OnBnClickedButtonFont()
{
	// TODO: Add your control notification handler code here
	CFontDialog dlg(&m_params.logfont);
	if (dlg.DoModal() == IDOK){
		memcpy(&m_params.logfont, dlg.m_cf.lpLogFont, sizeof(LOGFONT));
		m_params.isFontSelected = 1;
	}
}
