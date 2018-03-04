#pragma once

#include <afxdialogex.h>

// CUpdateDialog dialog

class CUpdateDialog : public CDialogEx
{
    DECLARE_DYNAMIC(CUpdateDialog)

public:
    CUpdateDialog(CWnd *pParent = NULL);
    virtual ~CUpdateDialog();

    // Dialog Data
    enum
    {
        IDD = IDD_DIALOGUPDCS
    };

protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
    DECLARE_MESSAGE_MAP()
    afx_msg void OnBnClickedButtonlink();
};
