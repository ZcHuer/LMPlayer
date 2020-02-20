#ifndef NETWORK_H_INCLUDE
#define NETWORK_H_INCLUDE

#include "Constants.h"

namespace LeConnect
{
	/*
	* @wangyh26
	*
	* ����״̬
	*/
	class NetworkStatus
	{
	private:
		static INT networkStatus;																	//@wangyh26: ����״̬��־λ
	public:
		static void setNetworkStatus(INT s){NetworkStatus::networkStatus = s;}						//@wangyh26: ���á�����״̬��־λ��
		static BOOLEAN isOnline(){return NetworkStatus::networkStatus != Network::STATUS_OFFLINE;}	//@wangyh26: �ж��Ƿ���������������������
		static INT getNetworkStatus(){return NetworkStatus::networkStatus;}							//@wangyh26: ��ȡ������״̬��־λ��
		static BOOLEAN checkNetworkStatus();														//???
		static std::wstring getProxyForUrlUsingPac(LPCTSTR DestinationUrl, const WCHAR* PacUri );	//???
	};



	/*
	* @wangyh26
	*
	* HTTP��ʱʱ������
	*/
	class HttpTimeouts
	{
	public:
		static const HttpTimeouts DEFAULT;
		static const HttpTimeouts DEFAULT_TERMINATE;
		static const INT TIMEOUT_SECONDS=1000;			//@wangyh26: ʱ�䵥λ1000ms=1s
		static const INT TIMEOUT_MILLISECONDS=1;		//@wangyh26: ʱ�䵥λ1ms
	public:
		INT dwResolveTimeout;	//@wangyh26: DNS��ѯ�ȴ�ʱ��
		INT dwConnectTimeout;	//@wangyh26: ���ӵȴ�ʱ��
		INT dwSendTimeout;		//@wangyh26: ���͵ȴ�ʱ��
		INT dwReceiveTimeout;	//@wangyh26: ���յȴ�ʱ��
		INT timeOutUnit;		//@wangyh26: ʱ�䵥λ <Ĭ��Ϊ1000ms����1s>
	public:
		HttpTimeouts(){}
		//@wangyh26: ��ʼ����������DNS��ѯ�ȴ�ʱ�䣬���ӵȴ�ʱ�䣬���͵ȴ�ʱ�䣬���յȴ�ʱ�䣬ʱ�䵥λ<Ĭ��Ϊ1000ms����1s>��
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
	* HTTP������������װHttpRequest
	*/
	/*
	class HttpRequestHandler
	{
	private:
		static const std::tstring EVENT_FORMAT_PRE;						//@wangyh26: 
		Configuration &_cfg;											//@wangyh26: 1.��ȡreaper.cfg�е�������Ϣ 2.�ɼ�������Ϣ

		INT _failedCount;												//@wangyh26: ʧ�ܵĴ���
		LONGLONG _lastFailedMs;											//@wangyh26: ���һ��ʧ�ܵ�ʱ����������ʽ
	public:
		static const INT FAILED_COUNT_MAX = 3;							//@wangyh26: ʧ�ܴ�������
		static const LONGLONG NETWORK_RECHECK_INTERVAL_MS = 10 * 1000;	//@wangyh26: �����ؼ�ʱ��10s

	private:
		LockBase _lock;
		
		//@wangyh26: HTTP����
		reaper::HttpRequest* _runningRequest;					//@wangyh26: HTTP�������HttpRequest��

		//@wangyh26: ��ʽ���������ļ��ַ���
		std::tstring _strPrefix;								//@wangyh26: ��ʽ���������ļ��ַ���
		std::tstring getPrefix(){return this->_strPrefix;}		//@wangyh26: ��ȡ����ʽ���������ļ��ַ�����
		std::tstring initPrefix();								//@wangyh26: ���ɡ���ʽ���������ļ��ַ�����
		BOOLEAN postFile(std::tistream& is, const DWORD len, std::tstring serverContext, HttpTimeouts timeouts= HttpTimeouts::DEFAULT);	//@wangyh26: ����HttpRequest���󣬲�������doPost()������POST���ݣ����ء��ɹ���־λ��
	public:
		void resetPrefix();										//@wangyh26: ���á���ʽ���������ļ��ַ�����

		//@wangyh26: ��ʼ��{Configuration��_cfg=c��ʧ�ܵĴ���=0�����һ��ʧ�ܵ�ʱ���=0�������á���ʽ���������ļ��ַ����� }
		HttpRequestHandler(Configuration& c):_cfg(c),_failedCount(0),_lastFailedMs(0),_runningRequest(NULL) //initialize
		{
			this->_strPrefix = initPrefix();
		}

		INT postEvents(std::list<Event>& listEvents, HttpTimeouts timeouts= HttpTimeouts::DEFAULT);	//@wangyh26: ����postFile()������Ϣ������HttpRequest���󣬲�������doPost()������POST���ݣ����ء��ɹ���־λ��
		void clearRequest();			//@wangyh26: ����HttpRequest::terminate() NOP
		BOOLEAN sendPre();				//@wangyh26: 
		void sendPost(BOOLEAN isS);		//@wangyh26:

		//@wangyh26: ʧ����Ϣ
		INT increaseFailedCount(){return ++_failedCount;}		//@wangyh26: ���ӡ�ʧ�ܵĴ�����
		void clearFailedCount(){_failedCount = 0;}				//@wangyh26: ��ա�ʧ�ܵĴ�����
		INT getFailedCount(){return _failedCount;}				//@wangyh26: ��ȡ��ʧ�ܵĴ�����
		void updateFailedTs(){_lastFailedMs = System::now();}	//@wangyh26: ���¡����һ��ʧ�ܵ�ʱ����������ʽ��
		BOOLEAN isFailedTimeout()								//@wangyh26: �ж��Ƿ񳬹��������ؼ�ʱ��10s��
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
	* ��װHTTP����������HttpRequestHandler��
	*/
	/*
	class ReportManager
	{
	private:
		HttpRequestHandler &handler;																	//@wangyh26: HttpRequestHandler
	public:
		INT postEvents(std::list<Event>& listEvents, HttpTimeouts timeouts= HttpTimeouts::DEFAULT);		//@wangyh26: ����HttpRequestHandler��postEvents����
		ReportManager(HttpRequestHandler& h):handler(h){}												//@wangyh26: ���ݲ�����ʼ��HttpRequestHandler
		HttpRequestHandler& getHandler(){return this->handler;}											//@wangyh26: ��ȡHttpRequestHandler
		void terminate();																				//@wangyh26: 
	};
	*/
}

#endif //NETWORK_H_INCLUDE