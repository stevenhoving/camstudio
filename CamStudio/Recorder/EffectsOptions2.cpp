// EffectsOptions2.cpp : implementation file
//

#include "stdafx.h"
#include "Recorder.h"
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
