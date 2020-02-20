#pragma once
//#include "../Global.h"
#include "Common.h"
#include "Network.h"
#include <winhttp.h>
#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib,"winhttp.lib")

namespace LeConnect
{	

	/*
	* @wangyh26
	*
	* 字符串封装
	*/
	class CQuickStringWrap
	{
		LPWSTR _szAlloc;		//@wangyh26: 用于保存字符串

	public:
		CQuickStringWrap()		//@wangyh26: 将_szAlloc置为NULL
		{
			_szAlloc = NULL;
		}

		~CQuickStringWrap()		//@wangyh26: 删除_szAlloc
		{
			if (_szAlloc != NULL)
				delete [] _szAlloc;
		}

		operator LPCWSTR() const { return _szAlloc;}	//@wangyh26: 重载隐式转换

		//@wangyh26: 将输入szIn保存至_szAlloc，并返回true
		BOOL Set(LPCWSTR szIn, DWORD dwLen)
		{
			//@wangyh26: 初始化临时变量
			LPWSTR szNew;
			szNew = new WCHAR[dwLen+1];
			if (szNew == NULL)	//@wangyh26: 如果创建临时变量失败，则设置内存溢出错误代码并返回false
			{
				SetLastError( ERROR_OUTOFMEMORY);
				return FALSE;
			}
			//@wangyh26: 将输入szIn保存至_szAlloc，并返回true
#if ENABLE_SAFTR_CHECK
			memcpy_s(szNew, dwLen*sizeof(WCHAR),szIn, dwLen*sizeof(WCHAR));
#else
			memcpy(szNew, szIn, dwLen*sizeof(WCHAR));
#endif
			szNew[dwLen] = L'\0';
			if (_szAlloc != NULL)
				delete [] _szAlloc;
			_szAlloc = szNew;
			return TRUE;
		}
	};



	/*
	* @wangyh26
	*
	* HTTP请求
	*/
	class HttpRequest
	{
		enum Method
		{
			POST, GET
		};
	private:
		//@wangyh26: 服务器信息
    	CQuickStringWrap _strTargetServer;		//@wangyh26: 目标服务器主机名
    	CQuickStringWrap _strTargetPath;		//@wangyh26: 目标服务器URLPATH
		WORD _nPort;							//@wangyh26: 目标服务器端口号
		WORD _nScheme;							//@wangyh26: 协议标识
		CQuickStringWrap _strTargetUsername;	//@wangyh26: 目标服务器用户名
		CQuickStringWrap _strTargetPassword;	//@wangyh26: 目标服务器密码
		CQuickStringWrap _strProxyServer;		//@wangyh26: 代理服务器主机名+端口
		CQuickStringWrap _strProxyUsername;		//@wangyh26: 代理服务器用户名
		CQuickStringWrap _strProxyPassword;		//@wangyh26: 代理服务器密码
		HttpTimeouts _timeouts;					//@wangyh26: HTTP超时时间设置

		//@wangyh26: 标志位
		BOOLEAN _isInit;						//@wangyh26: 初始化标志位
		BOOLEAN _isConnect;						//@wangyh26: 连接标志位
		BOOLEAN _isSuccess;						//@wangyh26: 成功标志位
		BOOLEAN _isWriteToDisk;					//@wangyh26: 写到磁盘标志位

		HINTERNET _hSession;					//@wangyh26: SESSION句柄
		HINTERNET _hConnect;					//@wangyh26: 连接句柄
		HINTERNET _hRequest;					//@wangyh26: 请求句柄

		DWORD _dwLastStatusCode;				//@wangyh26: 最后一次状态代码
		DWORD _dwStatusCode;					//@wangyh26: 当前返回的状态代码
		DWORD _dwContentLength;					//@wangyh26: 内容长度
		DWORD _dwProxyAuthScheme;				//@wangyh26: 代理服务器授权协议

		std::string _strContentType;			//@wangyh26: 

		std::tstring _strFileName;				//@wangyh26: 初始化时设置的本地文件名
		std::stringstream _ssContentStream;	//@wangyh26: 内容流
		DWORD _dwContentStreamLength;			//@wangyh26: 内容流长度

	private:
		// crack url and open request

		//@wangyh26: 建立会话_hSession、建立连接_hConnect、建立请求_hRequest
		BOOLEAN init(CSTRING szServerUrl, CSTRING szProxyUrl);								//@wangyh26: 初始化，获取_hSession，设置“目标服务器主机名”_strTargetServer，设置“目标服务器URLPATH“_strTargetPath，设置_nPort、_nScheme，代理服务器的主机名及端口_strProxyServer
		BOOLEAN connet();																	//@wangyh26: 建立连接，获取_hConnect
		BOOLEAN doRequest(HttpRequest::Method method, const BYTE *pData, DWORD dataSize);	//@wangyh26: 调用connet()建立连接_hConnect，设置连接标志位；建立HTTP请求_hRequest；

		//@wangyh26: doRequest()利用get0/post0发起真正的请求_hRequest
		BOOLEAN get0();																		//@wangyh26: 发起GET请求，接受响应；返回“状态码”_dwStatusCode；返回“内容长度”_dwContentLength；获取“内容类型”lpszContentType（text, html等）；获取响应，写入文件_strFileName，写入流_ssContentStream；
		BOOLEAN get0(const BYTE *pData, DWORD dataSize);	//@wangyh26: 发起POST请求，发送数据；接受响应，返回状态码、内容长度、写入文件_strFileName，写入流_ssContentStream
		BOOLEAN post0(const BYTE *pData, DWORD dataSize);	//@wangyh26: 发起POST请求，发送数据；接受响应，返回状态码、内容长度、写入文件_strFileName，写入流_ssContentStream
		//@wangyh26: get0/post0检查返回的信息
		BOOLEAN parseStatusCode();															//@wangyh26: 返回“状态码”_dwStatusCode
		BOOLEAN parseContentLength();														//@wangyh26: 返回“内容长度”_dwContentLength
		BOOLEAN parseContentType();															//@wangyh26: 获取“内容类型”lpszContentType（text, html等）
		//@wangyh26: 将get0/post0的结果写入文件和流
		BOOLEAN receiveResponse();															//@wangyh26: 获取响应，写入文件_strFileName，写入流_ssContentStream
		//@wangyh26: doRequest()检查建立请求_hRequest的结果
		BOOLEAN checkStatusCode();															//@wangyh26: 检查返回的状态代码_dwStatusCode，并设置最后一次状态代码_dwLastStatusCode
		DWORD chooseAuthScheme(HINTERNET hRequest, DWORD dwSupportedSchemes);				//@wangyh26: 选择授权协议



	public:
		//@wangyh26: 初始化
		//@wangyh26: 获取_hSession，当szFileName不为NULL时_isWriteToDisk为true，调用init()成功时_isInit为true；其余为false；提取服务器信息；
		HttpRequest(CSTRING szServerUrl, CSTRING szProxyUrl, CSTRING szFileName, HttpTimeouts timeouts=HttpTimeouts::DEFAULT);	

		//@wangyh26: 析构
		~HttpRequest();											//@wangyh26: 调用close()，关闭_hRequest、_hConnect、_hSession

		BOOLEAN doGet(const BYTE *pData=NULL, DWORD dataSize=0);						//@wangyh26: 调用doRequest()，建立GET连接，设置连接标志位；建立HTTP请求_hRequest；返回“成功标志位”_isSuccess
		BOOLEAN doPost(const BYTE *pData, DWORD dataSize);		//@wangyh26: 调用doRequest()，建立POST连接，设置连接标志位；建立HTTP请求_hRequest；返回“成功标志位”_isSuccess

		//@wangyh26: 标志位
		BOOLEAN isSuccess(){return this->_isSuccess;}			//@wangyh26: 返回“成功标志位”_isSuccess

		//@wangyh26: 获取响应信息
		DWORD getStatusCode(){return this->_dwStatusCode;}													//@wangyh26: 获取“当前返回的状态代码”_dwStatusCode
		DWORD getContentLength(){return this->_dwContentLength;}											//@wangyh26: 获取“内容长度”_dwContentLength
		DWORD getStreamLength(){return this->_dwContentStreamLength;}										//@wangyh26: 获取“内容流长度”_dwContentStreamLength
		std::stringstream& getStream(){this->_ssContentStream.seekg(0); return this->_ssContentStream;}	//@wangyh26: 获取“内容流”_ssContentStream

		void close();		//@wangyh26: 关闭_hRequest、_hConnect、_hSession
		void terminate();	//@wangyh26: NOP
	};
}

