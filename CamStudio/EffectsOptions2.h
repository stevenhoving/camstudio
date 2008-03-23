#pragma once

#include "EffectsOptions.h"

struct ImageAttributes
{
	Position position;
	CString text;
	const ImageAttributes& operator=(const ImageAttributes& obj)
	{
		text = obj.text;
		position = obj.position;
		return *this;
	}
};

// CEffectsOptions2 dialog

class CEffectsOptions2 : public CDialog
{
	DECLARE_DYNAMIC(CEffectsOptions2)

public:
	CEffectsOptions2(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEffectsOptions2();

// Dialog Data
	enum { IDD = IDD_EFFECTS_OPTIONS2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	ImageAttributes m_params;
};
