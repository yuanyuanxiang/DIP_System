#if !defined(AFX_DLGSETANGLE_H__B04D3F21_4B8E_11D6_B669_0010DCA65D26__INCLUDED_)
#define AFX_DLGSETANGLE_H__B04D3F21_4B8E_11D6_B669_0010DCA65D26__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DLGSetAngle.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDLGSetAngle dialog

class CDLGSetAngle : public CDialog
{
// Construction
public:
	CDLGSetAngle(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDLGSetAngle)
	enum { IDD = IDD_DLG_SETANGLE };
	float	m_fAngle;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDLGSetAngle)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDLGSetAngle)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSETANGLE_H__B04D3F21_4B8E_11D6_B669_0010DCA65D26__INCLUDED_)

