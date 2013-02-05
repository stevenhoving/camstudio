// EditImage.cpp : implementation file
// TODO: additional refactoring/simplifaction of enablewindow code
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Recorder.h"
#include "EditImage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditImageDlg dialog

CEditImageDlg::CEditImageDlg(CWnd* pParent /*=NULL*/)
: CDialog(CEditImageDlg::IDD, pParent)
, m_hCursorCross(::LoadCursor(NULL,IDC_CROSS))
, m_hCursorArrow(::LoadCursor(NULL,IDC_ARROW))
{
	//{{AFX_DATA_INIT(CEditImageDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_dialogInitialized = 0;
	pickingColor = 0;
}

void CEditImageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditImageDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP

	DDX_Control(pDX, IDC_RADIO1, m_ctrlButtonTransparent);
	DDX_Control(pDX, IDC_RADIO2, m_ctrlButtonNoBKColor);
	DDX_Control(pDX, IDC_RADIO3, m_ctrlButtonPreDefined);
	DDX_Control(pDX, IDC_BKCOLOR, m_ctrlButtonBKColor);
	DDX_Control(pDX, IDC_COLOR, m_ctrlButtonColor);
	DDX_Control(pDX, IDC_PICK_SCREEN_COLOR, m_ctrlButtonPickScreenColor);
	DDX_Control(pDX, IDC_COLORSTATIC, m_ctrlStaticColor);
	DDX_Control(pDX, IDC_STATICCHOOSE, m_ctrlStaticChoose);
	DDX_Control(pDX, IDC_PREDEFINEDSHAPE, m_ctrlCBPredefinedShape);
	DDX_Control(pDX, IDC_CHECK1, m_ctrlButtonAddBorder);
	DDX_Control(pDX, IDC_STATIC1, m_ctrlStaticBorderSize);
	DDX_Control(pDX, IDC_COLORSTATIC2, m_ctrlStaticColor2);
	DDX_Control(pDX, IDC_COLOR2, m_ctrlButtonBorderColor);
	DDX_Control(pDX, IDC_BORDERSIZE, m_ctrlEditBorderSize);
	DDX_Control(pDX, IDC_SPIN1, m_ctrlSpinBorderSize);
	DDX_Control(pDX, IDC_BORDERSTATIC, m_ctrlStaticBorderGroup);
	DDX_Control(pDX, IDC_LOAD, m_ctrlButtonLoadNewImage);
}

BEGIN_MESSAGE_MAP(CEditImageDlg, CDialog)
	//{{AFX_MSG_MAP(CEditImageDlg)
	ON_BN_CLICKED(IDC_RADIO2, OnNoCutout)
	ON_BN_CLICKED(IDC_RADIO1, OnCutoutTrans)
	ON_BN_CLICKED(IDC_RADIO3, OnCutoutPredefined)
	ON_BN_CLICKED(IDC_CHECK1, OnAddBorder)
	ON_BN_CLICKED(IDC_COLOR2, OnBorderColor)
	ON_BN_CLICKED(IDC_COLOR, OnColor)
	ON_CBN_SELCHANGE(IDC_PREDEFINEDSHAPE, OnSelchangePredefinedshape)
	ON_EN_CHANGE(IDC_BORDERSIZE, OnChangeBordersize)
	ON_BN_CLICKED(IDC_PICK_SCREEN_COLOR, OnColorPick)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BKCOLOR, OnBkcolor)
	ON_BN_CLICKED(IDC_LOAD, OnLoad)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditImageDlg message handlers

BOOL CEditImageDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here

	m_ctrlButtonColor.EnableWindow(TRUE);
	m_ctrlButtonPickScreenColor.EnableWindow(TRUE);
	m_ctrlStaticColor.EnableWindow(TRUE);
	m_ctrlStaticChoose.EnableWindow(TRUE);

	m_ctrlCBPredefinedShape.EnableWindow(TRUE);
	m_ctrlButtonAddBorder.EnableWindow(TRUE);
	m_ctrlStaticBorderSize.EnableWindow(TRUE);
	m_ctrlStaticColor2.EnableWindow(TRUE);
	m_ctrlButtonBorderColor.EnableWindow(TRUE);
	m_ctrlEditBorderSize.EnableWindow(TRUE);
	m_ctrlSpinBorderSize.EnableWindow(TRUE);
	m_ctrlStaticBorderGroup.EnableWindow(TRUE);

	m_ctrlSpinBorderSize.SetBuddy(&m_ctrlEditBorderSize);

	if (m_transWnd) {
		////CDC *tempDC = m_ctrlStaticColor.GetDC();
		//CDC *tempDC = GetWindowDC();
		//CRect winRect(100,100,400,300);
		////m_ctrlStaticColor.GetWindowRect(&winRect);
		////tempDC->FillSolidRect(&winRect,m_transWnd->m_transparentColor);
		//tempDC->Rectangle(&winRect);
		//ReleaseDC(tempDC);

		m_ctrlButtonBKColor.EnableWindow(0 != m_transWnd->BitMap());
		m_ctrlSpinBorderSize.SetRange(1,15);
		m_ctrlSpinBorderSize.SetPos(m_transWnd->BorderSize());

		//m_ctrlStaticColor.SetTextColor(m_transWnd->m_transparentColor);
		if ((0 <= m_transWnd->RegionPredefinedShape()) && (m_transWnd->RegionPredefinedShape() <= 2)) {
			m_ctrlCBPredefinedShape.SetCurSel(m_transWnd->RegionPredefinedShape());
		}

		m_ctrlButtonAddBorder.SetCheck(m_transWnd->BorderYes());
	}

	UpdateGUI();

	m_dialogInitialized = 1;

	if (m_transWnd) {
		if (m_transWnd->BaseType() == 1) {
			m_ctrlButtonBKColor.EnableWindow(FALSE);
			m_ctrlButtonLoadNewImage.EnableWindow(FALSE);
		}
	}

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CEditImageDlg::PreModal(CTransparentWnd *transWnd)
{
	m_transWnd = transWnd;
}

void CEditImageDlg::OnNoCutout()
{
	// TODO: Add your control notification handler code here
	if (!m_transWnd)
	{
		return;
	}

	m_transWnd->RegionType(regionNULL);
	UpdateGUI();

	m_transWnd->Invalidate();
	m_transWnd->InvalidateRegion();
}

void CEditImageDlg::OnCutoutTrans()
{
	// TODO: Add your control notification handler code here
	if (!m_transWnd)
	{
		return;
	}

	m_transWnd->RegionType(regionTRANSPARENTCOLOR);
	UpdateGUI();

	m_transWnd->Invalidate();
	m_transWnd->InvalidateRegion();
}

void CEditImageDlg::OnCutoutPredefined()
{
	// TODO: Add your control notification handler code here
	if (!m_transWnd)
	{
		return;
	}

	m_transWnd->RegionType(regionSHAPE);
	UpdateGUI();

	m_transWnd->Invalidate();
	m_transWnd->InvalidateRegion();
}

void CEditImageDlg::UpdateGUI()
{
	if (!m_transWnd) {
		return;
	}

	if (m_transWnd->RegionType() == regionNULL) {
		m_ctrlButtonNoBKColor.SetCheck(TRUE);
		m_ctrlButtonTransparent.SetCheck(FALSE);
		m_ctrlButtonPreDefined.SetCheck(FALSE);

		m_ctrlButtonColor.EnableWindow(FALSE);
		m_ctrlButtonPickScreenColor.EnableWindow(FALSE);
		m_ctrlStaticColor.EnableWindow(FALSE);
		m_ctrlStaticChoose.EnableWindow(FALSE);

		m_ctrlCBPredefinedShape.EnableWindow(FALSE);
		m_ctrlButtonAddBorder.EnableWindow(FALSE);
		m_ctrlStaticBorderSize.EnableWindow(FALSE);
		m_ctrlStaticColor2.EnableWindow(FALSE);
		m_ctrlButtonBorderColor.EnableWindow(FALSE);
		m_ctrlEditBorderSize.EnableWindow(FALSE);
		m_ctrlSpinBorderSize.EnableWindow(FALSE);
		m_ctrlStaticBorderGroup.EnableWindow(FALSE);
	} else if (m_transWnd->RegionType() == regionTRANSPARENTCOLOR) {
		m_ctrlButtonTransparent.SetCheck(TRUE);
		m_ctrlButtonNoBKColor.SetCheck(FALSE);
		m_ctrlButtonPreDefined.SetCheck(FALSE);

		m_ctrlButtonColor.EnableWindow(TRUE);
		m_ctrlButtonPickScreenColor.EnableWindow(TRUE);
		m_ctrlStaticColor.EnableWindow(TRUE);
		m_ctrlStaticChoose.EnableWindow(TRUE);

		m_ctrlCBPredefinedShape.EnableWindow(FALSE);
		m_ctrlButtonAddBorder.EnableWindow(FALSE);
		m_ctrlStaticBorderSize.EnableWindow(FALSE);
		m_ctrlStaticColor2.EnableWindow(FALSE);
		m_ctrlButtonBorderColor.EnableWindow(FALSE);
		m_ctrlEditBorderSize.EnableWindow(FALSE);
		m_ctrlSpinBorderSize.EnableWindow(FALSE);
		m_ctrlStaticBorderGroup.EnableWindow(FALSE);
	} else if (m_transWnd->RegionType() == regionSHAPE) {
		m_ctrlButtonPreDefined.SetCheck(TRUE);
		m_ctrlButtonTransparent.SetCheck(FALSE);
		m_ctrlButtonNoBKColor.SetCheck(FALSE);

		m_ctrlButtonColor.EnableWindow(FALSE);
		m_ctrlButtonPickScreenColor.EnableWindow(FALSE);
		m_ctrlStaticColor.EnableWindow(FALSE);
		m_ctrlStaticChoose.EnableWindow(FALSE);

		m_ctrlCBPredefinedShape.EnableWindow(TRUE);
		m_ctrlButtonAddBorder.EnableWindow(TRUE);
		m_ctrlStaticBorderSize.EnableWindow(TRUE);
		m_ctrlStaticColor2.EnableWindow(TRUE);
		m_ctrlButtonBorderColor.EnableWindow(TRUE);
		m_ctrlEditBorderSize.EnableWindow(TRUE);
		m_ctrlSpinBorderSize.EnableWindow(TRUE);
		m_ctrlStaticBorderGroup.EnableWindow(TRUE);
	}

	if (!(m_transWnd->BitMap())) {
		m_ctrlButtonTransparent.EnableWindow(FALSE);
		m_ctrlButtonColor.EnableWindow(FALSE);
		m_ctrlButtonPickScreenColor.EnableWindow(FALSE);
		m_ctrlStaticColor.EnableWindow(FALSE);
		m_ctrlStaticChoose.EnableWindow(FALSE);
	} else {
		m_ctrlButtonTransparent.EnableWindow(TRUE);
	}
}

void CEditImageDlg::OnAddBorder()
{
	m_transWnd->BorderYes(!m_transWnd->BorderYes());
	m_ctrlButtonAddBorder.SetCheck(m_transWnd->BorderYes());
	m_transWnd->Invalidate();
}

void CEditImageDlg::OnBorderColor()
{
	// TODO: Add your control notification handler code here
	if (!m_transWnd) {
		return;
	}

	CColorDialog colordlg(m_transWnd->BorderColor(), CC_ANYCOLOR | CC_FULLOPEN |CC_RGBINIT,this);
	if (IDOK == colordlg.DoModal()) {
		m_transWnd->BorderColor(colordlg.GetColor());
	}

	m_transWnd->Invalidate();
}

void CEditImageDlg::OnColor()
{
	// TODO: Add your control notification handler code here
	if (!m_transWnd) {
		return;
	}

	CColorDialog colordlg(m_transWnd->TransparentColor(), CC_ANYCOLOR | CC_FULLOPEN |CC_RGBINIT,this);
	if (IDOK == colordlg.DoModal()) {
		m_transWnd->TransparentColor(colordlg.GetColor());
	}

	m_transWnd->RegionCreated(0);
	m_transWnd->InvalidateRegion();
	m_transWnd->Invalidate();
}

void CEditImageDlg::OnSelchangePredefinedshape()
{
	if (!m_transWnd)
	{
		return;
	}
	// TODO: Add your control notification handler code here
	m_transWnd->RegionPredefinedShape(m_ctrlCBPredefinedShape.GetCurSel());

	m_transWnd->Invalidate();
	m_transWnd->InvalidateRegion();
}

void CEditImageDlg::OnChangeBordersize()
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO: Add your control notification handler code here

	if (!m_transWnd)
	{
		return;
	}

	if (!m_dialogInitialized)
	{
		return;
	}

	m_transWnd->BorderSize(m_ctrlSpinBorderSize.GetPos());
	m_transWnd->Invalidate();
}

void CEditImageDlg::OnColorPick()
{
	// TODO: Add your control notification handler code here
	SetCapture();
	SetCursor(m_hCursorCross);
	pickingColor = 1;

	m_ctrlButtonPickScreenColor.EnableWindow(FALSE);
}

void CEditImageDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if (pickingColor == 1)
	{
		HDC hDC = ::GetDC(NULL);
		CPoint pt;
		GetCursorPos(&pt);
		COLORREF val = GetPixel(hDC,pt.x,pt.y);
		::ReleaseDC(NULL,hDC);

		ReleaseCapture();
		SetCursor(m_hCursorArrow);

		m_transWnd->TransparentColor(val);

		m_transWnd->RegionCreated(0);
		m_transWnd->InvalidateRegion();
		m_transWnd->Invalidate();

		m_ctrlButtonPickScreenColor.EnableWindow(TRUE);

		pickingColor = 0;
	}

	CDialog::OnLButtonDown(nFlags, point);
}

void CEditImageDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (pickingColor == 1)
	{
		SetCursor(m_hCursorCross);
	}

	CDialog::OnMouseMove(nFlags, point);
}

void CEditImageDlg::OnBkcolor()
{
	// TODO: Add your control notification handler code here
	if (!m_transWnd) {
		return;
	}

	CColorDialog colordlg(m_transWnd->BackgroundColor(), CC_ANYCOLOR | CC_FULLOPEN |CC_RGBINIT,this);
	if (IDOK == colordlg.DoModal()) {
		m_transWnd->BackgroundColor(colordlg.GetColor());
	}

	m_transWnd->Invalidate();
}

void CEditImageDlg::OnLoad()
{
	// TODO: Add your control notification handler code here
	if (!m_transWnd)
	{
		return;
	}

	CString filename;

	static char BASED_CODE szFilter[] = "Picture Files (*.bmp; *.jpg; *.gif)|*.bmp; *.jpg; *.gif||";
	static char szTitle[]="Load Picture";

	CFileDialog fdlg(TRUE,"*.bmp; *.jpg; *.gif","*.bmp; *.jpg; *.gif",OFN_LONGNAMES | OFN_FILEMUSTEXIST ,szFilter,this);
	fdlg.m_ofn.lpstrTitle = szTitle;

	if (IDOK == fdlg.DoModal())
	{
		filename = fdlg.GetPathName();
		m_transWnd->ReloadPic(filename);
		m_transWnd->Invalidate();
	}
}
