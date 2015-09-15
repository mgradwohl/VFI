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

#ifndef COLUMNINFO_H
#pragma once

#ifdef _DEBUG
	#ifdef DEBUG_NEW
		#define new DEBUG_NEW
	#endif
#endif

class CColumnInfo
{
public:
	CColumnInfo()
	{
		m_dx = 0;
		m_fVisible = true;
		m_idLabel = 0;
		m_iFmt = 0;
		m_dxFixed = 0;
	}

	virtual ~CColumnInfo()
	{
		m_dx = 0;
		m_fVisible = true;
		m_idLabel = 0;
		m_iFmt = 0;
		m_dxFixed = 0;
	}

	bool IsVisible()
		{ return m_fVisible; }
	void SetVisible(bool fVisible)
		{ m_fVisible = fVisible; }
	int GetWidth()
		{ return m_dx; }
	void SetWidth(int iWidth)
		{ m_dx = iWidth; }
	__forceinline int GetFixedWidth()
		{ return m_dxFixed; }
	void SetFixedWidth(int iWidth)
		{ m_dxFixed = iWidth; }
	bool IsFixedWidth()
		{ return m_dxFixed != 0 ;}
	UINT GetLabelID()
		{ return m_idLabel; }
	void SetLabelID(UINT idLabel)
		{ m_idLabel = idLabel; }
	int GetFormat()
		{ return m_iFmt; }
	void SetFormat(UINT iFormat)
		{ m_iFmt = iFormat; }

private:
	UINT	m_idLabel;
	int		m_iFmt;
	int		m_dx;
	int		m_dxFixed;		// non-zero means this is a fixed-width owner draw thing 
	bool	m_fVisible : 1;
};

#define COLUMNINFO_H
#endif//COLUMNINFO_H
