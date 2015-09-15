// String functions, not using CRT if possible

#ifdef _DEBUG
	#ifdef DEBUG_NEW
		#define new DEBUG_NEW
	#endif
#endif

#include "str.h"

void lstrinit(LPTSTR psz)
{
	*psz = _T('\0');
}

// detecting a null string
bool isnull(LPCTSTR psz)
{
	return (NULL == psz);
}

bool isempty(LPCTSTR psz)
{
	return ( (NULL == psz) || *psz == _T('\0') );
}

void lstrtrunc(LPTSTR psz, int cch)
{
	int i = 0;
	LPTSTR pch = psz;
	for (i=0; i < cch; i++)
	{
		pch = CharNext(pch);
	}
	*pch = _T('\0');
}

// lstrcb
// Returns: int, # of bytes from pszSource to terminating NUL
// Param:   LPCTSTR pszSource
// Note:    in _UNICODE builds NUL is 2 bytes.
int wstrcb(LPCWSTR psz)
{
	return lstrlenW(psz) * wccb;
}

int mbstrcb(LPCSTR psz)
{
	return lstrlenA(psz);
}


// lstrcbn
// Returns: int, # of bytes in first n TCHARs or cch, whichever is shorter
// Param:   LPCTSTR pszSource
int wstrcbn(LPCWSTR psz, int cch)
{
	return min(lstrlenW(psz), (int)(cch * wccb));
}

int mbstrcbn(LPCSTR psz, int cch)
{
	int i = 0;
	LPSTR pch = (LPSTR)psz;
	while ( NULL != psz && (i < cch) )
	{
		i++;
		if (IsDBCSLeadByte( *(LPBYTE)pch) )
			i++;
		pch = CharNextA(pch);
	}
	return i;
}


// lstrcmp
// TCHAR string compare
// Returns: int, same as CRT
// Param:   LPCSTR pszStr1
// Param:   LPCSTR pszStr2
int wstrcmp (LPCWSTR psz1, LPCWSTR psz2)
{
	int i = ::CompareStringW(LOCALE_USER_DEFAULT, NORM_IGNOREWIDTH, psz1, wstrcch(psz1), psz2, wstrcch(psz2));
	if (i != 0)
		i -= 2;
	return i;
}

int mbstrcmp (LPCSTR psz1, LPCSTR psz2)
{
	int i = ::CompareStringA(LOCALE_USER_DEFAULT, NORM_IGNOREWIDTH, psz1, mbstrcb(psz1), psz2, mbstrcb(psz2));
	if (i != 0)
		i -= 2;
	return i;
}

#if 0
// lstrcmpn
// TCHAR string compare first n characters
// Returns: int, same as CRT
// Param:   LPCSTR pszStr1
// Param:   LPCSTR pszStr2
int wstrcmpn (LPCWSTR psz1, LPCWSTR psz2, int cch)
{
	cch = min(wstrcch(psz1), cch);
	cch = min(wstrcch(psz2), cch);
	int i = ::CompareStringW(LOCALE_USER_DEFAULT, NORM_IGNOREWIDTH, psz1, cch, psz2, cch);
	if (i != 0)
		i -= 2;
	return i;
}

int mbstrcmpn (LPCSTR psz1, LPCSTR psz2, int cch)
{
	cch = min(mbstrcch(psz1), cch);
	cch = min(mbstrcch(psz2), cch);
	int i = ::CompareStringA(LOCALE_USER_DEFAULT, NORM_IGNOREWIDTH, psz1, cch, psz2, cch);
	if (i != 0)
		i -= 2;
	return i;
}
#endif

// lstricmp
// TCHAR string compare, case insensitive
// Returns: int, same as CRT
// Param:   LPCSTR pszStr1
// Param:   LPCSTR pszStr2
int wstricmp (LPCWSTR psz1, LPCWSTR psz2)
{
	int i = ::CompareStringW(LOCALE_USER_DEFAULT, NORM_IGNOREWIDTH | NORM_IGNORECASE, psz1, wstrcch(psz1), psz2, wstrcch(psz2));
	if (i != 0)
		i -= 2;
	return i;
}

int mbstricmp (LPCSTR psz1, LPCSTR psz2)
{
	int i = ::CompareStringA(LOCALE_USER_DEFAULT, NORM_IGNOREWIDTH | NORM_IGNORECASE, psz1, mbstrcb(psz1), psz2, mbstrcb(psz2));
	if (i != 0)
		i -= 2;
	return i;
}


#if 0
// lstrcmpin
// TCHAR string compare, case insensitive
// Returns: int, same as CRT
// Param:   LPCSTR pszStr1
// Param:   LPCSTR pszStr2
int wstrcmpin (LPCWSTR psz1, LPCWSTR psz2, int cch)
{
	cch = min(wstrcch(psz1), cch);
	cch = min(wstrcch(psz2), cch);
	int i = ::CompareStringW(LOCALE_USER_DEFAULT, NORM_IGNOREWIDTH | NORM_IGNORECASE, psz1, cch, psz2, cch);
	if (i != 0)
		i -= 2;
	return i;
}

int mbstrcmpin (LPCSTR psz1, LPCSTR psz2, int cch)
{
	cch = min(mbstrcch(psz1), cch);
	cch = min(mbstrcch(psz2), cch);
	int i = ::CompareStringA(LOCALE_USER_DEFAULT, NORM_IGNOREWIDTH | NORM_IGNORECASE, psz1, cch, psz2, cch);
	if (i != 0)
		i -= 2;
	return i;
}
#endif

// lstrrchr
LPTSTR lstrrchr(LPCTSTR pszStart, LPCTSTR pszEnd, TCHAR cFind)
{
	return StrRChr(pszStart, pszEnd, cFind);
}

// Get number as a string
bool int2str(LPTSTR pszDest, int i)
{
	TCHAR szIn[32];
	TCHAR szOut[32];
	TCHAR szDec[2];

	GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, szDec, 2);
	wsprintf(szIn,_T("%lu"),i);
	if (0 == GetNumberFormat( LOCALE_USER_DEFAULT, 0, szIn, NULL, szOut, 32 ))
		return false;

	LPTSTR pszDec = lstrrchr(szOut, NULL, szDec[0]);
	lstrinit(pszDec);
	return (NULL != lstrcpy( pszDest, szOut ) );
}

bool float2str(LPTSTR pszDest, double d)
{
	TCHAR szIn[32];
	TCHAR szOut[32];

	lstrprintf_s(szIn, _T("%.4f"), d, 32);
	if (0 == GetNumberFormat( LOCALE_USER_DEFAULT, 0, szIn, NULL, szOut, 32 ))
		return false;

	return (NULL != lstrcpy( pszDest, szOut ) );
}

// load a string resource
bool wstrload(HINSTANCE hInst, UINT nID, LPWSTR pszBuf, int cbBuf)
{
	return (0 != ::LoadStringW(hInst, nID, pszBuf, cbBuf * 2));
}

bool mbstrload(HINSTANCE hInst, UINT nID, LPSTR pszBuf, int cbBuf)
{
	return (0 != ::LoadStringA(hInst, nID, pszBuf, cbBuf));
}

// format a string
bool lstrfmt(HINSTANCE hInst, UINT nFormatID, LPTSTR pszBuf, int cchBuf...)
{
	// format message into temporary buffer lpszTemp
	va_list argList;
	va_start(argList, nFormatID);
		
	if (!LoadString(hInst, nFormatID, pszBuf, cchBuf))
	{
		return false;
	}

	LPTSTR pszTemp = NULL;
	if (::FormatMessage(FORMAT_MESSAGE_FROM_STRING|FORMAT_MESSAGE_ALLOCATE_BUFFER,
		pszBuf, 0, 0, pszTemp, 0, &argList) == 0 || pszTemp == NULL)
	{
		LocalFree(pszTemp);
		return false;
	}

	// assign szTemp into the resulting string and free szTemp
	lstrcpy(pszBuf, pszTemp);
	LocalFree(pszTemp);
	va_end(argList);

	return true;
}

LPTSTR lstrfindchar(LPCTSTR pszBuf, TCHAR chFind)
{
	return StrChr(pszBuf, chFind);
}

LPTSTR lstrfindfirst(LPCTSTR pszBuf, LPCTSTR pszFind)
{
	return StrPBrk(pszBuf, pszFind);
}

bool lstrcontains(LPCTSTR pszBuf, LPCTSTR pszSet)
{
	return ( (NULL == StrPBrk(pszBuf, pszSet)) ? false : true);
}

int lstrcount(LPCTSTR pszBuf, const TCHAR chFind)
{
	int nch = 0;
	LPTSTR pch = (LPTSTR) pszBuf;
	
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

bool isquote(LPTSTR pszBuf)
{
	return (*pszBuf == '\"') ? true : false;
}

// Conversion macros
bool lmb2w(LPCSTR pszIn, LPWSTR pszwOut, int cchOut)
{
	return( 0 == (::MultiByteToWideChar(CP_THREAD_ACP, MB_PRECOMPOSED, pszIn, mbstrcb(pszIn), pszwOut, cchOut)) ? false : true);
}

bool lw2mb(LPCWSTR pszwIn, LPSTR pszOut, int cbOut)
{
	return (0 == (::WideCharToMultiByte(CP_THREAD_ACP, WC_SEPCHARS, pszwIn, wstrcch(pszwIn), pszOut, cbOut, NULL, NULL)) ? false : true);
}

bool pipe2null(LPTSTR psz)
{
	LPTSTR pch = psz;
	while ((pch = _tcschr(pch, '|')) != NULL)
	*pch++ = '\0';
	return true;
}

DWORD MyGetLongPathName(LPCWSTR lpwszShortPath, LPWSTR lpwszLongPath, DWORD cchBuf)
{
//TODO:	if this is supposed to get a long path name from a short path name, I'm not sure how it does it.
//		this code never actually references lpwszShortPath so I'm not sure what it's returning in lpwszLong Path

	LPSHELLFOLDER psfDesktop = NULL;
	ULONG chEaten = 0;
	LPITEMIDLIST pidlShellItem = NULL;

	HRESULT hr = SHGetDesktopFolder(&psfDesktop);

	OLECHAR olePath[TMAX_PATH];

	hr = psfDesktop->ParseDisplayName(NULL, NULL, olePath, &chEaten, &pidlShellItem, NULL);
	psfDesktop->Release();

	if (FAILED(hr))
	{
		// If we couldn't get an ID list for short pathname, it must not exist.      
		return 0;
	}
	else
	{
		// We did get an ID list, convert it to a long pathname
		if (!SHGetPathFromIDList(pidlShellItem, lpwszLongPath))
			return 0;

		// Free the ID list allocated by ParseDisplayName
		LPMALLOC pMalloc = NULL;
		if (E_FAIL == SHGetMalloc(&pMalloc))
			return 0;

		pMalloc->Free(pidlShellItem);
		pMalloc->Release();
	}

	cchBuf;
	lpwszShortPath;

	return lstrlen(lpwszLongPath);
}

int mbstrcch(LPCSTR pszSource)
{
	int i=0;
	LPSTR pch = (LPSTR)pszSource;
	while (*pch)
	{
		pch = CharNextA(pch);
		++i;
	}
	return i;
}

int wstrcch(LPCWSTR psz)
{
	return lstrlenW(psz);
}