#define _WIN32_WINNT	0x0600
#define WINVER			0x0600
#define	_WIN32_IE		0x0700
#define	NTDDI_VERSION	NTDDI_LONGHORN 

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