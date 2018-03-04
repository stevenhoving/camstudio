#pragma once

class AudioFormat : public CDialog
{
public:
    AudioFormat(CWnd *pParent = NULL);
    void UpdateLocalCompressFormatInterface();
    void UpdateDeviceData(UINT deviceID, DWORD curr_sel_rec_format, LPWAVEFORMATEX curr_sel_pwfx);

    // Dialog Data
    //{{AFX_DATA(AudioFormat)
    enum
    {
        IDD = IDD_AUDIODIALOG
    };
    // NOTE: the ClassWizard will add data members here
    //}}AFX_DATA

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(AudioFormat)
protected:
    virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(AudioFormat)
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    afx_msg void OnChoose();
    afx_msg void OnSelchangeRecordformat();
    afx_msg void OnInterleave();
    virtual void OnCancel();
    afx_msg void OnVolume();
    afx_msg void OnSelchangeInputdevice();
    afx_msg void OnInterleaveframes();
    afx_msg void OnInterleaveseconds();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
