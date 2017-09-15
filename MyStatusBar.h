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

#ifndef MYSTATUSBAR_H
#pragma once
#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyStatusBar window
class CMainFrame;
class CMyDoc;

class CMyStatusBar : public CStatusBar
{
public:
	CMyStatusBar();
	virtual ~CMyStatusBar();

	CMainFrame* GetFrame()
	{
		return (reinterpret_cast<CMainFrame*> (AfxGetMainWnd()));
	}

	CMyDoc* GetActiveDoc()
	{
		CFrameWnd* pFrame = reinterpret_cast<CFrameWnd*>(AfxGetMainWnd());
		return (reinterpret_cast<CMyDoc*>(pFrame->GetActiveDocument()));
	}

	CToolTipCtrl* GetTip()
	{
		return &m_Tip;
	}

	bool Setup();
	void SetPaneSize( UINT nPaneID, UINT nStringID );
	bool UpdateProgress(UINT nID, DWORD dwPos, DWORD dwLimit);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyStatusBar)
	//}}AFX_VIRTUAL

protected:
	bool AddBarTips();
	bool CreateProgressCtrl();

	//{{AFX_MSG(CMyStatusBar)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI);
	DECLARE_MESSAGE_MAP()

private:
	DWORD m_dwLimit1;
	DWORD m_dwLimit2;
	CToolTipCtrl m_Tip;
	CProgressCtrl m_ctlProgress1;
	CProgressCtrl m_ctlProgress2;
};

#define MYSTATUSBAR_H
#endif//MYSTATUSBAR_H

