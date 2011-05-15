// CamFile.h	- include file for CamStudio Library file api's
/////////////////////////////////////////////////////////////////////////////
#ifndef CAMFILE_H
#define CAMFILE_H

#pragma once

// CamFile.cpp
const int USE_WINDOWS_TEMP_DIR		= 0;
const int USE_INSTALLED_DIR			= 1;
const int USE_USER_SPECIFIED_DIR	= 2;

CString GetTempFolder(int iAccess, const CString strFolder = _T(""));
CString GetProgPath();
CString FindExtension(const CString& name);
CString GetMyVideoPath();
CString GetAppDataPath();
bool    DoesFileExist(const CString& name);

#endif	// CAMFILE_H
