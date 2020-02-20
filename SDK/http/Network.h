#ifndef NETWORK_H_INCLUDE
#define NETWORK_H_INCLUDE

#include "Constants.h"

namespace LeConnect
{
	/*
	* @wangyh26
	*
	* 网络状态
	*/
	class NetworkStatus
	{
	private:
		static INT networkStatus;																	//@wangyh26: 网络状态标志位
	public:
		static void setNetworkStatus(INT s){NetworkStatus::networkStatus = s;}						//@wangyh26: 设置“网络状态标志位”
		static BOOLEAN isOnline(){return NetworkStatus::networkStatus != Network::STATUS_OFFLINE;}	//@wangyh26: 判断是否有网（包括：局域网）
		static INT getNetworkStatus(){return NetworkStatus::networkStatus;}							//@wangyh26: 获取“网络状态标志位”
		static BOOLEAN checkNetworkStatus();														//???
		static std::wstring getProxyForUrlUsingPac(LPCTSTR DestinationUrl, const WCHAR* PacUri );	//???
	};



	/*
	* @wangyh26
	*
	* HTTP超时时间设置
	*/
	class HttpTimeouts
	{
	public:
		static const HttpTimeouts DEFAULT;
		static const HttpTimeouts DEFAULT_TERMINATE;
		static const INT TIMEOUT_SECONDS=1000;			//@wangyh26: 时间单位1000ms=1s
		static const INT TIMEOUT_MILLISECONDS=1;		//@wangyh26: 时间单位1ms
	public:
		INT dwResolveTimeout;	//@wangyh26: DNS查询等待时间
		INT dwConnectTimeout;	//@wangyh26: 连接等待时间
		INT dwSendTimeout;		//@wangyh26: 发送等待时间
		INT dwReceiveTimeout;	//@wangyh26: 接收等待时间
		INT timeOutUnit;		//@wangyh26: 时间单位 <默认为1000ms，即1s>
	public:
		HttpTimeouts(){}
		//@wangyh26: 初始化，参数（DNS查询等待时间，连接等待时间，发送等待时间，接收等待时间，时间单位<默认为1000ms，即1s>）
		HttpTimeouts(INT r,INT c, INT s, INT rc, INT unit=TIMEOUT_SECONDS):
			dwResolveTimeout(r),
			dwConnectTimeout(c),
			dwSendTimeout(s),
			dwReceiveTimeout(rc),
			timeOutUnit(unit)
		{}
	};



	class HttpRequest;
	/*
	* @wangyh26
	*
	* HTTP请求处理句柄，封装HttpRequest
	*/
	/*
	class HttpRequestHandler
	{
	private:
		static const std::tstring EVENT_FORMAT_PRE;						//@wangyh26: 
		Configuration &_cfg;											//@wangyh26: 1.提取reaper.cfg中的配置信息 2.采集基础信息

		INT _failedCount;												//@wangyh26: 失败的次数
		LONGLONG _lastFailedMs;											//@wangyh26: 最后一次失败的时间戳，毫秒格式
	public:
		static const INT FAILED_COUNT_MAX = 3;							//@wangyh26: 失败次数上限
		static const LONGLONG NETWORK_RECHECK_INTERVAL_MS = 10 * 1000;	//@wangyh26: 网络重检时间10s

	private:
		LockBase _lock;
		
		//@wangyh26: HTTP请求
		reaper::HttpRequest* _runningRequest;					//@wangyh26: HTTP请求对象（HttpRequest）

		//@wangyh26: 格式化的配置文件字符串
		std::tstring _strPrefix;								//@wangyh26: 格式化的配置文件字符串
		std::tstring getPrefix(){return this->_strPrefix;}		//@wangyh26: 获取“格式化的配置文件字符串”
		std::tstring initPrefix();								//@wangyh26: 生成“格式化的配置文件字符串”
		BOOLEAN postFile(std::tistream& is, const DWORD len, std::tstring serverContext, HttpTimeouts timeouts= HttpTimeouts::DEFAULT);	//@wangyh26: 建立HttpRequest对象，并调用其doPost()方法，POST数据，返回“成功标志位”
	public:
		void resetPrefix();										//@wangyh26: 重置“格式化的配置文件字符串”

		//@wangyh26: 初始化{Configuration类_cfg=c，失败的次数=0，最后一次失败的时间戳=0，，设置“格式化的配置文件字符串” }
		HttpRequestHandler(Configuration& c):_cfg(c),_failedCount(0),_lastFailedMs(0),_runningRequest(NULL) //initialize
		{
			this->_strPrefix = initPrefix();
		}

		INT postEvents(std::list<Event>& listEvents, HttpTimeouts timeouts= HttpTimeouts::DEFAULT);	//@wangyh26: 调用postFile()发送信息。建立HttpRequest对象，并调用其doPost()方法，POST数据，返回“成功标志位”
		void clearRequest();			//@wangyh26: 调用HttpRequest::terminate() NOP
		BOOLEAN sendPre();				//@wangyh26: 
		void sendPost(BOOLEAN isS);		//@wangyh26:

		//@wangyh26: 失败信息
		INT increaseFailedCount(){return ++_failedCount;}		//@wangyh26: 增加“失败的次数”
		void clearFailedCount(){_failedCount = 0;}				//@wangyh26: 清空“失败的次数”
		INT getFailedCount(){return _failedCount;}				//@wangyh26: 获取“失败的次数”
		void updateFailedTs(){_lastFailedMs = System::now();}	//@wangyh26: 更新“最后一次失败的时间戳，毫秒格式”
		BOOLEAN isFailedTimeout()								//@wangyh26: 判断是否超过“网络重检时间10s”
		{
			LONGLONG iMs = System::now() - _lastFailedMs;
			if(iMs >= 0 && iMs < NETWORK_RECHECK_INTERVAL_MS)
				return false;
			return true;
		}

	};
	*/


	/*
	* @wangyh26
	*
	* 封装HTTP请求处理句柄（HttpRequestHandler）
	*/
	/*
	class ReportManager
	{
	private:
		HttpRequestHandler &handler;																	//@wangyh26: HttpRequestHandler
	public:
		INT postEvents(std::list<Event>& listEvents, HttpTimeouts timeouts= HttpTimeouts::DEFAULT);		//@wangyh26: 调用HttpRequestHandler的postEvents方法
		ReportManager(HttpRequestHandler& h):handler(h){}												//@wangyh26: 根据参数初始化HttpRequestHandler
		HttpRequestHandler& getHandler(){return this->handler;}											//@wangyh26: 获取HttpRequestHandler
		void terminate();																				//@wangyh26: 
	};
	*/
}

#endif //NETWORK_H_INCLUDE