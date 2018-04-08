#pragma once

class CAudioSpeakersDlg : public CDialog
{
public:
    explicit CAudioSpeakersDlg(CWnd *pParent = nullptr);
    void OnUpdateLineConfig(int line_proper);
    void OnUpdateVolume();

    // Dialog Data
    //{{AFX_DATA(CAudioSpeakersDlg)
    enum
    {
        IDD = IDD_AUDIOWAVEOUT
    };
    // NOTE: the ClassWizard will add data members here
    //}}AFX_DATA

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAudioSpeakersDlg)
protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(CAudioSpeakersDlg)
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    afx_msg void OnVolume();
    afx_msg void OnAutoconfig();
    afx_msg void OnSelchangeSounddevice();
    afx_msg void OnManualconfig();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
private:
    CSliderCtrl m_ctrlSliderVolume;
    CStatic m_ctrlStaticVolume;
    CStatic m_ctrlStaticLineInfo;
    CComboBox m_ctrlCBSoundDevice;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


