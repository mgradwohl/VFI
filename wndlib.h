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
#ifndef WNDLIB_H

#ifndef __cplusplus
	#error WndLib requires C++ compilation (use a .cpp suffix)
#endif

//#ifndef _LIB
//		#ifdef _DEBUG
//			#pragma comment(lib, "wndlibdu.lib")
//		#else
//			#pragma comment(lib, "wndlibu.lib")
//		#endif
//#endif//_LIB

#include <windows.h>

// QWORD
#define QWORD			DWORDLONG
#define LPQWORD			QWORD*
#define MAKEDWORD(a, b)	((DWORD)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
#define MAKEQWORD(a, b)	((QWORD)(((DWORD)(a)) | ((QWORD)((DWORD)(b))) << 32))
//#define LODWORD(l)		((DWORD)(l))
//#define HIDWORD(l)		((DWORD)(((QWORD)(l) >> 32) & 0xFFFFFFFF))

// MoveWindowEx flags
enum { CDF_CENTER, CDF_TOPLEFT, CDF_NONE };

#define MWX_CENTER	0x0000	// 0000 0000  0000 0000
#define MWX_NORTH	0x0001	// 0000 0000  0000 0001
#define MWX_NE		0x0002	// 0000 0000  0000 0010
#define MWX_EAST	0x0003	// 0000 0000  0000 0011
#define MWX_SE		0x0004	// 0000 0000  0000 0100
#define MWX_SOUTH	0x0005	// 0000 0000  0000 0101
#define MWX_SW		0x0006	// 0000 0000  0000 0110
#define MWX_WEST	0x0007	// 0000 0000  0000 0111
#define MWX_NW		0x0008	// 0000 0000  0000 1000
#define MWX_APP		0x8000	// 1000 0000  0000 0000

UINT32 UClamp(INT_PTR val);
__int32 Clamp(INT_PTR val);
int Width(LPCRECT prc);
int Height(LPCRECT prc);
int Height(const HWND hWnd);
int Width(const HWND hWnd);
HFONT GetFont(const HWND hWnd);
void SetFont(const HWND hWnd, const HFONT hFont, const bool fRedraw);
bool ScreenToClient(const HWND hWnd, LPRECT prc);
bool ClientToScreen(const HWND hWnd, LPRECT prc);
bool MoveWindow(const HWND hWnd, LPCRECT prc, const bool fRepaint);
void ChangeDialogFont(const HWND hWnd, const HFONT hFont, const int nFlag);
HFONT GetMessageFont();
void CenterWindow(const HWND hWndParent, const HWND hWnd);
bool MoveWindowEx(const HWND hWnd, const WORD wAlign);
int ErrorMessageBox(const HWND hWnd, const DWORD dwError, LPCWSTR pszTitle, LPCWSTR pszMessage);
int ErrorMessageBox(const HINSTANCE hInst, const HWND hWnd, const DWORD dwError, const UINT idTitle, const UINT idMessage);
bool SaveBox(const HWND hWnd, LPCWSTR pszTitle, LPCWSTR pszFilter, LPWSTR pszFile, const DWORD dwFlags);
bool OpenBox(const HWND hWnd, LPCWSTR pszTitle, LPCWSTR pszFilter, LPWSTR pszFile, LPWSTR pszFolder, const DWORD dwFlags);
bool BrowseForFile(const HWND hWnd, LPCWSTR pszTitle, LPWSTR pszFolder);
bool BrowseForFolder(const HWND hWnd, LPCWSTR pszTitle, LPWSTR pszFolder);

#define WNDLIB_H
#endif//WNDLIB_H
