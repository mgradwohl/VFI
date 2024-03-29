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

// Frame.cpp : implementation of the CMainFrame class

#include "stdafx.h"
#include "VFI.h"
#include "doc.h"
#include "Frame.h"
#include "mylistview.h"
#include "helpdlg.h"
#include "globals.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static CHAR THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLTIPS, OnUpdateViewTooltips)
	ON_WM_DROPFILES()
	ON_WM_DESTROY()
	ON_COMMAND(ID_VIEW_TOOLTIPS, OnViewTooltips)
	ON_COMMAND(ID_HELP, OnMyHelp)
	ON_WM_ENDSESSION()
	ON_COMMAND(ID_HELP_ATTRIBS, OnHelpAttribs)
	ON_COMMAND(ID_HELP_FILEFLAGS, OnHelpFlags)
	ON_COMMAND(ID_HELP_FILETYPE, OnHelpType)
	ON_COMMAND(ID_HELP_OS, OnHelpOS)
	ON_COMMAND(ID_HELP_CRC, OnHelpCRC)
	ON_COMMAND(ID_HELP_VERSION, OnHelpVersion)
	ON_COMMAND(ID_HELP_LANGUAGE, OnHelpLanguage)
	ON_COMMAND(ID_HELP_CODEPAGE, OnHelpCodepage)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(ID_PROGRESS, OnUpdateProgress)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_COUNT, OnUpdateCount)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_SIZE, OnUpdateSize)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_BUFFER, OnUpdateBuffer)

END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_PROGRESS,			// Progress Control
	ID_INDICATOR_COUNT,
	ID_INDICATOR_SIZE,
	ID_INDICATOR_BUFFER,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_iCount = -1;
	m_qwSize = (DWORD) -1;
	m_fShowToolTips = true;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// Turn off Redraw
	DWORD dwStyle = ::GetClassLong (m_hWnd, GCL_STYLE);
	::SetClassLong (m_hWnd, GCL_STYLE, dwStyle & ~(CS_HREDRAW | CS_VREDRAW));

	if (!EnableToolTips(true))
		return -1;

	NONCLIENTMETRICS ncm;
	ZeroMemory(&ncm, sizeof(NONCLIENTMETRICS));
	ncm.cbSize = sizeof(NONCLIENTMETRICS);
	SystemParametersInfo( SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);
	m_fntUI.CreateFontIndirect(&ncm.lfMessageFont);

	// Create a Status Bar
	if (!m_wndStatusBar.Create(this))
	{
		TRACE(L"Failed to create status bar\n");
		return -1;
	}

	// set up status bar panes and progress
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
	m_wndStatusBar.SetPaneInfo( 0, m_wndStatusBar.GetItemID( 0 ), SBPS_STRETCH, NULL );
	m_wndStatusBar.SetPaneSize(ID_INDICATOR_COUNT, IDS_PANESIZE);
	m_wndStatusBar.SetPaneSize(ID_INDICATOR_SIZE, IDS_PANESIZE);
	m_wndStatusBar.SetPaneSize(ID_INDICATOR_BUFFER, IDS_PANESIZE);
	m_wndStatusBar.SetPaneSize(ID_PROGRESS, ID_PROGRESS);

	if (!m_wndStatusBar.Setup())
	{
		TRACE(L"Failed to create status bar\n");
		return -1;
	}

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT | TBSTYLE_TRANSPARENT , WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC ) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE(L"Failed to create toolbar\n");
		return -1;      // fail to create
	}

	// higher resolution image for default state
	HBITMAP hbm = (HBITMAP) ::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS | LR_LOADTRANSPARENT);
	CBitmap bm;
	bm.Attach(hbm);
	  
	m_ilToolBar.Create(32,32,ILC_COLOR8,4,4);
	m_ilToolBar.Add(&bm,(CBitmap*)NULL);

	m_wndToolBar.GetToolBarCtrl().SetImageList(&m_ilToolBar);

	// higher resolution image for disabled state
	HBITMAP hbmd = (HBITMAP) ::LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAMED), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS | LR_LOADTRANSPARENT);
	CBitmap bmd;
	bmd.Attach(hbmd);
	  
	m_ilToolBarGrey.Create(32,32,ILC_COLOR8,4,4);
	m_ilToolBarGrey.Add(&bmd,(CBitmap*)NULL);

	m_wndToolBar.GetToolBarCtrl().SetDisabledImageList(&m_ilToolBarGrey);

	// Toolbar title
	CString strToolTitle;

	strToolTitle.LoadString(STR_TITLE_TOOLS);
	m_wndToolBar.SetWindowText(strToolTitle);

	// turn the tips on or off
	m_wndStatusBar.GetTip()->Activate(m_fShowToolTips);
	if (!m_fShowToolTips)
	{
		m_wndToolBar.SetBarStyle( m_wndToolBar.GetBarStyle() & ~CBRS_TOOLTIPS);
	}
	else
	{
		m_wndToolBar.SetBarStyle( m_wndToolBar.GetBarStyle() |  CBRS_TOOLTIPS);
	}

	// allow bars to be docked
	EnableDocking(CBRS_ALIGN_ANY);

	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_TOP);
	DockControlBar(&m_wndToolBar);

	// create a UI update timer
	if ( !::SetTimer(NULL, TIM_UPDATE, TIM_UPDATE_INTERVAL, (TIMERPROC) TimerProc) )
		return -1;

	// set menu default items
	const CMenu* pMenu = GetMenu();
	if (pMenu)
	{
		CMenu* pSubMenu = pMenu->GetSubMenu(0);
		if (pSubMenu) pSubMenu->SetDefaultItem(1, true);

		pSubMenu = pMenu->GetSubMenu(1);
		if (pSubMenu) pSubMenu->SetDefaultItem(0, true);

		pSubMenu = pMenu->GetSubMenu(2);
		if (pSubMenu) pSubMenu->SetDefaultItem(5, true);

		pSubMenu = pMenu->GetSubMenu(3);
		if (pSubMenu) pSubMenu->SetDefaultItem(3, true);
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG

void CMainFrame::OnViewTooltips() 
{
	m_fShowToolTips = !m_fShowToolTips;
	m_wndStatusBar.GetTip()->Activate(m_fShowToolTips);

	if (m_fShowToolTips)
	{
		m_wndToolBar.SetBarStyle( m_wndToolBar.GetBarStyle() |  CBRS_TOOLTIPS);
	}
	else
	{
		m_wndToolBar.SetBarStyle( m_wndToolBar.GetBarStyle() & ~CBRS_TOOLTIPS);
	}
}

void CMainFrame::OnUpdateViewTooltips(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck( m_fShowToolTips ? 1 : 0);
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CFrameWnd::PreCreateWindow(cs))
	{
		return false;
	}

	m_fShowToolTips = theApp.GetProfileInt( L"Preferences", L"ToolTips",true);

	cs.style &= ~FWS_ADDTOTITLE;

	return true;
}

void CMainFrame::OnDropFiles(HDROP hDropInfo) 
{
	const CMyListView* pView = static_cast<CMyListView*> (GetActiveView());
	if (pView == nullptr)
	{
		return;
	}
	pView->SendMessage(WM_DROPFILES, (WPARAM)hDropInfo, 0);
}

void CMainFrame::OnUpdateCount(CCmdUI* pCmdUI)
{
	if (NULL == pCmdUI)
	{
		return;
	}
	
	pCmdUI->Enable(); 

	CMyDoc* pDoc = static_cast<CMyDoc*> (GetActiveDocument());
	if (pDoc == nullptr)
	{
		return;
	}

	if (m_iCount == pDoc->GetItemCount())
	{
		return;
	}
	m_iCount = pDoc->GetItemCount();

	WCHAR szBuf[64];
	int2str(szBuf, m_iCount);							//StrFormatByteSize64(m_qwSize, szBuf, 32 );
	wcscat_s(szBuf, 64, L" files\0");

	m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex( ID_INDICATOR_COUNT ), szBuf, true);
}

void CMainFrame::OnUpdateSize(CCmdUI* pCmdUI)
{
	pCmdUI;
	CMyDoc* pDoc = static_cast<CMyDoc*> (GetActiveDocument());
	if (pDoc == nullptr)
	{
		return;
	}

	m_qwSize = pDoc->GetTotalSize();
	WCHAR szBuf[64];
	int2str(szBuf, m_qwSize);							//StrFormatByteSize64(m_qwSize, szBuf, 32 );
	wcscat_s(szBuf, 64, L" bytes\0");

	m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex( ID_INDICATOR_SIZE ), szBuf, true);
}

void CMainFrame::SavePreferences()
{
	TRACE(L"CMainFrame::SavePreferences\n");

	theApp.WriteProfileInt( L"Preferences", L"ToolTips",m_fShowToolTips);
}

void CMainFrame::OnDestroy() 
{
	::KillTimer(NULL, TIM_UPDATE);
	SavePreferences();

	CFrameWnd::OnDestroy();
}

void CMainFrame::OnUpdateProgress(CCmdUI* pCmdUI)
{
	if (NULL == pCmdUI)
	{
		return;
	}
	pCmdUI->Enable();
	UpdateProgress();
}

void CMainFrame::OnUpdateBuffer(CCmdUI* pCmdUI)
{
	if (NULL == pCmdUI)
	{
		return;
	}
	
	if (NULL == g_pBuf)
		return;

	pCmdUI->Enable(); 

	WCHAR szBuf[32];
	// normally use int2str but here I want something properly formatted like "32 MB"
	StrFormatByteSize(g_dwChunk, szBuf, 32);

	m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex( ID_INDICATOR_BUFFER ), szBuf, true);
}

void CMainFrame::OnMyHelp() 
{
	bool fShowQuickHelp = (1 == theApp.GetProfileInt( L"Preferences", L"ShowQuickHelp",1) );
	CHelpDlg dlgHelp(this, IDR_RTF_HELP, true);
	dlgHelp.m_fShowAgain = fShowQuickHelp;
	dlgHelp.DoModal();
	fShowQuickHelp = dlgHelp.m_fShowAgain;
	theApp.WriteProfileInt( L"Preferences", L"ShowQuickHelp",fShowQuickHelp == TRUE ? 1 : 0);
}

void CMainFrame::UpdateProgress()
{
	CMyDoc* pDoc = static_cast<CMyDoc*> (GetActiveDocument());
	if (pDoc == nullptr)
	{
		return;
	}

	m_wndStatusBar.UpdateProgress(1, pDoc->m_dwDirtyInfo, pDoc->GetItemCount());
	m_qwSize = pDoc->GetTotalSize();
	m_wndStatusBar.UpdateProgress(2, (DWORD((m_qwSize - pDoc->SizeRead())) >> 20), ((DWORD)m_qwSize >> 20));
}

void CALLBACK EXPORT CMainFrame::TimerProc(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime)
{
	dwTime;
	nMsg;
	hWnd;

	if (TIM_UPDATE != nIDEvent)
	{
		return;
	}

	CMainFrame* pFrame = static_cast<CMainFrame*> (AfxGetMainWnd());
	if (NULL == pFrame)
	{
		return;
	}

	CCmdUI cui;
	cui.m_nID = ID_PROGRESS;
	cui.m_nIndex = 0;
	cui.m_pMenu = NULL;
	cui.m_pOther = pFrame->GetStatusBar();
	pFrame->OnUpdateProgress(&cui);

	cui.m_nID = ID_INDICATOR_COUNT;
	cui.m_nIndex++;
	pFrame->OnUpdateCount(&cui);

	cui.m_nID = ID_INDICATOR_SIZE;
	cui.m_nIndex++;
	pFrame->OnUpdateSize(&cui);

	cui.m_nID = ID_INDICATOR_BUFFER;
	cui.m_nIndex++;
	pFrame->OnUpdateBuffer(&cui);
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{
	m_wndStatusBar.GetTip()->RelayEvent(pMsg);
	return CFrameWnd::PreTranslateMessage(pMsg);
}

void CMainFrame::OnEndSession(BOOL bEnding) 
{
	TRACE(L"CMainFrame::OnEndSession\n");
	if (!bEnding)
		return;

	CFrameWnd::OnEndSession(bEnding);
}

void CMainFrame::OnHelpAttribs() 
{
	CHelpDlg dlgHelp(this, IDR_RTF_ATTRIBS, false);
	dlgHelp.DoModal();
}

void CMainFrame::OnHelpFlags() 
{
	CHelpDlg dlgHelp(this, IDR_RTF_FLAGS, false);
	dlgHelp.DoModal();
}

void CMainFrame::OnHelpType() 
{
	CHelpDlg dlgHelp(this, IDR_RTF_TYPE, false);
	dlgHelp.DoModal();
}

void CMainFrame::OnHelpOS() 
{
	CHelpDlg dlgHelp(this, IDR_RTF_OS, false);
	dlgHelp.DoModal();
}

void CMainFrame::OnHelpCRC() 
{
	CHelpDlg dlgHelp(this, IDR_RTF_CRC, false);
	dlgHelp.DoModal();
}

void CMainFrame::OnHelpVersion() 
{
	CHelpDlg dlgHelp(this, IDR_RTF_VERSION, false);
	dlgHelp.DoModal();
}

void CMainFrame::OnHelpLanguage() 
{
	CHelpDlg dlgHelp(this, IDR_RTF_LANGUAGE, false);
	dlgHelp.DoModal();
}

void CMainFrame::OnHelpCodepage() 
{
	CHelpDlg dlgHelp(this, IDR_RTF_CODEPAGE, false);
	dlgHelp.DoModal();
}

void CMainFrame::OnClose() 
{
	CFrameWnd::OnClose();
}
