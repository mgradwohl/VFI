// strlib.cpp
// mattgr
// 3/23/2000

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

#include <tchar.h>	//only for _stprintf
#include "strlib.h"

int mbstrcch(LPCSTR pszSource)
{
    LPSTR pch = (LPSTR)pszSource;
	int i = 0;

	while (*pch)
    {
		i++;
		pch = CharNextA(pch);
    }
    return i;
}

int wstrcch(LPCWSTR pszSource)
{
	return lstrlenW(pszSource);
}

// lstrcb
// Returns: int, # of bytes from pszSource to terminating NUL
// Param:   LPCWSTR pszSource
// Note:    in _UNICODE builds NUL is 2 bytes.
int wstrcb(LPCWSTR psz)
{
	return lstrlenW(psz) * wccb;
}

int mbstrcb(LPCSTR psz)
{
	return lstrlenA(psz);
}

// Get number as a string
bool int2str(LPWSTR pszDest, QWORD i)
{
	if (NULL == pszDest)
	{
		return false;
	}

	WCHAR szIn[67];
	WCHAR szOut[67];
	WCHAR szDec[2];

	//swprintf((LPWSTR)szIn, L"%I64d", i);
	_ui64tow_s(i, szIn, 67, 10);

	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, (LPWSTR)szDec, 2);
	if (0 == GetNumberFormat( LOCALE_USER_DEFAULT, 0, (LPWSTR)szIn, NULL, (LPWSTR)szOut, 65 ))
		return false;

	LPWSTR pszDec = lstrrchr((LPWSTR)szOut, NULL, szDec[0]);
	*pszDec = '\0';
	return (NULL != lstrcpy( pszDest, (LPWSTR)szOut ) );
}

bool float2str(LPWSTR pszDest, double d)
{
	if (NULL == pszDest)
	{
		return false;
	}
	
	WCHAR szIn[32];
	WCHAR szOut[32];

	swprintf_s(szIn, _T("%.4f"), d);
	if (0 == GetNumberFormat( LOCALE_USER_DEFAULT, 0, (LPWSTR)szIn, NULL, (LPWSTR)szOut, 32 ))
		return false;

	return (NULL != lstrcpy( pszDest, (LPWSTR)szOut ) );
}

bool lstrcontains(LPCWSTR pszBuf, LPCWSTR pszSet)
{
	return ( (NULL == StrPBrk(pszBuf, pszSet)) ? false : true);
}

int lstrcount(LPCWSTR pszBuf, WCHAR chFind)
{
	int nch = 0;
	LPWSTR pch = (LPWSTR) pszBuf;
	
	while (*pch)
	{
		if (*pch == chFind)
		{
			nch++;
		}
		pch = CharNext(pch);
	}
	return nch;
}

bool isquote(LPWSTR pszBuf)
{
	if (NULL == pszBuf)
	{
		return false;
	}

	return (*pszBuf == '\"') ? true : false;
}

// Conversion macros
bool lmb2w(LPCSTR pszIn, LPWSTR pszOut, int cchOut)
{
	if (NULL == pszIn)
	{
		return false;
	}
	if (NULL == pszOut)
	{
		return false;
	}

	return( 0 == (::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pszIn, -1, pszOut, cchOut)) ? false : true);
}

bool lw2mb(LPCWSTR pszIn, LPSTR pszOut, int cbOut)
{
	if (NULL == pszIn)
	{
		return false;
	}
	if (NULL == pszOut)
	{
		return false;
	}

	return (0 == (::WideCharToMultiByte(CP_ACP, 0, pszIn, -1, pszOut, cbOut, NULL, NULL)) ? false : true);
}

bool pipe2null(LPWSTR pszSource)
{
	if (NULL == pszSource)
	{
		return false;
	}

	LPWSTR pch = pszSource;
	while ((pch = StrChr(pch, '|')) != NULL)
	*pch++ = '\0';
	return true;
}

bool MyGetUserName(LPWSTR pszUserName)
{
	if (NULL == pszUserName)
	{
		return false;
	}

	DWORD dwLen = TMAX_USERNAME;
	return (FALSE != GetUserName(pszUserName, &dwLen));
}

bool strfmt(HINSTANCE hInstance, LPWSTR pszBuffer, UINT nFormatID, ...)
{
	if (NULL == pszBuffer)
	{
		return false;
	}

	WCHAR szString[512];
	if (!LoadString(hInstance, nFormatID, (LPWSTR)szString, 512))
	{
		return false;
	}

	// format message into temporary buffer lpszTemp
	va_list argList;
	va_start(argList, nFormatID);
	LPWSTR pszTemp;
	if (FormatMessage(FORMAT_MESSAGE_FROM_STRING | FORMAT_MESSAGE_ALLOCATE_BUFFER, szString, 0, 0, (LPWSTR)&pszTemp, 0, &argList) == 0 || pszTemp == NULL)
	{
		va_end(argList);
		return false;
	}

	// assign lpszTemp into the resulting string and free lpszTemp
	lstrcpy(pszBuffer, pszTemp);
	LocalFree(pszTemp);
	va_end(argList);

	return true;
}