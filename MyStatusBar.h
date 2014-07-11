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
		CFrameWnd* pFrame = (CFrameWnd*)AfxGetMainWnd();
		return (reinterpret_cast<CMyDoc*> (pFrame->GetActiveDocument()));
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
	virtual int OnToolHitTest(CPoint point, TOOLINFO* pTI);
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

