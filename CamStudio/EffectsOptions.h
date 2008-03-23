#pragma once
//#include "annotationeffectsoptions.h"


// CEffectsOptions dialog

enum Position {
	TOP_LEFT = 0,
	TOP_CENTER = 1,
	TOP_RIGHT = 2,
	CENTER_LEFT = 3,
	CENTER_CENTER = 4,
	CENTER_RIGHT = 5,
	BOTTOM_LEFT = 6,
	BOTTOM_CENTER = 7,
	BOTTOM_RIGHT = 8,
};

struct TextAttributes
{
	Position position;
	CString text;
	COLORREF backgroundColor;
	COLORREF textColor;
	int isFontSelected;
	LOGFONT logfont;
	const TextAttributes& operator=(const TextAttributes& obj)
	{
		text = obj.text;
		memcpy(&logfont, &obj.logfont, sizeof(LOGFONT));
		isFontSelected = obj.isFontSelected;
		backgroundColor = obj.backgroundColor;
		textColor = obj.textColor;
		position = obj.position;
		return *this;
	}
};

class CEffectsOptions : public CDialog
{
	DECLARE_DYNAMIC(CEffectsOptions)

public:
	CEffectsOptions(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEffectsOptions();

// Dialog Data
	enum { IDD = IDD_EFFECTS_OPTIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonBackgroundColor();
	afx_msg void OnBnClickedButtonTextColor();
	afx_msg void OnBnClickedButtonFont();
	//int m_Position;
	TextAttributes m_params;
};
