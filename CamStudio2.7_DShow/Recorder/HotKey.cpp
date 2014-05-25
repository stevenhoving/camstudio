#include "StdAfx.h"
#include "HotKey.h"
#include "Profile.h"

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
bool sHotKeyOpts::Read(Setting& cProfile)
{
	std::string hk("Start");
	if(cProfile.exists(hk)) {
		const Setting& s = cProfile[hk];
		s.lookupValue("Key", m_RecordStart.m_vKey);
		s.lookupValue("Mod", m_RecordStart.m_fsMod);
	}
	hk = "End";
	if(cProfile.exists(hk)) {
		const Setting& s = cProfile[hk];
		s.lookupValue("Key", m_RecordEnd.m_vKey);
		s.lookupValue("Mod", m_RecordEnd.m_fsMod);
	}
	hk = "Cancel";
	if(cProfile.exists(hk)) {
		const Setting& s = cProfile[hk];
		s.lookupValue("Key", m_RecordCancel.m_vKey);
		s.lookupValue("Mod", m_RecordCancel.m_fsMod);
	}
	hk = "Next";
	if(cProfile.exists(hk)) {
		const Setting& s = cProfile[hk];
		s.lookupValue("Key", m_Next.m_vKey);
		s.lookupValue("Mod", m_Next.m_fsMod);
	}
	hk = "Prev";
	if(cProfile.exists(hk)) {
		const Setting& s = cProfile[hk];
		s.lookupValue("Key", m_Prev.m_vKey);
		s.lookupValue("Mod", m_Prev.m_fsMod);
	}
	hk = "Layout";
	if(cProfile.exists(hk)) {
		const Setting& s = cProfile[hk];
		s.lookupValue("Key", m_ShowLayout.m_vKey);
		s.lookupValue("Mod", m_ShowLayout.m_fsMod);
	}
	hk = "Zoom";
	if(cProfile.exists(hk)) {
		const Setting& s = cProfile[hk];
		s.lookupValue("Key", m_Zoom.m_vKey);
		s.lookupValue("Mod", m_Zoom.m_fsMod);
	}
	hk = "AutoSpan";
	if(cProfile.exists(hk)){
		const Setting& s = cProfile[hk];
		s.lookupValue("Key", m_Autopan.m_vKey);
		s.lookupValue("Mod", m_Autopan.m_fsMod);
	}
	bool pass = false;
	cProfile.lookupValue("PassThrough", pass);
	//setPassThrough(pass); // see hook.cpp
	return true;
}

bool sHotKeyOpts::Write(Setting& cProfile)
{
	Setting* s;
	std::string hk("Start");
	if(cProfile.exists(hk)) {
		s = &(cProfile[hk]);
	} else
		s = &(cProfile.add(hk, Setting::TypeGroup));
	UpdateSetting(*s, "Key", (int&)m_RecordStart.m_vKey, Setting::TypeInt);
	UpdateSetting(*s, "Mod", (int&)m_RecordStart.m_fsMod, Setting::TypeInt);

	hk = "End";
	if(cProfile.exists(hk)) {
		s = &(cProfile[hk]);
	} else
		s = &(cProfile.add(hk, Setting::TypeGroup));
	UpdateSetting(*s, "Key", (int&)m_RecordEnd.m_vKey, Setting::TypeInt);
	UpdateSetting(*s, "Mod", (int&)m_RecordEnd.m_fsMod, Setting::TypeInt);

	hk = "Cancel";
	if(cProfile.exists(hk)) {
		s = &(cProfile[hk]);
	} else
		s = &(cProfile.add(hk, Setting::TypeGroup));
	UpdateSetting(*s, "Key", (int&)m_RecordCancel.m_vKey, Setting::TypeInt);
	UpdateSetting(*s, "Mod", (int&)m_RecordCancel.m_fsMod, Setting::TypeInt);

	hk = "Next";
	if(cProfile.exists(hk)) {
		s = &(cProfile[hk]);
	} else
		s = &(cProfile.add(hk, Setting::TypeGroup));
	UpdateSetting(*s, "Key", (int&)m_Next.m_vKey, Setting::TypeInt);
	UpdateSetting(*s, "Mod", (int&)m_Next.m_fsMod, Setting::TypeInt);

	hk = "Prev";
	if(cProfile.exists(hk)) {
		s = &(cProfile[hk]);
	} else
		s = &(cProfile.add(hk, Setting::TypeGroup));
	UpdateSetting(*s, "Key", (int&)m_Prev.m_vKey, Setting::TypeInt);
	UpdateSetting(*s, "Mod", (int&)m_Prev.m_fsMod, Setting::TypeInt);

	hk = "Layout";
	if(cProfile.exists(hk)) {
		s = &(cProfile[hk]);
	} else
		s = &(cProfile.add(hk, Setting::TypeGroup));
	UpdateSetting(*s, "Key", (int&)m_ShowLayout.m_vKey, Setting::TypeInt);
	UpdateSetting(*s, "Mod", (int&)m_ShowLayout.m_fsMod, Setting::TypeInt);

	hk = "Zoom";
	if(cProfile.exists(hk)) {
		s = &(cProfile[hk]);
	} else
		s = &(cProfile.add(hk, Setting::TypeGroup));
	UpdateSetting(*s, "Key", (int&)m_Zoom.m_vKey, Setting::TypeInt);
	UpdateSetting(*s, "Mod", (int&)m_Zoom.m_fsMod, Setting::TypeInt);
	
	hk = "AutoSpan";
	if(cProfile.exists(hk)) {
		s = &(cProfile[hk]);
	} else
		s = &(cProfile.add(hk, Setting::TypeGroup));
	UpdateSetting(*s, "Key", (int&)m_Autopan.m_vKey, Setting::TypeInt);
	UpdateSetting(*s, "Mod", (int&)m_Autopan.m_fsMod, Setting::TypeInt);
	
	return true;
}
