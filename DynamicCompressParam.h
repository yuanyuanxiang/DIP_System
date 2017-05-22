
#pragma once

// DynamicCompressParam.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDynamicCompressParam dialog
#include "DIP_SystemDoc.h"

class CDynamicCompressParam : public CDialog
{
// Construction
public:
	CDynamicCompressParam(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	enum { IDD = IDD_DLG_DYNAMICCOMPRESS };
	CSpinButtonCtrl	m_ctrlSpin;
	CSliderCtrl	m_ctrlSlider;
	int	m_nCValue;

// Overrides
// ClassWizard generated virtual function overrides
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
public:
	CDIP_SystemDoc* m_pDoc; 

protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnReleasedcaptureParamset(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSet();
	afx_msg void OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
};