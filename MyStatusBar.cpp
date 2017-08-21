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

// MyStatusBar.cpp : implementation file
//

#include "stdafx.h"
#include "VFI.h"
#include "frame.h"
#include "doc.h"
#include "MyStatusBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static CHAR THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyStatusBar

CMyStatusBar::CMyStatusBar()
{
	m_dwLimit1 = 0;
	m_dwLimit2 = 0;
}

CMyStatusBar::~CMyStatusBar()
{
}


BEGIN_MESSAGE_MAP(CMyStatusBar, CStatusBar)
	//{{AFX_MSG_MAP(CMyStatusBar)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX( TTN_NEEDTEXTA, 0, OnToolTipText )
	ON_NOTIFY_EX( TTN_NEEDTEXTW, 0, OnToolTipText )
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMyStatusBar message handlers

bool CMyStatusBar::AddBarTips()
{
	RECT rc;
	int nIndex = 0;

	EnableToolTips(TRUE);
	m_ctlProgress1.EnableToolTips(TRUE);
	m_ctlProgress2.EnableToolTips(TRUE);

	if (!m_Tip.AddTool(&m_ctlProgress1, LPSTR_TEXTCALLBACK, NULL, 0))
		return FALSE;

	if (!m_Tip.AddTool(&m_ctlProgress2, LPSTR_TEXTCALLBACK, NULL, 0))
		return FALSE;
	

	nIndex = CommandToIndex(ID_INDICATOR_COUNT);
	GetItemRect( nIndex, &rc );
	if (!m_Tip.AddTool(this, TIP_COUNT, &rc, ID_INDICATOR_COUNT))
		return FALSE;

	nIndex = CommandToIndex(ID_INDICATOR_SIZE);
	GetItemRect( nIndex, &rc );
	if (!m_Tip.AddTool(this, TIP_SIZE, &rc, ID_INDICATOR_SIZE))
		return FALSE;

	nIndex = CommandToIndex(ID_INDICATOR_BUFFER);
	GetItemRect( nIndex, &rc );
	if (!m_Tip.AddTool(this, TIP_BUFFER, &rc, ID_INDICATOR_BUFFER))
		return FALSE;

	return TRUE;
}

bool CMyStatusBar::CreateProgressCtrl()
{
	RECT rc;
	int nIndex;

	nIndex = CommandToIndex( ID_PROGRESS );
	GetItemRect( nIndex, &rc );
	SetPaneText(nIndex, L"", TRUE);
	SetPaneStyle(nIndex, SBPS_NORMAL);// | SBPS_DISABLED);

	// half the height of the pane
	int dY = abs(rc.bottom - rc.top) / 2;

	rc.bottom = rc.top + dY - 1;
	if (FALSE == m_ctlProgress1.Create( /*PBS_SMOOTH |*/ WS_CHILD | WS_VISIBLE, rc, this, ID_PROGRESS))
	{
		return false;
	}
	m_ctlProgress1.SetPos(0);
	m_ctlProgress1.SetStep(1);

	// move the 2nd control down
	rc.top += dY + 1;
	rc.bottom += dY + 1;
	if (FALSE == m_ctlProgress2.Create( /*PBS_SMOOTH |*/ WS_CHILD | WS_VISIBLE, rc, this, ID_PROGRESS))
	{
		return false;
	}
	m_ctlProgress2.SetPos(0);
	m_ctlProgress2.SetStep(1);

	m_ctlProgress1.SendMessage(PBM_SETBARCOLOR, 0, (LPARAM)(COLORREF) 0x0000ff00);
	m_ctlProgress2.SendMessage(PBM_SETBARCOLOR, 0, (LPARAM)(COLORREF) 0x000000ff);

	m_ctlProgress1.SendMessage(PBM_SETBKCOLOR, 0, (LPARAM)(COLORREF) 0x00000000);
	m_ctlProgress2.SendMessage(PBM_SETBKCOLOR, 0, (LPARAM)(COLORREF) 0x00000000);
	
	return true;
}

bool CMyStatusBar::Setup()
{
	 // create progress bars on the status bar
	if(!CreateProgressCtrl())
	{
		TRACE(L"Failed to create progress control\n");
		return false;
	}

	// Create a tool tip control
	if (!m_Tip.Create(this, TTS_NOPREFIX))
	{
		TRACE(L"Failed to create tooltip\n");
		return false;
	}

	if (!AddBarTips())
	{
		TRACE(L"Failed to add tooltips\n");
		return false;
	}

	return true;
}

void CMyStatusBar::SetPaneSize( UINT nPaneID, UINT nStringID )
{
	HGDIOBJ hOldFont = NULL;
	HFONT   hFont    = (HFONT)SendMessage(WM_GETFONT);

	CDC* pdc = GetDC();
	if (hFont != NULL) 
	{
		hOldFont = pdc->SelectObject(hFont);
	}

	CString strCount;
	strCount.Empty();
#pragma warning(suppress: 6031)
	strCount.LoadString(nStringID);
	CSize sz = pdc->GetTextExtent(strCount);
	if (hOldFont!=NULL) 
	{
		pdc->SelectObject(hOldFont);
	}

	// Done with the DC, release it
	ReleaseDC(pdc);

	UINT nID, nStyle;
	int nWidth;
	int nIndex = nIndex = CommandToIndex( nPaneID );
	GetPaneInfo(nIndex, nID, nStyle, nWidth);
	SetPaneInfo(nIndex, nID, nStyle, sz.cx);
}

bool CMyStatusBar::UpdateProgress(UINT nID, DWORD dwPos, DWORD dwLimit)
{
	if (nID == 1)
	{
		if (::IsWindow(m_ctlProgress1.m_hWnd))
		{
			if (dwLimit != m_dwLimit1)
			{
				m_ctlProgress1.SetRange32(0,dwLimit);
				m_dwLimit1 = dwLimit;
			}
			m_ctlProgress1.SetPos(dwPos);
		}
		return true;
	}

	if (nID == 2)
	{
		if (::IsWindow(m_ctlProgress2.m_hWnd))
		{
			if (dwLimit != m_dwLimit2)
			{
				m_ctlProgress2.SetRange32(0,dwLimit);
				m_dwLimit2 = dwLimit;
			}
			m_ctlProgress2.SetPos(dwPos);
		return true;
		}
	}

	return false;
}

void CMyStatusBar::OnSize(UINT nType, int cx, int cy) 
{
	CStatusBar::OnSize(nType, cx, cy);
	
	RECT rc;
	int nIndex;

	if (::IsWindow(m_hWnd))
	{
		if ( ::IsWindow(m_ctlProgress1.m_hWnd) && ::IsWindow(m_ctlProgress2.m_hWnd) )
		{
			nIndex = CommandToIndex( ID_PROGRESS );
			GetItemRect( nIndex, &rc );

			// half the height of the pane
			int dY = abs(rc.bottom - rc.top) / 2;

			rc.bottom = rc.top + dY - 1;
			m_ctlProgress1.MoveWindow( &rc, TRUE );

			// move the 2nd control down
			rc.top += dY + 1;
			rc.bottom += dY + 1;
			m_ctlProgress2.MoveWindow( &rc, TRUE );
		}
		if (::IsWindow(m_Tip.m_hWnd))
		{
			// Tell the tooltip where the panes are now
			nIndex = CommandToIndex(ID_INDICATOR_COUNT);
			GetItemRect( nIndex, &rc );
			m_Tip.SetToolRect(this, ID_INDICATOR_COUNT, &rc );

			nIndex = CommandToIndex(ID_INDICATOR_SIZE);
			GetItemRect( nIndex, &rc );
			m_Tip.SetToolRect(this, ID_INDICATOR_SIZE, &rc);

			nIndex = CommandToIndex(ID_INDICATOR_BUFFER);
			GetItemRect( nIndex, &rc );
			m_Tip.SetToolRect(this, ID_INDICATOR_BUFFER, &rc);
		}
	}
}

INT_PTR CMyStatusBar::OnToolHitTest(CPoint point, TOOLINFO* pTI)
{
	ASSERT(FALSE);
	CRect rc1;
	CRect rc2;
	m_ctlProgress1.GetWindowRect(&rc1);
	m_ctlProgress2.GetWindowRect(&rc2);

	pTI->uFlags |= TTF_IDISHWND;

	if (rc1.PtInRect(point))
	{
		pTI->rect = rc1;
		pTI->uId = (UINT_PTR)m_ctlProgress1.m_hWnd;
		return 1;
	}
	if (rc2.PtInRect(point))
	{
		pTI->rect = rc2;
		pTI->uId = (UINT_PTR)m_ctlProgress2.m_hWnd;
		return 1;
	}
	return CStatusBar::OnToolHitTest(point, pTI);
}

BOOL CMyStatusBar::OnToolTipText( UINT id, NMHDR* pNMHDR, LRESULT* pResult )
{
	id;
	CMyDoc* pDoc = GetFrame()->GetDocument();
	ASSERT(pDoc);
	if (pDoc == nullptr)
	{
		return FALSE;
	}
	ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;

	WCHAR szRead[128];
	WCHAR szTotal[128];


	DWORD dw = pDoc->GetItemCount();
	StrFormatByteSize64(pDoc->m_qwSize, szTotal, 128);
	StrFormatByteSize64(pDoc->SizeRead(), szRead, 128);
	
	UINT_PTR nID = pNMHDR->idFrom;

	WCHAR szTip[128];
	szTip[0] = '\0';

	if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
		pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
	{
		if (nID == (UINT_PTR)m_ctlProgress1.m_hWnd)
		{
			strfmt(AfxGetResourceHandle(), szTip, TIP_PROGRESS_INFO, dw - pDoc->m_dwDirtyInfo, dw);
		}
		if (nID == (UINT_PTR)m_ctlProgress2.m_hWnd)
		{
			strfmt(AfxGetResourceHandle(), szTip, TIP_PROGRESS_CRC, dw - pDoc->m_dwDirtyCRC, dw, szRead, szTotal);
		}
		wcscat_s(szTip, 128, L"\0");

		if (pNMHDR->code == TTN_NEEDTEXTW)
			wcsncpy_s(pTTTW->szText, 128, szTip, _countof(pTTTW->szText));

		*pResult = 0;
		// bring the tooltip window above other popup windows
		::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,
			SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_NOOWNERZORDER);

		return TRUE;
	}
	return FALSE;
}


