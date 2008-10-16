// EffectsOptions.h
// include file for CEffectsOptions dialog
/////////////////////////////////////////////////////////////////////////////
#ifndef EFFECTSOPTIONS_H	// because pragma once is compiler specific
#define EFFECTSOPTIONS_H

#pragma once

#include "TextAttributes.h"

#ifndef IDD_EFFECTS_OPTIONS
	#error include 'resource.h' before including this file for dialog resource
#endif

// CEffectsOptions dialog

class CEffectsOptions : public CDialog
{
	DECLARE_DYNAMIC(CEffectsOptions)

public:
	CEffectsOptions(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEffectsOptions();

// Dialog Data
	enum { IDD = IDD_EFFECTS_OPTIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonBackgroundColor();
	afx_msg void OnBnClickedButtonTextColor();
	afx_msg void OnBnClickedButtonFont();
	//int m_Position;
	TextAttributes m_params;
};

#endif	// EFFECTSOPTIONS_H
