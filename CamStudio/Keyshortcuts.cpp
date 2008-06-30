  // Keyshortcuts.cpp : implementation file
//

#include "stdafx.h"
#include "vscap.h"
#include "Keyshortcuts.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern UINT keyRecordStart;
extern UINT keyRecordEnd;
extern UINT keyRecordCancel;

extern UINT keyRecordStartCtrl;
extern UINT keyRecordEndCtrl;
extern UINT keyRecordCancelCtrl;

extern UINT keyRecordStartAlt;
extern UINT keyRecordEndAlt;
extern UINT keyRecordCancelAlt;

extern UINT keyRecordStartShift;
extern UINT keyRecordEndShift;
extern UINT keyRecordCancelShift;

extern UINT keyNext;
extern UINT keyPrev;
extern UINT keyShowLayout;

extern UINT keyNextCtrl;
extern UINT keyPrevCtrl;
extern UINT keyShowLayoutCtrl;

extern UINT keyNextAlt;
extern UINT keyPrevAlt;
extern UINT keyShowLayoutAlt;

extern UINT keyNextShift;
extern UINT keyPrevShift;
extern UINT keyShowLayoutShift;


UINT keyNextLocal;
UINT keyPrevLocal;
UINT keyShowLayoutLocal;

UINT keyNextLocalCtrl;
UINT keyPrevLocalCtrl;
UINT keyShowLayoutLocalCtrl;

UINT keyNextLocalAlt;
UINT keyPrevLocalAlt;
UINT keyShowLayoutLocalAlt;

UINT keyNextLocalShift;
UINT keyPrevLocalShift;
UINT keyShowLayoutLocalShift;


UINT keyRecordStartLocal;
UINT keyRecordEndLocal;
UINT keyRecordCancelLocal;

UINT keyRecordStartLocalCtrl;
UINT keyRecordEndLocalCtrl;
UINT keyRecordCancelLocalCtrl;

UINT keyRecordStartLocalAlt;
UINT keyRecordEndLocalAlt;
UINT keyRecordCancelLocalAlt;

UINT keyRecordStartLocalShift;
UINT keyRecordEndLocalShift;
UINT keyRecordCancelLocalShift;


int numkeys = 6;
int numSpecial = 3;
int key[6];
int keySpecial[6][3];
CString keyName[6];

extern int MessageOut(HWND hWnd,long strMsg, long strTitle, UINT mbstatus);
int TestKeysOverlap(int& o1, int& o2); 


/////////////////////////////////////////////////////////////////////////////
// Keyshortcuts dialog


Keyshortcuts::Keyshortcuts(CWnd* pParent /*=NULL*/)
	: CDialog(Keyshortcuts::IDD, pParent)
{
	//{{AFX_DATA_INIT(Keyshortcuts)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void Keyshortcuts::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(Keyshortcuts)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(Keyshortcuts, CDialog)
	//{{AFX_MSG_MAP(Keyshortcuts)
	ON_CBN_SELCHANGE(IDC_STOPKEY, OnSelchangeStopkey)
	ON_CBN_SELCHANGE(IDC_CANCELKEY, OnSelchangeCancelkey)
	ON_CBN_SELCHANGE(IDC_RECORDKEY, OnSelchangeRecordkey)
	ON_CBN_SELCHANGE(IDC_NEXTKEY, OnSelchangeNextkey)
	ON_CBN_SELCHANGE(IDC_PREVKEY, OnSelchangePrevkey)
	ON_CBN_SELCHANGE(IDC_SHOWKEY, OnSelchangeShowkey)
	ON_BN_CLICKED(IDC_CTRL1, OnCtrl1)
	ON_BN_CLICKED(IDC_CTRL2, OnCtrl2)
	ON_BN_CLICKED(IDC_CTRL3, OnCtrl3)
	ON_BN_CLICKED(IDC_CTRL4, OnCtrl4)
	ON_BN_CLICKED(IDC_CTRL5, OnCtrl5)
	ON_BN_CLICKED(IDC_CTRL6, OnCtrl6)
	ON_BN_CLICKED(IDC_SHIFT1, OnShift1)
	ON_BN_CLICKED(IDC_SHIFT2, OnShift2)
	ON_BN_CLICKED(IDC_SHIFT3, OnShift3)
	ON_BN_CLICKED(IDC_SHIFT4, OnShift4)
	ON_BN_CLICKED(IDC_SHIFT5, OnShift5)
	ON_BN_CLICKED(IDC_SHIFT6, OnShift6)
	ON_BN_CLICKED(IDC_ALT1, OnAlt1)
	ON_BN_CLICKED(IDC_ALT2, OnAlt2)
	ON_BN_CLICKED(IDC_ALT3, OnAlt3)
	ON_BN_CLICKED(IDC_ALT4, OnAlt4)
	ON_BN_CLICKED(IDC_ALT5, OnAlt5)
	ON_BN_CLICKED(IDC_ALT6, OnAlt6)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// Keyshortcuts message handlers


int TestKeysOverlap(int& o1, int& o2) 
{

	

	key[0]=	keyRecordStartLocal;
	key[1]=	keyRecordEndLocal;
	key[2]=	keyRecordCancelLocal;
	key[3]=	keyNextLocal;
	key[4]=	keyPrevLocal;
	key[5]=	keyShowLayoutLocal;

	keySpecial[0][0]=	keyRecordStartLocalCtrl;
	keySpecial[1][0]=	keyRecordEndLocalCtrl;
	keySpecial[2][0]=	keyRecordCancelLocalCtrl;
	keySpecial[3][0]=	keyNextLocalCtrl;
	keySpecial[4][0]=	keyPrevLocalCtrl;
	keySpecial[5][0]=	keyShowLayoutLocalCtrl;

	keySpecial[0][1]=	keyRecordStartLocalAlt;
	keySpecial[1][1]=	keyRecordEndLocalAlt;
	keySpecial[2][1]=	keyRecordCancelLocalAlt;
	keySpecial[3][1]=	keyNextLocalAlt;
	keySpecial[4][1]=	keyPrevLocalAlt;
	keySpecial[5][1]=	keyShowLayoutLocalAlt;

	keySpecial[0][2]=	keyRecordStartLocalShift;
	keySpecial[1][2]=	keyRecordEndLocalShift;
	keySpecial[2][2]=	keyRecordCancelLocalShift;
	keySpecial[3][2]=	keyNextLocalShift;
	keySpecial[4][2]=	keyPrevLocalShift;
	keySpecial[5][2]=	keyShowLayoutLocalShift;


	keyName[0].LoadString(IDS_STRINGSPRKEY);
	keyName[1].LoadString(IDS_STRINGSRKEY);
	keyName[2].LoadString(IDS_STRINGCRKEY);
	keyName[3].LoadString(IDS_STRINGNLKEY);	
	keyName[4].LoadString(IDS_STRINGPLKEY);	
	keyName[5].LoadString(IDS_STRINGSHLKEY);		
	
	//keyName[0] = "Start/Pause Record Key";
	//keyName[1] = "Stop Record Key";
	//keyName[2] = "Cancel Record Key";
	//keyName[3] = "Next Layout Key";
	//keyName[4] = "Previous Layout Key";
	//keyName[5] = "Show / Hide Layout Key";

	
	for (int i=0; i<numkeys; i++)
	{
		for (int j=0; j<numkeys; j++)
		{

			if (i!=j)
			{

				if (key[i]==key[j])
				{

					int statusSpecial = 0;
					for (int k=0; k<numSpecial; k++)
					{
						
						if (keySpecial[i][k]!=keySpecial[j][k])
							statusSpecial = 1;

					}
					if (!statusSpecial)
					{					
						o1 = i;
						o2 = j;
						return 1;

					}		


				} //(key[i]==key[j])
			} //i!=j
		}

	}


	return 0;

}


void Keyshortcuts::OnOK() 
{

	// TODO: Add extra validation here

	
	int key1 = 0 ;
	int key2 = 0 ;
	if (TestKeysOverlap( key1, key2))
	{
		if ((key1>=0) && (key1<numkeys) && (key2>=0) && (key2<numkeys))
		{
			CString errStr, formatStr, noteStr;
			formatStr.LoadString(IDS_STRINGSAMEKEY);
			errStr.Format(formatStr,keyName[key1],keyName[key2]);

			noteStr.LoadString(IDS_STRING_NOTE);
			MessageBox(errStr, noteStr ,MB_OK | MB_ICONEXCLAMATION);

			//MessageBox(errStr, "Note",MB_OK | MB_ICONEXCLAMATION);

			return;

		}
	}
	
	keyRecordStart = keyRecordStartLocal;
	keyRecordEnd = keyRecordEndLocal;
	keyRecordCancel = keyRecordCancelLocal;

	keyNext = keyNextLocal;
	keyPrev = keyPrevLocal;
	keyShowLayout = keyShowLayoutLocal;


	keyRecordStartCtrl = keyRecordStartLocalCtrl;
	keyRecordEndCtrl = keyRecordEndLocalCtrl;
	keyRecordCancelCtrl = keyRecordCancelLocalCtrl;

	keyNextCtrl = keyNextLocalCtrl;
	keyPrevCtrl = keyPrevLocalCtrl;
	keyShowLayoutCtrl = keyShowLayoutLocalCtrl;


	keyRecordStartAlt = keyRecordStartLocalAlt;
	keyRecordEndAlt = keyRecordEndLocalAlt;
	keyRecordCancelAlt = keyRecordCancelLocalAlt;

	keyNextAlt = keyNextLocalAlt;
	keyPrevAlt = keyPrevLocalAlt;
	keyShowLayoutAlt = keyShowLayoutLocalAlt;

	keyRecordStartShift = keyRecordStartLocalShift;
	keyRecordEndShift = keyRecordEndLocalShift;
	keyRecordCancelShift = keyRecordCancelLocalShift;

	keyNextShift = keyNextLocalShift;
	keyPrevShift = keyPrevLocalShift;
	keyShowLayoutShift = keyShowLayoutLocalShift;

	
	CDialog::OnOK();
}

BOOL Keyshortcuts::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	keyRecordStartLocal = keyRecordStart;
	keyRecordEndLocal = keyRecordEnd;
	keyRecordCancelLocal = keyRecordCancel;

	keyRecordStartLocalCtrl = keyRecordStartCtrl;
	keyRecordEndLocalCtrl = keyRecordEndCtrl;
	keyRecordCancelLocalCtrl = keyRecordCancelCtrl;

	keyRecordStartLocalAlt = keyRecordStartAlt;
	keyRecordEndLocalAlt = keyRecordEndAlt;
	keyRecordCancelLocalAlt = keyRecordCancelAlt;

	keyRecordStartLocalShift = keyRecordStartShift;
	keyRecordEndLocalShift = keyRecordEndShift;
	keyRecordCancelLocalShift = keyRecordCancelShift;


	keyNextLocal = keyNext;
	keyPrevLocal = keyPrev;
	keyShowLayoutLocal = keyShowLayout;

	keyNextLocalCtrl = keyNextCtrl;
	keyPrevLocalCtrl = keyPrevCtrl;
	keyShowLayoutLocalCtrl = keyShowLayoutCtrl;

	keyNextLocalAlt = keyNextAlt;
	keyPrevLocalAlt = keyPrevAlt;
	keyShowLayoutLocalAlt = keyShowLayoutAlt;

	keyNextLocalShift = keyNextShift;
	keyPrevLocalShift = keyPrevShift;
	keyShowLayoutLocalShift = keyShowLayoutShift;




	int nx = GetIndex(keyRecordStart);
	((CComboBox *) GetDlgItem(IDC_RECORDKEY))->SetCurSel(nx);

	nx = GetIndex(keyRecordEnd);
	((CComboBox *) GetDlgItem(IDC_STOPKEY))->SetCurSel(nx);

	nx = GetIndex(keyRecordCancel);
	((CComboBox *) GetDlgItem(IDC_CANCELKEY))->SetCurSel(nx);

	nx = GetIndex(keyNext);
	((CComboBox *) GetDlgItem(IDC_NEXTKEY))->SetCurSel(nx);

	nx = GetIndex(keyPrev);
	((CComboBox *) GetDlgItem(IDC_PREVKEY))->SetCurSel(nx);

	nx = GetIndex(keyShowLayout);
	((CComboBox *) GetDlgItem(IDC_SHOWKEY))->SetCurSel(nx);

	
	((CButton *) GetDlgItem(IDC_CTRL1))->SetCheck(keyRecordStartCtrl);
	((CButton *) GetDlgItem(IDC_CTRL2))->SetCheck(keyRecordEndCtrl);
	((CButton *) GetDlgItem(IDC_CTRL3))->SetCheck(keyRecordCancelCtrl);
	((CButton *) GetDlgItem(IDC_CTRL4))->SetCheck(keyNextCtrl);
	((CButton *) GetDlgItem(IDC_CTRL5))->SetCheck(keyPrevCtrl);
	((CButton *) GetDlgItem(IDC_CTRL6))->SetCheck(keyShowLayoutCtrl);

	((CButton *) GetDlgItem(IDC_ALT1))->SetCheck(keyRecordStartAlt);
	((CButton *) GetDlgItem(IDC_ALT2))->SetCheck(keyRecordEndAlt);
	((CButton *) GetDlgItem(IDC_ALT3))->SetCheck(keyRecordCancelAlt);
	((CButton *) GetDlgItem(IDC_ALT4))->SetCheck(keyNextAlt);
	((CButton *) GetDlgItem(IDC_ALT5))->SetCheck(keyPrevAlt);
	((CButton *) GetDlgItem(IDC_ALT6))->SetCheck(keyShowLayoutAlt);

	((CButton *) GetDlgItem(IDC_SHIFT1))->SetCheck(keyRecordStartShift);
	((CButton *) GetDlgItem(IDC_SHIFT2))->SetCheck(keyRecordEndShift);
	((CButton *) GetDlgItem(IDC_SHIFT3))->SetCheck(keyRecordCancelShift);
	((CButton *) GetDlgItem(IDC_SHIFT4))->SetCheck(keyNextShift);
	((CButton *) GetDlgItem(IDC_SHIFT5))->SetCheck(keyPrevShift);
	((CButton *) GetDlgItem(IDC_SHIFT6))->SetCheck(keyShowLayoutShift);


	

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


int Keyshortcuts::GetIndex(UINT keyShortCut) {
	
	int index = 0;

	switch (keyShortCut) {

		case VK_F1 :  index = 0; break;
		case VK_F2 :  index = 1; break;
		case VK_F3 :  index = 2; break;
		case VK_F4 :  index = 3; break;
		case VK_F5 :  index = 4; break;
		case VK_F6 :  index = 5; break;
		case VK_F7 :  index = 6; break;
		case VK_F8 :  index = 7; break;
		case VK_F9 :  index = 8; break;
		case VK_F10 :  index = 9; break;			
		case VK_F11 :  index = 10; break;
		case VK_F12 :  index = 11; break;
		case 'A' :  index = 12; break;
		case 'B' :  index = 13; break;
		case 'C' :  index = 14; break;
		case 'D' :  index = 15; break;
		case 'E' :  index = 16; break;
		case 'F' :  index = 17; break;
		case 'G' :  index = 18; break;
		case 'H' :  index = 19; break;
		case 'I' :  index = 20; break;
		case 'J' :  index = 21; break;
		case 'K' :  index = 22; break;
		case 'L' :  index = 23; break;
		case 'M' :  index = 24; break;
		case 'N' :  index = 25; break;
		case 'O' :  index = 26; break;
		case 'P' :  index = 27; break;
		case 'Q' :  index = 28; break;
		case 'R' :  index = 29; break;
		case 'S' :  index = 30; break;
		case 'T' :  index = 31; break;
		case 'U' :  index = 32; break;
		case 'V' :  index = 33; break;
		case 'W' :  index = 34; break;
		case 'X' :  index = 35; break;
		case 'Y' :  index = 36; break;
		case 'Z' :  index = 37; break;		
		case '1' :  index = 38; break;
		case '2' :  index = 39; break;
		case '3' :  index = 40; break;
		case '4' :  index = 41; break;
		case '5' :  index = 42; break;
		case '6' :  index = 43; break;
		case '7' :  index = 44; break;
		case '8' :  index = 45; break;
		case '9' :  index = 46; break;
		case '0' :  index = 47; break;		
		case VK_NUMPAD0  :  index = 48; break;
		case VK_NUMPAD1  :  index = 49; break;
		case VK_NUMPAD2  :  index = 50; break;
		case VK_NUMPAD3  :  index = 51; break;
		case VK_NUMPAD4  :  index = 52; break;
		case VK_NUMPAD5  :  index = 53; break;
		case VK_NUMPAD6  :  index = 54; break;
		case VK_NUMPAD7  :  index = 55; break;
		case VK_NUMPAD8  :  index = 56; break;
		case VK_NUMPAD9  :  index = 57; break;				
		case VK_HOME  :  index = 58; break;
		case VK_INSERT  :  index = 59; break;		
		case VK_DELETE  :  index = 60; break;
		case VK_END  :  index = 61; break;
		case VK_RETURN  :  index = 62; break;
		case VK_SPACE  :  index = 63; break;
		case VK_TAB  :  index = 64; break;
		case 100000  :  index = 65; break;

	}
	
	
	return index;
	
	
};


UINT Keyshortcuts::GetCode(int index) {
	
	int vcode = 0;

	switch (index) {

		case  0 : vcode = VK_F1; break;
		case  1 : vcode = VK_F2; break;
		case  2 : vcode = VK_F3; break;
		case  3 : vcode = VK_F4; break;
		case  4 : vcode = VK_F5; break;
		case  5 : vcode = VK_F6; break;
		case  6 : vcode = VK_F7; break;
		case  7 : vcode = VK_F8; break;
		case  8 : vcode = VK_F9; break;
		case  9 : vcode = VK_F10; break;
		case  10 : vcode = VK_F11; break;
		case  11 : vcode = VK_F12; break;
		case  12 : vcode = 'A'; break;
		case  13 : vcode = 'B'; break;
		case  14 : vcode = 'C'; break;
		case  15 : vcode = 'D'; break;
		case  16 : vcode = 'E'; break;
		case  17 : vcode = 'F'; break;
		case  18 : vcode = 'G'; break;
		case  19 : vcode = 'H'; break;
		case  20 : vcode = 'I'; break;
		case  21 : vcode = 'J'; break;
		case  22 : vcode = 'K'; break;
		case  23 : vcode = 'L'; break;
		case  24 : vcode = 'M'; break;
		case  25 : vcode = 'N'; break;
		case  26 : vcode = 'O'; break;
		case  27 : vcode = 'P'; break;
		case  28 : vcode = 'Q'; break;
		case  29 : vcode = 'R'; break;
		case  30 : vcode = 'S'; break;
		case  31 : vcode = 'T'; break;
		case  32 : vcode = 'U'; break;
		case  33 : vcode = 'V'; break;
		case  34 : vcode = 'W'; break;
		case  35 : vcode = 'X'; break;
		case  36 : vcode = 'Y'; break;
		case  37 : vcode = 'Z'; break;
		case  38 : vcode = '1'; break;
		case  39 : vcode = '2'; break;
		case  40 : vcode = '3'; break;
		case  41 : vcode = '4'; break;
		case  42 : vcode = '5'; break;
		case  43 : vcode = '6'; break;
		case  44 : vcode = '7'; break;
		case  45 : vcode = '8'; break;
		case  46 : vcode = '9'; break;
		case  47 : vcode = '0'; break;
		case  48 : vcode = VK_NUMPAD0 ; break;
		case  49 : vcode = VK_NUMPAD1 ; break;
		case  50 : vcode = VK_NUMPAD2 ; break;
		case  51 : vcode = VK_NUMPAD3 ; break;
		case  52 : vcode = VK_NUMPAD4 ; break;
		case  53 : vcode = VK_NUMPAD5 ; break;
		case  54 : vcode = VK_NUMPAD6 ; break;
		case  55 : vcode = VK_NUMPAD7 ; break;
		case  56 : vcode = VK_NUMPAD8 ; break;
		case  57 : vcode = VK_NUMPAD9 ; break;		
		case  58 : vcode = VK_HOME ; break;
		case  59 : vcode = VK_INSERT ; break;
		case  60 : vcode = VK_DELETE ; break;
		case  61 : vcode = VK_END ; break;
		case  62 : vcode = VK_RETURN ; break;
		case  63 : vcode = VK_SPACE ; break;
		case  64 : vcode = VK_TAB ; break;
		case  65 : vcode = 100000 ; break;

	}
	
	return vcode;	
	
};



void Keyshortcuts::OnSelchangeStopkey() 
{
	// TODO: Add your control notification handler code here	
	int index = ((CComboBox *) GetDlgItem(IDC_STOPKEY))->GetCurSel();
	UINT vcode = GetCode(index);

	keyRecordEndLocal = vcode;

	/*
	if (vcode==keyRecordStartLocal) {
		
		//MessageBox("This key is used by the Record/Pause Key");
		MessageOut(this->m_hWnd,IDS_STRING_KEYRECORD ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
			
		int nx = GetIndex(keyRecordEndLocal);
		((CComboBox *) GetDlgItem(IDC_STOPKEY))->SetCurSel(nx);
		return;


	}
		
	if (vcode==keyRecordCancelLocal) {
		//MessageBox("This key is used by the Cancel Key");	
		MessageOut(this->m_hWnd,IDS_STRING_KEYCANCEL ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		
		int nx = GetIndex(keyRecordEndLocal);
		((CComboBox *) GetDlgItem(IDC_STOPKEY))->SetCurSel(nx);
		return;
	}

	keyRecordEndLocal = vcode;
	*/
	
}


void Keyshortcuts::OnSelchangeCancelkey() 
{
	// TODO: Add your control notification handler code here
	int index = ((CComboBox *) GetDlgItem(IDC_CANCELKEY))->GetCurSel();
	UINT vcode = GetCode(index);

	keyRecordCancelLocal = vcode;

	/*
	if (vcode==keyRecordStartLocal) {
		
		//MessageBox("This key is used by the Record/Pause Key");
		MessageOut(this->m_hWnd,IDS_STRING_KEYRECORD ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);


	
		int nx = GetIndex(keyRecordCancelLocal);
		((CComboBox *) GetDlgItem(IDC_CANCELKEY))->SetCurSel(nx);
		return;

	}
		
	if (vcode==keyRecordEndLocal) {
		
		//MessageBox("This key is used by the Stop Key");	
		MessageOut(this->m_hWnd,IDS_STRING_KEYSTOP ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);

		
		int nx = GetIndex(keyRecordCancelLocal);
		((CComboBox *) GetDlgItem(IDC_CANCELKEY))->SetCurSel(nx);
		return;
	}

	keyRecordCancelLocal = vcode;
	*/
	
}

void Keyshortcuts::OnSelchangeRecordkey() 
{
	// TODO: Add your control notification handler code here
	int index = ((CComboBox *) GetDlgItem(IDC_RECORDKEY))->GetCurSel();
	UINT vcode = GetCode(index);

	keyRecordStartLocal = vcode;

	/*
	if (vcode==keyRecordCancelLocal) {
		
		//MessageBox("This key is used by the Cancel Recording Key");
		MessageOut(this->m_hWnd,IDS_STRING_KEYCANCEL ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
	
		int nx = GetIndex(keyRecordStartLocal);
		((CComboBox *) GetDlgItem(IDC_RECORDKEY))->SetCurSel(nx);
		return;

	}
		
	if (vcode==keyRecordEndLocal) {
		
		//MessageBox("This key is used by the Stop Key");	
		MessageOut(this->m_hWnd,IDS_STRING_KEYSTOP ,IDS_STRING_NOTE,MB_OK | MB_ICONEXCLAMATION);
		
		int nx = GetIndex(keyRecordStartLocal);
		((CComboBox *) GetDlgItem(IDC_RECORDKEY))->SetCurSel(nx);
		return;
	}
	

	keyRecordStartLocal = vcode;
	*/
	
}

void Keyshortcuts::OnSelchangeNextkey() 
{
	// TODO: Add your control notification handler code here
	int index = ((CComboBox *) GetDlgItem(IDC_NEXTKEY))->GetCurSel();
	UINT vcode = GetCode(index);

	keyNextLocal = vcode;
	
}

void Keyshortcuts::OnSelchangePrevkey() 
{
	// TODO: Add your control notification handler code here
	int index = ((CComboBox *) GetDlgItem(IDC_PREVKEY))->GetCurSel();
	UINT vcode = GetCode(index);

	keyPrevLocal = vcode;
	
}

void Keyshortcuts::OnSelchangeShowkey() 
{
	// TODO: Add your control notification handler code here
	int index = ((CComboBox *) GetDlgItem(IDC_SHOWKEY))->GetCurSel();
	UINT vcode = GetCode(index);

	keyShowLayoutLocal = vcode;
	
}

void Keyshortcuts::OnCtrl1() 
{
	// TODO: Add your control notification handler code here
	int val = ((CButton *) GetDlgItem(IDC_CTRL1))->GetCheck();
	keyRecordStartLocalCtrl = val;
	
}

void Keyshortcuts::OnCtrl2() 
{
	// TODO: Add your control notification handler code here
	int val = ((CButton *) GetDlgItem(IDC_CTRL2))->GetCheck();
	keyRecordEndLocalCtrl = val;
	
	
}

void Keyshortcuts::OnCtrl3() 
{
	// TODO: Add your control notification handler code here
	int val = ((CButton *) GetDlgItem(IDC_CTRL3))->GetCheck();
	keyRecordCancelLocalCtrl = val;
	
	
}

void Keyshortcuts::OnCtrl4() 
{
	// TODO: Add your control notification handler code here
	int val = ((CButton *) GetDlgItem(IDC_CTRL4))->GetCheck();
	keyNextLocalCtrl = val;
	
	
}

void Keyshortcuts::OnCtrl5() 
{
	// TODO: Add your control notification handler code here
	int val = ((CButton *) GetDlgItem(IDC_CTRL5))->GetCheck();
	keyPrevLocalCtrl = val;
	
	
}

void Keyshortcuts::OnCtrl6() 
{
	// TODO: Add your control notification handler code here
	int val = ((CButton *) GetDlgItem(IDC_CTRL6))->GetCheck();
	keyShowLayoutLocalCtrl = val;
	
	
}

void Keyshortcuts::OnShift1() 
{
	// TODO: Add your control notification handler code here
	int val = ((CButton *) GetDlgItem(IDC_SHIFT1))->GetCheck();
	keyRecordStartLocalShift = val;

	
}

void Keyshortcuts::OnShift2() 
{
	// TODO: Add your control notification handler code here
	int val = ((CButton *) GetDlgItem(IDC_SHIFT2))->GetCheck();
	keyRecordEndLocalShift = val;
	
}

void Keyshortcuts::OnShift3() 
{
	// TODO: Add your control notification handler code here
	int val = ((CButton *) GetDlgItem(IDC_SHIFT3))->GetCheck();
	keyRecordCancelLocalShift = val;
	
}

void Keyshortcuts::OnShift4() 
{
	// TODO: Add your control notification handler code here
	int val = ((CButton *) GetDlgItem(IDC_SHIFT4))->GetCheck();
	keyNextLocalShift = val;
	
}

void Keyshortcuts::OnShift5() 
{
	// TODO: Add your control notification handler code here
	int val = ((CButton *) GetDlgItem(IDC_SHIFT5))->GetCheck();
	keyPrevLocalShift = val;
	
}

void Keyshortcuts::OnShift6() 
{
	// TODO: Add your control notification handler code here
	int val = ((CButton *) GetDlgItem(IDC_SHIFT6))->GetCheck();
	keyShowLayoutLocalShift = val;
}

void Keyshortcuts::OnAlt1() 
{
	// TODO: Add your control notification handler code here
	int val = ((CButton *) GetDlgItem(IDC_ALT1))->GetCheck();
	keyRecordStartLocalAlt = val;
	
}

void Keyshortcuts::OnAlt2() 
{
	// TODO: Add your control notification handler code here
	int val = ((CButton *) GetDlgItem(IDC_ALT2))->GetCheck();
	keyRecordEndLocalAlt = val;
	
}

void Keyshortcuts::OnAlt3() 
{
	// TODO: Add your control notification handler code here
	int val = ((CButton *) GetDlgItem(IDC_ALT3))->GetCheck();
	keyRecordCancelLocalAlt = val;
	
}

void Keyshortcuts::OnAlt4() 
{
	// TODO: Add your control notification handler code here
	int val = ((CButton *) GetDlgItem(IDC_ALT4))->GetCheck();
	keyNextLocalAlt = val;
	
}

void Keyshortcuts::OnAlt5() 
{
	// TODO: Add your control notification handler code here
	int val = ((CButton *) GetDlgItem(IDC_ALT5))->GetCheck();
	keyPrevLocalAlt = val;
	
}

void Keyshortcuts::OnAlt6() 
{
	// TODO: Add your control notification handler code here
	int val = ((CButton *) GetDlgItem(IDC_ALT6))->GetCheck();
	keyShowLayoutLocalAlt = val;
	
}
