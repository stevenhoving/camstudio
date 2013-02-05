// EffectsOptions2.h
// include file for CEffectsOptions2Dlg dialog
/////////////////////////////////////////////////////////////////////////////
#ifndef EFFECTSOPTIONS2_H	// because pragma once is compiler specific
#define EFFECTSOPTIONS2_H

#pragma once
#include "ImageAttributes.h"
#include "ximage.h"

#ifndef IDD_EFFECTS_OPTIONS2
	#error include 'resource.h' before including this file for dialog resource
#endif

// CEffectsOptions2Dlg dialog

class CEffectsOptions2Dlg : public CDialog
{
//	DECLARE_DYNAMIC(CEffectsOptions2Dlg)

// Construction
public:
	CEffectsOptions2Dlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEffectsOptions2Dlg();

// Dialog Data
	//{{AFX_DATA(CEffectsOptions2Dlg)
	enum { IDD = IDD_EFFECTS_OPTIONS2 };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEffectsOptions2Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CEffectsOptions2Dlg)
	virtual void OnOK();
	afx_msg void OnPaint();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	int CEffectsOptions2Dlg::EffectsOptions2SetXPosRatio( int nPosition );
	int CEffectsOptions2Dlg::EffectsOptions2SetYPosRatio( int nPosition );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	ImageAttributes m_params;
private:
	int m_iPos;			// image ePosition
	int m_iBrightness;	// image brightness
	int m_iContrast;	// image contrast
	CxImage m_cImage;
	CxImage m_cImageOriginal;	// cache to prevent flicker
	CStatic m_ctrlStaticPreview;
	CSliderCtrl m_ctrlSliderBrightness;
	CSliderCtrl m_ctrlSliderContrast;
public:
	virtual BOOL OnInitDialog();
private:
public:
	afx_msg void OnBnClickedOk();
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif	// EFFECTSOPTIONS2_H
