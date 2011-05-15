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
	// TODO: let's use Item Data
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
// TODO: If we still want to check HK overlap
// Let's reorganize m_ctrlXXXX into array
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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKeyshortcutsDlg message handlers

void CKeyshortcutsDlg::OnOK()
{
	CDialog::OnOK(); // RTFM: UpdateData() is called for us

	// validation
	// who cares

	// why don't we use Item Data here instead of conversion?

	cHotKeyOpts.m_RecordStart.m_vKey = GetCode(m_ctrlCBRecord.GetCurSel());// m_keyRecordStart;
	cHotKeyOpts.m_RecordEnd.m_vKey = GetCode(m_ctrlCBStop.GetCurSel());// m_keyRecordEnd;
	cHotKeyOpts.m_RecordCancel.m_vKey = GetCode(m_ctrlCBCancel.GetCurSel());// m_keyRecordCancel;

	cHotKeyOpts.m_Next.m_vKey = GetCode(m_ctrlCBNext.GetCurSel());// m_keyNext;
	cHotKeyOpts.m_Prev.m_vKey = GetCode(m_ctrlCBPrev.GetCurSel());// m_keyPrev;
	cHotKeyOpts.m_ShowLayout.m_vKey = GetCode(m_ctrlCBShow.GetCurSel());// m_ctrlm_keyShowLayout;

	cHotKeyOpts.m_RecordStart.m_fsMod =
		MOD_ALT * (m_ctrlButtonRecordALT.GetCheck() == BST_CHECKED) |
		MOD_CONTROL * (m_ctrlButtonRecordCTRL.GetCheck() == BST_CHECKED) |
		MOD_SHIFT * (m_ctrlButtonRecordSHFT.GetCheck() == BST_CHECKED);
	cHotKeyOpts.m_RecordEnd.m_fsMod =
		MOD_ALT * (m_ctrlButtonStopALT.GetCheck() == BST_CHECKED) |
		MOD_CONTROL * (m_ctrlButtonStopCTRL.GetCheck() == BST_CHECKED) |
		MOD_SHIFT * (m_ctrlButtonStopSHFT.GetCheck() == BST_CHECKED);
	cHotKeyOpts.m_RecordCancel.m_fsMod =
		MOD_ALT * (m_ctrlButtonCancelALT.GetCheck() == BST_CHECKED) |
		MOD_CONTROL * (m_ctrlButtonCancelCTRL.GetCheck() == BST_CHECKED) |
		MOD_SHIFT * (m_ctrlButtonCancelSHFT.GetCheck() == BST_CHECKED);

	cHotKeyOpts.m_Next.m_fsMod =
		MOD_ALT * (m_ctrlButtonNextALT.GetCheck() == BST_CHECKED) |
		MOD_CONTROL * (m_ctrlButtonNextCTRL.GetCheck() == BST_CHECKED) |
		MOD_SHIFT * (m_ctrlButtonNextSHFT.GetCheck() == BST_CHECKED);
	cHotKeyOpts.m_Prev.m_fsMod = 
		MOD_ALT * (m_ctrlButtonPrevALT.GetCheck() == BST_CHECKED) |
		MOD_CONTROL * (m_ctrlButtonPrevCTRL.GetCheck() == BST_CHECKED) |
		MOD_SHIFT * (m_ctrlButtonPrevSHFT.GetCheck() == BST_CHECKED);
	cHotKeyOpts.m_ShowLayout.m_fsMod =
		MOD_ALT * (m_ctrlButtonShowALT.GetCheck() == BST_CHECKED) |
		MOD_CONTROL * (m_ctrlButtonShowCTRL.GetCheck() == BST_CHECKED) |
		MOD_SHIFT * (m_ctrlButtonShowCHFT.GetCheck() == BST_CHECKED);
}

BOOL CKeyshortcutsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ctrlCBRecord.SetCurSel(GetIndex(cHotKeyOpts.m_RecordStart.m_vKey));
	m_ctrlCBStop.SetCurSel(GetIndex(cHotKeyOpts.m_RecordEnd.m_vKey));
	m_ctrlCBCancel.SetCurSel(GetIndex(cHotKeyOpts.m_RecordCancel.m_vKey));
	m_ctrlCBNext.SetCurSel(GetIndex(cHotKeyOpts.m_Next.m_vKey));
	m_ctrlCBPrev.SetCurSel(GetIndex(cHotKeyOpts.m_Prev.m_vKey));
	m_ctrlCBShow.SetCurSel(GetIndex(cHotKeyOpts.m_ShowLayout.m_vKey));

	m_ctrlButtonRecordCTRL.SetCheck((cHotKeyOpts.m_RecordStart.m_fsMod & MOD_CONTROL) > 0);
	m_ctrlButtonStopCTRL.SetCheck((cHotKeyOpts.m_RecordEnd.m_fsMod & MOD_CONTROL) > 0);
	m_ctrlButtonCancelCTRL.SetCheck((cHotKeyOpts.m_RecordCancel.m_fsMod & MOD_CONTROL) > 0);
	m_ctrlButtonNextCTRL.SetCheck((cHotKeyOpts.m_Next.m_fsMod & MOD_CONTROL) > 0);
	m_ctrlButtonPrevCTRL.SetCheck((cHotKeyOpts.m_Prev.m_fsMod & MOD_CONTROL) > 0);
	m_ctrlButtonShowCTRL.SetCheck((cHotKeyOpts.m_ShowLayout.m_fsMod & MOD_CONTROL) > 0);

	m_ctrlButtonRecordALT.SetCheck((cHotKeyOpts.m_RecordStart.m_fsMod & MOD_ALT) > 0);
	m_ctrlButtonStopALT.SetCheck((cHotKeyOpts.m_RecordEnd.m_fsMod & MOD_ALT) > 0);
	m_ctrlButtonCancelALT.SetCheck((cHotKeyOpts.m_RecordCancel.m_fsMod) > 0);
	m_ctrlButtonNextALT.SetCheck((cHotKeyOpts.m_Next.m_fsMod & MOD_ALT) > 0);
	m_ctrlButtonPrevALT.SetCheck((cHotKeyOpts.m_Prev.m_fsMod & MOD_ALT) > 0);
	m_ctrlButtonShowALT.SetCheck((cHotKeyOpts.m_ShowLayout.m_fsMod & MOD_ALT) > 0);

	m_ctrlButtonRecordSHFT.SetCheck((cHotKeyOpts.m_RecordStart.m_fsMod & MOD_SHIFT) > 0);
	m_ctrlButtonStopSHFT.SetCheck((cHotKeyOpts.m_RecordEnd.m_fsMod & MOD_SHIFT) > 0);
	m_ctrlButtonCancelSHFT.SetCheck((cHotKeyOpts.m_RecordCancel.m_fsMod & MOD_SHIFT) > 0);
	m_ctrlButtonNextSHFT.SetCheck((cHotKeyOpts.m_Next.m_fsMod & MOD_SHIFT) > 0);
	m_ctrlButtonPrevSHFT.SetCheck((cHotKeyOpts.m_Prev.m_fsMod & MOD_SHIFT) > 0);
	m_ctrlButtonShowCHFT.SetCheck((cHotKeyOpts.m_ShowLayout.m_fsMod & MOD_SHIFT) > 0);

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
