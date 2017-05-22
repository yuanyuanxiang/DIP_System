
#pragma once

//
//*** ÂË²¨Æ÷ ***
//
class CFilter
{
public:
	int                                 m_nFilterWidth;  //ÂË²¨Æ÷¿í
	int                                m_nFilterLength;  //ÂË²¨Æ÷³¤
	double*                                  m_pFilter;  //ÂË²¨Æ÷ÄÚÈÝ
	BOOL                                 m_bFilterType;  //½×Êý£ºÆæ-0,Å¼-1

public:
	CFilter();
	CFilter(int FilterWidth);
	CFilter(const CFilter &Filter);
	~CFilter();
	BOOL Create(int FilterWidth);
	BOOL Create(CFilter *Filter);
	BOOL Create(BYTE* dataSrc, int& ImageWidth, int& ImageHeight, int& ImageChannel, long& ImageRowlen, int curChannel, int curRow, int curCol, int curSize);
	BOOL Create(double* Data, int Width);
	BOOL Assign(BYTE* dataSrc, int& ImageWidth, int& ImageHeight, int& ImageChannel, long& ImageRowlen, int curChannel, int curRow, int curCol);
	double FindMax();
	double FindMin();
	double FindMed();
	double Multiply(CFilter* Filter);
	double& Visit(int x, int y);
};
