// AnnotationEffectsOptions.cpp : implementation file
// Modifies Timestamp, Caption, and Watermark
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Recorder.h"
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
	DDX_Text(pDX, IDC_EDIT_CAPTION_TEXT, m_caption.text);
	DDX_Control(pDX, IDC_FORMATPREVIEW, m_FormatPreview);
}

BEGIN_MESSAGE_MAP(CAnnotationEffectsOptionsDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_IMAGE_PATH, OnBnClickedButtonImagePath)
	ON_BN_CLICKED(IDC_BUTTON_CAPTION_OPTIONS, OnBnClickedButtonCaptionOptions)
	ON_BN_CLICKED(IDC_BUTTON_TIMESTAMP_FORMAT_OPTIONS, OnBnClickedButtonTimestampFormatOptions)
	ON_BN_CLICKED(IDC_BUTTON_WATERMARK_OPTIONS, OnBnClickedButtonWatermarkOptions)
	ON_EN_CHANGE(IDC_EDIT_TIMESTAMP_FORMAT, &CAnnotationEffectsOptionsDlg::OnEnChangeEditTimestampFormat)
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

void CAnnotationEffectsOptionsDlg::OnBnClickedButtonWatermarkOptions()
{
	// TODO: Add your control notification handler code here
	CEffectsOptions2Dlg optDlg;
	optDlg.m_params = m_image;
	if (optDlg.DoModal() == IDOK){
		m_image = optDlg.m_params;
	}
}

BOOL IsStrftimeSafe(char *buffer)
{
	char *ptr = buffer, nxt;
	int cur = 0;
	while (ptr = strchr(ptr, '%'))
	{
		nxt = *(ptr+sizeof(char));
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
		ptr+=sizeof(char);
	}
	return true;
}

void CAnnotationEffectsOptionsDlg::OnEnChangeEditTimestampFormat()
{
	CString str;
	char TimeBuff[256];
	struct tm   *newTime;
    time_t      szClock;
    time( &szClock );
    newTime = localtime( &szClock );
	GetDlgItem(IDC_EDIT_TIMESTAMP_FORMAT)->GetWindowTextA(str);
	if (IsStrftimeSafe(str.GetBuffer()))
	{
		strftime(TimeBuff, sizeof(TimeBuff), str, newTime);
		m_FormatPreview.SetWindowTextA(TimeBuff);
	}
	else
	{
		m_FormatPreview.SetWindowTextA("Error in timestamp format!");
	}
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
		m_timestamp.text = CString("Recording %H:%M:%S");
		m_timestamp.backgroundColor = RGB(255,255,255);
		m_timestamp.textColor = RGB(0,0,0);
		m_timestamp.logfont.lfHeight = 12;
	}

	return 0;
}
