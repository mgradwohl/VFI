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

// moved to stdafx.h
//#define _WIN32_WINNT	_WIN32_WINNT_WINBLUE //0x06000
//#define WINVER			_WIN32_WINNT_WINBLUE //0x0600
//#define	_WIN32_IE		_WIN32_IE_IE100 //0x0700
//#define	NTDDI_VERSION	NTDDI_WINBLUE //NTDDI_LONGHORN 

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

#include <windows.h>
#include "tevent.h"
//#include "FastHeap.hpp"
extern DWORD  g_dwThreadCRC;
extern HANDLE g_hThreadCRC;
extern TEvent g_eGoThreadCRC;

extern DWORD  g_dwThreadInfo;
extern HANDLE g_hThreadInfo;
extern TEvent g_eGoThreadInfo;

extern TEvent g_eTermThreads;

// CRC Memory handles
extern BYTE* g_pBuf;
extern DWORD g_dwChunk;

#ifdef TEST
	#define ALERT(x, s)	{	\
						if (!x) \
						{	\
							ErrorMessageBox(NULL, GetLastError(), L"VFI Alert", s);	\
						}	\
					}
	#else
		#define ALERT(x, s)
#endif

