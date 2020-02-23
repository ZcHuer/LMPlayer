#include "stdafx.h"
#include <WinInet.h>
#include "UpdateInstance.h"
#include "zip\unzip.h"
#include "Update.h"
#include <thread>
#include <corecrt_io.h>
#include "LeTools.h"

#pragma comment(lib,"urlmon.lib")
#pragma comment(lib,"WinInet.lib")

CUpdateInstance::CUpdateInstance()
{
	m_bDownloading = false;
	m_hMsgWnd = NULL;
	//m_pCall.SetMsgWndHandle(m_MsgWnd.GetHwnd());
	m_hThread_Download = INVALID_HANDLE_VALUE;
}

CUpdateInstance::~CUpdateInstance()
{
}

bool CUpdateInstance::IsDownLoading()
{
	return m_bDownloading;
}

unsigned int WINAPI _StartDownloadThread(void *param)
{
	CUpdateInstance * p = (CUpdateInstance *)param;
	if (NULL == p)
		return 0;

	p->Download();
	return 0;
}

bool CUpdateInstance::DownloadAndUnzip(LPCSTR lpcstr_url, LPCSTR lpcstr_vername, bool bThread, HWND hWnd/* = NULL*/)
{
	CancelDownload();
	m_bDownloading = true;
	m_strDownLoadUrl = lpcstr_url;
	m_strSvrVer = lpcstr_vername;
	m_hMsgWnd = hWnd;
	m_pCall.SetMsgWndHandle(m_hMsgWnd);
	
	bool bRet = false;
	if (bThread)
		m_hThread_Download = (HANDLE)_beginthreadex(NULL, 0, _StartDownloadThread, (void *)this, 0, 0);
	else
		bRet = Download();
	
	m_bDownloading = false;
	return bRet;
}

void CUpdateInstance::CancelDownload()
{
	m_hMsgWnd = NULL;
	m_pCall.SetMsgWndHandle(m_hMsgWnd);
	if (INVALID_HANDLE_VALUE != m_hThread_Download)
	{
		TerminateThread(m_hThread_Download, 0);
		CloseHandle(m_hThread_Download);
		m_hThread_Download = INVALID_HANDLE_VALUE;
	}
	return;
}

bool CUpdateInstance::SetCallBackHwnd(HWND hWnd)
{
	m_hMsgWnd = hWnd;
	m_pCall.SetMsgWndHandle(m_hMsgWnd);
	return true;
}

bool CUpdateInstance::Download()
{
	if (m_strDownLoadUrl.length() == 0)
		return false;

	string sUrl = m_strDownLoadUrl;
//	DeleteUrlCacheEntryA(sUrl.c_str());
	//在url后添加随机数，防止从IE缓存中读取。url后加随机数不会影响下载的。
	//如果想要从缓存中提取那么就把下面的注释掉
	CStringA szRand;
	szRand.Format("?skq=%d", GetTickCount());
	sUrl += szRand.GetString();
	char cPath[MAX_PATH] = { 0 };
	GetTempPathA(MAX_PATH, cPath);
	CStringA cstrA;
	DWORD dwTm = ::GetTickCount();
	cstrA.Format("%d.zip", dwTm);
	//string strPath = sUrl.substr(sUrl.rfind("/") + 1);
	string strPath = cPath;
	strPath += cstrA.GetString();	
	HRESULT hr = URLDownloadToFileA(0, sUrl.c_str(), strPath.c_str(), 0/*BINDF_GETNEWESTVERSION | BINDF_IGNORESECURITYPROBLEM*/, &m_pCall);
	if (hr == S_OK)
	{
		if (IsDownloadOk(strPath))
		{
			//解压文件
			CHAR cPath_CurModule[MAX_PATH] = { 0 };
			GetModuleFileNameA(NULL, cPath_CurModule, MAX_PATH);
			PathRemoveFileSpecA(cPath_CurModule);
			PathRemoveFileSpecA(cPath_CurModule);
			string strCurModule = cPath_CurModule;
			string strUnZip = strCurModule;
			strUnZip += "\\";
			strUnZip += m_strSvrVer;

			BOOL result = UnZipToPath(strPath.c_str(), strUnZip);
			if (result)
			{
				// 发送成功方法
				if (m_hMsgWnd)
					::PostMessage(m_hMsgWnd, eUPDATE_Success, 0, 0);
				return true;
			}
			else
			{
				//发送错误-解压失败
				if (m_hMsgWnd)
					::PostMessage(m_hMsgWnd, eUPDATE_Error_Unzip, 0, 0);
				return false;
			}
		}
		else
		{
			// 发送错误-文件错误
			if (m_hMsgWnd)
				::PostMessage(m_hMsgWnd, eUPDATE_Error_File, 0, 0);
			return false;
		}
	}
	else
	{
		// 发送错误-网络错误
		if (m_hMsgWnd)
			::PostMessage(m_hMsgWnd, eUPDATE_Error_Download, 0, 0);
		return false;
	}
}

bool CUpdateInstance::UnZipToPath(const char* filePath, std::string strUnzip)
{
	if (NULL == filePath)
		return false;

	if (0 == strlen(filePath))
		return false;

	if (0 == strUnzip.length())
		return false;

	if ((_access(filePath, 0)) != -1)
	{
		if (!PathFileExistsA(strUnzip.c_str()))
		{
			if (ERROR_SUCCESS != SHCreateDirectoryExA(NULL, strUnzip.c_str(), NULL))
				return false;
		}

		string str = filePath;
		wstring wstr = LeTools::s2ws(str);
		BOOL resultCurrent = SetCurrentDirectoryA(strUnzip.c_str());
		HZIP hz = OpenZip(wstr.c_str(), NULL);
		ZIPENTRY ze;
		GetZipItem(hz, -1, &ze);
		int numitems = ze.index;
		if (0 == numitems)
			return false;
		for (int i = 0; i < numitems; i++)
		{
			GetZipItem(hz, i, &ze);
			UnzipItem(hz, i, ze.name);
		}

		CloseZip(hz);
		return true;
	}

	return false;
}

bool CUpdateInstance::IsDownloadOk(string sPath)
{
	FILE* file = NULL;
	errno_t e = fopen_s(&file, sPath.c_str(), "rb");
	if (file)
	{
		_fseeki64(file, 0, SEEK_END);
		__int64 llSize = _ftelli64(file);
		_fseeki64(file, 0, SEEK_SET);
		char* lpData = (char*)malloc(llSize + 1);
		memset(lpData, 0, llSize + 1);
		fread(lpData, llSize, 1, file);
		fclose(file);

		//MD5 md5(lpData, llSize);
		//string sMd5 = md5.toString();
		//free(lpData);
		//if (stricmp(sMd5.c_str(),m_strMd5Value.c_str())==0)
		//{
			return true;
		//}
	}
	return false;
}
