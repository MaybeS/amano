// acognitive_post_def.h
#pragma once


#ifdef USE_DLLEXPORT
	#define DLLEXPORT	__declspec(dllexport)
#else
	#define DLLEXPORT	__declspec(dllimport)
#endif


#include <list>
#include <vector>
#include <string>

// typedef
typedef void*	AP_HANDLE;


// error
#define RIE_SUCCESS				0
#define RIE_INVALID_PARAMETER	-50 // �߸��� �Ķ����
#define RIE_FILE_NOT_EXIST		-51 // ������ ����
#define RIE_NEED_INIT			-52 // �ʱ�ȭ �ʼ�.
#define RIE_BUSY				-53 // busy


struct TBbox
{
	/** obj�� */
	char		m_obj_name[32];
	/** ��Ȯ�� */
	double		m_confidence;
	/** ���� */
	int x, y, w, h;
};


// ��û�� �̹��� structure.
// ��û���� + �������
struct TRequestImage
{
	char		m_RequestUrl[256];	// request yolo webserver url
	char		m_ImagePath[256];	// �̹��� ���
	BYTE*		m_ImageBuf;		// �̹��� buffer
	DWORD		m_ImageBufLen;
	char		m_SaveRoot[256];		// ������ ��û�̹����� ������ ����������ġ(/�����̱�/pru1/192.168.0.3)
	BOOL		m_bDontSave;	// true �� ���� ����.
	int			m_RequestError;	// 0: success

	TBbox*		m_Result;
	int			m_ResultCount;
};


// request�ݹ�
typedef long (_stdcall *PRequestImageCallBack)( TRequestImage* AImageResult, WPARAM wParam );




// �ʱ�ȭ
extern "C" DLLEXPORT AP_HANDLE _stdcall ap_Init( PRequestImageCallBack ACB, WPARAM wParam );
// ����
extern "C" DLLEXPORT void _stdcall ap_Close( AP_HANDLE& AHandle );
// �̹��� ���Ͽ��� ��û
extern "C" DLLEXPORT long _stdcall ap_Request_ImagePath( AP_HANDLE AHandle, LPCSTR ARequestUrl, LPCSTR AImagePath, LPCSTR ASaveRoot, BOOL ADontSave );
// �̹��� ���ۿ��� ��û
extern "C" DLLEXPORT long _stdcall ap_Request_ImageBuf( AP_HANDLE AHandle, LPCSTR ARequestUrl, BYTE* AImageBuf, DWORD AImageBufLen, LPCSTR ASaveRoot, BOOL ADontSave );




