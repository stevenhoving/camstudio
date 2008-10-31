/////////////////////////////////////////////////////////////////////////////
// CAnnotationEffectsOptionsDlg dialog
//
#ifndef ANNOTATIONEFFECTSOPTIONS_H	// belt and suspenders
#define ANNOTATIONEFFECTSOPTIONS_H

#pragma once

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
public:
//	virtual BOOL OnInitDialog();
public:
	// The input and output values
	TextAttributes	m_timestamp;
	TextAttributes	m_caption;
	ImageAttributes	m_image;
protected:
private:
	CBitmapButton m_buttonImagePath;
	CEdit m_ctlImagePath;

	afx_msg void OnBnClickedButtonImagePath();
	afx_msg void OnBnClickedButtonCaptionOptions();
	afx_msg void OnBnClickedButtonTimestampFormatOptions();
	afx_msg void OnBnClickedButtonWatermarkOptions();
};
#endif	// ANNOTATIONEFFECTSOPTIONS_H
