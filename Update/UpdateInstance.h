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

	// ��ѯ�Ƿ���������
	bool IsDownLoading();
	// ����������
	bool DownloadAndUnzip(LPCSTR lpcstr_url, LPCSTR lpcstr_vername, bool bThread, HWND hWnd = NULL);
	void CancelDownload();
	// �󶨻ص����
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