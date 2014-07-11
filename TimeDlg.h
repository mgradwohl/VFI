#ifndef TIMEDLG_H
#pragma once
#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

class CTimeDlg : public CDialog
{
// Construction
public:
	SYSTEMTIME m_stDate;
	SYSTEMTIME m_stTime;
	SYSTEMTIME m_stDateTime;
	CTimeDlg(CWnd* pParent = NULL);   // standard constructor
	void GetTime( SYSTEMTIME* pTime );
	void DateTimeMerge();

// Dialog Data
	//{{AFX_DATA(CTimeDlg)
	enum { IDD = IDD_TIMEBOX };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void AFXAPI DDX_DateTime(CDataExchange* pDX, int nIDC, SYSTEMTIME &SysTime);
	void AFXAPI DDX_Month(CDataExchange* pDX, int nIDC, SYSTEMTIME &SysTime);

	// Generated message map functions
	//{{AFX_MSG(CTimeDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#define TIMEDLG_H
#endif//TIMEDLG_H
