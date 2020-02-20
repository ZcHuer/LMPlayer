#include "stdafx.h"
#include <WinInet.h>
#include "RepireInstance.h"
#include "zip\unzip.h"
#include "UpdateRepire.h"
#include <thread>
#include "../Include/LeTools.h"
#include <corecrt_io.h>
#include "../SDK/Flog/FileLog.h"

#pragma comment(lib,"urlmon.lib")
#pragma comment(lib,"WinInet.lib")

CRepireInstance::CRepireInstance()
{
	m_bDownloading = false;
	m_hMsgWnd = NULL;
	//m_pCall.SetMsgWndHandle(m_hMsgWnd.GetHwnd());
}

CRepireInstance::~CRepireInstance()
{
}

bool CRepireInstance::IsDownLoading()
{
	return m_bDownloading;
}

bool CRepireInstance::DownloadAndUnzip(LPCSTR lpcstr_url, LPCSTR lpcstr_vername, bool bThread, HWND hWnd/* = NULL*/)
{
	m_bDownloading = true;
	m_strDownLoadUrl = lpcstr_url;
	m_strSvrVer = lpcstr_vername;
	m_hMsgWnd = hWnd;
	m_pCall.SetMsgWndHandle(m_hMsgWnd);

	bool bRet = false;
	if (bThread)
	{
		thread th = thread(&CRepireInstance::Download, this);
		th.detach();
	}
	else
	{
		bRet = Download();
	}

	m_bDownloading = false;
	return bRet;
}

bool CRepireInstance::DownloadAndRepire(LPCSTR lpcstr_url, LPCSTR lpcstr_vername, bool bThread, HWND hWnd)
{
	m_bDownloading = true;
	m_strDownLoadUrl = lpcstr_url;
	m_strSvrVer = lpcstr_vername;
	m_hMsgWnd = hWnd;
	m_pCall.SetMsgWndHandle(m_hMsgWnd);

	bool bRet = false;
	if (bThread)
	{
		thread th = thread(&CRepireInstance::Download, this);
		th.detach();
	}
	else
	{
		bRet = DownloadSetupAndInstall();
	}

	m_bDownloading = false;
	return bRet;
	return FALSE;
}
bool CRepireInstance::SetCallBackHwnd(HWND hWnd)
{
	m_hMsgWnd = hWnd;
	m_pCall.SetMsgWndHandle(m_hMsgWnd);
	return true;
}
bool CRepireInstance::DownloadSetupAndInstall()
{
	if (m_strDownLoadUrl.length() == 0)
		return false;

	string sUrl = m_strDownLoadUrl;
	DeleteUrlCacheEntryA(sUrl.c_str());
	char cPath[MAX_PATH] = { 0 };
	GetTempPathA(MAX_PATH, cPath);

	string strPath = sUrl.substr(sUrl.rfind("/") + 1);
	strPath = cPath + strPath;

	HRESULT hr = URLDownloadToFileA(0, sUrl.c_str(), strPath.c_str(), BINDF_GETNEWESTVERSION | BINDF_IGNORESECURITYPROBLEM, &m_pCall);
	if (hr == S_OK)
	{
		if (IsDownloadOk(strPath))
		{
			FLOG(L"�ļ��������:%s",LeTools::s2ws(strPath).c_str());
			//��ѹ�ļ�
			CHAR cPath_CurModule[MAX_PATH] = { 0 };
			GetModuleFileNameA(NULL, cPath_CurModule, MAX_PATH);
			PathRemoveFileSpecA(cPath_CurModule);
			PathRemoveFileSpecA(cPath_CurModule);
			string strCurModule = cPath_CurModule;
			string strUnZip = strCurModule;
			strUnZip += "\\LMPlayer";
			CreateDirectoryA(strUnZip.c_str(), NULL);
			//strUnZip += m_strSvrVer;
			FLOG(L"��ѹ�ļ�·����%s",LeTools::s2ws(strUnZip).c_str());
			BOOL result = UnZipToPath(strPath.c_str(), strUnZip);
			if (result)
			{
				FLOG(L"��ѹ�ɹ���%s", LeTools::s2ws(strUnZip));
				// ���ͳɹ�����
				if (m_hMsgWnd)
					::PostMessage(m_hMsgWnd, eUPDATE_Success, 0, 0);
				return true;
			}
			else
			{
				FLOG(L"��ѹʧ�ܣ�%s", LeTools::s2ws(strUnZip));
				//���ʹ���-��ѹʧ��
				if (m_hMsgWnd)
					::PostMessage(m_hMsgWnd, eUPDATE_Error_Unzip, 0, 0);
				return false;
			}
		}
		else
		{
			// ���ʹ���-�ļ�����
			if (m_hMsgWnd)
				::PostMessage(m_hMsgWnd, eUPDATE_Error_File, 0, 0);
			return false;
		}
	}
	else
	{
		// ���ʹ���-�������
		if (m_hMsgWnd)
			::PostMessage(m_hMsgWnd, eUPDATE_Error_Download, 0, 0);
		return false;
	}

}
bool CRepireInstance::Download()
{
	if (m_strDownLoadUrl.length() == 0)
		return false;

	string sUrl = m_strDownLoadUrl;
	DeleteUrlCacheEntryA(sUrl.c_str());
	char cPath[MAX_PATH] = { 0 };
	GetTempPathA(MAX_PATH, cPath);

	string strPath = sUrl.substr(sUrl.rfind("/") + 1);
	strPath = cPath + strPath;

	HRESULT hr = URLDownloadToFileA(0, sUrl.c_str(), strPath.c_str(), BINDF_GETNEWESTVERSION | BINDF_IGNORESECURITYPROBLEM, &m_pCall);
	if (hr == S_OK)
	{
		if (IsDownloadOk(strPath))
		{
			//��ѹ�ļ�
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
				// ���ͳɹ�����
				if (m_hMsgWnd)
					::PostMessage(m_hMsgWnd, eUPDATE_Success, 0, 0);
				return true;
			}
			else
			{
				//���ʹ���-��ѹʧ��
				if (m_hMsgWnd)
					::PostMessage(m_hMsgWnd, eUPDATE_Error_Unzip, 0, 0);
				return false;
			}
		}
		else
		{
			// ���ʹ���-�ļ�����
			if (m_hMsgWnd)
				::PostMessage(m_hMsgWnd, eUPDATE_Error_File, 0, 0);
			return false;
		}
	}
	else
	{
		// ���ʹ���-�������
		if (m_hMsgWnd)
			::PostMessage(m_hMsgWnd, eUPDATE_Error_Download, 0, 0);
		return false;
	}
}

bool CRepireInstance::UnZipToPath(const char* filePath, std::string strUnzip)
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
		FLOG(L"Create directory");
		string str = filePath;
		wstring wstr = LeTools::s2ws(str);
		FLOG(L"str:%s",);
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
	FLOG(L"���ذ�Ȩ�޲���");
	return false;
}

bool CRepireInstance::IsDownloadOk(string sPath)
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
