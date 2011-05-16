#if !defined(AFX_VIDEOOPTIONS_H__25063B0F_28D8_469C_871E_F8A0DD84DA6B__INCLUDED_)
#define AFX_VIDEOOPTIONS_H__25063B0F_28D8_469C_871E_F8A0DD84DA6B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "profile.h"

// VideoOptions.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVideoOptionsDlg dialog

class CVideoOptionsDlg : public CDialog
{
// Construction
	CVideoOptionsDlg(CWnd* pParent = NULL);   // standard constructor
public:
	CVideoOptionsDlg(const sVideoOpts& cOpts, CWnd* pParent = NULL);   // standard constructor

	const sVideoOpts& Opts() const	{return m_cOpts;}

	void RefreshCompressorButtons();
	void RefreshAutoOptions();
	void UpdateAdjustSliderVal();

	void DDV_KeyFrameInterval(CDataExchange* pDX, int value, int minVal, int maxVal);
	void DDV_CaptureInterval(CDataExchange* pDX, int value, int minVal, int maxVal);
	void DDV_PlaybackRate(CDataExchange* pDX, int value, int minVal, int maxVal);

// Dialog Data
	//{{AFX_DATA(CVideoOptionsDlg)
	enum { IDD = IDD_VIDEOOPTIONS };
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVideoOptionsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVideoOptionsDlg)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnAbout();
	afx_msg void OnSelchangeCompressors();
	afx_msg void OnConfigure();
	virtual void OnCancel();
	afx_msg void OnAuto();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	sVideoOpts m_cOpts;
	CComboBox m_ctrlCBCompressor;
	CSliderCtrl m_ctrlSliderQuality;
	CStatic m_ctrlStaticQuality;
	CEdit m_ctrlEditKeyFrameInterval;
	CEdit m_ctrlEdiPlaybackRate;
	CButton m_ctrlButtonAutoAdjust;
	CSliderCtrl m_ctrlSliderAdjust;
	CEdit m_ctrlEditCaptureInterval;
	CButton m_ctrlButtonAbout;
	CButton m_ctrlButtonConfigure;
	int m_iQuality;
	int m_iKeyFrameInterval;
	int m_iCaptureInterval;
	int m_iPlaybackRate;
	int m_iAdjust;
	int m_iStaticQuality;

	int LoadICList();
	void AutoSetRate(int val, int& framerate, int& delayms);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIDEOOPTIONS_H__25063B0F_28D8_469C_871E_F8A0DD84DA6B__INCLUDED_)
