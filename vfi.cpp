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
#include "version.h"
#include "helpdlg.h"
#include "globals.h"

//#define ARRAYSIZE(a)    (sizeof(a)/sizeof((a)[0]))

int BSTRToLocal(LPWSTR pLocal, BSTR pWide, DWORD dwChars);
int LocalToBSTR(BSTR pWide, LPWSTR pLocal, DWORD dwChars);

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static CHAR THIS_FILE[] = __FILE__;
#endif


int BSTRToLocal(LPWSTR pLocal, BSTR pWide, DWORD dwChars)
{
	*pLocal = 0;
	
#ifdef UNICODE
	lstrcpyn(pLocal, pWide, dwChars);
#else
	WideCharToMultiByte( CP_ACP, 
		0, 
		pWide, 
		-1, 
		pLocal, 
		dwChars, 
		NULL, 
		NULL);
#endif
	
	return lstrcch(pLocal);
}

int LocalToBSTR(BSTR pWide, LPWSTR pLocal, DWORD dwChars)
{
	*pWide = 0;
	
#ifdef UNICODE
	lstrcpyn(pWide, pLocal, dwChars);
#else
	MultiByteToWideChar( CP_ACP, 
		0, 
		pLocal, 
		-1, 
		pWide, 
		dwChars); 
#endif
	
	return lstrlenW(pWide);
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
	if (S_OK != CoInitialize(NULL))
	{
		ASSERT(false);
	}

	// Init the Rich Edit Box
	AfxInitRichEdit();

	// Register message for safely updating from another thread
	m_wmUpdateView = ::RegisterWindowMessage( L"VFI_UpdateView");
	if (0 == m_wmUpdateView)
	{
		return FALSE;
	}

	// Use an INI not the Registry
	WCHAR szIniName[MAX_PATH];
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
	::LoadString(AfxGetResourceHandle(), STR_INIFILENAME, szIniName, MAX_PATH);
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
	BOOL fShowQuickHelp = (1 == theApp.GetProfileInt( L"Preferences", L"ShowQuickHelp",1) );
	if (fShowQuickHelp)
	{
		CHelpDlg dlgHelp(AfxGetMainWnd(), IDR_RTF_HELP, true);
		dlgHelp.m_fShowAgain = fShowQuickHelp;
		dlgHelp.DoModal();
		fShowQuickHelp = dlgHelp.m_fShowAgain;
		WriteProfileInt( L"Preferences", L"ShowQuickHelp",fShowQuickHelp == TRUE ? 1 : 0);
	}
	//ShowHTMLBox();

	// Compact the Heap (garbage collect)
	// mattgr
	// 2/10/07: probably not needed in a Vista world
	//::HeapCompact(::GetProcessHeap(), 0);
	//::SetProcessWorkingSetSize( ::GetCurrentProcess(), 0xFFFFFFFF, 0xFFFFFFFF);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	bool ResizeDialog(bool fBigDialog = false);
	void GrowDialog();
	void ShrinkDialog();
	bool UpdateMemoryInfo();
	void Compact(bool fCompact = false);
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	public:
	virtual int DoModal();
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
	DWORD dwTotalPhys;
	DWORD dwAvailPhys;
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
                ::PostQuitMessage( msg.wParam );
				return true;
			}
			::TranslateMessage( &msg );
			::DispatchMessage( &msg );
		}
	}
	while ( AfxGetApp()->OnIdle(lIdle++ ) );
	return false;
}

int CAboutDlg::DoModal() 
{
	return CDialog::DoModal();
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	//ChangeDialogFont(this, theApp.GetUIFont(), CDF_TOPLEFT);

	MoveWindowEx(m_hWnd, MWX_APP | MWX_CENTER);
	// find out how big the dialog is
	CRect rcWnd;
	GetWindowRect(&rcWnd);

	// figure out the border size
	CStatic* pCtl = static_cast<CStatic*> (GetDlgItem(IDC_MEMORY));
	CRect rcCtl;
	pCtl->GetWindowRect(&rcCtl);
	m_iBorder = abs(rcWnd.bottom - rcCtl.bottom);

	ResizeDialog(false);

	// set the Title
	pCtl = static_cast<CStatic*> (GetDlgItem(IDC_TITLE));
	ASSERT(pCtl);
	CString str;
	str.FormatMessage(STR_TITLE, STAMPER_STR_PROD_VERSION, STAMPER_STR_FILE_VERSION);
	pCtl->SetWindowText(str);

	// set the Author
	pCtl = static_cast<CStatic*> (GetDlgItem(IDC_AUTHOR));
	ASSERT(pCtl);
	str.LoadString(STR_AUTHOR);
	pCtl->SetWindowText(str);

	// set the system info
	CSystemInformation si;
	WCHAR szOS[256];
	WCHAR szCPU[256];
	WCHAR szIE[256];
	WCHAR szComCtl[256];
	WCHAR szLanguage[256];
	WCHAR szKeyboard[256];
	WCHAR szCountry[256];
	//WCHAR szEvents[256];

	si.GetOSVersionString(szOS);
	si.GetCPUDescription(szCPU);
	si.GetIEVersionString(szIE);
	si.GetComCtlVersionString(szComCtl);
	si.GetLanguage(szLanguage, 256);
	si.GetCountry(szCountry, 256);
	si.GetKeyboardLayout(szKeyboard);

	str.Format( L"%s\r\nCPU: %s\r\nCountry: %s\r\nLanguage: %s\r\nKeyboard: %s\r\nInternet Explorer: %s\r\nCommon Controls: %s",
		szOS,
		szCPU,
		szCountry,
		szLanguage,
		szKeyboard,
		szIE,
		szComCtl);

	pCtl = static_cast<CStatic*> (GetDlgItem(IDC_SYSINFO));
	pCtl->SetWindowText(str);
	
	// set the Event information
	CMainFrame* pFrame = static_cast<CMainFrame*> (AfxGetMainWnd());
	CMyDoc* pDoc = static_cast<CMyDoc*> (pFrame->GetActiveDocument());
	ASSERT (pDoc);
	str.Format(L"Info Thread: %s\tCount: %u\r\nCRC Thread: %s\tCount: %u\r\nTerminate: %s",
		g_eGoThreadInfo.Signaled() ? L"running" : L"paused", pDoc->m_dwDirtyInfo,
		g_eGoThreadCRC.Signaled() ? L"running" : L"paused", pDoc->m_dwDirtyCRC,
		g_eTermThreads.Signaled() ? L"yes" : L"no");

	pCtl = static_cast<CStatic*> (GetDlgItem(IDC_EVENTINFO));
	pCtl->SetWindowText(str);

	Compact(false);

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
	int key = ::GetAsyncKeyState(VK_SHIFT);
	if ( 0xffff8001 == key)
	{
		Compact(false);
		UpdateMemoryInfo();
		ResizeDialog(!m_fBigDialog);
		return;
	}

	key = ::GetAsyncKeyState(VK_CONTROL);
	if ( 0xffff8001 == key)
	{
		Compact(true);
		UpdateMemoryInfo();
		return;
	}

	// fall thru if no special keys pressed
	CDialog::OnOK();
}

void CAboutDlg::Compact(bool fCompact)
{
	if (fCompact)
	{
		::HeapCompact( ::GetProcessHeap(), 0 );
		::SetProcessWorkingSetSize( ::GetCurrentProcess(), 0xFFFFFFFF, 0xFFFFFFFF );
	}
	
	MEMORYSTATUS ms;
	ZeroMemory( &ms, sizeof(MEMORYSTATUS) );
	ms.dwLength = sizeof( MEMORYSTATUS );
	GlobalMemoryStatus( &ms );

	dwAvailPhys = ms.dwAvailPhys;
	dwTotalPhys = ms.dwTotalPhys;
}

bool CAboutDlg::UpdateMemoryInfo()
{
	CStatic* pCtl = static_cast<CStatic*> (GetDlgItem(IDC_MEMORY));
	ASSERT(pCtl);
	if (!::IsWindow(m_hWnd) || !pCtl)
	{
		return false;
	}

	WCHAR szA[20];
	WCHAR szT[20];
	WCHAR szBuf[64];
	StrFormatByteSize( dwAvailPhys, szA, 20 );
	StrFormatByteSize( dwTotalPhys, szT, 20 );

	wsprintf(szBuf, L"Total:\t%s\r\nAvail:\t%s\r\n", szT, szA);
	pCtl->SetWindowText(szBuf);

	// set the Event information
	CMainFrame* pFrame = static_cast<CMainFrame*> (AfxGetMainWnd());
	CMyDoc* pDoc = static_cast<CMyDoc*> (pFrame->GetActiveDocument());
	ASSERT (pDoc);
	CString str;
	str.Format(L"Info Thread: %s\tCount: %u\r\nCRC Thread: %s\tCount: %u\r\nTerminate: %s",
		g_eGoThreadInfo.Signaled() ? L"running" : L"paused", pDoc->m_dwDirtyInfo,
		g_eGoThreadCRC.Signaled() ? L"running" : L"paused", pDoc->m_dwDirtyCRC,
		g_eTermThreads.Signaled() ? L"yes" : L"no");

	pCtl = static_cast<CStatic*> (GetDlgItem(IDC_EVENTINFO));
	pCtl->SetWindowText(str);

	return true;
}

void CAboutDlg::GrowDialog()
{
	// find out how big the dialog is
	CRect rcWnd;
	GetWindowRect(&rcWnd);

	// IDC_AUTHOR
	CStatic* pCtl1 = static_cast<CStatic*> (GetDlgItem(IDC_AUTHOR));
	ASSERT (pCtl1);
	CRect rc1;
	pCtl1->GetWindowRect(&rc1);
	// set full author string
	CString str;
	str.LoadString(STR_AUTHORFULL);
	pCtl1->SetWindowText(str);

	// IDC_SYSINFO
	CStatic* pCtl2 = static_cast<CStatic*> (GetDlgItem(IDC_SYSINFO));
	ASSERT (pCtl2);
	CRect rc2;
	pCtl2->GetWindowRect(&rc2);

	// IDC_MEMORY
	CStatic* pCtl3 = static_cast<CStatic*> (GetDlgItem(IDC_MEMORY));
	ASSERT (pCtl3);
	CRect rc3;
	pCtl3->GetWindowRect(&rc3);

	// IDC_EVENTINFO
	CStatic* pCtl4 = static_cast<CStatic*> (GetDlgItem(IDC_EVENTINFO));
	ASSERT (pCtl4);
	CRect rc4;
	pCtl4->GetWindowRect(&rc4);

	// IDOK
	CButton* pCtl5 = static_cast<CButton*> (GetDlgItem(IDOK));
	ASSERT (pCtl5);
	CRect rc5;
	pCtl5->GetWindowRect(&rc5);

	// resize the dialog
	rcWnd.bottom =	rc1.bottom
					+ 5
					+ rc2.Height()
					+ 5
					+ rc3.Height()
					+ 5
					+ rc4.Height()
					+ 10
					+ rc5.Height()
					+ m_iBorder;
	
	ScreenToClient(&rc1);
	ScreenToClient(&rc2);
	ScreenToClient(&rc3);
	ScreenToClient(&rc4);
	ScreenToClient(&rc5);

	// move the controls
	int i = rc2.Height();
	rc2.top = rc1.bottom + 5;
	rc2.bottom = rc2.top + i;
	pCtl2->MoveWindow(&rc2, FALSE);
	pCtl2->ShowWindow(SW_SHOWNORMAL);

	i = rc3.Height();
	rc3.top = rc2.bottom + 5;
	rc3.bottom = rc3.top + i;
	pCtl3->MoveWindow(&rc3, FALSE);
	pCtl3->ShowWindow(SW_SHOWNORMAL);

	i = rc4.Height();
	rc4.top = rc3.bottom + 5;
	rc4.bottom = rc4.top + i;
	pCtl4->MoveWindow(&rc4, FALSE);
	pCtl4->ShowWindow(SW_SHOWNORMAL);

	i = rc5.Height();
	rc5.top = rc4.bottom + 10;
	rc5.bottom = rc5.top + i;
	pCtl5->MoveWindow(&rc5, FALSE);
	pCtl5->ShowWindow(SW_SHOWNORMAL);

	m_fBigDialog = true;
	MoveWindow(&rcWnd, TRUE);
	CenterWindow();
}

void CAboutDlg::ShrinkDialog()
{
	// find out how big the dialog is
	CRect rcWnd;
	GetWindowRect(&rcWnd);

	// IDC_AUTHOR
	CStatic* pCtl1 = static_cast<CStatic*> (GetDlgItem(IDC_AUTHOR));
	ASSERT (pCtl1);
	CRect rc1;
	pCtl1->GetWindowRect(&rc1);
	// set author string
	CString str;
	str.LoadString(STR_AUTHOR);
	pCtl1->SetWindowText(str);


	// IDC_SYSINFO
	CStatic* pCtl2 = static_cast<CStatic*> (GetDlgItem(IDC_SYSINFO));
	ASSERT (pCtl2);
	pCtl2->ShowWindow(SW_HIDE);

	// IDC_MEMORY
	CStatic* pCtl3 = static_cast<CStatic*> (GetDlgItem(IDC_MEMORY));
	ASSERT (pCtl3);
	pCtl3->ShowWindow(SW_HIDE);

	// IDC_EVENTINFO
	CStatic* pCtl4 = static_cast<CStatic*> (GetDlgItem(IDC_EVENTINFO));
	ASSERT (pCtl4);
	pCtl4->ShowWindow(SW_HIDE);

	// IDOK
	CButton* pCtl5 = static_cast<CButton*> (GetDlgItem(IDOK));
	ASSERT (pCtl5);
	CRect rc5;
	pCtl5->GetWindowRect(&rc5);

	// resize the dialog
	rcWnd.bottom =	rc1.bottom
					+ 10
					+ rc5.Height()
					+ m_iBorder;

	ScreenToClient(&rc1);
	ScreenToClient(&rc5);

	int i = rc5.Height();
	rc5.top = rc1.bottom + 10;
	rc5.bottom = rc5.top + i;
	pCtl5->MoveWindow(&rc5, FALSE);

	m_fBigDialog = false;
	MoveWindow(&rcWnd, TRUE);
	CenterWindow();
}

bool CAboutDlg::ResizeDialog(bool fBigDialog)
{
	if (fBigDialog)
	{
		GrowDialog();
	}
	else
	{
		ShrinkDialog();
	}
	return m_fBigDialog;
}
