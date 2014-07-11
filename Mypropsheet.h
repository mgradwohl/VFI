// MyPropSheet.h : header file
//
// This class defines custom modal property sheet 
// CMyPropSheet.
 
#ifndef MYPROPSHEET_H
#pragma once
#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

#include "MyPropPage1.h"

/////////////////////////////////////////////////////////////////////////////
// CMyPropSheet

class CMyPropSheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CMyPropSheet)

// Construction
public:
	CMyPropSheet(CWnd* pParentWnd = NULL);

// Attributes
public:
	CPageGeneral m_PageGeneral;
	CPageColumn  m_PageColumn;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyPropSheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMyPropSheet();

// Generated message map functions
protected:
	//{{AFX_MSG(CMyPropSheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#define	MYPROPSHEET_H
#endif//MYPROPSHEET_H
