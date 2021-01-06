
// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#include "vld.h"

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ


#include <afxdisp.h>        // MFC �Զ�����



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // �������Ϳؼ����� MFC ֧��

#include <new>

#include <gdiplus.h>

using namespace Gdiplus;

#define SAFE_DELETE(p) if((p) != NULL){ delete [] (p); (p) = NULL; }	//��ȫɾ��ָ��p

#define SAFE_NEW(type, p) if ((p) == NULL) { (p) = new type; }		//����pΪ��ʱ������ڴ�

#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)				//����ÿ���ֽ���

#define PI 3.141592653

#define RAD(i) (PI * (i) / 180.0f)	//�Ƕ�ת����

#define CHECK_EDITABLE(p) if(NULL == (p) || (p)->IsNull()) return;

#define SETDLGICON(p) SetIcon(AfxGetApp()->LoadIcon(p), TRUE);

#define DELETE_WND(p) if(p) { (p)->DestroyWindow(); delete (p); }

#define memicmp _memicmp

#pragma comment(lib, "res/jpeg.lib")

// USING CUDA, 2016-12-4
#ifdef CUDA
#pragma comment(lib, "cublas.lib")
#pragma comment(lib, "cuda.lib")
#pragma comment(lib, "cudart.lib")
#endif // CUDA

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

// MS Visual Studio 2015
// vs2015�޷������ⲿ����__imp__fprintf �Լ�imp_iob_func����
// https://blog.csdn.net/hutianyou123/article/details/76577827
#if _MSC_VER>=1900
extern "C" { FILE __iob_func[3] = { *stdin,*stdout,*stderr }; }
#pragma comment(lib, "legacy_stdio_definitions.lib")
#endif
