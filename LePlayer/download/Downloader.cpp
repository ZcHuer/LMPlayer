#include "Downloader.h"
#include <wininet.h>
#pragma comment(lib,"Wininet.lib")

CDownloader::CDownloader()
	: m_bIsUrlValid(FALSE)
{
	SetRetry(10, 500);
	m_redirectDeep = 10;
}


CDownloader::~CDownloader()
{
}

BOOL CDownloader::DownloadTo(string sUrl,string* sResult)
{
	BOOL bDownResult = TRUE;
    URL_COMPONENTSA _ucCrackUrl;
    ZeroMemory(&_ucCrackUrl, sizeof(URL_COMPONENTS));
    _ucCrackUrl.dwStructSize = sizeof(URL_COMPONENTS);
    _ucCrackUrl.lpszHostName = m_szHostName;
    _ucCrackUrl.dwHostNameLength = sizeof(m_szHostName);
    _ucCrackUrl.lpszUrlPath = m_szUrlPath;
    _ucCrackUrl.dwUrlPathLength = sizeof(m_szUrlPath);
    m_bIsUrlValid = InternetCrackUrlA(sUrl.c_str(),sUrl.length(), 0, &_ucCrackUrl);

	//判断 url 地址是否有效
	if (m_bIsUrlValid == FALSE){	
		return FALSE;
	}

	//启用HTTP协议   
ReOpen:
    HINTERNET hOpen = InternetOpenA("Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/35.0.1916.114 Safari/537.36", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, NULL);
	if (hOpen == NULL){
		if (m_retryCount--){
			Sleep(m_retryInterval);
			//重新尝试打开连接
			goto ReOpen;
		}
		return FALSE;
	}

	//建立HTTP连接   
ReConnect:
    HINTERNET hConnect = InternetConnectA(hOpen,_ucCrackUrl.lpszHostName,_ucCrackUrl.nPort, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
	if (hConnect == NULL){
		if (m_retryCount--){
			Sleep(m_retryInterval);
			//重新建立连接
			goto ReConnect;
		}
        InternetCloseHandle(hOpen);
		return FALSE;
	}
	
	//创建一个URL请求 
ReOpenRequest:
    HINTERNET hRequest = HttpOpenRequestA(hConnect,"GET",_ucCrackUrl.lpszUrlPath,
        NULL, NULL, NULL,((_ucCrackUrl.nPort == 443) ? INTERNET_FLAG_SECURE : 0) 
        | INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | 
        INTERNET_FLAG_NO_AUTO_REDIRECT, 0);

	if (hRequest == NULL){
		if (m_retryCount--){
			Sleep(m_retryInterval);
			//重新创建一个URL请求 
			goto ReOpenRequest;
		}
        InternetCloseHandle(hOpen);
        InternetCloseHandle(hConnect);
		return FALSE;
	}
	
	//发送请求
ReSendRequest:
	string sRangHeader = "";
	if (!HttpSendRequestA(hRequest, sRangHeader.c_str(), sRangHeader.length(), NULL, 0)){
		if (GetLastError() == ERROR_INTERNET_TIMEOUT){
			if (m_retryCount--){
				Sleep(m_retryInterval);
				//重新发送请求
				goto ReSendRequest;
			}
		}else{
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hRequest);
			if (m_retryCount--){
				Sleep(m_retryInterval);
				//重新创建连接
				goto ReConnect;
			}
		}
        InternetCloseHandle(hOpen);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hRequest);
		return FALSE;
	}

	//查询状态码
	DWORD dwRetCode;
	DWORD dwSizeOfRq = sizeof(dwRetCode);
	if (!HttpQueryInfoA(hRequest, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwRetCode, &dwSizeOfRq, NULL)){
		if (m_retryCount--){
			Sleep(m_retryInterval);
			//重新发送请求
			goto ReSendRequest;
		}
	}

	//处理302
	if (dwRetCode == 302 || dwRetCode == 301){
		//查询跳转地址
		if (m_redirectDeep--){
			CHAR sLocation[2049];
			DWORD dwSizeOfLocation = 2048;
			if (HttpQueryInfoA(hRequest, HTTP_QUERY_LOCATION, &sLocation, &dwSizeOfLocation, NULL)){
                InternetCloseHandle(hOpen);
                InternetCloseHandle(hConnect);
                InternetCloseHandle(hRequest);
                return DownloadTo(sLocation,sResult);
			}else{
                InternetCloseHandle(hOpen);
                InternetCloseHandle(hConnect);
                InternetCloseHandle(hRequest);
				return FALSE;
			}
		}
        InternetCloseHandle(hOpen);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hRequest);
		return FALSE;
	}
	
	if (dwRetCode<200 || dwRetCode>=300){
		if (dwRetCode == 404){
            InternetCloseHandle(hOpen);
            InternetCloseHandle(hConnect);
            InternetCloseHandle(hRequest);
			return FALSE;
		}
		else{
			if (m_retryCount--){
				Sleep(m_retryInterval);
				//重新发送请求
				goto ReSendRequest;
			}
		}
	}

	//判断文件是否支持断点续传
    CHAR pRecvBuff[20] = { 0 };
	DWORD dwRecvCount = 0;
	DWORD dwRate = 0;
	INTERNET_BUFFERSA ibRecv;
	ZeroMemory(&ibRecv, sizeof(ibRecv));
	ibRecv.dwStructSize = sizeof(ibRecv);
	ibRecv.lpvBuffer = pRecvBuff;
	ibRecv.dwBufferLength = sizeof(pRecvBuff)-1;

	do{
		//开始读取文件   
		if (!InternetReadFileExA(hRequest,&ibRecv,0, 0)){
			if (m_retryCount--){
				Sleep(m_retryInterval);
				//重新发送请求
				goto ReSendRequest;
			}
            InternetCloseHandle(hOpen);
            InternetCloseHandle(hConnect);
            InternetCloseHandle(hRequest);
			return FALSE;
		}
		if (ibRecv.dwBufferLength&&sResult!=NULL){
            (*sResult).append((CHAR*)ibRecv.lpvBuffer, ibRecv.dwBufferLength);
		}
	} while (ibRecv.dwBufferLength);

    InternetCloseHandle(hOpen);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hRequest);
	return bDownResult;
}

void CDownloader::SetRetry(int count, int interval)
{
	m_retryCount = count;
	m_retryInterval = interval;
}
