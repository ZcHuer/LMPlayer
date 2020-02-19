#pragma once
#include <string>
using namespace std;

#include "BindDownloadCallback.h"

struct sUpdateInfo;
class CUpdateInstance
{
public:
	CUpdateInstance();
	~CUpdateInstance();

	// 查询是否正在升级
	bool IsDownLoading();
	// 下载升级包
	bool DownloadAndUnzip(LPCSTR lpcstr_url, LPCSTR lpcstr_vername, bool bThread, HWND hWnd = NULL);
	void CancelDownload();
	// 绑定回调句柄
	bool SetCallBackHwnd(HWND hWnd);

	bool Download();
	bool UnZipToPath(const char* filePath, std::string strUnzip);
	bool IsDownloadOk(string sPath);

private:
	bool m_bDownloading;
	string m_strDownLoadUrl;
	string m_strSvrVer;
	CBindDownloadCallback m_pCall;
	HWND m_hMsgWnd;
	HANDLE m_hThread_Download;
};