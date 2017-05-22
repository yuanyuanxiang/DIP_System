#if !defined(AFX_DLGSETFREQ_H__DCA407E2_13C9_11D6_B669_0010DCA65D26__INCLUDED_)
#define AFX_DLGSETFREQ_H__DCA407E2_13C9_11D6_B669_0010DCA65D26__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DLGSetFreq.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDLGSetFreq dialog

class CDLGSetFreq : public CDialog
{
// Construction
public:
	CDLGSetFreq(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDLGSetFreq)
	enum { IDD = IDD_DLG_SETFREQ };
	float	m_fValue;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDLGSetFreq)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDLGSetFreq)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSETFREQ_H__DCA407E2_13C9_11D6_B669_0010DCA65D26__INCLUDED_)
