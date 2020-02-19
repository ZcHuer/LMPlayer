#pragma once
#include <Windows.h>

#include <string>
using namespace std;

class CDownloader
{
public:
	CDownloader();
	~CDownloader();
	BOOL DownloadTo(string sUrl,string* sResult=NULL);
	void SetRetry(int count = 10,int interval = 5000);
protected:
	CHAR m_szHostName[256];
	CHAR m_szUrlPath[2048];
	BOOL m_bIsUrlValid;
	int m_retryCount;
	int m_retryInterval;
	int m_redirectDeep;
};

