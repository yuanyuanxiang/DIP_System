#pragma once

#include "resource.h"
#include "PreviewDlg.h"
#include "DIP_SystemDoc.h"
#include "DIP_SystemView.h"

// CRotateDlg 对话框

class CRotateDlg : public CDialog
{
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CRotateDlg)

public:
	int				m_nAngle;				// 旋转角度
	int				m_nPos;					// 旋转角度滑块关联
	int				m_nType;				// 旋转模式
	UINT			m_nWidth;				// 图像宽度
	UINT			m_nHeight;				// 图像高度
	UINT			m_nChannel;				// 图像通道
	LONG			m_nRowLen;				// 图像行宽
	LONG			m_nLength;				// 图像总字节
	BYTE*			m_pBuf;					// 指向缓存数据

	CRotateDlg(CWnd* pParent = NULL); 
	virtual ~CRotateDlg();

	void Refresh();
	virtual BOOL OnInitDialog();
	CDIP_SystemDoc* GetDocument();			// 获取活动文档
	CDIP_SystemView* GetActiveView();		// 获取活动视图

	// 消息响应函数
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedPrev();
	afx_msg void OnEnChangeAngle();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedSave();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

private:
	bool	m_bModify;	// 图像是否被修改

	// 对话框数据
	enum { IDD = IDD_DLG_ROTATE };
public:
	afx_msg void OnBnClickedCancel();
};