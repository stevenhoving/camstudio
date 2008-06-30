// EditImage.cpp : implementation file
//

#include "stdafx.h"
#include "vscap.h"
#include "EditImage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern void ErrMsg(char format[], ...);
HCURSOR cursorCross_EI = NULL;
HCURSOR cursorArrow_EI = NULL;

/////////////////////////////////////////////////////////////////////////////
// CEditImage dialog


CEditImage::CEditImage(CWnd* pParent /*=NULL*/)
	: CDialog(CEditImage::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditImage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT


	if (cursorCross_EI == NULL)
		cursorCross_EI=::LoadCursor(NULL,IDC_CROSS);
	//cursorCross=::LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_CURSORDROPPER));
	if (cursorArrow_EI == NULL)
		cursorArrow_EI=::LoadCursor(NULL,IDC_ARROW);

	m_dialogInitialized = 0;
	pickingColor = 0;
}


void CEditImage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditImage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditImage, CDialog)
	//{{AFX_MSG_MAP(CEditImage)
	ON_BN_CLICKED(IDC_RADIO2, OnNoCutout)
	ON_BN_CLICKED(IDC_RADIO1, OnCutoutTrans)
	ON_BN_CLICKED(IDC_RADIO3, OnCutoutPredefined)
	ON_BN_CLICKED(IDC_CHECK1, OnAddBorder)
	ON_BN_CLICKED(IDC_COLOR2, OnBorderColor)
	ON_BN_CLICKED(IDC_COLOR, OnColor)
	ON_CBN_SELCHANGE(IDC_PREDEFINEDSHAPE, OnSelchangePredefinedshape)
	ON_EN_CHANGE(IDC_BORDERSIZE, OnChangeBordersize)
	ON_BN_CLICKED(IDC_COLOR3, OnColorPick)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDBKCOLOR, OnBkcolor)
	ON_BN_CLICKED(IDLOAD, OnLoad)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditImage message handlers

BOOL CEditImage::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	
	((CButton *) GetDlgItem(IDC_COLOR))->EnableWindow(1);
	((CButton *) GetDlgItem(IDC_COLOR3))->EnableWindow(1);
	((CStatic *) GetDlgItem(IDC_COLORSTATIC))->EnableWindow(1);
	((CStatic *) GetDlgItem(IDC_STATICCHOOSE))->EnableWindow(1);

	
	((CComboBox *) GetDlgItem(IDC_PREDEFINEDSHAPE))->EnableWindow(1);
	((CButton *) GetDlgItem(IDC_CHECK1))->EnableWindow(1);
	((CStatic *) GetDlgItem(IDC_STATIC1))->EnableWindow(1);
	((CStatic *) GetDlgItem(IDC_COLORSTATIC2))->EnableWindow(1);
	((CButton *) GetDlgItem(IDC_COLOR2))->EnableWindow(1);
	((CEdit *) GetDlgItem(IDC_BORDERSIZE))->EnableWindow(1);
	((CSpinButtonCtrl *) GetDlgItem(IDC_SPIN1))->EnableWindow(1);
	((CStatic *) GetDlgItem(IDC_BORDERSTATIC))->EnableWindow(1);

	
	

	((CSpinButtonCtrl *) GetDlgItem(IDC_SPIN1))->SetBuddy(GetDlgItem(IDC_BORDERSIZE));

	

	
	

	if (m_transWnd) 
	{

		/*
		//CDC *tempDC = ((CStatic *) GetDlgItem(IDC_COLORSTATIC))->GetDC();
		CDC *tempDC = GetWindowDC();
		CRect winRect(100,100,400,300);
		//((CStatic *) GetDlgItem(IDC_COLORSTATIC))->GetWindowRect(&winRect);
		//tempDC->FillSolidRect(&winRect,m_transWnd->m_transparentColor);
		tempDC->Rectangle(&winRect);
		ReleaseDC(tempDC);
		*/

		if (m_transWnd->m_hbitmap) 
			((CButton *) GetDlgItem(IDBKCOLOR))->EnableWindow(0);		
		else
			((CButton *) GetDlgItem(IDBKCOLOR))->EnableWindow(1);
	
		((CSpinButtonCtrl *) GetDlgItem(IDC_SPIN1))->SetRange(1,15);
		((CSpinButtonCtrl *) GetDlgItem(IDC_SPIN1))->SetPos(m_transWnd->m_borderSize);
		
	
		
		//((CEdit *) GetDlgItem(IDC_COLORSTATIC))->SetTextColor(m_transWnd->m_transparentColor);
		if ((m_transWnd->m_regionPredefinedShape>=0) && (m_transWnd->m_regionPredefinedShape<=2))
			((CComboBox *) GetDlgItem(IDC_PREDEFINEDSHAPE))->SetCurSel(m_transWnd->m_regionPredefinedShape);

		if (m_transWnd->m_borderYes) 		
			((CButton *) GetDlgItem(IDC_CHECK1))->SetCheck(1);
		else
			((CButton *) GetDlgItem(IDC_CHECK1))->SetCheck(0);
	

		//CString txt;
		//txt.Format("%d",m_transWnd->m_borderSize);
		//((CEdit *) GetDlgItem(IDC_BORDERSIZE))->SetWindowText(txt);

	}
	

	UpdateGUI();

	m_dialogInitialized = 1;

	if (m_transWnd)
	{
		if (m_transWnd->baseType == 1)
		{
			((CButton *) GetDlgItem(IDBKCOLOR))->EnableWindow(0);	
			((CButton *) GetDlgItem(IDLOAD))->EnableWindow(0);	

		}
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEditImage::PreModal(CTransparentWnd *transWnd)
{
	m_transWnd = transWnd;

}

void CEditImage::OnNoCutout() 
{
	// TODO: Add your control notification handler code here
	if (!m_transWnd) return;

	m_transWnd->m_regionType = regionNULL;
	UpdateGUI();

	m_transWnd->Invalidate();
	m_transWnd->InvalidateRegion();
	
}

void CEditImage::OnCutoutTrans() 
{
	// TODO: Add your control notification handler code here
	if (!m_transWnd) return;

	m_transWnd->m_regionType = regionTRANSPARENTCOLOR;
	UpdateGUI();

	m_transWnd->Invalidate();
	m_transWnd->InvalidateRegion();
	
}

void CEditImage::OnCutoutPredefined() 
{
	// TODO: Add your control notification handler code here
	if (!m_transWnd) return;

	m_transWnd->m_regionType = regionSHAPE;
	UpdateGUI();

	m_transWnd->Invalidate();
	m_transWnd->InvalidateRegion();
	
}

void CEditImage::UpdateGUI()
{

	if (!m_transWnd) return;

	if (m_transWnd->m_regionType == regionNULL) {
		
		((CButton *) GetDlgItem(IDC_RADIO2))->SetCheck(1);
		((CButton *) GetDlgItem(IDC_RADIO1))->SetCheck(0);
		((CButton *) GetDlgItem(IDC_RADIO3))->SetCheck(0);

		((CButton *) GetDlgItem(IDC_COLOR))->EnableWindow(0);
		((CButton *) GetDlgItem(IDC_COLOR3))->EnableWindow(0);
		((CStatic *) GetDlgItem(IDC_COLORSTATIC))->EnableWindow(0);
		((CStatic *) GetDlgItem(IDC_STATICCHOOSE))->EnableWindow(0);

		((CComboBox *) GetDlgItem(IDC_PREDEFINEDSHAPE))->EnableWindow(0);
		((CButton *) GetDlgItem(IDC_CHECK1))->EnableWindow(0);
		((CStatic *) GetDlgItem(IDC_STATIC1))->EnableWindow(0);
		((CStatic *) GetDlgItem(IDC_COLORSTATIC2))->EnableWindow(0);
		((CButton *) GetDlgItem(IDC_COLOR2))->EnableWindow(0);
		((CEdit *) GetDlgItem(IDC_BORDERSIZE))->EnableWindow(0);
		((CSpinButtonCtrl *) GetDlgItem(IDC_SPIN1))->EnableWindow(0);
		((CStatic *) GetDlgItem(IDC_BORDERSTATIC))->EnableWindow(0);
		

	}
	else if (m_transWnd->m_regionType == regionTRANSPARENTCOLOR) {

		((CButton *) GetDlgItem(IDC_RADIO1))->SetCheck(1);
		((CButton *) GetDlgItem(IDC_RADIO2))->SetCheck(0);
		((CButton *) GetDlgItem(IDC_RADIO3))->SetCheck(0);

		((CButton *) GetDlgItem(IDC_COLOR))->EnableWindow(1);
		((CButton *) GetDlgItem(IDC_COLOR3))->EnableWindow(1);
		((CStatic *) GetDlgItem(IDC_COLORSTATIC))->EnableWindow(1);
		((CStatic *) GetDlgItem(IDC_STATICCHOOSE))->EnableWindow(1);

		((CComboBox *) GetDlgItem(IDC_PREDEFINEDSHAPE))->EnableWindow(0);
		((CButton *) GetDlgItem(IDC_CHECK1))->EnableWindow(0);
		((CStatic *) GetDlgItem(IDC_STATIC1))->EnableWindow(0);
		((CStatic *) GetDlgItem(IDC_COLORSTATIC2))->EnableWindow(0);
		((CButton *) GetDlgItem(IDC_COLOR2))->EnableWindow(0);
		((CEdit *) GetDlgItem(IDC_BORDERSIZE))->EnableWindow(0);
		((CSpinButtonCtrl *) GetDlgItem(IDC_SPIN1))->EnableWindow(0);
		((CStatic *) GetDlgItem(IDC_BORDERSTATIC))->EnableWindow(0);
		
		
		

	}
	else if (m_transWnd->m_regionType == regionSHAPE) {

		((CButton *) GetDlgItem(IDC_RADIO3))->SetCheck(1);
		((CButton *) GetDlgItem(IDC_RADIO1))->SetCheck(0);
		((CButton *) GetDlgItem(IDC_RADIO2))->SetCheck(0);

		((CButton *) GetDlgItem(IDC_COLOR))->EnableWindow(0);
		((CButton *) GetDlgItem(IDC_COLOR3))->EnableWindow(0);
		((CStatic *) GetDlgItem(IDC_COLORSTATIC))->EnableWindow(0);
		((CStatic *) GetDlgItem(IDC_STATICCHOOSE))->EnableWindow(0);

		((CComboBox *) GetDlgItem(IDC_PREDEFINEDSHAPE))->EnableWindow(1);
		((CButton *) GetDlgItem(IDC_CHECK1))->EnableWindow(1);
		((CStatic *) GetDlgItem(IDC_STATIC1))->EnableWindow(1);
		((CStatic *) GetDlgItem(IDC_COLORSTATIC2))->EnableWindow(1);
		((CButton *) GetDlgItem(IDC_COLOR2))->EnableWindow(1);
		((CEdit *) GetDlgItem(IDC_BORDERSIZE))->EnableWindow(1);
		((CSpinButtonCtrl *) GetDlgItem(IDC_SPIN1))->EnableWindow(1);
		((CStatic *) GetDlgItem(IDC_BORDERSTATIC))->EnableWindow(1);

	}


	if (!(m_transWnd->m_hbitmap))  
	{
		
		((CButton *) GetDlgItem(IDC_RADIO1))->EnableWindow(0);

		((CButton *) GetDlgItem(IDC_COLOR))->EnableWindow(0);
		((CButton *) GetDlgItem(IDC_COLOR3))->EnableWindow(0);
		((CStatic *) GetDlgItem(IDC_COLORSTATIC))->EnableWindow(0);
		((CStatic *) GetDlgItem(IDC_STATICCHOOSE))->EnableWindow(0);

	}
	else
		((CButton *) GetDlgItem(IDC_RADIO1))->EnableWindow(1);


			
	
		 

//#define regionROUNDRECT 0
//#define regionELLIPSE 1
//#define regionRECTANGLE 2


}

void CEditImage::OnAddBorder() 
{
	// TODO: Add your control notification handler code here
	if (m_transWnd->m_borderYes) {
		m_transWnd->m_borderYes = 0;
		((CButton *) GetDlgItem(IDC_CHECK1))->SetCheck(0);
	}
	else {
	
		m_transWnd->m_borderYes = 1;
		((CButton *) GetDlgItem(IDC_CHECK1))->SetCheck(1);

	}
	
	m_transWnd->Invalidate();
	
}

void CEditImage::OnBorderColor() 
{
	// TODO: Add your control notification handler code here
	if (!m_transWnd) return;

	CColorDialog colordlg(m_transWnd->m_borderColor,CC_ANYCOLOR | CC_FULLOPEN |CC_RGBINIT,this);
	if (colordlg.DoModal()==IDOK)
	{
		m_transWnd->m_borderColor = colordlg.GetColor();
	}

	m_transWnd->Invalidate();

	
}

void CEditImage::OnColor() 
{
	// TODO: Add your control notification handler code here
		if (!m_transWnd) return;

	CColorDialog colordlg(m_transWnd->m_transparentColor,CC_ANYCOLOR | CC_FULLOPEN |CC_RGBINIT,this);
	if (colordlg.DoModal()==IDOK)
	{
		m_transWnd->m_transparentColor = colordlg.GetColor();
	}

	m_transWnd->m_regionCreated = 0;
	m_transWnd->InvalidateRegion();
	m_transWnd->Invalidate();
	

	
}

void CEditImage::OnSelchangePredefinedshape() 
{
	if (!m_transWnd) return;
	// TODO: Add your control notification handler code here	
	m_transWnd->m_regionPredefinedShape = ((CComboBox *) GetDlgItem(IDC_PREDEFINEDSHAPE))->GetCurSel();
	
	m_transWnd->Invalidate();
	m_transWnd->InvalidateRegion();
	
}


void CEditImage::OnChangeBordersize() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
	if (!m_transWnd) return;

	if (!m_dialogInitialized)
		return;

	m_transWnd->m_borderSize = ((CSpinButtonCtrl *) GetDlgItem(IDC_SPIN1))->GetPos();
	m_transWnd->Invalidate();
	
	
}

void CEditImage::OnColorPick() 
{
	// TODO: Add your control notification handler code here
	SetCapture();
	SetCursor(cursorCross_EI);
	pickingColor = 1;	

	((CButton *) GetDlgItem(IDC_COLOR3))->EnableWindow(0);
	
}

void CEditImage::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	if (pickingColor == 1) {

		HDC hDC = ::GetDC(NULL);

		CPoint pt;
		GetCursorPos(&pt);
		COLORREF val = GetPixel(hDC,pt.x,pt.y); 
		::ReleaseDC(NULL,hDC);
		
		ReleaseCapture();
		SetCursor(cursorArrow_EI);

		m_transWnd->m_transparentColor = val;

		m_transWnd->m_regionCreated = 0;
		m_transWnd->InvalidateRegion();
		m_transWnd->Invalidate();

		((CButton *) GetDlgItem(IDC_COLOR3))->EnableWindow(1);

		pickingColor = 0; 

	}
	
	CDialog::OnLButtonDown(nFlags, point);
}

void CEditImage::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (pickingColor == 1) 
		SetCursor(cursorCross_EI);
		
	CDialog::OnMouseMove(nFlags, point);
}

void CEditImage::OnBkcolor() 
{
	// TODO: Add your control notification handler code here
	if (!m_transWnd) return;

	CColorDialog colordlg(m_transWnd->m_backgroundColor,CC_ANYCOLOR | CC_FULLOPEN |CC_RGBINIT,this);
	if (colordlg.DoModal()==IDOK)
	{
		m_transWnd->m_backgroundColor = colordlg.GetColor();
	}

	m_transWnd->Invalidate();

	
}


void CEditImage::OnLoad() 
{
	// TODO: Add your control notification handler code here
	if (!m_transWnd) return;
	
	CString filename;
	
	static char BASED_CODE szFilter[] =	"Picture Files (*.bmp; *.jpg; *.gif)|*.bmp; *.jpg; *.gif||";
	static char szTitle[]="Load Picture";		
	
	CFileDialog fdlg(TRUE,"*.bmp; *.jpg; *.gif","*.bmp; *.jpg; *.gif",OFN_LONGNAMES | OFN_FILEMUSTEXIST ,szFilter,this);	
	fdlg.m_ofn.lpstrTitle=szTitle;		

	if(fdlg.DoModal() == IDOK)
	{
		filename = fdlg.GetPathName();
		m_transWnd->ReloadPic(filename);
		m_transWnd->Invalidate();		
		
	}
	
	
}
