// RenderSoft CamStudio
//
// Copyright 2001 RenderSoft Software & Web Publishing
//
//
// CursorOptionsDlg.cpp : implementation file
//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Recorder.h"
#include "CursorOptionsDlg.h"
#include "CamCursor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCursorOptionsDlg dialog

CCursorOptionsDlg::CCursorOptionsDlg(CWnd* pParent /*=NULL*/)
: CDialog(CCursorOptionsDlg::IDD, pParent)
, m_pIconFileDlg(0)
, m_hPreviewCursor(NULL)
, m_hLoadCursor(CamCursor.Load())
, m_hCustomCursor(CamCursor.Custom())
, m_iCustomSel(iCustomSel)
, m_bRecordCursor(bRecordCursor)
, m_iCursorType(CamCursor.Select())
, m_bHighlightCursor(bHighlightCursor)
, m_iHighlightSize(iHighlightSize)
, m_iHighlightShape(iHighlightShape)
, m_bHighlightClick(bHighlightClick)
, m_clrHighlight(clrHighlightColor)
, m_clrHighlightClickLeft(clrHighlightClickColorLeft)
, m_clrHighlightClickRight(clrHighlightClickColorRight)
, m_bInitPaint(true)
, m_bSliding(false)
{
	//{{AFX_DATA_INIT(CCursorOptionsDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	
	CamCursor.AddID(IDI_CUSTOMICON_CONTEXTHELP);
	CamCursor.AddID(IDI_CUSTOMICON_MAGNIFY);
	CamCursor.AddID(IDI_CUSTOMICON_NODRAG);
	CamCursor.AddID(IDI_CUSTOMICON_SPLITBARH);
	CamCursor.AddID(IDI_CUSTOMICON_SPLITBARV);
	CamCursor.AddID(IDI_CUSTOMICON_TOPOFTABLE);
	CamCursor.AddID(IDI_CUSTOMICON_BOOK1);
	CamCursor.AddID(IDI_CUSTOMICON_BOOK2);
	CamCursor.AddID(IDI_CUSTOMICON_CLIP1);
	CamCursor.AddID(IDI_CUSTOMICON_CLIP2);
	CamCursor.AddID(IDI_CUSTOMICON_CLOCK1);
	CamCursor.AddID(IDI_CUSTOMICON_CLOCK2);
	CamCursor.AddID(IDI_CUSTOMICON_CARDFILE1);
	CamCursor.AddID(IDI_CUSTOMICON_CARDFILE2);
	CamCursor.AddID(IDI_CUSTOMICON_DISK1);
	CamCursor.AddID(IDI_CUSTOMICON_DISK2);
	CamCursor.AddID(IDI_CUSTOMICON_FILES1);
	CamCursor.AddID(IDI_CUSTOMICON_FILES2);
	CamCursor.AddID(IDI_CUSTOMICON_FOLDER2);
	CamCursor.AddID(IDI_CUSTOMICON_FOLDER1);
	CamCursor.AddID(IDI_CUSTOMICON_MAIL1);
	CamCursor.AddID(IDI_CUSTOMICON_MAIL2);
	CamCursor.AddID(IDI_CUSTOMICON_NOTE1);
	CamCursor.AddID(IDI_CUSTOMICON_NOTE2);
	CamCursor.AddID(IDI_CUSTOMICON_PEN1);
	CamCursor.AddID(IDI_CUSTOMICON_PEN2);
	CamCursor.AddID(IDI_CUSTOMICON_PENCIL1);
	CamCursor.AddID(IDI_CUSTOMICON_PENCIL2);
	CamCursor.AddID(IDI_CUSTOMICON_PHONE1);
	CamCursor.AddID(IDI_CUSTOMICON_PHONE2);
	CamCursor.AddID(IDI_CUSTOMICON_POINT1);
	CamCursor.AddID(IDI_CUSTOMICON_POINT2);
	CamCursor.AddID(IDI_CUSTOMICON_SERCURITY);
	CamCursor.AddID(IDI_CUSTOMICON_SECURITY2);
}

void CCursorOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCursorOptionsDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_RADIO1, m_ctrlButtonHideCursor);
	DDX_Control(pDX, IDC_HIGHLIGHTSIZE, m_ctrlSliderHighlightSize);
	DDX_Control(pDX, IDC_FILECURSOR, m_ctrlButtonfileCursor);
	DDX_Control(pDX, IDC_HIGHLIGHTSHAPE, m_ctrlCBHightlightShape);
	DDX_Control(pDX, IDC_CURSOR1, m_ctrlButtonActualCursor);
	DDX_Control(pDX, IDC_CURSOR2, m_ctrlButtonCustonCursor);
	DDX_Control(pDX, IDC_CURSOR3, m_ctrlButtonFileCursor);
	DDX_Control(pDX, IDC_STATIC_SIZE, m_ctrlStaticSize);
	DDX_Control(pDX, IDC_STATIC_SHAPE, m_ctrlStaticShape);
	DDX_Control(pDX, IDC_STATIC_HALFSIZE, m_ctrlStaticHalfSize);
	DDX_Control(pDX, IDC_HIGHLIGHTCURSOR, m_ctrlButtonHighlightCursor);
	DDX_Control(pDX, IDC_HIGHLIGHTCLICK, m_ctrlButtonHighlightClick);
	DDX_Control(pDX, IDC_HIGHLIGHTCOLORLB, m_ctrlButtonHighlightLB);
	DDX_Control(pDX, IDC_HIGHLIGHTCOLORRB, m_ctrlButtonHighlightRB);
	DDX_Control(pDX, IDC_HIGHLIGHTCOLOR, m_ctrlButtonHightlightColor);
	DDX_Control(pDX, IDC_CUSTOMCURSOR, m_ctrlCBCustomCursor);
	DDX_Control(pDX, IDC_RADIO2, m_ctrlButtonShowCursor);
	DDX_Control(pDX, IDC_ICONCURSOR, m_ctrlStaticIconCursor);
}

BEGIN_MESSAGE_MAP(CCursorOptionsDlg, CDialog)
	//{{AFX_MSG_MAP(CCursorOptionsDlg)
	ON_BN_CLICKED(IDC_RADIO1, OnHideCursor)
	ON_BN_CLICKED(IDC_RADIO2, OnShowCursor)
	ON_BN_CLICKED(IDC_CURSOR1, OnActualCursor)
	ON_BN_CLICKED(IDC_CURSOR2, OnCustomCursor)
	ON_BN_CLICKED(IDC_CURSOR3, OnFileCursor)
	ON_BN_CLICKED(IDC_FILECURSOR, OnFilecursor)
	ON_CBN_SELCHANGE(IDC_CUSTOMCURSOR, OnSelchangeCustomcursor)
	ON_CBN_SELCHANGE(IDC_HIGHLIGHTSHAPE, OnSelchangeHighlightshape)
	ON_BN_CLICKED(IDC_HIGHLIGHTCOLOR, OnHighlightcolor)
	ON_BN_CLICKED(IDC_HIGHLIGHTCURSOR, OnHighlightcursor)
	ON_WM_HSCROLL()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_HIGHLIGHTCOLORLB, OnHighlightColorLeft)
	ON_BN_CLICKED(IDC_HIGHLIGHTCOLORRB, OnHighlightColorRight)
	ON_BN_CLICKED(IDC_HIGHLIGHTCLICK, OnEnableVisualClickFeedback)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CCursorOptionsDlg::RefreshHighlight()
{
	CSize previewsize;
	previewsize.cx = 64;
	previewsize.cy = 64;
	int xoffset = 300;
	int yoffset = 230;

	int highlightsizehalf = m_iHighlightSize/2;

	COLORREF usecolor = m_clrHighlight;

	if (m_bHighlightClick == 1 && m_bSliding==FALSE)
	{
		SHORT tmpShort = GetKeyState(VK_LBUTTON);
		if (tmpShort != 0 && tmpShort != 1)
		{
			usecolor = m_clrHighlightClickLeft;
			highlightsizehalf *= 1.5;
		}
		tmpShort = GetKeyState(VK_RBUTTON);
		if (tmpShort != 0 && tmpShort != 1)
		{
			usecolor = m_clrHighlightClickRight;
			highlightsizehalf *= 1.5;
		}
	}

	// TODO: redo the math to use integers
	double x1 = 0.0;
	double x2 = 0.0;
	double y1 = 0.0;
	double y2 = 0.0;

	if ((m_iHighlightShape == 0) || (m_iHighlightShape == 2))
	{
		//circle and square
		x1 = (previewsize.cx - highlightsizehalf)/2.0;
		x2 = (previewsize.cx + highlightsizehalf)/2.0;
		y1 = (previewsize.cy - highlightsizehalf)/2.0;
		y2 = (previewsize.cy + highlightsizehalf)/2.0;
	}
	else if ((m_iHighlightShape == 1) || (m_iHighlightShape == 3))
	{
		//ellipse and rectangle
		x1 = (previewsize.cx - highlightsizehalf)/2.0;
		x2 = (previewsize.cx + highlightsizehalf)/2.0;
		y1 = (previewsize.cy - highlightsizehalf/2.0)/2.0;
		y2 = (previewsize.cy + highlightsizehalf/2.0)/2.0;
	}

	//OffScreen Buffer
	// TODO: this should all be done with a CDC
	CDC* cdc = GetWindowDC();
	HDC hdc = cdc->m_hDC;

	HBITMAP hbm = NULL;
	HBITMAP old_bitmap;
	HDC hdcBits = ::CreateCompatibleDC(hdc);
	hbm = (HBITMAP) ::CreateCompatibleBitmap(hdc,previewsize.cx,previewsize.cy);
	old_bitmap = (HBITMAP) ::SelectObject(hdcBits,hbm);

	HBRUSH ptbrush = (HBRUSH) ::GetStockObject(LTGRAY_BRUSH);
	HPEN nullpen = CreatePen( PS_NULL,0,0);
	HBRUSH hlbrush = CreateSolidBrush(usecolor);

	HBRUSH oldbrush = (HBRUSH) ::SelectObject(hdcBits,ptbrush);
	HPEN oldpen = (HPEN) ::SelectObject(hdcBits,nullpen);
	::Rectangle(hdcBits, 0, 0, previewsize.cx + 1, previewsize.cy + 1);

	if (m_bHighlightCursor)
	{
		//draw the shape only if highlight cursor is selected
		::SelectObject(hdcBits, hlbrush);

		if ((m_iHighlightShape == 0) || (m_iHighlightShape == 1))
		{
			//circle and ellipse
			::Ellipse(hdcBits,(int) x1,(int) y1,(int) x2,(int) y2);
		}
		else if ((m_iHighlightShape == 2) || (m_iHighlightShape == 3))
		{ //square and rectangle
			::Rectangle(hdcBits,(int) x1,(int) y1,(int) x2,(int) y2);
		}

		::SelectObject(hdcBits,oldbrush);
	}

	::SelectObject(hdcBits,oldpen);
	DeleteObject(hlbrush);
	DeleteObject(nullpen);

	//OffScreen Buffer
	BitBlt(hdc, xoffset, yoffset, previewsize.cx, previewsize.cy, hdcBits, 0, 0, SRCCOPY);
	SelectObject(hdcBits, old_bitmap);
	DeleteObject(hbm);
	DeleteDC(hdcBits);

	ReleaseDC(cdc);
}

void CCursorOptionsDlg::RefreshPreviewCursor()
{
	m_hPreviewCursor = NULL;
	if (m_bRecordCursor)
	{
		switch(m_iCursorType)
		{
		case 0:
			m_hPreviewCursor = GetCursor();
			break;
		case 1:
			{
				m_iCustomSel = m_ctrlCBCustomCursor.GetCurSel();
				DWORD customicon = (m_iCustomSel < 0) ? 0 : CamCursor.GetID(m_iCustomSel);
				m_hPreviewCursor = ::LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(customicon));
				m_hCustomCursor = m_hPreviewCursor;
			}
			break;
		case 2:
		default:
			m_hPreviewCursor = m_hLoadCursor;
			break;
		}
	}

	m_ctrlStaticIconCursor.SetIcon(m_hPreviewCursor);
}

/////////////////////////////////////////////////////////////////////////////
// CCursorOptionsDlg message handlers

BOOL CCursorOptionsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	HICON loadFileIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON1));
	m_ctrlButtonfileCursor.SetIcon(loadFileIcon);

	//Highlight UI
	m_ctrlSliderHighlightSize.EnableWindow(TRUE);
	m_ctrlCBHightlightShape.EnableWindow(TRUE);

	m_ctrlStaticSize.EnableWindow(TRUE);
	m_ctrlStaticShape.EnableWindow(TRUE);
	m_ctrlStaticHalfSize.EnableWindow(TRUE);

	m_ctrlSliderHighlightSize.SetRange(1, 128, TRUE);
	m_ctrlSliderHighlightSize.SetPos(m_iHighlightSize);
	m_ctrlCBHightlightShape.SetCurSel(m_iHighlightShape);

	m_ctrlButtonHighlightCursor.SetCheck(m_bHighlightCursor);
	m_ctrlButtonHighlightClick.SetCheck(m_bHighlightClick);

	m_ctrlButtonHideCursor.SetCheck(!m_bRecordCursor);
	m_ctrlButtonShowCursor.SetCheck(m_bRecordCursor);
	m_ctrlButtonActualCursor.SetCheck(m_iCursorType == 0);
	m_ctrlButtonCustonCursor.SetCheck(m_iCursorType == 1);
	m_ctrlButtonFileCursor.SetCheck(m_iCursorType == 2);
	m_ctrlCBCustomCursor.SetCurSel(m_iCustomSel);

	m_ctrlSliderHighlightSize.EnableWindow(m_bHighlightCursor);
	m_ctrlCBHightlightShape.EnableWindow(m_bHighlightCursor);
	m_ctrlButtonHightlightColor.EnableWindow(m_bHighlightCursor);
	m_ctrlStaticSize.EnableWindow(m_bHighlightCursor);
	m_ctrlStaticShape.EnableWindow(m_bHighlightCursor);
	m_ctrlStaticHalfSize.EnableWindow(m_bHighlightCursor);
	m_ctrlButtonHighlightClick.EnableWindow(m_bHighlightCursor);
	m_ctrlButtonHighlightLB.EnableWindow(m_bHighlightClick);
	m_ctrlButtonHighlightRB.EnableWindow(m_bHighlightClick);

	//Highlight UI
	m_ctrlButtonActualCursor.EnableWindow(m_bRecordCursor);
	m_ctrlButtonCustonCursor.EnableWindow(m_bRecordCursor);
	m_ctrlButtonFileCursor.EnableWindow(m_bRecordCursor);
	m_ctrlCBCustomCursor.EnableWindow(m_iCursorType == 1);
	m_ctrlButtonfileCursor.EnableWindow(m_iCursorType == 2);

	RefreshPreviewCursor();

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CCursorOptionsDlg::OnHideCursor()
{
	// TODO: Add your control notification handler code here

	m_bRecordCursor = false;
	m_ctrlButtonHideCursor.SetCheck(TRUE);
	m_ctrlButtonShowCursor.SetCheck(FALSE);

	m_ctrlButtonActualCursor.EnableWindow(FALSE);
	m_ctrlButtonCustonCursor.EnableWindow(FALSE);
	m_ctrlButtonFileCursor.EnableWindow(FALSE);
	m_ctrlCBCustomCursor.EnableWindow(FALSE);
	m_ctrlButtonfileCursor.EnableWindow(FALSE);

	RefreshPreviewCursor();
}

void CCursorOptionsDlg::OnShowCursor()
{
	m_bRecordCursor = true;
	m_ctrlButtonHideCursor.SetCheck(!m_bRecordCursor);
	m_ctrlButtonShowCursor.SetCheck(m_bRecordCursor);

	m_ctrlButtonActualCursor.EnableWindow(m_bRecordCursor);
	m_ctrlButtonCustonCursor.EnableWindow(m_bRecordCursor);
	m_ctrlButtonFileCursor.EnableWindow(m_bRecordCursor);

	m_ctrlCBCustomCursor.EnableWindow(m_iCursorType == 1);
	m_ctrlButtonfileCursor.EnableWindow(m_iCursorType == 2);

	RefreshPreviewCursor();
}

void CCursorOptionsDlg::OnActualCursor()
{
	// TODO: Add your control notification handler code here
	m_iCursorType = 0;
	m_ctrlButtonActualCursor.SetCheck(TRUE);
	m_ctrlButtonCustonCursor.SetCheck(FALSE);
	m_ctrlButtonFileCursor.SetCheck(FALSE);

	m_ctrlCBCustomCursor.EnableWindow(FALSE);
	m_ctrlButtonfileCursor.EnableWindow(FALSE);

	RefreshPreviewCursor();
}

void CCursorOptionsDlg::OnCustomCursor()
{
	// TODO: Add your control notification handler code here
	m_iCursorType = 1;

	m_ctrlButtonActualCursor.SetCheck(FALSE);
	m_ctrlButtonCustonCursor.SetCheck(TRUE);
	m_ctrlButtonFileCursor.SetCheck(FALSE);

	m_ctrlCBCustomCursor.EnableWindow(TRUE);
	m_ctrlButtonfileCursor.EnableWindow(FALSE);

	RefreshPreviewCursor();
}

void CCursorOptionsDlg::OnFileCursor()
{
	// TODO: Add your control notification handler code here
	m_iCursorType = 2;

	m_ctrlButtonActualCursor.SetCheck(FALSE);
	m_ctrlButtonCustonCursor.SetCheck(FALSE);
	m_ctrlButtonFileCursor.SetCheck(TRUE);

	m_ctrlCBCustomCursor.EnableWindow(FALSE);
	m_ctrlButtonfileCursor.EnableWindow(TRUE);

	RefreshPreviewCursor();
}

void CCursorOptionsDlg::OnSelchangeCustomcursor()
{
	// TODO: Add your control notification handler code here
	RefreshPreviewCursor();
}

void CCursorOptionsDlg::OnFilecursor()
{
	// TODO: Add your command handler code here
	CString fileName;
	CString filt="Icon and Cursor Files (*.ico; *.cur)|*.ico;*.cur||";

	if (m_pIconFileDlg == NULL)
	{
		m_pIconFileDlg = new CFileDialog(TRUE,"*.ico;*.cur","*.ico;*.cur",NULL,filt,this);
		if (!m_pIconFileDlg)
		{
			OnError(_T("CCursorOptionsDlg::OnFilecursor"));
			return;
		}
	}
	char dirx[200];
	VERIFY(GetWindowsDirectory(dirx, 200));
	CString initdir(dirx);
	initdir = initdir + "\\cursors";

	m_pIconFileDlg->m_ofn.Flags|=OFN_FILEMUSTEXIST;
	m_pIconFileDlg->m_ofn.lpstrTitle="File to load";

	if (CamCursor.Dir().IsEmpty())
	{
		CamCursor.Dir(initdir);
	}
	m_pIconFileDlg->m_ofn.lpstrInitialDir = CamCursor.Dir();
	if (IDOK == m_pIconFileDlg->DoModal())
	{
		fileName = m_pIconFileDlg->GetPathName();
		CString extension = fileName.Right(4);
		extension.MakeUpper();
		if ((extension == ".ICO") || (extension == ".CUR"))
		{
			m_hLoadCursor = LoadCursorFromFile(fileName);
			m_hPreviewCursor = m_hLoadCursor;
			m_ctrlStaticIconCursor.SetIcon(m_hPreviewCursor);
		}

		CamCursor.FileName(fileName);
		fileName = fileName.Left(fileName.ReverseFind('\\'));
		CamCursor.Dir(fileName);
	}
	delete m_pIconFileDlg, m_pIconFileDlg = 0;
}

void CCursorOptionsDlg::OnOK()
{
	//if (m_iCursorType == 0)
	//{
	//}
	//else 
	if (m_iCursorType == 1)
	{
		m_hCustomCursor = m_hPreviewCursor;
	}
	else
	{
		m_hLoadCursor = m_hPreviewCursor;

	} //cursortype

	iCustomSel = m_iCustomSel;
	bRecordCursor = m_bRecordCursor;
	CamCursor.Select(m_iCursorType);
	bHighlightCursor = m_bHighlightCursor;
	iHighlightSize = m_iHighlightSize;
	iHighlightShape = m_iHighlightShape;
	clrHighlightColor = m_clrHighlight;
	CamCursor.Load(m_hLoadCursor);
	CamCursor.Custom(m_hCustomCursor);
	bHighlightClick = m_bHighlightClick;
	clrHighlightClickColorLeft = m_clrHighlightClickLeft;
	clrHighlightClickColorRight = m_clrHighlightClickRight;

	CDialog::OnOK();
}

void CCursorOptionsDlg::OnSelchangeHighlightshape()
{
	// TODO: Add your control notification handler code here
	m_iHighlightShape = m_ctrlCBHightlightShape.GetCurSel();
	RefreshHighlight();
}

void CCursorOptionsDlg::OnHighlightcolor()
{
	CColorDialog colerdlog(m_clrHighlight, CC_ANYCOLOR | CC_FULLOPEN |CC_RGBINIT);
	if (colerdlog.DoModal() == IDOK)
	{
		m_clrHighlight = colerdlog.GetColor();
	}

	RefreshHighlight();
}

void CCursorOptionsDlg::OnHighlightcursor()
{
	// TODO: Add your control notification handler code here
	m_bHighlightCursor = m_ctrlButtonHighlightCursor.GetCheck();
	m_ctrlSliderHighlightSize.EnableWindow(m_bHighlightCursor);
	m_ctrlCBHightlightShape.EnableWindow(m_bHighlightCursor);
	m_ctrlButtonHightlightColor.EnableWindow(m_bHighlightCursor);
	m_ctrlStaticSize.EnableWindow(m_bHighlightCursor);
	m_ctrlStaticShape.EnableWindow(m_bHighlightCursor);
	m_ctrlStaticHalfSize.EnableWindow(m_bHighlightCursor);
	m_ctrlButtonHighlightClick.EnableWindow(m_bHighlightCursor);
	m_ctrlButtonHighlightLB.EnableWindow(m_bHighlightCursor && m_bHighlightClick);
	m_ctrlButtonHighlightRB.EnableWindow(m_bHighlightCursor && m_bHighlightClick);
	RefreshHighlight();
}

void CCursorOptionsDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	m_iHighlightSize = m_ctrlSliderHighlightSize.GetPos();
	m_bSliding = TRUE;
	RefreshHighlight();
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	m_bSliding = FALSE;
}

void CCursorOptionsDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	RefreshHighlight();
}

void CCursorOptionsDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CCursorOptionsDlg::OnHighlightColorLeft()
{
	CColorDialog colordlog(m_clrHighlightClickLeft, CC_ANYCOLOR | CC_FULLOPEN |CC_RGBINIT);
	if (IDOK == colordlog.DoModal())
	{
		m_clrHighlightClickLeft = colordlog.GetColor();
	}

	RefreshHighlight();
}

void CCursorOptionsDlg::OnHighlightColorRight()
{
	CColorDialog colerdlog(m_clrHighlightClickRight, CC_ANYCOLOR | CC_FULLOPEN |CC_RGBINIT);
	if (IDOK == colerdlog.DoModal())
	{
		m_clrHighlightClickRight = colerdlog.GetColor();
	}

	RefreshHighlight();
}

void CCursorOptionsDlg::OnEnableVisualClickFeedback()
{
	m_bHighlightClick = m_ctrlButtonHighlightClick.GetCheck();
	m_ctrlButtonHighlightLB.EnableWindow(m_bHighlightClick);
	m_ctrlButtonHighlightRB.EnableWindow(m_bHighlightClick);

	RefreshHighlight();
}

void CCursorOptionsDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	RefreshHighlight();
	CDialog::OnLButtonDown(nFlags, point);
}

void CCursorOptionsDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	RefreshHighlight();
	CDialog::OnLButtonUp(nFlags, point);
}

void CCursorOptionsDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	RefreshHighlight();
	CDialog::OnRButtonDown(nFlags, point);
}

void CCursorOptionsDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	RefreshHighlight();
	CDialog::OnRButtonUp(nFlags, point);
}

void CCursorOptionsDlg::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	RefreshHighlight();
	CDialog::OnRButtonDblClk(nFlags, point);
}

void CCursorOptionsDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	RefreshHighlight();
	CDialog::OnLButtonDblClk(nFlags, point);
}
