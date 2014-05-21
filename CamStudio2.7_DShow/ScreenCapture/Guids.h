/**********************************************
 *  File: Guids.h
 *  Desc: GUID for CamStudio Screen Capture
 *  Author: Alberto A. Heredia (bertoso)
 *
 **********************************************/

#pragma once

#ifndef __PUSHGUIDS_DEFINED
#define __PUSHGUIDS_DEFINED

#ifdef _WIN64
DEFINE_GUID(CLSID_PushSourceDesktop, 
0x4ea69364, 0x2c8a, 0x4ae6, 0xa5, 0x61, 0x56, 0xe4, 0xb5, 0x4, 0x44, 0x39);

#else

// {74553DBE-EC18-484d-87D4-96705CC1AB6B}
DEFINE_GUID(CLSID_PushSourceDesktop, 
0x74553dbe, 0xec18, 0x484d, 0x87, 0xd4, 0x96, 0x70, 0x5c, 0xc1, 0xab, 0x6b);


#endif

#endif
