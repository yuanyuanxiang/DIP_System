
#pragma once

// Morph.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMorph window
#include "dib.h"

class CMorph : public CWnd
{
// Construction
public:
	CMorph();
	CMorph(CDIB* pDIB);

// Implementation
public:
	void MAT();
	void Thick();
	void Thin();
	void VHErosion();
	void VHDilation();
	void CloseOperate();
	void OpenOperate();
	void HDilation();
	void VDilation();
	void VErosion();
	void HErosion();
	virtual ~CMorph();

// Generated message map functions
protected:
	CDIB* m_pDIB;

	DECLARE_MESSAGE_MAP()
};
