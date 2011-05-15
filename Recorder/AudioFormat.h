// CAudioFormatDlg.h : header file
//
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_AUDIOFORMAT_H__D1098020_67C5_491D_AC06_2EE69A7C135D__INCLUDED_)
#define AFX_AUDIOFORMAT_H__D1098020_67C5_491D_AC06_2EE69A7C135D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CAudioFormatDlg dialog

class CAudioFormatDlg : public CDialog
{
	// Construction
	CAudioFormatDlg(CWnd* pParent = NULL);   // standard constructor; disallowed
public:
	CAudioFormatDlg(const sAudioFormat& cFmt, CWnd* pParent = NULL);   // standard constructor

	const sAudioFormat& Format() const	{return m_cFmt;}

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
	afx_msg void OnInterleave();
	virtual void OnCancel();
	afx_msg void OnVolume();
	afx_msg void OnSelchangeInputdevice();
	afx_msg void OnInterleaveframes();
	afx_msg void OnInterleaveseconds();
	afx_msg void OnSystemrecord();
	afx_msg void OnHelp();
	//}}AFX_MSG
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
	CEdit m_ctrlEditFactor;
	CButton m_ctrlButtonInterleave;
	CButton m_ctrlButtonInterleaveFrames;
	CButton m_ctrlButtonInterleaveSeconds;
	CComboBox m_ctrlCBRecordFormat;
	CComboBox m_ctrlCBInputDevice;
	CButton m_ctrlButtonSystemRecord;
	CEdit m_ctrlEditCompressedFormatTag;
	CEdit m_ctrlEditCompressedFormat;
	CButton m_ctrlButtonChooseCompressedFormat;

	bool LoadDeviceList();
	bool FillFormatList();
	bool LoadFormatList();

	void AllocCompressFormat();
	void BuildLocalRecordingFormat(WAVEFORMATEX& rsWaveFormat);
	void SuggestLocalCompressFormat();
	BOOL GetFormatDescription(CString& rstrFormatTag, CString& rstrFormat);
	void UpdateCompressFormatInterface();
	void UpdateDeviceData(UINT deviceID, DWORD dwFormat, const WAVEFORMATEX& rwfx);

	BOOL OpenUsingRegisteredClass (CString link);
	BOOL Openlink (CString);
	BOOL OpenUsingShellExecute (CString);
	LONG GetRegKey (HKEY key, LPCTSTR subkey, LPTSTR retdata);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUDIOFORMAT_H__D1098020_67C5_491D_AC06_2EE69A7C135D__INCLUDED_)
