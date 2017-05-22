
#pragma once

// GrayProc.h : header file
//

#include "dib.h"

class CGrayProc : public CWnd
{
	// Construction
public:
	CGrayProc();
	CGrayProc(CDIB* pDIB);

	// Attributes
public:

	// Operations
public:

	// Implementation
public:
	void CutWave();
	void GetIntensity(int* nNs_Y);
	void DynamicCompress(int fC);
	void PointGML(BYTE bNum, int *npNu, float *fpPu);
	void PointSML(BYTE bNum,int* npNu,float* fpPu);
	void PointDT(BYTE bLow, BYTE bUp);
	void PointDZ(BYTE bLow, BYTE bUp);
	void WindowTrans(BYTE bLow,BYTE bUp);
	void GrayStretch(BYTE bX1,BYTE bY1,BYTE bX2,BYTE bY2);
	void PointEqua();
	void PointInvert();
	void GetIntensity(int *nNs_R, int *nNs_G, int *nNs_B);
	void GetIntensity(float *fPs_R, float *fPs_G, float *fPs_B);
	void GetIntensity(float *fPs_Y);
	void ToGray();
	virtual ~CGrayProc();

	// Generated message map functions
protected:
	CDIB* m_pDIB;
	DECLARE_MESSAGE_MAP()
};
