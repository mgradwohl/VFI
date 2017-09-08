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

// mylistview.cpp : implementation of the CMyListView class
//

#include "stdafx.h"
#include "filelib.h"
#include "wndlib.h"
#include "globals.h"

#include "resource.h"
#include "VFI.h"
#include "frame.h"
#include "WiseFile.h"
#include "Doc.h"
#include "mylistview.h"
#include "MyPropSheet.h"
#include "timedlg.h"
#include "progressbox.h"
#include <mmsystem.h>

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static CHAR THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyListView

IMPLEMENT_DYNCREATE(CMyListView, CListView)

BEGIN_MESSAGE_MAP(CMyListView, CListView)
	ON_WM_CONTEXTMENU()
	//{{AFX_MSG_MAP(CMyListView)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetDispInfo)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_WM_DROPFILES()
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVEAS, OnUpdateFileSaveAs)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
	ON_COMMAND(ID_FILE_RECURSEDIR, OnAddButtload)
	ON_COMMAND(ID_VIEW_OPTIONS, OnViewOptions)
	ON_UPDATE_COMMAND_UI(ID_FILE_NEW, OnUpdateFileNew)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_FILE_RENAME, OnFileRename)
	ON_UPDATE_COMMAND_UI(ID_FILE_RENAME, OnUpdateFileRename)
	ON_COMMAND(ID_FILE_TOUCH, OnFileTouch)
	ON_UPDATE_COMMAND_UI(ID_FILE_TOUCH, OnUpdateFileTouch)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
	ON_COMMAND(ID_FILE_ADD, OnFileAdd)
	ON_WM_DESTROY()
	ON_COMMAND(ID_FILE_PROPERTIES, OnFileProperties)
	ON_UPDATE_COMMAND_UI(ID_FILE_PROPERTIES, OnUpdateFileProperties)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_VIEW_SMARTFIT, OnViewSmartFit)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SMARTFIT, OnUpdateViewSmartFit)
	ON_COMMAND(ID_EDIT_REMOVE, OnEditRemove)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REMOVE, OnUpdateEditRemove)
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyListView construction/destruction

CMyListView::CMyListView()
{
	TRACE(L"CMyListView::CMyListView()\n");

	m_hBuf = NULL;
	m_pBuf = NULL;
	m_fLockHeaderWidth = false;
	m_fAutomatic = false;
	m_fFirstTime = true;
	m_fSavePrompt = true;
	m_fIncludePath = true;
	m_hSmallIcon = NULL;
	m_hLargeIcon = NULL;
	m_iLastItem = 0;
	m_iSortColumn = -1;
	m_fSortAscend = true;
	m_fCrashOnPause = false;
	m_dwExStyle = LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP;
}

CMyListView::~CMyListView()
{
	TRACE(L"CMyListView::~CMyListView()\n");
	delete [] m_pci;
	CloseBuffer();
}

/////////////////////////////////////////////////////////////////////////////
// CMyListView diagnostics

#ifdef _DEBUG
void CMyListView::AssertValid() const
{
	// BUGBUG why does this bust?
	CListView::AssertValid();
}

void CMyListView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CMyDoc* CMyListView::GetDocument()
{
	return static_cast<CMyDoc*> (m_pDocument);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMyListView message handlers

BOOL CMyListView::PreCreateWindow(CREATESTRUCT& cs)
{
	TRACE(L"CMyListView::PreCreateWindow()\n");
	if (!CListView::PreCreateWindow(cs))
	{
		return FALSE;
	}

	if (!RestorePreferences())
	{
		return FALSE;
	}

	cs.style |= LVS_SHOWSELALWAYS | LVS_REPORT | LVS_AUTOARRANGE;
	cs.dwExStyle |= LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT;
	cs.dwExStyle &= ~WS_EX_DLGMODALFRAME;
	return TRUE;
}

void CMyListView::OnInitialUpdate() 
{
	TRACE(L"CMyListView::OnInitialUpdate()\n");

	CListView::OnInitialUpdate();
	CMyDoc* pDoc=GetDocument();
	ASSERT_VALID( pDoc );

	if (m_fFirstTime)
	{
		// figure out appropriate font sizes
		// make the header bold
		CDC* pdc = GetDC();
		CListCtrl& theListCtrl=GetListCtrl();
		CHeaderCtrl* pHeader=theListCtrl.GetHeaderCtrl();
		if (!pHeader)
		{
			TRACE0( "InitialUpdate could not get Header\n.");
			return;
		}

		LOGFONT lf;
		::ZeroMemory(&lf, sizeof(LOGFONT));
		theApp.GetUIFont()->GetLogFont(&lf);

		LONG lfWeight = lf.lfWeight;
		lf.lfWeight = FW_MEDIUM;
		m_fntHeader.CreateFontIndirect(&lf);

		lf.lfWeight = lfWeight;
		m_fntView.CreateFontIndirect(&lf);

		lstrcpy(lf.lfFaceName, L"Courier New");
		m_fntMono.CreateFontIndirect(&lf);

		theListCtrl.SetFont(&m_fntView);
		pHeader->SetFont(&m_fntHeader);

		// measure the monospaced X
		pdc->SelectObject(m_fntMono);
		CSize size = pdc->GetTextExtent(L"X");
		ReleaseDC(pdc);
		m_nMonoX = size.cx;
		m_nMonoY = size.cy;
		
		// create the image list for sorting
		m_imlView.Create(16, 16, ILC_MASK, 2, 2);
		VERIFY(-1 != m_imlView.Add(theApp.LoadIcon(IDI_ASCEND)));
		VERIFY(-1 != m_imlView.Add(theApp.LoadIcon(IDI_DESCEND)));
		pHeader->SetImageList(&m_imlView);

		InitColumns();
		UpdateWidths();
		InitCRCMemory();

		// If we can't get a minimum size (see InitCRCMemory)
		if ((NULL == g_pBuf) || (g_dwChunk < 1 ))
		{
			CString strError;
#pragma warning(suppress: 6031)
			strError.LoadString(ERR_NOMEMORY);
			CString strTitle;
#pragma warning(suppress: 6031)
			strTitle.LoadString(ERR_TITLE);
			ErrorMessageBox(AfxGetMainWnd()->GetSafeHwnd(), GetLastError(), strTitle, strError);

			FreeCRCMemory();
		}

		m_fFirstTime = false;
		DragAcceptFiles();

		MyModifyStyleEx(0,m_dwExStyle );
		g_eGoThreadCRC.Signal( true );

		// Adds ability to use UNC paths and specify output file
		// Does NO directory or file checking though... 
		{
			LPWSTR *argv;
			int argc = 0;
			argv = CommandLineToArgvW(::GetCommandLine(),&argc);
			
			wmemset(m_szFolder,0,_MAX_PATH);
			wmemset(m_szCmdLineOutput,0,_MAX_PATH);

			// if we have the 3rd argument - path to save...
			if (argc == 3)
			{
				lstrcpy(m_szFolder,argv[1]); // copy the folder
				lstrcpy(m_szCmdLineOutput,argv[2]); // copy the path to the output file.
			}
			else if (argc == 2)	// if only 2 then use matt's old code
			{
				if (!GetCommandLineFolder(m_szFolder))
				{
					ShowCmdHelp();
				}
			}

			// clean up the memory from the CommandLineToArgvW() call
			GlobalFree(argv);
		}

		if (lstrcch(m_szFolder) > 0)
		{
			m_fAutomatic = true;
			pDoc->RecurseDir( m_szFolder );
		}
	}
}

bool CMyListView::AddItem(CWiseFile* pFileInfo)
{
	int iItem;
	int iSubItem=1;
	LV_ITEM lvi;

	CListCtrl& theListCtrl=GetListCtrl();
	iItem=theListCtrl.GetItemCount();
	TRACE1( ">>> CMyListView::AddItem() iItem==%d\r\n",iItem);

	ZeroMemory(&lvi, sizeof(lvi));
	lvi.iItem = iItem;
	lvi.iSubItem = 0;
	lvi.mask = LVIF_TEXT  | LVIF_PARAM | LVIF_DI_SETITEM;
	lvi.pszText = LPSTR_TEXTCALLBACK;
	lvi.cchTextMax = 1024;
	lvi.lParam = (LPARAM) pFileInfo;

	iItem=theListCtrl.InsertItem(&lvi);

	if ( -1 == iItem )
	{
		//adding failed
		CString strError = pFileInfo->GetName();
		CString strExt = pFileInfo->GetExt();
		if (! strExt.IsEmpty() )
		{
			strError += L".";
			strError += strExt;
		}

		CString strTitle;
#pragma warning(suppress: 6031)
		strTitle.LoadString(ERR_TITLE);
		strError.Format(ERR_ADDFILE_FAILED, (LPCWSTR)strError, GetLastError() );

		ErrorMessageBox(AfxGetMainWnd()->GetSafeHwnd(), GetLastError(), strTitle, strError);

		CMyDoc* pDoc=GetDocument();
		ASSERT (pDoc);
		VERIFY (FALSE!=pDoc->DeleteFile( pFileInfo ));
	}
	else
	{
		for (iSubItem=1; iSubItem < LIST_NUMCOLUMNS; iSubItem++)
		{
			theListCtrl.SetItemText(iItem, iSubItem, LPSTR_TEXTCALLBACK);
		}		
	}

	return TRUE;
}

void CMyListView::OnAddButtload() 
{
	TRACE(L"CMyListView::OnAddButtload()\n");

	WCHAR szFolder[_MAX_PATH];
	if (BrowseForFolder(m_hWnd, L"Choose a folder, all files in the folder will be added to the list.", szFolder))
	{
		CMyDoc* pDoc=GetDocument();
		ASSERT_VALID( pDoc );
		pDoc->RecurseDir( szFolder );
	}
}

void CMyListView::OnDropFiles(HDROP hDropInfo) 
{
	TRACE(L"CMyListView::OnDropFiles()\n");

	// redraw frame
	CMainFrame* pFrame = static_cast<CMainFrame*> (GetParentFrame());
	if (pFrame)
	{
		pFrame->SetForegroundWindow();
	}

	// get the document
	CMyDoc* pDoc=GetDocument();
	ASSERT_VALID( pDoc );
	
	// suspend processing
	pDoc->PauseAllThreads(true);

	// create the progess box
	CString strText;

	CProgressBox Box;
	Box.Create(this, MWX_APP | MWX_CENTER);

#pragma warning(suppress: 6031)
	strText.LoadString(STR_FILECOUNTING);
	Box.SetWindowText(strText);

	Box.m_ctlProgress.SetPos(0);
	Box.m_ctlProgress.SetStep(1);
	Box.ShowWindow(SW_SHOWNORMAL);
	strText.Empty();

	CStringList PathList;
	CStringList FileList;
	CString strSearch;
	CFileFind m_Find;
	bool fRun = false;
	UINT nFiles=0;
	UINT i=0;
	WCHAR szFileName[_MAX_PATH];
	ZeroMemory(szFileName, 4);
	WCHAR szBuf[256];

	// determine how many files we have
	nFiles = ::DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, 0);
	wsprintf(szBuf, L"nFiles %lu\r\n", nFiles);
	OutputDebugString(szBuf);
	
	// determine where they were dropped from, and remember it
	::DragQueryFileW( hDropInfo, 0, szFileName, _MAX_PATH);
	
	wsprintf(szBuf, L"nFiles %lu\r\n", nFiles);
	OutputDebugString(szBuf);
	
	WCHAR szDropFolder[_MAX_PATH];
	ZeroMemory(szDropFolder, 4);

	lstrcpy(szDropFolder, szFileName);

	// just add them to a list first
	while ( i < nFiles )
	{
		::DragQueryFile( hDropInfo, i, szFileName, _MAX_PATH);
		fRun = (TRUE == m_Find.FindFile(szFileName,0));
		while ( fRun )
		{
			fRun = (TRUE == m_Find.FindNextFile()); 
			// if it's a directory, and it's not dots then add it to the path list
			if ( m_Find.IsDirectory() && !m_Find.IsDots() )
			{
				PathList.AddTail( m_Find.GetFilePath() );
			}
			// it it's not a directory (it's a file) add it to the file list
			if ( !m_Find.IsDirectory() )
			{
				FileList.AddTail( m_Find.GetFilePath() );
			}
			theApp.ForwardMessages();
			if (g_eTermThreads.Signaled())
			{
				TRACE(L"OnDropFiles aborting\r\n");
				fRun = false;
			}
		}
		i++;
	}
	::DragFinish( hDropInfo );
	m_Find.Close();	

	// now add all files on the path list to the file list
	while ( !PathList.IsEmpty() )
	{
		strSearch=PathList.RemoveHead();
		if (strSearch[strSearch.GetLength()-1] != '\\')
		{
			strSearch += '\\';
		}
		strSearch += "*.*";
		fRun = (TRUE==m_Find.FindFile(strSearch,0));
		while ( fRun )
		{
			fRun = (TRUE==m_Find.FindNextFile());
			// if it's a directory, and it's not dots then add it to the path list
			if ( m_Find.IsDirectory() && !m_Find.IsDots() )
			{
				PathList.AddTail( m_Find.GetFilePath() );
			}
			// it it's not a directory (it's a file) add it to the file list
			if ( !m_Find.IsDirectory() )
			{
				FileList.AddTail( m_Find.GetFilePath() );
			}
			theApp.ForwardMessages();
			if (g_eTermThreads.Signaled())
			{
				TRACE(L"OnDropFiles aborting\r\n");
				fRun = false;
			}
		}
	}
	m_Find.Close();	

	// now we know how many files there are, so we add them
	int count = Clamp(FileList.GetCount());
	strText.FormatMessage(STR_FILEADD, count);
	Box.SetWindowText(strText);
	Box.m_ctlProgress.SetRange32(0, count);

	CString strFile;
	while ( !FileList.IsEmpty())
	{
		Box.m_ctlProgress.StepIt();
		strFile = FileList.RemoveHead();
		if (FALSE == pDoc->AddFile(strFile))
		{
			strText.FormatMessage(ERR_FILEINUSE, (LPCWSTR)strFile);
			UpdateStatus(strText);
		}

		theApp.ForwardMessages();
		if (g_eTermThreads.Signaled())
		{
			TRACE(L"OnDropFiles aborting\r\n");
			FileList.RemoveAll();
		}
	}
	Box.DestroyWindow();

	pDoc->SetPathName(szDropFolder);
	// resume processing
	pDoc->PauseAllThreads(false);
}

void CMyListView::OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = reinterpret_cast<LV_DISPINFO*> (pNMHDR);
	CWiseFile* pInfo = reinterpret_cast<CWiseFile*> (pDispInfo->item.lParam);

	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		pInfo->GetFieldString((LPWSTR)m_szItemText, pDispInfo->item.iSubItem, m_fIncludePath);
		pDispInfo->item.pszText = (LPWSTR) m_szItemText;
	}
	*pResult = 0;
}

void CMyListView::OnUpdateFileNew(CCmdUI* pCmdUI) 
{
	CMyDoc* pDoc=GetDocument();
	ASSERT_VALID( pDoc );

	pCmdUI->Enable( TRUE );
}

void CMyListView::OnUpdateFileTouch(CCmdUI* pCmdUI) 
{
	CListCtrl& theListCtrl=GetListCtrl();
	pCmdUI->Enable( theListCtrl.GetSelectedCount() > 0 );
}

void CMyListView::OnUpdateEditSelectAll(CCmdUI* pCmdUI) 
{
	CListCtrl& theListCtrl=GetListCtrl();
	pCmdUI->Enable( theListCtrl.GetItemCount() > 0 );
}

void CMyListView::OnUpdateFileProperties(CCmdUI* pCmdUI) 
{
	CListCtrl& theListCtrl=GetListCtrl();
	pCmdUI->Enable( theListCtrl.GetSelectedCount() == 1 );
}

void CMyListView::OnUpdateEditRemove(CCmdUI* pCmdUI) 
{
	CListCtrl& theListCtrl=GetListCtrl();
	pCmdUI->Enable( theListCtrl.GetSelectedCount() >0 );
}

void CMyListView::OnUpdateFileRename(CCmdUI* pCmdUI) 
{
	pCmdUI;
}

void CMyListView::OnUpdateFileSaveAs(CCmdUI* pCmdUI) 
{
	CMyDoc* pDoc=GetDocument();
	ASSERT_VALID( pDoc );

	pCmdUI->Enable( !pDoc->IsEmpty() );	
}

void CMyListView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	TRACE(L">>> CMyListView::OnUpdate Hint: %d\n",lHint);

	if (! ::IsWindow(m_hWnd))
		return;

	CMyDoc* pDoc = GetDocument();
	ASSERT_VALID( pDoc );
	pDoc->ResumeAllThreads();

	switch (lHint)
	{
		case HINT_EMPTY:	EmptyList();
							m_fAutomatic = false;
							break;
		case HINT_ADD:		ASSERT( NULL != pHint);
							AddItem( static_cast<CWiseFile*> (pHint));
							break;
		case HINT_DELETE:	ASSERT( NULL != pHint);
							m_fAutomatic = false;
							DeleteItem( static_cast<CWiseFile*> (pHint));
							break;
		case HINT_REFRESH:	ASSERT( NULL != pHint);
							RedrawItem( static_cast<CWiseFile*> (pHint));
							break;
		case HINT_DONEINFO:
		case HINT_DONECRC:
			{
				if (m_fAudioCue)
				{
					// try to use what the user likes
					if (!m_strWave.IsEmpty() && DoesFileExist(m_strWave))
					{
						PlaySound( m_strWave, NULL, SND_FILENAME | SND_ASYNC);
					}
					else
					{
						PlaySound( L"Windows Explorer\\Complete Navigation", NULL, SND_ALIAS | SND_ASYNC);
					}
				}

				if (m_fAutomatic && pDoc->GetDirtyCount() == 0)
				{			
					// SJ: if the user specified an output filename and path, then use it!
					if (m_szCmdLineOutput[0] == '\0')
					{
						// use the path generated
						WCHAR szPath[_MAX_PATH];
						pDoc->GetFileName(szPath);
						pDoc->WriteFileEx(szPath);
					}
					else
					{
						// use the path passed in
						pDoc->WriteFileEx(m_szCmdLineOutput);
					}
				
					PostQuitMessage(0);
				}
			}
	}

	pSender;
}

void CMyListView::OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TRACE(L">>> CMyListView::OnColumnClick\n");

	NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*> (pNMHDR);

	if (m_iSortColumn == pNMListView->iSubItem)
	{
		// clicked same column
		// change sort type and re mark column
		m_fSortAscend = !m_fSortAscend;
		MarkColumn();
	}
	else
	{
		// clicked different column
		// un mark old column
		UnMarkColumn();
		// set the new sorted column and sort type
		m_iSortColumn = pNMListView->iSubItem;
		m_fSortAscend = true;
		// mark the new column
		MarkColumn();
	}

	// do the sort
	CListCtrl& theListCtrl = GetListCtrl();
	theListCtrl.SortItems( ListViewCompareProc, (LPARAM) this);

	// ensure that the selected item is visible
	// only ensures that the first selected item is visible
	theListCtrl.EnsureVisible(theListCtrl.GetNextItem( -1, LVNI_SELECTED), TRUE);

	*pResult = 0;
}

int CALLBACK CMyListView::ListViewCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamView)
{
	CWiseFile* pFile1 = reinterpret_cast<CWiseFile*> (lParam1);
	CWiseFile* pFile2 = reinterpret_cast<CWiseFile*> (lParam2);
	WCHAR str1[_MAX_PATH];
	WCHAR str2[_MAX_PATH];
	int iResult = 0;
	CMyListView* pView = reinterpret_cast<CMyListView*> (lParamView);
	if (pFile1 && pFile2)
	{
		switch( pView->m_iSortColumn)
		{
			case 0:    // sort by Path
				{
					TRACE(L">>> Sorting by Path\n");
					pFile1->GetPath( str1 );
					pFile2->GetPath( str2 );

					iResult = StringCompare(str1, str2, pView->m_fSortAscend);
					break;
				}
			case 1:     // sort by Name
				{
					TRACE(L">>> Sorting by Name\n");
					pFile1->GetName( str1 );
					pFile2->GetName( str2 );
					iResult = StringCompare(str1, str2, pView->m_fSortAscend);
					break;
				}

			case 2:     // sort by Ext
				{
					pFile1->GetExt( str1 );
					pFile2->GetExt( str2 );
					iResult = StringCompare(str1, str2, pView->m_fSortAscend);
					break;
				}
			case 3:     // sort by Size
				{
					iResult = QWordCompare(pFile1->GetSize64(), pFile2->GetSize64(), pView->m_fSortAscend);
					break;
				}
			case 4:     // sort by Date
				{
					CTime time1( *pFile1->GetDateCreation() );
					CTime time2( *pFile2->GetDateCreation() );
					iResult = TimeCompare(time1, time2, pView->m_fSortAscend);
					break;
				}
			case 5:     // sort by Time
				{
					CTime time1( *pFile1->GetTimeCreation() );
					CTime time2( *pFile2->GetTimeCreation() );
					iResult = TimeCompare(time1, time2, pView->m_fSortAscend);
					break;
				}
			case 6:     // sort by Date
				{
					CTime time1(*pFile1->GetDateLastWrite());
					CTime time2(*pFile2->GetDateLastWrite());
					iResult = TimeCompare(time1, time2, pView->m_fSortAscend);
					break;
				}
			case 7:     // sort by Time
				{
					CTime time1(*pFile1->GetTimeLastWrite());
					CTime time2(*pFile2->GetTimeLastWrite());
					iResult = TimeCompare(time1, time2, pView->m_fSortAscend);
					break;
				}
			case 8:     // sort by Date
				{
					CTime time1(*pFile1->GetDateLastAccess());
					CTime time2(*pFile2->GetDateLastAccess());
					iResult = TimeCompare(time1, time2, pView->m_fSortAscend);
					break;
				}
			case 9:     // sort by Time
				{
					CTime time1(*pFile1->GetTimeLastAccess());
					CTime time2(*pFile2->GetTimeLastAccess());
					iResult = TimeCompare(time1, time2, pView->m_fSortAscend);
					break;
				}
			case 10:		// sort by Attribs
				{
					iResult = DWordCompare(pFile1->GetAttribs(), pFile2->GetAttribs(), pView->m_fSortAscend);
					break;
				}

			case 11:     // sort by File Version
				{
					iResult = QWordCompare(pFile1->GetFileVersion(), pFile2->GetFileVersion(), pView->m_fSortAscend);
					break;
				}

			case 12:     // sort by Product Version
				{
					iResult = QWordCompare(pFile1->GetProductVersion(), pFile2->GetProductVersion(), pView->m_fSortAscend);
					break;
				}

			case 13:     // sort by Language
				{
					iResult = WordCompare(pFile1->GetLanguage(), pFile2->GetLanguage(), pView->m_fSortAscend);
					break;

				}
			case 14:     // sort by Code Page
				{
					iResult = WordCompare(pFile1->GetCodePage().Get(), pFile2->GetCodePage().Get(), pView->m_fSortAscend);
					break;
				}
			case 15:   // sort by OS
				{
					iResult = DWordCompare(pFile1->GetOS(), pFile2->GetOS(), pView->m_fSortAscend);
					break;
				}
			case 16:   // sort by Type
				{
					iResult = DWordCompare(pFile1->GetType(), pFile2->GetType(), pView->m_fSortAscend);
					break;
				}
			case 17:   // sort by Flags
				{
					iResult = DWordCompare(pFile1->GetFlags(), pFile2->GetFlags(), pView->m_fSortAscend);
					break;
				}
			case 18:   // sort by CRC
				{
					iResult = DWordCompare(pFile1->GetCRC(), pFile2->GetCRC(), pView->m_fSortAscend);
					break;
				}
			default:
				iResult = 0;
				break;
		}
	}
	return(iResult);
}

void CMyListView::OnContextMenu(CWnd*, CPoint point)
{
	CMenu menu;
	VERIFY(menu.LoadMenu(CG_POPUP));
	
	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);

	CWnd* pWndPopupOwner = this;
	while (pWndPopupOwner->GetStyle() & WS_CHILD)
	{
		pWndPopupOwner = pWndPopupOwner->GetParent();
	}

	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		point.x, point.y,pWndPopupOwner);
}

BOOL CMyListView::PreTranslateMessage(MSG* pMsg)
{
	{
		// Shift+F10: show pop-up menu.
		if ((((pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN) && // If we hit a key and
			(pMsg->wParam == VK_F10) && (GetKeyState(VK_SHIFT) & ~1)) != 0) ||	// it's Shift+F10 OR
			(pMsg->message == WM_CONTEXTMENU))									// Natural keyboard key
		{
			CRect rect;
			GetClientRect(rect);
			ClientToScreen(rect);

			CPoint point = rect.TopLeft();
			point.Offset(5, 5);
			OnContextMenu(NULL, point);

			return TRUE;
		}
	}
	return CListView::PreTranslateMessage(pMsg);
}

bool CMyListView::RestorePreferences()
{
	TRACE(L"CMyListView::RestorePreferences()\n");

	CString strEntry;
	CString strValue;

	// BUGBUG this stuff can fail
	m_pci = new CColumnInfo[LIST_NUMCOLUMNS];
	if (NULL == m_pci)
	{
		return false;
	}
	
	// Some are left aligned, some are right aligned
	m_pci[0].SetFormat(LVCFMT_LEFT);
	m_pci[1].SetFormat(LVCFMT_LEFT);
	m_pci[2].SetFormat(LVCFMT_LEFT);
	m_pci[3].SetFormat(LVCFMT_RIGHT);
	m_pci[4].SetFormat(LVCFMT_RIGHT);
	m_pci[5].SetFormat(LVCFMT_RIGHT);
	m_pci[6].SetFormat(LVCFMT_RIGHT);
	m_pci[7].SetFormat(LVCFMT_RIGHT);
	m_pci[8].SetFormat(LVCFMT_RIGHT);
	m_pci[9].SetFormat(LVCFMT_RIGHT);
	m_pci[10].SetFormat(LVCFMT_LEFT);
	m_pci[11].SetFormat(LVCFMT_RIGHT);
	m_pci[12].SetFormat(LVCFMT_RIGHT);
	m_pci[13].SetFormat(LVCFMT_LEFT);
	m_pci[14].SetFormat(LVCFMT_LEFT);
	m_pci[15].SetFormat(LVCFMT_LEFT);
	m_pci[16].SetFormat(LVCFMT_LEFT);
	m_pci[17].SetFormat(LVCFMT_LEFT);
	m_pci[18].SetFormat(LVCFMT_LEFT);
	//m_pci[19].SetFormat(LVCFMT_LEFT);

	// some are fixed, some aren't
	m_pci[4].SetFixedWidth(12);		// '_MM/DD/YYYY_'
	m_pci[5].SetFixedWidth(10);		// '_00:00_XM_'
	m_pci[6].SetFixedWidth(12);		// '_MM/DD/YYYY_'
	m_pci[7].SetFixedWidth(10);		// '_00:00_XM_'
	m_pci[8].SetFixedWidth(12);		// '_MM/DD/YYYY_'
	m_pci[9].SetFixedWidth(10);		// '_00:00_XM_'
	m_pci[4].SetFixedWidth(12);		// '_MM/DD/YYYY_'
	m_pci[10].SetFixedWidth(10);	// // attribs
	m_pci[11].SetFixedWidth(19);	// _xx.xxxx.xxxx.xxxx_ file version
	m_pci[12].SetFixedWidth(19);	// _xx.xxxx.xxxx.xxxx_ product version
	m_pci[18].SetFixedWidth(10);	// CRC

	unsigned int t1,t2;
	for (int i=0; i < LIST_NUMCOLUMNS; i++)
	{
		m_pci[i].SetLabelID(STR_COLUMN0 + i);

		strEntry.Format( L"%d",i);
		strValue = theApp.GetProfileString(L"Columns",strEntry, L"50,1");
		swscanf_s(strValue, L"%u, %u", &t1, &t2);
		m_pci[i].SetWidth(t1);
		m_pci[i].SetVisible(t2==1);
	}

	m_fHexLanguage = (1 == theApp.GetProfileInt( L"Preferences", L"Language",0) );
	m_fLocalTime = (1 == theApp.GetProfileInt( L"Preferences", L"Time",1) );
	m_fHexSize = (1 == theApp.GetProfileInt( L"Preferences", L"Size",0) );
	m_fAllowFolders = (1 == theApp.GetProfileInt( L"Preferences", L"Folders",1) );
	m_fSavePrompt = (1 == theApp.GetProfileInt( L"Preferences", L"PromptToSave",1) );
	m_fIncludePath = (1 == theApp.GetProfileInt( L"Preferences", L"IncludePath",1) );
	m_fAutoFit = (1 == theApp.GetProfileInt( L"Preferences", L"AutoFit",1) );
	m_fAudioCue = (1 == theApp.GetProfileInt( L"Preferences", L"AudioCue",1) );
	m_strWave = theApp.GetProfileString( L"Preferences", L"Wave", L"");
	m_strSavePath = theApp.GetProfileString( L"Preferences", L"SavePath", L"");
	m_fCrashOnPause = (1 == theApp.GetProfileInt( L"Preferences", L"CrashOnPause",0) );

	if (!DoesFileExist(m_strWave) && !m_strWave.IsEmpty())
	{
		CString strError;
		CString strTitle;
#pragma warning(suppress: 6031)
		strTitle.LoadString(ERR_TITLE);
		strError.FormatMessage(ERR_WAVENOTFOUND, (LPCWSTR)m_strWave);
		ErrorMessageBox(AfxGetMainWnd()->GetSafeHwnd(), GetLastError(), strTitle, strError);

		m_strWave.Empty();
	}
	return true;
}

bool CMyListView::SavePreferences()
{
	TRACE(L"CMyListView::SavePreferences()\n");
	CListCtrl& theListCtrl=GetListCtrl();
	CString strEntry;
	CString strValue;

	if (NULL == m_pci)
	{
		return FALSE;
	}

	for (int i=0; i < LIST_NUMCOLUMNS; i++)
	{
		strEntry.Format( L"%d",i);
		if (!m_pci[i].IsVisible())
		{
			strValue.Format( L"%d, %d",m_pci[i].GetWidth(), m_pci[i].IsVisible() == TRUE ? 1 : 0 );
		}
		else
		{
			strValue.Format( L"%d, %d",theListCtrl.GetColumnWidth(i), m_pci[i].IsVisible() == TRUE ? 1 : 0 );
		}
		TRACE(L">>> CMyListView::SavePreferences\t%s\n",(LPCWSTR)strValue);
		theApp.WriteProfileString( L"Columns",strEntry,strValue);
	}

	theApp.WriteProfileInt( L"Preferences", L"Language",m_fHexLanguage == true ? 1 : 0);
	theApp.WriteProfileInt( L"Preferences", L"Time",m_fLocalTime == true ? 1 : 0);
	theApp.WriteProfileInt( L"Preferences", L"Size",m_fHexSize == true ? 1 : 0);
	theApp.WriteProfileInt( L"Preferences", L"Folders",m_fAllowFolders == true ? 1 : 0);
	theApp.WriteProfileInt( L"Preferences", L"PromptToSave",m_fSavePrompt == true ? 1 : 0);
	theApp.WriteProfileInt( L"Preferences", L"IncludePath",m_fIncludePath == true ? 1 : 0);
	theApp.WriteProfileInt( L"Preferences", L"AutoFit",m_fAutoFit == true ? 1 : 0);
	theApp.WriteProfileInt( L"Preferences", L"AudioCue",m_fAudioCue == true ? 1 : 0);
	theApp.WriteProfileString( L"Preferences", L"Wave",m_strWave);
	theApp.WriteProfileString( L"Preferences", L"SavePath",m_strSavePath);

	return true;
}

void CMyListView::OnViewOptions() 
{
	CMyDoc* pDoc=GetDocument();
	ASSERT_VALID( pDoc );

	CListCtrl& theListCtrl=GetListCtrl();
	for( int i=0; i < LIST_NUMCOLUMNS; i++ )
	{
		if (m_pci[i].IsVisible())
		{
			m_pci[i].SetWidth(theListCtrl.GetColumnWidth( i ));
		}
	}
	
	CMyPropSheet propSheet;
	propSheet.m_PageColumn.InitCheckList( m_pci );
	propSheet.m_PageGeneral.m_fIncludePath = m_fIncludePath;
	propSheet.m_PageGeneral.m_fAudioCue = m_fAudioCue;
	propSheet.m_PageGeneral.m_fSavePrompt = m_fSavePrompt;
	propSheet.m_PageGeneral.m_strSavePath = m_strSavePath;

	if (DoesFileExist(m_strWave))
	{
		propSheet.m_PageGeneral.m_strWave = m_strWave;
	}

	if (IDOK==propSheet.DoModal())
	{
		m_pci=propSheet.m_PageColumn.GetColumnInfo();
		UpdateWidths();

		m_fAudioCue = (TRUE == propSheet.m_PageGeneral.m_fAudioCue);
		m_strWave = propSheet.m_PageGeneral.m_strWave;
		m_strSavePath = propSheet.m_PageGeneral.m_strSavePath;
		m_fSavePrompt = (TRUE == propSheet.m_PageGeneral.m_fSavePrompt);
		m_fIncludePath = (TRUE == propSheet.m_PageGeneral.m_fIncludePath);

		Invalidate(true);
	}
}

void CMyListView::UpdateDirty()
{
}

void CMyListView::OnFileRename() 
{
	SHFILEOPSTRUCT	shFileOp;

	shFileOp.hwnd; 
	shFileOp.wFunc; 
	shFileOp.pFrom; 
	shFileOp.pTo; 
	shFileOp.fFlags; 
	shFileOp.fAnyOperationsAborted; 
	shFileOp.hNameMappings; 
	shFileOp.lpszProgressTitle; 
 }

void CMyListView::OnFileTouch() 
{
	m_fAutomatic = false;
	CListCtrl& theListCtrl=GetListCtrl();
	if (theListCtrl.GetSelectedCount() == 0)
	{
		CString strTitle;
#pragma warning(suppress: 6031)
		strTitle.LoadString(ERR_TITLE);
		CString strError;
#pragma warning(suppress: 6031)
		strError.LoadString(ERR_NOTOUCH);
		ErrorMessageBox(AfxGetMainWnd()->GetSafeHwnd(), GetLastError(), strTitle, strError);
		return;
	}

	CMyDoc* pDoc=GetDocument();
	ASSERT_VALID( pDoc );

	CTimeDlg dlg;
	if (IDOK!=dlg.DoModal())
	{
		return;
	}

	int iItem = 0;
	SYSTEMTIME time;
	FILETIME localftime;
	FILETIME ftime;
	CWiseFile* pInfo = NULL;

	SetRedraw(FALSE);

	CString strText;
	strText.FormatMessage(STR_FILETOUCH, theListCtrl.GetSelectedCount());
	
	CProgressBox Box;
	Box.Create(this, MWX_CENTER);
	Box.SetWindowText(strText);
	Box.m_ctlProgress.SetPos(0);
	Box.m_ctlProgress.SetRange32(0, theListCtrl.GetSelectedCount());
	Box.m_ctlProgress.SetStep(1);
	Box.ShowWindow(SW_SHOWNORMAL);
	strText.Empty();

	dlg.GetTime( &time );
	time.wSecond = 0;
	SystemTimeToFileTime( &time, &localftime);
	LocalFileTimeToFileTime( &localftime, &ftime);

	iItem = 0;
	POSITION pos = theListCtrl.GetFirstSelectedItemPosition();
	while (pos)
	{
		iItem = theListCtrl.GetNextSelectedItem(pos);
		pInfo = reinterpret_cast<CWiseFile*> (theListCtrl.GetItemData(iItem));
		ASSERT (pInfo);
		// party
		TRACE(L">>> Touching file.\r\n");
		pInfo->TouchFileTime( &ftime );//, TRUE, TRUE, TRUE);
		theApp.ForwardMessages();
		Box.m_ctlProgress.StepIt();
	}

	Box.DestroyWindow();

	SetRedraw(TRUE);
	Invalidate();
}

void CMyListView::OnEditSelectAll() 
{
	CListCtrl& theListCtrl=GetListCtrl();
	ListView_SetItemState( theListCtrl.GetSafeHwnd(), -1, LVIS_SELECTED, LVIS_SELECTED);
}

bool CMyListView::DeleteItem( int iItem )
{
	CListCtrl& theListCtrl=GetListCtrl();
	// BUGBUG What does this do?
	if(iItem == (theListCtrl.GetItemCount() - 1))
	{
		theListCtrl.SetItemState(iItem,0,0xFFFF);
		UpdateWindow();
	}

	return (TRUE==theListCtrl.DeleteItem( iItem ));
}

bool CMyListView::DeleteItem( CObject* pObject)
{
	TRACE(L" >>> CMyListView::DeleteItem\r\n");
	CListCtrl& theListCtrl=GetListCtrl();
	if (theListCtrl.GetSelectedCount() == 0)
	{

		ErrorMessageBox(AfxGetResourceHandle(),
			NULL,
			GetLastError(),
			ERR_TITLE,
			ERR_NODELETE);

		return FALSE;
	}

	int iItem;
	LV_FINDINFO lvfi;

	ZeroMemory( &lvfi, sizeof(lvfi));
	lvfi.flags=LVFI_PARAM;
	lvfi.lParam=(LPARAM) pObject;
	iItem=theListCtrl.FindItem( &lvfi, -1);
	if ( -1 == iItem)
	{
		return FALSE;
	}

	return DeleteItem( iItem );
}

bool CMyListView::InitColumns()
{
	TRACE(L">>> CMyListView::InitColumns()\r\n");
	int	iCol;
	LV_COLUMN lvc;

	ZeroMemory( &lvc, sizeof(lvc));
	CListCtrl& theListCtrl=GetListCtrl();

	WCHAR szText[LIST_MAXHEADLENGTH];

	lvc.mask=LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.pszText=szText;
	for (iCol=0; iCol < LIST_NUMCOLUMNS; iCol++)
	{
		lvc.iSubItem=iCol;
		::LoadString(AfxGetResourceHandle(),
				m_pci[iCol].GetLabelID(),
				szText, LIST_MAXHEADLENGTH);
				
		lvc.cx = max(m_pci[iCol].GetWidth(), 10);
		lvc.fmt = m_pci[iCol].GetFormat();
		if (theListCtrl.InsertColumn( iCol, &lvc) == -1 ) return false;
	}
	return true;
}

bool CMyListView::RedrawItem( int iItem )
{
	if (!::IsWindow(m_hWnd) || !::IsWindowVisible(m_hWnd))
	{
		return false;
	}

	RECT rcCrap;
	CListCtrl& theListCtrl = GetListCtrl();
	if (!theListCtrl.GetItemRect( iItem, &rcCrap, LVIR_BOUNDS))
	{
		TRACE(L"RedrawItem no item rect.\n");
		return false;
	}
	
	InvalidateRect( &rcCrap, FALSE);
	return true;
}

int CMyListView::FindVisibleItem(CObject* pObject)
{
	if (!::IsWindow(m_hWnd) || !::IsWindowVisible(m_hWnd))
	{
		return -1;
	}

	LVITEM lvi;
	ZeroMemory(&lvi, sizeof(LVITEM));
	lvi.mask = LVIF_PARAM;

	CListCtrl& theListCtrl=GetListCtrl();
	int iTop=theListCtrl.GetTopIndex();
	int iBottom = iTop + theListCtrl.GetCountPerPage();
	int iItem;
	
	for (iItem = iTop; iItem < iBottom; iItem++)
	{
		if (!::IsWindow(m_hWnd) || !::IsWindowVisible(m_hWnd))
		{
			return -1;
		}
		lvi.iItem = iItem;
		if (TRUE == theListCtrl.GetItem(&lvi))
		{
			if ( (LPARAM)pObject == (LPARAM)lvi.lParam )
			{
				return iItem;
			}
		}
	}

	CWiseFile* pFileInfo = reinterpret_cast<CWiseFile*> (theListCtrl.GetItemData(iItem));
	ASSERT(pFileInfo);
	TRACE(L"FindVisibleItem found item %s.\n", pFileInfo->GetFullPath());
	return -1;
}

bool CMyListView::RedrawItem( CObject* pObject )
{
	if (!::IsWindow(m_hWnd) || !::IsWindowVisible(m_hWnd))
	{
		return false;
	}

	int iItem = FindVisibleItem(pObject);
	if ( -1 == iItem)
	{
		return false;
	}

	return RedrawItem(iItem);
}

void CMyListView::UpdateWidths()
{
	CListCtrl& theListCtrl=GetListCtrl();
	SetRedraw(FALSE);
	for( int i=0; i < LIST_NUMCOLUMNS; i++ )
	{
		TRACE(L">>> CMyListView:UpdateWidths visible %d, %d\r\n",i,m_pci[i].IsVisible());
		if (FALSE == m_pci[i].IsVisible())
		{
			theListCtrl.SetColumnWidth( i, 0 );
		}
		else
		{
			theListCtrl.SetColumnWidth( i, m_pci[i].GetWidth() );
		}
	}
	SetRedraw(TRUE);
}

void CMyListView::OnFileAdd() 
{
	TRACE(L">>> CMyListView::OnFileAdd()\n");
	#define BUFSIZE	512 * _MAX_PATH

	LPWSTR pszBuf = new WCHAR[BUFSIZE];
	if (pszBuf == nullptr)
		return;

	*pszBuf = '\0';
	WCHAR szFilter[_MAX_PATH];
	WCHAR szTitle[_MAX_PATH];
	int nFiles = 0;

	CMyDoc* pDoc=GetDocument();
	ASSERT_VALID( pDoc );
	::LoadString(AfxGetResourceHandle(), STR_FILEFILTER,  szFilter, _MAX_PATH);
	::LoadString(AfxGetResourceHandle(), STR_OPENTITLE, szTitle, _MAX_PATH);
	pipe2null(szFilter);

	if (!OpenBox(m_hWnd, szTitle, szFilter, pszBuf, NULL, OFN_FILEMUSTEXIST | OFN_FORCESHOWHIDDEN | OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY))
		return;

	if (*pszBuf == '\0')
		return;

	pDoc->PauseAllThreads(true);

	if (DoesFileExist(pszBuf))
	{
		// one file, just add it
		pDoc->AddFile(pszBuf);
		// resume processing
		pDoc->PauseAllThreads(false);
		return;
	}

	// selected multiple files
	// first thing in the list is the folder
	wcscpy_s(szTitle, _MAX_PATH, pszBuf);

	// walk past the folder
	LPWSTR pchFile = pszBuf;
	while (*pchFile != '\0')
		pchFile = CharNext(pchFile);
	pchFile++;

	LPWSTR pch = pchFile;
	while(true)
	{
		while (*pch != '\0')
			pch = CharNext(pch);
		nFiles++;
		pch++;
		if (*pch == '\0') break;
	}

	bool fBox = false;
	CProgressBox Box;
	if (nFiles > 20)
	{
		CString strText;
		Box.Create(this, MWX_APP | MWX_CENTER);
#pragma warning(suppress: 6031)
		strText.LoadString(STR_FILEADD);
		Box.SetWindowText(strText);
		Box.m_ctlProgress.SetPos(0);
		Box.m_ctlProgress.SetRange32(0, nFiles);
		Box.m_ctlProgress.SetStep(1);
		Box.ShowWindow(SW_SHOWNORMAL);
		fBox = true;
	}

	pch = pchFile;
	while(true)
	{
		pDoc->AddFile(szTitle, pch);
		if (fBox) Box.m_ctlProgress.StepIt();

		while (*pch != '\0')
			pch = CharNext(pch);
		pch++;
		if (*pch == '\0') break;
	}

	if (fBox) Box.DestroyWindow();

	// resume processing
	pDoc->PauseAllThreads(false);
}

void CMyListView::OnDestroy() 
{
	SavePreferences();
	CListView::OnDestroy();
}

void CMyListView::MyModifyStyleEx( LRESULT dwRemove, LRESULT dwAdd)
{
	CListCtrl& theListCtrl=GetListCtrl();
	LRESULT dwExStyle = theListCtrl.SendMessage (LVM_GETEXTENDEDLISTVIEWSTYLE);
	dwExStyle |= dwAdd;
	dwExStyle &= ~dwRemove;
	theListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, 0, dwExStyle);
}

void CMyListView::OnFileProperties() 
{
	// get the doc
	CMyDoc* pDoc=GetDocument();
	ASSERT_VALID( pDoc );

	// get the listctrl
	CListCtrl& theListCtrl=GetListCtrl();
	if (theListCtrl.GetSelectedCount() > 1 )
	{
		return;
	}

	// get the first selected item
	POSITION pos = theListCtrl.GetFirstSelectedItemPosition();
	int iItem = theListCtrl.GetNextSelectedItem(pos);

	// get the object
	CWiseFile* pFileInfo = reinterpret_cast<CWiseFile*> (theListCtrl.GetItemData(iItem));
	ASSERT (pFileInfo);
	
	POINT pt = {0,0};
	GetItem(iItem, pt);
	pt.x += 2;
	pt.y += (m_nMonoY + 2);

	ShowShellFileProperties(m_hWnd, pFileInfo->GetFullPath());
}

void CMyListView::OnEditRemove() 
{
	CMyDoc* pDoc=GetDocument();
	ASSERT_VALID( pDoc );

	CListCtrl& theListCtrl=GetListCtrl();

	CProgressBox Box;
	CString strText;
	// create the progess box
	strText.FormatMessage(STR_FILEREMOVE, theListCtrl.GetSelectedCount());
	Box.Create(this, MWX_APP | MWX_CENTER);
	Box.SetWindowText(strText);
	Box.m_ctlProgress.SetPos(0);
	Box.m_ctlProgress.SetStep(1);
	Box.ShowWindow(SW_SHOWNORMAL);
	Box.m_ctlProgress.SetRange32(0, theListCtrl.GetSelectedCount());
	strText.Empty();

	SetRedraw(FALSE);
	CWiseFile* pInfo=NULL;
	int iItem = theListCtrl.GetNextItem( -1, LVNI_SELECTED);
	while (iItem != -1)
	{
		Box.m_ctlProgress.StepIt();
		pInfo= reinterpret_cast<CWiseFile*> (theListCtrl.GetItemData(iItem));
		ASSERT (pInfo);
		pDoc->DeleteFile( pInfo );
		iItem = theListCtrl.GetNextItem( iItem-1, LVNI_SELECTED);
	}
	SetRedraw(TRUE);
	
	if (theListCtrl.GetItemCount() > iItem)
	{
		theListCtrl.SetItemState( iItem, LVIS_FOCUSED, LVIS_FOCUSED );
	}
	else
	{
		theListCtrl.SetItemState( theListCtrl.GetItemCount()-1, LVIS_FOCUSED, LVIS_FOCUSED );
	}

	Box.DestroyWindow();
}

void CMyListView::OnViewSmartFit() 
{
	CListCtrl& theListCtrl = GetListCtrl();
	SetRedraw(FALSE);

	for (int i = 0; i < LIST_NUMCOLUMNS; i++)
	{
		if (TRUE == m_pci[i].IsVisible())
		{
			m_fLockHeaderWidth = true;
			theListCtrl.SetColumnWidth(i, LVSCW_AUTOSIZE);
		}
	}

	SetRedraw(TRUE);
	Invalidate(FALSE);

	m_fLockHeaderWidth = false;
}

void CMyListView::MarkColumn()
{
	if (m_iSortColumn == -1)
	{
		return;
	}

	CHeaderCtrl* pHeader = GetListCtrl().GetHeaderCtrl();
	if (NULL==pHeader)
	{
		TRACE0( "MarkColumn can't get header control\n.");
		return;
	}

	HDITEM hi;
	ZeroMemory(&hi, sizeof(HDITEM));
	if (!pHeader->GetItem(m_iSortColumn, &hi))
	{
		TRACE0( "MarkColumn can't get previously sorted column\n.");
		return;
	}

	hi.mask = HDI_FORMAT | HDI_IMAGE;
	hi.iImage=0;
	hi.fmt = HDF_STRING | HDF_IMAGE | HDF_BITMAP_ON_RIGHT;
	if (LVCFMT_RIGHT == m_pci[m_iSortColumn].GetFormat())
	{
		hi.fmt |= HDF_RIGHT;
	}
	else
	{
		hi.fmt |= HDF_LEFT;
	}

	if (m_fSortAscend!=SORT_ASCENDING)
	{
		hi.iImage=1;
	}

	pHeader->SetItem(m_iSortColumn, &hi);
}

void CMyListView::UnMarkColumn()
{
	if (m_iSortColumn == -1)
	{
		return;
	}

	CHeaderCtrl* pHeader = GetListCtrl().GetHeaderCtrl();
	if (NULL==pHeader)
	{
		TRACE0( "MarkColumn can't get header control\n.");
		return;
	}

	HDITEM hi;
	ZeroMemory(&hi, sizeof(HDITEM));
	if (!pHeader->GetItem(m_iSortColumn, &hi))
	{
		TRACE0( "MarkColumn can't get previously sorted column\n.");
		return;
	}

	hi.mask = HDI_FORMAT;
	hi.fmt = HDF_STRING;
	if (LVCFMT_RIGHT == m_pci[m_iSortColumn].GetFormat())
	{
		hi.fmt |= HDF_RIGHT;
	}
	else
	{
		hi.fmt |= HDF_LEFT;
	}

	pHeader->SetItem(m_iSortColumn, &hi);
}

int CMyListView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	HWND hWnd = GetListCtrl().GetSafeHwnd();

	DWORD dwStyle = ::GetClassLong (m_hWnd, GCL_STYLE);
	::SetClassLong (m_hWnd, GCL_STYLE, dwStyle & ~(CS_HREDRAW | CS_VREDRAW));

	dwStyle = ::GetClassLong (hWnd, GCL_STYLE);
	::SetClassLong (hWnd, GCL_STYLE, dwStyle & ~(CS_HREDRAW | CS_VREDRAW));

	return 0;
}

void CMyListView::UpdateStatus(LPCWSTR pszStatus)
{
	CMainFrame* pFrame = static_cast<CMainFrame*> (GetParentFrame());
	if (NULL == pFrame || !::IsWindow(pFrame->m_hWnd))
		return;

	pFrame->GetStatusBar()->SetPaneText(0, pszStatus, TRUE);
}

void CMyListView::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMLVCUSTOMDRAW* pCD = reinterpret_cast<NMLVCUSTOMDRAW*> (pNMHDR);
	*pResult = CDRF_DODEFAULT;

	if(pCD->nmcd.dwDrawStage == CDDS_PREPAINT)
	{
		// Request prepaint notifications for each item.
		*pResult = CDRF_NOTIFYITEMDRAW;
	}

	if(pCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
	{
		// Request prepaint notifications for each subitem.
		*pResult = CDRF_NOTIFYSUBITEMDRAW;
	}

	if(pCD->nmcd.dwDrawStage == (CDDS_ITEMPREPAINT | CDDS_SUBITEM))
	{
		// a SubItem (column) is being drawn
		if (m_pci[pCD->iSubItem].IsFixedWidth())
		{
			SelectObject(pCD->nmcd.hdc, (HFONT) m_fntMono);
			*pResult = CDRF_NEWFONT;
		}
		else
		{
			SelectObject(pCD->nmcd.hdc, (HFONT) m_fntView);
			*pResult = CDRF_NEWFONT;
		}
	}
}

BOOL CMyListView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	HD_NOTIFY* pHDN = reinterpret_cast<HD_NOTIFY*> (lParam);

	// handle column header double clicks
	if (pHDN->hdr.code == HDN_DIVIDERDBLCLICKW || pHDN->hdr.code == HDN_DIVIDERDBLCLICKA)
	{
		m_fLockHeaderWidth=true;
		return CListView::OnNotify(wParam, lParam, pResult);
	} 

	// handle column width changing
	if (pHDN->hdr.code == HDN_ITEMCHANGINGW || pHDN->hdr.code == HDN_ITEMCHANGINGA)
	{
		int iItem = pHDN->iItem;
		if (m_pci[iItem].IsFixedWidth())
		{
			if ( m_fLockHeaderWidth && m_pci[iItem].IsVisible() && (pHDN->pitem->cxy > m_nMonoX * 2) && (pHDN->pitem->cxy < (m_nMonoX * m_pci[iItem].GetFixedWidth())))
				pHDN->pitem->cxy = m_nMonoX * m_pci[iItem].GetFixedWidth();
		}
		return CListView::OnNotify(wParam, lParam, pResult);
	} 

	m_fLockHeaderWidth = false;
	return CListView::OnNotify(wParam, lParam, pResult);
}

LRESULT CMyListView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// took out check for ::IsWindow
	if (theApp.WM_UPDATEVIEW() == message)
	{
			OnUpdate(this, wParam, reinterpret_cast<CObject*> (lParam));
	}

	return CListView::WindowProc(message, wParam, lParam);
}

void CMyListView::OnEditCopy() 
{
	CMyDoc* pDoc = GetDocument();
	ASSERT_VALID( pDoc );
	if (!pDoc)
		return;

	// If we have dirty files alert the user
	if (pDoc->IsDirty())
	{
		if (IDNO == AfxMessageBox(STR_DIRTYCOPY, MB_YESNO | MB_ICONQUESTION))
		{
			return;
		}
	}

	CListCtrl& theListCtrl = GetListCtrl();
	if (theListCtrl.GetSelectedCount() > 1000)
	{
		// warn
		if (IDNO == AfxMessageBox(STR_COPYTONS, MB_YESNO | MB_ICONQUESTION))
		{
			return;
		}
	}

	if(!OpenClipboard())
	{
		ErrorMessageBox(AfxGetResourceHandle(),
			NULL,
			GetLastError(),
			ERR_TITLE,
			ERR_CLIPBOARD);
		return;
	}

	::EmptyClipboard() ;

	if (!FillBuffer(false, false))
	{
		ErrorMessageBox(AfxGetResourceHandle(),
			NULL,
			GetLastError(),
			ERR_TITLE,
			ERR_CLIPBOARD);
		CloseClipboard();
		return;
	}

	if (NULL == SetClipboardData (CF_UNICODETEXT, (LPVOID)GetBufferHandle()))
	{
		ErrorMessageBox(AfxGetResourceHandle(),
			NULL,
			GetLastError(),
			ERR_TITLE,
			ERR_CLIPBOARD);
	}

	CloseClipboard();
	CloseBuffer();
	return;
}

bool CMyListView::GetItem(int nItem, POINT pt)
{
	if (!::IsWindow(m_hWnd))
		return false;

	CListCtrl& theListCtrl = GetListCtrl();
	theListCtrl.GetItemPosition(nItem, &pt);
	if (pt.x < 0)
	{
		pt.x = 0;
	}
	if (pt.y < 0)
	{
		pt.y = 0;
	}
	return true;
}

bool CMyListView::PromptToSave()
{
	return m_fSavePrompt;
}

bool CMyListView::GetCommandLineFolder(LPWSTR pszFolder)
{
	// if there is something else on the command line
	// and it's a folder name recurse the folder without
	// showing the quickhelp dialog
	LPWSTR pszCommandLine = ::GetCommandLine();
	LPWSTR pch = pszCommandLine;

	// if we don't enter quotes, then we grab everything after the first whitespace
	// as soon as we enter quotes, we find the end quote and grab everything after the
	// first whitespace after that. we always end at the first whitespace, or the end
	// of the string

	// c:\something.exe foo -> foo
	// "c:\something.exe" foo -> foo
	// "c:\something.exe"x foo -> error, what's the x
	// "c:\something.exe" x foo -> x
	// c:\my programs\something.exe

	while(*pch != '\0')
	{
		// find the first quote or whitespace, which ever comes first
		if ( isquote(pch) || _istspace(*pch) )
			break;
		pch = CharNext(pch);
	}
	
	// if we found a quote, find the next one
	if (isquote(pch))
	{
		pch = CharNext(pch);
		while(*pch != '\0')
		{
			if (isquote(pch))
				break;
			pch = CharNext(pch);
		}
		pch = CharNext(pch);
	}

	// eat all whitespace between our argv[0] and the next thing
	while(*pch != '\0')
	{
		if (!_istspace(*pch))
			break;
		pch = CharNext(pch);
	}

	// pch is pointing at our argv[1]
	// if it starts with a " then we copy what's between the quotes
	// if it doesn't, then we copy up until a whitespace
	LPWSTR pchEnd = pch;
	pchEnd = CharNext(pchEnd);
	if (isquote(pch))
	{
		pch = CharNext(pch);
		while(*pchEnd != '\0')
		{
			if (isquote(pch))
				break;
			pchEnd = CharNext(pchEnd);
		}
		pchEnd = CharPrev(pch, pchEnd);
	}
	else
	{
		while(*pchEnd != '\0')
		{
			if (_istspace(*pchEnd))
				break;
			pchEnd = CharNext(pchEnd);
		}
	}
	*pchEnd = '\0';

	lstrcpy(pszFolder, pch);

	// if it's empty, then the call succeeded but there was no arg
	if (lstrcch(pszFolder) < 1)
	{
		return true;
	}

	// if it's full, and it's a valid folder, then it succeeded
	// and the arg is in szFolder
	if (DoesFolderExist(pszFolder))
	{
		return true;
	}

	// there was something specified, but it wasn't a folder
	return false;
}

// caller responsible for delete[]ing returned pointer
bool CMyListView::FillBuffer(bool fAllRows, bool fAllFields) 
{
	fAllFields;
	CMyDoc* pDoc = GetDocument();
	ASSERT_VALID( pDoc );
	if (!pDoc || pDoc->IsEmpty())
	{
		// no valid document
		return false;
	}

	CListCtrl& theListCtrl = GetListCtrl();
	if (!fAllRows && theListCtrl.GetSelectedCount() < 1)
	{
		// they want what's selected, but there's nothing selected
		return false;
	}

	DWORD cbBuf = 0;
	CWiseFile* pInfo = NULL;
	int iItem = 0;
	int c = 0;
	int j = 0;
	POSITION pos;

	CloseBuffer();
	// count up the size of the buffer we need
	if (fAllRows)
	{
		c = theListCtrl.GetItemCount();
		for (j = 0; j < c; j++)
		{
			pInfo = reinterpret_cast<CWiseFile*> (theListCtrl.GetItemData(j));
			ASSERT (pInfo);

			cbBuf += sizeof(*pInfo);
		}
	}
	else
	{
		pos = theListCtrl.GetFirstSelectedItemPosition();
		while (pos)
		{
			iItem = theListCtrl.GetNextSelectedItem(pos);
			pInfo = reinterpret_cast<CWiseFile*> (theListCtrl.GetItemData(iItem));
			ASSERT (pInfo);

			cbBuf += sizeof(*pInfo);
		}
	}

	// create buffer for holding individual items
	LPWSTR pszItem = new WCHAR [8192];
	ASSERT (pszItem);
	
	// create buffer for holding all output
	// done in clipboard friendly format
	m_hBuf = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, (cbBuf + 2048));
	if( m_hBuf == NULL)
	{
		delete [] pszItem;
		return false;
	}
	
	m_pBuf = (LPWSTR) GlobalLock(m_hBuf); 
	if (NULL == m_pBuf)
	{
		delete [] pszItem;
		GlobalUnlock(m_hBuf);
		GlobalFree(m_hBuf);
		return false;
	}

	// get header and put header into big buffer
	pDoc->GetHeaderString(pszItem);
	lstrcpy(m_pBuf, pszItem);
	lstrcat(m_pBuf, L"\r\n");

	// now put actual data in buffer
	if (fAllRows)
	{
		c = theListCtrl.GetItemCount();
		for (j = 0; j < c; j++)
		{
			pInfo = reinterpret_cast<CWiseFile*> (theListCtrl.GetItemData(j));
			ASSERT (pInfo);

			if (pInfo != nullptr)
			{
				pDoc->GetRowString(*pInfo, pszItem);
				lstrcat(m_pBuf, pszItem);
				lstrcat(m_pBuf, L"\r\n");
			}
		}
	}
	else
	{
		// copy the data into the buffer
		iItem = 0;
		pos = theListCtrl.GetFirstSelectedItemPosition();
		while (pos)
		{
			iItem = theListCtrl.GetNextSelectedItem(pos);
			pInfo = reinterpret_cast<CWiseFile*> (theListCtrl.GetItemData(iItem));
			if (pInfo == nullptr)
			{
				ASSERT(pInfo);
				delete[] pszItem;
				return false;
			}
			pDoc->GetRowString( *pInfo, pszItem );
			lstrcat( m_pBuf, pszItem );
			lstrcat( m_pBuf, L"\r\n" );
		}
	}

	delete [] pszItem;
	lstrcat(m_pBuf, L"\r\n");
		
	return true;
}

void CMyListView::CloseBuffer()
{
	if (NULL != m_hBuf)
	{
		GlobalUnlock(m_hBuf);
		GlobalFree(m_hBuf);
	}
	m_pBuf = NULL;
	m_hBuf = NULL;
}

LPWSTR CMyListView::GetBuffer()
{
	return m_pBuf;
}

SIZE_T CMyListView::GetBufferSize()
{
	return GlobalSize(m_hBuf);
}

HGLOBAL CMyListView::GetBufferHandle()
{
	return m_hBuf;
}

void CMyListView::ShowCmdHelp()
{
		CString strTitle;
#pragma warning(suppress: 6031)
		strTitle.LoadString(ERR_TITLE);
		CString strError;
		strError.FormatMessage(ERR_BADCOMMANDLINE, m_szFolder);
		ErrorMessageBox(AfxGetMainWnd()->GetSafeHwnd(), GetLastError(), strTitle, strError);
}

void CMyListView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (!m_fCrashOnPause)
	{
		CListView::OnKeyDown(nChar, nRepCnt, nFlags);
		return;
	}

	if (VK_PAUSE == nChar)
	{
		RaiseException( EXCEPTION_ACCESS_VIOLATION, 0, 0, 0);
	}
	else
		CListView::OnKeyDown(nChar, nRepCnt, nFlags);
}
