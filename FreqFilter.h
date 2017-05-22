
#pragma once

// FreqFilter.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFreqFilter window
#include "dib.h"

class CFreqFilter : public CWnd
{
// Construction
public:
	CFreqFilter();
	CFreqFilter(CDIB* pDIB);

// Attributes
public:

// Operations
public:

// Implementation
public:
	void MinPower(float fD);
	void PSE_Filter(float fD);
	void WienerFilter(float fD);
	void RetrorseFilter(float fD);
	void MutualFilter(CRect* pRect, int nSum);
	void ButterworthH(float fD);
	void ButterworthL(float fD);
	virtual ~CFreqFilter();

	// Generated message map functions
protected:
	CDIB* m_pDIB;
	DECLARE_MESSAGE_MAP()
};