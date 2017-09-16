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


//#ifndef TRACE
//	#define TRACE(x) OutputDebugString(x)
//#endif

#include <shlobj.h>
#include <shlwapi.h>
#include <shellapi.h>

#include "trace.h"
#include "strlib.h"
#include "filelib.h"
	
bool CreateFolder(LPCWSTR pszFolder)
{
	return (0 != CreateDirectory(pszFolder, NULL));
}

bool GetTempFolder(LPWSTR pszFolder)
{
	return (0 != GetTempPath(_MAX_PATH, pszFolder));
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

	DWORD const dw = ::GetFileAttributes( pszFileName );
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

	DWORD const dw = ::GetFileAttributes( pszFolder );
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

	::GetModuleFileName(hInst, pszFolder, _MAX_PATH);

	PathGetFolder(pszFolder);
	return true;
}

bool GetLogFolder(LPCWSTR pszAppname, LPWSTR pszFolder)
{
	if (IsBadWritePtr(pszFolder, _MAX_PATH) || pszFolder == nullptr)
	{
		TRACE(L"GetLogFolder pszFolder needs to hold _MAX_PATH characters\r\n");
		return false;
	}

	HKEY hKey = NULL;
	DWORD dwType = REG_SZ;
	DWORD dwSize = _MAX_PATH;

	// open the key
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		L"Software\\Microsoft\\Microsoft Games\\Logs", 0, KEY_READ, &hKey))
		goto USE_TEMP;

	WCHAR szModule[MAX_FNAME];
	lstrinit(szModule);

	if (NULL == pszAppname || lstrlen(pszAppname) < 1)
	{
		GetModuleName(GetModuleHandle(NULL), szModule);
	}
	else
	{
		lstrcpy(szModule, pszAppname);
	}
		
	// read the app specific folder
	dwSize = _MAX_PATH;
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
	dwSize = _MAX_PATH;
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
	SYSTEMTIME st;
	GetLocalTime(&st);

	WCHAR szUserName[MAX_USERNAME];
	if (!MyGetUserName(szUserName))
		return false;

	WCHAR szDate[11];
	if (0 == ::GetDateFormat(::GetThreadLocale(), 0, &st, L"yyyy-MM-dd", szDate, 11))
		return false;

	WCHAR szTime[5];
	if (0 == ::GetTimeFormat(::GetThreadLocale(), 0, &st, L"HHmm", szTime, 5))
		return false;

	WCHAR szFilename[_MAX_PATH];
	for (unsigned int i = 0; i < 999; i++)
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

	if (0 == ::GetModuleFileName(hInst, pszName, _MAX_PATH))
		return false;

	PathGetFileName(pszName);

	return true;
}

bool PathIsLocal(LPCWSTR pszPath)
{
	if (pszPath == nullptr)
	{
		return false;
	}

	if (PathIsNetworkPath(pszPath))
		return false;

	LPWSTR pszBuf = new WCHAR[_MAX_PATH];
	if (pszBuf == nullptr)
	{
		return false;
	}
	wcscpy_s(pszBuf, _MAX_PATH, pszPath);

	// removes .. and . and stuff like that
	PathStripToRoot(pszBuf);
	PathRemoveBackslash(pszBuf);

	if (!PathIsRoot(pszBuf))
	{
		// not an absolute
		delete [] pszBuf;
		return false;
	}

	// otherwise see if Win32 can determine the type
	if (DRIVE_REMOTE == GetDriveType(pszBuf))
	{
		delete [] pszBuf;
		return false;
	}

	delete [] pszBuf;
	return true;
}

bool PathIsWritable(LPCWSTR pszPath)
{
	if (!DoesFolderExist(pszPath))
		return false;

	WCHAR szBuf[_MAX_PATH];
	lstrcpy(szBuf, pszPath);
	PathAppend(szBuf, L"RandomNameThatWillNotExistInAllHistoryOfTime.tmp");
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

bool PathGetLongName(LPCWSTR pszShortPath, LPWSTR pszLongPath)
{
	LPSHELLFOLDER psfDesktop = NULL;
	if (S_OK != SHGetDesktopFolder(&psfDesktop))
	{
		return false;
	}

	LPITEMIDLIST pidlShellItem = NULL;
	OLECHAR olePath[_MAX_PATH];
	wcscpy_s(olePath, _MAX_PATH, pszShortPath);

	ULONG chEaten = 0;
	if (S_OK != psfDesktop->ParseDisplayName(NULL, NULL, olePath, &chEaten, &pidlShellItem, NULL))
	{
		psfDesktop->Release();
		return false;
	}
	psfDesktop->Release();

	// We did get an ID list, convert it to a long pathname
	if (!SHGetPathFromIDList(pidlShellItem, pszLongPath))
		return false;

	// Free the ID list allocated by ParseDisplayName
	LPMALLOC pMalloc = NULL;
	if (E_FAIL == SHGetMalloc(&pMalloc))
		return false;

	pMalloc->Free(pidlShellItem);
	pMalloc->Release();

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

	// c:\windows\foo\bar.tmp should return "foo" so, it's just
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
	if (pszFileSpec == nullptr)
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
