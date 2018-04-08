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

    TextAttributes m_xnote;
    ULONG m_ulXnoteCameraDelayInMilliSec;

    CButton m_CheckBoxXnoteRemoteControlMode;
    bool m_bXnoteRemoteControlMode;        // A bool but says if exteral conttol functions as Xnote Stopwatch and Video Motion Alerter are enabled.

    CButton m_CheckBoxXnoteDisplayCameraDelayMode;
    bool m_bXnoteDisplayCameraDelayMode;            // A bool but dialog requires that checkbox is a CButton.

    CButton m_CheckBoxXnoteDisplayCameraDelayDirection;
    bool m_bXnoteDisplayCameraDelayDirection;            // A bool but dialog requires that checkbox is a CButton.

    CString m_cXnoteDisplayFormatString;

    ULONG m_ulXnoteRecordDurationLimitInMilliSec;

    CButton m_CheckBoxXnoteRecordDurationLimitMode;
    bool m_bXnoteRecordDurationLimitMode;    // A bool but dialog requires that checkbox is a CButton.

    TextAttributes m_caption;
    ImageAttributes m_image;
protected:
private:
    CBitmapButton m_buttonImagePath;
    CEdit m_ctlImagePath;

    afx_msg void OnBnClickedButtonImagePath();
    
    afx_msg void OnBnClickedButtonCaptionOptions();
    afx_msg void OnBnClickedButtonTimestampFormatOptions();
    afx_msg void OnBnClickedButtonXNoteFormatOptions();
    afx_msg void OnBnClickedButtonWatermarkOptions();
public:
    CEdit m_FormatTimestampPreview; // Fields to preview calculated output
    CEdit m_FormatXNotePreview; // Fields to preview calculated output
    afx_msg void OnEnChangeEditTimestampFormat();
    afx_msg void OnEnChangeFormatpreview();
    afx_msg void OnEnChangeEditXNoteCameraDelayInMilliSec();
    afx_msg void OnBnClickedButtonXnoteRemoteControlMode();
    afx_msg void OnBnClickedXNoteDisplayCameraDelayMode();
    afx_msg void OnBnClickedXNoteDisplayCameraDelayDirection();

    afx_msg void OnEnChangeEditXnoteRecordDurationLimitInMilliSec();
    afx_msg void OnBnClickedXnoteRecordDurationLimitMode();

    afx_msg void OnBnClickedOk();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
private:
    bool IsStrftimeSafe(char * pbuffer);    // For user defined date/time layouts

public:
    // The preview field used to display delay and xnote combination in the settings dialog
    afx_msg void OnEnChangeFormatXnotepreview();
    //afx_msg void OnEnChangeEditXnoteFormat();
};
