// IconData.h : header file

#pragma once

class CIconData;

// Synomym for CIconData pointer
typedef CIconData* IconDataPtr;

// CIconData

class CIconData: public CObject
{
public:

	// Quad parameter constructor
	CIconData(	unsigned int uDefaultIconID, 
				LPSTR lpToolTip,
				unsigned int uHoverIconID = 0, 
				HICON hIcon = NULL)
	{
		SetDefaultIconID(uDefaultIconID);	
		SetHoverIconID(uHoverIconID);
		SetToolTip(lpToolTip);		
		SetIconHandler(hIcon);	
		LoadIcon(GetDefaultIconID());
	}

	// Copy constructor
	CIconData(const CIconData &pIconData)
	{
		CIconData(pIconData.m_uDefaultIconID, pIconData.m_lpToolTip, pIconData.m_uHoverIconID, pIconData.m_hIcon);
	}


	// Gets the selected icon's identifer
	unsigned int GetIconID() const
	{
		return m_uIconID;
	}

	// Gets the default icon identfier
	unsigned int GetDefaultIconID() const
	{
		return m_uDefaultIconID;
	}

	// Sets the default icon identifier
	void SetDefaultIconID(unsigned int uIconID)
	{
		m_uDefaultIconID = uIconID;		
	}

	// Gets the hover icon identifer
	unsigned int GetHoverIconID() const
	{
		return m_uHoverIconID;
	}

	// Sets the hover icon identifier
	void SetHoverIconID(unsigned int uIconID)
	{
		m_uHoverIconID = uIconID;
	}

	// Gets the tooltip
	LPSTR GetToolTip() const
	{
		return m_lpToolTip;
	}

	// Sets the tooltip
	void SetToolTip(LPSTR lpToolTip)
	{
		m_lpToolTip = lpToolTip;
		/*m_lpToolTip = new char[strlen(lpToolTip)];
		strcpy(m_lpToolTip, lpToolTip);*/
	}

	// Gets the icon handler
	HICON GetIconHandler() const
	{
		return m_hIcon;
	}

	// Selects and loads the speciifed icon into memory
	bool LoadIcon(unsigned int uIconID)
	{
		if(uIconID == m_uDefaultIconID || uIconID == m_uHoverIconID)
		{
			SetIconID(uIconID);
			SetIconHandler(AfxGetApp()->LoadIcon(GetIconID()));
			return true;
		}

		return false;
	}

	// Desctructor
	~CIconData()
	{
		//delete[] m_lpToolTip;
		m_lpToolTip = 0;
	}

private:

	CIconData(){}

	void SetIconID(unsigned int uIconID)
	{
		if(uIconID != 0)
			m_uIconID = uIconID;		
	}

	void SetIconHandler(HICON hIcon)
	{
		if(hIcon != NULL)
		{
			if(m_hIcon != NULL)
				DestroyIcon(m_hIcon);
			m_hIcon = hIcon;
		}
	}

	unsigned int m_uIconID;

	unsigned int m_uDefaultIconID;

	unsigned int m_uHoverIconID;	

	HICON m_hIcon;

	LPSTR m_lpToolTip;
};
