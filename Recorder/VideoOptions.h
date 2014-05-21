#if !defined(AFX_VIDEOOPTIONS_H__25063B0F_28D8_469C_871E_F8A0DD84DA6B__INCLUDED_)
#define AFX_VIDEOOPTIONS_H__25063B0F_28D8_469C_871E_F8A0DD84DA6B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "profile.h"
#include "VideoCompressorFilter.h"
#include "ScreenCaptureFilter.h"
// VideoOptions.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVideoOptionsDlg dialog

class CVideoOptionsDlg : public CDialog
{
public:
// Construction
	CVideoOptionsDlg(CWnd* pParent = NULL);   // standard constructor
public:
	CVideoOptionsDlg(const sVideoOpts& cOpts, CWnd* pParent = NULL);   // standard constructor

	const sVideoOpts& Opts() const	{return m_cOpts;}
	CScreenCaptureFilter* ScreenCaptureFilter() const {return m_pVidCapFilter;}
	CVideoCompressorFilter* VideoCompressorFilter() const {return m_pCompFilter;}
	//void RefreshCompressorButtons();
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
	afx_msg void OnLock();
	afx_msg void OnCaptureFrameRateInfo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	sVideoOpts m_cOpts;
	CComboBox m_ctrlCBCompressor;
	CComboBox m_ctrlCBVCF;
	CButton m_ctrlBtnAbout;
	CButton m_ctrlBtnCfgComp;
	CButton m_ctrlBtnCfgVidCap;
	CSliderCtrl m_ctrlSliderQuality;
	CStatic m_ctrlStaticQuality;
	CEdit m_ctrlEditKeyFrameInterval;
	CEdit m_ctrlEdiPlaybackRate;
	CButton m_ctrlButtonAutoAdjust;
	CButton m_ctrlButtonLock;
	CSliderCtrl m_ctrlSliderAdjust;
	CEdit m_ctrlEditCaptureInterval;
	CButton m_ctrlButtonAbout;
	CButton m_ctrlButtonConfigure;
	CButton m_ctrlButtonInfo;
	CBitmap m_bInfo;
	CButton m_ctrlButtonRoundDown;
	int m_iQuality;
	int m_iKeyFrameInterval;
	int m_iCaptureInterval;
	int m_iPlaybackRate;
	int m_iAdjust;
	int m_iStaticQuality;
	int m_iCurrentSliderPos;
	void AutoSetRate(int val, int& framerate, int& delayms);
	void AutoSetRateWithLock(int val, int& framerate, int& delayms);
	void AdjustSliderRange();
	int CurrentLockPosition();
	int CurrentAutoAdjustPosition();
	void InitAutoAdjustArray();
	void GetCurrentSliderPos();
	void LoadVideoCompressors();
	void LoadVideoCaptureFilters();
	void RefreshCompressorButtons();
	void RefreshVidCapConfigButton();
	const CString GetCurrentVideoDevice()const;
	const CString GetCurrentVideoCompressor()const;
	void ReInitializeVidCapFilter();
	void ReInitializeVidCompFilter();
	CVideoCompressorFilter *m_pCompFilter;
	CScreenCaptureFilter *m_pVidCapFilter;
public:
	afx_msg void OnBnClickedSupportrounddown();
	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnSelchangeVidcap();
	afx_msg void OnBnClickedConfigureVcf();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIDEOOPTIONS_H__25063B0F_28D8_469C_871E_F8A0DD84DA6B__INCLUDED_)
