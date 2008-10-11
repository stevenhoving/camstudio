// HotKey.h
// Interface to manage system-wide hot keys.
/////////////////////////////////////////////////////////////////////////////
#pragma once

#include <vector>

/////////////////////////////////////////////////////////////////////////////
// CHotKey class
// Defines behavior for a system-wide hot key.
/////////////////////////////////////////////////////////////////////////////
class CHotKey
{
	CHotKey();	// not implemented
public:
	CHotKey(int iID, UINT uModifier, UINT uVirtualKey);
	virtual ~CHotKey();

	BOOL Register(HWND hWnd);
	BOOL Unregister();

	const int ID() const
	{
		return m_iID;
	}
	int ID(int iID)
	{
		if (m_bRegistered)
		{
			HWND hWndOld = m_hWnd;
			Unregister();
			m_iID = iID;
			VERIFY(Register(hWndOld));
			return m_iID;
		}
		return m_iID = iID;
	}
	const UINT Modifier() const
	{
		return m_uModifier;
	}
	UINT SetModifier(UINT uModifier)
	{
		if (m_bRegistered)
		{
			HWND hWndOld = m_hWnd;
			Unregister();
			m_uModifier |= uModifier;
			VERIFY(Register(hWndOld));
			return m_uModifier;
		}
		return m_uModifier |= uModifier;
	}
	UINT ClearModifier(UINT uModifier)
	{
		if (m_bRegistered)
		{
			HWND hWndOld = m_hWnd;
			Unregister();
			m_uModifier ^= ~uModifier;
			VERIFY(Register(hWndOld));
			return m_uModifier;
		}
		return m_uModifier ^= ~uModifier;
	}
	
	const UINT VirtualKey() const
	{
		return m_uVirtualKey;
	}
	UINT VirtualKey(UINT uVirtualKey)
	{
		if (m_bRegistered)
		{
			HWND hWndOld = m_hWnd;
			Unregister();
			m_uVirtualKey = uVirtualKey;
			VERIFY(Register(hWndOld));
			return m_uVirtualKey;
		}
		return m_uVirtualKey = uVirtualKey;
	}

private:
	int m_iID;				// hotkey identifier
	UINT m_uModifier;		// additional keys (ALT, CTRL, SHIFT, WIN)
	UINT m_uVirtualKey;		// virtual key of the hotkey
	HWND m_hWnd;			// window associated with hotkey
	bool m_bRegistered;		// flag if key registered
};

/////////////////////////////////////////////////////////////////////////////
// CHotKeyList class
// Defines behavior for a list of system-wide hot keys.
/////////////////////////////////////////////////////////////////////////////
class CHotKeyList
{
public:
	CHotKeyList();
	virtual ~CHotKeyList();

	void AddKey(CHotKey & HotKey);
	bool Register(HWND hWnd);
	bool Unregister();
private:
	HWND m_hWnd;			// window associated with hotkey list
	std::vector <CHotKey> m_vHotKeys;
};

