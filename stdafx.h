// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma warning(disable: 4711)

#ifndef STRICT
	#define STRICT
#endif

#ifndef WINVER                  // Specifies that the minimum required platform is Windows Vista.
#define WINVER 0x0600           // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT            // Specifies that the minimum required platform is Windows Vista.
#define _WIN32_WINNT 0x0600     // Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINDOWS          // Specifies that the minimum required platform is Windows 98.
#define _WIN32_WINDOWS 0x0410   // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE               // Specifies that the minimum required platform is Internet Explorer 7.0.
#define _WIN32_IE 0x0700        // Change this to the appropriate value to target other versions of IE.
#endif

#define	NTDDI_VERSION	NTDDI_LONGHORN 

#ifdef	NOLISTVIEW
#undef	NOLISTVIEW
#endif

#ifdef	NODATETIMEPICK
#undef	NODATETIMEPICK
#endif

//    NOTREEVIEW   TreeView control.
//    NOTABCONTROL Tab control.
//    NOANIMATE    Animate control.

//#define ISOLATION_AWARE_ENABLED

#include <afxpriv.h>
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcview.h>		// MFC CListView
#include <afxcmn.h>			// MFC support for Windows 95 Common Controls
#include <afxmt.h>			// MFC MultiThreading Objects
#include <afxtempl.h>
#include <uxtheme.h>
#include <winnls.h>			// Language support
#include <winbase.h>		// Types, ZeroMemory, etc
#include <shlobj.h>			// Browse for folder
#include <shlwapi.h>		// StrFormatByteSize

#pragma warning(disable:4201)
#include <mmsystem.h>
#pragma warning(default:4201)

#include <afxdisp.h>

static const int SIZEMEG = 1048576;
static const int SIZEBUF = 2 * SIZEMEG;

static const int LIST_NUMCOLUMNS = 20;
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
static const int THREAD_WAIT	=100;
// How many times should we try to kill them
static const int THREAD_TRY		=50;
#define THREAD_PRIORITY THREAD_PRIORITY_NORMAL
#define APP_PRIORITY THREAD_PRIORITY_ABOVE_NORMAL

#include "WiseFile.h"
#include "resource.h"
#include "strlib.h"
#include "wndlib.h"
#include "filelib.h"

typedef CTypedPtrList< CObList, CWiseFile* > CMyObList;
