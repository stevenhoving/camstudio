// TextDialog.cpp : implementation file
//

#include "stdafx.h"
#include "vscap.h"
#include "TextDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int versionOp;
int msgShown = 0;

extern void ErrMsg(char format[], ...);
extern int MessageOutINT(HWND hWnd,long strMsg, long strTitle, UINT mbstatus,long val);
extern int MessageOutINT2(HWND hWnd,long strMsg, long strTitle, UINT mbstatus,long val1,long val2);
extern int MessageOut(HWND hWnd,long strMsg, long strTitle, UINT mbstatus);

/////////////////////////////////////////////////////////////////////////////
// TextDialog dialog


TextDialog::TextDialog(CWnd* pParent /*=NULL*/)
	: CDialog(TextDialog::IDD, pParent)
{

	mycharset = ANSI_CHARSET;
	//{{AFX_DATA_INIT(TextDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void TextDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(TextDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(TextDialog, CDialog)
	//{{AFX_MSG_MAP(TextDialog)
	ON_BN_CLICKED(IDFONT, OnFont)
	ON_EN_CHANGE(IDC_EDIT1, OnChangeEdit1)
	ON_BN_CLICKED(ID_JUSTLEFT, OnJustleft)
	ON_BN_CLICKED(ID_JUSTCENTER, OnJustcenter)
	ON_BN_CLICKED(ID_JUSTRIGHT, OnJustright)
	ON_BN_CLICKED(IDFONT2, OnFont2)
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
// TextDialog message handlers

void TextDialog::OnOK() 
{
	// TODO: Add extra validation here
	
	
	CDialog::OnOK();
}

BOOL TextDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	HICON justifyLeftIcon= LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICONJUSTIFYLEFT));
	((CButton *) GetDlgItem(ID_JUSTLEFT))->SetIcon(justifyLeftIcon);

	HICON justifyCenterIcon= LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICONJUSTIFYCENTER));
	((CButton *) GetDlgItem(ID_JUSTCENTER))->SetIcon(justifyCenterIcon);

	HICON justifyRightIcon= LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICONJUSTIFYRIGHT));
	((CButton *) GetDlgItem(ID_JUSTRIGHT))->SetIcon(justifyRightIcon);

	((CEdit *) GetDlgItem(IDC_EDIT1))->SetFocus(); 


	if ((logFont) && (displayStr)) {

		//CFont tempFont;

		//tempFont.CreateFontIndirect(logFont);	
		//((CEdit *) GetDlgItem(IDC_EDIT1))->SetFont(&tempFont);

		tempFont.DeleteObject();
		tempFont.CreateFontIndirect(logFont);
		((CEdit *) GetDlgItem(IDC_EDIT1))->SetFont(&tempFont, TRUE);		
		
		
		((CEdit *) GetDlgItem(IDC_EDIT1))->SetWindowText(*displayStr);

		//tempFont.DeleteObject();

	}

	
	if ((versionOp < 5) && (msgShown))
	{
		((CButton *) GetDlgItem(IDFONT2))->EnableWindow(FALSE);		
	}
	else
	{
		((CButton *) GetDlgItem(IDFONT2))->EnableWindow(TRUE);

	}

	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void TextDialog::OnFont() 
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
		((CEdit *) GetDlgItem(IDC_EDIT1))->SetFont(&tempFont, TRUE);
		
		//((CEdit *) GetDlgItem(IDC_EDIT1))->SetWindowText(*displayStr);
		//tempFont.DeleteObject();

	}	

	InvalidateTransWnd();
	
}


void TextDialog::PreModal(CString* inStr, LOGFONT *inFont,COLORREF* inRGB, CWnd* parent,int* horzalign) 
{

	logFont = inFont;
	displayStr = inStr;
	colorrgb = inRGB;
	myparent  = parent;
	myhorzalign =  horzalign;

}

void TextDialog::OnChangeEdit1() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_CHANGE flag ORed into the lParam mask.
	
	// TODO: Add your control notification handler code here
	((CEdit *) GetDlgItem(IDC_EDIT1))->GetWindowText( *displayStr); 

	InvalidateTransWnd();
	
}



void TextDialog::InvalidateTransWnd()
{


	myparent->Invalidate();
	//if (myparent)
	//	myparent->PostMessage(WM_USER_INVALIDATEWND,0,0);

}

void TextDialog::OnJustleft() 
{
	// TODO: Add your control notification handler code here
	*myhorzalign = DT_LEFT;
	InvalidateTransWnd();
	
}

void TextDialog::OnJustcenter() 
{
	// TODO: Add your control notification handler code here
	*myhorzalign = DT_CENTER;
	InvalidateTransWnd();
	
}

void TextDialog::OnJustright() 
{
	// TODO: Add your control notification handler code here
	*myhorzalign = DT_RIGHT;
	InvalidateTransWnd();

	//((CButton *) GetDlgItem(ID_JUSTRIGHT))->SetCheck(TRUE);
	
}



void TextDialog::ChooseScriptFont() 
{
	// TODO: Add your control notification handler code here
	//ZeroMemory(logFont, sizeof(LOGFONT));



	logFont->lfCharSet = mycharset;
	CFontDialog fontdlg(logFont, CF_SELECTSCRIPT | CF_EFFECTS | CF_SCREENFONTS);
	fontdlg.m_cf.rgbColors = *colorrgb;

	if (fontdlg.DoModal()==IDOK) {

		fontdlg.GetCurrentFont(logFont);
		*colorrgb = fontdlg.GetColor();

		tempFont.DeleteObject();
		tempFont.CreateFontIndirect(logFont);
		((CEdit *) GetDlgItem(IDC_EDIT1))->SetFont(&tempFont, TRUE);		
		
		InvalidateTransWnd();

	}	



}


void TextDialog::OnFont2() 
{
	// TODO: Add your control notification handler code here
	
	if (versionOp<5) {
		
		//int ret = MessageBox("This feature works only in Win 2000/ XP." ,"Note",MB_OK | MB_ICONEXCLAMATION);		
		int ret = MessageOut(this->m_hWnd,IDS_STRING_WORKSWINXP,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		
		msgShown = 1;				
		((CButton *) GetDlgItem(IDFONT2))->EnableWindow(FALSE);	
		
		//if (ret==IDNO)		
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

		pPopup->TrackPopupMenu(TPM_RIGHTBUTTON | TPM_LEFTALIGN,
							   point.x, point.y,
							   this); // route commands through main window
	}
	
}

void TextDialog::OnScriptWestern() 
{
	// TODO: Add your command handler code here
	mycharset = ANSI_CHARSET;
	ChooseScriptFont();
	
}

void TextDialog::OnScriptArabic() 
{
	// TODO: Add your command handler code here
	mycharset = ARABIC_CHARSET;
	ChooseScriptFont();
	
}

void TextDialog::OnScriptBaltic() 
{
	// TODO: Add your command handler code here
	mycharset = BALTIC_CHARSET;
	ChooseScriptFont();
	
}

void TextDialog::OnScriptChinesebig5() 
{
	// TODO: Add your command handler code here
	mycharset = CHINESEBIG5_CHARSET;
	ChooseScriptFont();
	
}

void TextDialog::OnScriptChinesegb2312() 
{
	// TODO: Add your command handler code here
	mycharset = GB2312_CHARSET;
	ChooseScriptFont();
	
}

void TextDialog::OnScriptDefault() 
{
	// TODO: Add your command handler code here
	mycharset = DEFAULT_CHARSET;
	ChooseScriptFont();
	
}

void TextDialog::OnScriptEasteurope() 
{
	// TODO: Add your command handler code here
	mycharset = EASTEUROPE_CHARSET;
	ChooseScriptFont();
	
}

void TextDialog::OnScriptGreek() 
{
	// TODO: Add your command handler code here
	mycharset = GREEK_CHARSET;
	ChooseScriptFont();
	
}

void TextDialog::OnScriptHangul() 
{
	// TODO: Add your command handler code here
	//mycharset = HANGUL_CHARSET;
	mycharset = HANGEUL_CHARSET;
	ChooseScriptFont();
	
}

void TextDialog::OnScriptHebrew() 
{
	// TODO: Add your command handler code here
	mycharset = HEBREW_CHARSET;
	ChooseScriptFont();
	
}

void TextDialog::OnScriptJapaneseshiftjis() 
{
	// TODO: Add your command handler code here
	mycharset = SHIFTJIS_CHARSET;
	ChooseScriptFont();

	
}

void TextDialog::OnScriptJohabkorean() 
{
	// TODO: Add your command handler code here
	mycharset = JOHAB_CHARSET;
	ChooseScriptFont();
	
}

void TextDialog::OnScriptMac() 
{
	// TODO: Add your command handler code here
	mycharset = MAC_CHARSET;
	ChooseScriptFont();
	
}

void TextDialog::OnScriptOem() 
{
	// TODO: Add your command handler code here
	mycharset = OEM_CHARSET;
	ChooseScriptFont();
	
}

void TextDialog::OnScriptRussian() 
{
	// TODO: Add your command handler code here
	mycharset = RUSSIAN_CHARSET;
	ChooseScriptFont();
	
}

void TextDialog::OnScriptSymbol() 
{
	// TODO: Add your command handler code here
	mycharset = SYMBOL_CHARSET;
	ChooseScriptFont();
	
}

void TextDialog::OnScriptThai() 
{
	// TODO: Add your command handler code here
	mycharset = THAI_CHARSET;
	ChooseScriptFont();
	
}

void TextDialog::OnScriptTurkish() 
{
	// TODO: Add your command handler code here
	mycharset = TURKISH_CHARSET;
	ChooseScriptFont();
	
}



			

void TextDialog::OnKillFocus(CWnd* pNewWnd) 
{
	CDialog::OnKillFocus(pNewWnd);
	
	
}
