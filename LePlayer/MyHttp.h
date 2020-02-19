#pragma once

#include <Windows.h>
#include <string>
#include <winhttp.h>
#pragma comment(lib,"winhttp.lib")
using namespace std;

namespace MyHTTP {
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
}
