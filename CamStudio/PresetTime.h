#if !defined(AFX_PRESETTIME_H__F8B0041D_C4EB_4568_AC06_E5D330F4408F__INCLUDED_)
#define AFX_PRESETTIME_H__F8B0041D_C4EB_4568_AC06_E5D330F4408F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PresetTime.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPresetTime dialog

class CPresetTime : public CDialog
{
// Construction
public:
	CPresetTime(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPresetTime)
	enum { IDD = IDD_PRESETTIME };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPresetTime)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPresetTime)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRESETTIME_H__F8B0041D_C4EB_4568_AC06_E5D330F4408F__INCLUDED_)
