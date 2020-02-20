#include "http.h"

namespace LeConnect
{	
	//@wangyh26: 获取_hSession，当szFileName不为NULL时_isWriteToDisk为true，成功时_isInit为true；其余为false；提取服务器信息
	HttpRequest::HttpRequest(CSTRING szServerUrl, CSTRING szProxyUrl, CSTRING szFileName, HttpTimeouts timeouts)
	{
		this->_isWriteToDisk = false;
		if(szFileName != NULL)
		{
			this->_strFileName.assign(szFileName);
			this->_isWriteToDisk = true;
		}
		this->_isConnect = false;
		this->_isSuccess = false;
		this->_timeouts = timeouts;
		this->_isInit = init(szServerUrl, szProxyUrl);
		//if(!this->_isInit)
		//	//g_log.e(_T("HttpRequest::HttpRequest init failed of error %d."),GetLastError());
	}

	//@wangyh26: 调用close()，关闭_hRequest、_hConnect、_hSession
	HttpRequest::~HttpRequest()
	{	
		this->close();
	}

	//@wangyh26: 关闭_hRequest、_hConnect、_hSession
	void HttpRequest::close()
	{
		// Close any open handles.
		if (_hRequest != NULL)
			WinHttpCloseHandle(_hRequest);

		if (_hConnect != NULL)
			WinHttpCloseHandle(_hConnect);

		if (_hSession != NULL)
			WinHttpCloseHandle(_hSession);
		_hRequest = NULL;
		_hConnect = NULL;
		_hSession = NULL;
	}

	//@wangyh26: NOP
	void HttpRequest::terminate()
	{
		/**
		if (_hRequest != NULL)
			WinHttpCloseHandle(_hRequest);
		_hRequest = NULL;
		**/
	}

	//@wangyh26: 调用doRequest()，建立GET连接，设置连接标志位；建立HTTP请求_hRequest；返回“成功标志位”_isSuccess
	BOOLEAN HttpRequest::doGet(const BYTE *pData, DWORD dataSize)
	{
		this->_isSuccess = this->doRequest(HttpRequest::GET, pData, dataSize);
		//if(!this->_isSuccess)
		//	//g_log.e(_T("HttpRequest::doGet failed of error %d."),GetLastError());
		this->close();
		return this->_isSuccess;
	}

	//@wangyh26: 调用doRequest()，建立POST连接，设置连接标志位；建立HTTP请求_hRequest；返回“成功标志位”_isSuccess
	BOOLEAN HttpRequest::doPost(const BYTE *pData, DWORD dataSize)
	{
		this->_isSuccess = this->doRequest(HttpRequest::POST, pData, dataSize);
		//if(!this->_isSuccess)
		//	//g_log.e(_T("HttpRequest::doPost failed of error %d."),GetLastError());
		this->close();
		return this->_isSuccess;
	}

	//@wangyh26: 初始化，获取_hSession，设置“目标服务器主机名”_strTargetServer，设置“目标服务器URLPATH“_strTargetPath，设置_nPort、_nScheme，代理服务器的主机名及端口_strProxyServer
	BOOLEAN HttpRequest::init(CSTRING szServerUrl, CSTRING szProxyUrl)
	{	
		//@wangyh26: 将szServerUrl赋予strServerUrl
		if(szServerUrl == NULL)
			return false;
		std::wstring strServerUrl;
#ifdef UNICODE
		strServerUrl.assign(szServerUrl);
#else
		std::string tmp(szServerUrl);
		strServerUrl.assign(tmp.begin(), tmp.end());
#endif

		//@wangyh26: 初始化“服务器URL组件结构体”
		URL_COMPONENTS urlServerComponents;
		_hRequest = NULL;
		_hConnect = NULL;
		_hSession = NULL;
		//From the server URL, we need a host, path, scheme and port.
		memset (&urlServerComponents, 0, sizeof(urlServerComponents));
		urlServerComponents.dwStructSize = sizeof(urlServerComponents);
		urlServerComponents.dwHostNameLength = 1;
		urlServerComponents.dwUrlPathLength = 1;
		urlServerComponents.dwSchemeLength = 1;
		urlServerComponents.dwUserNameLength = 1;
		urlServerComponents.dwPasswordLength = 1;
		//@wangyh26: 获取“服务器URL组件结构体”
		if (!WinHttpCrackUrl(strServerUrl.c_str(), 0, 0, &urlServerComponents))
		{
			////g_log.e(_T("HttpRequest::init There was a problem with the Server URL %s"), strServerUrl.c_str());
			return false;
		}
		if (urlServerComponents.lpszUserName == NULL)
			urlServerComponents.dwUserNameLength = 0;
		if (urlServerComponents.lpszPassword == NULL)
			urlServerComponents.dwPasswordLength = 0;

		if (!_strTargetServer.Set(urlServerComponents.lpszHostName, urlServerComponents.dwHostNameLength)	//@wangyh26: 设置“目标服务器主机名及长度”
			|| !_strTargetPath.Set(urlServerComponents.lpszUrlPath, urlServerComponents.dwUrlPathLength))	//@wangyh26: 设置“目标服务器URLPATH路径及长度” 
		{
			return false;
		}

		//  for the username and password, if they are empty, leave the string pointers as NULL.
		//  This allows for the current process's default credentials to be used.
		//@wangyh26: 如果有用户名或密码则返回false
		if (urlServerComponents.dwUserNameLength != 0
			&& !_strTargetUsername.Set(urlServerComponents.lpszUserName, 
			urlServerComponents.dwUserNameLength))
		{
			return false;
		}
		if (urlServerComponents.dwPasswordLength != 0
			&& !_strTargetPassword.Set(urlServerComponents.lpszPassword, 
			urlServerComponents.dwPasswordLength))
		{
			return false;
		}

		//@wangyh26: 设置端口和模式（HTTP或HTTPS）
		_nPort = urlServerComponents.nPort;
		_nScheme = urlServerComponents.nScheme;
	
		//@wangyh26: 将szProxyUrl赋值给strProxyUrl
		URL_COMPONENTS urlProxyComponents;		//@wangyh26: 代理服务器URL组成部分
		if(!ISEMPTYSTR(szProxyUrl))
		{
		std::wstring strProxyUrl;
#ifdef UNICODE
		strProxyUrl.assign(szProxyUrl);
#else
		std::string tmp2(szProxyUrl);
		strProxyUrl.assign(tmp2.begin(), tmp2.end());
#endif
			//crack proxy url
			//From the proxy URL, we need a host, username and password.
			//@wangyh26: 代理服务器URL结构体初始化
			memset (&urlProxyComponents, 0, sizeof(urlProxyComponents));
			urlProxyComponents.dwStructSize = sizeof(urlProxyComponents);
			urlProxyComponents.dwHostNameLength = 1;
			urlProxyComponents.dwUserNameLength = 1;
			urlProxyComponents.dwPasswordLength = 1;
			//@wangyh26: 获取“代理服务器URL组成部分”
			if (!WinHttpCrackUrl(strProxyUrl.c_str(), 0, 0, &urlProxyComponents))
			{
				////g_log.e(_T("HttpRequest::init There was a problem with the proxy URL %s"), strProxyUrl.c_str());
				return false;
			}
			if (urlProxyComponents.lpszUserName == NULL)
				urlProxyComponents.dwUserNameLength = 0;
			if (urlProxyComponents.lpszPassword == NULL)
				urlProxyComponents.dwPasswordLength = 0;

			//  We do something tricky here, taking from the host beginning 
			//to the beginning of the path as the strProxyServer.  What this 
			//does, is if you have urls like "http://proxy","http://proxy/",
			//"http://proxy:8080" is copy them as "proxy","proxy","proxy:8080" 
			//respectively.  This makes the port available for WinHttpOpen.
			//@wangyh26: 获取“代理服务器URL组成部分”的lpszUrlPath
			if (urlProxyComponents.lpszUrlPath == NULL)
			{
				urlProxyComponents.lpszUrlPath = wcschr(urlProxyComponents.lpszHostName, L'/');
				if(urlProxyComponents.lpszUrlPath == NULL)
				{
#if SAFE_CHECK_NEED_MODIFY
#if ENABLE_SAFTR_CHECK
					urlProxyComponents.lpszUrlPath = urlProxyComponents.lpszHostName
						+ wcsnlen(urlProxyComponents.lpszHostName, MAX_PATH);
#else
					urlProxyComponents.lpszUrlPath = urlProxyComponents.lpszHostName
						+ wcslen(urlProxyComponents.lpszHostName);
#endif
#endif
				}
			}

			//@wangyh26: 将端口号和主机名一起放入主机名属性lpszHostName中
			if (!_strProxyServer.Set(urlProxyComponents.lpszHostName, 
				(DWORD)(urlProxyComponents.lpszUrlPath - urlProxyComponents.lpszHostName)))
			{
				return false;
			}

			//  for the username and password, if they are empty, leave the string pointers as NULL.
			//  This allows for the current process's default credentials to be used.
			//@wangyh26: 代理服务器用户名和密码
			if (urlProxyComponents.dwUserNameLength != 0
				&& !_strProxyUsername.Set(urlProxyComponents.lpszUserName, 
				urlProxyComponents.dwUserNameLength))
			{
				return false;
			}
			if (urlProxyComponents.dwPasswordLength != 0
				&& !_strProxyPassword.Set(urlProxyComponents.lpszPassword, 
				urlProxyComponents.dwPasswordLength))
			{
				return false;
			}
		}// end proxy url crack

		//@wangyh26: 打开SESSION
		if(!ISEMPTYSTR(szProxyUrl))
		{
			// Open with proxy
			_hSession = WinHttpOpen( LeConnect::Http::HTTP_HEADER_USER_AGENT,  
				WINHTTP_ACCESS_TYPE_NAMED_PROXY,
				_strProxyServer,	
				L"<local>", 0 );
		}
		else
		{
			_hSession = WinHttpOpen( LeConnect::Http::HTTP_HEADER_USER_AGENT, WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
		}

		if(!_hSession)
		{
			//g_log.e(_T("HttpRequest::init WinHttpOpen failed"));
			return false;
		}else
		{
			//@wangyh26: 设置SESSION的超时时间
			if (!::WinHttpSetTimeouts(
				_hSession,
				this->_timeouts.dwResolveTimeout * this->_timeouts.timeOutUnit,
				this->_timeouts.dwConnectTimeout * this->_timeouts.timeOutUnit,
				this->_timeouts.dwSendTimeout * this->_timeouts.timeOutUnit,
				this->_timeouts.dwReceiveTimeout * this->_timeouts.timeOutUnit
			));
				//g_log.e(_T("HttpRequest::init WinHttpSetTimeouts failed"));

			//DWORD value, size;
			DWORD size  = sizeof(DWORD);
			DWORD value = this->_timeouts.dwReceiveTimeout * this->_timeouts.timeOutUnit;

			WinHttpSetOption(_hSession, WINHTTP_OPTION_RECEIVE_RESPONSE_TIMEOUT, &value, size);

			
			//@test
			//value = 0xdeadbeef;
			//WinHttpQueryOption(_hSession, WINHTTP_OPTION_CONNECT_TIMEOUT, &value, &size);
			////g_log.e(_T("CONNECT_TIMEOUT %u"),value);
			//value = 0xdeadbeef;
			//WinHttpQueryOption(_hSession, WINHTTP_OPTION_RESOLVE_TIMEOUT, &value, &size);
			////g_log.e(_T("RESOLVE_TIMEOUT %u"),value);
			//value = 0xdeadbeef;
			//WinHttpQueryOption(_hSession, WINHTTP_OPTION_SEND_TIMEOUT, &value, &size);
			////g_log.e(_T("REQUEST_TIMEOUT %u"),value);
			//value = 0xdeadbeef;
			//WinHttpQueryOption(_hSession, WINHTTP_OPTION_RECEIVE_TIMEOUT, &value, &size);
			////g_log.e(_T("RECEIVE_TIMEOUT %u"),value);
			//value = 0xdeadbeef;
			//WinHttpQueryOption(_hSession, WINHTTP_OPTION_RECEIVE_RESPONSE_TIMEOUT, &value, &size);
			////g_log.e(_T("RECEIVE_RESPONSE_TIMEOUT %u"),value);
			
		}

		return true;
	}

	//@wangyh26: 建立连接，获取_hConnect
	BOOLEAN HttpRequest::connet()
	{
		if(!this->_isInit)
			return false;

		// Specify an HTTP server.
		if( _hSession )
			_hConnect = WinHttpConnect( 
			_hSession, 
			_strTargetServer,
			_nPort, 
			0 );
		else
		{
			////g_log.e(_T("HttpRequest::connet WinHttpOpen error"));
			return false;
		}

		return true;		
	}

	// send get request. save the response to file pszFileName 
	//@wangyh26: 调用connet()建立连接_hConnect，设置连接标志位；建立HTTP请求_hRequest；
	BOOLEAN HttpRequest::doRequest(HttpRequest::Method method, const BYTE *pData, DWORD dwDataSize)
	{
		if(!this->_isInit)return false;
		this->_isConnect = this->connet();	//@wangyh26: 建立连接，设置连接标志位
		if(!this->_isConnect)
		{
			//g_log.e(_T("HttpRequest::doRequest connect failed of error %d."),GetLastError());
			return false;
		}

		//@wangyh26: 用methodStr记录method传递的GET/POST方法
		std::wstring methodStr;
		if(method == HttpRequest::GET)
			methodStr.append(L"GET");
		else if (method == HttpRequest::POST)
			methodStr.append(L"POST");
		else
		{
			//g_log.e(_T("HttpRequest::doRequest not supported method type %d."), method);
			return false;
		}

		//@wangyh26: 建立HTTP请求，_hRequest包含有GET/POST标识
		PCWSTR pwszAcceptTypes[] = {L"*/*", NULL};
		// Create an HTTP request handle.
		if( _hConnect )
		{
			_hRequest = WinHttpOpenRequest( 
				_hConnect, 
				methodStr.c_str(), 
				_strTargetPath,
				NULL, 
				WINHTTP_NO_REFERER, 
				pwszAcceptTypes,
				_nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0 );
			if(_hRequest == NULL)
			{
				//g_log.e(_T("HttpRequest::doRequest WinHttpOpenRequest error"));
				return false;
			}
		}
		else
		{
			//g_log.e(_T("HttpRequest::doRequest WinHttpConnect error"));
			return false;
		}

		BOOLEAN bDone = false;
		_dwProxyAuthScheme = 0;
		int loopNum = 3;
		while(!bDone && (loopNum--) > 0)
		//while(!bDone)
		{
			if (_strProxyUsername != NULL && _strProxyPassword != NULL)
			{
				//  If a proxy auth challenge was responded to, reset those credentials
				//before each SendRequest.  This is done because after responding to a 401
				//or perhaps a redirect the proxy may require re-authentication.  You
				//could get into a 407,401,407,401,etc loop otherwise.
				if (_dwProxyAuthScheme != 0)
				{
					if( !WinHttpSetCredentials( 
						_hRequest, 
						WINHTTP_AUTH_TARGET_PROXY, 
						_dwProxyAuthScheme, 
						_strProxyUsername,
						_strProxyPassword, NULL))
					{
						return false;
					}
				}
			}//end proxy string check

			BOOLEAN bResult = false;
			switch(method)
			{
			case HttpRequest::GET: 
					{
						if(dwDataSize>0)
							bResult = get0(pData,dwDataSize);
						else
							bResult = get0();
					}
					break;
			case HttpRequest::POST: bResult = post0(pData, dwDataSize); break;
			default:
				//g_log.e(_T("HttpRequest::doRequest not supported request type %d"), method);
				return false;
			}
			if(!bResult)
				return false;

			// check status code
			bDone = checkStatusCode();
		}// end while(!bDone);
		return bDone;
	}
	
	//@wangyh26: 发起GET请求，接受响应；返回“状态码”_dwStatusCode；返回“内容长度”_dwContentLength；获取“内容类型”lpszContentType（text, html等）；获取响应，写入文件_strFileName，写入流_ssContentStream；
	BOOLEAN HttpRequest::get0()
	{
		// Send a request.
		//@wangyh26: 发起GET请求
		if( !WinHttpSendRequest( 
			_hRequest,							//HINTERNET hRequest
			WINHTTP_NO_ADDITIONAL_HEADERS,		//LPCWSTR pwszHeaders
			0,									//DWORD dwHeadersLength
			WINHTTP_NO_REQUEST_DATA,			//LPVOID lpOptional
			0,									//DWORD dwOptionalLength
			0, 									//DWORD dwTotalLength
			0 ) )								//DWORD_PTR dwContext
		{
			////g_log.e(_T("HttpRequest::get0 WinHttpSendRequest error"));
			//g_log.e(_T("HttpRequest::get0 WinHttpSendRequest error %d."), GetLastError());	//@author: wangyh26
			return false;
		}

		//@wangyh26: 接受WinHttpSendRequest()的响应 
		if(!WinHttpReceiveResponse( _hRequest, NULL ))
		{
			////g_log.e(_T("HttpRequest::get0 WinHttpSendRequest error"));
			//g_log.e(_T("HttpRequest::get0 WinHttpSendRequest error %d."), GetLastError());	//@author: wangyh26
			return false;
		}

		// Query header
		//@wangyh26: 返回“状态码”_dwStatusCode
		if(!this->parseStatusCode())
		{
			return false;
		}
		
		this->parseContentLength();			//@wangyh26: 返回“内容长度”_dwContentLength
		this->parseContentType();			//@wangyh26: 获取“内容类型”lpszContentType（text, html等）
		return this->receiveResponse();		//@wangyh26: 获取响应，写入文件_strFileName，写入流_ssContentStream
	}

	BOOLEAN HttpRequest::get0(const BYTE *pData, DWORD dataSize)
	{
		// Send a request.
		//@wangyh26: 发起GET请求
		if (!WinHttpSendRequest(
			_hRequest,							//HINTERNET hRequest
			//WINHTTP_NO_ADDITIONAL_HEADERS,		0,
			L"Content-Type:application/json", 0,
			WINHTTP_NO_REQUEST_DATA,			//LPVOID lpOptional
			0,									//DWORD dwOptionalLength
			dataSize,						//DWORD dwTotalLength
			0))								//DWORD_PTR dwContext
		{
			////g_log.e(_T("HttpRequest::get0 WinHttpSendRequest error"));
			//g_log.e(_T("HttpRequest::get0 WinHttpSendRequest error %d."), GetLastError());	//@author: wangyh26
			return false;
		}

		// Write data
		//@wangyh26: 发送数据
		DWORD dwByteWrite = 0;
		DWORD err = ::GetLastError();
		if (!::WinHttpWriteData(_hRequest, pData, dataSize, &dwByteWrite))
		{
			err = ::GetLastError();
			//g_log.e(_T("HttpRequest::post0 WinHttpWriteData error. err=%lu, dwByteWrite=%lu"), err, dwByteWrite);
			if (err != ERROR_INVALID_PARAMETER) //IGNORE this error to get response code
				return false;
		}
		//@wangyh26: 接受WinHttpSendRequest()的响应 
		if (!WinHttpReceiveResponse(_hRequest, NULL))
		{
			////g_log.e(_T("HttpRequest::get0 WinHttpSendRequest error"));
			//g_log.e(_T("HttpRequest::get0 WinHttpSendRequest error %d."), GetLastError());	//@author: wangyh26
			return false;
		}

		// Query header
		//@wangyh26: 返回“状态码”_dwStatusCode
		if (!this->parseStatusCode())
		{
			return false;
		}

		this->parseContentLength();			//@wangyh26: 返回“内容长度”_dwContentLength
		this->parseContentType();			//@wangyh26: 获取“内容类型”lpszContentType（text, html等）
		return this->receiveResponse();		//@wangyh26: 获取响应，写入文件_strFileName，写入流_ssContentStream
	}

	//@wangyh26: 发起POST请求，发送数据；接受响应，返回状态码、内容长度、写入文件_strFileName，写入流_ssContentStream
	BOOLEAN HttpRequest::post0(const BYTE *pData, DWORD dataSize)
	{	
		//@wangyh26: 处理HTTPS及HTTP

		DWORD dwFlags;
		DWORD dwBuffLen = sizeof(dwFlags);
		if (_nScheme == INTERNET_SCHEME_HTTPS)
		{
			dwFlags = 0;
			dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
			dwFlags |= SECURITY_FLAG_IGNORE_CERT_DATE_INVALID;
			dwFlags |= SECURITY_FLAG_IGNORE_CERT_CN_INVALID;
			dwFlags |= SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE;
			WinHttpSetOption(_hRequest, WINHTTP_OPTION_SECURITY_FLAGS,
				&dwFlags, sizeof(dwFlags));
		}
		
		// Send a request.
		//@wangyh26: 发起POST请求
		
		INT count = 0;
		while (!WinHttpSendRequest(_hRequest,
			L"Content-Type:application/json",0,
			//WINHTTP_NO_ADDITIONAL_HEADERS, 0, 
			WINHTTP_NO_REQUEST_DATA, 0, dataSize, 0)
			&& 3 > count)
		{
			++count;
			//g_log.e(_T("HttpRequest::post0 WinHttpSendRequest error=%lu"), ::GetLastError());
		}
		if (3 == count)
			return false;
		
		// Write data
		//@wangyh26: 发送数据
		DWORD dwByteWrite = 0;
		DWORD err =  ::GetLastError() ;
		if(!::WinHttpWriteData(_hRequest, pData, dataSize, &dwByteWrite))
		{
			err =  ::GetLastError() ;
			//g_log.e(_T("HttpRequest::post0 WinHttpWriteData error. err=%lu, dwByteWrite=%lu"), err, dwByteWrite);
			if( err != ERROR_INVALID_PARAMETER) //IGNORE this error to get response code
				return false;
		}

		//@wangyh26: 接受响应，返回状态码、内容长度、写入文件_strFileName，写入流_ssContentStream
		if(!::WinHttpReceiveResponse( _hRequest, NULL ))
		{
			//g_log.e(_T("HttpRequest::post0 WinHttpReceiveResponse error"));
			return false;
		}
		if(!this->parseStatusCode())
			return false;
		this->parseContentLength();
		return this->receiveResponse();
	}

	//@wangyh26: 返回“状态码”_dwStatusCode
	BOOLEAN HttpRequest::parseStatusCode()
	{
		if(!this->_isConnect)
			return false;
		//status code/content-length/content-type
		DWORD dwINTLength = sizeof(_dwStatusCode);
		//@wangyh26: 返回“状态码”
		if (!WinHttpQueryHeaders(
			_hRequest,
			WINHTTP_QUERY_FLAG_NUMBER | WINHTTP_QUERY_STATUS_CODE,
			NULL,
			&_dwStatusCode,
			&dwINTLength,
			NULL)) 
		{
			//g_log.e(_T("HttpRequest::parseStatusCode WINHTTP_QUERY_STATUS_CODE failed"));
			return false;
		}
		//g_log.d(_T("HttpRequest::parseStatusCode %d"), _dwStatusCode);
		return true;
	}

	//@wangyh26: 返回“内容长度”_dwContentLength
	BOOLEAN HttpRequest::parseContentLength()
	{
		if(!this->_isConnect)
			return false;
		DWORD dwINTLength = sizeof(_dwStatusCode);
		if (!WinHttpQueryHeaders(
			_hRequest,
			WINHTTP_QUERY_FLAG_NUMBER | WINHTTP_QUERY_CONTENT_LENGTH,
			NULL,
			&_dwContentLength,
			&dwINTLength,
			NULL)) 
		{
			//g_log.e(_T("HttpRequest::parseContentLength WINHTTP_QUERY_CONTENT_LENGTH failed"));
			return false;
		}
		//g_log.d(_T("HttpRequest::parseContentLength %d"), _dwContentLength);
		return true;
	}

	//@wangyh26: 获取“内容类型”lpszContentType（text, html等）
	BOOLEAN HttpRequest::parseContentType()
	{
		if(!this->_isConnect)
			return false;
		DWORD dwContentTypeLength = 0;
		// first query length
		WinHttpQueryHeaders(
			_hRequest,
			WINHTTP_QUERY_CONTENT_TYPE,
			NULL,
			NULL,
			&dwContentTypeLength,
			NULL);
		// allocate and query content
		LPWSTR lpszContentType = new WCHAR[dwContentTypeLength/sizeof(WCHAR)];
		if (!WinHttpQueryHeaders(
			_hRequest,
			WINHTTP_QUERY_CONTENT_TYPE,
			NULL,
			lpszContentType,
			&dwContentTypeLength,
			NULL)) 
		{
			//g_log.e(_T("HttpRequest::parseContentType WINHTTP_QUERY_CONTENT_TYPE failed"));
			return false;
		}
		//g_log.d(_T("HttpRequest::parseContentType %S"), lpszContentType);

		//free lpszContentType
		delete [] lpszContentType;
		return true;
	}
	
	//@wangyh26: 获取响应，写入文件_strFileName，写入流_ssContentStream
	BOOLEAN HttpRequest::receiveResponse()
	{
		// open local file for write
		std::tofstream ofDownload;
		if(this->_isWriteToDisk)
			ofDownload.open(this->_strFileName.c_str(), std::ios::out | std::ios::binary);
		this->_ssContentStream.str(std::string());
		this->_ssContentStream.clear();
		this->_dwContentStreamLength = 0;

		// Keep checking for data until there is nothing left.
		DWORD dwSize = 0, dwDownloaded = 0;
		LPSTR pszOutBuffer;
		do	
		{
			// Check for available data.
			dwSize = 0;
			if( !WinHttpQueryDataAvailable( _hRequest, &dwSize ) )
			{
				//g_log.e(_T("HttpRequest::receiveResponse Error %d in WinHttpQueryDataAvailable."), GetLastError());
				return false;
			}
				
			// Allocate space for the buffer.
			pszOutBuffer = new char[dwSize+1];
			if( !pszOutBuffer )
			{
				//g_log.e(_T("HttpRequest::receiveResponse Out of memory"));
				dwSize=0;
			}
			else
			{
				// Read the data.
				ZeroMemory( pszOutBuffer, dwSize+1 );

				if( !WinHttpReadData( _hRequest, (LPVOID)pszOutBuffer,	
					dwSize, &dwDownloaded ) )
				{
					//g_log.e(_T("HttpRequest::receiveResponse Error %d in WinHttpReadData."),GetLastError());
					delete [] pszOutBuffer;
					return false;
				}
				else
				{
					//std::printf( "%s", pszOutBuffer );
					_ssContentStream << pszOutBuffer;
					if(ofDownload.is_open())
						ofDownload << pszOutBuffer;
					this->_dwContentStreamLength += dwSize;
				}	

				// Free the memory allocated to the buffer.
				delete [] pszOutBuffer;
			}
		} while( dwSize > 0 );

		if(ofDownload.is_open())
			ofDownload.close();
		
		this->_ssContentStream.seekg(0);
		//g_log.d(_T("HttpRequest::receiveResponse succeed. StatusCode:%lu, ContentLength:%lu, StreamLength=%lu"),
		//	this->_dwStatusCode, this->_dwContentLength, this->_dwContentStreamLength);

		return true;
	}

	//@wangyh26: 检查返回的状态代码_dwStatusCode，并设置最后一次状态代码_dwLastStatusCode
	BOOLEAN HttpRequest::checkStatusCode()
	{
		DWORD dwSupportedSchemes, dwFirstScheme, dwTarget, dwSelectedScheme;
		switch (_dwStatusCode)
		{
		case 200: 
		case 204:
		case 206:
			// The resource was successfully retrieved.
			// You could use WinHttpReadData to read the contents of the server's response.
			//g_log.d(_T("HttpRequest::checkStatusCode request was successfully completed."));
			break;
		case 401:
			// The server requires authentication.
			//g_log.d(_T("HttpRequest::checkStatusCode The server requires authentication. Sending credentials..."));

			// Obtain the supported and preferred schemes.
			if( !WinHttpQueryAuthSchemes(_hRequest, &dwSupportedSchemes, &dwFirstScheme, &dwTarget))
				break;

			// Set the credentials before resending the request.
			dwSelectedScheme = this->chooseAuthScheme(_hRequest, dwSupportedSchemes);

			if (dwSelectedScheme == 0)
			{
				break;
			}
			else
			{
				if (!WinHttpSetCredentials( _hRequest, dwTarget, dwSelectedScheme, 
					_strTargetUsername, _strTargetPassword, NULL))
				{
					break;
				}
			}

			// If the same credentials are requested twice, abort the
			// request.  For simplicity, this sample does not check for
			// a repeated sequence of status codes.
			if (_dwLastStatusCode==401)
			{
				//g_log.d(_T("HttpRequest::checkStatusCode Server Authentication failed."));
				break;
			}
			_dwLastStatusCode = _dwStatusCode;
			return false;
		case 407:
			// The proxy requires authentication.
			//g_log.d(_T("HttpRequest::checkStatusCode The proxy requires authentication.  Sending credentials..."));

			// Obtain the supported and preferred schemes.
			if (!WinHttpQueryAuthSchemes(_hRequest, &dwSupportedSchemes, &dwFirstScheme, &dwTarget))
				break;

			// Set the credentials before resending the request.
			_dwProxyAuthScheme = this->chooseAuthScheme(_hRequest, dwSupportedSchemes);

			// If the same credentials are requested twice, abort the
			// request.  For simplicity, this sample does not check for
			// a repeated sequence of status codes.
			if (_dwLastStatusCode==407)
			{
				printf("\nProxy Authentication failed.");
				break;
			}
			_dwLastStatusCode = _dwStatusCode;
			return false;
		case 405:
			//g_log.d(_T("HttpRequest::checkStatusCode 405 post method not support"));
		default:
			// The status code does not indicate success.
			//g_log.w(_T("HttpRequest::checkStatusCode %lu returned."), _dwStatusCode);
			_dwLastStatusCode = _dwStatusCode;
			return false;
		}// end switch
		_dwLastStatusCode = _dwStatusCode;
		return true;
	}

	//@wangyh26: 选择授权协议
	DWORD HttpRequest::chooseAuthScheme(HINTERNET hRequest, DWORD dwSupportedSchemes)
	{
		//  It is the servers responsibility to only accept authentication schemes
		//which provide the level of security needed to protect the server's
		//resource.
		//  However the client has some obligation when picking an authentication
		//scheme to ensure it provides the level of security needed to protect
		//the client's username and password from being revealed.  The Basic authentication
		//scheme is risky because it sends the username and password across the
		//wire in a format anyone can read.  This is not an issue for SSL connections though.

		if (dwSupportedSchemes & WINHTTP_AUTH_SCHEME_NEGOTIATE)
			return WINHTTP_AUTH_SCHEME_NEGOTIATE;
		else if (dwSupportedSchemes & WINHTTP_AUTH_SCHEME_NTLM)
			return WINHTTP_AUTH_SCHEME_NTLM;
		else if (dwSupportedSchemes & WINHTTP_AUTH_SCHEME_PASSPORT)
			return WINHTTP_AUTH_SCHEME_PASSPORT;
		else if (dwSupportedSchemes & WINHTTP_AUTH_SCHEME_DIGEST)
			return WINHTTP_AUTH_SCHEME_DIGEST;
		else if (dwSupportedSchemes & WINHTTP_AUTH_SCHEME_BASIC)
		{
			DWORD dwValue;
			DWORD dwSize = sizeof(dwValue);
			if (WinHttpQueryOption(hRequest, WINHTTP_OPTION_SECURITY_FLAGS, &dwValue,&dwSize)
				&& (dwValue & SECURITY_FLAG_SECURE))
			{
				return WINHTTP_AUTH_SCHEME_BASIC;
			}
			else
				return 0;
		}
		else
			return 0;
	}
}

