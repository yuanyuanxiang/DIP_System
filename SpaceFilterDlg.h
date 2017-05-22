#pragma once
#include "resource.h"

// CSpaceFilterDlg 对话框

class CSpaceFilterWnd;

class CSpaceFilterDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSpaceFilterDlg)

public:
	CSpaceFilterDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSpaceFilterDlg();

// 对话框数据
	enum { IDD = IDD_DLG_SPACEFILTER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public://新增成员与函数
	CSpaceFilterWnd*      m_pSpaceFilterWnd;//2200
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
};