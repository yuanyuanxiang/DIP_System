#if !defined(AFX_NOISE_H__85B8D222_37EC_11D6_B669_0010DCA65D26__INCLUDED_)
#define AFX_NOISE_H__85B8D222_37EC_11D6_B669_0010DCA65D26__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Noise.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNoise window
#include "dib.h"

class CNoise : public CWnd
{
// Construction
public:
	CNoise();
	CNoise(CDIB* pDIB);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNoise)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SaltNoise();
	void RandomNoise();
	virtual ~CNoise();

	// Generated message map functions
protected:
	CDIB* m_pDIB;
	//{{AFX_MSG(CNoise)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NOISE_H__85B8D222_37EC_11D6_B669_0010DCA65D26__INCLUDED_)
