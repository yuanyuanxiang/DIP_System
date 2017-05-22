#if !defined(AFX_DLGINTENSITY_H)
#define AFX_DLGINTENSITY_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// DlgIntensity.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CDlgIntensity dialog

class CDlgIntensity : public CDialog
{
	// Construction
public:
	// 各个灰度值的计数
	float m_fIntensity[256];

	CDlgIntensity(CWnd* pParent = NULL);   // standard constructor

	// Dialog Data
	enum { IDD = IDD_DLG_INTENSITY };

	// Overrides
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_DLGINTENSITY_H)

