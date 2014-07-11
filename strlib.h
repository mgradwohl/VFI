// strlib.h
// mattgr
// 3/23/2000

#pragma once
#ifndef STRLIB_H

#ifndef __cplusplus
	#error StrLib requires C++ compilation (use a .cpp suffix)
#endif

//#ifndef _LIB
//		#ifdef _DEBUG
//			#pragma comment(lib, "strlibdu.lib")
//		#else
//			#pragma comment(lib, "strlibu.lib")
//		#endif
//#endif//_LIB

#include <windows.h>
#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib")

#include <lmcons.h>		// for UNLEN only

#include <stdio.h>

#ifndef QWORD
	#define QWORD			DWORDLONG
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
#if !defined(__T)
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
#endif

// types
typedef unsigned char* LPXSTR;
typedef const unsigned char* LPCXSTR;

#if defined(UNICODE)
	#define TMAX_PATH	(_MAX_PATH)
	#define TMAX_DRIVE	(_MAX_DRIVE)
	#define TMAX_DIR	(_MAX_DIR)
	#define TMAX_FNAME	(_MAX_FNAME)
	#define TMAX_EXT	(_MAX_EXT)
	#define TMAX_USERNAME ((UNLEN + 1))
#else
	#define TMAX_PATH	(_MAX_PATH*2)
	#define TMAX_DRIVE	(_MAX_DRIVE*2)
	#define TMAX_DIR	(_MAX_DIR*2)
	#define TMAX_FNAME	(_MAX_FNAME*2)
	#define TMAX_EXT	(_MAX_EXT*2)
	#define TMAX_USERNAME ((UNLEN + 1) * 2)
#endif

// Double these because in the worst case in an MBCS system, they are wide

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
#define zero(x)			(::SecureZeroMemory((LPVOID)&x, sizeof(x)))

// lstrcch
// Returns: int, # of characters from pszSource to terminating NULL
// Param:   LPCWSTR pszSource
// Note:
int mbstrcch(LPCSTR pszSource);
int wstrcch(LPCWSTR psz);

// lstrcb
// Returns: int, # of bytes from pszSource to terminating NULL
// Param:   LPCWSTR pszSource
// Note:
int wstrcb(LPCWSTR psz);
int mbstrcb(LPCSTR psz);

// lstrcbn
// Returns: int, # of bytes in first n characters or lstrcb, whichever is shorter
// Param:   LPCWSTR pszSource
int wstrcbn(LPCWSTR psz, int cch);
int mbstrcbn(LPCSTR psz, int cch);

#if defined(UNICODE)
	#define lstrcch	wstrcch
	#define lstrcb	wstrcb
	#define lstrcbn	wstrcbn
#elif defined(MBCS)
	#define lstrcch	mbstrcch
	#define lstrcb	mbstrcb
	#define lstrcbn	mbstrcbn
#endif

#ifdef lstrcmp
	#undef lstrcmp
#endif

#ifdef lstrcmpi
	#undef lstrcmpi
#endif

#define lstrcmp		StrCmp
#define lstrcmpi	StrCmpI
#define lstrcmpn	StrCmpN
#define lstrcmpni	StrCmpNI
#define lstrchr		StrChr
#define lstrrchr	StrRChr

#define lstrfindfirst	StrPBrk

// Get number as a string
bool int2str(LPWSTR pszDest, QWORD i);
bool float2str(LPWSTR pszDest, double d);


bool lstrcontains(LPCWSTR pszBuf, LPCWSTR pszSet);

int lstrcount(LPCWSTR pszBuf, TCHAR chFind);

bool isquote(LPWSTR pszBuf);
bool lmb2w(LPCSTR pszIn, LPWSTR pszOut, int cchOut);
bool lw2mb(LPCWSTR pszIn, LPSTR pszOut, int cbOut);
bool pipe2null(LPWSTR psz);

bool strfmt(HINSTANCE hInstance, LPWSTR pszBuffer, UINT nFormatID, ...);

bool MyGetUserName(LPWSTR pszUserName);

#define STRLIB_H
#endif//STRLIB_H
