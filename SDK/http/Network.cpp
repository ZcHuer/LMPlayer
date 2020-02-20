#include "Network.h"
#include "Constants.h"

namespace LeConnect
{
	
	INT NetworkStatus::networkStatus = Network::STATUS_NOTCONCERN;

	const HttpTimeouts HttpTimeouts::DEFAULT(5,5,5,5);

	const HttpTimeouts HttpTimeouts::DEFAULT_TERMINATE(1,1,2,1);
	
	BOOLEAN NetworkStatus::checkNetworkStatus()		//check whether network online
	{
		HRESULT hr = 0; 
		VARIANT_BOOL vb = 0;
		INetworkListManager* pNlm = NULL;

		//initialize STA
		hr = CoInitialize( NULL );
		if( FAILED(hr) ) {
			////g_log.e(_T("NetworkStatus::checkNetworkStatus CoInitialize Failed."));
			return NetworkStatus::isOnline();
		}

		//initialize COM
		hr = CoCreateInstance(
			CLSID_NetworkListManager,
			NULL,
			CLSCTX_ALL,
			IID_INetworkListManager,
			(void**) &pNlm );

		if( FAILED(hr) ) {
			//g_log.e(_T("NetworkStatus::checkNetworkStatus CoCreateInstance Failed."));
			CoUninitialize();
			return NetworkStatus::isOnline();
		}

		//???
		//hr = pNlm->get_IsConnected ( &vb );
		hr = pNlm->get_IsConnectedToInternet(&vb);
		if( SUCCEEDED(hr) ) {
			if( vb == VARIANT_TRUE) {
				NetworkStatus::setNetworkStatus(Network::STATUS_NOTCONCERN);
			}
			else {
				NetworkStatus::setNetworkStatus(Network::STATUS_OFFLINE);
			}
		}
		else {
			//g_log.e(_T("NetworkStatus::checkNetworkStatus get_IsConnected Failed."));
		}

		pNlm->Release();

		CoUninitialize();

		return NetworkStatus::isOnline();
	}
		
	std::wstring NetworkStatus::getProxyForUrlUsingPac(LPCTSTR DestinationUrl, const WCHAR* PacUri ){

		std::wstring destUrl;
#ifdef UNICODE
		destUrl.assign(DestinationUrl);
#else
		std::string tmpStr(DestinationUrl);
		destUrl.assign(tmpStr.begin(), tmpStr.end());
#endif

		//get http session handle
		HINTERNET WinHttpSession = WinHttpOpen(L"User",
			WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, 
			NULL, 
			NULL, 
			0);

		WINHTTP_AUTOPROXY_OPTIONS  ProxyOptions;
		WINHTTP_PROXY_INFO ProxyInfo;

		ProxyOptions.dwFlags           = WINHTTP_AUTOPROXY_CONFIG_URL;		//download PAC file according to the lpszAutoConfigUrl
		ProxyOptions.dwAutoDetectFlags = (WINHTTP_AUTO_DETECT_TYPE_DHCP |
			WINHTTP_AUTO_DETECT_TYPE_DNS_A);								//??? dwFlags not include WINHTTP_AUTOPROXY_AUTO_DETECT, dwAutoDetectFlags should be 0.
		ProxyOptions.lpszAutoConfigUrl = PacUri;							//PAC file URL, PAC (Proxy Auto-Configuration)

		// Get Proxy 
		BOOLEAN IsSuccess = ::WinHttpGetProxyForUrl( WinHttpSession, 
			destUrl.c_str(),	//http request URL
			&ProxyOptions,
			&ProxyInfo );

		::WinHttpCloseHandle(WinHttpSession);

		if ( IsSuccess && ProxyInfo.lpszProxy!= NULL)
				return std::wstring(ProxyInfo.lpszProxy);	//proxy server list
		return std::wstring();
	}

	/*
	const std::tstring	HttpRequestHandler::EVENT_FORMAT_PRE = std::tstring()
		.append(_T("%s")).append(reaper::ServerConfig::VALUE_SEP)
		.append(_T("%s")).append(reaper::ServerConfig::VALUE_SEP)
		.append(_T("%s")).append(reaper::ServerConfig::VALUE_SEP)
		.append(_T("%s")).append(reaper::ServerConfig::VALUE_SEP)
		.append(_T("%s")).append(reaper::ServerConfig::VALUE_SEP)

		.append(_T("%s")).append(reaper::ServerConfig::VALUE_SEP)
		.append(_T("%d")).append(reaper::ServerConfig::VALUE_SEP)
		.append(_T("%s")).append(reaper::ServerConfig::VALUE_SEP)
		.append(_T("%s")).append(reaper::ServerConfig::VALUE_SEP)
		.append(_T("%s")).append(reaper::ServerConfig::VALUE_SEP)

		.append(_T("%s")).append(reaper::ServerConfig::VALUE_SEP)
		.append(_T("%s")).append(reaper::ServerConfig::VALUE_SEP)
		.append(_T("%s")).append(reaper::ServerConfig::VALUE_SEP)
		.append(_T("%s")).append(reaper::ServerConfig::VALUE_SEP);	//format for HttpRequestHandler::initPrefix --> SNPRINTF_S

	//@wangyh26: 调用HttpRequestHandler的postEvents方法
	INT ReportManager::postEvents(std::list<Event> &listEvents, HttpTimeouts timeouts)
	{
		if(listEvents.size() == 0)
			return 0;
		////g_log.i(_T("ReportManager::postEvents start postEvents"));
		return this->handler.postEvents(listEvents, timeouts);
	}

	void ReportManager::terminate()
	{
		this->handler.clearRequest();
	}
	
	//@wangyh26: 设置“格式化的配置文件字符串”
	void HttpRequestHandler::resetPrefix()
	{
		this->_strPrefix = initPrefix();
	}

	//get configuration
	//@wangyh26: 生成“格式化的配置文件字符串”
	std::tstring HttpRequestHandler::initPrefix()	//called in HttpRequestHandler object initizlize.
	{
		const INT in_iBufSize = 1024;
		TCHAR out_sStr[in_iBufSize];
		INT len = SNPRINTF_S(out_sStr, in_iBufSize, _TRUNCATE, HttpRequestHandler::EVENT_FORMAT_PRE.c_str(),
			_T("KLog"),
			_cfg.getSdkVersion(),
			_cfg.getDeviceIdType(),
			_cfg.getDeviceId(),
			_cfg.getApplicationToken(),

			_cfg.getVersionName(), 
			_cfg.getVersionCode(),
			_cfg.getChannel(), 
			_cfg.getOsVersion(), 
			_cfg.getLanguage(),

			_cfg.getCountry(), 
			_cfg.getDeviceModel(), 
			_cfg.getManufacture(), 
			_cfg.getResolution()); //return num of wide characters in buffer, exclude terminate null wide character.

		return std::tstring(out_sStr, len);
	}

	/*
	false: network not online OR exceed max failed times
	true: otherwise
	*/
	/*
	BOOLEAN HttpRequestHandler::sendPre()
	{
		//network not online OR exceed max failed times
		if(!NetworkStatus::isOnline() || this->getFailedCount() >= HttpRequestHandler::FAILED_COUNT_MAX)
		{
			if(this->isFailedTimeout())				//whether exceed the interval(now - last failed timestamp)
			{
				if(!NetworkStatus::checkNetworkStatus())	//network not online
				{
					//g_log.e(_T("HttpRequestHandler::postEvents network not ready"));	//error
					this->updateFailedTs();			//update last failed timestamp
					this->increaseFailedCount();		//add failed times
					return false;
				}
				//g_log.w(_T("HttpRequestHandler::postEvents network reconnected. FailedCount=%d"), this->getFailedCount());			//warn
			}else
			{
				//g_log.w(_T("HttpRequestHandler::postEvents network not ready: check interval not reached"));
				this->increaseFailedCount();			//why do this??? add failed times
				return false;
			}
		}
		return true;
	}

	void HttpRequestHandler::sendPost(BOOLEAN isS)
	{
		if(!isS)
		{
			if(this->increaseFailedCount() >= HttpRequestHandler::FAILED_COUNT_MAX)
			{
				NetworkStatus::checkNetworkStatus();
				//g_log.w(_T("HttpRequestHandler::postEvents send failed 3 times continuously. NetworkStatus=%d"), NetworkStatus::getNetworkStatus());
			}
			this->updateFailedTs();		//update last failed timestamp
		}else
			this->clearFailedCount();	//clear failed times
	}

	//@wangyh26: 调用postFile()发送信息。建立HttpRequest对象，并调用其doPost()方法，POST数据，返回“成功标志位”
	INT HttpRequestHandler::postEvents(std::list<Event>& listEvents, HttpTimeouts timeouts)
	{
		//g_log.d(_T("HttpRequestHandler::postEvents enter listSize=%d"), listEvents.size()); //debug
		INT postLength = listEvents.size();
		if(postLength == 0 || ISEMPTYSTR(this->_cfg.getPostUrl()))
			return 0;

		std::tstringstream ss;
		std::tstring dataPrefix = this->getPrefix();				//get prefix (formatted configuration string)
		//@wangyh26: 拼接待发送的所有事件信息
		std::list<Event>::iterator it = listEvents.begin();		//prefix+event+"\n"+prefix+event+"\n"+...
		for(;it!=listEvents.end();it++)
		{
			//this->_cfg.setAppToken(it->_sAccount);
			//this->resetPrefix();
			//std::tstring dataPrefix = this->getPrefix();				//get prefix (formatted configuration string)
	
			ss << dataPrefix;
			//@wangyh26: use Event.toString() method to get Event Info, like category, action, parampairs etc.
			ss << it->toString();
			ss << _T("\n");
		}
		std::tstring dataStr = ss.str();
		//g_log.d(_T("HttpRequestHandler::postEvents events number: %d, events length: %d"), postLength, dataStr.size()); //debug
		//g_log.d(_T("HttpRequestHandler::postEvents data: %s"), dataStr.c_str()); //debug
		if(AnalyticsTracker::getServerConfigManager()->needCompress(postLength))
		{
			//g_log.w(_T("HttpRequestHandler::postEvents compress not supported")); //warn
		}
		ss.seekg(0);
		BOOLEAN isS = this->postFile(ss, dataStr.size(), this->_cfg.getPostUrl(), timeouts); //@wangyh26: 建立HttpRequest对象，并调用其doPost()方法，POST数据，返回“成功标志位”
		this->sendPost(isS);
		return isS?postLength:0;
	}

	//@wangyh26: 建立HttpRequest对象，并调用其doPost()方法，POST数据，返回“成功标志位”
	BOOLEAN HttpRequestHandler::postFile(std::tistream &is, const DWORD len, std::tstring serverContext, HttpTimeouts timeouts)
	{
		BYTE *data = new BYTE[len];
		TCHAR tch;
		DWORD count = 0;
		while(!is.eof() && count < len)
		{
			tch=is.get();
#ifdef UNICODE
			// cast wchar to char
			CHAR *ch = ((CHAR*)(&tch));
			data[count] = ch[0];
#else
			data[count] = tch;
#endif
			count++;
		}

		//@test
		//g_log.d(_T("HttpRequestHandler::postFile serverContext: %s, ProxyServerUrl: %s"), serverContext.c_str(), this->_cfg.getProxyServerUrl()); //debug

		HttpRequest req(serverContext.size()==0?NULL:serverContext.c_str(), this->_cfg.getProxyServerUrl(), NULL, timeouts);

		{
			Lock il(&this->_lock);
			_runningRequest = &req;
		}
		BOOLEAN isSucceed = req.doPost(data, len);
		{
			Lock il(&this->_lock);
			_runningRequest = NULL;
			//@test
			req.close();
		}
		delete data;
		return isSucceed;
	}

	void HttpRequestHandler::clearRequest()
	{
		this->_lock.unlock();

		//@wangyh26: deprecated
		this->_lock.wakeAll();

		//this->_lock.signalConditionAll();
		//Lock il(&this->_lock);
		if(_runningRequest!=NULL)
			_runningRequest->terminate();
	}

	*/
}