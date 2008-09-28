// AnnotationEffectsOptions.cpp : implementation file
//

#include "stdafx.h"
#include "vscap.h"
#include "AnnotationEffectsOptions.h"
#include ".\annotationeffectsoptions.h"


// CAnnotationEffectsOptions dialog

IMPLEMENT_DYNAMIC(CAnnotationEffectsOptions, CDialog)
CAnnotationEffectsOptions::CAnnotationEffectsOptions(CWnd* pParent /*=NULL*/)
: CDialog(CAnnotationEffectsOptions::IDD, pParent)
{
}

CAnnotationEffectsOptions::~CAnnotationEffectsOptions()
{
}

void CAnnotationEffectsOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_IMAGE_PATH, m_buttonImagePath);
	DDX_Text(pDX, IDC_EDIT_IMAGE_PATH, m_image.text);
	DDX_Control(pDX, IDC_EDIT_IMAGE_PATH, m_ctlImagePath);
	DDX_Text(pDX, IDC_EDIT_TIMESTAMP_FORMAT, m_timestamp.text);
	DDX_Text(pDX, IDC_EDIT_CAPTION_TEXT, m_caption.text);
}


BEGIN_MESSAGE_MAP(CAnnotationEffectsOptions, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_IMAGE_PATH, OnBnClickedButtonImagePath)
	ON_BN_CLICKED(IDC_BUTTON_CAPTION_OPTIONS, OnBnClickedButtonCaptionOptions)
	ON_BN_CLICKED(IDC_BUTTON_TIMESTAMP_FORMAT_OPTIONS, OnBnClickedButtonTimestampFormatOptions)
	ON_BN_CLICKED(IDC_BUTTON_WATERMARK_OPTIONS, OnBnClickedButtonWatermarkOptions)
END_MESSAGE_MAP()


// CAnnotationEffectsOptions message handlers

void CAnnotationEffectsOptions::OnBnClickedButtonImagePath()
{
	// TODO: Add your control notification handler code here
	static char BASED_CODE szFilter[] = "Bitmap Files (*.bmp)|*.bmp|GIF Files (*.gif)|*.gif|JPEG Files (*.jpg;*.jpeg)|*.jpg; *.jpeg|All Files (*.*)|*.*||";
	CFileDialog dlg(TRUE, 0, m_image.text, OFN_HIDEREADONLY, szFilter);
	if(dlg.DoModal() == IDOK){
		m_image.text = dlg.GetPathName();
		m_ctlImagePath.SetWindowText(m_image.text);
		//AfxMessageBox(m_imagePath);
	}
}

void CAnnotationEffectsOptions::OnBnClickedButtonCaptionOptions()
{
	// TODO: Add your control notification handler code here
	CEffectsOptions optDlg;
	optDlg.m_params = m_caption;
	if(optDlg.DoModal() == IDOK){
		m_caption = optDlg.m_params;
	}
}

void CAnnotationEffectsOptions::OnBnClickedButtonTimestampFormatOptions()
{
	// TODO: Add your control notification handler code here
	CEffectsOptions optDlg;
	optDlg.m_params = m_timestamp;
	if(optDlg.DoModal() == IDOK){
		m_timestamp = optDlg.m_params;
	}
}

void CAnnotationEffectsOptions::OnBnClickedButtonWatermarkOptions()
{
	// TODO: Add your control notification handler code here
	CEffectsOptions2 optDlg;
	optDlg.m_params = m_image;
	if(optDlg.DoModal() == IDOK){
		m_image = optDlg.m_params;
	}
}
