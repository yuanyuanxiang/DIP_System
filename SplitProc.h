#if !defined(AFX_SPLITPROC_H__D4B2F665_FD10_11D5_B668_0010DCA65D26__INCLUDED_)
#define AFX_SPLITPROC_H__D4B2F665_FD10_11D5_B668_0010DCA65D26__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SplitProc.h : header file
//

#include "dib.h"

/////////////////////////////////////////////////////////////////////////////
// CSplitProc window

class CSplitProc : public CWnd
{
// Construction
public:
	CSplitProc();
	CSplitProc(CDIB* pDIB);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplitProc)
	//}}AFX_VIRTUAL

// Implementation
public:
	void Threshold(int *nNY);
	void TraceDIB(COLORREF clrSeed);
	void ContourDIB(COLORREF clrSeed_I,COLORREF clrSeed_II);
	void Kirsch();
	void Hough();
	void GuassLaplacian();
	void Laplacian();
	void Roberts();
	void Prewitt();
	void Sobel();
	void RgnGrow(COLORREF clrSeed_I,COLORREF clrSeed_II);
	virtual ~CSplitProc();

	// Generated message map functions
protected:
	CDIB* m_pDIB;
	//{{AFX_MSG(CSplitProc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPLITPROC_H__D4B2F665_FD10_11D5_B668_0010DCA65D26__INCLUDED_)
