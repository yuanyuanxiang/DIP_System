
#include "stdafx.h"
#include "Filter.h"

CFilter::CFilter()
{
	m_nFilterWidth = 3;
	m_nFilterLength = 9;
	m_bFilterType = 0;
	m_pFilter = NULL;
}

CFilter::CFilter(int FilterWidth)
{
	m_nFilterWidth = FilterWidth;
	m_nFilterLength = FilterWidth*FilterWidth;
	m_bFilterType = (FilterWidth+1)%2;
	m_pFilter = new double[m_nFilterLength];
}

CFilter::CFilter(const CFilter &Filter)
{
	m_nFilterWidth = Filter.m_nFilterWidth;
	m_nFilterLength = m_nFilterWidth*m_nFilterWidth;
	m_bFilterType = (m_nFilterWidth+1)%2;
	SAFE_DELETE(m_pFilter);
	m_pFilter = new double[m_nFilterLength];
	for (int i = 0; i<m_nFilterLength; i++)
		m_pFilter[i] = Filter.m_pFilter[i];
}

CFilter::~CFilter()
{
	SAFE_DELETE(m_pFilter);
}

//创建一个阶数为FilterWidth的滤波器
BOOL CFilter::Create(int FilterWidth)
{
	m_nFilterWidth = FilterWidth;
	m_nFilterLength = FilterWidth*FilterWidth;
	m_bFilterType = (FilterWidth+1)%2;
	SAFE_DELETE(m_pFilter);
	m_pFilter = new double[m_nFilterLength];
	return TRUE;
}

BOOL CFilter::Create(CFilter *Filter)
{
	m_nFilterWidth = Filter->m_nFilterWidth;
	m_nFilterLength = m_nFilterWidth*m_nFilterWidth;
	m_bFilterType = (m_nFilterWidth+1)%2;
	SAFE_DELETE(m_pFilter);
	m_pFilter = new double[m_nFilterLength];
	for (int i = 0; i<m_nFilterLength; i++)
		m_pFilter[i] = Filter->m_pFilter[i];
	return TRUE;
}

// 从图像创建滤波器。dataSrc：图像数据,2-5：图像信息,curChannel：当前通道,curRow：当前中心点的行坐标,curCol：当前中心点的列坐标,curSize：滤波器大小
BOOL CFilter::Create(BYTE* dataSrc, int& ImageWidth, int& ImageHeight, int& ImageChannel, long& ImageRowlen, int curChannel, int curRow, int curCol, int curSize)
{
	m_nFilterWidth = curSize;
	m_nFilterLength = curSize*curSize;
	m_bFilterType = (curSize+1)%2;
	SAFE_DELETE(m_pFilter);
	m_pFilter = new double[m_nFilterLength];

	int R = curSize/2;
	for(int k = 0, y = curRow-R+m_bFilterType; y<=curRow+R; y++)
	{
		for(int x = curCol-R+m_bFilterType; x<=curCol+R; x++)
		{
			if (0<=x && x<ImageWidth && 0<=y && y<ImageHeight)
			{
				BYTE* pixel = dataSrc + x*ImageChannel + y*ImageRowlen;
				m_pFilter[k] = *(pixel+curChannel);
			}
			else
			{
				m_pFilter[k] = 0;
			}
			k++;
		}
	}
	return TRUE;
}

// 从一维数组创建一个滤波器
BOOL CFilter::Create(double* Data, int Width)
{
	m_nFilterWidth = Width;
	m_nFilterLength = Width*Width;
	m_bFilterType = (Width+1)%2;
	SAFE_DELETE(m_pFilter);
	m_pFilter = new double[m_nFilterLength];
	for (int i = 0; i<m_nFilterLength; i++)
		m_pFilter[i] = Data[i];
	return TRUE;
}

// 用图像修改滤波器。dataSrc：图像数据,2-5：图像信息,curChannel：当前通道,curRow：当前中心点的行坐标,curCol：当前中心点的列坐标
BOOL CFilter::Assign(BYTE* dataSrc, int& ImageWidth, int& ImageHeight, int& ImageChannel, long& ImageRowlen, int curChannel, int curRow, int curCol)
{
	int R = m_nFilterWidth/2;
	for(int s = 0, y = curRow-R+m_bFilterType; y<=curRow+R; y++, s++)
	{
		for(int t = 0, x = curCol-R+m_bFilterType; x<=curCol+R; x++, t++)
		{
			if (0<=x && x<ImageWidth && 0<=y && y<ImageHeight)
			{
				BYTE* pixel = dataSrc + x*ImageChannel + y*ImageRowlen;
				Visit(t, s) = *(pixel+curChannel);
			}
			else
			{
				Visit(t, s) = 0;
			}
		}
	}
	return TRUE;
}

//获取滤波器最大元
double CFilter::FindMax()
{
	double MAX = -1;
	for (int i = 0; i<m_nFilterLength; i++)
	{
		if (m_pFilter[i]>MAX)
		{
			MAX = m_pFilter[i];
		}
	}
	return MAX;
}

//获取滤波器最小元
double CFilter::FindMin()
{
	double MIN = 256;
	for (int i = 0; i<m_nFilterLength; i++)
	{
		if (m_pFilter[i]<MIN)
		{
			MIN = m_pFilter[i];
		}
	}
	return MIN;
}

//获取滤波器中值
double CFilter::FindMed()
{
	double* Temp = new double[m_nFilterLength], SWAP;
	for (int i = 0; i<m_nFilterLength; i++)
	{//拷贝一份
		Temp[i] = m_pFilter[i];
	}
	//对拷贝进行排序，以获取中值
	for (int i = 0; i<m_nFilterLength; i++)
	{
		for (int j = i+1; j<m_nFilterLength; j++)
		{
			if (Temp[j]<Temp[i])
			{
				SWAP = Temp[j];
				Temp[j] = Temp[i];
				Temp[i] = SWAP;
			}
		}
	}
	double MED1 = Temp[m_nFilterLength/2-m_bFilterType];
	double MED2 = Temp[m_nFilterLength/2];
	delete [] Temp;
	return (MED1+MED2)/2;
}

// 像素块相乘
double CFilter::Multiply(CFilter* Filter)
{
	double Result = 0;
	for (int i = 0; i<m_nFilterLength; i++)
		Result += m_pFilter[i] * Filter->m_pFilter[i];
	return Result;
}

//获取滤波器x列y行的值
double& CFilter::Visit(int x, int y)
{
	return m_pFilter[x + y*m_nFilterWidth];
}
