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
#define HOTKEY_ZOOM						6
#define HOTKEY_AUTOPAN_SHOW_HIDE_KEY	7

#include <vector>

const UINT VK_UNDEFINED = 100000;

struct sHotKeyDef
{
public:
	sHotKeyDef(UINT vKey = 0, UINT fsMod = 0)
		: m_vKey(vKey)
		, m_fsMod(fsMod)
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
		m_fsMod		= rhs.m_fsMod;

		return *this;
	}

	UINT m_vKey;
	UINT m_fsMod;
};
struct sHotKeyOpts
{
	sHotKeyOpts()
		: m_RecordStart(VK_F8)
		, m_RecordEnd(VK_F9)
		, m_RecordCancel(VK_F10)
		, m_Next(VK_F11, MOD_CONTROL)
		, m_Prev(VK_UNDEFINED, MOD_CONTROL)
		, m_ShowLayout(VK_UNDEFINED)
		, m_Zoom(VK_UNDEFINED)
		, m_Autopan(VK_UNDEFINED)
	{
	}
	sHotKeyOpts(const sHotKeyOpts& rhs)
	{
		*this = rhs;
	}
	// TODO: default copy constructor??
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
		m_Zoom			= rhs.m_Zoom;
		m_Autopan		= rhs.m_Autopan;

		return *this;
	}
	bool Read(Setting& cProfile);
	bool Write(Setting& cProfile);

	sHotKeyDef m_RecordStart;
	sHotKeyDef m_RecordEnd;
	sHotKeyDef m_RecordCancel;
	sHotKeyDef m_Next;
	sHotKeyDef m_Prev;
	sHotKeyDef m_ShowLayout;
	sHotKeyDef m_Zoom;
	sHotKeyDef m_Autopan;
};

extern sHotKeyOpts cHotKeyOpts;
