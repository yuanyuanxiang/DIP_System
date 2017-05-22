
#pragma once

/////////////////////////////////////////////////////////////////////////////
// CDlgPointDT dialog
#include "DIP_SystemDoc.h"

class CDlgPointDT : public CDialog
{
// Construction
public:
	CDlgPointDT(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	enum { IDD = IDD_DLG_POINTDT };
		
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
public:
	CDIP_SystemDoc* m_pDoc; 

	// 各个灰度值的计数
	float m_fIntensity[256];

protected:
	// 当前鼠标拖动状态，0表示未拖动，1表示正在拖动下限，2表示正在拖动上限。
	int		m_iIsDraging;
	
	// 相应鼠标事件的矩形区域
	CRect	m_MouseRect;

	// 窗口的下限
	BYTE	m_bLow;
	
	// 窗口的上限
	BYTE	m_bUp;

protected:
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
};