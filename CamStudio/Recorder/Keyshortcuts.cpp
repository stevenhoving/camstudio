// CKeyshortcutsDlg.cpp : implementation file
//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Recorder.h"
#include "Keyshortcuts.h"
#include "HotKey.h"

#include <vector>
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKeyshortcutsDlg dialog

CKeyshortcutsDlg::CKeyshortcutsDlg(CWnd* pParent /*=NULL*/)
: CDialog(CKeyshortcutsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CKeyshortcutsDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_iNumKeys = 6;
	m_iNumSpecial = 3;
	m_vKeyCode.push_back(VK_F1);
	m_vKeyCode.push_back(VK_F2);
	m_vKeyCode.push_back(VK_F3);
	m_vKeyCode.push_back(VK_F4);
	m_vKeyCode.push_back(VK_F5);
	m_vKeyCode.push_back(VK_F6);
	m_vKeyCode.push_back(VK_F7);
	m_vKeyCode.push_back(VK_F8);
	m_vKeyCode.push_back(VK_F9);
	m_vKeyCode.push_back(VK_F10);
	m_vKeyCode.push_back(VK_F11);
	m_vKeyCode.push_back(VK_F12);
	m_vKeyCode.push_back('A');
	m_vKeyCode.push_back('B');
	m_vKeyCode.push_back('C');
	m_vKeyCode.push_back('D');
	m_vKeyCode.push_back('E');
	m_vKeyCode.push_back('F');
	m_vKeyCode.push_back('G');
	m_vKeyCode.push_back('H');
	m_vKeyCode.push_back('I');
	m_vKeyCode.push_back('J');
	m_vKeyCode.push_back('K');
	m_vKeyCode.push_back('L');
	m_vKeyCode.push_back('M');
	m_vKeyCode.push_back('N');
	m_vKeyCode.push_back('O');
	m_vKeyCode.push_back('P');
	m_vKeyCode.push_back('Q');
	m_vKeyCode.push_back('R');
	m_vKeyCode.push_back('S');
	m_vKeyCode.push_back('T');
	m_vKeyCode.push_back('U');
	m_vKeyCode.push_back('V');
	m_vKeyCode.push_back('W');
	m_vKeyCode.push_back('X');
	m_vKeyCode.push_back('Y');
	m_vKeyCode.push_back('Z');
	m_vKeyCode.push_back('1');
	m_vKeyCode.push_back('2');
	m_vKeyCode.push_back('3');
	m_vKeyCode.push_back('4');
	m_vKeyCode.push_back('5');
	m_vKeyCode.push_back('6');
	m_vKeyCode.push_back('7');
	m_vKeyCode.push_back('8');
	m_vKeyCode.push_back('9');
	m_vKeyCode.push_back('0');
	m_vKeyCode.push_back(VK_NUMPAD0);
	m_vKeyCode.push_back(VK_NUMPAD1);
	m_vKeyCode.push_back(VK_NUMPAD2);
	m_vKeyCode.push_back(VK_NUMPAD3);
	m_vKeyCode.push_back(VK_NUMPAD4);
	m_vKeyCode.push_back(VK_NUMPAD5);
	m_vKeyCode.push_back(VK_NUMPAD6);
	m_vKeyCode.push_back(VK_NUMPAD7);
	m_vKeyCode.push_back(VK_NUMPAD8);
	m_vKeyCode.push_back(VK_NUMPAD9);
	m_vKeyCode.push_back(VK_HOME);
	m_vKeyCode.push_back(VK_INSERT);
	m_vKeyCode.push_back(VK_DELETE);
	m_vKeyCode.push_back(VK_END);
	m_vKeyCode.push_back(VK_RETURN);
	m_vKeyCode.push_back(VK_SPACE);
	m_vKeyCode.push_back(VK_TAB);
	m_vKeyCode.push_back(100000);	// sentinel value; must be last
}

void CKeyshortcutsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKeyshortcutsDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_CTRL1, m_ctrlButtonRecordCTRL);
	DDX_Control(pDX, IDC_ALT1, m_ctrlButtonRecordALT);
	DDX_Control(pDX, IDC_SHIFT1, m_ctrlButtonRecordSHFT);
	DDX_Control(pDX, IDC_CTRL2, m_ctrlButtonStopCTRL);
	DDX_Control(pDX, IDC_ALT2, m_ctrlButtonStopALT);
	DDX_Control(pDX, IDC_SHIFT2, m_ctrlButtonStopSHFT);
	DDX_Control(pDX, IDC_CTRL3, m_ctrlButtonCancelCTRL);
	DDX_Control(pDX, IDC_ALT3, m_ctrlButtonCancelALT);
	DDX_Control(pDX, IDC_SHIFT3, m_ctrlButtonCancelSHFT);
	DDX_Control(pDX, IDC_CTRL4, m_ctrlButtonNextCTRL);
	DDX_Control(pDX, IDC_ALT4, m_ctrlButtonNextALT);
	DDX_Control(pDX, IDC_SHIFT4, m_ctrlButtonNextSHFT);
	DDX_Control(pDX, IDC_CTRL5, m_ctrlButtonPrevCTRL);
	DDX_Control(pDX, IDC_ALT5, m_ctrlButtonPrevALT);
	DDX_Control(pDX, IDC_SHIFT5, m_ctrlButtonPrevSHFT);
	DDX_Control(pDX, IDC_CTRL6, m_ctrlButtonShowCTRL);
	DDX_Control(pDX, IDC_ALT6, m_ctrlButtonShowALT);
	DDX_Control(pDX, IDC_SHIFT6, m_ctrlButtonShowCHFT);
	DDX_Control(pDX, IDC_RECORDKEY, m_ctrlCBRecord);
	DDX_Control(pDX, IDC_STOPKEY, m_ctrlCBStop);
	DDX_Control(pDX, IDC_CANCELKEY, m_ctrlCBCancel);
	DDX_Control(pDX, IDC_NEXTKEY, m_ctrlCBNext);
	DDX_Control(pDX, IDC_PREVKEY, m_ctrlCBPrev);
	DDX_Control(pDX, IDC_SHOWKEY, m_ctrlCBShow);
}

BEGIN_MESSAGE_MAP(CKeyshortcutsDlg, CDialog)
	//{{AFX_MSG_MAP(CKeyshortcutsDlg)
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
// CKeyshortcutsDlg message handlers

int CKeyshortcutsDlg::TestKeysOverlap(int& o1, int& o2)
{
	m_iKey[0] = m_keyRecordStart;
	m_iKey[1] = m_keyRecordEnd;
	m_iKey[2] = m_keyRecordCancel;
	m_iKey[3] = m_keyNext;
	m_iKey[4] = m_keyPrev;
	m_iKey[5] = m_keyShowLayout;

	m_iKeySpecial[0][0] = m_keyRecordStartCtrl;
	m_iKeySpecial[1][0] = m_keyRecordEndCtrl;
	m_iKeySpecial[2][0] = m_keyRecordCancelCtrl;
	m_iKeySpecial[3][0] = m_keyNextCtrl;
	m_iKeySpecial[4][0] = m_keyPrevCtrl;
	m_iKeySpecial[5][0] = m_keyShowLayoutCtrl;

	m_iKeySpecial[0][1] = m_keyRecordStartAlt;
	m_iKeySpecial[1][1] = m_keyRecordEndAlt;
	m_iKeySpecial[2][1] = m_keyRecordCancelAlt;
	m_iKeySpecial[3][1] = m_keyNextAlt;
	m_iKeySpecial[4][1] = m_keyPrevAlt;
	m_iKeySpecial[5][1] = m_keyShowLayoutAlt;

	m_iKeySpecial[0][2] = m_keyRecordStartShift;
	m_iKeySpecial[1][2] = m_keyRecordEndShift;
	m_iKeySpecial[2][2] = m_keyRecordCancelShift;
	m_iKeySpecial[3][2] = m_keyNextShift;
	m_iKeySpecial[4][2] = m_keyPrevShift;
	m_iKeySpecial[5][2] = m_keyShowLayoutShift;

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

	for (int i = 0; i < m_iNumKeys; i++)
	{
		for (int j = 0; j < m_iNumKeys; j++)
		{
			if (i != j)
			{
				if (m_iKey[i] == m_iKey[j])
				{
					bool bStatusSpecial = false;
					for (int k = 0; k < m_iNumSpecial; k++)
					{
						if (m_iKeySpecial[i][k] != m_iKeySpecial[j][k])
						{
							bStatusSpecial = true;
							break;	// finished; mismatch found
						}
					}
					if (!bStatusSpecial)
					{
						o1 = i;
						o2 = j;
						return 1;
					}
				}
			}
		}
	}

	return 0;
}

void CKeyshortcutsDlg::OnOK()
{
	// validation

	int key1 = 0;
	int key2 = 0;
	if (TestKeysOverlap(key1, key2))
	{
		if ((0 <= key1) && (key1 < m_iNumKeys) && (key2 >= 0) && (key2 < m_iNumKeys))
		{
			CString errStr;
			CString formatStr;
			CString noteStr;
			formatStr.LoadString(IDS_STRINGSAMEKEY);
			errStr.Format(formatStr,keyName[key1],keyName[key2]);

			noteStr.LoadString(IDS_STRING_NOTE);
			MessageBox(errStr, noteStr ,MB_OK | MB_ICONEXCLAMATION);

			//MessageBox(errStr, "Note",MB_OK | MB_ICONEXCLAMATION);

			return;
		}
	}

	cHotKeyOpts.m_RecordStart.m_vKey = m_keyRecordStart;
	cHotKeyOpts.m_RecordEnd.m_vKey = m_keyRecordEnd;
	cHotKeyOpts.m_RecordCancel.m_vKey = m_keyRecordCancel;

	cHotKeyOpts.m_Next.m_vKey = m_keyNext;
	cHotKeyOpts.m_Prev.m_vKey = m_keyPrev;
	cHotKeyOpts.m_ShowLayout.m_vKey = m_keyShowLayout;

	cHotKeyOpts.m_RecordStart.m_bCtrl = m_keyRecordStartCtrl;
	cHotKeyOpts.m_RecordEnd.m_bCtrl = m_keyRecordEndCtrl;
	cHotKeyOpts.m_RecordCancel.m_bCtrl = m_keyRecordCancelCtrl;

	cHotKeyOpts.m_Next.m_bCtrl = m_keyNextCtrl;
	cHotKeyOpts.m_Prev.m_bCtrl = m_keyPrevCtrl;
	cHotKeyOpts.m_ShowLayout.m_bCtrl = m_keyShowLayoutCtrl;

	cHotKeyOpts.m_RecordStart.m_bAlt = m_keyRecordStartAlt;
	cHotKeyOpts.m_RecordEnd.m_bAlt = m_keyRecordEndAlt;
	cHotKeyOpts.m_RecordCancel.m_bAlt = m_keyRecordCancelAlt;

	cHotKeyOpts.m_Next.m_bAlt = m_keyNextAlt;
	cHotKeyOpts.m_Prev.m_bAlt = m_keyPrevAlt;
	cHotKeyOpts.m_ShowLayout.m_bAlt = m_keyShowLayoutAlt;

	cHotKeyOpts.m_RecordStart.m_bShift = m_keyRecordStartShift;
	cHotKeyOpts.m_RecordEnd.m_bShift = m_keyRecordEndShift;
	cHotKeyOpts.m_RecordCancel.m_bShift = m_keyRecordCancelShift;

	cHotKeyOpts.m_Next.m_bShift = m_keyNextShift;
	cHotKeyOpts.m_Prev.m_bShift = m_keyPrevShift;
	cHotKeyOpts.m_ShowLayout.m_bShift = m_keyShowLayoutShift;

	CDialog::OnOK();
}

BOOL CKeyshortcutsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here
	m_keyRecordStart = cHotKeyOpts.m_RecordStart.m_vKey;
	m_keyRecordEnd = cHotKeyOpts.m_RecordEnd.m_vKey;
	m_keyRecordCancel = cHotKeyOpts.m_RecordCancel.m_vKey;

	m_keyRecordStartCtrl = cHotKeyOpts.m_RecordStart.m_bCtrl;
	m_keyRecordEndCtrl = cHotKeyOpts.m_RecordEnd.m_bCtrl;
	m_keyRecordCancelCtrl = cHotKeyOpts.m_RecordCancel.m_bCtrl;

	m_keyRecordStartAlt = cHotKeyOpts.m_RecordStart.m_bAlt;
	m_keyRecordEndAlt = cHotKeyOpts.m_RecordEnd.m_bAlt;
	m_keyRecordCancelAlt = cHotKeyOpts.m_RecordCancel.m_bAlt;

	m_keyRecordStartShift = cHotKeyOpts.m_RecordStart.m_bShift;
	m_keyRecordEndShift = cHotKeyOpts.m_RecordEnd.m_bShift;
	m_keyRecordCancelShift = cHotKeyOpts.m_RecordCancel.m_bShift;

	m_keyNext = cHotKeyOpts.m_Next.m_vKey;
	m_keyPrev = cHotKeyOpts.m_Prev.m_vKey;
	m_keyShowLayout = cHotKeyOpts.m_ShowLayout.m_vKey;

	m_keyNextCtrl = cHotKeyOpts.m_Next.m_bCtrl;
	m_keyPrevCtrl = cHotKeyOpts.m_Prev.m_bCtrl;
	m_keyShowLayoutCtrl = cHotKeyOpts.m_ShowLayout.m_bCtrl;

	m_keyNextAlt = cHotKeyOpts.m_Next.m_bAlt;
	m_keyPrevAlt = cHotKeyOpts.m_Prev.m_bAlt;
	m_keyShowLayoutAlt = cHotKeyOpts.m_ShowLayout.m_bAlt;

	m_keyNextShift = cHotKeyOpts.m_Next.m_bShift;
	m_keyPrevShift = cHotKeyOpts.m_Prev.m_bShift;
	m_keyShowLayoutShift = cHotKeyOpts.m_ShowLayout.m_bShift;

	m_ctrlCBRecord.SetCurSel(GetIndex(cHotKeyOpts.m_RecordStart.m_vKey));
	m_ctrlCBStop.SetCurSel(GetIndex(cHotKeyOpts.m_RecordEnd.m_vKey));
	m_ctrlCBCancel.SetCurSel(GetIndex(cHotKeyOpts.m_RecordCancel.m_vKey));
	m_ctrlCBNext.SetCurSel(GetIndex(cHotKeyOpts.m_Next.m_vKey));
	m_ctrlCBPrev.SetCurSel(GetIndex(cHotKeyOpts.m_Prev.m_vKey));
	m_ctrlCBShow.SetCurSel(GetIndex(cHotKeyOpts.m_ShowLayout.m_vKey));

	m_ctrlButtonRecordCTRL.SetCheck(cHotKeyOpts.m_RecordStart.m_bCtrl);
	m_ctrlButtonStopCTRL.SetCheck(cHotKeyOpts.m_RecordEnd.m_bCtrl);
	m_ctrlButtonCancelCTRL.SetCheck(cHotKeyOpts.m_RecordCancel.m_bCtrl);
	m_ctrlButtonNextCTRL.SetCheck(cHotKeyOpts.m_Next.m_bCtrl);
	m_ctrlButtonPrevCTRL.SetCheck(cHotKeyOpts.m_Prev.m_bCtrl);
	m_ctrlButtonShowCTRL.SetCheck(cHotKeyOpts.m_ShowLayout.m_bCtrl);

	m_ctrlButtonRecordALT.SetCheck(cHotKeyOpts.m_RecordStart.m_bAlt);
	m_ctrlButtonStopALT.SetCheck(cHotKeyOpts.m_RecordEnd.m_bAlt);
	m_ctrlButtonCancelALT.SetCheck(cHotKeyOpts.m_RecordCancel.m_bAlt);
	m_ctrlButtonNextALT.SetCheck(cHotKeyOpts.m_Next.m_bAlt);
	m_ctrlButtonPrevALT.SetCheck(cHotKeyOpts.m_Prev.m_bAlt);
	m_ctrlButtonShowALT.SetCheck(cHotKeyOpts.m_ShowLayout.m_bAlt);

	m_ctrlButtonRecordSHFT.SetCheck(cHotKeyOpts.m_RecordStart.m_bShift);
	m_ctrlButtonStopSHFT.SetCheck(cHotKeyOpts.m_RecordEnd.m_bShift);
	m_ctrlButtonCancelSHFT.SetCheck(cHotKeyOpts.m_RecordCancel.m_bShift);
	m_ctrlButtonNextSHFT.SetCheck(cHotKeyOpts.m_Next.m_bShift);
	m_ctrlButtonPrevSHFT.SetCheck(cHotKeyOpts.m_Prev.m_bShift);
	m_ctrlButtonShowCHFT.SetCheck(cHotKeyOpts.m_ShowLayout.m_bShift);

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

int CKeyshortcutsDlg::GetIndex(UINT keyShortCut)
{	
	std::vector <UINT>::iterator iter = std::find(m_vKeyCode.begin(), m_vKeyCode.end(), keyShortCut);
	return (iter - m_vKeyCode.begin());
}


UINT CKeyshortcutsDlg::GetCode(size_t index)
{
	return ((index < 0) || (m_vKeyCode.size() <= index))
		? m_vKeyCode[m_vKeyCode.size()-1]
		: m_vKeyCode[index];
}

void CKeyshortcutsDlg::OnSelchangeStopkey()
{
	m_keyRecordEnd = GetCode(m_ctrlCBStop.GetCurSel());
}

void CKeyshortcutsDlg::OnSelchangeCancelkey()
{
	m_keyRecordCancel = GetCode(m_ctrlCBCancel.GetCurSel());
}

void CKeyshortcutsDlg::OnSelchangeRecordkey()
{
	m_keyRecordStart = GetCode(m_ctrlCBRecord.GetCurSel());
}

void CKeyshortcutsDlg::OnSelchangeNextkey()
{
	m_keyNext = GetCode(m_ctrlCBNext.GetCurSel());
}

void CKeyshortcutsDlg::OnSelchangePrevkey()
{
	m_keyPrev = GetCode(m_ctrlCBPrev.GetCurSel());
}

void CKeyshortcutsDlg::OnSelchangeShowkey()
{
	m_keyShowLayout = GetCode(m_ctrlCBShow.GetCurSel());
}

void CKeyshortcutsDlg::OnCtrl1()
{
	m_keyRecordStartCtrl = m_ctrlButtonRecordCTRL.GetCheck();
}

void CKeyshortcutsDlg::OnCtrl2()
{
	m_keyRecordEndCtrl = m_ctrlButtonStopCTRL.GetCheck();
}

void CKeyshortcutsDlg::OnCtrl3()
{
	m_keyRecordCancelCtrl = m_ctrlButtonCancelCTRL.GetCheck();
}

void CKeyshortcutsDlg::OnCtrl4()
{
	m_keyNextCtrl = m_ctrlButtonNextCTRL.GetCheck();
}

void CKeyshortcutsDlg::OnCtrl5()
{
	m_keyPrevCtrl = m_ctrlButtonPrevCTRL.GetCheck();
}

void CKeyshortcutsDlg::OnCtrl6()
{
	m_keyShowLayoutCtrl = m_ctrlButtonShowCTRL.GetCheck();
}

void CKeyshortcutsDlg::OnShift1()
{
	m_keyRecordStartShift = m_ctrlButtonRecordSHFT.GetCheck();
}

void CKeyshortcutsDlg::OnShift2()
{
	m_keyRecordEndShift = m_ctrlButtonStopSHFT.GetCheck();
}

void CKeyshortcutsDlg::OnShift3()
{
	m_keyRecordCancelShift = m_ctrlButtonCancelSHFT.GetCheck();
}

void CKeyshortcutsDlg::OnShift4()
{
	m_keyNextShift = m_ctrlButtonNextSHFT.GetCheck();
}

void CKeyshortcutsDlg::OnShift5()
{
	m_keyPrevShift = m_ctrlButtonPrevSHFT.GetCheck();
}

void CKeyshortcutsDlg::OnShift6()
{
	m_keyShowLayoutShift = m_ctrlButtonShowCHFT.GetCheck();
}

void CKeyshortcutsDlg::OnAlt1()
{
	m_keyRecordStartAlt = m_ctrlButtonRecordALT.GetCheck();
}

void CKeyshortcutsDlg::OnAlt2()
{
	m_keyRecordEndAlt = m_ctrlButtonStopALT.GetCheck();
}

void CKeyshortcutsDlg::OnAlt3()
{
	m_keyRecordCancelAlt = m_ctrlButtonCancelALT.GetCheck();
}

void CKeyshortcutsDlg::OnAlt4()
{
	m_keyNextAlt = m_ctrlButtonNextALT.GetCheck();
}

void CKeyshortcutsDlg::OnAlt5()
{
	m_keyPrevAlt = m_ctrlButtonPrevALT.GetCheck();
}

void CKeyshortcutsDlg::OnAlt6()
{
	m_keyShowLayoutAlt = m_ctrlButtonShowALT.GetCheck();
}
