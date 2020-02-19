//////////////////////////////////////////////////////////////////////////
// �汾��V1.0
// ������2011.9.11
// ���ߣ���ɭ��
//------------------------------------------------------------------------
// ���ܣ� HTTP��������
//------------------------------------------------------------------------
// 2011.1.8 �����ӿڷ���
//------------------------------------------------------------------------
// Copyright (c) www.baofeng.com All Rights Reserved
//////////////////////////////////////////////////////////////////////////
#pragma once

enum HttpMode
{
	Http_Mode_Socket,		//Socket
	Http_Mode_Winhttp,		//WinHttp
};

//////////////////////////////////////////////////////////////////////////
//�ɽ���������أ��Ǽ����������ж���
//�ûص�ֻ֪ͨ�ڡ�DL_Event_Progress����DL_Event_Resume���²���Ч
enum DLNotify
{
	DL_Notify_Abort = 0,								//�ж�����
	DL_Notify_Contine,							//��������
};

//��httpģ����߽��棬���ڷ������¼��ˡ�
enum DLEvent
{
	DL_Event_Progress=0,				//����֪ͨ�����ؽ���
	DL_Event_Complete=3,					//����֪ͨ���������, dwReadByte��0=ʧ�ܣ�1=�ɹ�
};

enum DLState
{
	DL_Abort = -2,					//�û��ж�
	DL_Fail = -1,					//����ʧ��
	DL_Resume = 0,					//�ϵ�������δ������
	DL_OK_Has_NoData = 1,			//�ɹ�����������
	DL_OK = 2,						//��ȫ���سɹ�
};

typedef DLNotify (WINAPI* PFNOnProgress)( DLEvent iEvent, DWORD dwReadByte, DWORD dwTotalByte, PVOID pParam);

//[DL_Event_Complete]��������غ�
#define DL_RESULT_FAIL				0	//����ʧ��
#define DL_RESULT_OK				1	//���سɹ�
#define DL_RESULT_OK_NODATA			2	//�ɹ�û����

//С�ļ��Ľ���
#define SMALL_FILE_SIZE				(512*1024)		//С��512K��ʶΪС�ļ�
//////////////////////////////////////////////////////////////////////////
typedef struct _tagDLValue
{
	DWORD cbSize;					//�ṹ��С
	DWORD dwLastStatus;				//���Http��Ӧ״̬
	FILETIME ftLastModifyTime;		//����޸�ʱ��
	FILETIME ftExpiresTime;			//����ʱ��
	DWORD dwFileSize;			//���ݴ�С
	BOOL bChunked;				//Http Chunked���ݷ�ʽ

	DWORD dwIP;						//IP��ַ��ֻSocket֧�֡�
	WCHAR szIPString[32];			//IP��ַ�ַ�����ֻSocket֧�֡�
}DLVALUE,*PDLVALUE;
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
typedef struct _tagHttpDownloadParam
{
	DWORD cbSize;						//�ṹ��С
	BOOL bThreadDownload;				//�����߳�����
	DWORD dwDownloadTimeOut;			//�����ܳ�ʱʱ�䣬��λ�����룬 Ϊ0��ʾʹ��Ĭ��ֵ
	HttpMode mode;						//���ط�ʽ
	int nTimeOut;						//��ʱʱ�䣨socket�����糬ʱʱ�䣩����λ�����룬 Ϊ0��ʾʹ��Ĭ��ֵ
	BOOL bAutoRedirect;					//�����ض���֧��
	BOOL bAutoSwitchMode;				//�ڲ�ģ���Զ��л���socket��winhttp�л�
	BOOL bAutoUseDNS;					//ʹ�ñ�������
	BOOL bResumeDownload;				//���öϵ�����
	BOOL bMultiPartDownload;			//����Ƭ������	//������ [��ɭ�� 2012.5.10]
	PFNOnProgress pOnProgress;			//���ؽ��̻ص�
	PVOID pParam;						//����
	
	BOOL bUseMultiThreadDownload;		//ʹ�ö��߳�����
	int nDownloadThreads;				//�����߳���

	BOOL bEnableCookie;					//����Cookie��֧��	//������ [��ɭ�� 2012.5.10]
}HTTPDOWNLOADPARAM,*PHTTPDOWNLOADPARAM;

typedef struct _tagHttpDownloadInfo
{
	DWORD cbSize;								//�ṹ��С
	void* pHttpDownloadFile;					//���߳����ظýӿ���Ч���ӿڣ�IHttpDownloadFile
	void* pHttpMultiThreadDownloadFile;			//���߳����ظýӿ���Ч���ӿڣ�IHttpMultiThreadDownloadFile
	FILETIME ftLastTime;						//����޸�ʱ��
	DWORD dwHttpStatus;							//����ʱ����Ӧ��http״̬
}HTTPDOWNLOADINFO,*PHTTPDOWNLOADINFO;

//////////////////////////////////////////////////////////////////////////
//�ṹ��ʼ��������
//////////////////////////////////////////////////////////////////////////
#define InitDefaultHttpDownloadParam(downloadParam, bUseAutoRedirect, bUseResumeDownload, bUseMultiPart)\
{\
	downloadParam.cbSize=sizeof(HTTPDOWNLOADPARAM);\
	downloadParam.bThreadDownload= FALSE;\
	downloadParam.dwDownloadTimeOut=0;\
	downloadParam.mode= Http_Mode_Socket;\
	downloadParam.nTimeOut=0;\
	downloadParam.bAutoRedirect=bUseAutoRedirect;\
	downloadParam.bAutoSwitchMode=TRUE;\
	downloadParam.bAutoUseDNS=TRUE;\
	downloadParam.bResumeDownload=bUseResumeDownload;\
	downloadParam.bMultiPartDownload=bUseMultiPart;\
	downloadParam.pOnProgress=NULL;\
	downloadParam.pParam=NULL;\
	downloadParam.bUseMultiThreadDownload=FALSE;\
	downloadParam.nDownloadThreads=0;\
	downloadParam.bEnableCookie=FALSE;\
}