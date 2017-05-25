// ************************************************************************
//  文件名：FreqCalculate.cpp
//
//  图像正交变换函数库：
//
//	FFT()				- 一维快速付立叶变换
//  IFFT()				- 一维快速付立叶逆变换
//  Fourier()			- 二维快速傅立叶变换
//  IFourier()			- 二维快速傅立叶逆变换
//  DCT()				- 一维快速离散余弦变换
//  IDCT()				- 一维快速离散余弦逆变换
//	FreqDCT()			- 二维快速离散余弦变换
//  IFreqDCT()			- 二维快速离散余弦逆变换
//  WALSH()				- 一维沃尔什－哈达玛变换
//  IWALSH()			- 一维沃尔什－哈达玛逆变换
//	FreqWALSH()			- 二维沃尔什－哈达玛变换
//	IFreqWALSH()		- 二维沃尔什－哈达玛逆变换
//	DWT()				- 二维点阵的小波分解
//	IDWT()				- 二维点阵的小波重构
//	
//  DIBFourier()		- 图像的付立叶变换
//  DIBDCT()			- 图像的离散余弦变换
//  DIBWalsh()			- 图像的沃尔什－哈达玛变换
//  DIBDWT()			- 图象的二维离散小波变换
//
//*************************************************************************

#include "stdafx.h"
#include "dip_system.h"
#include "FreqCalculate.h"
#include "math.h"
#include <complex>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// 常数π
#ifndef PI
#define PI 3.1415926535
#endif

/////////////////////////////////////////////////////////////////////////////
// CFreqCalculate

CFreqCalculate::CFreqCalculate()
{
}

// 将pDIB赋值给内部变量
CFreqCalculate::CFreqCalculate(CDIB* pDIB)
{
	m_pDIB = pDIB;
}

CFreqCalculate::~CFreqCalculate()
{
}

BEGIN_MESSAGE_MAP(CFreqCalculate, CWnd)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFreqCalculate message handlers

/*************************************************************************
 *
 * 函数名称：
 *   FFT()
 *
 * 参数:
 *   complex<double> * TD	- 指向时域数组的指针
 *   complex<double> * FD	- 指向频域数组的指针
 *   r						－2的幂数，即迭代次数
 *
 * 返回值:
 *   无。
 *
 * 说明:
 *   该函数用来实现快速付立叶变换。
 *
 ************************************************************************/
void CFreqCalculate::FFT(complex<double> * TD, complex<double> * FD, int r)
{
	
	LONG	i;
	LONG	j;
	LONG	k;
	
	// 中间变量
	int		p;
	
	// 角度
	double	angle;
	
	complex<double> *W,*X1,*X2,*X;
	
	// 计算付立叶变换点数
	LONG N = 1 << r;
	
	// 分配运算所需存储器
	W  = new complex<double>[N / 2];
	X1 = new complex<double>[N];
	X2 = new complex<double>[N];
	
	// 计算加权系数
	for(i = 0; i < N / 2; i++)
	{
		angle = -i * PI * 2 / N;
		W[i] = complex<double> (cos(angle), sin(angle));
	}
	
	// 将时域点写入X1
	memcpy(X1, TD, sizeof(complex<double>) * N);
	
	// 采用蝶形算法进行快速付立叶变换
	for(k = 0; k < r; k++)
	{
		for(j = 0; j < 1 << k; j++)
		{
			for(i = 0; i < 1<<(r - k -1); i++)
			{
				p = j * (1<<(r - k));
				X2[i + p] = X1[i + p] + X1[i + p + (int)(1<<(r - k -1))];
				X2[i + p + (int)(1<<(r - k -1))] = (X1[i + p] - X1[i + p + (int)(1<<(r - k -1))]) * W[i * (1<<k)];
			}
		}
		X  = X1;
		X1 = X2;
		X2 = X;
	}
	
	// 重新排序
	for(j = 0; j < N; j++)
	{
		p = 0;
		for(i = 0; i < r; i++)
		{
			if (j&(1<<i))
			{
				p+=1<<(r - i - 1);
			}
		}
		FD[j] = X1[p];
	}
	
	// 释放内存
	delete W;
	delete X1;
	delete X2;
}

/*************************************************************************
 *
 * 函数名称：
 *   IFFT()
 *
 * 参数:
 *   complex<double> * FD	- 指向频域值的指针
 *   complex<double> * TD	- 指向时域值的指针
 *   r						－2的幂数
 *
 * 返回值:
 *   无。
 *
 * 说明:
 *   该函数用来实现快速付立叶逆变换。
 *
 ************************************************************************/
void CFreqCalculate::IFFT(complex<double> * FD, complex<double> * TD, int r)
{
	
	int		i;
	
	complex<double> *X;
	
	// 计算付立叶变换点数
	LONG N = 1<<r;
	
	// 分配运算所需存储器
	X = new complex<double>[N];
	
	// 将频域点写入X
	memcpy(X, FD, sizeof(complex<double>) * N);
	
	// 求共轭
	for (i = 0; i < N; i++)
	{
		X[i] = complex<double> (X[i].real(), -X[i].imag());
	}
	
	// 调用快速付立叶变换
	FFT(X, TD, r);
	
	// 求时域点的共轭
	for (i = 0; i < N; i++)
	{
		TD[i] = complex<double> (TD[i].real() / N, -TD[i].imag() / N);
	}
	
	// 释放内存
	delete X;
}

/*************************************************************************
 *
 * 函数名称：
 *   Fourier()
 *
 * 参数:
 *   complex* TD		- 输入的时域序列
 *	 LONG lWidth		- 图象宽度
 *	 LONG lHeight		- 图象高度
 *	 complex* FD		- 输出的频域序列
 *
 * 返回值:
 *   BOOL               - 成功返回TRUE，否则返回FALSE。
 *
 * 说明:
 *   该函数进行二维快速付立叶变换。
 *
 ************************************************************************/
BOOL CFreqCalculate::Fourier(complex<double> * TD, LONG lWidth, LONG lHeight, complex<double> * FD)
{
	
	LONG	i;
	LONG	j;
	LONG    k;

	BeginWaitCursor();
	
	// 进行付立叶变换的宽度和高度（2的整数次方）
	LONG w = 1;
	LONG h = 1;
	int wp = 0;
	int hp = 0;
	
	// 计算进行付立叶变换的宽度和高度（2的整数次方）
	while (w < lWidth)
	{
		w *= 2;
		wp++;
	}
	         
	while (h < lHeight)
	{
		h *= 2;
		hp++;
	}
		
	// 分配内存
	complex<double> *TempT, *TempF;
	TempT = new complex<double>[h];
	TempF = new complex<double>[h];
	
	// 对y方向进行快速付立叶变换
	for (i = 0; i < w * 3; i++)
	{
		// 抽取数据
		for (j = 0; j < h; j++)
			TempT[j] = TD[j * w * 3 + i];
		
		// 一维快速傅立叶变换
		FFT(TempT, TempF, hp);

		// 保存变换结果
		for (j = 0; j < h; j++)
			TD[j * w * 3 + i] = TempF[j];
	}
	
	// 释放内存
	delete TempT;
	delete TempF;

	// 分配内存
	TempT = new complex<double>[w];
	TempF = new complex<double>[w];
	
	// 对x方向进行快速付立叶变换
	for (i = 0; i < h; i++)
	{
		for (k = 0; k < 3; k++)
		{
			// 抽取数据
			for (j = 0; j < w; j++)
				TempT[j] = TD[i * w * 3 + j * 3 + k];

			// 一维快速傅立叶变换
			FFT(TempT, TempF, wp);

			// 保存变换结果
			for (j = 0; j < w; j++)
				FD[i * w * 3 + j * 3 + k] = TempF[j];
		}
	}

	// 释放内存
	delete TempT;
	delete TempF;

	return TRUE;
}

/*************************************************************************
 *
 * 函数名称：
 *   IFourier()
 *
 * 参数:
 *   BYTE* TD			- 返回的空域数据
 *   LONG lWidth		- 空域图象的宽度
 *	 LONG lHeight		- 空域图象的高度
 *	 complex* FD		- 输入的频域数据
 *
 * 返回值:
 *   BOOL               - 成功返回TRUE，否则返回FALSE。
 *
 * 说明:
 *   该函数进行二维快速付立叶逆变换。
 *
 ************************************************************************/
BOOL CFreqCalculate::IFourier(BYTE* TD, LONG lWidth, LONG lHeight, complex<double> * FD)
{
	
	LONG	i;
	LONG	j;
	LONG    k;

	BeginWaitCursor();
	
	// 进行付立叶变换的宽度和高度（2的整数次方）
	LONG w = 1;
	LONG h = 1;
	int wp = 0;
	int hp = 0;
	
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

	LONG lLineBytes = m_pDIB->RowLen();

	// 分配内存
	complex<double> *TempT, *TempF;
	TempT = new complex<double>[w];
	TempF = new complex<double>[w];
	
	// 对x方向进行快速付立叶变换
	for (i = 0; i < h; i++)
	{
		for (k = 0; k < 3; k++)
		{
			// 抽取数据
			for (j = 0; j < w; j++)
				TempF[j] = FD[i * w * 3 + j * 3 + k];

			// 一维快速傅立叶变换
			IFFT(TempF, TempT, wp);

			// 保存变换结果
			for (j = 0; j < w; j++)
				FD[i * w * 3 + j * 3 + k] = TempT[j];
		}
	}

	// 释放内存
	delete TempT;
	delete TempF;
	
	TempT = new complex<double>[h];
	TempF = new complex<double>[h];

	// 对y方向进行快速付立叶变换
	for (i = 0; i < w * 3; i++)
	{
		// 抽取数据
		for (j = 0; j < h; j++)
			TempF[j] = FD[j * w * 3 + i];
		
		// 一维快速傅立叶变换
		IFFT(TempF, TempT, hp);

		// 保存变换结果
		for (j = 0; j < h; j++)
			FD[j * w * 3 + i] = TempT[j];
	}
	
	// 释放内存
	delete TempT;
	delete TempF;

	for (i = 0; i < h; i++)
	{
		for (j = 0; j < w * 3; j++)
		{
			if (i < lHeight && j < lLineBytes)
				*(TD + i * lLineBytes + j) = (BYTE)FD[i * w * 3 + j].real();
		}
	}

	return TRUE;
}

/*************************************************************************
 *
 * 函数名称：
 *   DIBFourier()
 *
 * 参数:
 *   HDIB	hDIB		- 待处理的DIB
 *
 * 返回值:
 *   BOOL               - 成功返回TRUE，否则返回FALSE。
 *
 * 说明:
 *   该函数用来对图像进行付立叶变换。
 *
 ************************************************************************/
BOOL CFreqCalculate::DIBFourier()
{
	// 指向源图像的指针
	BYTE*	lpSrc;

	// 中间变量
	double	dTemp;
	LONG TI,TJ;
	
	LONG	i;
	LONG	j;
	
	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	if (m_pDIB->Channel() < 3)
	{
		MessageBox(L"请先将其转换为24位色(或32位色)，再进行处理！", L"系统提示" , MB_ICONINFORMATION | MB_OK);
		return FALSE;
	}

	BeginWaitCursor();
	
	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();

	LONG lLineBytes = m_pDIB->RowLen();

	// 进行付立叶变换的宽度和高度（2的整数次方）
	LONG w = 1;
	LONG h = 1;
	int wp = 0;
	int hp = 0;
	
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

	// 分配内存
	complex<double> *FD, *TD, *TempD;
	FD = new complex<double>[w * h * 3];
	TD = new complex<double>[w * h * 3];
	TempD =  new complex<double>[w * h * 3];
	
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

	// 进行频谱分析
	if (Fourier(TD, lWidth, lHeight, FD) == FALSE)
		return FALSE;

	// 释放内存
	delete []TD;

	// 将原点放置于图像中心位置
	for(i = 0; i < h; i++)
	{
		for(j = 0; j < 3 * w; j++)
		{
			if (i < h / 2)
				TI = i + h / 2;
			else
				TI = i - h / 2;

			if (j < w * 3 /2)
				TJ = j + 3 * w / 2;
			else 
				TJ = j - 3 * w / 2;

			// 保存转换后的频谱图像
			TempD[i * w * 3 + j] = FD[TI * w * 3 + TJ];
		}
	}

	// 行
	for(i = (int)(h - lHeight) / 2; i < (int)(h + lHeight) / 2; i++)
	{
		// 列
		for(j = (int)(w * 3 - lLineBytes) / 2; j < (int)(w * 3 + lLineBytes) / 2; j += 3)
		{
			// 计算频谱
			dTemp = sqrt(1.0*TempD[w * 3 * i + j].real() * TempD[w * 3 * i + j].real() + 
				         TempD[w * 3 * i + j].imag() * TempD[w * 3 * i + j].imag()) / 100;

			// 判断是否超过255
			if (dTemp > 255)
			{
				// 对于超过的，直接设置为255
				dTemp = 255;
			}

			// 限制为原图大小范围
			TI = i - (h - lHeight) / 2;
			TJ = j / 3 - (w - lWidth) / 2;
			
			// 对应象素指针
			lpSrc = (BYTE*)lpDIBBits + lLineBytes * TI + TJ * 3;

			// 更新源图像
			* (lpSrc) = (BYTE) (dTemp);
			* (lpSrc + 1) = (BYTE) (dTemp);
			* (lpSrc + 2) = (BYTE) (dTemp);
		}
	}

	// 删除临时变量
	delete []FD;
	delete []TempD;
	
	EndWaitCursor();
	
	return TRUE;
}

/*************************************************************************
 *
 * 函数名称：
 *   DCT()
 *
 * 参数:
 *   double * f				- 指向时域值的指针
 *   double * F				- 指向频域值的指针
 *   r						－2的幂数
 *
 * 返回值:
 *   无。
 *
 * 说明:
 *   该函数用来实现一维快速离散余弦变换
 *
 ************************************************************************/
void CFreqCalculate::DCT(double *f, double *F, int r)
{
	
	int		i;
	
	// 中间变量
	double	dTemp;
	
	// 计算离散余弦变换点数
	LONG N = 1<<r;
	
	// 申请并分配内存
	complex<double> *XIn;
	complex<double> *XOut;
	XIn = new complex<double>[N * 2];
	XOut = new complex<double>[N * 2];
	
	// 赋初值为0
	memset(XIn, 0, sizeof(complex<double>) * N * 2);
	memset(XOut, 0, sizeof(complex<double>) * N * 2);
	
	// 将时域点写入数组X
	for (i = 0; i < N; i++)
		XIn[i] = complex<double>(*(f + i), 0);
	
	// 调用快速付立叶变换
	FFT(XIn, XOut, r + 1);
	
	// 调整系数
	dTemp = 1 / sqrt(1.0*N);
	
	// 求F[0]
	F[0] = XOut[0].real() * dTemp;
	
	dTemp *= sqrt(1.0*2);
	
	// 求F[u]
	for (i = 1; i < N; i++)
		*(F + i) = (XOut[i].real() * cos(i * PI / (N * 2)) + XOut[i].imag() * sin(i * PI / (N * 2))) * dTemp;
	
	// 释放内存
	delete[] XIn;
	delete[] XOut;
}

/*************************************************************************
 *
 * 函数名称：
 *   IDCT()
 *
 * 参数:
 *   double * F				- 指向频域值的指针
 *   double * f				- 指向时域值的指针
 *   r						－2的幂数
 *
 * 返回值:
 *   无。
 *
 * 说明:
 *   该函数实现一维快速离散余弦逆变换
 *
 ************************************************************************/
void CFreqCalculate::IDCT(double *F, double *f, int r)
{
	
	int		i;
	
	// 中间变量
	double	dTemp, d0;
		
	// 计算离散余弦变换点数
	LONG N = 1<<r;
	
	// 分配内存
	complex<double> *XIn;
	complex<double> *XOut;
	XIn = new complex<double>[N * 2];
	XOut = new complex<double>[N * 2];

	// 赋初值为0
	memset(XIn, 0, sizeof(complex<double>) * N * 2);
	memset(XOut, 0, sizeof(complex<double>) * N * 2);
	
	// 将频域变换后点写入数组X
	for (i = 0; i < N; i++)
		XIn[i] = complex<double>(F[i] * cos(i * PI / (N * 2)), F[i] * sin(i * PI / (N * 2)));
	
	// 调用快速付立叶反变换
	IFFT(XIn, XOut, r + 1);
	
	// 调整系数
	dTemp = sqrt(1.0*2.0 / N);
	d0 = (sqrt(1.0*1.0 / N) - dTemp) * F[0];
	
	// 计算f(x)
	for (i = 0; i < N; i++)
		f[i] = d0 + XOut[i].real()* dTemp * 2 * N;
	
	// 释放内存
	delete[] XIn;
	delete[] XOut;
}

/*************************************************************************
 *
 * 函数名称：
 *   FreqDCT()
 *
 * 参数:
 *   double* f			- 输入的时域序列
 *   double* F			- 输出的频域序列
 *	 LONG lWidth		- 图象宽度
 *	 LONG lHeight		- 图象高度
 *
 * 返回值:
 *   BOOL               - 成功返回TRUE，否则返回FALSE。
 *
 * 说明:
 *   该函数进行二维快速离散余弦变换。
 *
 ************************************************************************/
BOOL CFreqCalculate::FreqDCT(double *f, double *F, LONG lWidth, LONG lHeight)
{
	
	LONG	i;
	LONG	j;
	LONG    k;

	BeginWaitCursor();
	
	// 进行离散余弦变换的宽度和高度（2的整数次方）
	LONG w = 1;
	LONG h = 1;
	int wp = 0;
	int hp = 0;
	
	// 计算进行离散余弦变换的宽度和高度（2的整数次方）
	while (w < lWidth)
	{
		w *= 2;
		wp++;
	}
	
	while (h < lHeight)
	{
		h *= 2;
		hp++;
	}
		
	// 分配内存
	double* TempIn = new double[h];
	double* TempOut = new double[h];
	
	// 对y方向进行离散余弦变换
	for (i = 0; i < w * 3; i++)
	{
		// 抽取数据
		for (j = 0; j < h; j++)
			TempIn[j] = f[j * w * 3 + i];
		
		// 一维快速离散余弦变换
		DCT(TempIn, TempOut, hp);

		// 保存变换结果
		for (j = 0; j < h; j++)
			f[j * w * 3 + i] = TempOut[j];
	}
	
	// 释放内存
	delete TempIn;
	delete TempOut;

	// 分配内存
	TempIn = new double[w];
	TempOut = new double[w];
	
	// 对x方向进行快速离散余弦变换
	for (i = 0; i < h; i++)
	{
		for (k = 0; k < 3; k++)
		{
			// 抽取数据
			for (j = 0; j < w; j++)
				TempIn[j] = f[i * w * 3 + j * 3 + k];

			// 一维快速离散余弦变换
			DCT(TempIn, TempOut, wp);

			// 保存变换结果
			for (j = 0; j < w; j++)
				F[i * w * 3 + j * 3 + k] = TempOut[j];
		}
	}

	// 释放内存
	delete TempIn;
	delete TempOut;

	return TRUE;
}

/*************************************************************************
 *
 * 函数名称：
 *   IFreqDCT()
 *
 * 参数:
 *   double* f			- 输入的时域序列
 *   double* F			- 输出的频域序列
 *	 LONG lWidth		- 图象宽度
 *	 LONG lHeight		- 图象高度
 *
 * 返回值:
 *   BOOL               - 成功返回TRUE，否则返回FALSE。
 *
 * 说明:
 *   该函数进行二维快速离散余弦逆变换。
 *
 ************************************************************************/
BOOL CFreqCalculate::IFreqDCT(double *f, double *F, LONG lWidth, LONG lHeight)
{
	
	LONG	i;
	LONG	j;
	LONG    k;

	BeginWaitCursor();
	
	// 进行离散余弦变换的宽度和高度（2的整数次方）
	LONG w = 1;
	LONG h = 1;
	int wp = 0;
	int hp = 0;
	
	// 计算进行付立叶变换的宽度和高度（2的整数次方）
	while (w < lWidth)
	{
		w *= 2;
		wp++;
	}
	
	while (h < lHeight)
	{
		h *= 2;
		hp++;
	}

	LONG lLineBytes = m_pDIB->RowLen();

	// 分配内存
	double* TempIn = new double[w];
	double* TempOut = new double[w];
	
	// 对x方向进行快速付立叶变换
	for (i = 0; i < h; i++)
	{
		for (k = 0; k < 3; k++)
		{
			// 抽取数据
			for (j = 0; j < w; j++)
				TempIn[j] = F[i * w * 3 + j * 3 + k];

			// 一维快速傅立叶变换
			IDCT(TempIn, TempOut, wp);

			// 保存变换结果
			for (j = 0; j < w; j++)
				F[i * w * 3 + j * 3 + k] = TempOut[j];
		}
	}

	// 释放内存
	delete TempIn;
	delete TempOut;
	
	TempIn = new double[h];
	TempOut = new double[h];

	// 对y方向进行快速付立叶变换
	for (i = 0; i < w * 3; i++)
	{
		// 抽取数据
		for (j = 0; j < h; j++)
			TempIn[j] = F[j * w * 3 + i];
		
		// 一维快速傅立叶变换
		IDCT(TempIn, TempOut, hp);

		// 保存变换结果
		for (j = 0; j < h; j++)
			F[j * w * 3 + i] = TempOut[j];
	}
	
	// 释放内存
	delete TempIn;
	delete TempOut;
	
	for (i = 0; i < h; i++)
	{
		for (j = 0; j < w * 3; j++)
		{
			if (i < lHeight && j < lLineBytes)
				*(f + i * lLineBytes + j) = F[i * w * 3 + j];
		}
	}

	return TRUE;
}

/*************************************************************************
 *
 * 函数名称：
 *   DIBDCT()
 *
 * 参数:
 *   HDIB  hDIB		    - 待处理的DIB
 *
 * 返回值:
 *   BOOL               - 成功返回TRUE，否则返回FALSE。
 *
 * 说明:
 *   该函数用来对图像进行离散余弦变换。
 *
 ************************************************************************/
BOOL CFreqCalculate::DIBDCT()
{
	// 进行离散余弦变换的宽度和高度（2的整数次方）
	LONG	i;
	LONG	j;
	
	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	if (m_pDIB->Channel() < 3)
	{
		MessageBox(L"请先将其转换为24位色(或32位色)，再进行处理！", L"系统提示" , MB_ICONINFORMATION | MB_OK);
		return FALSE;
	}

	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();
	
	LONG lLineBytes = m_pDIB->RowLen();
	
	// 进行离散余弦变换的宽度和高度（2的整数次方）
	LONG w = 1;
	LONG h = 1;
	int wp = 0;
	int hp = 0;
	
	// 计算进行离散余弦变换的宽度和高度（2的整数次方）
	while (w < lWidth)
	{
		w *= 2;
		wp++;
	}
	
	while (h < lHeight)
	{
		h *= 2;
		hp++;
	}

	// 分配内存
	double *f = new double[w * h * 3];
	double *F = new double[w * h * 3];

	// 向时域赋值并补零
	for (i = 0; i < h; i++)
	{
		for (j = 0; j < w * 3; j++)
		{
			if (i < lHeight && j < lLineBytes)
				f[i * w * 3 + j] = *(BYTE*)(lpDIBBits + lLineBytes * i + j);
			else
				f[w * i * 3 + j] = 0.0f;
		}
	}

	// 进行频谱分析
	if (FreqDCT(f, F,lWidth, lHeight) == FALSE)
		return FALSE;

	// 更新所有象素
	for (i = 0; i < lHeight; i++)
	{
		for (j = 0; j < lLineBytes; j++)
		{
			// 判断是否超过255
			if (fabs(F[i * w * 3 + j]) > 255)
			{
				// 对于超过的，直接设置为255
				*(BYTE*)(lpDIBBits + lLineBytes * (lHeight - 1 - i) + j) = 255;
			}
			else
			{
				// 如果没有超过，则按实际计算结果赋值
				*(BYTE*)(lpDIBBits + lLineBytes * (lHeight - 1 - i) + j) = (BYTE)fabs(F[i * w * 3 + j]);
			}
		}
	}

	// 释放内存
	delete []f;
	delete[] F;

	return TRUE;
}

/*************************************************************************
 *
 * 函数名称：
 *   WALSH()
 *
 * 参数:
 *   double* f			- 输入的时域序列
 *   double* F			- 输出的频域序列
 *   int r				- 2的幂数		
 *
 * 返回值:
 *   BOOL               - 成功返回TRUE，否则返回FALSE。
 *
 * 说明:
 *   该函数进行一维快速沃尔什——哈达马变换。
 *
 ************************************************************************/
void CFreqCalculate::WALSH(double *f, double *F, int r)
{
	
	LONG	i;
	LONG	j;
	LONG	k;
	
	// 中间变量
	int		p;	
	double* X;
	
	// 计算快速沃尔什变换点数
	LONG N = 1 << r;
	
	// 分配运算所需的数组
	double* X1 = new double[N];
	double* X2 = new double[N];
	
	// 将时域点写入数组X1
	memcpy(X1, f, sizeof(double) * N);
	
	// 蝶形运算
	for (k = 0; k < r; k++)
	{
		for (j = 0; j < 1<<k; j++)
		{
			for (i = 0; i < 1<<(r - k - 1); i++)
			{
				p = j * (1<<(r - k));
				X2[i + p] = X1[i + p] + X1[i + p + (int)(1<<(r - k - 1))];
				X2[i + p + (int)(1<<(r - k - 1))] = X1[i + p] - X1[i + p + (int)(1<<(r - k - 1))];
			}
		}
		
		// 互换X1和X2  
		X = X1;
		X1 = X2;
		X2 = X;
	}
	
	// 调整系数
	for (j = 0; j < N; j++)
	{
		p = 0;
		for (i = 0; i < r; i++)
		{
			if (j & (1<<i))
			{
				p += 1<<(r - i - 1);
			}
		}

		F[j] = X1[p] / N;
	}
	
	// 释放内存
	delete X1;
	delete X2;
}

/*************************************************************************
 *
 * 函数名称：
 *   IWALSH()
 *
 * 参数:
 *   double* f			- 输入的时域序列
 *   double* F			- 输出的频域序列
 *   int r				- 2的幂数		
 *
 * 返回值:
 *   BOOL               - 成功返回TRUE，否则返回FALSE。
 *
 * 说明:
 *   该函数进行一维快速沃尔什——哈达马逆变换。
 *
 ************************************************************************/
void CFreqCalculate::IWALSH(double *F, double *f, int r)
{
	
	int		i;
	
	// 计算变换点数
	LONG N = 1 << r;
	
	// 调用快速沃尔什－哈达玛变换进行反变换
	WALSH(F, f, r);
	
	// 调整系数
	for (i = 0; i < N; i++)
		f[i] *= N;
}

/*************************************************************************
 *
 * 函数名称：
 *   FreqWALSH()
 *
 * 参数:
 *   double* f			- 输入的时域序列
 *   double* F			- 输出的频域序列
 *	 LONG lWidth		- 图象宽度
 *	 LONG lHeight		- 图象高度
 *
 * 返回值:
 *   BOOL               - 成功返回TRUE，否则返回FALSE。
 *
 * 说明:
 *   该函数进行二维快速沃尔什——哈达玛变换。
 *
 ************************************************************************/
BOOL CFreqCalculate::FreqWALSH(double *f, double *F, LONG lWidth, LONG lHeight)
{
	
	LONG	i;
	LONG	j;
	LONG    k;

	BeginWaitCursor();
	
	// 进行离散余弦变换的宽度和高度（2的整数次方）
	LONG w = 1;
	LONG h = 1;
	int wp = 0;
	int hp = 0;
	
	// 计算进行离散余弦变换的宽度和高度（2的整数次方）
	while (w < lWidth)
	{
		w *= 2;
		wp++;
	}
	
	while (h < lHeight)
	{
		h *= 2;
		hp++;
	}
		
	// 分配内存
	double* TempIn = new double[h];
	double* TempOut = new double[h];
	
	// 对y方向进行离散余弦变换
	for (i = 0; i < w * 3; i++)
	{
		// 抽取数据
		for (j = 0; j < h; j++)
			TempIn[j] = f[j * w * 3 + i];
		
		// 一维快速离散余弦变换
		WALSH(TempIn, TempOut, hp);

		// 保存变换结果
		for (j = 0; j < h; j++)
			f[j * w * 3 + i] = TempOut[j];
	}
	
	// 释放内存
	delete TempIn;
	delete TempOut;

	// 分配内存
	TempIn = new double[w];
	TempOut = new double[w];
	
	// 对x方向进行快速离散余弦变换
	for (i = 0; i < h; i++)
	{
		for (k = 0; k < 3; k++)
		{
			// 抽取数据
			for (j = 0; j < w; j++)
				TempIn[j] = f[i * w * 3 + j * 3 + k];

			// 一维快速离散余弦变换
			WALSH(TempIn, TempOut, wp);

			// 保存变换结果
			for (j = 0; j < w; j++)
				F[i * w * 3 + j * 3 + k] = TempOut[j];
		}
	}

	// 释放内存
	delete TempIn;
	delete TempOut;

	return TRUE;
}

/*************************************************************************
 *
 * 函数名称：
 *   IFreqWALSH()
 *
 * 参数:
 *   double* f			- 输入的时域序列
 *   double* F			- 输出的频域序列
 *	 LONG lWidth		- 图象宽度
 *	 LONG lHeight		- 图象高度
 *
 * 返回值:
 *   BOOL               - 成功返回TRUE，否则返回FALSE。
 *
 * 说明:
 *   该函数进行二维快速沃尔什——哈达玛逆变换。
 *
 ************************************************************************/
BOOL CFreqCalculate::IFreqWALSH(double *f, double *F, LONG lWidth, LONG lHeight)
{
	
	LONG	i;
	LONG	j;
	LONG    k;

	BeginWaitCursor();
	
	// 赋初值
	LONG w = 1;
	LONG h = 1;
	int wp = 0;
	int hp = 0;
	
	// 计算进行付立叶变换的宽度和高度（2的整数次方）
	while (w < lWidth)
	{
		w *= 2;
		wp++;
	}
	
	while (h < lHeight)
	{
		h *= 2;
		hp++;
	}

	LONG lLineBytes = m_pDIB->RowLen();

	// 分配内存
	double* TempIn = new double[w];
	double* TempOut = new double[w];
	
	// 对x方向进行快速付立叶变换
	for (i = 0; i < h; i++)
	{
		for (k = 0; k < 3; k++)
		{
			// 抽取数据
			for (j = 0; j < w; j++)
				TempIn[j] = F[i * w * 3 + j * 3 + k];

			// 一维快速傅立叶变换
			IWALSH(TempIn, TempOut, wp);

			// 保存变换结果
			for (j = 0; j < w; j++)
				F[i * w * 3 + j * 3 + k] = TempOut[j];
		}
	}

	// 释放内存
	delete TempIn;
	delete TempOut;
	
	TempIn = new double[h];
	TempOut = new double[h];

	// 对y方向进行快速付立叶变换
	for (i = 0; i < w * 3; i++)
	{
		// 抽取数据
		for (j = 0; j < h; j++)
			TempIn[j] = F[j * w * 3 + i];
		
		// 一维快速傅立叶变换
		IWALSH(TempIn, TempOut, hp);

		// 保存变换结果
		for (j = 0; j < h; j++)
			F[j * w * 3 + i] = TempOut[j];
	}
	
	// 释放内存
	delete TempIn;
	delete TempOut;

	for (i = 0; i < h; i++)
	{
		for (j = 0; j < w * 3; j++)
		{
			if (i < lHeight && j < lLineBytes)
				*(f + i * lLineBytes + j) = F[i * w * 3 + j];
		}
	}

	return TRUE;
}

/*************************************************************************
 *
 * 函数名称：
 *   DIBWalsh()
 *
 * 参数:
 *   HDIB	hDIB		- 待处理的DIB
 *
 * 返回值:
 *   BOOL               - 成功返回TRUE，否则返回FALSE。
 *
 * 说明:
 *   该函数对图象进行二维快速沃尔什——哈达马变换。
 *
 ************************************************************************/
BOOL CFreqCalculate::DIBWalsh()
{
	
	LONG	i;
	LONG	j;
	
	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	if (m_pDIB->Channel() < 3)
	{
		MessageBox(L"请先将其转换为24位色(或32位色)，再进行处理！", L"系统提示" , MB_ICONINFORMATION | MB_OK);
		return FALSE;
	}
	
	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();

	LONG lLineBytes = m_pDIB->RowLen();
	
	// 进行沃尔什——哈达玛变换的宽度和高度（2的整数次方）
	LONG w = 1;
	LONG h = 1;
	int wp = 0;
	int hp = 0;
	
	// 计算进行离散余弦变换的宽度和高度（2的整数次方）
	while (w < lWidth)
	{
		w *= 2;
		wp++;
	}
	
	while (h < lHeight)
	{
		h *= 2;
		hp++;
	}
	
	// 分配内存
	double *f = new double[w * h * 3];
	double *F = new double[w * h * 3];
	
	// 向时域赋值并补零
	for (i = 0; i < h; i++)
	{
		for (j = 0; j < w * 3; j++)
		{
			if (i < lHeight && j < lLineBytes)
				f[i * w * 3 + j] = *(BYTE*)(lpDIBBits + lLineBytes * i + j);
			else
				f[w * i * 3 + j] = 0.0f;
		}
	}

	// 进行频谱分析
	if (FreqWALSH(f, F,lWidth, lHeight) == FALSE)
		return FALSE;
	
	// 更新所有象素
	for (i = 0; i < lHeight; i++)
	{
		for (j = 0; j < lLineBytes; j++)
		{
			// 判断是否超过255
			if (fabs(F[i * w * 3 + j] * 1000) > 255)
			{
				// 对于超过的，直接设置为255
				*(BYTE*)(lpDIBBits + lLineBytes * (lHeight - 1 - i) + j) = 255;
			}
			else
			{
				// 如果没有超过，则按实际计算结果赋值
				*(BYTE*)(lpDIBBits + lLineBytes * (lHeight - 1 - i) + j) = (BYTE)fabs(F[i * w * 3 + j] * 1000);
			}
		}
	}

	//释放内存
	delete[] f;
	delete[] F;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////
//																	     //
//							小波变换									 //
//																		 //
///////////////////////////////////////////////////////////////////////////

#define DD 13
float h[DD] = { -0.00332761f,0.00569794f,	0.0196637f,	-0.0482603f,	-0.0485391f,
				0.292562f,	0.564406f,	0.292562f,	-0.0485391f,	-0.0482602f,
				-0.0196637f,	0.00569794f,	-0.0033276f};
float g[DD] = {0.00332761f,  0.00569794f,	-0.0196637f,	-0.0482603f,	0.0485391f,
			   0.292562f,	-0.564406f,	0.292562f,	0.0485391f,	-0.0482602f,
			   0.0196637f,	0.00569794f,	0.0033276f};
float hi[DD];
float gi[DD];

int CFreqCalculate::a(int nX, int nXSize)
{
	if (nX < 0)
		nX = -nX;
	if (nX >= nXSize)
		nX = nXSize * 2 - nX - 2;
	return nX;
}

/*************************************************************************
 *
 * 函数名称：
 *   DWT()
 *
 * 参数:
 *   HDIB	hDIB		- 待处理的DIB
 *
 * 返回值:
 *   void	            - 无返回值
 *
 * 说明:
 *   该函数对二维点阵进行离散小波变换。
 *
 ************************************************************************/
void CFreqCalculate::DWT(BYTE* lpData, int nX, int nY, int nXSize, int nYSize)
{
	
	int i, j, k, n;

	// 中间变量
	float temp1, temp2;
	float* BufferX;
	float* BufferY;

	BufferX = new float[nXSize];
	BufferY = new float[nYSize];

	// 水平方向
	for (n = 0; n < nY; n++)
	{
		for (i = 0; i < nX; i += 2)
		{
			temp1 = temp2 = 0;
			for (j = -(DD - 1) / 2; j <= (DD - 1) / 2; j++)
				temp1 = temp1 + *(lpData + n * nX + a(i + j, nX)) * h[j + (DD - 1) / 2];
			for (j = -(DD - 1) / 2 + 1; j <= (DD - 1) / 2 + 1; j++)
				temp2 = temp2 + *(lpData + n * nX + a(i + j, nX)) * g[j + (DD - 1) / 2 - 1];
			BufferX[i / 2] = temp1;
			BufferX[i / 2 + nX / 2] = temp2;
		}

		// 回存数据
		for (k = 0; k < nX; k++)
			*(lpData + n * nX + k) = (BYTE)BufferX[k];
	}

	// 垂直方向
	for (n = 0; n < nX; n++)
	{
		for (i = 0; i < nY; i += 2)
		{
			temp1 = temp2 = 0;
			for (j = -(DD - 1) / 2; j <= (DD - 1) / 2; j++)
				temp1 = temp1 + *(lpData + a(i + j, nY) * nX + n) * h[j + (DD - 1) / 2];
			for (j = -(DD - 1) / 2 + 1; j <= (DD - 1) / 2 + 1; j++)
				temp2 = temp2 + *(lpData + a(i + j, nY) * nX + n) * g[j + (DD - 1) / 2 - 1];
			BufferY[i / 2] = temp2;
			BufferY[i / 2 + nY / 2] = temp1;
		}

		// 回存数据
		for (k = 0; k < nY; k++)
			*(lpData + k * nX + n) = (BYTE)BufferY[k];
	}
	
	delete[] BufferX;
	delete[] BufferY;
}

/*************************************************************************
 *
 * 函数名称：
 *   IDWT()
 *
 * 参数:
 *   HDIB	hDIB		- 待处理的DIB
 *
 * 返回值:
 *   void	            - 无返回值
 *
 * 说明:
 *   该函数对二维点阵进行小波重构。
 *
 ************************************************************************/
void CFreqCalculate::IDWT(BYTE* lpData, int nX, int nY, int nXSize, int nYSize)
{
	
	int i, j, k, n;

	// 中间变量
	float temp1, temp2;
	float* Buffer1;
	float* Buffer2;

	// 申请空间时多加了一个1，2014年12月7日修改下述两条语句
	Buffer1 = new float[nXSize + 1];
	Buffer2 = new float[nYSize + 1];

	for (i = 0; i < DD; i++)
	{
		hi[i] = h[DD - 1 - i];
		gi[i] = g[DD - 1 - i];
	}

	// 垂直方向
	for (n = 0; n < nX; n++)
	{
		for (k = 0; k < nY / 2; k++)
		{
			Buffer1[k * 2] = *(lpData + k * nX + n);
			Buffer1[k * 2 + 1] = 0;
		}
		for (i = 0; i <  nY; i++)
		{
			temp1 = 0;
			for (j = -(DD - 1) / 2; j <= (DD - 1) / 2; j++)
			{
				temp1 = temp1 + Buffer1[a(i + j, nY)] * hi[j + (DD - 1) / 2];
				Buffer2[i] = temp1;
			}
		}
		for (k = 0; k < nY / 2; k++)
		{
			Buffer1[k * 2] = *(lpData + (k + nY / 2) * nX + n);
			Buffer1[k * 2 + 1] = 0;
		}
		for (i = 0; i < nY; i++)
		{
			temp1 = 0;
			for (j = -(DD - 1) / 2 - 1; j <= (DD - 1) / 2 - 1; j++)
				temp1 = temp1 + Buffer1[a(i + j, nY)] * gi[j + (DD - 1) / 2 + 1];
			temp2 = temp1 + Buffer2[i];
			Buffer2[i] = temp2;
		}

		// 回存数据
		for (k = 0; k < nY; k++)
			*(lpData + k * nX + n) = (BYTE)Buffer2[k];
	}

	// 水平方向
	for (n = 0; n < nY; n++)
	{
		for (k = 0; k < nX / 2; k++)
		{
			Buffer1[k * 2] = *(lpData + n * nX + k);
			Buffer1[k * 2 + 1] = 0;
		}
		
		for (i = 0; i < nX; i++)
		{
			temp1 = 0;
			for (j = -(DD - 1) / 2; j <= (DD - 1) / 2; j++)
				temp1 = temp1 + Buffer1[a(i + j, nX)] * hi[j + (DD - 1) / 2];
			Buffer2[i] = temp1;
		}
		
		for (k = 0; k < nX / 2; k++)
		{
			Buffer1[k * 2] = *(lpData + n * nX + k + nX / 2);
			Buffer1[k * 2 + 1] = 0;
		}

		for (i = 0; i < nX; i++)
		{
			temp1 = 0;
			for (j = -(DD - 1) / 2 - 1; j <= (DD - 1) / 2 - 1; j++)
				temp1 = temp1 + gi[j + (DD - 1) / 2 + 1] * Buffer1[a(i + j, nX)];
			temp2 = temp1 + Buffer2[i];
			Buffer2[i] = temp2;
		}

		// 回存数据
		for (k = 0; k < nX; k++)
			*(lpData + n * nX + k) = (BYTE)(Buffer2[k] * 4);
	}

	delete [] Buffer1;
	delete [] Buffer2;
}

/*************************************************************************
 *
 * 函数名称：
 *   DIBDWT()
 *
 * 参数:
 *   HDIB	hDIB		- 待处理的DIB
 *
 * 返回值:
 *   void	            - 无返回值
 *
 * 说明:
 *   该函数对图象进行二维离散小波变换。
 *
 ************************************************************************/
void CFreqCalculate::DIBDWT()
{
	
	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	// 判断是否是可以处理
	if (m_pDIB->BitCount() != 8)
	{
		
		MessageBox(L"请先将其转换为8位色位图，再进行处理！", L"系统提示" , MB_ICONINFORMATION | MB_OK);
		
		return;
	}
	
	BeginWaitCursor();
	
	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();

	// 进行小波分解
	DWT(lpDIBBits, lWidth, lHeight, lWidth, lHeight);

	EndWaitCursor();
}

/*************************************************************************
 *
 * 函数名称：
 *   DIBIDWT()
 *
 * 参数:
 *   HDIB	hDIB		- 待处理的DIB
 *
 * 返回值:
 *   void	            - 无返回值
 *
 * 说明:
 *   该函数对图象进行二维小波重构。
 *
 ************************************************************************/
void CFreqCalculate::DIBIDWT()
{
	
	BYTE* lpDIBBits = m_pDIB->FindAddress();
	
	// 判断是否是可以处理
	if (m_pDIB->BitCount() != 8)
	{
		
		MessageBox(L"请先将其转换为8位色位图，再进行处理！", L"系统提示" , MB_ICONINFORMATION | MB_OK);
		
		return;
	}
	
	BeginWaitCursor();
	
	LONG lWidth = m_pDIB->Width();
	
	LONG lHeight = m_pDIB->Height();

	// 进行小波重构	
	IDWT(lpDIBBits, lWidth, lHeight, lWidth, lHeight);

	EndWaitCursor();
}


