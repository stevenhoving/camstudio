// EffectsOptions.cpp : implementation file
//

#include "stdafx.h"
#include "../Recorder.h"
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
	ON_BN_CLICKED(IDOK, &CEffectsOptionsDlg::OnBnClickedOk)
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

void CEffectsOptionsDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here

	// Free floating position not yet support in this dialog. but other modules (screen.cpp) still support it
	m_params.posType   = PREDEFINED_POSITION;
	m_params.xPosRatio = EffectsOptionsSetXPosRatio( m_params.position );
	m_params.yPosRatio = EffectsOptionsSetYPosRatio( m_params.position );

	// TRACE("CEffectsOptionsDlg::EffectsOptionsSetXPosRatio  nPosition=[%i], xRatio=[%i], yRatio=[%i]\n",  m_params.position, m_params.xPosRatio, m_params.yPosRatio );

	OnOK();
}



// Define Position X Ratio
// Scale  LEFT=0, CENTER= 50, RIGHT= 100
int CEffectsOptionsDlg::EffectsOptionsSetXPosRatio( int nPosition ){

	int nRatio = 0;

	// Define X ratio
	switch (nPosition)
	{
	case TOP_LEFT:
	case CENTER_LEFT:
	case BOTTOM_LEFT:
		nRatio = 0;
		break;
	case TOP_CENTER:
	case CENTER_CENTER:
	case BOTTOM_CENTER:
		nRatio = 50;
		break;
	case TOP_RIGHT:
	case CENTER_RIGHT:
	case BOTTOM_RIGHT:
		nRatio = 100;
		break;
	default:
		nRatio = 0;
		break;
	}
	return ( nRatio );
}

// Define Position Y Ratio
// Scale  TOP=0,  CENTER= 50, BOTTOM= 100			
int CEffectsOptionsDlg::EffectsOptionsSetYPosRatio( int nPosition )
{
	int nRatio = 0;

	// Define outer Y offset
	switch (nPosition)
	{
	case TOP_LEFT:
	case TOP_CENTER:
	case TOP_RIGHT:
		nRatio = 0;
		break;
	case CENTER_LEFT:
	case CENTER_CENTER:
	case CENTER_RIGHT:
		nRatio = 50;
		break;
	case BOTTOM_LEFT:
	case BOTTOM_CENTER:
	case BOTTOM_RIGHT:
		nRatio = 100;
		break;
	default:
		nRatio = 0;
		break;
	}
	return ( nRatio );
}
