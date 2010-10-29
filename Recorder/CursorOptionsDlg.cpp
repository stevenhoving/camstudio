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

// WTF???! Can this life be easier?
// works both ways
// TODO: move it somewhere
__inline __declspec(naked) DWORD COLORREFtoARGB(COLORREF, BYTE)
{
	__asm
	{
		mov eax, DWORD PTR 4[esp]
		bswap eax
		mov al, BYTE PTR 8[esp]
		rcr eax, 8
		ret
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCursorOptionsDlg dialog

CCursorOptionsDlg::CCursorOptionsDlg(CWnd* pParent /*=NULL*/)
: CDialog(CCursorOptionsDlg::IDD, pParent)
, m_cCursor(CamCursor)
, m_pIconFileDlg(0)
, m_hPreviewCursor(NULL)
, m_hLoadCursor(CamCursor.Load())
, m_hCustomCursor(CamCursor.Custom())
, m_iCustomSel(CamCursor.CustomType())
, m_bRecordCursor(CamCursor.Record())
, m_iCursorType(CamCursor.Select())
, m_bHighlightCursor(CamCursor.Highlight())
, m_iHighlightSize(CamCursor.HighlightSize())
, m_iHighlightShape(CamCursor.HighlightShape())
, m_bHighlightClick(CamCursor.HighlightClick())
, m_clrHighlight(COLORREFtoARGB(CamCursor.HighlightColor(),0)) // plain GDI doesn't support transparency
, m_clrHighlightClickLeft(COLORREFtoARGB(CamCursor.ClickLeftColor(),0))
, m_clrHighlightClickRight(COLORREFtoARGB(CamCursor.ClickRightColor(),0))
, m_bInitPaint(true)
, m_bSliding(false)
{
	//{{AFX_DATA_INIT(CCursorOptionsDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	VERIFY(LoadStdCursors());
}

CCursorOptionsDlg::CCursorOptionsDlg(const CCamCursor& cCursor, CWnd* pParent)
: CDialog(CCursorOptionsDlg::IDD, pParent)
, m_cCursor(cCursor)
, m_pIconFileDlg(0)
, m_hPreviewCursor(NULL)
, m_hLoadCursor(cCursor.Load())
, m_hCustomCursor(cCursor.Custom())
, m_iCustomSel(cCursor.CustomType())
, m_bRecordCursor(cCursor.Record())
, m_iCursorType(cCursor.Select())
, m_bHighlightCursor(cCursor.Highlight())
, m_iHighlightSize(cCursor.HighlightSize())
, m_iHighlightShape(cCursor.HighlightShape())
, m_bHighlightClick(cCursor.HighlightClick())
, m_clrHighlight(COLORREFtoARGB(cCursor.HighlightColor(),0))
, m_clrHighlightClickLeft(COLORREFtoARGB(cCursor.ClickLeftColor(),0))
, m_clrHighlightClickRight(COLORREFtoARGB(cCursor.ClickRightColor(),0))
, m_bInitPaint(true)
, m_bSliding(false)
{
	VERIFY(LoadStdCursors());
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
	DDX_Control(pDX, IDC_STATIC_HIGHLIGHT_PREVIEW, m_ctrlStaticPreviewHighlight);
}

BEGIN_MESSAGE_MAP(CCursorOptionsDlg, CDialog)
	//{{AFX_MSG_MAP(CCursorOptionsDlg)
	ON_BN_CLICKED(IDC_RADIO1, OnHideCursor)
	ON_BN_CLICKED(IDC_RADIO2, OnShowCursor)
	ON_BN_CLICKED(IDC_CURSOR1, OnActualCursor)
	ON_BN_CLICKED(IDC_CURSOR2, OnCustomCursor)
	ON_BN_CLICKED(IDC_CURSOR3, OnFileCursor)
	ON_BN_CLICKED(IDC_FILECURSOR, OnBnClickedFileCursor)
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
	CSize previewsize(64, 64);
	int xoffset = 300;
	int yoffset = 230;
	CRect rectFrame;
	m_ctrlStaticPreviewHighlight.GetWindowRect(&rectFrame);
	ScreenToClient(&rectFrame);
	previewsize.cx = rectFrame.Width();
	previewsize.cy = rectFrame.Height();
	xoffset = rectFrame.left;
	yoffset = rectFrame.top + previewsize.cy/3;

	int highlightsizehalf = m_iHighlightSize/2;

	COLORREF usecolor = m_clrHighlight;

	if (m_bHighlightClick == 1 && m_bSliding==FALSE)
	{
		SHORT tmpShort = GetKeyState(VK_LBUTTON);
		if (tmpShort != 0 && tmpShort != 1)
		{
			usecolor = m_clrHighlightClickLeft;
			highlightsizehalf = (int)(highlightsizehalf * 1.5);
		}
		tmpShort = GetKeyState(VK_RBUTTON);
		if (tmpShort != 0 && tmpShort != 1)
		{
			usecolor = m_clrHighlightClickRight;
			highlightsizehalf = (int)(highlightsizehalf * 1.5);
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

	// OffScreen Buffer
	CWindowDC cWindowDC(this);
	CDC cDCBits;
	cDCBits.CreateCompatibleDC(&cWindowDC);
	CBitmap cBitmap;
	cBitmap.CreateCompatibleBitmap(&cWindowDC, previewsize.cx, previewsize.cy);
	CBitmap * pOldBitmap = cDCBits.SelectObject(&cBitmap);
	CBrush ptbrush;
	ptbrush.CreateStockObject(LTGRAY_BRUSH);
	CPen cNullPen;
	cNullPen.CreatePen(PS_NULL, 0, COLORREF(0));
	CBrush hlbrush;
	hlbrush.CreateSolidBrush(usecolor);
	CBrush * pOldbrush = cDCBits.SelectObject(&ptbrush);
	CPen * pOldPen = cDCBits.SelectObject(&cNullPen);
	cDCBits.Rectangle(0, 0, previewsize.cx + 1, previewsize.cy + 1);

	if (m_bHighlightCursor)
	{
		//draw the shape only if highlight cursor is selected
		cDCBits.SelectObject(&hlbrush);

		if ((m_iHighlightShape == 0) || (m_iHighlightShape == 1))
		{
			//circle and ellipse
			cDCBits.Ellipse((int) x1,(int) y1,(int) x2,(int) y2);
		}
		else if ((m_iHighlightShape == 2) || (m_iHighlightShape == 3))
		{ //square and rectangle
			cDCBits.Rectangle((int) x1,(int) y1,(int) x2,(int) y2);
		}
	}

	cDCBits.SelectObject(pOldbrush);
	cDCBits.SelectObject(pOldPen);
	//OffScreen Buffer
	cWindowDC.BitBlt(xoffset, yoffset, previewsize.cx, previewsize.cy, &cDCBits, 0, 0, SRCCOPY);
	cDCBits.SelectObject(pOldBitmap);
}

void CCursorOptionsDlg::RefreshPreviewCursor()
{
	m_hPreviewCursor = NULL;
	if (m_bRecordCursor)
	{
		switch (m_iCursorType)
		{
		case 0:
			m_hPreviewCursor = GetCursor();
			break;
		case 1:
			{
				m_iCustomSel = m_ctrlCBCustomCursor.GetCurSel();
				if (m_iCustomSel < 0) {
					// TODO: handle error
					return;
				}
				DWORD customicon = m_cCursor.GetID(m_iCustomSel);
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
	m_ctrlButtonActualCursor.SetCheck(m_iCursorType == ACTIVE);
	m_ctrlButtonCustonCursor.SetCheck(m_iCursorType == CUSTOM);
	m_ctrlButtonFileCursor.SetCheck(m_iCursorType == CUSTOMFILE);
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
	m_ctrlCBCustomCursor.EnableWindow(m_iCursorType == CUSTOM);
	m_ctrlButtonfileCursor.EnableWindow(m_iCursorType == CUSTOMFILE);

	RefreshPreviewCursor();

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CCursorOptionsDlg::OnHideCursor()
{
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

	m_ctrlCBCustomCursor.EnableWindow(m_iCursorType == CUSTOM);
	m_ctrlButtonfileCursor.EnableWindow(m_iCursorType == CUSTOMFILE);

	RefreshPreviewCursor();
}

void CCursorOptionsDlg::OnActualCursor()
{
	m_iCursorType = ACTIVE;
	m_ctrlButtonActualCursor.SetCheck(TRUE);
	m_ctrlButtonCustonCursor.SetCheck(FALSE);
	m_ctrlButtonFileCursor.SetCheck(FALSE);

	m_ctrlCBCustomCursor.EnableWindow(FALSE);
	m_ctrlButtonfileCursor.EnableWindow(FALSE);

	RefreshPreviewCursor();
}

void CCursorOptionsDlg::OnCustomCursor()
{
	m_iCursorType = CUSTOM;

	m_ctrlButtonActualCursor.SetCheck(FALSE);
	m_ctrlButtonCustonCursor.SetCheck(TRUE);
	m_ctrlButtonFileCursor.SetCheck(FALSE);

	m_ctrlCBCustomCursor.EnableWindow(TRUE);
	m_ctrlButtonfileCursor.EnableWindow(FALSE);

	RefreshPreviewCursor();
}

void CCursorOptionsDlg::OnFileCursor()
{
	m_iCursorType = CUSTOMFILE;

	m_ctrlButtonActualCursor.SetCheck(FALSE);
	m_ctrlButtonCustonCursor.SetCheck(FALSE);
	m_ctrlButtonFileCursor.SetCheck(TRUE);

	m_ctrlCBCustomCursor.EnableWindow(FALSE);
	m_ctrlButtonfileCursor.EnableWindow(TRUE);

	RefreshPreviewCursor();
}

void CCursorOptionsDlg::OnSelchangeCustomcursor()
{
	RefreshPreviewCursor();
}

void CCursorOptionsDlg::OnBnClickedFileCursor()
{
	CString fileName;
	CString filt="Icon and Cursor Files (*.ico; *.cur)|*.ico;*.cur||";

	if (m_pIconFileDlg == NULL)
	{
		m_pIconFileDlg = new CFileDialog(TRUE,"*.ico;*.cur","*.ico;*.cur",NULL,filt,this);
		if (!m_pIconFileDlg)
		{
			::OnError(_T("CCursorOptionsDlg::OnFilecursor"));
			return;
		}
	}
	char dirx[200];
	VERIFY(GetWindowsDirectory(dirx, 200));
	CString initdir(dirx);
	initdir = initdir + "\\cursors";

	m_pIconFileDlg->m_ofn.Flags|=OFN_FILEMUSTEXIST;
	m_pIconFileDlg->m_ofn.lpstrTitle="File to load";

	if (m_cCursor.Dir().IsEmpty())
	{
		m_cCursor.Dir(initdir);
	}
	m_pIconFileDlg->m_ofn.lpstrInitialDir = m_cCursor.Dir();
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

		m_cCursor.FileName(fileName);
		fileName = fileName.Left(fileName.ReverseFind('\\'));
		m_cCursor.Dir(fileName);
	}
	delete m_pIconFileDlg, m_pIconFileDlg = 0;
}

void CCursorOptionsDlg::OnOK()
{
	if (m_iCursorType == CUSTOM)
	{
		m_hCustomCursor = m_hPreviewCursor;
	}
	else
	{
		m_hLoadCursor = m_hPreviewCursor;

	} //cursortype

	m_cCursor.Record(m_bRecordCursor);
	m_cCursor.CustomType(m_iCustomSel);
	m_cCursor.Select(m_iCursorType);
	m_cCursor.Load(m_hLoadCursor);
	m_cCursor.Custom(m_hCustomCursor);
	m_cCursor.Highlight(m_bHighlightCursor);
	m_cCursor.HighlightSize(m_iHighlightSize);
	m_cCursor.HighlightShape(m_iHighlightShape);
	m_cCursor.HighlightColor(COLORREFtoARGB(m_clrHighlight, m_cCursor.HighlightColor() >> 24)); // preserve transparency level
	m_cCursor.HighlightClick(m_bHighlightClick);
	m_cCursor.ClickLeftColor(COLORREFtoARGB(m_clrHighlightClickLeft, m_cCursor.ClickLeftColor() >> 24));
	m_cCursor.ClickRightColor(COLORREFtoARGB(m_clrHighlightClickRight, m_cCursor.ClickRightColor() >> 24));

	CDialog::OnOK(); // TODO: usually should go first since calls UpdateData to update m_xx stuff
}

void CCursorOptionsDlg::OnSelchangeHighlightshape()
{
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
	m_bHighlightCursor = m_ctrlButtonHighlightCursor.GetCheck() ? true : false;
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
{ // what is this?
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
	m_bHighlightClick = m_ctrlButtonHighlightClick.GetCheck() ? true : false;
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

bool CCursorOptionsDlg::LoadStdCursors()
{
	m_cCursor.AddID(IDI_CUSTOMICON_CONTEXTHELP);
	m_cCursor.AddID(IDI_CUSTOMICON_MAGNIFY);
	m_cCursor.AddID(IDI_CUSTOMICON_NODRAG);
	m_cCursor.AddID(IDI_CUSTOMICON_SPLITBARH);
	m_cCursor.AddID(IDI_CUSTOMICON_SPLITBARV);
	m_cCursor.AddID(IDI_CUSTOMICON_TOPOFTABLE);
	m_cCursor.AddID(IDI_CUSTOMICON_BOOK1);
	m_cCursor.AddID(IDI_CUSTOMICON_BOOK2);
	m_cCursor.AddID(IDI_CUSTOMICON_CLIP1);
	m_cCursor.AddID(IDI_CUSTOMICON_CLIP2);
	m_cCursor.AddID(IDI_CUSTOMICON_CLOCK1);
	m_cCursor.AddID(IDI_CUSTOMICON_CLOCK2);
	m_cCursor.AddID(IDI_CUSTOMICON_CARDFILE1);
	m_cCursor.AddID(IDI_CUSTOMICON_CARDFILE2);
	m_cCursor.AddID(IDI_CUSTOMICON_DISK1);
	m_cCursor.AddID(IDI_CUSTOMICON_DISK2);
	m_cCursor.AddID(IDI_CUSTOMICON_FILES1);
	m_cCursor.AddID(IDI_CUSTOMICON_FILES2);
	m_cCursor.AddID(IDI_CUSTOMICON_FOLDER2);
	m_cCursor.AddID(IDI_CUSTOMICON_FOLDER1);
	m_cCursor.AddID(IDI_CUSTOMICON_MAIL1);
	m_cCursor.AddID(IDI_CUSTOMICON_MAIL2);
	m_cCursor.AddID(IDI_CUSTOMICON_NOTE1);
	m_cCursor.AddID(IDI_CUSTOMICON_NOTE2);
	m_cCursor.AddID(IDI_CUSTOMICON_PEN1);
	m_cCursor.AddID(IDI_CUSTOMICON_PEN2);
	m_cCursor.AddID(IDI_CUSTOMICON_PENCIL1);
	m_cCursor.AddID(IDI_CUSTOMICON_PENCIL2);
	m_cCursor.AddID(IDI_CUSTOMICON_PHONE1);
	m_cCursor.AddID(IDI_CUSTOMICON_PHONE2);
	m_cCursor.AddID(IDI_CUSTOMICON_POINT1);
	m_cCursor.AddID(IDI_CUSTOMICON_POINT2);
	m_cCursor.AddID(IDI_CUSTOMICON_SERCURITY);
	m_cCursor.AddID(IDI_CUSTOMICON_SECURITY2);
	return true;
}
