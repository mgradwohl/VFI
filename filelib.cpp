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

//#ifndef WINVER
//	#define WINVER			0x0501
//#endif
//
//#ifndef _WIN32_WINNT
//	#define _WIN32_WINNT	0x0501
//#endif
//
//#ifndef _WIN32_IE
//	#define _WIN32_IE		0x0600
//#endif

#ifndef STRICT
	#define STRICT 1
#endif

//#ifndef WIN32
//	#define WIN32
//#endif
//
//#ifndef VC_EXTRALEAN
//	#define VC_EXTRALEAN
//#endif
//
//#ifndef WIN32_LEAN_AND_MEAN
//	#define WIN32_LEAN_AND_MEAN
//#endif

#ifdef _UNICODE
	#ifndef UNICODE
		#define UNICODE         // UNICODE is used by Windows headers
	#endif
#endif

#ifdef UNICODE
	#ifndef _UNICODE
		#define _UNICODE        // _UNICODE is used by C-runtime/MFC headers
	#endif
#endif

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

#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <winnetwk.h>

#include "strlib.h"
#include "filelib.h"
	
bool CreateFolder(LPCWSTR pszFolder)
{
	return (0 != CreateDirectory(pszFolder, NULL));
}

bool GetTempFolder(LPWSTR pszFolder)
{
	return (0 != GetTempPath(MAX_PATH, pszFolder));
}

bool GetWindowsFolder(LPWSTR pszFolder)
{
	return (TRUE == SHGetSpecialFolderPath(NULL, pszFolder, CSIDL_WINDOWS, FALSE));
}

bool GetDesktopFolder(LPWSTR pszFolder)
{
	return ( TRUE == SHGetSpecialFolderPath(NULL, pszFolder, CSIDL_DESKTOPDIRECTORY, FALSE));
}

bool GetSystemFolder(LPWSTR pszFolder)
{
	return ( TRUE == SHGetSpecialFolderPath(NULL, pszFolder, CSIDL_SYSTEM, FALSE));
}

bool GetRecycleFolder(LPWSTR pszFolder)
{
	return ( TRUE == SHGetSpecialFolderPath(NULL, pszFolder, CSIDL_BITBUCKET, FALSE));
}

bool GetDocumentsFolder(LPWSTR pszFolder)
{
	return ( TRUE == SHGetSpecialFolderPath(NULL, pszFolder, CSIDL_PERSONAL, FALSE));
}

bool GetFontFolder(LPWSTR pszFolder)
{
	return ( TRUE == SHGetSpecialFolderPath(NULL, pszFolder, CSIDL_FONTS, FALSE));
}

bool GetProfileFolder(LPWSTR pszFolder)
{
	return ( TRUE == SHGetSpecialFolderPath(NULL, pszFolder, CSIDL_PROFILE, FALSE));
}

bool GetProgramFilesFolder(LPWSTR pszFolder)
{
	return ( TRUE == SHGetSpecialFolderPath(NULL, pszFolder, CSIDL_PROGRAM_FILES, FALSE));
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

bool PathGetFolder(LPWSTR pszFileSpec)
{

	if (NULL == pszFileSpec)
	{
		return false;
	}

	PathRemoveFileSpec(pszFileSpec);
	return true;
}

bool PathGetFileName(LPWSTR pszFileSpec)
{

	if (NULL == pszFileSpec)
	{
		return false;
	}

	pszFileSpec = PathFindFileName(pszFileSpec);

	return true;
}

bool GetModuleFolder(HINSTANCE hInst, LPWSTR pszFolder)
{
	if (NULL == pszFolder)
	{
		return false;
	}

	::GetModuleFileName(hInst, pszFolder, MAX_PATH);

	PathGetFolder(pszFolder);
	return true;
}

bool GetLogFolder(LPCWSTR pszAppname, LPWSTR pszFolder)
{
	if (IsBadWritePtr(pszFolder, MAX_PATH))
	{
		TRACE(L"GetLogFolder pszFolder needs to hold MAX_PATH characters\r\n");
		return false;
	}

	HKEY hKey;
	DWORD dwType = REG_SZ;
	DWORD dwSize = MAX_PATH;

	// open the key
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		L"Software\\Microsoft\\Microsoft Games\\Logs", 0, KEY_READ, &hKey))
		goto USE_TEMP;

	WCHAR szModule[MAX_FNAME];
	if (NULL == pszAppname || lstrlen(pszAppname) < 1)
	{
		GetModuleName(GetModuleHandle(NULL), szModule);
	}
	else
	{
		lstrcpy(szModule, pszAppname);
	}
		
	// read the app specific folder
	dwSize = MAX_PATH;
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
	dwSize = MAX_PATH;
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

bool GetLogFileName(LPCWSTR pszFolder, LPCWSTR pszPrefix, LPCWSTR pszPostfix, LPWSTR pszFilename, LPWSTR pszExt)
{
	WCHAR szUserName[MAX_USERNAME];
	WCHAR szDate[11];
	WCHAR szTime[5];

	SYSTEMTIME st;
	GetLocalTime(&st);

	if (!MyGetUserName(szUserName))
		return false;

	if (0 == ::GetDateFormat(::GetThreadLocale(), 0, &st, L"yyyy-MM-dd", szDate, 11))
		return false;

	if (0 == ::GetTimeFormat(::GetThreadLocale(), 0, &st, L"HHmm", szTime, 5))
		return false;

	WCHAR szFilename[MAX_PATH];
	for (int i = 0; i < 999; i++)
	{
		szFilename[0] = L'\0';
		wsprintf(szFilename, L"%s%s_%s_%s_%s_%03lu_%s.%s",
			pszFolder,
			pszPrefix,
			szUserName,
			szDate,
			szTime,
			i,
			pszPostfix,
			pszExt);
		if (!DoesFileExist(szFilename))
			break;
	}

	// we tried 999 times and couldn't get a filename
	if (DoesFileExist(szFilename))
		return false;

	lstrcpy(pszFilename, szFilename);
	return true;
}

bool GetModuleName(HINSTANCE hInst, LPWSTR pszName)
{
	if (NULL == pszName)
	{
		return false;
	}

	if (0 == ::GetModuleFileName(hInst, pszName, MAX_PATH))
		return false;

	PathGetFileName(pszName);

	return true;
}

bool PathIsLocal(LPCWSTR pszPath)
{
	if (NULL == pszPath)
	{
		return false;
	}

	if (lstrcch(pszPath) < 1)
	{
		return false;
	}

	LPWSTR pszBuf = new WCHAR[lstrcb(pszPath) + lccb];
	if (NULL == pszBuf)
	{
		return false;
	}
	lstrcpy(pszBuf, pszPath);

	// removes .. and . and stuff like that
	PathCanonicalize(pszBuf, pszBuf);
	PathStripToRoot(pszBuf);

	if (!PathIsRoot(pszBuf))
	{
		// not an absolute
		delete [] pszBuf;
		return false;
	}

	// if it's on a UNC share
	if (0 == lstrcmpn(pszBuf, L"\\\\", 2))
	{
		// maybe it's this machine
		if (lstrcmpni(pszBuf, L"\\\\localhost\\", 12))
		{
			delete [] pszBuf;
			return true;
		}

		if (lstrcmpn(pszBuf, L"\\\\127.0.0.1\\", 12))
		{
			delete [] pszBuf;
			return true;
		}
		
		delete [] pszBuf;
		return false;
	}

	// otherwise see if Win32 can determine the type
	if (DRIVE_REMOTE == GetDriveType(pszBuf))
	{
		delete [] pszBuf;
		return false;
	}

	DWORD dwSize = 0;
	LPWSTR pszRemote = NULL;
	WNetGetConnection(pszBuf, pszRemote, &dwSize);

	pszRemote = new WCHAR[(dwSize + 1)];
	if (NULL == pszRemote)
	{
		delete [] pszBuf;
		return false;
	}
	if (WN_SUCCESS == WNetGetConnection(pszBuf, pszRemote, &dwSize))
	{
		delete [] pszBuf;
		delete [] pszRemote;
		return false;
	}

	delete [] pszBuf;
	delete [] pszRemote;
	return true;
}

bool PathIsWritable(LPCWSTR pszPath)
{
	if (!DoesFolderExist(pszPath))
		return false;

	WCHAR szBuf[MAX_PATH];
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

	WCHAR szBuf[MAX_FNAME];
	lstrcpy(szBuf, pszFolder);
	PathAppend(szBuf, __T("*.*"));

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

bool PathGetLongName(LPCWSTR pszShortPath, LPWSTR pszLongPath)
{
	LPSHELLFOLDER psfDesktop = NULL;
	ULONG chEaten = 0;
	LPITEMIDLIST pidlShellItem = NULL;

	HRESULT hr = SHGetDesktopFolder(&psfDesktop);

	OLECHAR olePath[MAX_PATH];
#ifndef _UNICODE
	MultiByteToWideChar( CP_ACP, 
						 MB_PRECOMPOSED, 
						 pszShortPath, 
						 -1, 
						 olePath,
						 sizeof(olePath));

#else
	lstrcpyW(olePath, pszShortPath);
#endif
	hr = psfDesktop->ParseDisplayName(NULL, NULL, olePath, &chEaten, &pidlShellItem, NULL);
	psfDesktop->Release();

	if (FAILED(hr))
	{
		// If we couldn't get an ID list for short pathname, it must not exist.      
		return false;
	}
	else
	{
		// We did get an ID list, convert it to a long pathname
		if (!SHGetPathFromIDList(pidlShellItem, pszLongPath))
			return false;

		// Free the ID list allocated by ParseDisplayName
		LPMALLOC pMalloc = NULL;
		if (E_FAIL == SHGetMalloc(&pMalloc))
			return false;

		pMalloc->Free(pidlShellItem);
		pMalloc->Release();
	}

	return true;
}

bool PathIsRootOnly(LPCWSTR pszPath)
{
	if (!PathIsRoot(pszPath))
	{
		return false;
	}

	LPWSTR pch = (LPWSTR) pszPath;

	// starts with a drive letter
	if (!IsCharAlpha(*pch))
		return false;

	// more than 3 characters: "D:\"
	if (lstrcch(pch) > 3)
		return false;

	if (*pch == L'\\')
	{
		// it's a UNC and I don't handle those right now
		return false;
	}

	pch = CharNext(pch);
	if (*pch == L':')
	{
		pch = CharNext(pch);
		if (*pch == L'\\')
		{
			return true;
		}
	}

	return false;
}

LPWSTR PathGetRootName(LPWSTR pszPath)
{
	if (!pszPath) return NULL;

	LPWSTR pch = (LPWSTR) pszPath;
	LPWSTR pch2 = (LPWSTR) pszPath;

	if (*pch == L'\\')
	{
		pch = CharNext(pch);
		if(*pch != L'\\')
		{
			// not a UNC
			return NULL;
		}

		// walk to the next '\\'
		pch = CharNext(pch);
		pch2 = pch;
		while ( (*pch2 != L'\0') && (*pch2 != L'\\') )
		{
			pch2 = CharNext(pch2);
		}

		*pch2 = L'\0';

		pszPath = pch;
		return pch;
	}

	// should be a drive letter
	pch2 = CharNext(pch);
	if(*pch2 != L':')
	{
		// not a drive letter
		return NULL;
	}
	
	*pch2 = L'\0';

	pszPath = pch;
	return pch;
}

bool GetVolumeLabel(LPCWSTR pszRootPathName, LPWSTR pszVolumeName, DWORD cchVolumeName)
{
	return (0 != GetVolumeInformation(pszRootPathName, pszVolumeName, cchVolumeName, NULL, NULL, NULL, NULL, 0));
}

bool PathGetContainingFolder(LPWSTR pszFileSpec)
{
	if (NULL == pszFileSpec)
	{
		return false;
	}

	// c:\windows\suckit\bang.tmp should return "suckit" so, it's just
	// what's between the last '\\' and the one before it

	PathRemoveFileSpec(pszFileSpec);
	LPWSTR psz = PathFindFileName(pszFileSpec);
	lstrcpy(pszFileSpec, psz);
	return true;
}

__int64 SetFilePointer64 (HANDLE hf, __int64 distance, DWORD MoveMethod)
{
   LARGE_INTEGER li;
   li.QuadPart = distance;
   li.LowPart = SetFilePointer (hf, li.LowPart, &li.HighPart, MoveMethod);

   if (li.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR)
   {
      li.QuadPart = -1;
   }

   return li.QuadPart;
}

__int64 GetFileSize64(LPCWSTR pszFileSpec)
{
	if (NULL == pszFileSpec)
		return 0;

	HANDLE hFile = CreateFile(pszFileSpec,
					GENERIC_READ,
					FILE_SHARE_READ,
					NULL,
					OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL, NULL);

	if ( INVALID_HANDLE_VALUE == hFile)
	{
		CloseHandle(hFile);
		return 0;
	}

	LARGE_INTEGER qwSize;
	if (!GetFileSizeEx(hFile, &qwSize))
	{
		CloseHandle(hFile);
		return 0;
	}
	CloseHandle(hFile);
	return qwSize.QuadPart;
}

void ShowShellFileProperties(HWND hwnd, LPCWSTR pwszFile)
{
	SHELLEXECUTEINFO ShExecInfo;
	ZeroMemory(&ShExecInfo, sizeof(SHELLEXECUTEINFO));

	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_INVOKEIDLIST ;
	ShExecInfo.hwnd = hwnd;
	ShExecInfo.lpVerb = L"properties";
	ShExecInfo.lpFile = pwszFile;
	ShExecInfo.lpParameters = L""; 
	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_SHOW;
	ShExecInfo.hInstApp = NULL; 
	ShellExecuteEx(&ShExecInfo);
}
