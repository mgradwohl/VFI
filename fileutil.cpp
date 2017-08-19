// Visual File Information
// Copyright (c) Microsoft Corporation
// All rights reserved. 
// 
// MIT License
// 
// Permission is hereby granted, free of charge, to any person obtaining 
// a copy of this software and associated documentation files (the ""Software""), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom 
// the Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included 
// in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED *AS IS*, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS 
// OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR 
// IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <windows.h>
#include <WCHAR.h>
#include <lmcons.h>		// for UNLEN only
#include <stdlib.h>		// _tsplitpath
#include <shlobj.h>
#include <shlwapi.h>
#include <winnetwk.h>

#include "str.h"
#include "fileutil.h"

#ifdef _DEBUG
	#ifdef DEBUG_NEW
		#define new DEBUG_NEW
		#undef THIS_FILE
		static WCHAR THIS_FILE[] = __FILE__;
	#endif
#endif

#ifndef TRACE
	#define TRACE(x) OutputDebugString(x)
#endif

#define TMAX_USERNAME (UNLEN + 1)
	
bool CreateFolder(LPCWSTR pszFolder)
{
	return (0 != CreateDirectory(pszFolder, NULL));
}

bool GetTempFolder(LPWSTR pszFolder)
{
	return (0 != GetTempPath(TMAX_PATH, pszFolder));
}

bool GetWindowsFolder(LPWSTR pszFolder)
{
	return (S_OK == SHGetKnownFolderPath(FOLDERID_Windows, 0, NULL, &pszFolder));
}

bool GetDesktopFolder(LPWSTR pszFolder)
{
	//return ( TRUE == SHGetSpecialFolderPath(NULL, pszFolder, CSIDL_DESKTOPDIRECTORY, FALSE));
	return (S_OK == SHGetFolderPath(NULL, CSIDL_DESKTOPDIRECTORY, NULL, SHGFP_TYPE_CURRENT, pszFolder));
}

bool GetSystemFolder(LPWSTR pszFolder)
{
	//return false; //( TRUE == SHGetSpecialFolderPath(NULL, pszFolder, CSIDL_SYSTEM, FALSE));
	return (S_OK == SHGetFolderPath(NULL, CSIDL_SYSTEM, NULL, SHGFP_TYPE_CURRENT, pszFolder));
}

bool GetRecycleFolder(LPWSTR pszFolder)
{
	return ( TRUE == SHGetSpecialFolderPath(NULL, pszFolder, CSIDL_BITBUCKET, FALSE));
	//return (S_OK == SHGetFolderPath(NULL, CSIDL_BITBUCKET, NULL, SHGFP_TYPE_CURRENT, pszFolder));
}

bool GetDocumentsFolder(LPWSTR pszFolder)
{
	//return ( TRUE == SHGetSpecialFolderPath(NULL, pszFolder, CSIDL_PERSONAL, FALSE));
	return (S_OK == SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, pszFolder));
}

bool GetFontFolder(LPWSTR pszFolder)
{
	return ( TRUE == SHGetSpecialFolderPath(NULL, pszFolder, CSIDL_FONTS, FALSE));
	//return (S_OK == SHGetFolderPath(NULL, CSIDL_FONTS, NULL, SHGFP_TYPE_CURRENT, pszFolder));
}

bool GetProfileFolder(LPWSTR pszFolder)
{
	return ( TRUE == SHGetSpecialFolderPath(NULL, pszFolder, CSIDL_PROFILE, FALSE));
}

bool GetProgramFilesFolder(LPWSTR pszFolder)
{
	//return ( TRUE == SHGetSpecialFolderPath(NULL, pszFolder, CSIDL_PROGRAM_FILES, FALSE));
	return (S_OK == SHGetFolderPath(NULL, CSIDL_PROGRAM_FILES, NULL, SHGFP_TYPE_CURRENT, pszFolder));

}

bool GetUserName(LPWSTR pszUserName)
{
	DWORD dwLen = TMAX_USERNAME;
	return (FALSE != GetUserName(pszUserName, &dwLen));
}

bool DoesFileExist(LPCWSTR pszFileName)
{
	if (NULL == pszFileName)
	{
		return false;
	}

	DWORD dw = ::GetFileAttributes( pszFileName );
	if ( dw == (DWORD)-1 || dw & FILE_ATTRIBUTE_DIRECTORY || dw & FILE_ATTRIBUTE_OFFLINE )
	{
		return false;
	}

	return true;
}

bool DoesFolderExist(LPCWSTR pszFolder)
{
	if (NULL == pszFolder)
	{
		return false;
	}

	DWORD dw = ::GetFileAttributes( pszFolder );
	if ( dw == (DWORD)-1 || dw & FILE_ATTRIBUTE_OFFLINE )
	{
		return false;
	}

	if (dw & FILE_ATTRIBUTE_DIRECTORY)
	{
		return true;
	}

	return false;
}

bool PathGetFolder(LPCWSTR pszFileName, LPWSTR pszFolder)
{

	if (NULL == pszFileName || NULL == pszFolder)
	{
		return false;
	}

	WCHAR szDrive[TMAX_DRIVE];
	WCHAR szDir[TMAX_DIR];
	_wsplitpath_s(pszFileName, szDrive, TMAX_DRIVE, szDir, TMAX_DIR, NULL, 0, NULL, 0);
	//_tsplitpath( pszFileName, szDrive, szDir, NULL, NULL );

	lstrcpy(pszFolder, szDrive);
	lstrcat(pszFolder, szDir);

	return true;
}

bool PathGetFileName(LPCWSTR pszFileSpec, LPWSTR pszFile)
{

	if (NULL == pszFileSpec || NULL == pszFile)
	{
		return false;
	}

	WCHAR szFile[TMAX_FNAME];
	WCHAR szExt[TMAX_EXT];
	_wsplitpath_s(pszFileSpec, NULL, 0, NULL, 0, szFile, TMAX_FNAME, szExt, TMAX_EXT);
	//_tsplitpath( pszFileSpec, NULL, NULL, szFile, szExt);

	lstrcpy(pszFile, szFile);
	lstrcat(pszFile, szExt);

	return true;
}

bool GetModuleFolder(HINSTANCE hInst, LPWSTR pszFolder)
{
	if (NULL == pszFolder)
	{
		return false;
	}

	::GetModuleFileName(hInst, pszFolder, TMAX_PATH);

	WCHAR szDrive[TMAX_DRIVE];
	WCHAR szDir[TMAX_DIR];
	_tsplitpath_s( pszFolder, szDrive, TMAX_DRIVE, szDir, TMAX_DIR, NULL, 0, NULL, 0 );

	lstrcpy(pszFolder, szDrive);
	lstrcat(pszFolder, szDir);

	return true;
}

bool GetLogFolder(LPCWSTR pszAppname, LPWSTR pszFolder)
{
	if (IsBadWritePtr(pszFolder, TMAX_PATH))
	{
		TRACE(L"GetLogFolder pszFolder needs to hold TMAX_PATH characters\r\n");
		return false;
	}

	HKEY hKey;
	DWORD dwType = REG_SZ;
	DWORD dwSize = TMAX_PATH;

	// open the key
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		L"Software\\Microsoft\\Microsoft Games\\Logs", 0, KEY_READ, &hKey))
		goto USE_TEMP;

	WCHAR szModule[TMAX_FNAME];
	if (NULL == pszAppname || lstrlen(pszAppname) < 1)
	{
		GetModuleName(GetModuleHandle(NULL), szModule);
	}
	else
	{
		lstrcpy(szModule, pszAppname);
	}
		
	// read the app specific folder
	dwSize = TMAX_PATH;
	if (ERROR_SUCCESS != (RegQueryValueEx(hKey, szModule, NULL, &dwType, (LPBYTE)pszFolder, &dwSize)))
		goto USE_TEMP;

	if (PathIsLocal(pszFolder))
		goto USE_DEFAULT;

	// the app specific key is remote
	// if the folder doesn't exist, try to use the default
	if (!DoesFolderExist(pszFolder))
	{
		TRACE(L"GetLogFolder remote path does not exist\r\n");
		goto USE_DEFAULT;
	}

	// the app specific key is remote, and the folder exists
	// if the folder isn't writable, try to use the default
	if (!PathIsWritable(pszFolder))
	{
		TRACE(L"GetLogFolder remote path not writable\r\n");
		goto USE_DEFAULT;
	}

	RegCloseKey(hKey);
	TRACE(L"GetLogFolder using app specific folder\r\n");
	goto DONE;

USE_DEFAULT:
	// if the default key doesn't exist, try to use the temp folder
	dwSize = TMAX_PATH;
	if (ERROR_SUCCESS != (RegQueryValueEx(hKey, L"", NULL, &dwType, (LPBYTE)pszFolder, &dwSize)))
		goto USE_TEMP;
	if (lstrlen(pszFolder) < 1)
		goto USE_TEMP;
	TRACE(L"GetLogFolder using default folder\r\n");
	RegCloseKey(hKey);
	goto DONE;

USE_TEMP:
	TRACE(L"GetLogFolder using temp folder\r\n");
	if (!GetTempFolder(pszFolder))
	{
		TRACE(L"GetLogFolder could not get temp folder\r\n");
		return false;
	}
	RegCloseKey(hKey);
	goto DONE;

DONE:
	PathAddBackslash(pszFolder);
	if (!DoesFolderExist(pszFolder))
	{
		TRACE(L"GetLogFolder creating folder\r\n");
		if (!CreateFolder(pszFolder))
		{
			TRACE(L"GetLogFolder could not create folder\r\n");
			return false;
		}
	}
	return true;
}

bool GetLogFileName(LPCWSTR pszFolder, LPCWSTR pszPrefix, LPCSTR pszPostfix, LPWSTR pszFilename)
{
	WCHAR szUserName[TMAX_USERNAME];
	WCHAR szDate[11];
	WCHAR szTime[5];

	SYSTEMTIME st;
	GetLocalTime(&st);

	if (!GetUserName(szUserName))
		return false;

	if (0 == ::GetDateFormat(::GetThreadLocale(), 0, &st, L"yyyy-MM-dd", szDate, 11))
		return false;

	if (0 == ::GetTimeFormat(::GetThreadLocale(), 0, &st, L"HHmm", szTime, 5))
		return false;

	int i;
	for (i = 0; i < 999; i++)
	{
		wsprintf(pszFilename, L"%s%s_%s_%s_%s_%03lu_%s.log",
			pszFolder,
			pszPrefix,
			szUserName,
			szDate,
			szTime,
			i,
			pszPostfix);
		if (!DoesFileExist(pszFilename))
			break;
	}

	if (DoesFileExist(pszFilename))
		return false;
	return true;
}

bool GetModuleName(HINSTANCE hInst, LPWSTR pszName)
{
	if (NULL == pszName)
	{
		return false;
	}

	if (0 == ::GetModuleFileName(hInst, pszName, TMAX_PATH))
		return false;

	_tsplitpath_s( pszName, NULL, 0, NULL, 0, pszName, TMAX_PATH, NULL, 0 );

	return true;
}

bool PathIsLocal(LPCWSTR pszPath)
{
	WCHAR szBuf[TMAX_PATH];
	lstrcpy(szBuf, pszPath);

	if (NULL == szBuf)
	{
		return false;
	}

	// removes .. and . and stuff like that
	PathCanonicalize(szBuf, szBuf);
	PathStripToRoot(szBuf);

	if (!PathIsRoot(szBuf))
	{
		// not an absolue
		return false;
	}

	// if it's on a UNC share
	if (0 == lstrcmpn(szBuf, _T("\\\\"), 2))
	{
		return false;
	}

	// otherwise see if Win32 can determine the type
	if (DRIVE_REMOTE == GetDriveType( szBuf ))
	{
		return false;
	}

	DWORD dwSize = 0;
	WNetGetConnection(NULL, szBuf, &dwSize);
	LPWSTR pszRemote = new WCHAR[(dwSize + 1)];
	if (NO_ERROR == WNetGetConnection(pszRemote, szBuf, &dwSize))
		return false;

	return true;
}

bool PathIsWritable(LPCWSTR pszPath)
{
	if (!DoesFolderExist(pszPath))
		return false;

	WCHAR szBuf[TMAX_PATH];
	lstrcpy(szBuf, pszPath);
	PathAppend(szBuf, L"MattGr.tmp");
	if (DoesFileExist(szBuf))
	{
		if (!DeleteFile(szBuf))
			return false;
	}

	HANDLE hFile = CreateFile(szBuf, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
		return false;
	}

	CloseHandle(hFile);
	DeleteFile(szBuf);
	return true;
}

#if 0
bool NukeFolder(LPCWSTR pszFolder)
{
	if (NULL == pszFolder || lstrlen(pszFolder) < 1)
		return false;
	if (!DoesFolderExist(pszFolder))
		return false;

	WIN32_FIND_DATA fd;
	ZeroMemory(&fd, sizeof(fd));

	WCHAR szBuf[TMAX_FNAME];
	lstrcpy(szBuf, pszFolder);
	PathAppend(szBuf, _T("*.*"));

	HANDLE hFind = FindFirstFileEx( pszFolder, 
		FindExInfoStandard, 
		&fd, 
		FindExSearchLimitToDirectories, 
		NULL, 
		0 );

	if (INVALID_HANDLE_VALUE == hFind)
	{
		FindClose(hFind);
		return false;
	}

	while (0 != ::FindNextFile(hFind, &fd))
	{
		// you found a folder
		// add it to the list
		// remove all the files in it
		// then remove it
	}
	FindClose(hFind);
	return true;
}
#endif
