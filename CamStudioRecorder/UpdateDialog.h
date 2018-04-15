#pragma once

#include <afxdialogex.h>

class CUpdateDialog : public CDialogEx
{
    DECLARE_DYNAMIC(CUpdateDialog)
public:
    explicit CUpdateDialog(CWnd *pParent = nullptr);
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
