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

extern UINT keyRecordStart;
extern UINT keyRecordEnd;
extern UINT uKeyRecordCancel;

extern UINT keyRecordStartCtrl;
extern UINT keyRecordEndCtrl;
extern UINT keyRecordCancelCtrl;
extern UINT keyRecordStartAlt;
extern UINT keyRecordEndAlt;
extern UINT keyRecordCancelAlt;
extern UINT keyRecordStartShift;
extern UINT keyRecordEndShift;
extern UINT keyRecordCancelShift;

extern UINT keyNext;
extern UINT keyPrev;
extern UINT keyShowLayout;
extern UINT keyNextCtrl;
extern UINT keyPrevCtrl;
extern UINT keyShowLayoutCtrl;
extern UINT keyNextAlt;
extern UINT keyPrevAlt;
extern UINT keyShowLayoutAlt;
extern UINT keyNextShift;
extern UINT keyPrevShift;
extern UINT keyShowLayoutShift;
