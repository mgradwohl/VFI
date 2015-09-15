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

// ISOCheck.cpp : Defines the entry point for the DLL application.
//
#ifndef WINVER
	#define WINVER			0x0501
#endif

#ifndef _WIN32_WINNT
	#define _WIN32_WINNT	0x0501
#endif

#ifndef _WIN32_IE
	#define _WIN32_IE		0x0600
#endif

#ifndef STRICT
	#define STRICT 1
#endif

#ifndef WIN32
	#define WIN32
#endif

#ifndef VC_EXTRALEAN
	#define VC_EXTRALEAN
#endif

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif

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
#include <assert.h>

#include "strlib.h"
	//#pragma comment(lib, "strlibu.lib")

#include "ISOCheck.h"
#include "isocheckresource.h"

WORD isoGetISOFlags(LPCWSTR wzPath, LPCWSTR wzName, LPCWSTR wzExt);
WORD isoGetISOFlags(LPCWSTR wzFullPath);
LPWSTR isoGetISOFlagsString(const WORD wISOFlags);

static WCHAR g_wzBuf[8192];
WCHAR* g_pwz[ISO_MAX];
HANDLE g_hModule;
static int g_cCache = 0;
static volatile long g_cProcess = 0;

bool CheckISOPath(LPCWSTR pszPath, WORD* pwISOFlags);
bool CheckISOName(LPCWSTR pszPath, WORD* pwISOFlags);
bool CheckISOExtension(LPCWSTR pszPath, WORD* pwISOFlags);

void InitStringArray();
void FreeStringArray();

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  dwReason, LPVOID lpReserved)
{
	lpReserved;
	g_hModule = hModule;

	switch (dwReason)
	{
		case DLL_PROCESS_ATTACH:
			{
				#ifdef DEBUG
					OutputDebugString(L"isoCheck:attach\r\n");
				#endif
				InterlockedIncrement(&g_cProcess);
				if (g_cProcess == 1)
				{
					InitStringArray();
				}
				break;
			}
		case DLL_THREAD_ATTACH:
			{
				break;
			}

		case DLL_THREAD_DETACH:
			{
				break;
			}
		case DLL_PROCESS_DETACH:
			{
				#ifdef DEBUG
					OutputDebugString(L"isoCheck:detach\r\n");
				#endif
				InterlockedDecrement(&g_cProcess);
				if (g_cProcess < 1)
				{
					FreeStringArray();
				}
				break;
			}
		break;
	}
    return TRUE;
}

WORD isoGetISOFlags(LPCWSTR wzFullPath)
{
	// don't call this
	// wzFullPath c:\folder\name.txt
	// split it up

	WORD wFlags = 0;
	if (!CheckISOPath(wzFullPath, &wFlags))
		return 0;

	if (!CheckISOName(wzFullPath, &wFlags))
		return 0;

	if (!CheckISOExtension(wzFullPath, &wFlags))
		return 0;

	return wFlags;
}

WORD isoGetISOFlags(LPCWSTR wzPath, LPCWSTR wzName, LPCWSTR wzExt)
{
	WORD wFlags = 0;
	if (NULL != wzPath)
	{
        if (!CheckISOPath(wzPath, &wFlags))
		return 0;
	}

	if (!CheckISOName(wzName, &wFlags))
		return 0;

	if (!CheckISOExtension(wzExt, &wFlags))
		return 0;

	return wFlags;
}

bool CheckISOPath(LPCWSTR pszPath, WORD* pwISOFlags)
{
	if (NULL == pszPath)
	{
		return false;
	}

	WCHAR szPath[MAX_PATH];
	lstrcpy(szPath, pszPath);

	// The full path can't be too long
	if (lstrcch(szPath) > 31)
	{
		*pwISOFlags |= ISO_PATH_TOOLONG;
	}
	
	if (lstrcontains(szPath, L"abcdefghijklmnopqrstuvwxyz"))
	{
		*pwISOFlags |= ISO_PATH_LOWERCASE;
	}

	// Unique path checking problem
	// If the 1st character is a letter, then the
	// 2nd character must be a : and the third must be a \
	// and there must be no more : characters
	// If the 1t character isn't a letter, then it must be
	// a \ and the 2nd character must be a \ and there can be
	// no more : characters.
	bool fLocal = true;
	LPWSTR pch = (LPWSTR)szPath;
	CharUpper(szPath);
	if (!isalpha(*pch))
	{
		if (!lstrcmpn(pch, L"\\\\", 2))
		{
			// should never happen
			// it's either "X:\" or "\\"
			return false;
		}
		fLocal = false;
		pch = CharNext(pch);
		pch = CharNext(pch);
	}
	else
	{
		// skip Drive ID
		pch = CharNext(pch);
		if (!lstrcmpn(pch, L"\\:", 2))
		{
			// should never happen
			// it's either "X:\" or "\\"
			return false;
		}
		pch = CharNext(pch);
		pch = CharNext(pch);
	}
	// Now we now if the path is 'Local' (X:\file) or UNC (\\server\share)
	// and pch points to the first thing after the path type stuff

	// check for too many ':'
	if (fLocal)
	{
		// one ':' allowed
		if (lstrcount(pszPath, ':') > 1)
		{
			*pwISOFlags |= ISO_PATH_BADCHAR;
		}
	}
	else
	{
		// no ':' allowed
		if (lstrcount(pszPath, ':') > 0)
		{
			*pwISOFlags |= ISO_PATH_BADCHAR;
		}
	}
	
	while (*pch)
	{
		if (!( (( *pch >= 'A' ) && ( *pch <= 'Z' )) ||
			   (( *pch >= '0' ) && ( *pch <= '9' )) ||
			   (( *pch == '_' ) || ( *pch == '.' )  || ( *pch == ':' ) || ( *pch == '\\' ) )))
		{
			*pwISOFlags |=  ISO_PATH_BADCHAR;
			break;
		}

		pch = CharNext(pch);
	}

	// Checking the path for bad characters
	if (lstrcontains(szPath, L"/\""))
	{
		*pwISOFlags |= ISO_PATH_BADCHAR;
	}

	// checking for too many dots
	if (lstrcount(pszPath, '.') > 0)
	{
		*pwISOFlags |= ISO_PATH_HASDOT;
	}

	// checking for depth
	if (lstrcount(pszPath, '\\') > 8)
	{
		*pwISOFlags |= ISO_PATH_TOODEEP;
	}

	return true;
}

bool CheckISOName(LPCWSTR pszPath, WORD* pwISOFlags)
{
	if (NULL == pszPath)
	{
		return false;
	}

	WCHAR szName[MAX_PATH];
	lstrcpy(szName, pszPath);

	if (lstrcch(szName) > 8)
	{
		*pwISOFlags |= ISO_FILE_TOOLONG;
	}

	if (lstrcontains(szName, L"\\/:\"*<>|"))
	{
		*pwISOFlags |= ISO_FILE_BADCHAR;
	}
	if (lstrcontains(szName, L"abcdefghijklmnopqrstuvwxyz"))
	{
		*pwISOFlags |= ISO_FILE_LOWERCASE;
	}

	CharUpper(szName);
	LPWSTR pch = (LPWSTR)szName;
	while (*pch)
	{
		if (!( (( *pch >= 'A' ) && ( *pch <= 'Z' )) ||
			   (( *pch >= '0' ) && ( *pch <= '9' )) ||
			   (( *pch == '_' ) || ( *pch == '.' )  || ( *pch == ':' ) )))
		{
			*pwISOFlags |= ISO_FILE_BADCHAR;
			break;
		}

		pch = CharNext(pch);
	}

	if (lstrcount(pszPath, '.') > 0)
	{
		*pwISOFlags |= ISO_FILE_HASDOT;
	}

	return true;
}

bool CheckISOExtension(LPCWSTR pszPath, WORD* pwISOFlags)
{
	if (NULL == pszPath)
	{
		return false;
	}

	WCHAR szExt[MAX_PATH];
	lstrcpy(szExt, pszPath);

	if (lstrcch(szExt) > 3)
	{
		*pwISOFlags |= ISO_EXT_TOOLONG;
	}

	if (lstrcontains(szExt, L"\\/:\"*<>|"))
	{
		*pwISOFlags |= ISO_EXT_BADCHAR;
	}

	if (lstrcontains(szExt, L"abcdefghijklmnopqrstuvwxyz"))
	{
		*pwISOFlags |= ISO_EXT_LOWERCASE;
	}

	CharUpper(szExt);
	LPWSTR pch = (LPWSTR)szExt;
	while (*pch)
	{
		if (!( (( *pch >= 'A' ) && ( *pch <= 'Z' )) ||
			   (( *pch >= '0' ) && ( *pch <= '9' )) ||
			   (( *pch == '_' ) || ( *pch == ':' ) )))
		{
			*pwISOFlags |= ISO_EXT_BADCHAR;
			break;
		}

		pch = CharNext(pch);
	}

	if (lstrcount(pszPath, '.') > 1)
	{
		*pwISOFlags |= ISO_EXT_HASDOT;
	}

	return true;
}

LPWSTR isoGetISOFlagsString(const WORD wISOFlags)
{
	if (0 == wISOFlags)
		return (LPWSTR) L"\0";

	if (wISOFlags > ISO_MAX)
		return (LPWSTR) L"\0";

	if (NULL == g_pwz[wISOFlags])
	{
		// Load it
		int cch = LoadStringW((HINSTANCE)g_hModule, wISOFlags + STR_ISOFLAG_0000, g_wzBuf, 8192);
		if (0 == cch)
			return (LPWSTR) L"\0";

		//store it
		g_pwz[wISOFlags] = (LPWSTR) new WCHAR [(4 + cch) * 2];
		if (NULL == g_pwz[wISOFlags])
		{
			WCHAR wzMsg[512];
			wsprintf(wzMsg, L"Failed to fill ISO bucket %04u", wISOFlags);
			MessageBox(NULL, wzMsg, L"ISO Check Error", MB_OK | MB_ICONINFORMATION);
			return NULL;
		}

		g_cCache++;
		lstrcpyW( g_pwz[wISOFlags], g_wzBuf);
	}

	return (LPWSTR) g_pwz[wISOFlags];
}

// OLD
// non cached version
LPWSTR _isoGetISOFlagsString(WORD wISOFlags)
{
	if (0 == wISOFlags)
		return NULL;

	if (wISOFlags > ISO_MAX)
		return NULL;

	static WCHAR wzBuf[8192];
	int cch = LoadStringW((HINSTANCE)g_hModule, wISOFlags, wzBuf, 8192);

	if (0 == cch)
		return NULL;

	return wzBuf;
}

void InitStringArray()
{
	#ifdef DEBUG
	WCHAR wzMsg[512];
	wsprintf(wzMsg, L"isoCheck:InitStringArray() g_cCache==%u\r\n", g_cCache);
	OutputDebugString(wzMsg);
	#endif

	for (int i = 0; i < ISO_MAX; i++)
	{
		g_pwz[i] = NULL;
	}
}

void FreeStringArray()
{
	#ifdef DEBUG
	WCHAR wzMsg[512];
	wsprintf(wzMsg, L"isoCheck:FreeStringArray() g_cCache==%u\r\n", g_cCache);
	OutputDebugString(wzMsg);
	#endif

	int i;
	for (i = 0; i < ISO_MAX; i++)
	{
		//ROCKALL
		//g_Heap.DeleteAll(false);

		//WINDOWS
		delete [] g_pwz[i];
	}
}
