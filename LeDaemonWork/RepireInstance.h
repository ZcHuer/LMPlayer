#pragma once
#include "BindDownloadCallback.h"

#include <string>
using namespace std;
class CRepireInstance
{
public:
	CRepireInstance();
	~CRepireInstance();

	// 查询是否正在升级
	bool IsDownLoading();
	// 下载升级包
	bool DownloadAndUnzip(LPCSTR lpcstr_url, LPCSTR lpcstr_vername, bool bThread, HWND hWnd = NULL);
	// 绑定回调句柄
	bool SetCallBackHwnd(HWND hWnd);
	// 下载安装包并执行
	bool DownloadAndRepire(LPCSTR lpcstr_url, LPCSTR lpcstr_vername, bool bThread, HWND hWnd = NULL);


protected:
	bool Download();
	bool DownloadSetupAndInstall();
	bool UnZipToPath(const char* filePath, std::string strUnzip);
	bool IsDownloadOk(std::string sPath);

private:
	bool m_bDownloading;
	std::string m_strDownLoadUrl;
	std::string m_strSvrVer;
	CBindDownloadCallback m_pCall;
	HWND m_hMsgWnd;
};

