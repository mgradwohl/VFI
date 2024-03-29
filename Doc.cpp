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

// Doc.cpp : implementation of the CMyDoc class
//
#include "stdafx.h"
#include "globals.h"
#include "vfi.h"
#include "mylistview.h"
#include "frame.h"
#include "Doc.h"
#include <memory>
#include "filelib.h"
#include "wndlib.h"

#ifdef _DEBUG
	#ifdef DEBUG_NEW
		#define new DEBUG_NEW
		#undef THIS_FILE
		static CHAR THIS_FILE[] = __FILE__;
	#endif
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyDoc

IMPLEMENT_DYNCREATE(CMyDoc, CDocument)

BEGIN_MESSAGE_MAP(CMyDoc, CDocument)
//{{AFX_MSG_MAP(CMyDoc)
ON_COMMAND(ID_FILE_SAVEAS, OnFileSaveas)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyDoc construction/destruction

CMyDoc::CMyDoc()
{
	m_fLastTime = false;
	m_fFirstTime = true;
	m_fCheckPath = false;
	m_qwSize = 0;
	m_qwSizeRead = 0;
	m_dwDirtyCRC = 0;
	m_dwDirtyInfo = 0;
	
	m_Mutex.Unlock();
	m_KillList.RemoveAll();
	m_FileList.RemoveAll();
	m_DirtyList.RemoveAll();
}

CMyDoc::~CMyDoc()
{
	FreeCRCMemory();
}

BOOL CMyDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return false;
	
	m_qwSize = 0;
	m_qwSizeRead = 0;
	UpdateAllViews( NULL, HINT_EMPTY, NULL);
	
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CMyDoc serialization

void CMyDoc::Serialize(CArchive& ar)
{
	ar;
	SetModifiedFlag( false );
}

/////////////////////////////////////////////////////////////////////////////
// CMyDoc diagnostics

#ifdef _DEBUG
void CMyDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMyDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMyDoc commands


bool CMyDoc::DeleteFile( CWiseFile* pFileInfo )
{
	if (pFileInfo == nullptr)
		return false;

	if ( !pFileInfo->CheckState(FWFS_CRC_COMPLETE) )
	{
		InterlockedDecrement(&m_dwDirtyCRC);
	}
	
	if ( !pFileInfo->CheckState(FWFS_VERSION) )
	{
		InterlockedDecrement(&m_dwDirtyInfo);
	}
	
	// Update total filesize
	m_qwSize -= pFileInfo->GetSize();

	SetModifiedFlag( !m_FileList.isEmpty() );
	
	// Tell the view the file is gone
	UpdateAllViews( NULL, HINT_DELETE, pFileInfo);

	spWiseFile p;
	p.reset(pFileInfo);
	AddToKill(p);
	
	return true;
}

BOOL CMyDoc::OnSaveDocument(LPCWSTR lpszPathName) 
{
	lpszPathName;
	
	// If we have dirty files alert the user
	if (IsDirty())
	{
		if (IDNO == AfxMessageBox(STR_DIRTYSAVE, MB_YESNO | MB_ICONQUESTION))
		{
			return false;
		}
	}

	WCHAR szPath[_MAX_PATH];
	GetFileName(szPath);

	// Create A Common File Dialog, with the Desktop Folder as the default
	WCHAR szFilter[1024];
	WCHAR szTitle[128];
	LoadString(AfxGetResourceHandle(), STR_SAVEFILTER, szFilter, 1024);
	pipe2null(szFilter);
	LoadString(AfxGetResourceHandle(), STR_SAVEAS, szTitle, 128);
	if (!SaveBox(AfxGetMainWnd()->m_hWnd, szTitle, szFilter, szPath, OFN_OVERWRITEPROMPT))
		return false;

	if (WriteFileEx(szPath))
	{
		return true;
	}

	DWORD const dw = GetLastError();
	if (dw != 0)
	{
		// error
		ErrorMessageBox(AfxGetResourceHandle(),
			AfxGetMainWnd()->m_hWnd,
			GetLastError(),
			ERR_TITLE,
			ERR_SAVEGENERAL);
	}
	return false;
}

BOOL CMyDoc::SaveModified() 
{
	TRACE(L"CMyDoc::SaveModified()\n");
	const CMainFrame* pFrame = dynamic_cast<CMainFrame*> (theApp.GetMainWnd());
	CMyListView* const pView = dynamic_cast<CMyListView*> (pFrame->GetActiveView());

	if (!pView->PromptToSave())
	{
		return true;
	}

	if (!IsModified() )
	{
		return true;
	}
	
	const int iReturn = AfxMessageBox(STR_SAVEMODIFIED,MB_ICONQUESTION | MB_YESNOCANCEL);
	
	switch (iReturn)
	{
	case IDCANCEL:
		return false;
	case IDYES:
		OnSaveDocument( L"");
		return true;
	case IDNO:
		SetModifiedFlag(false);
		return true;
	}
	TRACE(L"CMyDoc::SaveModified() should never get here.\n");
	return false;
}

void CMyDoc::OnFileSaveas() 
{
	TRACE(L"CMyDoc::OnFileSaveas()\n");
	OnSaveDocument(L"");
}

DWORD CMyDoc::RecurseDir( LPWSTR pszPath )
{
	if (NULL == pszPath)
		return 0;
	
	SuspendAllThreads();

	CFileFind m_Find;
	CStringList PathList;
	CString strText;
	CStringList FileList;
	bool fRun=false;
	
	const CMainFrame* pFrame = dynamic_cast<CMainFrame*> (AfxGetMainWnd());
	if (pFrame == nullptr)
	{
		return (DWORD)-1;
	}
	CMyListView* const pView = dynamic_cast<CMyListView*> (pFrame->GetActiveView());
	if (pView == nullptr)
	{
		return (DWORD)-1;
	}

	strText.LoadString(STR_FILECOUNTING);
	pView->UpdateStatus(strText);

	PathList.AddTail( pszPath );

	WCHAR szSearch[_MAX_PATH];
	while ( !PathList.IsEmpty() )
	{
		lstrcpy(szSearch, PathList.RemoveHead());
		PathAppend(szSearch, L"*.*");
		
		// now process them
		fRun = m_Find.FindFile(szSearch,0);

		while ( fRun )
		{
			// if it's a directory, and it's not dots then add it to the path list
			// this will ASSERT in DEBUG and the ASSERT seems invalid
			// because we are not calling .Get()
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
				return (DWORD)-1;
			}
			fRun = m_Find.FindNextFile();
		} // there are more files
	} // there are more directories
	m_Find.Close();	
	
	while ( !PathList.IsEmpty() )
	{
		lstrcpy(szSearch, PathList.RemoveHead());
		PathAppend(szSearch, L"*.*");

		fRun = ( TRUE == m_Find.FindFile(szSearch,0) );
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
				return (DWORD)-1;
			}
		}
	}

	size_t count = FileList.GetCount();
	WCHAR szCount[64];
	int2str(szCount, count);
	strText.FormatMessage(STR_FILEADD, szCount);
	pView->UpdateStatus(strText);

	CString strFile;
	while ( !FileList.IsEmpty())
	{
		strFile = FileList.RemoveHead();
		if (false == this->AddFile(strFile))
		{
			strText.FormatMessage(ERR_FILEINUSE, (LPCWSTR)strFile);
		}
		theApp.ForwardMessages();
		if (g_eTermThreads.Signaled())
		{
			return (DWORD)-1;
		}
	}

	SetPathName(pszPath, false);
	pView->ResetStatus();
	ResumeAllThreads();

	return 0;
}

void CMyDoc::DeleteContents() 
{
	TRACE(L"CMyDoc::DeleteContents()\n");
	if (!m_fFirstTime)
	{
		UpdateAllViews( NULL, HINT_EMPTY, NULL);
		
		// Clear the Dirty List
		RemoveAllDirty();
		
		// Clear the Main List
		DeleteMainList();
		
		// Now get everything off the kill list
		DeleteKillList();

		TerminateThreads();
	}
	
	if (!m_fLastTime)
	{
		CreateThreads();
		SuspendAllThreads();
	}
	
	m_fFirstTime = false;
	m_qwSize = 0;
	m_qwSizeRead = 0;
	m_dwDirtyCRC = 0;
	m_dwDirtyInfo = 0;
}

void CMyDoc::RemoveAllDirty()
{
	TRACE(L"CMyDoc::RemoveAllDirty()\n");
	
	m_DirtyList.RemoveAll();
}

DWORD UpdateThreadInfo( LPVOID pParam )
{
	TRACE(L"THREADINFO: begin\n");
	
	CMyDoc* pDoc = static_cast<CMyDoc*> (pParam);
	if (pDoc == nullptr)
	{
		return (DWORD)-1;
	}

	spWiseFile pFileInfo = NULL;

	// keep the thread alive
	while (true)
	{
		g_eGoThreadInfo.Wait(INFINITE);
		if (g_eTermThreads.Signaled())
		{
			return (DWORD)-1;
		}
		
		// keep the thread working
		while (pDoc->IsDirty())
		{
			g_eGoThreadInfo.Wait(INFINITE);
			if (g_eTermThreads.Signaled())
			{
				return (DWORD)-1;
			}
			
			// Get something off the list
			pFileInfo = pDoc->RemoveDirtyHead();
			if (NULL == pFileInfo)
			{
				TRACE(L"THREADINFO: Null dirty list\n");
				break;
			}

			if (pFileInfo->CheckState(FWFS_DELETE))
			{
				TRACE(L"THREADINFO: killing %s\n",pFileInfo->GetFullPath());
				pDoc->AddToKill(pFileInfo);
				break;
			}

			if (pFileInfo->CheckState(FWFS_VERSION))
			{
				TRACE(L"THREADINFO: %s already has version info\n",pFileInfo->GetFullPath());
				pDoc->AddToDirty(pFileInfo);
				break;
			}

			TRACE(L"THREADINFO: setting %s\n",pFileInfo->GetFullPath());

			pFileInfo->ReadVersionInfo();
					
			TRACE(L"THREADINFO: ChangeItemState\n");
			pDoc->ChangeItemState(pFileInfo, FWFS_VERSION);

			// the file has version information, but doesn't have CRC information
			// so add it back to the dirty list so that the CRC thread will pick it up
			pDoc->AddToDirty(pFileInfo);
		} //while

		if (pDoc != nullptr)
		{
			if (pDoc->m_dwDirtyInfo < 1)
			{
				TRACE(L"THREADINFO: pausing\n");
				g_eGoThreadInfo.Reset();
			}
		}
	} // while
	TRACE(L"THREADINFO: exiting\n");

	return 0;
}

bool CMyDoc::ResumeAllThreads()
{
	TRACE(L"CMyDoc::ResumeAllThreads()\n");
	
	if ( (!g_eGoThreadInfo.Signaled()) && (m_dwDirtyInfo > 0) )
	{
		TRACE(L"ResumeAllThreads: Info thread paused, starting\r\n");
		g_eGoThreadInfo.Signal(true);
	}

	if ( (!g_eGoThreadCRC.Signaled()) && (m_dwDirtyCRC > 0) )
	{
		TRACE(L"ResumeAllThreads: CRC thread paused, starting\r\n");
		g_eGoThreadCRC.Signal(true);
	}
	
	return true;
}

bool CMyDoc::SuspendAllThreads()
{
	g_eGoThreadInfo.Reset();
	g_eGoThreadCRC.Reset();
	
	return true;
}

spWiseFile CMyDoc::RemoveDirtyHead()
{
	TRACE(L"CMyDoc::RemoveDirtyHead\n");
	
	spWiseFile pObject = NULL;

	CSingleLock m_sLock( &m_Mutex );
	while (!m_sLock.Lock(MTX_LOCK));
		if (m_DirtyList.isEmpty())
		{
			TRACE (L"Attempt to remove from empty dirty list");
			pObject = NULL;
		}
		else
		{
			pObject = m_DirtyList.RemoveHead();
		}
	m_sLock.Unlock();
	
	return pObject;
}

bool CMyDoc::RemoveFromDirty( spWiseFile pFileInfo )
{
	TRACE(L"CMyDoc::RemoveFromDirty\n");

	if (pFileInfo==nullptr || m_DirtyList.isEmpty())
	{
		return false;
	}
	
	CSingleLock m_sLock( &m_Mutex );
	while (!m_sLock.Lock(MTX_LOCK));
		m_DirtyList.Remove(pFileInfo);
	m_sLock.Unlock();
	
	return true;
}

bool CMyDoc::AddToDirty( spWiseFile pFileInfo )
{
	TRACE(L"CMyDoc::AddToDirty\n");

	if (pFileInfo == nullptr)
	{
		return false;
	}

	CSingleLock m_sLock( &m_Mutex );
		while (!m_sLock.Lock(MTX_LOCK));
		m_DirtyList.AddTail(pFileInfo);
	m_sLock.Unlock();
	
	return true;
}

void CMyDoc::ChangeItemState( spWiseFile pFileInfo, WORD wState )
{
	TRACE(L"CMyDoc::ChangeItemState\n");
	if (pFileInfo == nullptr)
	{
		return;
	}

	
	if (pFileInfo->CheckState(FWFS_DELETE))
	{
		TRACE(L"ChangeItemState: file marked for deletion, ignoring new state\n");
		return;
	}
	
	TRACE(L"ChangeItemState: changing state and counts\n");
	pFileInfo->OrState(wState);

	if ( FWFS_VERSION == wState )
	{
		InterlockedDecrement(&m_dwDirtyInfo);
		SafeUpdateAllViews(NULL, HINT_REFRESH, static_cast<CObject*> (pFileInfo.get()));
	}
	
	if ( FWFS_CRC_COMPLETE == wState)
	{
		InterlockedDecrement(&m_dwDirtyCRC);
		SafeUpdateAllViews(NULL, HINT_REFRESH, static_cast<CObject*> (pFileInfo.get()));
	}

	if ( FWFS_CRC_ERROR == wState)
	{
		pFileInfo->OrState(FWFS_CRC_COMPLETE);
		InterlockedDecrement(&m_dwDirtyCRC);
		SafeUpdateAllViews(NULL, HINT_REFRESH, static_cast<CObject*> (pFileInfo.get()));
	}

	if ((FWFS_CRC_WORKING == wState) || (FWFS_CRC_PENDING == wState))
	{
		SafeUpdateAllViews(NULL, HINT_REFRESH, static_cast<CObject*> (pFileInfo.get()));
	}

	if (0 == m_dwDirtyCRC && !m_fSentHintCRC)
	{
		SafeUpdateAllViews( NULL, HINT_DONECRC, NULL);
		m_fSentHintCRC = true;
	}
	
	if (0 == m_dwDirtyInfo && !m_fSentHintInfo)
	{
		SafeUpdateAllViews( NULL, HINT_DONEINFO, NULL);
		m_fSentHintInfo = true;
	}
}

void CMyDoc::AddToKill( spWiseFile pFileInfo )
{
	TRACE(L"CMyDoc::AddToKill\n");
	if (NULL == pFileInfo)
		return;
	
	pFileInfo->OrState( FWFS_DELETE );
	RemoveFromDirty(pFileInfo);
	RemoveFromMain(pFileInfo);
	m_KillList.AddHead(pFileInfo);
}

void CMyDoc::DeleteKillList()
{
	m_KillList.RemoveAll();
}

bool CMyDoc::AddFile(CString strFolder, CString strFileName)
{
	PathAddBackslash(strFolder.GetBuffer(_MAX_PATH));
	strFolder.ReleaseBuffer(-1);
	strFolder += strFileName;
	return AddFile(strFolder);
}

bool CMyDoc::AddFile(LPCWSTR pszFilename)
{
	TRACE(L"CMyDoc::AddFile()\n");
	
	// the mutex is a good signal that the Doc is going away
	if (g_eTermThreads.Signaled())
	{
		TRACE(L"AddFile aborting\r\n");
		return false;
	}

	spWiseFile pNewFile (new CWiseFile);
	if (pNewFile == nullptr)
	{
		return false;
	}

	if (FWF_SUCCESS != pNewFile->Attach( pszFilename))
	{
		TRACE(L"AddFile skipping %s\r\n", pszFilename);
	}
	else
	{
		if (m_Mutex == NULL)
		{
			// get out
			TRACE(L"AddFile aborting: mutex is gone\r\n");
			return false;
		}

		// Add the SAME file pointer to 2 lists, one that is the view's list
		// and one that is the list of files for the thread to act on
		AddToMain(pNewFile);
		AddToDirty(pNewFile);

		InterlockedIncrement(&m_dwDirtyCRC);
		InterlockedIncrement(&m_dwDirtyInfo);

		SetModifiedFlag( true );
		UpdateAllViews( NULL, HINT_ADD, pNewFile.get());
		
		m_fSentHintInfo = false;
		m_fSentHintCRC = false;

		// set the pathname
		SetPathName(pszFilename, false);
	}
	return true;
}

bool CMyDoc::RemoveFromMain( spWiseFile pFileInfo )
{
	TRACE(L"CMyDoc::RemoveFromMain\n");
	
	if (pFileInfo==nullptr || m_FileList.isEmpty())
		return false;
	
	CSingleLock m_sLock( &m_Mutex );
	while (!m_sLock.Lock(MTX_LOCK));
		m_FileList.Remove(pFileInfo);
	m_sLock.Unlock();
	
	return true;
}

bool CMyDoc::AddToMain( spWiseFile pFileInfo )
{
	TRACE(L"CMyDoc::AddToMain\n");
	
	if (pFileInfo == nullptr)
		return false;

	CSingleLock m_sLock( &m_Mutex );
	while (!m_sLock.Lock(MTX_LOCK));
		m_FileList.AddTail(pFileInfo);
	m_sLock.Unlock();

	m_qwSize += pFileInfo->GetSize();
	return true;
}

bool CMyDoc::CreateThreads()
{
	TRACE(L"CMyDoc::CreateThreads()\n");
	
	g_eTermThreads.Create( L"TermThreads");
	g_eTermThreads.Reset();
	g_eGoThreadInfo.Create(L"GoThreadInfo");
	g_eGoThreadCRC.Create(L"GoThreadCRC");
	
	// Create the Threads
	g_hThreadInfo=CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)UpdateThreadInfo, this, THREAD_PRIORITY, &g_dwThreadInfo);
	if (g_hThreadInfo == NULL)
	{
		return false;
	}
	
	g_hThreadCRC=CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)UpdateThreadCRC, this, THREAD_PRIORITY, &g_dwThreadCRC);
	if (g_hThreadCRC == NULL)
	{
		return false;
	}

	return true;
}

void CMyDoc::RemoveAllMain()
{
	TRACE(L"CMyDoc::RemoveAllMain()\n");
	if (m_FileList.isEmpty())
		return;
	
	int i=1;
	while ( !m_FileList.isEmpty() )
	{
		TRACE(L"\tCMyDoc::RemoveAllMain: %lu\n",i++);
		m_KillList.AddHead(m_FileList.RemoveHead());
	}
}

void CMyDoc::DeleteMainList()
{
	TRACE(L"CMyDoc::DeleteMainList\n");
	m_FileList.RemoveAll();
}

void CMyDoc::TerminateThreads()
{
	TRACE(L"CMyDoc::TerminateThreads()\n");

	// Unlock the lists
	CSingleLock m_sLock(&m_Mutex);
	m_sLock.Unlock();

	// Tell Threads To Die and resume them
	g_eTermThreads.Signal();
	g_eGoThreadCRC.Signal();
	g_eGoThreadInfo.Signal();
	
	if (g_hThreadCRC)
	{
		// Wait for CRC thread to terminate
		for (int i=0; i< THREAD_TRY; i++)
		{
			TRACE(L"THREADMAIN: CRC thread terminating\n");
			g_eGoThreadCRC.Signal();
			if (WAIT_OBJECT_0==WaitForSingleObject (g_hThreadCRC, THREAD_WAIT))
				break;
		}
		if (CloseHandle(g_hThreadCRC) == FALSE)
		{
			TerminateThread(g_hThreadCRC, (DWORD)-1);
			TRACE(L"THREADMAIN: CRC thread nuked\n");
		}
		else
		{
			TRACE(L"THREADMAIN: CRC thread died\n");
		}
	}
	else
	{
		TRACE(L"THREADMAIN: CRC thread already closed\n");
	}
	g_hThreadCRC = NULL;

	
	if (g_hThreadInfo)
	{
		// Wait for Info thread to terminate
		for (int i=0; i<THREAD_TRY; i++)
		{
			TRACE(L"THREADMAIN: Info thread terminating\n");
			g_eGoThreadInfo.Signal();
			if (WAIT_OBJECT_0==WaitForSingleObject (g_hThreadInfo, THREAD_WAIT))
				break;
		}
		
		if (CloseHandle(g_hThreadInfo) == FALSE)
		{
			TerminateThread(g_hThreadInfo, (DWORD)-1);
			TRACE(L"THREADMAIN: Info thread nuked\n");
		}
		else
		{
			TRACE(L"THREADMAIN: Info thread died\n");
		}
	}
	else
	{
		TRACE(L"THREADMAIN: Info thread already closed\n");
	}
	g_hThreadInfo = NULL;

	
	g_eTermThreads.Close();
	g_eGoThreadCRC.Close();
	g_eGoThreadInfo.Close();
}

void CMyDoc::OnCloseDocument() 
{
	TRACE(L"THREADMAIN: OnCloseDocument()\n");
	m_fLastTime = true;
	m_bAutoDelete = true;

	TerminateThreads();

	CDocument::OnCloseDocument();
}

void CMyDoc::SafeUpdateAllViews(CView* pSender, LPARAM lHint, CObject* pHint)
{
	if (pSender == nullptr || !m_viewList.IsEmpty())
	{
		return;
	}

	POSITION pos = GetFirstViewPosition();
	CView* pView = NULL;
	while (pos != NULL)
	{
		pView = GetNextView(pos);
		if (pView != pSender)
			pView->PostMessage(theApp.WM_UPDATEVIEW(), lHint, (LPARAM) pHint);
	}
}

bool CMyDoc::GetHeaderString(LPWSTR pszBuf)
{
	if (NULL == pszBuf)
	{
		return false;
	}

	// Get the View
	const CMainFrame* pFrame = static_cast<CMainFrame*> (theApp.GetMainWnd());
	CMyListView* pView = static_cast<CMyListView*> (pFrame->GetActiveView());
	CColumnInfo* pci= pView->GetColumnInfo();
	if (NULL == pci)
	{
		return false;
	}

	WCHAR szText[LIST_MAXHEADLENGTH];
	lstrinit(szText);
	*pszBuf = 0;

	for (int i = 0; i <  LIST_NUMCOLUMNS; i++)
	{
		if (pci[i].IsVisible())
		{
			LoadString(AfxGetResourceHandle(), pci[i].GetLabelID(), szText, LIST_MAXHEADLENGTH);
			lstrcat( pszBuf, szText );
			lstrcat( pszBuf, L"\t");
		}
	}

	LPWSTR pszTab = lstrrchr(pszBuf, NULL, '\t');
	*pszTab = 0;

	return true;
}

bool CMyDoc::GetRowString(CWiseFile& rFile, LPWSTR pszBuf)
{
	if (NULL == pszBuf)
	{
		return false;
	}

	// Get the View
	const CMainFrame* pFrame = dynamic_cast<CMainFrame*> (theApp.GetMainWnd());
	CMyListView* const pView = dynamic_cast<CMyListView*> (pFrame->GetActiveView());
	CColumnInfo* pci = pView->GetColumnInfo();
	DWORD cbItem = sizeof(rFile);
	LPWSTR pszText = (LPWSTR) HeapAlloc(GetProcessHeap(), 0, (cbItem + 1) );
	if (pszText == nullptr)
	{
		return false;
	}

	*pszBuf = 0;
	*pszText = 0;

	for (int i = 0; i <  LIST_NUMCOLUMNS - 1; i++)
	{
		if (pci[i].IsVisible())
		{
			rFile.GetFieldString(pszText, i, false);
			lstrcat( pszBuf, pszText );
			lstrcat( pszBuf, L"\t");
		}
	}

	HeapFree(GetProcessHeap(), 0, (LPVOID) pszText);

	return true;
}

#define UPDC32(octet,crc) (crc_32_tab[((crc)^ (octet)) & 0xff] ^ ((crc) >> 8))

void InitCRCMemory()
{
	if ((NULL == g_pBuf) || (0 == g_dwChunk))
	{
		MEMORYSTATUSEX ms;
		ZeroMemory( &ms, sizeof(MEMORYSTATUSEX) );
		ms.dwLength=sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx( &ms );
		DWORD const dwIdeal = (DWORD) (ms.ullAvailPhys / 10);

		g_dwChunk = 0;
		while (g_dwChunk < dwIdeal)
		{
			g_dwChunk += SIZEMEG;
		}

		if (g_dwChunk > (32 * SIZEMEG))
		{
			g_dwChunk = (32 * SIZEMEG);
		}
		
		//TODO: Research the ideal size for this buffer
		TRACE(L">>> InitCRCMemory. Chunk Size %lu\n", g_dwChunk);
		g_pBuf = (LPBYTE) HeapAlloc(GetProcessHeap(), 0, g_dwChunk);
	}

	if (NULL == g_pBuf)
	{
		ALERT(true, L"HeapAlloc failure.");
	}

	return;
}

void FreeCRCMemory()
{
	if (!HeapFree(GetProcessHeap(), 0, g_pBuf))
	{
		ALERT(true, L"HeapFree failure.");
	}
	g_dwChunk = 0;
	g_pBuf = NULL;

	return;
}

static DWORD crc_32_tab[] =
{ /* CRC polynomial 0xedb88320 */
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
	0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
	0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
	0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
	0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
	0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
	0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
	0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
	0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
	0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
	0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
	0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
	0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
	0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
	0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
	0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
	0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
	0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
	0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
	0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
	0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
	0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
	0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
	0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
	0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
	0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
	0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
	0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
	0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
	0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
	0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

DWORD UpdateThreadCRC( LPVOID pParam )
{
	TRACE(L"THREADCRC: begin\n");

	if (NULL == g_pBuf)
	{
		TRACE(L"THREADCRC: InitCRCMemory()\n");
		InitCRCMemory();

		if (NULL == g_pBuf)
		{
			TRACE(L"THREADCRC: InitCRCMemory() failed\n");
			return 0x00000000;
		}
	}

	if (NULL == pParam)
	{
		TRACE(L"THREADCRC: Couldn't get list of files\n");
		return 0x00000000;
	}
	CMyDoc* pDoc = static_cast<CMyDoc*> (pParam);

	// Now that we have the basic things we need, declare variables
	// TODO: Try to reduce the number of allocations
	spWiseFile pFileInfo;
	unsigned long crc32 = 0;
	BYTE* pchCurrent = nullptr;
	HANDLE hFile = NULL;
	DWORD dwAttribs = 0;
	QWORD qwBytesRemaining = 0;
	QWORD qwSize = 0;
	DWORD dwBytesRead = 0;
	DWORD dwError = 0;
	unsigned int nChunk = 0;
	LARGE_INTEGER li;
	ZeroMemory(&li, sizeof(LARGE_INTEGER));

	// this loop keeps this thread alive until the first if statement below breaks
	while (true)
	{
		g_eGoThreadCRC.Wait(INFINITE);
		if (g_eTermThreads.Signaled())
		{
			return (DWORD)-1;
		}

		// this loop keeps this thread *working* until it can't find files to work on
		// at which point it Resets the event and the Wait() below waits
		while (pDoc->IsDirty())
		{
			g_eGoThreadCRC.Wait(INFINITE);
			if (g_eTermThreads.Signaled())
			{
				return (DWORD)-1;
			}

			// Get something valid off the list
			pFileInfo = pDoc->RemoveDirtyHead();
			if (pFileInfo == nullptr)
			{
				TRACE(L"THREADCRC: null pFileInfo\n");
				break;
			}

			if (pFileInfo->CheckState( FWFS_DELETE ))
			{
				//TODO: See if this ever fires. If it doesn't then take this check out
				TRACE(L"THREADCRC: killing %s\n", pFileInfo->GetFullPath());
				pDoc->AddToKill(pFileInfo);
				break;
			}

			if (!pFileInfo->CheckState(FWFS_VERSION))
			{
				pDoc->AddToDirty(pFileInfo);
				TRACE(L"THREADCRC:  skipping %s\n", pFileInfo->GetFullPath());
				break;
			}

			if (pFileInfo->CheckState(FWFS_CRC_COMPLETE))
			{
				// don't need to add it to the dirty, cause if we got here, it has a version and doesn't need a CRC.
				TRACE(L"THREADCRC:  %s already has a CRC\n", pFileInfo->GetFullPath());
				break;
			}

			// Initialize Stuff
			crc32 = 0xFFFFFFFF;
			pchCurrent = NULL;
			hFile = INVALID_HANDLE_VALUE;
			dwAttribs = FILE_ATTRIBUTE_NORMAL;
			qwBytesRemaining = 0;
			qwSize = 0;
			dwBytesRead = 0;
			dwError = 0;
			nChunk = 0;

			pDoc->ChangeItemState(pFileInfo, FWFS_CRC_WORKING);

			// we only get to this point if we have found something on the dirty list to work on
			TRACE(L"THREADCRC: setting %s\n",pFileInfo->GetFullPath());

			// Open the file
			hFile = CreateFile(pFileInfo->GetFullPath(),
							GENERIC_READ,
							FILE_SHARE_READ,
							NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL, NULL);

			if ( INVALID_HANDLE_VALUE == hFile)
			{
				pDoc->ChangeItemState(pFileInfo, FWFS_CRC_ERROR);
				TRACE(L"THREADCRC: Invalid file handle.\n");
				break;
			}

			// Start generating chunks
			// While there are still chunks as big as g_dwChunk left
			qwBytesRemaining = qwSize = pFileInfo->GetSize64();
			dwBytesRead = (DWORD) min(g_dwChunk, qwBytesRemaining);
			for (qwBytesRemaining = qwSize;
				qwBytesRemaining > 0;
				qwBytesRemaining -= dwBytesRead)
			{
				// if we need to stop, or the file is being removed from the list
				if ( (g_eTermThreads.Signaled()) || (pFileInfo->CheckState( FWFS_DELETE )) )
				{
					break;
				}

				li.QuadPart = (nChunk * g_dwChunk);
				if (0 == SetFilePointerEx(hFile, li, NULL, FILE_BEGIN))
				{
					// failure
					pDoc->ChangeItemState(pFileInfo, FWFS_CRC_ERROR);
					break;
				}
				
				// read a chunk (or just what's remaining)
				dwBytesRead = (DWORD) min(g_dwChunk, qwBytesRemaining);
				TRACE(L"THREADCRC: Requesting %lu bytes\n",dwBytesRead);
				if (false == ReadFile(hFile, g_pBuf, min(g_dwChunk, ((DWORD)qwBytesRemaining)), &dwBytesRead, NULL))
				{
					pDoc->ChangeItemState(pFileInfo,FWFS_CRC_ERROR);
					break;
				}
				TRACE(L"THREADCRC: Read %lu bytes\n",dwBytesRead);

				pDoc->IncSizeRead(dwBytesRead);

				// calculate the CRC
				for (pchCurrent = g_pBuf; pchCurrent < (g_pBuf+dwBytesRead); pchCurrent++)
				{
					crc32 = UPDC32( *pchCurrent, crc32);
				}
				nChunk++;
			} // for-loop, until end-of-file

			// Close the file
			CloseHandle( hFile );

			// set the CRC
			pFileInfo->_SetCRC(~crc32);
			pDoc->ChangeItemState(pFileInfo,FWFS_CRC_COMPLETE);

			// the file has version information and has CRC information
			// so it does not need to be added to the dirty list
		} // while files still need CRCs

		// this point is only reached if the while loop above cannot find a file that it needs to work on
		if (pDoc->m_dwDirtyCRC < 1)
		{
			TRACE(L"THREADCRC: pausing: no files to work on\n");
			g_eGoThreadCRC.Reset();
		}
	} // while true

	// this point is reached iff the outer while loop detects that it's time to terminate threads
	// it then breaks, and we return
	TRACE(L"THREADCRC: exiting\n");
	return 0;
}

void CMyDoc::Shutdown()
{
	m_fLastTime = true;
}

bool CMyDoc::GetFileName(LPWSTR pszFile)
{
	WCHAR szFile[_MAX_PATH];
	WCHAR szPath[_MAX_PATH];
	WCHAR szPostfix[_MAX_PATH];

	// figure out the postfix -- folder name or drive label
	lstrcpy(szPostfix, GetPathName());
	PathGetFolder(szPostfix);
	// If the containing folder is the root, get a name
	if (PathIsRootOnly(szPostfix))
	{
		GetVolumeLabel(szPostfix, szPostfix, _MAX_PATH);
	}
	else
	{
		lstrcpy(szPostfix, GetPathName());
		PathGetContainingFolder(szPostfix);
	}

	// User Default Folder
	const CMainFrame* pFrame = dynamic_cast<CMainFrame*> (theApp.GetMainWnd());
	const CMyListView* const pView = dynamic_cast<CMyListView*> (pFrame->GetActiveView());
	if (!pView->m_strSavePath.IsEmpty() && DoesFolderExist(pView->m_strSavePath))
	{
		lstrcpy(szPath, pView->m_strSavePath);
		PathAddBackslash(szPath);
		goto DONE;
	}

	// No User Default Folder
	// Last Add location
	if (!GetPathName().IsEmpty())
	{
		lstrcpy(szPath, GetPathName());
		PathGetFolder(szPath);
		PathAddBackslash(szPath);
		if (DoesFolderExist(szPath) && PathIsWritable(szPath))
		{
			goto DONE;
		}
	}

	// No User Default Folder
	// No Last Add Location
	// Documents or Desktop
	if (!GetDocumentsFolder(szPath))
	{
		if (!GetDesktopFolder(szPath))
		{
			GetTempFolder(szPath);
		}
	}
	PathAddBackslash(szPath);

DONE:
	PathAddBackslash(szPath);
	if (!GetLogFileName(szPath, L"VFI", (LPCWSTR)szPostfix, (LPWSTR)szFile, L"csv"))
	{
		LoadString(AfxGetResourceHandle(), STR_INITFILE, szFile, _MAX_PATH);
		lstrcat(szPath, szFile);
		lstrcat(szPath, L".txt");
	}

	lstrcpy(pszFile, szFile);
	return true;
}

bool CMyDoc::WriteFileEx(LPCWSTR pszFile)
{
	const CMainFrame* pFrame = dynamic_cast<CMainFrame*> (AfxGetMainWnd());
	if (pFrame==nullptr)
		return false;
	
	CMyListView* pView = static_cast<CMyListView*> (pFrame->GetActiveView());
	if (pView==nullptr)
		return false;

	// New strategy
	// create a progress box
	// open the file
	// iterate through the list
	// get the data
	// write the data
	// update progress
	// close the file

	HANDLE hFile = CreateFile(
		pszFile,
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return false;

	// do the work here
	int c = 0;
	int j = 0;
	DWORD dw = 0;

	LPWSTR pwz = (LPWSTR) HeapAlloc(GetProcessHeap(), 0, 8192 * 2);
	if (NULL == pwz)
	{
		CloseHandle(hFile);
		return false;
	}

	CString	strText;
	strText.LoadString(STR_FILESAVING);
	pView->UpdateStatus(strText);

	const CListCtrl& theListCtrl = pView->GetListCtrl();
	c = theListCtrl.GetItemCount();

	// the reason we use the view is we write out the columns the user has chosen, not all the data
	
	// Why can't this be a spWiseFile (shared_ptr)?
	CWiseFile* pInfo = nullptr;
	for (j = 0; j < c; j++)
	{
		pInfo = reinterpret_cast<CWiseFile*> (theListCtrl.GetItemData(j));
		if (pInfo != nullptr)
		{
			GetRowString(*pInfo, pwz);
		}

		lstrcat( pwz, L"\r\n" );

		if (0 == ::WriteFile(hFile, (LPCVOID) pwz, lstrcb(pwz), &dw, NULL))
		{
			CloseHandle(hFile);
			::DeleteFile(pszFile);
			HeapFree(GetProcessHeap(), 0, (LPVOID) pwz);
			return false;
		}
		::SetFilePointer(hFile, 0, NULL, FILE_END);
	}

	HeapFree(GetProcessHeap(), 0, (LPVOID) pwz);
	CloseHandle(hFile);

	pView->ResetStatus();
	SetModifiedFlag(false);
	return true;
}
