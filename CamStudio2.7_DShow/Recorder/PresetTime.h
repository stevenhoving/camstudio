// PresetTime.h : header file
//
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_PRESETTIME_H__F8B0041D_C4EB_4568_AC06_E5D330F4408F__INCLUDED_)
#define AFX_PRESETTIME_H__F8B0041D_C4EB_4568_AC06_E5D330F4408F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// PresetTime.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPresetTimeDlg dialog

class CPresetTimeDlg : public CDialog
{
// Construction
public:
	CPresetTimeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPresetTimeDlg)
	enum { IDD = IDD_PRESETTIME };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPresetTimeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPresetTimeDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CSpinButtonCtrl m_ctrlSpinTime;
	CButton m_ctrlButtonTime;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRESETTIME_H__F8B0041D_C4EB_4568_AC06_E5D330F4408F__INCLUDED_)
