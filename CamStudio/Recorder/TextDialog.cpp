// CTextDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Recorder.h"
#include "TextDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int msgShown = 0;

/////////////////////////////////////////////////////////////////////////////
// CTextDlg dialog

CTextDlg::CTextDlg(CWnd* pParent /*=NULL*/)
: CDialog(CTextDlg::IDD, pParent)
{

	mycharset = ANSI_CHARSET;
	//{{AFX_DATA_INIT(CTextDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CTextDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTextDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, ID_JUSTLEFT, m_ctrlButtonJustifyLeft);
	DDX_Control(pDX, ID_JUSTCENTER, m_ctrlButtonJustifyCenter);
	DDX_Control(pDX, ID_JUSTRIGHT, m_ctrlButtonJustifyRight);
	DDX_Control(pDX, IDC_EDIT1, m_ctrlEditText);
	DDX_Control(pDX, ID_FONT2, m_ctrlButtonLanguage);
}

BEGIN_MESSAGE_MAP(CTextDlg, CDialog)
	//{{AFX_MSG_MAP(CTextDlg)
	ON_BN_CLICKED(ID_FONT, OnFont)
	ON_EN_CHANGE(IDC_EDIT1, OnChangeEdit1)
	ON_BN_CLICKED(ID_JUSTLEFT, OnJustleft)
	ON_BN_CLICKED(ID_JUSTCENTER, OnJustcenter)
	ON_BN_CLICKED(ID_JUSTRIGHT, OnJustright)
	ON_BN_CLICKED(ID_FONT2, OnFont2)
	ON_COMMAND(ID_SCRIPT_WESTERN, OnScriptWestern)
	ON_COMMAND(ID_SCRIPT_ARABIC, OnScriptArabic)
	ON_COMMAND(ID_SCRIPT_BALTIC, OnScriptBaltic)
	ON_COMMAND(ID_SCRIPT_CHINESEBIG5, OnScriptChinesebig5)
	ON_COMMAND(ID_SCRIPT_CHINESEGB2312, OnScriptChinesegb2312)
	ON_COMMAND(ID_SCRIPT_DEFAULT, OnScriptDefault)
	ON_COMMAND(ID_SCRIPT_EASTEUROPE, OnScriptEasteurope)
	ON_COMMAND(ID_SCRIPT_GREEK, OnScriptGreek)
	ON_COMMAND(ID_SCRIPT_HANGUL, OnScriptHangul)
	ON_COMMAND(ID_SCRIPT_HEBREW, OnScriptHebrew)
	ON_COMMAND(ID_SCRIPT_JAPANESESHIFTJIS, OnScriptJapaneseshiftjis)
	ON_COMMAND(ID_SCRIPT_JOHABKOREAN, OnScriptJohabkorean)
	ON_COMMAND(ID_SCRIPT_MAC, OnScriptMac)
	ON_COMMAND(ID_SCRIPT_OEM, OnScriptOem)
	ON_COMMAND(ID_SCRIPT_RUSSIAN, OnScriptRussian)
	ON_COMMAND(ID_SCRIPT_SYMBOL, OnScriptSymbol)
	ON_COMMAND(ID_SCRIPT_THAI, OnScriptThai)
	ON_COMMAND(ID_SCRIPT_TURKISH, OnScriptTurkish)
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTextDlg message handlers

void CTextDlg::OnOK()
{
	// TODO: Add extra validation here

	CDialog::OnOK();
}

BOOL CTextDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here

	HICON justifyLeftIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICONJUSTIFYLEFT));
	m_ctrlButtonJustifyLeft.SetIcon(justifyLeftIcon);

	HICON justifyCenterIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICONJUSTIFYCENTER));
	m_ctrlButtonJustifyCenter.SetIcon(justifyCenterIcon);

	HICON justifyRightIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICONJUSTIFYRIGHT));
	m_ctrlButtonJustifyRight.SetIcon(justifyRightIcon);

	if ((logFont) && (displayStr))
	{
		//CFont tempFont;
		//tempFont.CreateFontIndirect(logFont);
		//((CEdit *) GetDlgItem(IDC_EDIT1))->SetFont(&tempFont);

		tempFont.DeleteObject();
		tempFont.CreateFontIndirect(logFont);
		m_ctrlEditText.SetFont(&tempFont, TRUE);
		m_ctrlEditText.SetWindowText(*displayStr);

		//tempFont.DeleteObject();
	}

	CRecorderApp *pApp = (CRecorderApp *)AfxGetApp();
	m_ctrlButtonLanguage.EnableWindow(((pApp->VersionOp() >= 5) && (msgShown)));

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTextDlg::OnFont()
{
	// TODO: Add your control notification handler code here

	if (!logFont) return;

	CFontDialog fontdlg(logFont, CF_EFFECTS | CF_SCREENFONTS |CF_INITTOLOGFONTSTRUCT);
	fontdlg.m_cf.rgbColors = *colorrgb;

	if (fontdlg.DoModal()==IDOK) {

		fontdlg.GetCurrentFont(logFont);
		*colorrgb = fontdlg.GetColor();

		tempFont.DeleteObject();
		tempFont.CreateFontIndirect(logFont);
		m_ctrlEditText.SetFont(&tempFont, TRUE);

		//((CEdit *) GetDlgItem(IDC_EDIT1))->SetWindowText(*displayStr);
		//tempFont.DeleteObject();
	}

	InvalidateTransWnd();
}

void CTextDlg::PreModal(CString* inStr, LOGFONT *inFont,COLORREF* inRGB, CWnd* parent,int* horzalign)
{

	logFont = inFont;
	displayStr = inStr;
	colorrgb = inRGB;
	myparent = parent;
	myhorzalign = horzalign;
}

void CTextDlg::OnChangeEdit1()
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.

	// TODO: Add your control notification handler code here
	m_ctrlEditText.GetWindowText( *displayStr);

	InvalidateTransWnd();
}

void CTextDlg::InvalidateTransWnd()
{

	myparent->Invalidate();
	//if (myparent)
	// myparent->PostMessage(WM_USER_INVALIDATEWND,0,0);
}

void CTextDlg::OnJustleft()
{
	// TODO: Add your control notification handler code here
	*myhorzalign = DT_LEFT;
	InvalidateTransWnd();
}

void CTextDlg::OnJustcenter()
{
	// TODO: Add your control notification handler code here
	*myhorzalign = DT_CENTER;
	InvalidateTransWnd();
}

void CTextDlg::OnJustright()
{
	// TODO: Add your control notification handler code here
	*myhorzalign = DT_RIGHT;
	InvalidateTransWnd();

	//((CButton *) GetDlgItem(ID_JUSTRIGHT))->SetCheck(TRUE);
}

void CTextDlg::ChooseScriptFont()
{
	// TODO: Add your control notification handler code here
	//ZeroMemory(logFont, sizeof(LOGFONT));

	// OEM_CHARSET (255) is currently the one with the highest number. 
	// Cast to elimimate C4244 warning. (Assume that other charset will be always less than 255)
	logFont->lfCharSet = static_cast<BYTE>(mycharset);	

	CFontDialog fontdlg(logFont, CF_SELECTSCRIPT | CF_EFFECTS | CF_SCREENFONTS);
	fontdlg.m_cf.rgbColors = *colorrgb;

	if (fontdlg.DoModal()==IDOK) {

		fontdlg.GetCurrentFont(logFont);
		*colorrgb = fontdlg.GetColor();

		tempFont.DeleteObject();
		tempFont.CreateFontIndirect(logFont);
		m_ctrlEditText.SetFont(&tempFont, TRUE);

		InvalidateTransWnd();
	}
}

void CTextDlg::OnFont2()
{
	CRecorderApp *pApp = (CRecorderApp *)AfxGetApp();
	if (pApp->VersionOp() < 5)
	{
		//int ret = MessageBox("This feature works only in Win 2000/ XP." ,"Note",MB_OK | MB_ICONEXCLAMATION);
		MessageOut(m_hWnd,IDS_STRING_WORKSWINXP,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		msgShown = 1;
		m_ctrlButtonLanguage.EnableWindow(FALSE);
		return;
	}

	POINT point;
	GetCursorPos(&point);
	CPoint local = point;
	ScreenToClient(&local);

	CMenu menu;

	int menuToLoad = IDR_CONTEXTLANGUAGE;
	if (menu.LoadMenu(menuToLoad))
	{
		CMenu* pPopup = menu.GetSubMenu(0);
		ASSERT(pPopup != NULL);

		// route commands through main window
		pPopup->TrackPopupMenu(TPM_RIGHTBUTTON | TPM_LEFTALIGN, point.x, point.y, this);
	}
}

void CTextDlg::OnScriptWestern()
{
	// TODO: Add your command handler code here
	mycharset = ANSI_CHARSET;
	ChooseScriptFont();

}

void CTextDlg::OnScriptArabic()
{
	// TODO: Add your command handler code here
	mycharset = ARABIC_CHARSET;
	ChooseScriptFont();
}

void CTextDlg::OnScriptBaltic()
{
	// TODO: Add your command handler code here
	mycharset = BALTIC_CHARSET;
	ChooseScriptFont();
}

void CTextDlg::OnScriptChinesebig5()
{
	// TODO: Add your command handler code here
	mycharset = CHINESEBIG5_CHARSET;
	ChooseScriptFont();
}

void CTextDlg::OnScriptChinesegb2312()
{
	// TODO: Add your command handler code here
	mycharset = GB2312_CHARSET;
	ChooseScriptFont();
}

void CTextDlg::OnScriptDefault()
{
	// TODO: Add your command handler code here
	mycharset = DEFAULT_CHARSET;
	ChooseScriptFont();
}

void CTextDlg::OnScriptEasteurope()
{
	// TODO: Add your command handler code here
	mycharset = EASTEUROPE_CHARSET;
	ChooseScriptFont();
}

void CTextDlg::OnScriptGreek()
{
	// TODO: Add your command handler code here
	mycharset = GREEK_CHARSET;
	ChooseScriptFont();
}

void CTextDlg::OnScriptHangul()
{
	// TODO: Add your command handler code here
	//mycharset = HANGUL_CHARSET;
	mycharset = HANGEUL_CHARSET;
	ChooseScriptFont();
}

void CTextDlg::OnScriptHebrew()
{
	// TODO: Add your command handler code here
	mycharset = HEBREW_CHARSET;
	ChooseScriptFont();
}

void CTextDlg::OnScriptJapaneseshiftjis()
{
	// TODO: Add your command handler code here
	mycharset = SHIFTJIS_CHARSET;
	ChooseScriptFont();
}

void CTextDlg::OnScriptJohabkorean()
{
	// TODO: Add your command handler code here
	mycharset = JOHAB_CHARSET;
	ChooseScriptFont();
}

void CTextDlg::OnScriptMac()
{
	// TODO: Add your command handler code here
	mycharset = MAC_CHARSET;
	ChooseScriptFont();
}

void CTextDlg::OnScriptOem()
{
	// TODO: Add your command handler code here
	mycharset = OEM_CHARSET;
	ChooseScriptFont();
}

void CTextDlg::OnScriptRussian()
{
	// TODO: Add your command handler code here
	mycharset = RUSSIAN_CHARSET;
	ChooseScriptFont();
}

void CTextDlg::OnScriptSymbol()
{
	// TODO: Add your command handler code here
	mycharset = SYMBOL_CHARSET;
	ChooseScriptFont();
}

void CTextDlg::OnScriptThai()
{
	// TODO: Add your command handler code here
	mycharset = THAI_CHARSET;
	ChooseScriptFont();
}

void CTextDlg::OnScriptTurkish()
{
	// TODO: Add your command handler code here
	mycharset = TURKISH_CHARSET;
	ChooseScriptFont();
}

void CTextDlg::OnKillFocus(CWnd* pNewWnd)
{
	CDialog::OnKillFocus(pNewWnd);
}
