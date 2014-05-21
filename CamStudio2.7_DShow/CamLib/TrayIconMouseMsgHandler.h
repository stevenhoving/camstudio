// TrayIconMouseMsgHandler.h : header file

#pragma once

class CTrayIconMouseMsgHandler;

// Synomym for CTrayIconMouseMsgHandler pointer
typedef CTrayIconMouseMsgHandler* MouseMsgHandlerPtr;

// CTrayIconMouseMsgHandler

class CTrayIconMouseMsgHandler: public CObject
{
public:

	// Single paramenter constructor, a mosue message identfier has to be passed
	CTrayIconMouseMsgHandler(unsigned int uMouseMsgID)
	{
		m_uMouseMsgID = uMouseMsgID;
	}

	// Pure-virtual callback mouse message handler, necessary to be implemented by the derived class, for handling system tray icon notifications.
	virtual void MouseMsgHandler() = 0;

	// Gets the mouse message identifier
	unsigned int GetMouseMsgID() const
	{
		return m_uMouseMsgID;
	}

	// Sets the mosue message identifier
	void SetMouseMsgID(unsigned int uMouseMsgID)
	{
		m_uMouseMsgID = uMouseMsgID;
	}

	// Virtual descturctor, to be implemented by the derived class if some clean-up has to be performed
	virtual ~CTrayIconMouseMsgHandler(){}

private:
	
	CTrayIconMouseMsgHandler(){}

	unsigned int m_uMouseMsgID;
};