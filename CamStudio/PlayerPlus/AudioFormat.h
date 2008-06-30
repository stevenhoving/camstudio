#if !defined(AFX_AUDIOFORMAT_H__D1098020_67C5_491D_AC06_2EE69A7C135D__INCLUDED_)
#define AFX_AUDIOFORMAT_H__D1098020_67C5_491D_AC06_2EE69A7C135D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AudioFormat.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AudioFormat dialog

class AudioFormat : public CDialog
{
// Construction
public:
	AudioFormat(CWnd* pParent = NULL);   // standard constructor
	void UpdateLocalCompressFormatInterface();
	void UpdateDeviceData(UINT deviceID, DWORD curr_sel_rec_format, LPWAVEFORMATEX  curr_sel_pwfx);

// Dialog Data
	//{{AFX_DATA(AudioFormat)
	enum { IDD = IDD_AUDIODIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AudioFormat)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AudioFormat)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnChoose();
	afx_msg void OnSelchangeRecordformat();
	afx_msg void OnInterleave();
	virtual void OnCancel();
	afx_msg void OnVolume();
	afx_msg void OnSelchangeInputdevice();
	afx_msg void OnInterleaveframes();
	afx_msg void OnInterleaveseconds();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUDIOFORMAT_H__D1098020_67C5_491D_AC06_2EE69A7C135D__INCLUDED_)
