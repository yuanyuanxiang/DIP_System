#pragma once

#include "ColorGradWnd.h"
#include "resource.h"

// CColorGradDlg 对话框

class CColorGradDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CColorGradDlg)

public:
	CColorGradDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CColorGradDlg();

// 对话框数据
	enum { IDD = IDD_DLG_COLORGRAD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public://唯一的成员变量
	CColorGradWnd* m_pColorGradWnd;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
};

