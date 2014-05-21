#pragma once


// CVideoSourceDialog dialog

class CVideoSourceDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CVideoSourceDialog)

public:
	CVideoSourceDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CVideoSourceDialog();

// Dialog Data
	enum { IDD = IDD_VIDEOSOURCE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	CComboBox m_ctrlCBVideoSource;

};
