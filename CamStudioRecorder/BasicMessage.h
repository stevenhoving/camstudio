#pragma once
#include <afxwin.h>
#include "resource.h" //Added by janhgm, otherwise IDD_BASICMESSAGE is undeclared identifier

// CBasicMessage dialog

class CBasicMessage : public CDialog
{
    DECLARE_DYNAMIC(CBasicMessage)

public:
    CBasicMessage(CWnd *pParent = nullptr);
    virtual ~CBasicMessage();
    void SetText(LPCTSTR lpString);
    void SetTitle(LPCTSTR lpString);
    virtual void OnCancel();
    bool Cancelled();
    // Dialog Data
    enum
    {
        IDD = IDD_BASICMESSAGE
    };

protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
    CString strText;

    DECLARE_MESSAGE_MAP()
public:
    CEdit m_MessageText;
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

private:
    bool m_bCancelled;
};

