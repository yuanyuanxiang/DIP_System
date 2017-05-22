
#pragma once

// GeoTrans.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGeoTrans window
#include "dib.h"

class CGeoTrans : public CWnd
{
// Construction
public:
	CGeoTrans();
	virtual ~CGeoTrans();
	CGeoTrans(CDIB* pDIB);

// Implementation
public:
	void Rectinication(float fAngle);
	void Rotate(float fAngle);
	BOOL Zoom(int widthnew, int heightnew);
	BOOL ZoomDIB(float fZoomRatio);

// Generated message map functions
protected:
	CDIB* m_pDIB;
	DECLARE_MESSAGE_MAP()
};
