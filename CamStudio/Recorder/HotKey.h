// HotKey.h
// Interface to manage system-wide hot keys.
/////////////////////////////////////////////////////////////////////////////
#pragma once

// Values used with Hotkey in program.
#define HOTKEY_RECORD_START_OR_PAUSE	0
#define HOTKEY_RECORD_STOP				1
#define HOTKEY_RECORD_CANCELSTOP		2
#define HOTKEY_LAYOUT_KEY_NEXT			3
#define HOTKEY_LAYOUT_KEY_PREVIOUS		4
#define HOTKEY_LAYOUT_SHOW_HIDE_KEY		5

#include <vector>

const UINT VK_UNDEFINED = 100000;

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

	const int ID() const			{return m_iID;}
	int ID(int iID);

	const UINT Modifier() const		{return m_uModifier;}
	UINT SetModifier(UINT uModifier);
	UINT ClearModifier(UINT uModifier);
	
	const UINT VirtualKey() const	{return m_uVirtualKey;}
	UINT VirtualKey(UINT uVirtualKey);

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

struct sHotKeyDef
{
public:
	sHotKeyDef(UINT vKey = 0, bool bCtrl = false, bool bAlt = false, bool bShift = false)
		: m_vKey(vKey)
		, m_bCtrl(bCtrl)
		, m_bAlt(bAlt)
		, m_bShift(bShift)
	{
	}

	sHotKeyDef(const sHotKeyDef& rhs)
	{
		*this = rhs;
	}
	sHotKeyDef& operator=(const sHotKeyDef& rhs)
	{
		if (this == &rhs)
			return *this;

		m_vKey		= rhs.m_vKey;
		m_bCtrl		= rhs.m_bCtrl;
		m_bAlt		= rhs.m_bAlt;
		m_bShift	= rhs.m_bShift;

		return *this;
	}

	UINT m_vKey;
	bool m_bCtrl;
	bool m_bAlt;
	bool m_bShift;
};
struct sHotKeyOpts
{
	sHotKeyOpts()
		: m_RecordStart(VK_F8)
		, m_RecordEnd(VK_F9)
		, m_RecordCancel(VK_F10)
		, m_Next(VK_F11, true)
		, m_Prev(VK_UNDEFINED, true)
		, m_ShowLayout(VK_UNDEFINED)
	{
	}
	sHotKeyOpts(const sHotKeyOpts& rhs)
	{
		*this = rhs;
	}
	sHotKeyOpts& operator=(const sHotKeyOpts& rhs)
	{
		if (this == &rhs)
			return *this;

		m_RecordStart	= rhs.m_RecordStart;
		m_RecordEnd		= rhs.m_RecordEnd;
		m_RecordCancel	= rhs.m_RecordCancel;
		m_Next			= rhs.m_Next;
		m_Prev			= rhs.m_Prev;
		m_ShowLayout	= rhs.m_ShowLayout;

		return *this;
	}
	bool Read(CProfile& cProfile)
	{
		VERIFY(cProfile.Read(KEYRECORDSTART, m_RecordStart.m_vKey));
		VERIFY(cProfile.Read(KEYRECORDSTARTCTRL, m_RecordStart.m_bCtrl));
		VERIFY(cProfile.Read(KEYRECORDSTARTALT, m_RecordStart.m_bAlt));
		VERIFY(cProfile.Read(KEYRECORDSTARTSHIFT, m_RecordStart.m_bShift));
		
		VERIFY(cProfile.Read(KEYRECORDEND, m_RecordEnd.m_vKey));
		VERIFY(cProfile.Read(KEYRECORDENDCTRL, m_RecordEnd.m_bCtrl));
		VERIFY(cProfile.Read(KEYRECORDENDALT, m_RecordEnd.m_bAlt));
		VERIFY(cProfile.Read(KEYRECORDENDSHIFT, m_RecordEnd.m_bShift));

		VERIFY(cProfile.Read(KEYRECORDCANCEL, m_RecordCancel.m_vKey));
		VERIFY(cProfile.Read(KEYRECORDCANCELCTRL, m_RecordCancel.m_bCtrl));
		VERIFY(cProfile.Read(KEYRECORDCANCELALT, m_RecordCancel.m_bAlt));
		VERIFY(cProfile.Read(KEYRECORDCANCELSHIFT, m_RecordCancel.m_bShift));

		VERIFY(cProfile.Read(KEYNEXT, m_Next.m_vKey));
		VERIFY(cProfile.Read(KEYNEXTCTRL, m_Next.m_bCtrl));
		VERIFY(cProfile.Read(KEYNEXTALT, m_Next.m_bAlt));
		VERIFY(cProfile.Read(KEYNEXTSHIFT, m_Next.m_bShift));

		VERIFY(cProfile.Read(KEYPREV, m_Prev.m_vKey));
		VERIFY(cProfile.Read(KEYPREVCTRL, m_Prev.m_bCtrl));
		VERIFY(cProfile.Read(KEYPREVALT, m_Prev.m_bAlt));
		VERIFY(cProfile.Read(KEYPREVSHIFT, m_Prev.m_bShift));

		VERIFY(cProfile.Read(KEYSHOWLAYOUT, m_ShowLayout.m_vKey));
		VERIFY(cProfile.Read(KEYSHOWLAYOUTCTRL, m_ShowLayout.m_bCtrl));
		VERIFY(cProfile.Read(KEYSHOWLAYOUTALT, m_ShowLayout.m_bAlt));
		VERIFY(cProfile.Read(KEYSHOWLAYOUTSHIFT, m_ShowLayout.m_bShift));
		return true;
	}
	bool Write(CProfile& cProfile)
	{
		VERIFY(cProfile.Write(KEYRECORDSTART, m_RecordStart.m_vKey));
		VERIFY(cProfile.Write(KEYRECORDSTARTCTRL, m_RecordStart.m_bCtrl));
		VERIFY(cProfile.Write(KEYRECORDSTARTALT, m_RecordStart.m_bAlt));
		VERIFY(cProfile.Write(KEYRECORDSTARTSHIFT, m_RecordStart.m_bShift));
		
		VERIFY(cProfile.Write(KEYRECORDEND, m_RecordEnd.m_vKey));
		VERIFY(cProfile.Write(KEYRECORDENDCTRL, m_RecordEnd.m_bCtrl));
		VERIFY(cProfile.Write(KEYRECORDENDALT, m_RecordEnd.m_bAlt));
		VERIFY(cProfile.Write(KEYRECORDENDSHIFT, m_RecordEnd.m_bShift));

		VERIFY(cProfile.Write(KEYRECORDCANCEL, m_RecordCancel.m_vKey));
		VERIFY(cProfile.Write(KEYRECORDCANCELCTRL, m_RecordCancel.m_bCtrl));
		VERIFY(cProfile.Write(KEYRECORDCANCELALT, m_RecordCancel.m_bAlt));
		VERIFY(cProfile.Write(KEYRECORDCANCELSHIFT, m_RecordCancel.m_bShift));

		VERIFY(cProfile.Write(KEYNEXT, m_Next.m_vKey));
		VERIFY(cProfile.Write(KEYNEXTCTRL, m_Next.m_bCtrl));
		VERIFY(cProfile.Write(KEYNEXTALT, m_Next.m_bAlt));
		VERIFY(cProfile.Write(KEYNEXTSHIFT, m_Next.m_bShift));

		VERIFY(cProfile.Write(KEYPREV, m_Prev.m_vKey));
		VERIFY(cProfile.Write(KEYPREVCTRL, m_Prev.m_bCtrl));
		VERIFY(cProfile.Write(KEYPREVALT, m_Prev.m_bAlt));
		VERIFY(cProfile.Write(KEYPREVSHIFT, m_Prev.m_bShift));

		VERIFY(cProfile.Write(KEYSHOWLAYOUT, m_ShowLayout.m_vKey));
		VERIFY(cProfile.Write(KEYSHOWLAYOUTCTRL, m_ShowLayout.m_bCtrl));
		VERIFY(cProfile.Write(KEYSHOWLAYOUTALT, m_ShowLayout.m_bAlt));
		VERIFY(cProfile.Write(KEYSHOWLAYOUTSHIFT, m_ShowLayout.m_bShift));
		return true;
	}

	sHotKeyDef m_RecordStart;
	sHotKeyDef m_RecordEnd;
	sHotKeyDef m_RecordCancel;
	sHotKeyDef m_Next;
	sHotKeyDef m_Prev;
	sHotKeyDef m_ShowLayout;
};

extern sHotKeyOpts cHotKeyOpts;
