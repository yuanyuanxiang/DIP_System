#pragma once

// CCLAHEWnd

#include "DIP_SystemDoc.h"
#include "DIP_SystemView.h"
#include "resource.h"
#include <fstream>
using namespace std;

class CDIPSysDlg;

class CHistogramWnd : public CWnd
{
	DECLARE_DYNAMIC(CHistogramWnd)

public:
	CHistogramWnd();
	virtual ~CHistogramWnd();

protected:
	DECLARE_MESSAGE_MAP()

public://新增成员与函数
	CRect*                   m_pPaintRect;  //绘图区
	CRect*                  m_pClientRect;  //客户区

	//按钮
	CMFCButton*                   m_pBnOK;  //确定
	CMFCButton*               m_pBnCancel;  //取消
	CMFCButton*                   m_pBnHE;  //HE均衡化
	CMFCButton*                  m_pBnLHE;  //LHE均衡化
	CMFCButton*                  m_pBnAHE;  //AHE均衡化
	CMFCButton*                m_pBnCLAHE;  //CLAHE均衡化
	CMFCButton*                m_pBnReset;  //重置

	//数据
	double*              m_pfHistogram[4];    //直方图:4*256
	double*              m_pfTotalHist[4];    //累计直方图
	int*                 m_V4Transform[4];    //灰度变换:4*m_nWidth
	BYTE*                       m_dataSrc;    //图像源数据
	BYTE*                       m_curData;    //图像当前数据

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

	//局部直方图均衡参数
	int                     m_nRegionSize;  //区域大小
	int                   m_nRegionLength;  //区域长度

	//获取主程序位图指针
	CImage*                      m_pImage;  //位图指针

	//*** 函数 ***
	BOOL GetImageInfo();                    //获取图像信息
	CDIP_SystemDoc* GetDocument();          //获得活动文档
	CDIP_SystemView* GetActiveView();       //获得活动视图
	BOOL GetHistogram();                    //通过计算，取得直方图
	void HistogramEq(BYTE** Region, double Map[256]);//获取局部直方图，并修改中心像素
	void MappingLHE();                      //局部直方图均衡
	void MappingHE();                       //求均衡化映射（旧图->新图）关系，并写入新图像
	void Reshape(CWnd* pWnd, double space, int Number);
	void GetBits(int channel, BYTE*);       //获取某个通道数据
	void SetBits(int channel, BYTE*);       //设置某个通道数据
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCmdOK();
	afx_msg void OnCmdCancel();
	afx_msg void OnCmdHe();
	afx_msg void OnCmdLhe();
	afx_msg void OnCmdAhe();
	afx_msg void OnCmdClahe();
	afx_msg void OnCmdReset();
};

//*** 其他 ***

COLORREF ColorSetting(int Channel, int Strength, int WhereUse = 0);//设置颜色
template<typename TYPE>
void Write2File(CString FilePath, TYPE* dataSrc, int Width, int Height);

//
/*********************** CLAHE ************************/
//

#define uiNR_OF_GREY (256)

int CLAHE(BYTE* pImage, unsigned int uiXRes, unsigned int uiYRes, BYTE Min, BYTE Max, unsigned int uiNrX, unsigned int uiNrY, unsigned int uiNrBins, float fCliplimit);

static void ClipHistogram (unsigned long*, unsigned int, unsigned long);

static void MakeHistogram (BYTE*, unsigned int, unsigned int, unsigned int, unsigned long*, unsigned int, BYTE*);

static void MapHistogram (unsigned long*, BYTE, BYTE, unsigned int, unsigned long);

static void MakeLut (BYTE*, BYTE, BYTE, unsigned int);

static void Interpolate (BYTE*, int, unsigned long*, unsigned long*, unsigned long*, unsigned long*, unsigned int, unsigned int, BYTE*);

