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

#ifndef FRAME_H
#pragma once
#ifdef _DEBUG
	#ifdef DEBUG_NEW
		#define new DEBUG_NEW
	#endif
#endif

#include "MyStatusBar.h"
#include "doc.h"

class CMainFrame : public CFrameWnd
{
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:
	void SavePreferences();
	CMyStatusBar* GetStatusBar()
	{
		return static_cast<CMyStatusBar*> (&m_wndStatusBar);
	}

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	CMyDoc* GetDocument()
	{
		return (CMyDoc*)GetActiveDocument();
	}

	CFont* CMainFrame::GetUIFont()
	{
		return &m_fntUI;
	}

	void UpdateProgress();
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CFont m_fntUI;
	CMyStatusBar m_wndStatusBar;
	CToolBar m_wndToolBar;
	CImageList m_ilToolBar;
	CImageList m_ilToolBarGrey;

	BOOL m_fShowToolTips;
	static void CALLBACK EXPORT TimerProc(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime);

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUpdateViewTooltips(CCmdUI* pCmdUI);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnDestroy();
	afx_msg void OnViewTooltips();
	afx_msg void OnMyHelp();
	afx_msg void OnEndSession(BOOL bEnding);
	afx_msg void OnHelpISO();
	afx_msg void OnHelpAttribs();
	afx_msg void OnHelpFlags();
	afx_msg void OnHelpType();
	afx_msg void OnHelpOS();
	afx_msg void OnHelpCRC();
	afx_msg void OnHelpVersion();
	afx_msg void OnHelpLanguage();
	afx_msg void OnHelpCodepage();
	afx_msg void OnClose();
	//}}AFX_MSG
	afx_msg void OnUpdateCount(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSize(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBuffer(CCmdUI* pCmdUI);
	afx_msg void OnUpdateProgress(CCmdUI* pCmdUI);

	DECLARE_MESSAGE_MAP()

private:
	QWORD m_qwSize;
	int m_iCount;
};

#define FRAME_H
#endif//FRAME_H
