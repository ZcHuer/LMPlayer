#include "stdafx.h"
#include "QwBase.h"
#include <winhttp.h>
#include <TlHelp32.h>
namespace QwBase
{
	string ConvertGBKToUtf8(TCHAR * str)
	{
		string strOut = "";
#ifndef  UNICODE
		//获得临时变量的大小
		int len = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
		wchar_t* wstr = new wchar_t[len + 1];
		memset(wstr, 0, len + 1);
		MultiByteToWideChar(CP_ACP, 0, str, -1, wstr, len);
		len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
		char* lpOut = new char[len + 1];
		memset(lpOut, 0, len + 1);
		WideCharToMultiByte(CP_UTF8, 0, wstr, -1, lpOut, len, NULL, NULL);
		if (wstr) delete[] wstr;
		strOut = lpOut;
		if (lpOut) delete[] lpOut;
#else
		//获得临时变量的大小
		int i = WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
		char* lpOut = new char[i + 1];
		memset(lpOut, 0, i + 1);
		WideCharToMultiByte(CP_UTF8, 0, str, -1, lpOut, i, NULL, NULL);
		strOut = lpOut;
		if (lpOut) delete[] lpOut;
#endif
		return strOut;
	}

    string ConvertUtf8ToGBK(string strUtf8)
    {
        int len = MultiByteToWideChar(CP_UTF8, 0, strUtf8.c_str(), -1, NULL, 0);
        wchar_t * wszGBK = new wchar_t[len];
        memset(wszGBK, 0, len);
        MultiByteToWideChar(CP_UTF8, 0, strUtf8.c_str(), -1, wszGBK, len);
        len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
        char *szGBK = new char[len + 1];
        memset(szGBK, 0, len + 1);
        WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
        string strOut = szGBK;
        delete[] szGBK;
        delete[] wszGBK;
        return strOut;
    }

    string ws2s(wstring str)
    {
        string s = "";
        if (!str.empty()) {
            int nStrLen = str.length();
            SAFE_CREATELP(char, lpBuff, nStrLen * 2);
            WideCharToMultiByte(CP_ACP, 0, str.c_str(), nStrLen, lpBuff, nStrLen * 2, 0, 0);
            s = lpBuff;
            SAFE_DELETE(lpBuff);
        }
        return s;
    }

    wstring s2ws(string str)
    {
        wstring ws = L"";
        if (!str.empty())
        {
            int nStrLen = str.length();
            SAFE_CREATELP(WCHAR, lpBuff, nStrLen);
            MultiByteToWideChar(CP_ACP, 0, str.c_str(), nStrLen, lpBuff, nStrLen);
            ws = lpBuff;
            SAFE_DELETE(lpBuff);
        }
        return ws;
    }

    BOOL IsTextUTF8(char* str, ULONGLONG length)
    {
        DWORD nBytes = 0;//UFT8可用1-6个字节编码,ASCII用一个字节  
        UCHAR chr;
        BOOL bAllAscii = TRUE; //如果全部都是ASCII, 说明不是UTF-8  
        for (int i = 0; i<length; ++i)
        {
            chr = *(str + i);
            if ((chr & 0x80) != 0) // 判断是否ASCII编码,如果不是,说明有可能是UTF-8,ASCII用7位编码,但用一个字节存,最高位标记为0,o0xxxxxxx  
                bAllAscii = FALSE;
            if (nBytes == 0) //如果不是ASCII码,应该是多字节符,计算字节数  
            {
                if (chr >= 0x80)
                {
                    if (chr >= 0xFC && chr <= 0xFD)
                        nBytes = 6;
                    else if (chr >= 0xF8)
                        nBytes = 5;
                    else if (chr >= 0xF0)
                        nBytes = 4;
                    else if (chr >= 0xE0)
                        nBytes = 3;
                    else if (chr >= 0xC0)
                        nBytes = 2;
                    else
                        return FALSE;

                    nBytes--;
                }
            }
            else //多字节符的非首字节,应为 10xxxxxx  
            {
                if ((chr & 0xC0) != 0x80)
                    return FALSE;

                nBytes--;
            }
        }
        if (nBytes > 0) //违返规则  
            return FALSE;
        if (bAllAscii) //如果全部都是ASCII, 说明不是UTF-8  
            return FALSE;

        return TRUE;
    }

    wstring GetSpecialFolderPath(int csidl)
    {
        WCHAR szBuffer[MAX_PATH];
        SHGetSpecialFolderPathW(NULL, szBuffer, csidl,FALSE);
        return szBuffer;
    }
    std::string GetMac()
    {
        string mac;
        SAFE_CREATELP(char, Str, 32);
        ASTAT Adapter;
        NCB Ncb;
        UCHAR uRetCode;
        LANA_ENUM lenum;
        int i = 0;
        memset(&Ncb, 0, sizeof(Ncb));
        Ncb.ncb_command = NCBENUM;
        Ncb.ncb_buffer = (UCHAR *)&lenum;
        Ncb.ncb_length = sizeof(lenum);
        uRetCode = Netbios(&Ncb);
        for (i = 0; i < lenum.length; i++)
        {
            memset(&Ncb, 0, sizeof(Ncb));
            Ncb.ncb_command = NCBRESET;
            Ncb.ncb_lana_num = lenum.lana[i];
            uRetCode = Netbios(&Ncb);
            memset(&Ncb, 0, sizeof(Ncb));
            Ncb.ncb_command = NCBASTAT;
            Ncb.ncb_lana_num = lenum.lana[i];
            strcpy_s((char *)Ncb.ncb_callname, NCBNAMSZ, "* ");
            Ncb.ncb_buffer = (unsigned char *)&Adapter;
            Ncb.ncb_length = sizeof(Adapter);
            uRetCode = Netbios(&Ncb);
            if (uRetCode == 0)
            {
                sprintf_s(Str, 32, "%02X-%02X-%02X-%02X-%02X-%02X",
                    Adapter.adapt.adapter_address[0],
                    Adapter.adapt.adapter_address[1],
                    Adapter.adapt.adapter_address[2],
                    Adapter.adapt.adapter_address[3],
                    Adapter.adapt.adapter_address[4],
                    Adapter.adapt.adapter_address[5]
                );
				break;
            }
        }
        mac = Str;
        DELETE_LP_NULL(Str);
        return mac;
    }
	string Num2Str(int nNum)
	{
		char cBuff[120] = { 0 };
		_itoa_s(nNum, cBuff, 120, 10);
		return cBuff;
	}

	string doPostUrl(wstring host, wstring path, string sData)
	{
		DWORD dwSize = 0;
		DWORD dwDownloaded = 0;
		LPSTR pszOutBuffer = NULL;
		HINTERNET  hSession = NULL, hConnect = NULL, hRequest = NULL;

		BOOL  bResults = FALSE;

		//初始化，为应用程序，使用WinHTTP的功能，并返回一个WinHTTP的会话句柄。
		hSession = WinHttpOpen(L"User-Agent", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);

		if (hSession)
		{
			host = host.substr(host.find(L"//") + 2);
			host = host.substr(0, host.find(L"/"));
			//指定一个HTTP请求的初始目标服务器并返回一个HINTERNET连接句柄该初始目标的HTTP会话。
			hConnect = WinHttpConnect(hSession,
				host.c_str(), INTERNET_DEFAULT_HTTP_PORT, 0);
		}

		if (hConnect)
		{
			//创建一个HTTP请求句柄。
			hRequest = WinHttpOpenRequest(hConnect, L"POST",
				path.c_str(), NULL,
				WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
		}
		if (hRequest)
		{
			WinHttpAddRequestHeaders(hRequest,
				L"Content-Type: application/x-www-form-urlencoded",
				wcslen(L"Content-Type: application/x-www-form-urlencoded"),
				WINHTTP_ADDREQ_FLAG_ADD);

			std::string data = sData;//"name=6000000000&password=60000000&timestamp=1497333252384";

			const void *ss = (const char *)data.c_str();

			bResults = WinHttpSendRequest(hRequest, 0, 0, const_cast<void *>(ss),
				data.length(), data.length(), 0);
		}

		if (bResults)
		{
			bResults = WinHttpReceiveResponse(hRequest, NULL);

		}
		string sResult = "";
		if (bResults)
		{
			do
			{
				// Check for available data.

				dwSize = 0;

				if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
				{
					printf("Error %u in WinHttpQueryDataAvailable.\n", GetLastError());

					break;
				}

				if (!dwSize)
					break;

				pszOutBuffer = new char[dwSize + 1];

				if (!pszOutBuffer)
				{
					printf("Out of memory\n");
					break;
				}

				ZeroMemory(pszOutBuffer, dwSize + 1);

				if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded))
				{
					printf("Error %u in WinHttpReadData.\n", GetLastError());
				}
				else
				{
					printf("%s", pszOutBuffer);
				}
				sResult = pszOutBuffer;
				delete[] pszOutBuffer;

				if (!dwDownloaded)
					break;

			} while (dwSize > 0);
		}
		if (hRequest) WinHttpCloseHandle(hRequest);
		if (hConnect) WinHttpCloseHandle(hConnect);
		if (hSession) WinHttpCloseHandle(hSession);
		return sResult;
	}
    std::string GetMacString()
    {
        string mac;
        SAFE_CREATELP(char, Str, 32);
        ASTAT Adapter;
        NCB Ncb;
        UCHAR uRetCode;
        LANA_ENUM lenum;
        int i = 0;
        memset(&Ncb, 0, sizeof(Ncb));
        Ncb.ncb_command = NCBENUM;
        Ncb.ncb_buffer = (UCHAR *)&lenum;
        Ncb.ncb_length = sizeof(lenum);
        uRetCode = Netbios(&Ncb);
        for (i = 0; i < lenum.length; i++)
        {
            memset(&Ncb, 0, sizeof(Ncb));
            Ncb.ncb_command = NCBRESET;
            Ncb.ncb_lana_num = lenum.lana[i];
            uRetCode = Netbios(&Ncb);
            memset(&Ncb, 0, sizeof(Ncb));
            Ncb.ncb_command = NCBASTAT;
            Ncb.ncb_lana_num = lenum.lana[i];
            strcpy_s((char *)Ncb.ncb_callname, NCBNAMSZ, "* ");
            Ncb.ncb_buffer = (unsigned char *)&Adapter;
            Ncb.ncb_length = sizeof(Adapter);
            uRetCode = Netbios(&Ncb);
            if (uRetCode == 0)
            {
                sprintf_s(Str, 32, "%02X%02X%02X%02X%02X%02X",
                    Adapter.adapt.adapter_address[0],
                    Adapter.adapt.adapter_address[1],
                    Adapter.adapt.adapter_address[2],
                    Adapter.adapt.adapter_address[3],
                    Adapter.adapt.adapter_address[4],
                    Adapter.adapt.adapter_address[5]
                );
				break;
            }
        }
        mac = Str;
        DELETE_LP_NULL(Str);
        return mac;
    }
	BOOL IsDiskExist(CHAR cDiskName)
	{
		DWORD dwDrivers;
		int i = cDiskName - 'A';

		//dwDrivers的每一个二进制位表示对应的驱动器是否存在。  
		dwDrivers = GetLogicalDrives();
		//判断当前位是否有驱动器  
		if ((dwDrivers & (1 << (i))) != 0)
		{
			return TRUE;
		}
		return FALSE;
	}

	string doPost(wstring host, wstring path, string sData)
	{
		DWORD dwSize = 0;
		DWORD dwDownloaded = 0;
		LPSTR pszOutBuffer = NULL;
		HINTERNET  hSession = NULL, hConnect = NULL, hRequest = NULL;

		BOOL  bResults = FALSE;

		//初始化，为应用程序，使用WinHTTP的功能，并返回一个WinHTTP的会话句柄。
		hSession = WinHttpOpen(L"User-Agent", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);

		if (hSession)
		{
			host = host.substr(host.find(L"//") + 2);
			host = host.substr(0, host.find(L"/"));
			//指定一个HTTP请求的初始目标服务器并返回一个HINTERNET连接句柄该初始目标的HTTP会话。
			hConnect = WinHttpConnect(hSession,
				host.c_str(), INTERNET_DEFAULT_HTTP_PORT, 0);
		}

		if (hConnect)
		{
			//创建一个HTTP请求句柄。
			hRequest = WinHttpOpenRequest(hConnect, L"POST",
				path.c_str(), NULL,
				WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
		}
		if (hRequest)
		{
			WinHttpAddRequestHeaders(hRequest,
				L"Content-Type: application/json",
				wcslen(L"Content-Type: application/json"),
				WINHTTP_ADDREQ_FLAG_ADD);

			std::string data = sData;//"name=6000000000&password=60000000&timestamp=1497333252384";

			const void *ss = (const char *)data.c_str();

			bResults = WinHttpSendRequest(hRequest, 0, 0, const_cast<void *>(ss),
				data.length(), data.length(), 0);
		}

		if (bResults)
		{
			bResults = WinHttpReceiveResponse(hRequest, NULL);

		}
		string sResult = "";
		if (bResults)
		{
			do
			{
				// Check for available data.
				dwSize = 0;
				if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
				{
					printf("Error %u in WinHttpQueryDataAvailable.\n", GetLastError());
					break;
				}
				if (!dwSize)
					break;
				pszOutBuffer = new char[dwSize + 1];
				if (!pszOutBuffer)
				{
					printf("Out of memory\n");
					break;
				}
				ZeroMemory(pszOutBuffer, dwSize + 1);
				if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded))
				{
					printf("Error %u in WinHttpReadData.\n", GetLastError());
				}
				else
				{
					printf("%s", pszOutBuffer);
				}
				sResult += pszOutBuffer;
				delete[] pszOutBuffer;

				if (!dwDownloaded)
					break;

			} while (dwSize > 0);
		}
		if (hRequest) WinHttpCloseHandle(hRequest);
		if (hConnect) WinHttpCloseHandle(hConnect);
		if (hSession) WinHttpCloseHandle(hSession);
		return sResult;
	}

	unsigned char ToHex(unsigned char x)
	{
		return  x > 9 ? x + 55 : x + 48;
	}

	std::string UrlEncode(const std::string& str)
	{
		if (str.length() == 0)return "";
		std::string strTemp = "";
		size_t length = str.length();
		for (size_t i = 0; i < length; i++)
		{
			if (isalnum((unsigned char)str[i]) ||
				(str[i] == '-') ||
				(str[i] == '_') ||
				(str[i] == '.') ||
				(str[i] == '~'))
				strTemp += str[i];
			else if (str[i] == ' ')
				strTemp += "+";
			else
			{
				strTemp += '%';
				strTemp += ToHex((unsigned char)str[i] >> 4);
				strTemp += ToHex((unsigned char)str[i] % 16);
			}
		}
		return strTemp;
	}

	BOOL KillProcess(const WCHAR * lpwProcess)
	{
		LPWSTR File = NULL;
		HANDLE hProcessSnap;
		PROCESSENTRY32 pe32;
		//通过获取进程信息为指定的进程、进程使用的堆[HEAP]、模块[MODULE]、线程[THREAD]建立一个快照[snapshot]
		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		pe32.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hProcessSnap, &pe32))
		{
			do
			{
				File = pe32.szExeFile;
				if (_wcsicmp(File, lpwProcess) == 0)
				{
					HANDLE hProcessHandle = ::OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
					::TerminateProcess(hProcessHandle, 4);
					CloseHandle(hProcessSnap);
					return TRUE;
				}
			} while (Process32Next(hProcessSnap, &pe32));
		}
		CloseHandle(hProcessSnap);
		return FALSE;
	}
}


