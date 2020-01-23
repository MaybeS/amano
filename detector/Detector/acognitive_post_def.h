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
#define RIE_INVALID_PARAMETER	-50 // 잘못된 파라미터
#define RIE_FILE_NOT_EXIST		-51 // 파일이 없다
#define RIE_NEED_INIT			-52 // 초기화 필수.
#define RIE_BUSY				-53 // busy


struct TBbox
{
	/** obj값 */
	char		m_obj_name[32];
	/** 정확도 */
	double		m_confidence;
	/** 영역 */
	int x, y, w, h;
};


// 요청할 이미지 structure.
// 요청정보 + 결과정보
struct TRequestImage
{
	char		m_RequestUrl[256];	// request yolo webserver url
	char		m_ImagePath[256];	// 이미지 경로
	BYTE*		m_ImageBuf;		// 이미지 buffer
	DWORD		m_ImageBufLen;
	char		m_SaveRoot[256];		// 서버에 요청이미지를 저장할 서버폴더위치(/방배사이길/pru1/192.168.0.3)
	BOOL		m_bDontSave;	// true 면 저장 안함.
	int			m_RequestError;	// 0: success

	TBbox*		m_Result;
	int			m_ResultCount;
};


// request콜백
typedef long (_stdcall *PRequestImageCallBack)( TRequestImage* AImageResult, WPARAM wParam );




// 초기화
extern "C" DLLEXPORT AP_HANDLE _stdcall ap_Init( PRequestImageCallBack ACB, WPARAM wParam );
// 해제
extern "C" DLLEXPORT void _stdcall ap_Close( AP_HANDLE& AHandle );
// 이미지 파일에서 요청
extern "C" DLLEXPORT long _stdcall ap_Request_ImagePath( AP_HANDLE AHandle, LPCSTR ARequestUrl, LPCSTR AImagePath, LPCSTR ASaveRoot, BOOL ADontSave );
// 이미지 버퍼에서 요청
extern "C" DLLEXPORT long _stdcall ap_Request_ImageBuf( AP_HANDLE AHandle, LPCSTR ARequestUrl, BYTE* AImageBuf, DWORD AImageBufLen, LPCSTR ASaveRoot, BOOL ADontSave );




