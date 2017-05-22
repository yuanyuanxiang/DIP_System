#pragma once
#include "afxwin.h"
#include "PreviewDlg.h"
#include "DIP_SystemDoc.h"
#include "DIP_SystemView.h"

// CStretchDlg 对话框

class CStretchDlg : public CDialog
{
	// 当前操作控制点类型
	enum PointType
	{
		PT_NULL,		// 控制点1
		PT_POINT_ONE,	// 控制点2
		PT_POINT_TWO	// 控制点3
	};

	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CStretchDlg)

public:
	CStatic			m_stiPrev;				// 图像预览框
	float			m_dPsX1;				// 控制点1的x坐标
	float			m_dPsY1;				// 控制点1的y坐标
	float			m_dPsX2;				// 控制点2的x坐标
	float			m_dPsY2;				// 控制点2的y坐标
	bool			m_bInitialized;			// 标记为初始化
	int				m_nWidth;				// 图像宽度
	int				m_nHeight;				// 图像高度
	int				m_nChannel;				// 图像通道
	LONG			m_nRowLen;				// 图像行宽
	LONG			m_nLength;				// 图像总字节
	BYTE*			m_pSrc;					// 指向原始数据
	BYTE*			m_pBuf;					// 指向缓存数据

	// 标准构造析构函数
	CStretchDlg(CWnd* pParent = NULL);
	virtual ~CStretchDlg();

	void Refresh();							// 刷新预览框

	CDIP_SystemDoc* GetDocument();			// 获取活动文档
	CDIP_SystemView* GetActiveView();		// 获取活动视图
	virtual BOOL OnInitDialog();			// 对话框初始化
	afx_msg void OnPaint();					// 重新绘制对话框
	afx_msg void OnEnChangePointoneX();		// 改变控制点1的x坐标时的响应函数
	afx_msg void OnEnChangePointoneY();		// 改变控制点1的y坐标时的响应函数
	afx_msg void OnEnChangePointtwoX();		// 改变控制点2的x坐标时的响应函数
	afx_msg void OnEnChangePointtwoY();		// 改变控制点2的y坐标时的响应函数
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
											// 鼠标移动时响应拖动动作
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
											// 按下鼠标左键的响应函数
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
											// 释放鼠标左键的响应函数
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
											// 设置鼠标光标
	afx_msg void OnBnClickedSave();			// “保存”按钮响应函数
	afx_msg void OnBnClickedOk();			// “确定”按钮响应函数
	afx_msg void OnBnClickedCancel();		// “取消”按钮响应函数

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	
private:
	CPreviewDlg*	dlg;			// 预览对话框
	bool			m_bModify;		// 图像是否被修改
	bool			m_bIsDraging;	// 是否正在拖动控制点
	int				m_nPointType;	// 当前操作控制点类型

	// 对话框数据
	enum { IDD = IDD_DLG_STRETCH };
};

