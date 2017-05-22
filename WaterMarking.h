
#pragma once

// WaterMarking.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWaterMarking window
#include "dib.h"

class CWaterMarking : public CWnd
{
// Construction
public:
	CWaterMarking();
	CWaterMarking(CDIB* pDIB);

// Implementation
public:
	void Get(CFile &file);
	void Set();
	virtual ~CWaterMarking();

// Generated message map functions
protected:
	CDIB* m_pDIB;
	DECLARE_MESSAGE_MAP()
};
