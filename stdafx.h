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

// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

//#pragma warning(disable: 4711)

#ifndef STRICT
	#define STRICT
#endif

/*
#define _WIN32_WINNT	_WIN32_WINNT_WINBLUE //0x06000
#define WINVER			_WIN32_WINNT_WINBLUE //0x0600
#define	_WIN32_IE		_WIN32_IE_IE100 //0x0700
#define	NTDDI_VERSION	NTDDI_WINBLUE //NTDDI_LONGHORN
*/

#ifndef WINVER						// Specifies that the minimum required platform is Windows Vista.
#define WINVER _WIN32_WINNT_WINBLUE	//0x0600           // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT						// Specifies that the minimum required platform is Windows Vista.
#define _WIN32_WINNT _WIN32_WINNT_WINBLUE	//0x0600     // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINDOWS          // Specifies that the minimum required platform is Windows 98.
#define _WIN32_WINDOWS 0x0410   // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE					// Specifies that the minimum required platform is Internet Explorer 7.0.
#define _WIN32_IE _WIN32_IE_IE100	//0x0700        // Change this to the appropriate value to target other versions of IE.
#endif


#ifdef	NOLISTVIEW
#undef	NOLISTVIEW
#endif

#ifdef	NODATETIMEPICK
#undef	NODATETIMEPICK
#endif

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcview.h>		// MFC CListView
#include <afxmt.h>			// MFC MultiThreading Objects
#include <winnls.h>			// Language support
#include <winbase.h>		// Types, ZeroMemory, etc
#include <shlobj.h>			// Browse for folder
#include <mmsystem.h>

static const int SIZEMEG = 1048576;
static const int SIZEBUF = 2 * SIZEMEG;

static const int LIST_NUMCOLUMNS = 19;
static const int LIST_MAXHEADLENGTH	= (64 * sizeof(WCHAR));


// Hints
enum Hints
{
	HINT_EMPTY	=	0x0001,
	HINT_ADD	=	0x0002,
	HINT_DELETE	=	0x0003,
	HINT_REFRESH=	0x0004,
	HINT_REBUILD=	0x0005,
	HINT_DONECRC=	0x0F01,
	HINT_DONEINFO=	0x0F02
};


//// Column Sort Flags
//enum Sorts
//{
//	SORT_NONE		=0,
//	SORT_ASCENDING	=1,
//	SORT_DESCENDING	=2
//};

// MoveWindowEx flags
enum MWXFlags
{
	MWX_CENTER	=0x0000,
	MWX_NORTH	=0x0001,
	MWX_NE		=0x0002,
	MWX_EAST	=0x0003,
	MWX_SE		=0x0004,
	MWX_SOUTH	=0x0005,
	MWX_SW		=0x0006,
	MWX_WEST	=0x0007,
	MWX_NW		=0x0008,
	MWX_APP		=0x8000
};
						
// How long should we wait for the threads to die in ms
static const int THREAD_WAIT	=250;
// How many times should we try to kill them
static const int THREAD_TRY		=10;
#define THREAD_PRIORITY THREAD_PRIORITY_NORMAL
#define APP_PRIORITY THREAD_PRIORITY_ABOVE_NORMAL

#include "WiseFile.h"
#include "resource.h"
#include "strlib.h"
#include "wndlib.h"
#include "filelib.h"
#include <memory>
#include <vector>

using namespace std;
