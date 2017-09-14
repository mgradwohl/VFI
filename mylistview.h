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

// mylistview.h : interface of the CMyListView class
//
/////////////////////////////////////////////////////////////////////////////
#ifndef MYLISTVIEW_H
#pragma once
#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

#include "columninfo.h"

class CMyDoc;

class CMyListView : public CListView
{
protected: // create from serialization only
	CMyListView();
	DECLARE_DYNCREATE(CMyListView)

public:
	void ShowCmdHelp();
	HGLOBAL GetBufferHandle();
	SIZE_T GetBufferSize();
	LPWSTR GetBuffer();
	void CloseBuffer();
	bool FillBuffer(bool fAllRows, bool fAllFields);
	WCHAR m_szFolder[_MAX_PATH];
	WCHAR m_szCmdLineOutput[_MAX_PATH];
	bool GetCommandLineFolder(LPWSTR pszFolder);
	bool PromptToSave();
	bool GetItem(int nItem, POINT pt);
	int FindVisibleItem(CObject* pObject);
	void UpdateStatus(LPCWSTR pszStatus);
	void ResetStatus();
	CColumnInfo* GetColumnInfo();
	virtual ~CMyListView();
	CString	m_strSavePath;
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	// misc
	void UpdateWidths();

	// Item Stuff
	bool AddItem(CWiseFile* pFileInfo);
	bool DeleteItem( int iItem );
	bool DeleteItem( CObject* pObject);
	bool RedrawItem( int iItem );
	bool RedrawItem( CObject* pObject );

	// List stuff
	void EmptyList();
	void UpdateDirty();

	// Preferences
	bool SavePreferences();
	bool RestorePreferences();
	CMyDoc* GetDocument();

	// Window stuff
	LRESULT MyGetExStyle();
	void MyModifyStyleEx( LRESULT dwRemove, LRESULT dwAdd);
	bool SetViewType(DWORD dwViewType);
	DWORD GetViewType();
	bool InitColumns();
	void UnMarkColumn();
	void MarkColumn();

	static int CALLBACK ListViewCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamView);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyListView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Generated message map functions
protected:
	//{{AFX_MSG(CMyListView)
	afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
	afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAddButtload();
	afx_msg void OnViewOptions();
	afx_msg void OnUpdateFileNew(CCmdUI* pCmdUI);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnFileRename();
	afx_msg void OnUpdateFileRename(CCmdUI* pCmdUI);
	void OnFileTouch();
	afx_msg void OnUpdateFileTouch(CCmdUI* pCmdUI);
	afx_msg void OnEditSelectAll();
	afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnFileAdd();
	afx_msg void OnDestroy();
	afx_msg void OnFileProperties();
	afx_msg void OnUpdateFileProperties(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnViewSmartFit();
	afx_msg void OnUpdateViewSmartFit(CCmdUI* pCmdUI);
	afx_msg void OnEditRemove();
	afx_msg void OnUpdateEditRemove(CCmdUI* pCmdUI);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	LPWSTR m_pBuf;
	HGLOBAL m_hBuf;
	CBitmap m_bmBackground;
	bool m_fAutomatic;
	CImageList m_imlView;
	CFont m_fntView;
	bool m_fCrashOnPause;
	CFont m_fntHeader;
	CFont m_fntMono;

	bool m_fSortAscend;
	bool m_fHexSize;
	bool m_fHexLanguage;
	bool m_fLocalTime;
	bool m_fLockHeaderWidth;
	bool m_fSavePrompt;

public:
	bool m_fIncludePath;
private:
	int m_nMonoX;
	int m_nMonoY;

	// For sorting
	static int TimeCompare( CTime time1, CTime time2, bool fSortAscending );
	static int WordCompare( WORD w1, WORD w2, bool fSortAscending );
	static int DWordCompare( DWORD dw1, DWORD dw2, bool fSortAscending );
	static int StringCompare( CString str1, CString str2, bool fSortAscending );
	static int QWordCompare( QWORD qw1, QWORD qw2, bool fSortAscending);
	int m_iSortColumn;

	// Columns, styles and window collateral
	CColumnInfo* m_pci;
	DWORD m_dwExStyle;
	DWORD m_dwViewStyle;
	HICON m_hSmallIcon;
	HICON m_hLargeIcon;
	CWnd* m_pBox;

	// Other Class members
	bool m_fFirstTime;
	int m_iLastItem;

	// Preferences
	bool m_fAllowDupes:1;
	bool m_fAllowFolders:1;
	bool m_fAudioCue:1;
	bool m_fAutoFit:1;
	CString	m_strWave;
public:
	WCHAR m_szItemText[1024];
};

#ifndef _DEBUG  // debug version in mylistview.cpp
inline CMyDoc* CMyListView::GetDocument()
   { return reinterpret_cast<CMyDoc*> (CListView::GetDocument()); }
#endif

inline bool CMyListView::SetViewType(DWORD dwViewType)
{
	return ( TRUE == ModifyStyle(LVS_TYPEMASK, dwViewType & LVS_TYPEMASK));
}

inline DWORD CMyListView::GetViewType()
{
	return(GetStyle() & LVS_TYPEMASK);
}

inline void CMyListView::EmptyList()
{
	TRACE( L">>> CMyListView::EmptyList\r\n");
	GetListCtrl().DeleteAllItems();
}

inline LRESULT CMyListView::MyGetExStyle()
{
	CListCtrl& theListCtrl=GetListCtrl();
	return theListCtrl.SendMessage (LVM_GETEXTENDEDLISTVIEWSTYLE);
}

inline CColumnInfo* CMyListView::GetColumnInfo()
{
	return m_pci;
}

inline int CMyListView::StringCompare(CString str1, CString str2, bool fSortAscending)
{
	int i;

	if (fSortAscending)
	{
		i=CompareString(LOCALE_USER_DEFAULT, 0, str1, -1, str2, -1);
		//return lstrcmpi( str1, str2 );
	}
	else
	{
		i=CompareString(LOCALE_USER_DEFAULT, 0, str2, -1, str1, -1);
		//return lstrcmpi( str2, str1 );
	}
	return i-2;
}

inline int CMyListView::WordCompare( WORD w1, WORD w2, bool fSortAscending)
{
	int iResult=0;
	if (fSortAscending)
	{
		iResult = w1 - w2;
		return iResult;
	}

	iResult = w2 - w1;
	return iResult;
}

inline int CMyListView::DWordCompare( DWORD dw1, DWORD dw2, bool fSortAscending)
{
	int iResult=0;
	if (fSortAscending)
	{
		iResult= HIWORD(dw1) - HIWORD(dw2);
		if (iResult == 0)
		{
			iResult= LOWORD(dw1) - LOWORD(dw2);
		}
	}
	else
	{
		iResult= HIWORD(dw2) - HIWORD(dw1);
		if (iResult == 0)
		{
			iResult= LOWORD(dw2) - LOWORD(dw1);
		}
	}
	return iResult;
}

inline int CMyListView::QWordCompare( QWORD qw1, QWORD qw2, bool fSortAscending)
{
	int iResult=0;
	if (fSortAscending)
	{
		iResult= HIDWORD(qw1) - HIDWORD(qw2);
		if (iResult == 0)
		{
			iResult= LODWORD(qw1) - LODWORD(qw2);
		}
	}
	else
	{
		iResult= HIDWORD(qw2) - HIDWORD(qw1);
		if (iResult == 0)
		{
			iResult= LODWORD(qw2) - LODWORD(qw1);
		}
	}
	return iResult;
}

inline void CMyListView::OnUpdateViewSmartFit(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}

inline void CMyListView::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	CListCtrl& theListCtrl = GetListCtrl();
	pCmdUI->Enable(theListCtrl.GetSelectedCount() > 0);	
}

inline int CMyListView::TimeCompare( CTime time1, CTime time2, bool fSortAscending )
{
	int iResult=0;
	if (time1 < time2)
	{
		iResult = -1;
	}
	else
	if (time1 > time2)
	{
		iResult = 1;
	}

	if (fSortAscending)
	{
		return iResult;
	}
	return -iResult;
}

/////////////////////////////////////////////////////////////////////////////
#define MYLISTVIEW_H
#endif//MYLISTVIEW_H
