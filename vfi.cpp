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

// VFI.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include <mshtmhst.h>
#include <urlmon.h>

#include "filelib.h"
#include "wndlib.h"
#include "vfi.h"
#include "Frame.h"
#include "Doc.h"
#include "mylistview.h"
//#include "version.h"
#include "helpdlg.h"
#include "globals.h"

size_t BSTRToLocal(LPWSTR pLocal, BSTR pWide, DWORD dwChars);
size_t LocalToBSTR(BSTR pWide, LPWSTR pLocal, DWORD dwChars);

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static CHAR THIS_FILE[] = __FILE__;
#endif


size_t BSTRToLocal(LPWSTR pLocal, BSTR pWide, DWORD dwChars)
{
	if (pLocal == nullptr)
		return 0;

	*pLocal = 0;
	wcscpy_s(pLocal, dwChars, pWide);
	return wcslen(pLocal);
}

size_t LocalToBSTR(BSTR pWide, LPWSTR pLocal, DWORD dwChars)
{
	if (pWide == nullptr)
		return 0;

	*pWide = 0;
	wcscpy_s(pWide, dwChars, pLocal);
	return wcslen(pWide);
}

void ChangeDialogFont(CWnd* pWnd, CFont* pFont, int nFlag)
{
	CRect windowRect;

	// grab old and new text metrics
	TEXTMETRIC tmOld, tmNew;
	CDC * pDC = pWnd->GetDC();
	CFont * pSavedFont = pDC->SelectObject(pWnd->GetFont());
	pDC->GetTextMetrics(&tmOld);
	pDC->SelectObject(pFont);
	pDC->GetTextMetrics(&tmNew);
	pDC->SelectObject(pSavedFont);
	pWnd->ReleaseDC(pDC);

	long oldHeight = tmOld.tmHeight+tmOld.tmExternalLeading;
	long newHeight = tmNew.tmHeight+tmNew.tmExternalLeading;

	if (nFlag != CDF_NONE)
	{
		// calculate new dialog window rectangle
		CRect clientRect, newClientRect, newWindowRect;

		pWnd->GetWindowRect(windowRect);
		pWnd->GetClientRect(clientRect);
		long xDiff = windowRect.Width() - clientRect.Width();
		long yDiff = windowRect.Height() - clientRect.Height();
	
		newClientRect.left = newClientRect.top = 0;
		newClientRect.right = clientRect.right * tmNew.tmAveCharWidth / tmOld.tmAveCharWidth;
		newClientRect.bottom = clientRect.bottom * newHeight / oldHeight;

		if (nFlag == CDF_TOPLEFT) // resize with origin at top/left of window
		{
			newWindowRect.left = windowRect.left;
			newWindowRect.top = windowRect.top;
			newWindowRect.right = windowRect.left + newClientRect.right + xDiff;
			newWindowRect.bottom = windowRect.top + newClientRect.bottom + yDiff;
		}
		else if (nFlag == CDF_CENTER) // resize with origin at center of window
		{
			newWindowRect.left = windowRect.left - 
							(newClientRect.right - clientRect.right)/2;
			newWindowRect.top = windowRect.top -
							(newClientRect.bottom - clientRect.bottom)/2;
			newWindowRect.right = newWindowRect.left + newClientRect.right + xDiff;
			newWindowRect.bottom = newWindowRect.top + newClientRect.bottom + yDiff;
		}
		pWnd->MoveWindow(newWindowRect);
	}

	pWnd->SetFont(pFont);

	// iterate through and move all child windows and change their font.
	CWnd* pChildWnd = pWnd->GetWindow(GW_CHILD);

	while (pChildWnd)
	{
		pChildWnd->SetFont(pFont);
		pChildWnd->GetWindowRect(windowRect);

		CString strClass;
		::GetClassName(pChildWnd->m_hWnd, strClass.GetBufferSetLength(32), 31);
		strClass.MakeUpper();
		if(strClass=="COMBOBOX")
		{
			CRect rect;
			pChildWnd->SendMessage(CB_GETDROPPEDCONTROLRECT,0,(LPARAM) &rect);
			windowRect.right = rect.right;
			windowRect.bottom = rect.bottom;
		}

		pWnd->ScreenToClient(windowRect);
		windowRect.left = windowRect.left * tmNew.tmAveCharWidth / tmOld.tmAveCharWidth;
		windowRect.right = windowRect.right * tmNew.tmAveCharWidth / tmOld.tmAveCharWidth;
		windowRect.top = windowRect.top * newHeight / oldHeight;
		windowRect.bottom = windowRect.bottom * newHeight / oldHeight;
		pChildWnd->MoveWindow(windowRect);
		
		pChildWnd = pChildWnd->GetWindow(GW_HWNDNEXT);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMyApp

BEGIN_MESSAGE_MAP(CMyApp, CWinApp)
	//{{AFX_MSG_MAP(CMyApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyApp construction

CMyApp::CMyApp()
{
	SetThreadPriority(APP_PRIORITY);
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMyApp object

CMyApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMyApp initialization

BOOL CMyApp::InitInstance()
{
#ifdef _DEBUG
	afxDump.SetDepth(2);
#endif

	// Allow only one instance
	if ( FALSE == FirstInstance() )
	{
		SetFocusToPrevInstance();
		return FALSE;
	}

	// for LVM_SETBKIMAGE
	//if (S_OK != CoInitialize(NULL))
	//{
	//	ASSERT(false);
	//}

	// Init the Rich Edit Box
	AfxInitRichEdit();

	// Register message for safely updating from another thread
	m_wmUpdateView = ::RegisterWindowMessage( L"VFI_UpdateView");
	if (0 == m_wmUpdateView)
	{
		return FALSE;
	}

	// Use an INI not the Registry
	WCHAR szIniName[_MAX_PATH];
	GetModuleFolder(GetModuleHandle(NULL), m_szIniPath);
	if (!PathIsLocal(m_szIniPath) || !PathIsWritable(m_szIniPath))
	{
		ErrorMessageBox(AfxGetResourceHandle(),
			NULL,
			GetLastError(),
			ERR_TITLE,
			ERR_INSTALL);
		return FALSE;
	}
	::LoadString(AfxGetResourceHandle(), STR_INIFILENAME, szIniName, _MAX_PATH);
	PathAppend(m_szIniPath, szIniName);
	free((void*) (m_pszProfileName));

	// tried to use StrDup but it causes a crash on ~CWinApp
	m_pszProfileName = _tcsdup(m_szIniPath);
	TRACE1( ">>> m_pszProfileName: %s\r\n",m_pszProfileName);

	LoadStdProfileSettings( 0 );

	// Register document templates
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CMyDoc),
		RUNTIME_CLASS(CMainFrame),
		RUNTIME_CLASS(CMyListView));
	AddDocTemplate(pDocTemplate);

	// Create a new Doc
	OnFileNew();

	// Allow Drag and Drop
	m_pMainWnd->DragAcceptFiles();

	// Show quick help
	BOOL fShowQuickHelp = (1 == theApp.GetProfileInt(L"Preferences", L"ShowQuickHelp", 1));
	if (fShowQuickHelp)
	{
		CHelpDlg dlgHelp(AfxGetMainWnd(), IDR_RTF_HELP, true);
		dlgHelp.m_fShowAgain = fShowQuickHelp;
		dlgHelp.DoModal();
		fShowQuickHelp = dlgHelp.m_fShowAgain;
		WriteProfileInt( L"Preferences", L"ShowQuickHelp",fShowQuickHelp == TRUE ? 1 : 0);
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	public:
	virtual INT_PTR DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	WORD m_wAlign;
	int m_iBorder;
	bool m_fBigDialog;
	SIZE_T dwTotalPhys;
	SIZE_T dwAvailPhys;
public:
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
	m_fBigDialog = false;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CMyApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CMyApp commands

bool CMyApp::ForwardMessages()
{
	MSG msg;
	LONG lIdle = 0;

	while (::PeekMessage(&msg, NULL, 0,0, PM_REMOVE) )
	{
		if (!AfxGetApp()->PreTranslateMessage( &msg ))
		{
			if( msg.message == WM_QUIT ||
				msg.message == WM_CLOSE ||
				msg.message == WM_SYSCOMMAND ||
				msg.message == WM_DESTROY )
			{
				::PostQuitMessage( (int)msg.wParam );
				return true;
			}
			::TranslateMessage( &msg );
			::DispatchMessage( &msg );
		}
	}
	while ( AfxGetApp()->OnIdle(lIdle++ ) );
	return false;
}

INT_PTR CAboutDlg::DoModal() 
{
	return CDialog::DoModal();
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// set the Title
	CStatic* pCtl = static_cast<CStatic*> (GetDlgItem(IDC_TITLE));
	if (pCtl == nullptr)
	{
		return false;
	}

	CString str;
	//TODO put our version information in here
	str.FormatMessage(STR_TITLE, L"4.90", L"4.90.1708.2201");
	pCtl->SetWindowText(str);

	// set the Author
	pCtl = static_cast<CStatic*> (GetDlgItem(IDC_AUTHOR));
	if (pCtl == nullptr)
	{
		return false;
	}

	str.LoadString(STR_AUTHOR);
	pCtl->SetWindowText(str);

	return TRUE;
}


BOOL CMyApp::FirstInstance()
{
	CString strEvent;

	strEvent.LoadString(AFX_IDS_APP_TITLE);
	strEvent += "::SingleInstanceEvent";
	if (m_eOneTime.Create(strEvent))
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CMyApp::SetFocusToPrevInstance()
{
	CWnd* pWndPrev;
	CWnd* pWndChild;
	CString strWndTitle;

	strWndTitle.LoadString(AFX_IDS_APP_TITLE);

	pWndPrev = CWnd::FindWindow(NULL, strWndTitle);
	if (NULL != pWndPrev)
	{
		pWndChild = pWndPrev->GetLastActivePopup();

		if (pWndPrev->IsIconic())
		{
			pWndPrev->ShowWindow(SW_RESTORE);
		}
		pWndChild->SetForegroundWindow();
		
		return TRUE;
	}
	return FALSE;
}

int CMyApp::ExitInstance() 
{
	CoUninitialize();
	m_eOneTime.Close();
	return CWinApp::ExitInstance();
}

CFont* CMyApp::GetUIFont()
{
	CMainFrame* pFrame = static_cast<CMainFrame*> (AfxGetMainWnd());
	if (NULL == pFrame)
		return NULL;

	return pFrame->GetUIFont();
}


void CAboutDlg::OnOK() 
{
	CDialog::OnOK();
}