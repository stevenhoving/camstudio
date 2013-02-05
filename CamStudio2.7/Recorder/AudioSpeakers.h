// CAudioSpeakersDlg.h : header file
//
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_AUDIOSPEAKERS_H__125948FB_55C2_4D8E_8E21_36B3AE266E28__INCLUDED_)
#define AFX_AUDIOSPEAKERS_H__125948FB_55C2_4D8E_8E21_36B3AE266E28__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CAudioSpeakersDlg dialog

class CAudioSpeakersDlg : public CDialog
{
// Construction
public:
	CAudioSpeakersDlg(CWnd* pParent = NULL);   // standard constructor
	void OnUpdateLineConfig(int line_proper);
	void OnUpdateVolume();

// Dialog Data
	//{{AFX_DATA(CAudioSpeakersDlg)
	enum { IDD = IDD_AUDIOWAVEOUT };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAudioSpeakersDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAudioSpeakersDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnVolume();
	afx_msg void OnAutoconfig();
	afx_msg void OnSelchangeSounddevice();
	afx_msg void OnManualconfig();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CSliderCtrl m_ctrlSliderVolume;
	CStatic m_ctrlStaticVolume;
	CStatic m_ctrlStaticLineInfo;
	CComboBox m_ctrlCBSoundDevice;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUDIOSPEAKERS_H__125948FB_55C2_4D8E_8E21_36B3AE266E28__INCLUDED_)
