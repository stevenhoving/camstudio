#pragma once

#include "EffectsOptions.h"
#include "EffectsOptions2.h"
#include <afxwin.h>

class CAnnotationEffectsOptionsDlg : public CDialog
{
    DECLARE_DYNAMIC(CAnnotationEffectsOptionsDlg)

public:
    CAnnotationEffectsOptionsDlg(CWnd* pParent = nullptr);
    ~CAnnotationEffectsOptionsDlg() override;

// Dialog Data
    enum { IDD = IDD_DIALOG_MAIN_EFFECTS_OPTIONS };

protected:
    void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

    DECLARE_MESSAGE_MAP()
public:
//    virtual BOOL OnInitDialog();
public:
    // The input and output values
    TextAttributes m_timestamp;

    TextAttributes m_caption;
    ImageAttributes m_image;
protected:
private:
    CBitmapButton m_buttonImagePath;
    CEdit m_ctlImagePath;

    afx_msg void OnBnClickedButtonImagePath();
    afx_msg void OnBnClickedButtonCaptionOptions();
    afx_msg void OnBnClickedButtonTimestampFormatOptions();
    afx_msg void OnBnClickedButtonWatermarkOptions();
public:
    CEdit m_FormatTimestampPreview; // Fields to preview calculated output
    afx_msg void OnEnChangeEditTimestampFormat();
    afx_msg void OnEnChangeFormatpreview();
    afx_msg void OnBnClickedOk();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
private:
    bool IsStrftimeSafe(char * pbuffer);    // For user defined date/time layouts
};
