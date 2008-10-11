#include "StdAfx.h"
#include "Recorder.h"
#include "HotKey.h"

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CHotKey::CHotKey(int iID, UINT uModifier, UINT uVirtualKey)
: m_iID(iID)
, m_uModifier(uModifier)
, m_uVirtualKey(uVirtualKey)
, m_hWnd(0)
, m_bRegistered(false)
{
}

CHotKey::~CHotKey()
{
	if (m_bRegistered)
	{
		Unregister();
	}
}

BOOL CHotKey::Register(HWND hWnd)
{
	BOOL bResult = m_bRegistered;	// already registered?
	if (bResult)
	{
		return !bResult;	// fail
	}
	bResult = ::RegisterHotKey(hWnd, m_iID, m_uModifier, m_uVirtualKey);
	m_bRegistered = bResult ? true : false; 
	if (!m_bRegistered)
	{
		OnError("CHotKey::Register");
	}
	m_hWnd = m_bRegistered ? hWnd : 0;

	return bResult;
}

BOOL CHotKey::Unregister()
{
	BOOL bResult = m_bRegistered;	// already registered?
	if (!bResult)
	{
		return !bResult;	// fail
	}
	ASSERT(m_hWnd);
	bResult = ::UnregisterHotKey(m_hWnd, m_iID);
	m_bRegistered = bResult ? false : true; 
	if (m_bRegistered)
	{
		OnError("CHotKey::Unregister");
	}
	m_hWnd = m_bRegistered ? m_hWnd : 0;

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CHotKeyList::CHotKeyList()
{
}

CHotKeyList::~CHotKeyList()
{
}

void CHotKeyList::AddKey(CHotKey & HotKey)
{
	m_vHotKeys.push_back(HotKey);
}

bool CHotKeyList::Register(HWND hWnd)
{
	bool bResult = (0 != hWnd);
	if (!bResult)
	{
		return bResult;
	}

	m_hWnd = hWnd;
	for (std::vector<CHotKey>::iterator iter = m_vHotKeys.begin(); bResult && (iter != m_vHotKeys.end()); ++iter)
	{
		bResult = bResult && iter->Register(m_hWnd);
	}
	return bResult;
}

bool CHotKeyList::Unregister()
{
	bool bResult = true;
	for (std::vector<CHotKey>::iterator iter = m_vHotKeys.begin(); bResult && (iter != m_vHotKeys.end()); ++iter)
	{
		bResult = bResult && iter->Unregister();
	}
	m_hWnd = bResult ? 0 : m_hWnd;
	return bResult;
}
