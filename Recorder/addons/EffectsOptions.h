// EffectsOptions.h
// include file for CEffectsOptionsDlg dialog
/////////////////////////////////////////////////////////////////////////////
#ifndef EFFECTSOPTIONS_H	// because pragma once is compiler specific
#define EFFECTSOPTIONS_H

#pragma once

#include "TextAttributes.h"

#ifndef IDD_EFFECTS_OPTIONS
	#error include 'resource.h' before including this file for dialog resource
#endif

// CEffectsOptionsDlg dialog

class CEffectsOptionsDlg : public CDialog
{
	DECLARE_DYNAMIC(CEffectsOptionsDlg)

public:
	CEffectsOptionsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEffectsOptionsDlg();

// Dialog Data
	enum { IDD = IDD_EFFECTS_OPTIONS };


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual int EffectsOptionsSetXPosRatio( int nPosition );
	virtual int EffectsOptionsSetYPosRatio( int nPosition );

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonBackgroundColor();
	afx_msg void OnBnClickedButtonTextColor();
	afx_msg void OnBnClickedButtonFont();
	//int m_Position;
	TextAttributes m_params;
	afx_msg void OnBnClickedOk();
};

#endif	// EFFECTSOPTIONS_H
