// CamFile.cpp	- CamStudio Library file functions
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "CamFile.h"

/////////////////////////////////////////////////////////////////////////////
// GetTempPath
// n.b. Overloads DWORD GetTempPath(DWORD nBufferLength, LPTSTR lpBuffer)
// There is a conversion from LPTSTR to CString
// TODO: Rename; this is needlessly dangerous
// TODO GetTempFolder does not contain the temp folder but returns the udf target folder. Hence the function name is not correct..!
/////////////////////////////////////////////////////////////////////////////
CString GetTempFolder(int iAccess, const CString strFolder)
{
	if (iAccess == USE_WINDOWS_TEMP_DIR) {
		TCHAR dirx[_MAX_PATH];
		::GetWindowsDirectory(dirx, _MAX_PATH);
		CString tempdir;
		tempdir.Format(_T("%s\\temp"), dirx);

		// Verify the chosen temp path is valid

		WIN32_FIND_DATA wfd;
		::ZeroMemory(&wfd, sizeof (wfd));
		HANDLE hdir = ::FindFirstFile(LPCTSTR(tempdir), &wfd);
		if (!hdir) {
			return GetProgPath();
		}
		::FindClose(hdir);

		// If valid directory, return Windows\temp as temp directory
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			return tempdir;
		}

		//else return program path as temp directory
		//iAccess = USE_INSTALLED_DIR;	// TODO: nonsense; iAccess is NOT an output
		return GetProgPath();
	}
	
	if (iAccess == USE_USER_SPECIFIED_DIR) {
		CString tempdir = strFolder;

		//Verify the chosen temp path is valid
		WIN32_FIND_DATA wfd;
		::ZeroMemory(&wfd, sizeof (wfd));
		HANDLE hdir = ::FindFirstFile(LPCTSTR(tempdir), &wfd);
		if (!hdir) {
			return GetProgPath();
		}
		::FindClose(hdir);

		//If valid directory, return Windows\temp as temp directory
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			return tempdir;
		}

		//set temp path to installed directory
		//iAccess = USE_INSTALLED_DIR;	// TODO: nonsense; iAccess is NOT an output
		return GetProgPath();
	}

	// default returns program folder
	return GetProgPath();
}

CString GetProgPath()
{
	// get root of Camstudio application
	TCHAR szTemp[300];
	::GetModuleFileName(NULL, szTemp, 300);
	CString path = szTemp;
	if (0 < path.ReverseFind('\\')) {
		path = path.Left(path.ReverseFind('\\'));
	}
	return path;
}

CString FindExtension(const CString& name)
{
	int len = name.GetLength();
	for (int i = len-1; i >= 0; i--) {
		if (name[i] == '.') {
			return name.Mid(i+1);
		}
	}
	return "";
}
