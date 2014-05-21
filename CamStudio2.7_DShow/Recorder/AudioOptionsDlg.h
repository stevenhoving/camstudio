// CAudioFormatDlg.h : header file
//
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_AUDIOFORMAT_H__D1098020_67C5_491D_AC06_2EE69A7C135D__INCLUDED_)
#define AFX_AUDIOFORMAT_H__D1098020_67C5_491D_AC06_2EE69A7C135D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AudioCaptureFilter.h"
#include "AudioCompressorFilter.h"
#include <vector>
/////////////////////////////////////////////////////////////////////////////
// CAudioFormatDlg dialog

class CAudioFormatDlg : public CDialog
{
	// Construction
	CAudioFormatDlg(CWnd* pParent = NULL);   // standard constructor; disallowed
public:
	CAudioFormatDlg(const sAudioFormat& cFmt, CWnd* pParent = NULL);   // standard constructor

	const sAudioFormat& Format() const	{return m_cFmt;}
	CAudioCaptureFilter* AudioCaptureFilter() const {return m_pAudioCapFilter;}
	CAudioCompressorFilter* AudioCompressorFilter() const {return m_pAudioCompFilter;}
	// Dialog Data
	//{{AFX_DATA(CAudioFormatDlg)
	enum { IDD = IDD_AUDIODIALOG };
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAudioFormatDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAudioFormatDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelectCompression();
	afx_msg void OnSelchangeRecordformat();
	virtual void OnCancel();
	afx_msg void OnVolume();
	afx_msg void OnSelchangeInputdevice();
	DECLARE_MESSAGE_MAP()
private:
	typedef std::pair<DWORD, LPCTSTR> pairIDFormat;
	std::vector <pairIDFormat> m_vFormat;

	sAudioFormat m_cFmt;
	LPWAVEFORMATEX m_pwfx;
	DWORD m_cbwfx;
	int m_iAudioBitsPerSample;
	int m_iAudioNumChannels;
	int m_iAudioSamplesPerSeconds;
	BOOL m_bAudioCompression;
	int m_iNumFormat;
	int m_iNumDevice;
	UINT m_devicemap[15];

	// dialog variables
	int m_iInterleavePeriod;

	// dialog controls
	CComboBox m_ctrlCBAudioCompressors;
	CComboBox m_ctrlCBInputDevice;
	CComboBox m_ctrlCBSupportedFormats;
	CButton m_ctrlBTNConfigure;
	
	CEdit m_ctrlEditBitsPerSample;
	CEdit m_ctrlEditChannels;
	CEdit m_ctrlSamplesPerSec;
	CEdit m_ctrlAvgBytesPerSec;

	void LoadAudioDevices();
	void LoadAudioCompressors();
	void LoadCompressionFormats();
	void UpdateCompressFormatInterface();

	void ReInitializeCaptureFilter();
	void ReInitializeCompressorFilter();

	WAVEFORMATEX *GetCurrentFormat()const;
	const CString GetCurrentCaptureDevice() const;
	const CString GetCurrentCompressor() const;
	const int GetCurrentCompression() const;
	CString GetManufacturerFromFormatTag(WORD wFormatTAg);
	CAudioCaptureFilter *m_pAudioCapFilter;
	CAudioCompressorFilter *m_pAudioCompFilter;
	std::vector<WAVEFORMATEX*> m_vectorCompressionFormats;

public:
	afx_msg void OnBnClickedAcconfig();
	afx_msg void OnEnChangeCompressedformattag();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnSelchangeAudiocompressor();
	afx_msg void OnCbnSelchangeSupportedformat();
	afx_msg void OnCbnSelchangeInputdevice();
	afx_msg void OnBnClickedVolume();
	afx_msg void OnBnClickedConfigure();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUDIOFORMAT_H__D1098020_67C5_491D_AC06_2EE69A7C135D__INCLUDED_)
