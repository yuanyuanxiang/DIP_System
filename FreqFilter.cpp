// ************************************************************************
//  文件名：FreqFilter.cpp
//
//  图像频域滤波函数库：
//
//	ButterworthL()		- 巴特沃斯低通滤波器
//	ButterworthH()		- 巴特沃斯高通滤波器
//	MutualFilter()		- 交互式带阻滤波器
//	RetrorseFilter()	- 巴特沃斯低通滤波器的逆滤波
//	WienerFilter()		- 有约束恢复的维纳滤波
//	PSE_Filter()		- 有约束恢复的功率谱均衡滤波
//	MinPower()			- 有约束恢复的最小平方滤波
//
//*************************************************************************

#include "stdafx.h"
#include "dip_system.h"
#include "FreqFilter.h"
#include "FreqCalculate.h"
#include <complex>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFreqFilter

CFreqFilter::CFreqFilter()
{
}

CFreqFilter::CFreqFilter(CDIB* pDIB)
{
	m_pDIB = pDIB;
}

CFreqFilter::~CFreqFilter()
{
}

BEGIN_MESSAGE_MAP(CFreqFilter, CWnd)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFreqFilter message handlers

/*************************************************************************
 *
 * 函数名称：
 *   ButterworthL()
 *
 * 参数:
 *   HDIB	hDIB		- 待处理的DIB
 *	 float  fD			- 低通滤波阀值
 *
 * 返回值:
 *   void			    - 无返回值
 *
 * 说明:
 *   该函数对图象进行巴特沃斯低通滤波
 *
 ************************************************************************/
void CFreqFilter::ButterworthL(float fD)
{
	// 临时变量
	LONG	i;
	LONG	j;

	// 进行付立叶变换的宽度和高度（2的整数次方）
	LONG	w;
	LONG	h;
	
	int		wp;
	int		hp;

	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();

	LONG lLineBytes = m_pDIB->RowLen();

	// 赋初值
	w = 1;
	h = 1;
	wp = 0;
	hp = 0;
	
	// 计算进行付立叶变换的宽度和高度（2的整数次方）
	while(w < lWidth)
	{
		w *= 2;
		wp++;
	}
	
	while(h < lHeight)
	{
		h *= 2;
		hp++;
	}

	CFreqCalculate clsFreqCalculate(m_pDIB);

	// 保存频域数据
	complex<double> *FD = NULL, *TD = NULL;
	FD = new complex<double>[w * h * 3];
	TD = new complex<double>[w * h * 3];

	// 行
	for(i = 0; i < h; i++)
	{
		// 列
		for(j = 0; j < 3 * w; j++)
		{
			if(i < lHeight && j < lLineBytes)
			{
				// 获取时域数值
				BYTE Value = *((BYTE *)lpDIBBits + lLineBytes * i + j);
			
				// 时域赋值
				TD[w * i * 3 + j] = complex<double>(Value, 0.0f);
			}
			else
			{
				// 否则补零
				TD[w * i * 3 + j] = complex<double>(0.0f, 0.0f);
			}
		}
	}

	// 进行频谱分析 时域->频域
	if (clsFreqCalculate.Fourier(TD, lWidth, lHeight, FD) == FALSE)
		return;

	// 释放内存
	delete[] TD;

	// 当前频率
	float fDN;

	// 对图象进行低通滤波
	for(i = 0; i < h; i++)
	{
		for(j = 0; j < w * 3; j++)
		{
			// 计算距离
			int k = (int)(j / 3);
			fDN = (float)sqrt(1.0* i * i + k * k);		

			// 构造巴特沃斯低通滤波器并滤波
			FD[i * 3 * w + j] *= complex<double>(1 / (1 + 0.414 * (fDN / fD) * (fDN / fD)), 0.0f);
		}
	}

	// 进行频谱分析 频域->时域
	if (clsFreqCalculate.IFourier(lpDIBBits, lWidth, lHeight, FD) == FALSE)
		return;

	// 释放内存
	delete[] FD;

	EndWaitCursor();
}

/*************************************************************************
 *
 * 函数名称：
 *   ButterworthH()
 *
 * 参数:
 *   HDIB	hDIB		- 待处理的DIB
 *	 float  fD			- 高通滤波阀值
 *
 * 返回值:
 *   void			    - 无返回值
 *
 * 说明:
 *   该函数对图象进行巴特沃斯高通滤波
 *
 ************************************************************************/
void CFreqFilter::ButterworthH(float fD)
{
	// 临时变量
	LONG	i;
	LONG	j;

	// 进行付立叶变换的宽度和高度（2的整数次方）
	LONG	w;
	LONG	h;
	
	int		wp;
	int		hp;

	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();

	LONG lLineBytes = m_pDIB->RowLen();

	// 赋初值
	w = 1;
	h = 1;
	wp = 0;
	hp = 0;
	
	// 计算进行付立叶变换的宽度和高度（2的整数次方）
	while(w < lWidth)
	{
		w *= 2;
		wp++;
	}
	
	while(h < lHeight)
	{
		h *= 2;
		hp++;
	}

	CFreqCalculate clsFreqCalculate(m_pDIB);

	// 保存频域数据
	complex<double> *FD, *TD;
	FD = new complex<double>[w * h * 3];
	TD = new complex<double>[w * h * 3];

	// 行
	for(i = 0; i < h; i++)
	{
		// 列
		for(j = 0; j < 3 * w; j++)
		{
			if(i < lHeight && j < lLineBytes)
			{
				// 获取时域数值
				BYTE Value = *((BYTE *)lpDIBBits + lLineBytes * i + j);
			
				// 时域赋值
				TD[w * i * 3 + j] = complex<double>(Value, 0.0f);
			}
			else
			{
				// 否则补零
				TD[w * i * 3 + j] = complex<double>(0.0f, 0.0f);
			}
		}
	}

	// 进行频谱分析 时域->频域
	if (clsFreqCalculate.Fourier(TD, lWidth, lHeight, FD) == FALSE)
		return;

	// 释放内存
	delete[] TD;

	// 当前频率
	float fDN;

	// 对图象进行高通滤波
	for(i = 0; i < h; i++)
	{
		for(j = 0; j < w * 3; j++)
		{
			// 计算距离
			int k = (int)(j / 3);
			fDN = (float)sqrt(1.0* i * i + k * k);		

			// 构造巴特沃斯高通滤波器并滤波
			FD[i * 3 * w + j] *= complex<double>(1.0 / (1.0 + 0.414 * (fD / fDN) * (fD / fDN) + 0.5), 0.0f);
		}
	}

	// 进行频谱分析 频域->时域
	if (clsFreqCalculate.IFourier(lpDIBBits, lWidth, lHeight, FD) == FALSE)
		return;

	// 释放内存
	delete[] FD;

	EndWaitCursor();
}

/*************************************************************************
 *
 * 函数名称：
 *   MutualFilter()
 *
 * 参数:
 *   HDIB	hDIB		- 待处理的DIB
 *	 CRect* pRect		- 频域带阻区域序列
 *	 int	nSum		- 带阻序列数目
 *
 * 返回值:
 *   void			    - 无返回值
 *
 * 说明:
 *   该函数对图象进行交互式带阻滤波
 *
 ************************************************************************/
void CFreqFilter::MutualFilter(CRect* pRect, int nSum)
{
	// 临时变量
	LONG	i;
	LONG	j;
	LONG	k;

	// 进行付立叶变换的宽度和高度（2的整数次方）
	LONG	w;
	LONG	h;
	
	int		wp;
	int		hp;

	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();

	LONG lLineBytes = m_pDIB->RowLen();

	// 赋初值
	w = 1;
	h = 1;
	wp = 0;
	hp = 0;
	
	// 计算进行付立叶变换的宽度和高度（2的整数次方）
	while(w < lWidth)
	{
		w *= 2;
		wp++;
	}
	
	while(h < lHeight)
	{
		h *= 2;
		hp++;
	}

	CFreqCalculate clsFreqCalculate(m_pDIB);

	// 保存频域数据
	complex<double> *FD, *TD;
	FD = new complex<double>[w * h * 3];
	TD = new complex<double>[w * h * 3];

	// 行
	for(i = 0; i < h; i++)
	{
		// 列
		for(j = 0; j < 3 * w; j++)
		{
			if(i < lHeight && j < lLineBytes)
			{
				// 获取时域数值
				BYTE Value = *((BYTE *)lpDIBBits + lLineBytes * i + j);
			
				// 时域赋值
				TD[w * i * 3 + j] = complex<double>(Value, 0.0f);
			}
			else
			{
				// 否则补零
				TD[w * i * 3 + j] = complex<double>(0.0f, 0.0f);
			}
		}
	}

	// 进行频谱分析 时域->频域
	if (clsFreqCalculate.Fourier(TD, lWidth, lHeight, FD) == FALSE)
		return;

	// 释放内存
	delete[] TD;

	// 对所选区域实施带阻滤波
	for(k = 0; k < nSum; k++)
	{
		// 计算在频域补零后的平面内的坐标
		pRect[k].top += (h - lHeight) / 2;
		pRect[k].bottom += (h - lHeight) / 2;
		pRect[k].left *= 3;
		pRect[k].right *= 3;
		pRect[k].left += (w - lWidth) * 3 / 2;
		pRect[k].right += (w - lWidth) * 3 / 2;

		// 恢复到变换前的象限位置
		if (pRect[k].top < h / 2)
			pRect[k].top += h / 2;
		else
			pRect[k].top -= h / 2;

		if (pRect[k].bottom < h / 2)
			pRect[k].bottom += h / 2;
		else
			pRect[k].bottom -= h / 2;

		if (pRect[k].left < w * 3 /2)
			pRect[k].left += 3 * w / 2;
		else 
			pRect[k].left -= 3 * w / 2;

		if (pRect[k].right < w * 3 /2)
			pRect[k].right += 3 * w / 2;
		else 
			pRect[k].right -= 3 * w / 2;
	
		// 如果所选区域在同一象限
		if (pRect[k].top < pRect[k].bottom && pRect[k].left < pRect[k].right)
		{
			// 对区域进行带阻滤波
			for (i = h - pRect[k].bottom; i < h - pRect[k].top; i++)
			{
				for (j = pRect[k].left * 3; j < pRect[k].right * 3; j++)
					FD[i * w * 3 + j] = complex<double>(0, 0);
			}
		}

		// 如果所选区域为一、二或三、四象限
		if (pRect[k].top < pRect[k].bottom && pRect[k].left > pRect[k].right)
		{
			// 对区域进行带阻滤波
			for (i = h - pRect[k].bottom; i < h - pRect[k].top; i++)
			{
				// 对二或三象限进行滤波
				for (j = 0; j < pRect[k].right * 3; j++)
					FD[i * w * 3 + j] = complex<double>(0, 0);

				// 对一或四象限进行滤波
				for (j = pRect[k].left * 3; j < h; j++)
					FD[i * w * 3 + j] = complex<double>(0, 0);
			}
		}

		// 如果所选区域为一、四或二、三象限
		if (pRect[k].top > pRect[k].bottom && pRect[k].left < pRect[k].right)
		{
			// 对区域进行带阻滤波
			// 对三或四象限进行滤波
			for (i = 0; i < h - pRect[k].top; i++)
			{
				for (j = pRect[k].left * 3; j < pRect[k].right * 3; j++)
					FD[i * w * 3 + j] = complex<double>(0, 0);
			}
			// 对一或二象限进行滤波
			for (i = h - pRect[k].bottom; i < h; i++)
			{
				for (j = pRect[k].left * 3; j < pRect[k].right * 3; j++)
					FD[i * w * 3 + j] = complex<double>(0, 0);
			}
		}

		// 如果所选区域为一、二、三、四象限
		if (pRect[k].top > pRect[k].bottom && pRect[k].left > pRect[k].right)
		{
			// 对区域进行带阻滤波
			for (i = 0; i < h - pRect[k].top; i++)
			{
				// 对三象限进行滤波
				for (j = 0; j < pRect[k].right * 3; j++)
					FD[i * w * 3 + j] = complex<double>(0, 0);
				
				// 对四象限进行滤波
				for (j = pRect[k].left * 3; j < h; j++)
					FD[i * w * 3 + j] = complex<double>(0, 0);
			}
			for (i = h - pRect[k].bottom; i < h; i++)
			{
				// 对二象限进行滤波
				for (j = 0; j < pRect[k].right * 3; j++)
					FD[i * w * 3 + j] = complex<double>(0, 0);

				// 对一象限进行滤波
				for (j = pRect[k].left * 3; j < h; j++)
					FD[i * w * 3 + j] = complex<double>(0, 0);
			}
		}
	}

	// 进行频谱分析 频域->时域
	if (clsFreqCalculate.IFourier(lpDIBBits, lWidth, lHeight, FD) == FALSE)
		return;

	// 释放内存
	delete[] FD;

	EndWaitCursor();
}

/*************************************************************************
 *
 * 函数名称：
 *   RetrorseFilter()
 *
 * 参数:
 *   HDIB	hDIB		- 待处理的DIB
 *	 float  fD			- 低通滤波阀值
 *
 * 返回值:
 *   void			    - 无返回值
 *
 * 说明:
 *   该函数对图象进行巴特沃斯低通滤波的逆滤波处理
 *
 ************************************************************************/
void CFreqFilter::RetrorseFilter(float fD)
{
	// 临时变量
	LONG	i;
	LONG	j;

	// 进行付立叶变换的宽度和高度（2的整数次方）
	LONG	w;
	LONG	h;
	
	int		wp;
	int		hp;

	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();

	LONG lLineBytes = m_pDIB->RowLen();

	// 赋初值
	w = 1;
	h = 1;
	wp = 0;
	hp = 0;
	
	// 计算进行付立叶变换的宽度和高度（2的整数次方）
	while(w < lWidth)
	{
		w *= 2;
		wp++;
	}
	
	while(h < lHeight)
	{
		h *= 2;
		hp++;
	}

	CFreqCalculate clsFreqCalculate(m_pDIB);

	// 保存频域数据
	complex<double> *FD, *TD;
	FD = new complex<double>[w * h * 3];
	TD = new complex<double>[w * h * 3];

	// 行
	for(i = 0; i < h; i++)
	{
		// 列
		for(j = 0; j < 3 * w; j++)
		{
			if(i < lHeight && j < lLineBytes)
			{
				// 获取时域数值
				BYTE Value = *((BYTE *)lpDIBBits + lLineBytes * i + j);
			
				// 时域赋值
				TD[w * i * 3 + j] = complex<double>(Value, 0.0f);
			}
			else
			{
				// 否则补零
				TD[w * i * 3 + j] = complex<double>(0.0f, 0.0f);
			}
		}
	}

	// 进行频谱分析 时域->频域
	if (clsFreqCalculate.Fourier(TD, lWidth, lHeight, FD) == FALSE)
		return;

	// 释放内存
	delete[] TD;

	// 当前频率
	float fDN;

	// 对图象进行低通滤波
	for(i = 0; i < h; i++)
	{
		for(j = 0; j < w * 3; j++)
		{
			// 计算距离
			int k = (int)(j / 3);
			fDN = (float)sqrt(1.0* i * i + k * k);		
			
			float fH = 1 / (1 + 0.414f * (fDN / fD) * (fDN / fD));

			// 逆滤波恢复
			if(fH > 0.25f)
				FD[i * 3 * w + j] /= complex<double>(fH, 0.0f);
			else
				FD[i * 3 * w + j] *= complex<double>(0.2f, 0.0f);
		}
	}

	// 进行频谱分析 频域->时域
	if (clsFreqCalculate.IFourier(lpDIBBits, lWidth, lHeight, FD) == FALSE)
		return;

	// 释放内存
	delete[] FD;

	EndWaitCursor();
}

/*************************************************************************
 *
 * 函数名称：
 *   WienerFilter()
 *
 * 参数:
 *   HDIB	hDIB		- 待处理的DIB
 *	 float  fD			- 低通滤波阀值
 *
 * 返回值:
 *   void			    - 无返回值
 *
 * 说明:
 *   该函数对图象进行有约束恢复的维纳滤波处理
 *
 ************************************************************************/
void CFreqFilter::WienerFilter(float fD)
{
	// 临时变量
	LONG	i;
	LONG	j;

	// 进行付立叶变换的宽度和高度（2的整数次方）
	LONG	w;
	LONG	h;
	
	int		wp;
	int		hp;

	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();

	LONG lLineBytes = m_pDIB->RowLen();

	// 赋初值
	w = 1;
	h = 1;
	wp = 0;
	hp = 0;
	
	// 计算进行付立叶变换的宽度和高度（2的整数次方）
	while(w < lWidth)
	{
		w *= 2;
		wp++;
	}
	
	while(h < lHeight)
	{
		h *= 2;
		hp++;
	}

	CFreqCalculate clsFreqCalculate(m_pDIB);

	// 保存频域数据
	complex<double> *FD, *TD;
	FD = new complex<double>[w * h * 3];
	TD = new complex<double>[w * h * 3];

	// 行
	for(i = 0; i < h; i++)
	{
		// 列
		for(j = 0; j < 3 * w; j++)
		{
			if(i < lHeight && j < lLineBytes)
			{
				// 获取时域数值
				BYTE Value = *((BYTE *)lpDIBBits + lLineBytes * i + j);
			
				// 时域赋值
				TD[w * i * 3 + j] = complex<double>(Value, 0.0f);
			}
			else
			{
				// 否则补零
				TD[w * i * 3 + j] = complex<double>(0.0f, 0.0f);
			}
		}
	}

	// 进行频谱分析 时域->频域
	if (clsFreqCalculate.Fourier(TD, lWidth, lHeight, FD) == FALSE)
		return;

	// 释放内存
	delete[] TD;

	// 当前频率
	float fDN;

	// 预先设定的经验常数
	float K = 0.05f;

	// 对图象进行低通滤波
	for(i = 0; i < h; i++)
	{
		for(j = 0; j < w * 3; j++)
		{
			// 计算距离
			int k = (int)(j / 3);
			fDN = (float)sqrt(1.0* i * i + k * k);		

			float fH = 1 / (1 + 0.414f * (fDN / fD) * (fDN / fD));

			// 维纳滤波恢复
			FD[i * 3 * w + j] *= complex<double>(fH / (fH * fH + K), 0.0f);
		}
	}

	// 进行频谱分析 频域->时域
	if (clsFreqCalculate.IFourier(lpDIBBits, lWidth, lHeight, FD) == FALSE)
		return;

	// 释放内存
	delete[] FD;

	EndWaitCursor();
}

/*************************************************************************
 *
 * 函数名称：
 *   PSE_Filter()
 *
 * 参数:
 *   HDIB	hDIB		- 待处理的DIB
 *	 float  fD			- 低通滤波阀值
 *
 * 返回值:
 *   void			    - 无返回值
 *
 * 说明:
 *   该函数对图象进行功率谱均衡滤波
 *
 ************************************************************************/
void CFreqFilter::PSE_Filter(float fD)
{
	// 临时变量
	LONG	i;
	LONG	j;

	// 进行付立叶变换的宽度和高度（2的整数次方）
	LONG	w;
	LONG	h;
	
	int		wp;
	int		hp;

	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();

	LONG lLineBytes = m_pDIB->RowLen();

	// 赋初值
	w = 1;
	h = 1;
	wp = 0;
	hp = 0;
	
	// 计算进行付立叶变换的宽度和高度（2的整数次方）
	while(w < lWidth)
	{
		w *= 2;
		wp++;
	}
	
	while(h < lHeight)
	{
		h *= 2;
		hp++;
	}

	CFreqCalculate clsFreqCalculate(m_pDIB);

	// 保存频域数据
	complex<double> *FD, *TD;
	FD = new complex<double>[w * h * 3];
	TD = new complex<double>[w * h * 3];

	// 行
	for(i = 0; i < h; i++)
	{
		// 列
		for(j = 0; j < 3 * w; j++)
		{
			if(i < lHeight && j < lLineBytes)
			{
				// 获取时域数值
				BYTE Value = *((BYTE *)lpDIBBits + lLineBytes * i + j);
			
				// 时域赋值
				TD[w * i * 3 + j] = complex<double>(Value, 0.0f);
			}
			else
			{
				// 否则补零
				TD[w * i * 3 + j] = complex<double>(0.0f, 0.0f);
			}
		}
	}

	// 进行频谱分析 时域->频域
	if (clsFreqCalculate.Fourier(TD, lWidth, lHeight, FD) == FALSE)
		return;

	// 释放内存
	delete[] TD;

	// 当前频率
	float fDN;

	// 预先设定的经验常数
	float K=0.05f;

	// 对图象进行低通滤波
	for(i = 0; i < h; i++)
	{
		for(j = 0; j < w * 3; j++)
		{
			// 计算距离
			int k = (int)(j / 3);
			fDN = (float)sqrt(1.0* i * i + k * k);		
			
			float fH = 1 / (1 + 0.414f * (fDN / fD) * (fDN / fD));

			// 功率谱均衡滤波
			FD[i * 3 * w + j] *= complex<double>(1 / sqrt(1.0*fH * fH + K), 0.0f);
		}
	}

	// 进行频谱分析 频域->时域
	if (clsFreqCalculate.IFourier(lpDIBBits, lWidth, lHeight, FD) == FALSE)
		return;

	// 释放内存
	delete[] FD;

	EndWaitCursor();
}

/*************************************************************************
 *
 * 函数名称：
 *   MinPower()
 *
 * 参数:
 *   HDIB	hDIB		- 待处理的DIB
 *	 float  fD			- 低通滤波阀值
 *
 * 返回值:
 *   void			    - 无返回值
 *
 * 说明:
 *   该函数对图象进行有约束最小平方滤波滤波
 *
 ************************************************************************/
void CFreqFilter::MinPower(float fD)
{
	// 临时变量
	LONG	i;
	LONG	j;

	// 进行付立叶变换的宽度和高度（2的整数次方）
	LONG	w;
	LONG	h;
	
	int		wp;
	int		hp;

	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	CHECK_EDITABLE(m_pDIB->GetImage());
	
	BeginWaitCursor();
	
	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();

	LONG lLineBytes = m_pDIB->RowLen();

	// 赋初值
	w = 1;
	h = 1;
	wp = 0;
	hp = 0;
	
	// 计算进行付立叶变换的宽度和高度（2的整数次方）
	while(w < lWidth)
	{
		w *= 2;
		wp++;
	}
	
	while(h < lHeight)
	{
		h *= 2;
		hp++;
	}

	CFreqCalculate clsFreqCalculate(m_pDIB);

	// 保存频域数据
	complex<double> *FD, *TD;
	FD = new complex<double>[w * h * 3];
	TD = new complex<double>[w * h * 3];

	// 行
	for(i = 0; i < h; i++)
	{
		// 列
		for(j = 0; j < 3 * w; j++)
		{
			if(i < lHeight && j < lLineBytes)
			{
				// 获取时域数值
				BYTE Value = *((BYTE *)lpDIBBits + lLineBytes * i + j);
			
				// 时域赋值
				TD[w * i * 3 + j] = complex<double>(Value, 0.0f);
			}
			else
			{
				// 否则补零
				TD[w * i * 3 + j] = complex<double>(0.0f, 0.0f);
			}
		}
	}

	// 进行频谱分析 时域->频域
	if (clsFreqCalculate.Fourier(TD, lWidth, lHeight, FD) == FALSE)
		return;

	// 释放内存
	delete[] TD;

	// 当前频率
	float fDN;

	// 预先设定的经验常数
	float S = 0.00003f;

	// 对图象进行低通滤波
	for(i = 0; i < h; i++)
	{
		for(j = 0; j < w * 3; j++)
		{
			// 计算距离
			int k = (int)(j / 3);
			fDN = (float)sqrt(1.0* i * i + k * k);		

			// 计算用Q实现的高通滤波器的转移函数P(u,v)
			float P = (float) (2 * PI * fDN);
			
			// 计算H(u,v)
			float fH = 1 / (1 + 0.414f * (fDN / fD) * (fDN / fD));

			// 最小平方滤波恢复
			FD[i * 3 * w + j] *= complex<double>(fH / (fH * fH + S * P), 0.0f);
		}
	}

	// 进行频谱分析 频域->时域
	if (clsFreqCalculate.IFourier(lpDIBBits, lWidth, lHeight, FD) == FALSE)
		return;

	// 释放内存
	delete[] FD;

	EndWaitCursor();
}


