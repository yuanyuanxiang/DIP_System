// DIP_System.h : main header file for the DIP_SYSTEM application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#define MIX_FILE_NUMBER  8  // 最近打开最大文件条数

/////////////////////////////////////////////////////////////////////////////
// CDIP_SystemApp:
// See DIP_System.cpp for the implementation of this class
//

class CDIP_SystemApp : public CWinApp
{
public:
	CDIP_SystemApp();

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	void OnFileMynew();
	afx_msg void OnAppAbout();
	afx_msg void OnAuthorBlog();
	afx_msg void OnAuthorMicroBlog();
	afx_msg void OnFileMyopen();
	afx_msg void OnFileOpenPgm();
	afx_msg void OnFileOpenPbm();
	afx_msg void OnFileOpenPpm();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnHotkeyCtrlN();
};