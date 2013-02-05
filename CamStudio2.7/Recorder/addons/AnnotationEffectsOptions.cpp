// AnnotationEffectsOptions.cpp : implementation file
// Modifies Timestamp, Caption, and Watermark
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "../Recorder.h"
#include "XnoteStopwatchFormat.h"
#include "AnnotationEffectsOptions.h"

// CAnnotationEffectsOptionsDlg dialog

IMPLEMENT_DYNAMIC(CAnnotationEffectsOptionsDlg, CDialog)

CAnnotationEffectsOptionsDlg::CAnnotationEffectsOptionsDlg(CWnd* pParent /*=NULL*/)
: CDialog(CAnnotationEffectsOptionsDlg::IDD, pParent)
{
	//TRACE("## CAnnotationEffectsOptionsDlg::CAnnotationEffectsOptionsDlg() \n");
	// Todo: Update preview fields.
}

CAnnotationEffectsOptionsDlg::~CAnnotationEffectsOptionsDlg()
{
	//TRACE("## CAnnotationEffectsOptionsDlg::~CAnnotationEffectsOptionsDlg() \n");
}

///////////////////////////////////////////////////
// Connencted (passed) value are defined and set with CRecorderView::OnEffectsOptions()
///////////////////////////////////////////////////
void CAnnotationEffectsOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_IMAGE_PATH, m_buttonImagePath);
	DDX_Text(pDX, IDC_EDIT_IMAGE_PATH, m_image.text);
	DDX_Control(pDX, IDC_EDIT_IMAGE_PATH, m_ctlImagePath);
	DDX_Text(pDX, IDC_EDIT_TIMESTAMP_FORMAT, m_timestamp.text);

	DDX_Control(pDX, IDC_BUTTON_XNOTEREMOTECONTROLMODE, m_CheckBoxXnoteRemoteControlMode );

	DDX_Text(pDX, IDC_EDIT_XNOTECAMERADELAYINMILLISEC, m_ulXnoteCameraDelayInMilliSec  );
	DDX_Control(pDX, IDC_BUTTON_XNOTEDISPLAYCAMERADELAYMODE, m_CheckBoxXnoteDisplayCameraDelayMode );
	DDX_Control(pDX, IDC_BUTTON_XNOTEDISPLAYCAMERADELAYDIRECTION, m_CheckBoxXnoteDisplayCameraDelayDirection );

	DDX_Text(pDX, IDC_EDIT_XNOTERECORDDURATIONLIMITINMILLISEC, m_ulXnoteRecordDurationLimitInMilliSec  );
	DDX_Control(pDX, IDC_BUTTON_XNOTERECORDDURATIONLIMITMODE, m_CheckBoxXnoteRecordDurationLimitMode );

	DDX_Text(pDX, IDC_EDIT_XNOTE_FORMAT, m_xnote.text);		// Used for locationpositioning and layout formatting definitions, do not deleted..!

	DDX_Text(pDX, IDC_EDIT_CAPTION_TEXT, m_caption.text);
	DDX_Control(pDX, IDC_FORMATTIMESTAMPPREVIEW,m_FormatTimestampPreview );
	DDX_Control(pDX, IDC_FORMAT_XNOTEPREVIEW, m_FormatXNotePreview);

	//TRACE("## CAnnotationEffectsOptionsDlg::DoDataExchange\n" );
	//TRACE("## -----------------------------------------------\n" );
	//TRACE("## m_CheckBoxXnoteDisplayCameraDelayMode=[%d]\n", m_CheckBoxXnoteDisplayCameraDelayMode.GetCheck() );
	//TRACE("## m_bXnoteDisplayCameraDelayMode=[%d]\n", m_bXnoteDisplayCameraDelayMode);
	//TRACE("## m_ulXnoteRecordDurationLimitInMilliSec=[%d]\n", m_ulXnoteRecordDurationLimitInMilliSec);
	//TRACE("## -----------------------------------------------\n" );
	//TRACE("## m_CheckBoxXnoteRecordDurationLimitMode=[%d]\n", m_CheckBoxXnoteRecordDurationLimitMode.GetCheck()  );
	//TRACE("## m_bXnoteRecordDurationLimitMode=[%d]\n", m_bXnoteRecordDurationLimitMode);
	//TRACE("## m_ulXnoteCameraDelayInMilliSec=[%lu]\n", m_ulXnoteCameraDelayInMilliSec);
	//TRACE("## -----------------------------------------------\n" );

	// Init Checkboxes according the correct state
	m_CheckBoxXnoteRemoteControlMode.SetCheck( m_bXnoteRemoteControlMode ? 1 : 0 );
	m_CheckBoxXnoteDisplayCameraDelayMode.SetCheck( m_bXnoteDisplayCameraDelayMode ? 1 : 0 );
	m_CheckBoxXnoteDisplayCameraDelayDirection.SetCheck( m_bXnoteDisplayCameraDelayDirection ? 1 : 0 );
	m_CheckBoxXnoteRecordDurationLimitMode.SetCheck( m_bXnoteRecordDurationLimitMode ? 1 : 0 ); 

	//TRACE("## m_CheckBoxXnoteDisplayCameraDelayMode=[%d]\n", m_CheckBoxXnoteDisplayCameraDelayMode.GetCheck() );
	//TRACE("## m_CheckBoxXnoteRecordDurationLimitMode=[%d]\n", m_CheckBoxXnoteRecordDurationLimitMode.GetCheck()  );
	//TRACE("## -----------------------------------------------\n" );
}

BEGIN_MESSAGE_MAP(CAnnotationEffectsOptionsDlg, CDialog)

	ON_EN_CHANGE(IDC_EDIT_TIMESTAMP_FORMAT, &CAnnotationEffectsOptionsDlg::OnEnChangeEditTimestampFormat)
	ON_BN_CLICKED(IDC_BUTTON_TIMESTAMP_FORMAT_OPTIONS, OnBnClickedButtonTimestampFormatOptions)

	ON_BN_CLICKED(IDC_BUTTON_XNOTEREMOTECONTROLMODE, &CAnnotationEffectsOptionsDlg::OnBnClickedButtonXnoteRemoteControlMode)

	ON_EN_CHANGE(IDC_EDIT_XNOTECAMERADELAYINMILLISEC, &CAnnotationEffectsOptionsDlg::OnEnChangeEditXNoteCameraDelayInMilliSec)
	ON_BN_CLICKED(IDC_BUTTON_XNOTEDISPLAYCAMERADELAYMODE, &CAnnotationEffectsOptionsDlg::OnBnClickedXNoteDisplayCameraDelayMode)
	ON_BN_CLICKED(IDC_BUTTON_XNOTEDISPLAYCAMERADELAYDIRECTION, &CAnnotationEffectsOptionsDlg::OnBnClickedXNoteDisplayCameraDelayDirection)

	
	ON_BN_CLICKED(IDC_BUTTON_XNOTECAMERADELAYMODE, OnBnClickedButtonXNoteFormatOptions)

	ON_EN_CHANGE(IDC_EDIT_XNOTERECORDDURATIONLIMITINMILLISEC, &CAnnotationEffectsOptionsDlg::OnEnChangeEditXnoteRecordDurationLimitInMilliSec)
	ON_BN_CLICKED(IDC_BUTTON_XNOTERECORDDURATIONLIMITMODE, &CAnnotationEffectsOptionsDlg::OnBnClickedXnoteRecordDurationLimitMode)

	ON_BN_CLICKED(IDC_BUTTON_CAPTION_OPTIONS, OnBnClickedButtonCaptionOptions)

	ON_BN_CLICKED(IDC_BUTTON_WATERMARK_OPTIONS, OnBnClickedButtonWatermarkOptions)

	ON_BN_CLICKED(IDC_BUTTON_IMAGE_PATH, OnBnClickedButtonImagePath)
	ON_BN_CLICKED(IDOK, &CAnnotationEffectsOptionsDlg::OnBnClickedOk)
	ON_WM_CREATE()

	ON_EN_CHANGE(IDC_FORMAT_XNOTEPREVIEW, &CAnnotationEffectsOptionsDlg::OnEnChangeFormatXnotepreview)
	ON_EN_CHANGE(IDC_FORMATTIMESTAMPPREVIEW, &CAnnotationEffectsOptionsDlg::OnEnChangeFormatpreview)
END_MESSAGE_MAP()

// CAnnotationEffectsOptionsDlg message handlers

void CAnnotationEffectsOptionsDlg::OnBnClickedButtonImagePath()
{
	const TCHAR *szFilter = TEXT("Bitmap Files (*.bmp)|*.bmp|GIF Files (*.gif)|*.gif|JPEG Files (*.jpg;*.jpeg)|*.jpg; *.jpeg|All Files (*.*)|*.*||");
	CFileDialog dlg(TRUE, 0, m_image.text, OFN_HIDEREADONLY, szFilter);
	if (dlg.DoModal() == IDOK){
		m_image.text = dlg.GetPathName();
		m_ctlImagePath.SetWindowText(m_image.text);
		//AfxMessageBox(m_imagePath);
	}
}

void CAnnotationEffectsOptionsDlg::OnBnClickedButtonCaptionOptions()
{
	// TODO: Add your control notification handler code here
	CEffectsOptionsDlg optDlg;
	optDlg.m_params = m_caption;
	if (optDlg.DoModal() == IDOK){
		m_caption = optDlg.m_params;
	}
}


void CAnnotationEffectsOptionsDlg::OnBnClickedButtonTimestampFormatOptions()
{
	// TODO: Add your control notification handler code here
	CEffectsOptionsDlg optDlg;
	optDlg.m_params = m_timestamp;
	if (optDlg.DoModal() == IDOK){
		m_timestamp = optDlg.m_params;
	}
}

void CAnnotationEffectsOptionsDlg::OnBnClickedButtonXNoteFormatOptions()
{
	// TODO: Add your control notification handler code here
	//TRACE("## CAnnotationEffectsOptionsDlg::OnBnClickedButtonXNoteFormatOptions \n");
	CEffectsOptionsDlg optDlg;
	optDlg.m_params = m_xnote;
	if (optDlg.DoModal() == IDOK){
		m_xnote = optDlg.m_params;
	}
}


void CAnnotationEffectsOptionsDlg::OnBnClickedButtonWatermarkOptions()
{
	// TODO: Add your control notification handler code here
	CEffectsOptions2Dlg optDlg;
	optDlg.m_params = m_image;
	if (optDlg.DoModal() == IDOK){
		m_image = optDlg.m_params;
	}
}

void CAnnotationEffectsOptionsDlg::OnEnChangeEditTimestampFormat()
{
	// Update Timestamp formatted preview presentation field
	CAnnotationEffectsOptionsDlg::OnEnChangeFormatpreview();
}


// Function show layout of output in preview field on dialog.
// This opens when checkbox is clicked but also when the value in the text field is changed
void CAnnotationEffectsOptionsDlg::OnEnChangeEditXNoteCameraDelayInMilliSec()
{
	//TRACE("## CAnnotationEffectsOptionsDlg::OnEnChangeEditXNoteCameraDelayInMilliSec()\n");

	// Update Xnote formatted preview presentation field
	CAnnotationEffectsOptionsDlg::OnEnChangeFormatXnotepreview();
}

							    								   
void CAnnotationEffectsOptionsDlg::OnBnClickedXNoteDisplayCameraDelayMode()
{
	//TRACE("## CAnnotationEffectsOptionsDlg::OnBnClickedXnoteCameraDelayMode()\n");
	// bool bBool = m_CheckBoxXnoteDisplayCameraDelayMode.GetCheck()? true : false;

	// Update presentationfield
	CAnnotationEffectsOptionsDlg::OnEnChangeFormatXnotepreview();
}

void CAnnotationEffectsOptionsDlg::OnBnClickedXNoteDisplayCameraDelayDirection()
{
	//TRACE("## CAnnotationEffectsOptionsDlg::OnBnClickedXnoteCameraDelayMode()\n");
	// bool bBool = m_CheckBoxXnoteDisplayCameraDelayMode.GetCheck()? true : false;

	// Update presentationfield
	CAnnotationEffectsOptionsDlg::OnEnChangeFormatXnotepreview();
}



void CAnnotationEffectsOptionsDlg::OnBnClickedOk()
{
	CString str;
	GetDlgItem(IDC_EDIT_TIMESTAMP_FORMAT)->GetWindowTextA(str);
	if (!IsStrftimeSafe(str.GetBuffer()))
	{
		MessageBox("There is an error in the timestamp format!", "Error!", MB_OK);
		return;
	}

	// Convert values that are defined by buttons again to bools or other values.
	m_bXnoteRemoteControlMode = m_CheckBoxXnoteRemoteControlMode.GetCheck() ? true : false ;
	m_bXnoteDisplayCameraDelayMode = m_CheckBoxXnoteDisplayCameraDelayMode.GetCheck() ? true : false ;
	m_bXnoteDisplayCameraDelayDirection = m_CheckBoxXnoteDisplayCameraDelayDirection.GetCheck() ? true : false ;
	m_bXnoteRecordDurationLimitMode = m_CheckBoxXnoteRecordDurationLimitMode.GetCheck() ? true : false ;

	Invalidate();
	OnOK();
}

int CAnnotationEffectsOptionsDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	/* TODO: Is this the most appropiate place to put defaults? Probably not ...
	 * Also, theres probably a better check than if the text is zero weather
	 * or not the data has been initialized.*/

	/*
	 * Comment Jun2010: Indeed there is a better place for initialisation. 
	 * Connected (passed) value are defined and set with CRecorderView::OnEffectsOptions()
	 */

	if (strlen(m_timestamp.text) == 0)
	{
		m_timestamp.text = CString("Time: %H:%M:%S");
		m_timestamp.backgroundColor = RGB(255,255,255);
		m_timestamp.textColor = RGB(0,0,0);
		m_timestamp.logfont.lfHeight = 12;
	}

    // At first I believed that m_cXnoteDisplayFormatString had not any added value here.
	// But it appeared that the length of this filed determines the visible tect in annotation.
	// Code below determine the string length of the filed to use.

	//TRACE("## CAnnotationEffectsOptionsDlg::OnCreate   m_cXnoteDisplayFormatString=[%s](%d)  m_ulXnoteCameraDelayInMilliSec:[%d]\n",m_cXnoteDisplayFormatString, strlen(m_cXnoteDisplayFormatString), m_ulXnoteCameraDelayInMilliSec );
	if ( strlen( m_cXnoteDisplayFormatString ) == 0) 
	{
		//TRACE("## CAnnotationEffectsOptionsDlg::OnCreate   Change m_cXnoteDisplayFormatString now\n");

		// Format (delay) hh:mm:ss.ttt"
		char cTmpBuff[128] = "";
		(void) CXnoteStopwatchFormat::FormatXnoteSampleString(cTmpBuff, m_ulXnoteCameraDelayInMilliSec, m_bXnoteDisplayCameraDelayMode, m_bXnoteDisplayCameraDelayDirection );
		m_cXnoteDisplayFormatString = CString( cTmpBuff );
	}

	return 0;
}

bool CAnnotationEffectsOptionsDlg::IsStrftimeSafe(char * pbuffer)
{
	pbuffer = ::strchr(pbuffer, '%');
	while (pbuffer)
	{
		char nxt = *(++pbuffer);
		if (nxt != 'a' &&
			nxt != 'A' &&
			nxt != 'b' &&
			nxt != 'B' &&
			nxt != 'c' &&
			nxt != 'd' &&
			nxt != 'H' &&
			nxt != 'I' &&
			nxt != 'j' &&
			nxt != 'm' &&
			nxt != 'M' &&
			nxt != 'p' &&
			nxt != 'S' &&
			nxt != 'U' &&
			nxt != 'w' &&
			nxt != 'W' &&
			nxt != 'x' &&
			nxt != 'X' &&
			nxt != 'y' &&
			nxt != 'Y' &&
			nxt != 'Z' &&
			nxt != '%')
			return false;

		pbuffer = ::strchr(++pbuffer, '%');
	}

	return true;
}

							     
void CAnnotationEffectsOptionsDlg::OnBnClickedXnoteRecordDurationLimitMode()
{
	// TODO: Add your control notification handler code here	
	//TRACE("## CAnnotationEffectsOptionsDlg::OnBnClickedXnoteRecordDurationLimitMode() State=[%d] \n", m_CheckBoxXnoteRecordDurationLimitMode.GetCheck() ? true : false );

}

void CAnnotationEffectsOptionsDlg::OnEnChangeEditXnoteRecordDurationLimitInMilliSec()
{
	//TRACE("## CAnnotationEffectsOptionsDlg::OnEnChangeEditXnoteRecordDurationLimitInMilliSec()\n");
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}



void CAnnotationEffectsOptionsDlg::OnEnChangeFormatXnotepreview()
{
	// TRACE("## CAnnotationEffectsOptionsDlg::OnEnChangeFormatXnotepreview()\n");
	CString str;
	char cTmpBuff[128] = "";

	GetDlgItem(IDC_EDIT_XNOTECAMERADELAYINMILLISEC)->GetWindowText(str);
	ULONG ul_DelayTimeInMilliSec = atol(str);

	bool bDisplayCameraDelay = m_CheckBoxXnoteDisplayCameraDelayMode.GetCheck() ? true : false ;
	bool bDisplayCameraDelay2 = m_CheckBoxXnoteDisplayCameraDelayDirection.GetCheck() ? true : false ;
	
	// format (delay) hh:mm:ss.ttt"
	(void) CXnoteStopwatchFormat::FormatXnoteSampleString ( cTmpBuff, ul_DelayTimeInMilliSec, bDisplayCameraDelay, bDisplayCameraDelay2 );

	m_FormatXNotePreview.SetWindowText(cTmpBuff); 
}


	// Zoek CAnnotationEffectsOptionsDlg::OnEnChangeFormatXnotepreview

void CAnnotationEffectsOptionsDlg::OnEnChangeFormatpreview()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TRACE("CAnnotationEffectsOptionsDlg::OnEnChangeFormatpreview() \n");
	CString str;
	GetDlgItem(IDC_EDIT_TIMESTAMP_FORMAT)->GetWindowText(str);

	if (IsStrftimeSafe(str.GetBuffer())) {
		time_t szClock;
		time(&szClock);
		struct tm *newTime = localtime(&szClock);
		char TimeBuff[256];
		strftime(TimeBuff, sizeof(TimeBuff), str, newTime);
		m_FormatTimestampPreview.SetWindowText(TimeBuff);
	}
	else
	{
		m_FormatTimestampPreview.SetWindowTextA("Error in timestamp format!");
	}

}

void CAnnotationEffectsOptionsDlg::OnBnClickedButtonXnoteRemoteControlMode()
{
	// TODO: Add your control notification handler code here
}
