#pragma once

#include <afxwin.h>
#include "resource.h" //Added by janhgm, otherwise IDD_BASICMESSAGE is undeclared identifier

class CBasicMessageDlg : public CDialog
{
    DECLARE_DYNAMIC(CBasicMessageDlg)

public:
    CBasicMessageDlg(CWnd *pParent = nullptr);
    virtual ~CBasicMessageDlg();
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

