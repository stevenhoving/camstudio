#pragma once

#include "TextAttributes.h"

#ifndef IDD_EFFECTS_OPTIONS
    #error include 'resource.h' before including this file for dialog resource
#endif

// CEffectsOptionsDlg dialog

class CEffectsOptionsDlg : public CDialog
{
    DECLARE_DYNAMIC(CEffectsOptionsDlg)

public:
    CEffectsOptionsDlg(CWnd* pParent = nullptr);
    ~CEffectsOptionsDlg() override;

// Dialog Data
    enum { IDD = IDD_EFFECTS_OPTIONS };


protected:
    void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
    virtual int EffectsOptionsSetXPosRatio( int nPosition );
    virtual int EffectsOptionsSetYPosRatio( int nPosition );

    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonBackgroundColor();
    afx_msg void OnBnClickedButtonTextColor();
    afx_msg void OnBnClickedButtonFont();
    //int m_Position;
    TextAttributes m_params;
    afx_msg void OnBnClickedOk();
};

