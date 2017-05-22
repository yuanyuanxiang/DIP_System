#pragma once

#include "DIP_SystemDoc.h"
#include "DIP_SystemView.h"
#include "resource.h"
#include <vector>
using namespace std;

// CColorGradWnd

class CColorGradWnd : public CWnd
{
	DECLARE_DYNAMIC(CColorGradWnd)

public:
	CColorGradWnd();
	virtual ~CColorGradWnd();

protected:
	DECLARE_MESSAGE_MAP()

public://新增成员与函数
	CRect*                   m_pPaintRect;  //绘图区
	CRect*                  m_pClientRect;  //客户区

	//按钮
	CMFCButton*                   m_pBnOK;  //确定
	CMFCButton*               m_pBnCancel;  //取消
	CButton*                m_pBnGroupBox;  //GroupBox
	CStatic*               m_pStaticInput;  //输入色阶
	CComboBox*             m_pComboBoxRGB;  //RGB组合框
	CEdit*                 m_pEditLeftTop;  //左上角的编辑框
	CEdit*                     m_pEditTop;  //正上方的编辑框
	CEdit*                m_pEditRightTop;  //右上角的编辑框
	CEdit*              m_pEditLeftBottom;  //左下角的编辑框
	CEdit*             m_pEditRightBottom;  //右下角的编辑框
	CStatic*              m_pStaticOutput;  //输出色阶

	//控制点
	CPoint                       m_ptDark;  //左上方
	CPoint                     m_ptMedian;  //中间
	CPoint                     m_ptBright;  //右上方
	CPoint                        m_ptMin;  //左下方
	CPoint                        m_ptMax;  //右下方
	int                        m_nCurDrag;  //当前拽动的点
	int                       m_nLastDrag;  //上次拽动的点
	double                    m_dRatio[5];  //控制点相对绘图区宽度的比率
	double                    m_dValue[5];  //CEdit显示的值
	double                      m_dMedian;  //中间显示的明暗调比率

	//数据
	double*              m_pfHistogram[4];    //直方图:4*256
	int*                 m_V4Transform[4];    //灰度变换:4*m_nWidth
	BYTE*                       m_dataSrc;    //图像源数据
	BYTE*                       m_dataCur;    //图像当前数据

	//图像信息
	int                     m_nImageWidth;  //图像宽度
	int                    m_nImageHeight;  //图像高度
	int                        m_nChannel;  //图像通道
	LONG                        m_nRowlen;  //每行字节
	LONG                       m_nlenData;  //总的长度

	int                m_nChannelSelected;    //当前通道
	int                      m_nRectWidth;    //绘图区宽度
	int                     m_nRectHeight;    //绘图区高度
	double                  m_fWidthRatio;    //宽度缩放比
	double                 m_fHeightRatio;    //高度缩放比

	//字体
	CFont*                     m_pWndFont;  //窗口字体
	CString                    m_FontName;  //字体名称
	int                       m_nFontSize;  //字体大小

	//指针数组
	vector<CWnd*>             m_WndVector;  //窗口指针数组
	CPoint*                  m_ptArray[5];  //控制点数组

	//其他
	int                         m_nCurPos;  //当前位置
	int                        m_nLastPos;  //上一个位置
	double                     m_dOldRate;  //中间点占的比率

	//获取主程序位图指针
	CImage*                      m_pImage;  //位图指针

	//*** 函数 ***
	CDIP_SystemDoc* GetDocument();          //获得活动文档
	CDIP_SystemView* GetActiveView();       //获得活动视图
	BOOL GetHistogram();                    //通过计算，取得直方图
	BOOL GrayTransform();                   //依照5个控制点进行灰度变换
	void SplineFunc();                      //三次样条插值函数
	void ShowEditValue();                   //显示CEdit控件的值
	void UpdateControls(int number);        //更新控制点

	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnChannelSelected();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnClickOK();
	afx_msg void OnClickCancel();
	afx_msg void OnEditLeftTop();
	afx_msg void OnEditTop();
	afx_msg void OnEditRightTop();
	afx_msg void OnEditLeftBottom();
	afx_msg void OnEditRightBottom();
	virtual void DoDataExchange(CDataExchange* pDX);

private://新增加的私有成员函数
	//
	//*** 其他 ***
	//
	COLORREF ColorSetting(int Channel, int Strength, int WhereUse = 0);//设置颜色
	void DrawPoint(CPoint& pt, int size, CDC& MemDC);                  //绘制控制点
	//
	// *** 三次样条插值函数 ***
	//
	void Spline(double* Node, double* Value, double* x0, double* s, int n, int m, double c1, double c2, int CASE = 2);
	void Chase(double* a, double* b, double* c, double* f, double* x, int n);
};

