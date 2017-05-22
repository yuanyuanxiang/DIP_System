#pragma once

// CSpaceFilterWnd

//
// *** class CSpaceFilterWnd ***
//

#include "DIP_SystemDoc.h"
#include "DIP_SystemView.h"
#include "resource.h"

#define MAX_FILTER_SIZE				7

class CFilter;

class CSpaceFilterWnd : public CWnd
{
	DECLARE_DYNAMIC(CSpaceFilterWnd)

public:
	CSpaceFilterWnd();
	virtual ~CSpaceFilterWnd();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

public://新增成员与函数
	CRect*                                m_pPaintRect;  //绘图区
	CImage*                                   m_pImage;  //原图像指针
	BYTE*                                    m_dataSrc;  //原图像数据
	CEdit*m_pElements[MAX_FILTER_SIZE*MAX_FILTER_SIZE];  //编辑框

	CDIP_SystemDoc*                             m_pDoc;  //活动文档
	CDIP_SystemView*                           m_pView;  //活动视图

	//图像基本信息
	int                                  m_nImageWidth;  //图像宽度
	int                                 m_nImageHeight;  //图像高度
	int                                     m_nChannel;  //图像通道
	long                                     m_nRowlen;  //行字节数
	long                                    m_nlenData;  //数据长度

	//按钮
	CMFCButton*                                m_pBnOK;  //确定
	CMFCButton*                            m_pBnCancel;  //取消
	CMFCButton*                             m_pBnReset;  //重置
	CMFCButton*                      m_pBnMedianFilter;  //中值滤波
	CMFCButton*                    m_pBnAdaptiveFilter;  //自适应中值滤波
	CMFCButton*                m_pBnAddPepperSaltNoise;  //椒盐噪声
	CMFCButton*                  m_pBnAddGaussianNoise;  //高斯噪声
	CMFCButton*                             m_pBnApply;  //应用操作
	CComboBox*                       m_pComboOperation;  //操作类型
	CComboBox*                      m_pComboFilterSize;  //滤波器大小
	CFont*                                  m_pAppFont;  //字体
	CFont*                                m_pSmallFont;  //更小的字体

	//其他
	CFilter*                             m_pPixelBlock;  //像素块
	CFilter*                               m_pMyFilter;  //滤波器
	int                                  m_nFilterSize;  //滤波器阶数
	int                               m_nMaxFilterSize;  //最大阶数
	int                                   m_nOperation;  //选中的操作

	//Gauss Noise
	double                                      m_Mean;  //均值
	double                                       m_Std;  //方差

	//Pepper & Salt Noise
	double                                    m_Pepper;  //椒盐噪声中胡椒比例
	double                                 m_Pollution;  //图像污染程度

	//*** 函数 ***
	void Reshape(CWnd* pWnd, double Space, int nRow, int nCol = 1);
	                                                     //重绘按钮
	BOOL AddPepperSaltNoise();                           //添加椒盐噪声
	BOOL AddGaussianNoise();                             //添加高斯噪声
	BOOL MedianFilter();                                 //中值滤波
	BOOL AdaptMedianFilter();                            //自适应中值滤波
	int FilterProcessA(BYTE* dataSrc, int curChannel, int y, int x, int FilterSize);
	int FilterProcessB(int Zmin, int Zmax, int Zmed, int Zxy);
	void Convolute(CFilter* Filter);                     // 卷积

	afx_msg void OnCmdOK();
	afx_msg void OnCmdCancel();
	afx_msg void OnCmdReset();
	afx_msg void OnCmdMedianFilter();
	afx_msg void OnCmdAdaptiveFilter();
	afx_msg void OnCmdAddPepperSaltNoise();
	afx_msg void OnCmdAddGaussianNoise();
	afx_msg void OnCmdApply();
	afx_msg void OnCmdChooseOperation();
	afx_msg void OnCmdChangeFilterSize();
};

double ProduceGaussianNoise(double nMean = 0, double nStd = 1);   //产生高斯噪声:采用中心极限定理
double ProduceGaussianNoise2(double nMean = 0, double nStd = 1);  //产生高斯噪声:采用Box-Muller办法
int ProducePepperSaltNoise(double Probability = 0.05);            //产生椒盐噪声，Probability为胡椒比例

