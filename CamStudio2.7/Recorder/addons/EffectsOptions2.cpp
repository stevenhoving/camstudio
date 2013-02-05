// EffectsOptions2.cpp : implementation file
//

#include "stdafx.h"
#include "../Recorder.h"
#include "EffectsOptions2.h"

// CEffectsOptions2Dlg dialog

//IMPLEMENT_DYNAMIC(CEffectsOptions2Dlg, CDialog)

CEffectsOptions2Dlg::CEffectsOptions2Dlg(CWnd* pParent /*=NULL*/)
: CDialog(CEffectsOptions2Dlg::IDD, pParent)
, m_iPos(0)
, m_iBrightness(0)
, m_iContrast(0)
{
	//{{AFX_DATA_INIT(CEffectsOptions2Dlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CEffectsOptions2Dlg::~CEffectsOptions2Dlg()
{
}

void CEffectsOptions2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEffectsOptions2Dlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_STATIC_PREVIEW_IMAGE, m_ctrlStaticPreview);
	DDX_Radio(pDX, IDC_RADIO_TOP_LEFT, m_iPos);
	DDX_Control(pDX, IDC_SLIDER_BRIGHTNESS, m_ctrlSliderBrightness);
	DDX_Control(pDX, IDC_SLIDER_CONTRAST, m_ctrlSliderContrast);
}

BEGIN_MESSAGE_MAP(CEffectsOptions2Dlg, CDialog)
	//{{AFX_MSG_MAP(CEffectsOptions2Dlg)
	ON_WM_PAINT()
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CEffectsOptions2Dlg::OnBnClickedOk)
END_MESSAGE_MAP()

// CEffectsOptions2Dlg message handlers

BOOL CEffectsOptions2Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_iPos = (int)m_params.position;
	m_iContrast = m_params.m_lContrast;
	m_iBrightness = m_params.m_lBrightness;
	UpdateData(FALSE);

	m_ctrlSliderBrightness.SetRange(-255, 255, TRUE);
	m_ctrlSliderBrightness.SetPos(m_iBrightness);
	m_ctrlSliderContrast.SetRange(-100, 100, TRUE);
	m_ctrlSliderContrast.SetPos(m_iContrast);

	if (!m_params.text.IsEmpty()) {
		VERIFY(m_cImage.Load(m_params.text));
		m_cImageOriginal.Copy(m_cImage);
		m_cImage.SetTransIndex(m_cImage.GetPixelIndex(0L, 0L));
		m_cImage.Light(m_iBrightness, m_iContrast);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CEffectsOptions2Dlg::OnPaint()
{
	CPaintDC cPaintDC(this); // device context for painting
	CRect rectFrame;
	m_ctrlStaticPreview.GetWindowRect(rectFrame);
	ScreenToClient(&rectFrame);
	m_cImage.Draw(cPaintDC, rectFrame);
}

void CEffectsOptions2Dlg::OnOK()
{
	UpdateData();
	m_params.position = ePosition(m_iPos);
	m_params.m_lBrightness = m_ctrlSliderBrightness.GetPos();
	m_params.m_lContrast = m_ctrlSliderContrast.GetPos();
	// TODO: update m_params
	CDialog::OnOK();
}

void CEffectsOptions2Dlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	m_iBrightness = m_ctrlSliderBrightness.GetPos();
	m_iContrast = m_ctrlSliderContrast.GetPos();
	TRACE("Brightness: %d\nContrast: %d\n", m_iBrightness, m_iContrast);
	if (!m_params.text.IsEmpty()) {
		//VERIFY(m_cImage.Load(m_params.text));
		m_cImage.Copy(m_cImageOriginal);
		m_cImage.SetTransIndex(m_cImage.GetPixelIndex(0L, 0L));
		m_cImage.Light(m_iBrightness, m_iContrast);
		CRect rectFrame;
		m_ctrlStaticPreview.GetWindowRect(rectFrame);
		ScreenToClient(&rectFrame);
		InvalidateRect(&rectFrame);
	}
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CEffectsOptions2Dlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	// Free floating position not yet support in this dialog. but still used in other modules
	m_params.posType   = PREDEFINED_POSITION;
	m_params.xPosRatio = EffectsOptions2SetXPosRatio( m_params.position );
	m_params.yPosRatio = EffectsOptions2SetYPosRatio( m_params.position );

	// TRACE("CEffectsOptionsDlg::EffectsOptionsSetXPosRatio  nPosition=[%i], xRatio=[%i], yRatio=[%i]\n",  m_params.position, m_params.xPosRatio, m_params.yPosRatio );

	OnOK();
}


// Define Position X Ratio
// Scale  LEFT=0, CENTER= 50, RIGHT= 100
int CEffectsOptions2Dlg::EffectsOptions2SetXPosRatio( int nPosition ){

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
int CEffectsOptions2Dlg::EffectsOptions2SetYPosRatio( int nPosition )
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
