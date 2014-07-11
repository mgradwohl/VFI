#ifndef PROGRESSBOX_X
#pragma once
#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

class CProgressBox : public CDialog
{
// Construction
public:
	BOOL Create(CWnd* pParentWnd, WORD wAlign);
	CProgressBox(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProgressBox)
	enum { IDD = IDD_PROGRESS };
	CProgressCtrl	m_ctlProgress;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgressBox)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProgressBox)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	WORD m_wAlign;
};

#define PROGRESSBOX_X
#endif//PROGRESSBOX_X
