#pragma once
#include "BindDownloadCallback.h"

#include <string>
using namespace std;
class CRepireInstance
{
public:
	CRepireInstance();
	~CRepireInstance();

	// ��ѯ�Ƿ���������
	bool IsDownLoading();
	// ����������
	bool DownloadAndUnzip(LPCSTR lpcstr_url, LPCSTR lpcstr_vername, bool bThread, HWND hWnd = NULL);
	// �󶨻ص����
	bool SetCallBackHwnd(HWND hWnd);
	// ���ذ�װ����ִ��
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

