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

// String functions, not using CRT if possible

#pragma once
#ifndef STR_H

#include <windows.h>

#include <shlwapi.h>
	#pragma comment(lib, "shlwapi.lib")

#include <shlobj.h>
	#pragma comment(lib, "shell32.lib")

#include <stdio.h>		// for _stprintf
#include <tchar.h>

#ifdef  __cplusplus
extern "C" {
#endif

// make sure that the CRT and Win32 are in-synch re: UNICODE and MBCS
#ifdef UNICODE
	#ifndef _UNICODE
		#define _UNICODE
	#endif
#endif

#ifdef _UNICODE
	#ifndef UNICODE
		#define UNICODE
	#endif
#endif

#ifndef UNICODE
	#ifndef MBCS
		#define MBCS
	#endif
	#ifndef _MBCS
		#define _MBCS
	#endif
#endif

// define TCHAR the same way windows.h does
#if defined(UNICODE)
	#define __T(x)	L##x
	#define TCHAR	WCHAR
#else
	#define __T(x)	x
	#define TCHAR	CHAR
#endif

// TCHAR helper macros
#define _T(x)       __T(x)
#define _TEXT(x)    __T(x)

// types
typedef unsigned char* LPXSTR;
typedef const unsigned char* LPCXSTR;

// Double these because in the worst case in an MBCS system, they are wide
#define TMAX_PATH	(_MAX_PATH*2)
#define TMAX_DRIVE	(_MAX_DRIVE*2)
#define TMAX_DIR	(_MAX_DIR*2)
#define TMAX_FNAME	(_MAX_FNAME*2)
#define TMAX_EXT	(_MAX_EXT*2)

// lccb
// LCHAR count bytes
#define sccb	 sizeof(CHAR)
#define	mccb	 sizeof(WCHAR)
#define wccb	 sizeof(WCHAR)

#if (defined(UNICODE) || defined(MBCS))
	#define lccb wccb
#else
	#define lccb sccb
#endif

// pointer macros
#define zero(x)			(::ZeroMemory((LPVOID)&x, sizeof(x)))

// wsprintf that does floats
#define lstrprintf _stprintf
#define lstrprintf_s _stprintf_s

// initialization
void lstrinit(LPTSTR psz);

// detecting a null string
bool isnull(LPCTSTR psz);
bool isempty(LPCTSTR psz);
void lstrtrunc(LPTSTR psz, int cch);

// lstrcch
// Returns: int, # of characters from pszSource to terminating NUL
// Param:   LPCTSTR pszSource
// Note:    in _UNICODE builds NUL is 2 bytes.
int mbstrcch(LPCSTR pszSource);
int wstrcch(LPCWSTR psz);

#if defined(UNICODE)
	#define lstrcch	wstrcch
#elif defined(MBCS)
	#define lstrcch	mbstrcch
#endif


// lstrcb
// Returns: int, # of bytes from pszSource to terminating NUL
// Param:   LPCTSTR pszSource
// Note:    in _UNICODE builds NUL is 2 bytes.
int wstrcb(LPCWSTR psz);
int mbstrcb(LPCSTR psz);

#if defined(UNICODE)
	#define lstrcb	wstrcb
#elif defined(MBCS)
	#define lstrcb	mbstrcb
#endif

// lstrcbn
// Returns: int, # of bytes in first n TCHARs or cch, whichever is shorter
// Param:   LPCTSTR pszSource
int wstrcbn(LPCWSTR psz, int cch);
int mbstrcbn(LPCSTR psz, int cch);

#if defined(UNICODE)
	#define lstrcbn	wstrcbn
#elif defined(MBCS)
	#define lstrcbn	mbstrcbn
#endif


// lstrcmp
// TCHAR string compare
// Returns: int, same as CRT
// Param:   LPCSTR pszStr1
// Param:   LPCSTR pszStr2
int wstrcmp (LPCWSTR psz1, LPCWSTR psz2);
int mbstrcmp (LPCSTR psz1, LPCSTR psz2);

#ifdef lstrcmp
	#undef lstrcmp
#endif

#if defined(UNICODE)
	#define lstrcmp	wstrcmp
#elif defined(MBCS)
	#define lstrcmp	mbstrcmp
#endif


// lstrcmpn
#ifdef lstrcmpn
	#undef lstrcmpn
#endif
#define lstrcmpn	StrCmpN

// lstricmp
#ifdef lstrcmpn
	#undef lstrcmpn
#endif
#define lstrcmpn	StrCmpNI


// lstrcmpin
// TCHAR string compare, case insensitive
// Returns: int, same as CRT
// Param:   LPCSTR pszStr1
// Param:   LPCSTR pszStr2
int wstrcmpin (LPCWSTR psz1, LPCWSTR psz2, int cch);
int mbstrcmpin (LPCSTR psz1, LPCSTR psz2, int cch);

#if defined(UNICODE)
	#define lstrcmpin	wstrcmpin
#elif defined(MBCS)
	#define lstrcmpin	mbstrcmpin
#endif

// lstrrchr
LPTSTR lstrrchr(LPCTSTR pszStart, LPCTSTR pszEnd, TCHAR cFind);

// format a string
bool lstrfmt(HINSTANCE hInst, UINT nFormatID, LPTSTR pszBuf, int cchBuf...);

LPTSTR lstrfindchar(LPCTSTR pszBuf, TCHAR chFind);
LPTSTR lstrfindfirst(LPCTSTR pszBuf, LPCTSTR pszFind);
bool lstrcontains(LPCTSTR pszBuf, LPCTSTR pszSet);
int lstrcount(LPCTSTR pszBuf, const TCHAR chFind);
bool isquote(LPTSTR pszBuf);
bool lmb2w(LPCSTR pszIn, LPWSTR pszwOut, int cchOut);
bool lw2mb(LPCWSTR pszwIn, LPSTR pszOut, int cbOut);
bool pipe2null(LPTSTR psz);

DWORD MyGetLongPathName(LPCTSTR lpszShortPath, LPTSTR lpszLongPath, DWORD cchBuf);

#ifdef  __cplusplus
}
#endif

#define STR_H
#endif//STR_H
