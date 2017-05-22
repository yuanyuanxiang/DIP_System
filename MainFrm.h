// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
	DECLARE_MESSAGE_MAP()

public:
	CMainFrame();
	virtual ~CMainFrame();

public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:  // control bar embedded members
	CToolBar    m_wndToolBar;
	CToolBar	m_MyToolBar;
	CImageList	m_ImageList;
	HACCEL		m_hHotKey;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnHotkeyCtrlN();
	afx_msg void OnHotkeyCtrlO();
	afx_msg void OnHotkeyCtrlQ();
};
