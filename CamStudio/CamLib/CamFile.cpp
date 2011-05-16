// CamFile.cpp	- CamStudio Library file functions
/////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "CamFile.h"
#include "shlobj.h"

/////////////////////////////////////////////////////////////////////////////
// GetTempPath
// n.b. Overloads DWORD GetTempPath(DWORD nBufferLength, LPTSTR lpBuffer)
// There is a conversion from LPTSTR to CString
// TODO: Rename; this is needlessly dangerous
// TODO GetTempFolder does not contain the temp folder but returns the udf target folder. Hence the function name is not correct..!
/////////////////////////////////////////////////////////////////////////////
CString GetTempFolder(int iAccess, const CString strFolder)
{
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
   }

   char tempPath[_MAX_PATH+1] = { '\0' };
   if (GetTempPath(_MAX_PATH, tempPath))
      {
	   CString tempdir;
      tempdir = tempPath;
      if (0 < tempdir.ReverseFind('\\'))
	      tempdir = tempdir.Left(tempdir.ReverseFind('\\'));
      return tempdir;
      }

   //This code looks for an old style temp directory. NOT standard windows.
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

#ifndef CSIDL_MYVIDEO
#define CSIDL_MYVIDEO                   0x000e        // "My Videos" folder
#endif

CString GetMyVideoPath()
{
   //Get the user's video path
   int folder = CSIDL_MYVIDEO;

   char szPath[MAX_PATH+100]; szPath[0]=0;
	CString path = szPath;

   if (SUCCEEDED(SHGetFolderPath(NULL, folder, 0, 0, szPath)))
      {
      path = szPath;
      }
   else if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL, 0, 0, szPath)))
      {
      path = szPath;
      }
	return path;
}

CString GetAppDataPath()
   {
   int folder = CSIDL_APPDATA;

   char szPath[MAX_PATH+100]; szPath[0]=0;
	CString path = szPath;

   if (SUCCEEDED(SHGetFolderPath(NULL, folder, 0, 0, szPath)))
      {
      path = szPath;
      }
   else if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL, 0, 0, szPath)))
      {
      path = szPath;
      }
	return path;
   }

bool DoesFileExist(const CString& name)
   {
   return (GetFileAttributes(name.GetString()) == 0xffffffff) ? false : true;
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
