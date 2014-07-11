// MyPropSheet.cpp : implementation file
//

#include "stdafx.h"
#include "wndlib.h"
#include "resource.h"
#include "VFI.h"
#include "MyPropSheet.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static CHAR THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyPropSheet

IMPLEMENT_DYNAMIC(CMyPropSheet, CPropertySheet)

CMyPropSheet::CMyPropSheet(CWnd* pWndParent)
	 : CPropertySheet(IDS_PROPSHT_CAPTION, pWndParent)
{
	// Add all of the property pages here.  Note that
	// the order that they appear in here will be
	// the order they appear in on screen.  By default,
	// the first page of the set is the active one.
	// One way to make a different property page the 
	// active one is to call SetActivePage().

	AddPage(&m_PageGeneral);
	AddPage(&m_PageColumn);
}

CMyPropSheet::~CMyPropSheet()
{
}


BEGIN_MESSAGE_MAP(CMyPropSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CMyPropSheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyPropSheet message handlers
BOOL CMyPropSheet::OnInitDialog() 
{
	BOOL fRet=CPropertySheet::OnInitDialog();

	//ChangeDialogFont(this, theApp.GetUIFont(), CDF_CENTER);

	// change the font for each page
	CPropertyPage* pPage = NULL;
	for (int i = 0; i < GetPageCount (); i++)
	{
		VERIFY (SetActivePage(i));
		pPage = GetActivePage();
		ASSERT (pPage);
		//ChangeDialogFont(pPage, theApp.GetUIFont(), CDF_CENTER);

	}

	// set the default page active
	VERIFY (SetActivePage (0));

	return fRet;
}
