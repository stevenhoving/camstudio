// AnnotationEffectsOptions.cpp : implementation file
// Modifies Timestamp, Caption, and Watermark
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Recorder.h"
#include "XnoteStopwatchFormat.h"
#include "AnnotationEffectsOptions.h"

// CAnnotationEffectsOptionsDlg dialog

IMPLEMENT_DYNAMIC(CAnnotationEffectsOptionsDlg, CDialog)

CAnnotationEffectsOptionsDlg::CAnnotationEffectsOptionsDlg(CWnd* pParent /*=NULL*/)
: CDialog(CAnnotationEffectsOptionsDlg::IDD, pParent)
{
}

CAnnotationEffectsOptionsDlg::~CAnnotationEffectsOptionsDlg()
{
}

void CAnnotationEffectsOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_IMAGE_PATH, m_buttonImagePath);
	DDX_Text(pDX, IDC_EDIT_IMAGE_PATH, m_image.text);
	DDX_Control(pDX, IDC_EDIT_IMAGE_PATH, m_ctlImagePath);
	DDX_Text(pDX, IDC_EDIT_TIMESTAMP_FORMAT, m_timestamp.text);

	DDX_Text(pDX, IDC_EDIT_XNOTECAMERADELAYINMILLISEC, m_ulXnoteCameraDelayInMilliSec  );
	DDX_Control(pDX, IDC_XNOTEDISPLAYCAMERADELAY, m_CheckBoxXnoteDisplayCameraDelay);
	DDX_Text(pDX, IDC_EDIT_XNOTE_FORMAT, m_xnote.text);

	DDX_Text(pDX, IDC_EDIT_CAPTION_TEXT, m_caption.text);
	DDX_Control(pDX, IDC_FORMATPREVIEW,m_FormatPreview );
	DDX_Control(pDX, IDC_FORMAT_XNOTEPREVIEW, m_FormatXNotePreview);
}

BEGIN_MESSAGE_MAP(CAnnotationEffectsOptionsDlg, CDialog)

	ON_EN_CHANGE(IDC_EDIT_TIMESTAMP_FORMAT, &CAnnotationEffectsOptionsDlg::OnEnChangeEditTimestampFormat)
	ON_BN_CLICKED(IDC_BUTTON_TIMESTAMP_FORMAT_OPTIONS, OnBnClickedButtonTimestampFormatOptions)
	
	ON_EN_CHANGE(IDC_EDIT_XNOTECAMERADELAYINMILLISEC, &CAnnotationEffectsOptionsDlg::OnEnChangeEditXNoteFormat)
	ON_BN_CLICKED(IDC_BUTTON_XNOTE_FORMAT_OPTIONS, OnBnClickedButtonXNoteFormatOptions)
	ON_BN_CLICKED(IDC_XNOTEDISPLAYCAMERADELAY, &CAnnotationEffectsOptionsDlg::OnEnChangeEditXNoteFormat)
	
	ON_BN_CLICKED(IDC_BUTTON_CAPTION_OPTIONS, OnBnClickedButtonCaptionOptions)

	ON_BN_CLICKED(IDC_BUTTON_WATERMARK_OPTIONS, OnBnClickedButtonWatermarkOptions)

	ON_BN_CLICKED(IDC_BUTTON_IMAGE_PATH, OnBnClickedButtonImagePath)
	ON_BN_CLICKED(IDOK, &CAnnotationEffectsOptionsDlg::OnBnClickedOk)
	ON_WM_CREATE()
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
	CString str;
	GetDlgItem(IDC_EDIT_TIMESTAMP_FORMAT)->GetWindowText(str);
	if (IsStrftimeSafe(str.GetBuffer())) {
		time_t szClock;
		time(&szClock);
		struct tm *newTime = localtime(&szClock);
		char TimeBuff[256];
		strftime(TimeBuff, sizeof(TimeBuff), str, newTime);
		m_FormatPreview.SetWindowText(TimeBuff);
	}
	else
	{
		m_FormatPreview.SetWindowTextA("Error in timestamp format!");
	}
}


// Function show layout of output in preview field on dialog.
void CAnnotationEffectsOptionsDlg::OnEnChangeEditXNoteFormat()
{
	CString str;
	char cTmpBuff[64];

	GetDlgItem(IDC_EDIT_XNOTECAMERADELAYINMILLISEC)->GetWindowText(str);
	ULONG ul_DelayTimeInMilliSec = atol(str);

	bool bDisplayCameraDelay = m_CheckBoxXnoteDisplayCameraDelay.GetCheck()? true : false;
	
	// format (delay) hh:mm:ss.ttt"
	(void) CXnoteStopwatchFormat::FormatXnoteSampleString ( cTmpBuff, ul_DelayTimeInMilliSec, bDisplayCameraDelay);

	m_FormatXNotePreview.SetWindowText(cTmpBuff); 
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
	OnOK();
}

int CAnnotationEffectsOptionsDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	/* TODO: Is this the most appropiate place to put defaults? Probably not ...
	 * Also, theres probably a better check than if the text is zero weather
	 * or not the data has been initialized.*/
	if (strlen(m_timestamp.text) == 0)
	{
		m_timestamp.text = CString("Time: %H:%M:%S");
		m_timestamp.backgroundColor = RGB(255,255,255);
		m_timestamp.textColor = RGB(0,0,0);
		m_timestamp.logfont.lfHeight = 12;
	}

	if ( m_ulXnoteCameraDelayInMilliSec == 0)
	{
		// We need format to display time in milli seconds here.... (E.g. Time from xNote)
		m_ulXnoteCameraDelayInMilliSec = 149;
	}

    // m_cXnoteDisplayFormatString, had zo te zien geen toegevoegde waarde is alleen maar tekst?
	// maar dit blijkt de layout van de overlay te bepalen ..!
	// onderstaande bepaalt in elk geval de lengte van het te gebriken veld.

	//TRACE("## CAnnotationEffectsOptionsDlg::OnCreate   m_cXnoteDisplayFormatString=[%s](%d)  m_ulXnoteCameraDelayInMilliSec:[%d]\n",m_cXnoteDisplayFormatString, strlen(m_cXnoteDisplayFormatString), m_ulXnoteCameraDelayInMilliSec );
	if ( strlen( m_cXnoteDisplayFormatString ) == 0) 
	{
		//TRACE("## CAnnotationEffectsOptionsDlg::OnCreate   Change m_cXnoteDisplayFormatString now\n");

		// Format (delay) hh:mm:ss.ttt"
		char cTmpBuff[64];
		(void) CXnoteStopwatchFormat::FormatXnoteSampleString(cTmpBuff, m_ulXnoteCameraDelayInMilliSec, m_bXnoteDisplayCameraDelay );
		m_cXnoteDisplayFormatString = CString( cTmpBuff );
	}

	// Check if options are defined. Apply defaults, otherwise nothing will be seen.
	if ( m_xnote.textColor == m_xnote.backgroundColor )
	{
		m_xnote.backgroundColor = RGB(255,255,255);
		m_xnote.textColor = RGB(0,0,0);
		m_xnote.logfont.lfHeight = 12;
		m_xnote.text = m_cXnoteDisplayFormatString;
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
