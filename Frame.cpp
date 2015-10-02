// Frame.cpp : implementation of the CMainFrame class
//

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

#define TIM_UPDATE			101
#define TIM_UPDATE_INTERVAL	500

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
	ON_COMMAND(ID_HELP_ISO, OnHelpISO)
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
	m_fShowToolTips = TRUE;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// turn off all the redraw shit
    DWORD dwStyle = ::GetClassLong (m_hWnd, GCL_STYLE);
    ::SetClassLong (m_hWnd, GCL_STYLE, dwStyle & ~(CS_HREDRAW | CS_VREDRAW));

	if (!EnableToolTips(TRUE))
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

	//if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
	//	| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
	//	!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	//{
	//	TRACE(L"Failed to create toolbar\n");
	//	return -1;      // fail to create
	//}

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
	CMenu* pMenu = GetMenu();
	if (pMenu)
	{
		CMenu* pSubMenu = pMenu->GetSubMenu(0);
		if (pSubMenu) pSubMenu->SetDefaultItem(1, TRUE);

		pSubMenu = pMenu->GetSubMenu(1);
		if (pSubMenu) pSubMenu->SetDefaultItem(0, TRUE);

		pSubMenu = pMenu->GetSubMenu(2);
		if (pSubMenu) pSubMenu->SetDefaultItem(5, TRUE);

		pSubMenu = pMenu->GetSubMenu(3);
		if (pSubMenu) pSubMenu->SetDefaultItem(3, TRUE);
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
		return FALSE;
	}

	m_fShowToolTips = theApp.GetProfileInt( L"Preferences", L"ToolTips",TRUE);

	cs.style &= ~FWS_ADDTOTITLE;

	return TRUE;
}

void CMainFrame::OnDropFiles(HDROP hDropInfo) 
{
	CMyListView* pView = static_cast<CMyListView*> (GetActiveView());
	ASSERT( pView );
	pView->SendMessage(WM_DROPFILES, (WPARAM) hDropInfo, 0);
}

void CMainFrame::OnUpdateCount(CCmdUI* pCmdUI)
{
	if (NULL == pCmdUI)
	{
		return;
	}
	
    pCmdUI->Enable(); 

	CMyDoc* pDoc = static_cast<CMyDoc*> (GetActiveDocument());
	ASSERT (pDoc);

	if (m_iCount == pDoc->GetItemCount())
	{
		return;
	}
	m_iCount = pDoc->GetItemCount();

	CString strCount;
    strCount.Format( STR_FILECOUNT, m_iCount ); 

	m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex( ID_INDICATOR_COUNT ), strCount, TRUE);
}

void CMainFrame::OnUpdateSize(CCmdUI* pCmdUI)
{
	pCmdUI;
	CMyDoc* pDoc = static_cast<CMyDoc*> (GetActiveDocument());
	ASSERT (pDoc);

	m_qwSize = pDoc->GetTotalSize();
	WCHAR szBuf[32];
	StrFormatByteSize64(m_qwSize, szBuf, 32 );

	m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex( ID_INDICATOR_SIZE ), szBuf, TRUE);
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
	if (NULL == g_pBuf)
	{
		StrFormatByteSize(0, szBuf, 32);
	}
	else
	{
		StrFormatByteSize(g_dwChunk, szBuf, 32);
	}
	m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex( ID_INDICATOR_BUFFER ), szBuf, TRUE);
}

void CMainFrame::OnMyHelp() 
{
	BOOL fShowQuickHelp = (1 == theApp.GetProfileInt( L"Preferences", L"ShowQuickHelp",1) );
	CHelpDlg dlgHelp(this, IDR_RTF_HELP, true);
	dlgHelp.m_fShowAgain = fShowQuickHelp;
	dlgHelp.DoModal();
	fShowQuickHelp = dlgHelp.m_fShowAgain;
	theApp.WriteProfileInt( L"Preferences", L"ShowQuickHelp",fShowQuickHelp == TRUE ? 1 : 0);
}

void CMainFrame::UpdateProgress()
{
	CMyDoc* pDoc = static_cast<CMyDoc*> (GetActiveDocument());
	ASSERT (pDoc);
	m_wndStatusBar.UpdateProgress(1, pDoc->m_dwDirtyInfo, pDoc->GetItemCount());
	m_qwSize = pDoc->GetTotalSize();
	m_wndStatusBar.UpdateProgress(2, (DWORD( (m_qwSize - pDoc->SizeRead()) ) >> 20), ((DWORD)m_qwSize >> 20) );
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

	CMyDoc* pDoc = static_cast<CMyDoc*> (pFrame->GetActiveDocument());
	ASSERT (pDoc);
	
	if (pDoc->IsDirty())
	//	if ((pDoc->m_dwDirtyInfo>0) || (pDoc->m_dwDirtyCRC > 0)|| (pDoc->GetDirtyCount() > 0))
	{
		pDoc->ResumeAllThreads();
	}
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

	CMyDoc* pDoc = static_cast<CMyDoc*> (GetActiveDocument());
	ASSERT (pDoc);
	pDoc->TerminateThreads();

	CFrameWnd::OnEndSession(bEnding);
}

void CMainFrame::OnHelpISO() 
{
	CHelpDlg dlgHelp(this, IDR_RTF_ISO, false);
	dlgHelp.DoModal();
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
