//////////////////////////////////////////////////////////////////////////
// 版本：V1.0
// 创建：2011.9.11
// 作者：黄森堂
//------------------------------------------------------------------------
// 功能： HTTP数据类型
//------------------------------------------------------------------------
// 2011.1.8 调整接口方法
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
//由界面告诉下载，是继续，还是中断了
//该回调通知只在【DL_Event_Progress】【DL_Event_Resume】下才有效
enum DLNotify
{
	DL_Notify_Abort = 0,								//中断下载
	DL_Notify_Contine,							//继续下载
};

//由http模块告诉界面，现在发现了事件了。
enum DLEvent
{
	DL_Event_Progress=0,				//下载通知：下载进度
	DL_Event_Complete=3,					//下载通知：下载完成, dwReadByte：0=失败，1=成功
};

enum DLState
{
	DL_Abort = -2,					//用户中断
	DL_Fail = -1,					//下载失败
	DL_Resume = 0,					//断点续传，未下载完
	DL_OK_Has_NoData = 1,			//成功，但无数据
	DL_OK = 2,						//完全下载成功
};

typedef DLNotify (WINAPI* PFNOnProgress)( DLEvent iEvent, DWORD dwReadByte, DWORD dwTotalByte, PVOID pParam);

//[DL_Event_Complete]有两个相关宏
#define DL_RESULT_FAIL				0	//下载失败
#define DL_RESULT_OK				1	//下载成功
#define DL_RESULT_OK_NODATA			2	//成功没数据

//小文件的界线
#define SMALL_FILE_SIZE				(512*1024)		//小于512K都识为小文件
//////////////////////////////////////////////////////////////////////////
typedef struct _tagDLValue
{
	DWORD cbSize;					//结构大小
	DWORD dwLastStatus;				//最后Http响应状态
	FILETIME ftLastModifyTime;		//最后修改时间
	FILETIME ftExpiresTime;			//过期时间
	DWORD dwFileSize;			//数据大小
	BOOL bChunked;				//Http Chunked数据方式

	DWORD dwIP;						//IP地址【只Socket支持】
	WCHAR szIPString[32];			//IP地址字符串【只Socket支持】
}DLVALUE,*PDLVALUE;
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
typedef struct _tagHttpDownloadParam
{
	DWORD cbSize;						//结构大小
	BOOL bThreadDownload;				//开启线程下载
	DWORD dwDownloadTimeOut;			//下载总超时时间，单位：毫秒， 为0表示使用默认值
	HttpMode mode;						//下载方式
	int nTimeOut;						//超时时间（socket的网络超时时间），单位：毫秒， 为0表示使用默认值
	BOOL bAutoRedirect;					//启动重定向支持
	BOOL bAutoSwitchMode;				//内部模块自动切换，socket到winhttp切换
	BOOL bAutoUseDNS;					//使用备用域名
	BOOL bResumeDownload;				//启用断点续传
	BOOL bMultiPartDownload;			//启用片段下载	//已作废 [黄森堂 2012.5.10]
	PFNOnProgress pOnProgress;			//下载进程回调
	PVOID pParam;						//参数
	
	BOOL bUseMultiThreadDownload;		//使用多线程下载
	int nDownloadThreads;				//下载线程数

	BOOL bEnableCookie;					//开启Cookie的支持	//已作废 [黄森堂 2012.5.10]
}HTTPDOWNLOADPARAM,*PHTTPDOWNLOADPARAM;

typedef struct _tagHttpDownloadInfo
{
	DWORD cbSize;								//结构大小
	void* pHttpDownloadFile;					//单线程下载该接口有效，接口：IHttpDownloadFile
	void* pHttpMultiThreadDownloadFile;			//多线程下载该接口有效，接口：IHttpMultiThreadDownloadFile
	FILETIME ftLastTime;						//最后修改时间
	DWORD dwHttpStatus;							//出错时，对应的http状态
}HTTPDOWNLOADINFO,*PHTTPDOWNLOADINFO;

//////////////////////////////////////////////////////////////////////////
//结构初始化辅助宏
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