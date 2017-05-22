#pragma once
#include "afxwin.h"

// CChangeBppDlg 对话框

class CChangeBppDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CChangeBppDlg)

public:
	CChangeBppDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CChangeBppDlg();

// 对话框数据
	enum { IDD = IDD_DLG_CHANGE_BPP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CStatic		m_StaticBpp;
	CComboBox	m_ComboBpp;
	int			m_nTargetBpp;
	afx_msg void OnSelchangeComboBpp();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};

