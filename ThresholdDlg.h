#pragma once

#include "PreviewDlg.h"
#include "afxcmn.h"
#include "DIP_SystemDoc.h"
#include "DIP_SystemView.h"

// CThresholdDlg 对话框

class CThresholdDlg : public CDialog
{
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CThresholdDlg)

public:
	CSliderCtrl		m_SliderNum;			// 阈值的滑块对象
	int				m_nNum;					// 阈值
	int				m_nPos;					// 滑块对象关联变量
	int				m_nWidth;				// 图像宽度
	int				m_nHeight;				// 图像高度
	int				m_nChannel;				// 图像通道
	LONG			m_nRowLen;				// 图像行宽
	LONG			m_nLength;				// 图像总字节
	BYTE*			m_pSrc;					// 指向原始数据
	BYTE*			m_pBuf;					// 指向缓存数据

	// 标准构造和析构函数
	CThresholdDlg(CWnd* pParent = NULL);
	virtual ~CThresholdDlg();

	void Refresh();						// 刷新预览窗口

	CDIP_SystemDoc* GetDocument();		// 获取活动文档
	CDIP_SystemView* GetActiveView();	// 获取活动视图
	virtual BOOL OnInitDialog();		// 初始化对话框
	afx_msg void OnEnChangeNum();		// 阈值被修改时的响应函数
	afx_msg void OnBnClickedSave();		// “保存”按钮响应函数 
	afx_msg void OnBnClickedOk();		// “确定”按钮的响应函数 
	afx_msg void OnBnClickedCancel();	// “取消”按钮的响应函数
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
										// 滚动条响应函数

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

private:
	CPreviewDlg*	dlg;				// 预览对话框
	bool			m_bModify;			// 图像是否被修改

	// 对话框数据
	enum { IDD = IDD_DLG_THRESHOLD };
};

