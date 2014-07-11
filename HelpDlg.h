#ifndef HELPDLG_H
#pragma once
#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CHelpDlg dialog

class CHelpDlg : public CDialog
{
// Construction
public:
	CMemFile m_fileResource;
	bool m_fShowCheck;
	bool LoadRTF();
	bool LoadRTF(UINT nID);
//	CHelpDlg(CWnd* pParent = NULL);   // standard constructor
	CHelpDlg(CWnd* pParent = NULL, UINT nHelpID = 0, bool fShowCheck = false);

// Dialog Data
	//{{AFX_DATA(CHelpDlg)
	enum { IDD = IDD_RTFHELP };
	BOOL	m_fShowAgain;
	//}}AFX_DATA
	CRichEditCtrl m_RTF;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHelpDlg)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	static DWORD CALLBACK EditStreamInCallBack(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG* pcb);
	// Generated message map functions
	//{{AFX_MSG(CHelpDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	UINT m_nHelpID;
};

#define HELPDLG_H
#endif//HELPDLG_H
