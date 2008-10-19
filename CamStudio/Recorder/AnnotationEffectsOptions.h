#pragma once
#include "afxext.h"
#include "afxwin.h"

#include "EffectsOptions.h"
#include "EffectsOptions2.h"

// CAnnotationEffectsOptionsDlg dialog

class CAnnotationEffectsOptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(CAnnotationEffectsOptionsDlg)

public:
	CAnnotationEffectsOptionsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAnnotationEffectsOptionsDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_MAIN_EFFECTS_OPTIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	CBitmapButton m_buttonImagePath;
public:
//	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonImagePath();
protected:
	CEdit m_ctlImagePath;
public:
	TextAttributes m_timestamp;
	TextAttributes m_caption;
	ImageAttributes m_image;
	afx_msg void OnBnClickedButtonCaptionOptions();
	afx_msg void OnBnClickedButtonTimestampFormatOptions();
	afx_msg void OnBnClickedButtonWatermarkOptions();
};
