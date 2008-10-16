// EffectsOptions2.h
// include file for CEffectsOptions2 dialog
/////////////////////////////////////////////////////////////////////////////
#ifndef EFFECTSOPTIONS2_H	// because pragma once is compiler specific
#define EFFECTSOPTIONS2_H

#pragma once
#include "ImageAttributes.h"

#ifndef IDD_EFFECTS_OPTIONS2
	#error include 'resource.h' before including this file for dialog resource
#endif

// CEffectsOptions2 dialog

class CEffectsOptions2 : public CDialog
{
	DECLARE_DYNAMIC(CEffectsOptions2)

public:
	CEffectsOptions2(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEffectsOptions2();

// Dialog Data
	enum { IDD = IDD_EFFECTS_OPTIONS2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	ImageAttributes m_params;
};

#endif	// EFFECTSOPTIONS2_H
