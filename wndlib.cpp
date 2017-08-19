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

#ifndef STRICT
	#define STRICT 1
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

#include <windows.h>
#include <shlobj.h>
#include "strlib.h"
#include "wndlib.h"


UINT32 UClamp(INT_PTR val)
{
	__int64 ret = val;
	if (ret > _UI32_MAX)
	{
		ret = _UI32_MAX - 1;
	}
	return (UINT32)ret;
}

__int32 Clamp(INT_PTR val)
{
	__int64 ret = val;
	if (ret > _I32_MAX)
	{
		ret = _I32_MAX - 1;
	}
	return (__int32)ret;
}

int Width(LPCRECT prc)
{
	return abs(prc->right - prc->left);
}

int Height(LPCRECT prc)
{
	return abs(prc->bottom - prc->top);
}

int Height(const HWND hWnd)
{
	RECT rc;
	GetWindowRect(hWnd, &rc);
	return Height(&rc);
}

int Width(const HWND hWnd)
{
	RECT rc;
	GetWindowRect(hWnd, &rc);
	return Width(&rc);
}

HFONT GetFont(const HWND hWnd)
{
	return (HFONT) SendMessage(hWnd, WM_GETFONT, 0, 0);
}

void SetFont(const HWND hWnd, const HFONT hFont, const bool fRedraw)
{
	SendMessage(hWnd, WM_SETFONT, (WPARAM) hFont, MAKELPARAM(fRedraw, 0));
}

bool ScreenToClient(const HWND hWnd, LPRECT prc)
{
	return (0 != MapWindowPoints(HWND_DESKTOP, hWnd, (LPPOINT) prc, 2));
}

bool ClientToScreen(const HWND hWnd, LPRECT prc)
{
	return (0 != MapWindowPoints(hWnd, HWND_DESKTOP, (LPPOINT) prc, 2));
}

bool MoveWindow(const HWND hWnd, LPCRECT prc, const bool fRepaint)
{
	return (0 != MoveWindow(hWnd, prc->left, prc->top, Width(prc), Height(prc), fRepaint));
}

void ChangeDialogFont(const HWND hWnd, const HFONT hFont, const int nFlag)
{
	RECT rcClient, rcNewClient, rcWnd, rcNewWnd;

	// grab old and new text metrics
	TEXTMETRIC tmOld, tmNew;
	HDC hDC = GetDC(hWnd);
	HFONT hFontOld = (HFONT) SelectObject(hDC, GetFont(hWnd));
	GetTextMetrics(hDC, &tmOld);
	SelectObject(hDC, hFont);
	GetTextMetrics(hDC, &tmNew);
	SelectObject(hDC, hFontOld);
	ReleaseDC(hWnd, hDC);

	long oldHeight = tmOld.tmHeight+tmOld.tmExternalLeading;
	long newHeight = tmNew.tmHeight+tmNew.tmExternalLeading;

	if (nFlag != CDF_NONE)
	{
		// calculate new dialog window rectangle
		GetWindowRect(hWnd, &rcWnd);
		GetClientRect(hWnd, &rcClient);
		long xDiff = Width(&rcWnd) - Width(&rcClient);
		long yDiff = Height(&rcWnd) - Height(&rcClient);
	
		rcClient.left = rcNewClient.top = 0;
		rcNewClient.right = rcClient.right * tmNew.tmAveCharWidth / tmOld.tmAveCharWidth;
		rcNewClient.bottom = rcClient.bottom * newHeight / oldHeight;

		if (nFlag == CDF_TOPLEFT) // resize with origin at top/left of window
		{
			rcNewWnd.left = rcWnd.left;
			rcNewWnd.top = rcWnd.top;
			rcNewWnd.right = rcWnd.left + rcNewClient.right + xDiff;
			rcNewWnd.bottom = rcWnd.top + rcNewClient.bottom + yDiff;
		}
		else if (nFlag == CDF_CENTER) // resize with origin at center of window
		{
			rcNewWnd.left = rcWnd.left - (rcNewClient.right - rcClient.right)/2;
			rcNewWnd.top = rcWnd.top - (rcNewClient.bottom - rcClient.bottom)/2;
			rcNewWnd.right = rcNewWnd.left + rcNewClient.right + xDiff;
			rcNewWnd.bottom = rcNewWnd.top + rcNewClient.bottom + yDiff;
		}
		MoveWindow(hWnd, &rcNewWnd, false);
	}

	SetFont(hWnd, hFont, false);

	// iterate through and move all child windows and change their font.
	HWND hWndChild = GetWindow(hWnd, GW_CHILD);
	WCHAR szBuf[32];
	while (hWndChild)
	{
		SetFont(hWndChild, hFont, false);
		GetWindowRect(hWndChild, &rcWnd);
		GetClassName(hWndChild, szBuf, 31);
		if(0==lstrcmpiW(szBuf, L"COMBOBOX"))
		{
			RECT rc;
			SendMessage(hWndChild, CB_GETDROPPEDCONTROLRECT,0,(LPARAM) &rc);
			rcWnd.right = rc.right;
			rcWnd.bottom = rc.bottom;
		}

		ScreenToClient(hWnd, &rcWnd);
		rcWnd.left = rcWnd.left * tmNew.tmAveCharWidth / tmOld.tmAveCharWidth;
		rcWnd.right = rcWnd.right * tmNew.tmAveCharWidth / tmOld.tmAveCharWidth;
		rcWnd.top = rcWnd.top * newHeight / oldHeight;
		rcWnd.bottom = rcWnd.bottom * newHeight / oldHeight;
		MoveWindow(hWndChild, &rcWnd, false);
		
		hWndChild = GetWindow(hWndChild, GW_HWNDNEXT);
	}
}

HFONT GetMessageFont()
{
    NONCLIENTMETRICS ncm;
    ZeroMemory(&ncm, sizeof(NONCLIENTMETRICS));
    ncm.cbSize = sizeof(NONCLIENTMETRICS);
    SystemParametersInfo( SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0);
	return CreateFontIndirect(&ncm.lfMessageFont);
}

void CenterWindow(const HWND hWndParent, const HWND hWnd)
{
	// unused
	hWndParent;

	RECT rcWnd;
	RECT rcMon;
    GetWindowRect(hWnd, &rcWnd);

	MONITORINFO mi;
    mi.cbSize = sizeof(mi);
    GetMonitorInfo(MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST), &mi);

    rcMon = mi.rcMonitor; // or mi.rcWork

    rcWnd.left   = rcMon.left + ((rcMon.right  - rcMon.left) - 
                                 (rcWnd.right  - rcWnd.left)) / 2;
    rcWnd.top    = rcMon.top  + ((rcMon.bottom - rcMon.top)  - 
                                 (rcWnd.bottom - rcWnd.top)) / 2;

    SetWindowPos(hWnd, NULL, rcWnd.left, rcWnd.top, 
                 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

bool MoveWindowEx(const HWND hWnd, const WORD wAlign)
{
	if (!IsWindow(hWnd))
	{
		return false;
	}
	
	RECT rc;
	HWND hWndParent = GetParent(hWnd);

	if (wAlign & MWX_APP)
	{
		// move based on application window rect
		if (NULL==hWndParent || !::IsWindow(hWndParent))
		{
			TRACE(L"MoveWindowEx invalid main window, centering on desktop.\n");
			SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
		}
		else
		{
			GetWindowRect(hWndParent, &rc);
		}
	}
	else
	{
		// move based on usable window rect
		HMONITOR hMon = ::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi;
		ZeroMemory(&mi, sizeof(mi));
		mi.cbSize = sizeof(mi);
		if (!GetMonitorInfo(hMon, &mi))
		{
			SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
		}
		else
		{
			CopyRect(&rc, &(mi.rcWork));
		}
	}

	// Get size of window and calc current width and height
	RECT rcBox;
	GetWindowRect(hWnd, &rcBox);
	int iW = Width(&rcBox);
	int iH = Height(&rcBox);

	switch (LOBYTE(wAlign))
	{
	case MWX_CENTER:
		{
			rcBox.top = abs(rc.bottom - rc.top - iH) / 2;
			rcBox.left = abs(rc.right - rc.left - iW) / 2;
		}
		break;
	case MWX_NORTH:
		{
			rcBox.top = rc.top;
			rcBox.left = abs(rc.right - rc.left - iW) / 2;
		}
		break;
	case MWX_NE:
		{
			rcBox.top = rc.top;
			rcBox.left = rc.right - iW;
		}
		break;
	case MWX_EAST:
		{
			rcBox.top = abs(rc.bottom - rc.top - iH) / 2;
			rcBox.left = rc.right - iW;
		}
		break;
	case MWX_SE:
		{
			rcBox.top = rc.bottom - iH;
			rcBox.left = rc.right - iW;
		}
		break;
	case MWX_SOUTH:
		{
			rcBox.top = rc.bottom - iH;
			rcBox.left = abs(rc.right - rc.left - iW) / 2;
		}
		break;
	case MWX_SW:
		{
			rcBox.top = rc.bottom - iH;
			rcBox.left = rc.left;
		}
		break;
	case MWX_WEST:
		{
			rcBox.top = abs(rc.bottom - rc.top - iH) / 2;
			rcBox.left = rc.left;
		}
		break;
	case MWX_NW:
		{
			rcBox.top = rc.top;
			rcBox.left = rc.left;
		}
		break;
	}
	rcBox.bottom = rcBox.top + iH;
	rcBox.right = rcBox.left + iW;

	if (wAlign & MWX_APP)
	{
		ClientToScreen(hWnd, &rcBox);
	}
	MoveWindow(hWnd, &rcBox, TRUE);
	return true;
}

int ErrorMessageBox(const HWND hWnd, const DWORD dwError, LPCWSTR pszTitle, LPCWSTR pszMessage)
{
	LPVOID pBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, dwError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&pBuf, 0, NULL);

	LPWSTR pszBuf = new WCHAR[(lstrlen(pszMessage) + lstrlen((LPCWSTR)pBuf) + 256) * 2];
	wsprintf(pszBuf, TEXT("%s\r\n\r\nError number: %lu\r\n\r\n%s"), pszMessage, dwError, (LPCWSTR)pBuf);
	int error = MessageBox(hWnd, pszBuf, pszTitle, MB_OK | MB_ICONINFORMATION);

	LocalFree(pBuf);
	delete [] pszBuf;
	return error;
}

int ErrorMessageBox(const HINSTANCE hInst, const HWND hWnd, const DWORD dwError, const UINT idTitle, const UINT idMessage)
{
	LPVOID pBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, dwError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&pBuf, 0, NULL);

	WCHAR szMessage[4096];
	WCHAR szTitle[128];
	LoadString(hInst, idMessage, szMessage, 4096);
	LoadString(hInst, idTitle, szTitle, 128);

	LPWSTR pszBuf = new WCHAR[(lstrlen(szMessage) + lstrlen((LPCWSTR)pBuf) + 256)];
	wsprintf(pszBuf, TEXT("%s\r\n\r\nError number: %lu\r\n\r\n%s"), szMessage, dwError, (LPCWSTR)pBuf);
	int error = MessageBox(hWnd, pszBuf, szTitle, MB_OK | MB_ICONINFORMATION);

	LocalFree(pBuf);
	delete [] pszBuf;
	return error;
}

bool SaveBox(const HWND hWnd, LPCWSTR pszTitle, LPCWSTR pszFilter, LPWSTR pszFile, const DWORD dwFlags)
{
	WCHAR szFile[MAX_PATH];
	*szFile = L'\0';
	WCHAR szDesktop[MAX_PATH];
	SHGetSpecialFolderPath(NULL, szDesktop, CSIDL_DESKTOPDIRECTORY, FALSE);

	OPENFILENAME of;
	::ZeroMemory(&of, sizeof(OPENFILENAME));

    of.lStructSize = sizeof(OPENFILENAME); 
    of.hwndOwner = hWnd; 
    of.hInstance = GetModuleHandle(NULL); 
    of.lpstrFilter = pszFilter;	//TEXT("Executable\0*.EXE\0Dynamic Link Library\0*.DLL\0Movie\0*.AVI\0\0"); 
    of.lpstrFile = pszFile; 
    of.nMaxFile = MAX_PATH; 
    of.lpstrInitialDir = szDesktop;
    of.lpstrTitle = pszTitle;
    of.Flags = dwFlags | OFN_LONGNAMES | OFN_ENABLESIZING | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_NONETWORKBUTTON; 
 
	return (0 != GetSaveFileName(&of));
}

bool OpenBox(const HWND hWnd, LPCWSTR pszTitle, LPCWSTR pszFilter, LPWSTR pszFile, LPWSTR pszFolder, const DWORD dwFlags)
{

	OPENFILENAME of;
	::ZeroMemory(&of, sizeof(OPENFILENAME));
	WCHAR szFile[MAX_PATH];
	*szFile = L'\0';
	
	if (NULL == pszFolder)
	{
        WCHAR szDesktop[MAX_PATH];
		SHGetSpecialFolderPath(NULL, szDesktop, CSIDL_DESKTOPDIRECTORY, FALSE);
	    of.lpstrInitialDir = szDesktop;
	}
	else
	{
	    of.lpstrInitialDir = pszFolder;
	}

    of.lStructSize = sizeof(OPENFILENAME); 
    of.hwndOwner = hWnd; 
    of.hInstance = GetModuleHandle(NULL); 
    of.lpstrFilter = pszFilter;	//TEXT("Executable\0*.EXE\0Dynamic Link Library\0*.DLL\0Movie\0*.AVI\0\0"); 
    of.lpstrFile = pszFile; 
    of.nMaxFile = MAX_PATH; 
    of.lpstrTitle = pszTitle;
    of.Flags = dwFlags | OFN_DONTADDTORECENT | OFN_LONGNAMES | OFN_ENABLESIZING | OFN_EXPLORER | OFN_NONETWORKBUTTON; 
 
	return (0 != GetOpenFileName(&of));
}

bool BrowseForFile(const HWND hWnd, LPCWSTR pszTitle, LPWSTR pszFolder)
{
	if(NULL == pszFolder)
		return false;
	
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(BROWSEINFO));
	LPMALLOC pMalloc;
	if (SUCCEEDED(CoGetMalloc(MEMCTX_TASK, &pMalloc)))
	{
		bi.hwndOwner = hWnd;
		bi.lpszTitle = pszTitle;
		bi.lpfn = NULL;
		bi.pszDisplayName = pszFolder;
		bi.ulFlags = BIF_BROWSEINCLUDEFILES | BIF_VALIDATE | BIF_EDITBOX | BIF_USENEWUI | BIF_NEWDIALOGSTYLE | BIF_STATUSTEXT | BIF_RETURNONLYFSDIRS;
		bi.pidlRoot = NULL;

		LPITEMIDLIST pidList = SHBrowseForFolder(&bi);
		if (NULL == pidList)
		{
			return false;
		}

		if (TRUE == SHGetPathFromIDList(pidList, pszFolder))
		{
			pMalloc->Free(pidList);
		}
		pMalloc->Release();
		return true;
	}
	return false;
}

bool BrowseForFolder(const HWND hWnd, LPCWSTR pszTitle, LPWSTR pszFolder)
{
	if(NULL == pszFolder)
		return false;
	
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(BROWSEINFO));
	LPMALLOC pMalloc;
	//SHGetMalloc(&pMalloc);
	if (SUCCEEDED(CoGetMalloc(MEMCTX_TASK, &pMalloc)))
	{
		bi.hwndOwner = hWnd;
		bi.lpszTitle = pszTitle;
		bi.lpfn = NULL;
		bi.pszDisplayName = pszFolder;
		bi.ulFlags = BIF_VALIDATE | BIF_EDITBOX | BIF_USENEWUI | BIF_NEWDIALOGSTYLE | BIF_STATUSTEXT | BIF_RETURNONLYFSDIRS;
		bi.pidlRoot = NULL;

		LPITEMIDLIST pidList = SHBrowseForFolder(&bi);
		if (NULL == pidList)
		{
			pMalloc->Release();
			return false;
		}

		if (TRUE == SHGetPathFromIDList(pidList, pszFolder))
		{
			pMalloc->Free(pidList);
		}

		pMalloc->Release();
		return true;
	}
	return false;
}
