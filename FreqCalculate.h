
#pragma once

// FreqCalculate.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFreqCalculate window
#include "dib.h"
#include <complex>
using namespace std;

class CFreqCalculate : public CWnd
{
// Construction
public:
	CFreqCalculate();
	CFreqCalculate(CDIB* pDIB);

// Attributes
public:

// Operations
public:

// Implementation
public:
	void DIBIDWT();
	void IDWT(BYTE* lpData, int nX, int nY, int nXSize, int nYSize);
	void DIBDWT();
	int a(int nX, int nXSize);
	void DWT(BYTE* lpData, int nX, int nY, int nXSize, int nYSize);
	BOOL IFreqWALSH(double* f, double* F, LONG lWidth, LONG lHeight);
	BOOL FreqWALSH(double* f, double* F, LONG lWidth, LONG lHeight);
	BOOL DIBWalsh();
	void IWALSH(double* F, double* f, int r);
	BOOL IFreqDCT(double* f, double* F, LONG lWidth, LONG lHeight);
	void WALSH(double* f, double* F, int r);
	BOOL FreqDCT(double* f, double* F, LONG lWidth, LONG lHeight);
	BOOL DIBDCT();
	void IDCT(double* F,double* f, int r);
	void DCT(double* f, double* F, int r);
	BOOL IFourier(BYTE* TD, LONG lWidth, LONG lHeight, complex<double> * FD);
	BOOL Fourier(complex<double> * TD, LONG lWidth, LONG lHeight, complex<double> * FD);
	BOOL DIBFourier();
	void IFFT(complex<double> * FD, complex<double> * TD, int r);
	void FFT(complex<double> * TD, complex<double> * FD, int r);
	virtual ~CFreqCalculate();

	// Generated message map functions
protected:
	CDIB* m_pDIB;
	DECLARE_MESSAGE_MAP()
};