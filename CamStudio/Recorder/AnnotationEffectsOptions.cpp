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
}

BEGIN_MESSAGE_MAP(CAnnotationEffectsOptionsDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_IMAGE_PATH, OnBnClickedButtonImagePath)
	ON_BN_CLICKED(IDC_BUTTON_CAPTION_OPTIONS, OnBnClickedButtonCaptionOptions)
	ON_BN_CLICKED(IDC_BUTTON_TIMESTAMP_FORMAT_OPTIONS, OnBnClickedButtonTimestampFormatOptions)
	ON_BN_CLICKED(IDC_BUTTON_WATERMARK_OPTIONS, OnBnClickedButtonWatermarkOptions)
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
