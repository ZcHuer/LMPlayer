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
	* �ַ�����װ
	*/
	class CQuickStringWrap
	{
		LPWSTR _szAlloc;		//@wangyh26: ���ڱ����ַ���

	public:
		CQuickStringWrap()		//@wangyh26: ��_szAlloc��ΪNULL
		{
			_szAlloc = NULL;
		}

		~CQuickStringWrap()		//@wangyh26: ɾ��_szAlloc
		{
			if (_szAlloc != NULL)
				delete [] _szAlloc;
		}

		operator LPCWSTR() const { return _szAlloc;}	//@wangyh26: ������ʽת��

		//@wangyh26: ������szIn������_szAlloc��������true
		BOOL Set(LPCWSTR szIn, DWORD dwLen)
		{
			//@wangyh26: ��ʼ����ʱ����
			LPWSTR szNew;
			szNew = new WCHAR[dwLen+1];
			if (szNew == NULL)	//@wangyh26: ���������ʱ����ʧ�ܣ��������ڴ����������벢����false
			{
				SetLastError( ERROR_OUTOFMEMORY);
				return FALSE;
			}
			//@wangyh26: ������szIn������_szAlloc��������true
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
	* HTTP����
	*/
	class HttpRequest
	{
		enum Method
		{
			POST, GET
		};
	private:
		//@wangyh26: ��������Ϣ
    	CQuickStringWrap _strTargetServer;		//@wangyh26: Ŀ�������������
    	CQuickStringWrap _strTargetPath;		//@wangyh26: Ŀ�������URLPATH
		WORD _nPort;							//@wangyh26: Ŀ��������˿ں�
		WORD _nScheme;							//@wangyh26: Э���ʶ
		CQuickStringWrap _strTargetUsername;	//@wangyh26: Ŀ��������û���
		CQuickStringWrap _strTargetPassword;	//@wangyh26: Ŀ�����������
		CQuickStringWrap _strProxyServer;		//@wangyh26: ���������������+�˿�
		CQuickStringWrap _strProxyUsername;		//@wangyh26: ����������û���
		CQuickStringWrap _strProxyPassword;		//@wangyh26: �������������
		HttpTimeouts _timeouts;					//@wangyh26: HTTP��ʱʱ������

		//@wangyh26: ��־λ
		BOOLEAN _isInit;						//@wangyh26: ��ʼ����־λ
		BOOLEAN _isConnect;						//@wangyh26: ���ӱ�־λ
		BOOLEAN _isSuccess;						//@wangyh26: �ɹ���־λ
		BOOLEAN _isWriteToDisk;					//@wangyh26: д�����̱�־λ

		HINTERNET _hSession;					//@wangyh26: SESSION���
		HINTERNET _hConnect;					//@wangyh26: ���Ӿ��
		HINTERNET _hRequest;					//@wangyh26: ������

		DWORD _dwLastStatusCode;				//@wangyh26: ���һ��״̬����
		DWORD _dwStatusCode;					//@wangyh26: ��ǰ���ص�״̬����
		DWORD _dwContentLength;					//@wangyh26: ���ݳ���
		DWORD _dwProxyAuthScheme;				//@wangyh26: �����������ȨЭ��

		std::string _strContentType;			//@wangyh26: 

		std::tstring _strFileName;				//@wangyh26: ��ʼ��ʱ���õı����ļ���
		std::stringstream _ssContentStream;	//@wangyh26: ������
		DWORD _dwContentStreamLength;			//@wangyh26: ����������

	private:
		// crack url and open request

		//@wangyh26: �����Ự_hSession����������_hConnect����������_hRequest
		BOOLEAN init(CSTRING szServerUrl, CSTRING szProxyUrl);								//@wangyh26: ��ʼ������ȡ_hSession�����á�Ŀ���������������_strTargetServer�����á�Ŀ�������URLPATH��_strTargetPath������_nPort��_nScheme����������������������˿�_strProxyServer
		BOOLEAN connet();																	//@wangyh26: �������ӣ���ȡ_hConnect
		BOOLEAN doRequest(HttpRequest::Method method, const BYTE *pData, DWORD dataSize);	//@wangyh26: ����connet()��������_hConnect���������ӱ�־λ������HTTP����_hRequest��

		//@wangyh26: doRequest()����get0/post0��������������_hRequest
		BOOLEAN get0();																		//@wangyh26: ����GET���󣬽�����Ӧ�����ء�״̬�롱_dwStatusCode�����ء����ݳ��ȡ�_dwContentLength����ȡ���������͡�lpszContentType��text, html�ȣ�����ȡ��Ӧ��д���ļ�_strFileName��д����_ssContentStream��
		BOOLEAN get0(const BYTE *pData, DWORD dataSize);	//@wangyh26: ����POST���󣬷������ݣ�������Ӧ������״̬�롢���ݳ��ȡ�д���ļ�_strFileName��д����_ssContentStream
		BOOLEAN post0(const BYTE *pData, DWORD dataSize);	//@wangyh26: ����POST���󣬷������ݣ�������Ӧ������״̬�롢���ݳ��ȡ�д���ļ�_strFileName��д����_ssContentStream
		//@wangyh26: get0/post0��鷵�ص���Ϣ
		BOOLEAN parseStatusCode();															//@wangyh26: ���ء�״̬�롱_dwStatusCode
		BOOLEAN parseContentLength();														//@wangyh26: ���ء����ݳ��ȡ�_dwContentLength
		BOOLEAN parseContentType();															//@wangyh26: ��ȡ���������͡�lpszContentType��text, html�ȣ�
		//@wangyh26: ��get0/post0�Ľ��д���ļ�����
		BOOLEAN receiveResponse();															//@wangyh26: ��ȡ��Ӧ��д���ļ�_strFileName��д����_ssContentStream
		//@wangyh26: doRequest()��齨������_hRequest�Ľ��
		BOOLEAN checkStatusCode();															//@wangyh26: ��鷵�ص�״̬����_dwStatusCode�����������һ��״̬����_dwLastStatusCode
		DWORD chooseAuthScheme(HINTERNET hRequest, DWORD dwSupportedSchemes);				//@wangyh26: ѡ����ȨЭ��



	public:
		//@wangyh26: ��ʼ��
		//@wangyh26: ��ȡ_hSession����szFileName��ΪNULLʱ_isWriteToDiskΪtrue������init()�ɹ�ʱ_isInitΪtrue������Ϊfalse����ȡ��������Ϣ��
		HttpRequest(CSTRING szServerUrl, CSTRING szProxyUrl, CSTRING szFileName, HttpTimeouts timeouts=HttpTimeouts::DEFAULT);	

		//@wangyh26: ����
		~HttpRequest();											//@wangyh26: ����close()���ر�_hRequest��_hConnect��_hSession

		BOOLEAN doGet(const BYTE *pData=NULL, DWORD dataSize=0);						//@wangyh26: ����doRequest()������GET���ӣ��������ӱ�־λ������HTTP����_hRequest�����ء��ɹ���־λ��_isSuccess
		BOOLEAN doPost(const BYTE *pData, DWORD dataSize);		//@wangyh26: ����doRequest()������POST���ӣ��������ӱ�־λ������HTTP����_hRequest�����ء��ɹ���־λ��_isSuccess

		//@wangyh26: ��־λ
		BOOLEAN isSuccess(){return this->_isSuccess;}			//@wangyh26: ���ء��ɹ���־λ��_isSuccess

		//@wangyh26: ��ȡ��Ӧ��Ϣ
		DWORD getStatusCode(){return this->_dwStatusCode;}													//@wangyh26: ��ȡ����ǰ���ص�״̬���롱_dwStatusCode
		DWORD getContentLength(){return this->_dwContentLength;}											//@wangyh26: ��ȡ�����ݳ��ȡ�_dwContentLength
		DWORD getStreamLength(){return this->_dwContentStreamLength;}										//@wangyh26: ��ȡ�����������ȡ�_dwContentStreamLength
		std::stringstream& getStream(){this->_ssContentStream.seekg(0); return this->_ssContentStream;}	//@wangyh26: ��ȡ����������_ssContentStream

		void close();		//@wangyh26: �ر�_hRequest��_hConnect��_hSession
		void terminate();	//@wangyh26: NOP
	};
}

