// BasicMessage.h : interface of the CBasicMessage class
//
/////////////////////////////////////////////////////////////////////////////
#if !defined(_BASICMESSAGE_H__DCC4865E_3B37_402E_AC1B_C8ABF4519F51__INCLUDED_)
#define _BASICMESSAGE_H__DCC4865E_3B37_402E_AC1B_C8ABF4519F51__INCLUDED_

#pragma once
#include "afxwin.h"
#include "resource.h"			//Added by janhgm, otherwise IDD_BASICMESSAGE is undeclared identifier

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

#endif // !defined(_BASICMESSAGE_H__DCC4865E_3B37_402E_AC1B_C8ABF4519F51__INCLUDED_)
