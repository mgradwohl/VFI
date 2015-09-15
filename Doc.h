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

#ifndef DOC_H
#pragma once
#ifdef _DEBUG
	#define new DEBUG_NEW
#endif


void InitCRCMemory();
void FreeCRCMemory();

class CMyDoc : public CDocument
{
protected: // create from serialization only
	DECLARE_DYNCREATE(CMyDoc)
	CMyDoc();

// Attributes
public:
	CWiseFile* GetItem(int i)
	{
		return m_FileList.GetAt(m_FileList.FindIndex(i));
	}

	QWORD SizeRead()
	{
		return m_qwSizeRead;
	}

	void IncSizeRead(DWORD dw)
	{
		m_qwSizeRead += dw;
	}

	void TerminateThreads();
	volatile LONG m_dwDirtyInfo;
	volatile LONG m_dwDirtyCRC;
	bool m_fLastTime;
	void DeleteMainList();
	void RemoveAllMain();
	BOOL CreateThreads();
	BOOL AddToMain( CWiseFile* pFileInfo );
	BOOL RemoveFromMain( CWiseFile* pFileInfo );
	void DeleteKillList();
	void AddToKill( CWiseFile* pFileInfo );
	void ChangeItemState( CWiseFile* pFileInfo, WORD wState );
	BOOL AddToDirty( CWiseFile* pFileInfo );
	BOOL RemoveFromDirty( CWiseFile* pFileInfo );
	CWiseFile* RemoveDirtyHead();
	BOOL SuspendAllThreads();
	BOOL ResumeAllThreads();
	void RemoveAllDirty();
	DWORD RecurseDir( LPWSTR pszPath);
	//DWORD GetFiles (LPWSTR pszPath, LPWSTR pszFileName);
	
	int GetDirtyCount()
	{
		return (int) m_DirtyList.GetCount();
	}
	
	int GetItemCount()
	{
		return (int) m_FileList.GetCount();
	}
	
	QWORD GetTotalSize()
	{
		if (m_FileList.GetCount() == 0)
		{
			// No files, size must be 0
			m_qwSize=0;
		}
		return m_qwSize;
	}

	QWORD m_qwSize;
// Operations
public:
	BOOL DeleteFile( CWiseFile* pFileInfo );
	CMyObList m_FileList;
	CMyObList m_DirtyList;
	CMyObList m_KillList;

	CMyObList* GetList() {return &m_FileList;}

	static CMyDoc* GetDoc();
	BOOL AddFile(CString strFolder, CString strFileName);
	BOOL AddFile(LPCWSTR pszFileName);
	BOOL IsEmpty() { return m_FileList.IsEmpty(); }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnSaveDocument(LPCWSTR lpszPathName);
	virtual void DeleteContents();
	virtual void OnCloseDocument();
	protected:
	virtual BOOL SaveModified();
	//}}AFX_VIRTUAL

// Implementation
public:
	bool GetFileName(LPWSTR pszFile);
	//bool WriteFile(LPCWSTR pszFile);
	bool WriteFileEx(LPCWSTR pszFile);
	void Shutdown();
	inline bool IsDirty()
	{
#ifdef DEBUG
		TRACE (">>>  DIRTY LIST CHECK\r\n");
		TRACE1("     Dirty list: %lu items\r\n", m_DirtyList.GetCount());
		TRACE1("     Dirty CRC:  %lu\r\n", m_dwDirtyCRC);
		TRACE1("     Dirty Info: %lu\r\n", m_dwDirtyInfo);
#endif
		return ( m_dwDirtyCRC + m_dwDirtyInfo > 0 );
	}

	bool GetRowString(CWiseFile& rFile, LPWSTR pszBuf);
	bool GetHeaderString(LPWSTR pszBuf);

	inline bool CMyDoc::IsDirtyEmpty()
	{
		return (TRUE == m_DirtyList.IsEmpty());
	}

	void PauseAllThreads(bool fPause);
	virtual void SafeUpdateAllViews(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual ~CMyDoc();
	CMutex m_Mutex;
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	// Generated message map functions
protected:
	//{{AFX_MSG(CMyDoc)
	afx_msg void OnFileSaveas();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	bool m_fCheckPath;
	QWORD m_qwSizeRead;
	bool m_fPause;
	bool m_fSentHintCRC;
	bool m_fSentHintInfo;
	bool m_fFirstTime;
};

inline CMyDoc* CMyDoc::GetDoc()
{
	TRACE( L"CMyDoc::GetDoc()\n");
	CFrameWnd* pFrame = static_cast<CFrameWnd*> (AfxGetApp()->m_pMainWnd);
	return static_cast<CMyDoc*> (pFrame->GetActiveDocument());
}

DWORD UpdateThreadInfo (LPVOID pParam);
DWORD UpdateThreadCRC (LPVOID pParam);
/////////////////////////////////////////////////////////////////////////////
#define DOC_H
#endif//DOC_H
