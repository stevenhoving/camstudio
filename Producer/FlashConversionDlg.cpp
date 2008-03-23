// FlashConversionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "playplus.h"
#include "FlashConversionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CString GetProgPath();
extern void MsgC(const char fmt[], ...);

extern int noLoop;
extern int noAutoPlay;
extern int convertBits;
extern CString swfname;
extern CString swfbasename; 
extern CString swfhtmlname;

extern int Max_HalfKeyDepth;
extern int sampleFPS;
extern int useAudio;
extern int useAudioCompression;
extern int addControls;
extern int addPreloader;
extern int produceRaw;

CString m_newbasefile;
CString m_newbaseHTMLfile;

extern HINSTANCE   ghInstApp;

#ifndef BUFSIZE
	#define BUFSIZE 300
#endif

/////////////////////////////////////////////////////////////////////////////
// FlashConversionDlg property page

IMPLEMENT_DYNCREATE(FlashConversionDlg, CPropertyPage)

FlashConversionDlg::FlashConversionDlg() : CPropertyPage(FlashConversionDlg::IDD)
{
	//{{AFX_DATA_INIT(FlashConversionDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

FlashConversionDlg::~FlashConversionDlg()
{
}

void FlashConversionDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(FlashConversionDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(FlashConversionDlg, CPropertyPage)
	//{{AFX_MSG_MAP(FlashConversionDlg)
	ON_BN_CLICKED(IDC_SELECTFLASH, OnSelectflash)
	ON_BN_CLICKED(IDC_SELECTHTML, OnSelecthtml)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RAW, OnRaw)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FlashConversionDlg message handlers

BOOL FlashConversionDlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	// TODO: Add extra initialization here
	((CEdit *)GetDlgItem(IDC_BASENAME))->SetWindowText(swfbasename);
	((CEdit *)GetDlgItem(IDC_FLASHNAME))->SetWindowText(swfname);
	((CEdit *)GetDlgItem(IDC_HTMLNAME))->SetWindowText(swfhtmlname);
		

	if (convertBits == 32)
	{
		((CButton *) GetDlgItem(IDC_RADIO1))->SetCheck(FALSE);
		((CButton *) GetDlgItem(IDC_RADIO2))->SetCheck(TRUE);

	}
	else
	{
		((CButton *) GetDlgItem(IDC_RADIO2))->SetCheck(FALSE);
		((CButton *) GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);

	}


	int sampleFPSIndex=8;
	if (sampleFPS <=5 )
		sampleFPSIndex = 0;
	else if (sampleFPS <=10 )
		sampleFPSIndex = 1;
	else if (sampleFPS <=12 )
		sampleFPSIndex = 2;
	else if (sampleFPS <=15 )
		sampleFPSIndex = 3;
	else if (sampleFPS <=20 )
		sampleFPSIndex = 4;
	else if (sampleFPS <=25 )
		sampleFPSIndex = 5;
	else if (sampleFPS <=30 )
		sampleFPSIndex = 6;
	else if (sampleFPS <=35 )
		sampleFPSIndex = 7;
	else
		sampleFPSIndex = 8;
	((CComboBox *) GetDlgItem(IDC_SAMPLEFPS))->SetCurSel(sampleFPSIndex);

	int keyframeIndex = 3;
	if (Max_HalfKeyDepth <=5 )
		keyframeIndex = 0;
	else if (Max_HalfKeyDepth <=10 )
		keyframeIndex = 1;
	else if (Max_HalfKeyDepth <=12 )
		keyframeIndex = 2;
	else if (Max_HalfKeyDepth <=15 )
		keyframeIndex = 3;
	else 
		keyframeIndex = 4;		
	((CComboBox *) GetDlgItem(IDC_KEYFRAMERATE))->SetCurSel(keyframeIndex);
	
	if (useAudio)
		((CButton *) GetDlgItem(IDC_ENCODEAUDIO))->SetCheck(TRUE);
	else
		((CButton *) GetDlgItem(IDC_ENCODEAUDIO))->SetCheck(FALSE);

	((CComboBox *) GetDlgItem(IDC_AUDIOCOMPRESSION))->SetCurSel(useAudioCompression);


	if (addControls)
		((CButton *) GetDlgItem(IDC_ADDCONTROLS))->SetCheck(TRUE);
	else
		((CButton *) GetDlgItem(IDC_ADDCONTROLS))->SetCheck(FALSE);

	if (noLoop)
		((CButton *) GetDlgItem(IDC_LOOP))->SetCheck(FALSE);
	else
		((CButton *) GetDlgItem(IDC_LOOP))->SetCheck(TRUE);

	if (noAutoPlay)
		((CButton *) GetDlgItem(IDC_AUTOSTART))->SetCheck(FALSE);
	else
		((CButton *) GetDlgItem(IDC_AUTOSTART))->SetCheck(TRUE);

	if (addPreloader)
		((CButton *) GetDlgItem(IDC_ADDPRELOADER))->SetCheck(TRUE);
	else
		((CButton *) GetDlgItem(IDC_ADDPRELOADER))->SetCheck(FALSE);

	
	UpdateBehavior(produceRaw);
	if (produceRaw)
		((CButton *) GetDlgItem(IDC_RAW))->SetCheck(TRUE);
	else
		((CButton *) GetDlgItem(IDC_RAW))->SetCheck(FALSE);
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void FlashConversionDlg::OnOK() 
{
	// TODO: Add extra validation here	

	((CEdit *)GetDlgItem(IDC_BASENAME))->GetWindowText(swfbasename);
	((CEdit *)GetDlgItem(IDC_FLASHNAME))->GetWindowText(swfname);
	((CEdit *)GetDlgItem(IDC_HTMLNAME))->GetWindowText(swfhtmlname);
	
	FILE* testSWF = fopen(LPCTSTR(swfname),"wb");
	if (testSWF==NULL)
	{
		CString msg, title, formatstr;
		formatstr.LoadString(IDS_UCFS);
		msg.Format(formatstr,LPCTSTR(swfname));
		title.LoadString(IDS_NOTE);		
		MessageBox(msg,title,MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	else
	{	
		fclose(testSWF);
		DeleteFile(swfname);

	}

	int val = ((CButton *) GetDlgItem(IDC_RADIO1))->GetCheck();
	if (val)
		convertBits = 16;
	else
		convertBits = 32;

	int sampleFPSIndex = ((CComboBox *) GetDlgItem(IDC_SAMPLEFPS))->GetCurSel();	 
	if	(sampleFPSIndex == 0)
		sampleFPS =5;
	else if	(sampleFPSIndex == 1)
		sampleFPS =10;
	else if	(sampleFPSIndex == 2)
		sampleFPS =12;
	else if	(sampleFPSIndex == 3)
		sampleFPS =15;
	else if	(sampleFPSIndex == 4)
		sampleFPS =20;
	else if	(sampleFPSIndex == 5)
		sampleFPS =25;
	else if	(sampleFPSIndex == 6)
		sampleFPS =30;
	else if	(sampleFPSIndex == 7)
		sampleFPS =35;
	else if	(sampleFPSIndex == 8)
		sampleFPS =40;

	int keyframeIndex = ((CComboBox *) GetDlgItem(IDC_KEYFRAMERATE))->GetCurSel();	 
	if (keyframeIndex ==0)
		Max_HalfKeyDepth =5; 
	else if (keyframeIndex ==1)
		Max_HalfKeyDepth =10; 
	else if (keyframeIndex ==2)
		Max_HalfKeyDepth =12; 
	else if (keyframeIndex ==3)
		Max_HalfKeyDepth =15; 
	else 
		Max_HalfKeyDepth =20; 

		
	int bVal = ((CButton *) GetDlgItem(IDC_ENCODEAUDIO))->GetCheck();
	if (bVal)
		useAudio = 1;
	else
		useAudio = 0;
		
	useAudioCompression = ((CComboBox *) GetDlgItem(IDC_AUDIOCOMPRESSION))->GetCurSel();


	bVal = ((CButton *) GetDlgItem(IDC_ADDCONTROLS))->GetCheck();
	if (bVal)
		addControls=1;
	else
		addControls=0;

	bVal = ((CButton *) GetDlgItem(IDC_ADDPRELOADER))->GetCheck();
	if (bVal)
		addPreloader=1;
	else
		addPreloader=0;
		

	bVal = ((CButton *) GetDlgItem(IDC_LOOP))->GetCheck();
	if (bVal)
		noLoop=0;
	else
		noLoop=1;
	

	bVal = ((CButton *) GetDlgItem(IDC_AUTOSTART))->GetCheck();
	if (bVal)
		noAutoPlay=0;
	else
		noAutoPlay=1;


	bVal = ((CButton *) GetDlgItem(IDC_RAW))->GetCheck();
	if (bVal)
		produceRaw=1;
	else
		produceRaw=0;
	
	CPropertyPage::OnOK();
}

void FlashConversionDlg::OnSelectflash() 
{
			

	//Prompt the user for the filename
	static char BASED_CODE szFilter[] =	"Flash Movie Files (*.swf)|*.swf||";	
	char szTitle[BUFSIZE];	
	
	LoadString( ghInstApp, IDS_SAVESWF1, szTitle, BUFSIZE );	 		
				
	CFileDialog fdlg(FALSE,"*.swf","*.swf",OFN_LONGNAMES,szFilter,this);	
	fdlg.m_ofn.lpstrTitle=szTitle;	
	
			
	CString m_newfile;
	
	
	if(fdlg.DoModal() == IDOK)
	{
		m_newfile = fdlg.GetPathName();		
		m_newbasefile = fdlg.GetFileName();

		CString m_newHTMLfile = m_newfile +".html";
		((CEdit *)GetDlgItem(IDC_BASENAME))->SetWindowText(m_newbasefile);
		((CEdit *)GetDlgItem(IDC_FLASHNAME))->SetWindowText(m_newfile);
		((CEdit *)GetDlgItem(IDC_HTMLNAME))->SetWindowText(m_newHTMLfile);
		
		//swfname = m_newfile;							
		//swfbasename = m_newbasefile;				

	}
	else {				
		
		return;

	}

				 

	
}

void FlashConversionDlg::OnSelecthtml() 
{
	// TODO: Add your control notification handler code here
	static char BASED_CODE szFilter[] =	"HTML Files (*.html)|*.html||";	
	char szTitle[BUFSIZE];	
	
	LoadString( ghInstApp, IDS_SAVEHTML, szTitle, BUFSIZE );	 		
				
	CFileDialog fdlg(FALSE,"*.html","*.html",OFN_LONGNAMES,szFilter,this);	
	fdlg.m_ofn.lpstrTitle=szTitle;	
	
			
	CString m_newfile;
	
	
	if(fdlg.DoModal() == IDOK)
	{
		m_newfile = fdlg.GetPathName();		
		((CEdit *)GetDlgItem(IDC_HTMLNAME))->SetWindowText(m_newfile);
		
		//swfname = m_newfile;							
		//swfbasename = m_newbasefile;				

	}
	else {				
		
		return;

	}

	
}

void FlashConversionDlg::OnRadio2() 
{
	// TODO: Add your control notification handler code here
	((CButton *) GetDlgItem(IDC_RADIO1))->SetCheck(FALSE);
	((CButton *) GetDlgItem(IDC_RADIO2))->SetCheck(TRUE);

	
}

void FlashConversionDlg::OnRadio1() 
{
	// TODO: Add your control notification handler code here
	((CButton *) GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
	((CButton *) GetDlgItem(IDC_RADIO2))->SetCheck(FALSE);
	
}

void FlashConversionDlg::OnRaw() 
{
	int val = ((CButton *) GetDlgItem(IDC_RAW))->GetCheck();
	UpdateBehavior(val);
	
}

void FlashConversionDlg::UpdateBehavior(int val) 
{

	if (val)
	{
		((CButton *) GetDlgItem(IDC_ADDPRELOADER))->EnableWindow(FALSE);
		((CButton *) GetDlgItem(IDC_ADDCONTROLS))->EnableWindow(FALSE);
		((CButton *) GetDlgItem(IDC_LOOP))->EnableWindow(FALSE);
		((CButton *) GetDlgItem(IDC_AUTOSTART))->EnableWindow(FALSE);	

		((CButton *) GetDlgItem(IDC_ADDPRELOADER))->SetCheck(FALSE);
		((CButton *) GetDlgItem(IDC_ADDCONTROLS))->SetCheck(FALSE);
		((CButton *) GetDlgItem(IDC_LOOP))->SetCheck(TRUE);
		((CButton *) GetDlgItem(IDC_AUTOSTART))->SetCheck(TRUE);	

	}
	else
	{
		((CButton *) GetDlgItem(IDC_ADDPRELOADER))->EnableWindow(TRUE);
		((CButton *) GetDlgItem(IDC_ADDCONTROLS))->EnableWindow(TRUE);
		((CButton *) GetDlgItem(IDC_LOOP))->EnableWindow(TRUE);
		((CButton *) GetDlgItem(IDC_AUTOSTART))->EnableWindow(TRUE);	

		((CButton *) GetDlgItem(IDC_ADDPRELOADER))->SetCheck(TRUE);
		((CButton *) GetDlgItem(IDC_ADDCONTROLS))->SetCheck(TRUE);
		((CButton *) GetDlgItem(IDC_LOOP))->SetCheck(TRUE);
		((CButton *) GetDlgItem(IDC_AUTOSTART))->SetCheck(TRUE);	

	}



}
