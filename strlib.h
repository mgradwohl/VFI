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

#pragma once

#ifndef __cplusplus
	#error StrLib requires C++ compilation (use a .cpp suffix)
#endif

#include <shlwapi.h>
#include <lmcons.h>		// for UNLEN only
#include <stdio.h>

#include "globals.h"

// make sure that the CRT and Win32 are in-synch
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

#if defined(UNICODE)
	#define MAX_DRIVE	(_MAX_DRIVE)
	#define MAX_DIR	(_MAX_DIR)
	#define MAX_FNAME	(_MAX_FNAME)
	#define MAX_EXT	(_MAX_EXT)
	#define MAX_USERNAME ((UNLEN + 1))
#endif
size_t const maxExtendedPathLength = 0x7FFF - 24;

// QWORD
#define QWORD			DWORDLONG
#define LPQWORD			QWORD*
#define MAKEDWORD(a, b)	((DWORD)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
#define MAKEQWORD(a, b)	((QWORD)(((DWORD)(a)) | ((QWORD)((DWORD)(b))) << 32))

// lccb
// LCHAR count bytes
#define sccb	 sizeof(char)
#define	mccb	 sizeof(wchar_t)
#define wccb	 sizeof(wchar_t)

#define lccb wccb

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

	#define lstrcch	wstrcch
	#define lstrcb	wstrcb
	#define lstrcbn	wstrcbn

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

int lstrcount(LPCWSTR pszBuf, WCHAR chFind);

bool isquote(LPWSTR pszBuf);
bool lmb2w(LPCSTR pszIn, LPWSTR pszOut, int cchOut);
bool lw2mb(LPCWSTR pszIn, LPSTR pszOut, int cbOut);
bool pipe2null(LPWSTR psz);

bool strfmt(HINSTANCE hInstance, LPWSTR pszBuffer, UINT nFormatID, ...);

bool MyGetUserName(LPWSTR pszUserName);

__forceinline void lstrinit(LPWSTR psz)
{
	*psz = '\0';
}
