
#pragma once

// SpaceFilter.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSpaceFilter window
#include "dib.h"
#include "KernelDef.h"

class CSpaceFilter : public CWnd
{
// Construction
public:
	CSpaceFilter();
	CSpaceFilter(CDIB* pDIB);

// Implementation
public:
	void OverRun(LPKERNEL lpKernel, int T);
	void MedianFilter();
	void StencilFilter(LPKERNEL lpKernel);
	void MakeEmboss(int nEmbossMode);
	virtual ~CSpaceFilter();

// Generated message map functions
protected:
	CDIB* m_pDIB;

	void PixelFilter(BYTE* lpDst,BYTE* lpSrc,LPKERNEL lpKernel,DWORD dwWidthBytes,BOOL bEmboss);
	DECLARE_MESSAGE_MAP()
};
