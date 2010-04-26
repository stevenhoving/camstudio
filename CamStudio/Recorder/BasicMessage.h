#pragma once
#include "afxwin.h"

// CBasicMessage dialog

class CBasicMessage : public CDialog
{
	DECLARE_DYNAMIC(CBasicMessage)

public:
	CBasicMessage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBasicMessage();
	void SetText(LPCTSTR lpString);
	void SetTitle(LPCTSTR lpString);

// Dialog Data
	enum { IDD = IDD_BASICMESSAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CString strText;

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_MessageText;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
