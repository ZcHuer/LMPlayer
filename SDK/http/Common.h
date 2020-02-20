#pragma once

#include <cstdlib> 

#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <deque>
#include <map>
#include <list>
#include <set>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <sys/types.h>
#include <share.h>
#include <conio.h>
#include <errno.h>

#include <Io.h>
#include <Tchar.h>
#include <Windows.h>
#include <Process.h>
#include <Winhttp.h>
#include <Iphlpapi.h>
#include <Direct.h>
#include <Fcntl.h>
#include <Share.h>
#include <SYS/Stat.h>
#include <wtypes.h>

//DCOM
#include <comdef.h>
#include <Wbemidl.h>

//Network
#include <Netlistmgr.h>

//#include "Constants.h"

#define _CRT_NON_CONFORMING_SWPRINTFS

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

namespace LeConnect
{	

#ifndef  UNICODE
	typedef LPSTR STRING;
	typedef LPCSTR CSTRING;
#	define tprintf printf
#	define tsprintf	sprintf
#	define tfprintf	fprintf
#	define tprintf	printf
#	define tvprintf	vprintf
#	define tvfprintf vfprintf
#	define tstring	string
#	define tistream	istream
#	define tostream	ostream
#	define tiostream	iostream
#	define tstringstream	stringstream
#	define tstreambuf	streambuf
#	define tifstream ifstream
#	define tofstream ofstream
#	define tfstream	fstream
#	define STRLEN(x)	strlen(x)
#	define STRCMP(x, y)		strcmp(x, y)
#	define SNPRINTF_S		_snprintf_s
#	define STRCPY_S(x, y, z)		strcpy_s(x, y, z)
#   define STRNCPY_S(d, n, s, c)	strncpy_s(d, n, s, c)
#	define STRTOK_S(x, y, z)	strtok_s(x, y, z)
#	define ATOI(x)	atoi(x)
#	define STRLWR_S(x, y) _strlwr_s(x, y)
#	define STRUPR_S(x, y) _strupr_s(x, y)
#	define STRDUP(x)	_strdup(x)
#	define ISALNUM(x)	isalnum(x)
#	define ISSPACE(x)	isspace(x)
#	define REGEX	wregex
#	define mkdir	_mkdir
#	define rmdir	_rmdir
#	define getcwd	_getcwd
#	define getdcw	_getdcwd
#	define tfopen_s	fopen_s
#	define sopen_s	_sopen_s
#	define fsopen _fsopen
#else
	typedef LPWSTR STRING;
	typedef LPCWSTR CSTRING;
#	define tprintf wprintf
#	define tsprintf	swprintf
#	define tfprintf	fwprintf
#	define tprintf	wprintf
#	define tvprintf	vwprintf
#	define tvfprintf vfwprintf
#	define tstring	wstring
#	define tistream	wistream
#	define tostream	wostream
#	define tiostream	wiostream
#	define tstringstream	wstringstream
#	define tstreambuf	wstreambuf
#	define tifstream wifstream
#	define tofstream wofstream
#	define tfstream	wfstream
#	define STRLEN(x)	wcslen(x) // not include the terminating null-byte
#	define STRCMP(x, y)		wcscmp(x, y)
#	define SNPRINTF_S		_snwprintf_s
#	define STRCPY_S(x, y, z)		wcscpy_s(x, y, z)
#   define STRNCPY_S(d, n, s, c)	wcsncpy_s(d, n, s, c)
#	define STRTOK_S(x, y, z)	wcstok_s(x, y, z)
#	define ATOI(x)	_wtoi(x)
#	define STRLWR_S(x, y) _wcslwr_s(x, y)
#	define STRUPR_S(x, y) _wcsupr_s(x, y)
#	define STRDUP(x)	_wcsdup(x)
#	define ISALNUM(x)	iswalnum(x)
#	define ISSPACE(x)	iswspace(x)
#	define REGEX	regex
#	define mkdir	_wmkdir
#	define rmdir	_wrmdir
#	define getcwd	_wgetcwd
#	define getdcw	_wgetdcwd
#	define tfopen_s	wfopen_s
#	define sopen_s	_wsopen_s
#	define fsopen _wfsopen

#endif

#define STRCMP_B(x, y) (STRCMP(x, y)==0)
#define ARRAY_CPY(dst, src, len) {for(INT i=0;i<len;i++){dst[i] = src[i];}}
#define ARRAY_LEN(src) (sizeof(src) / sizeof(src[0]))
#define ATOI_ERROR_RETURN if(errno != 0)return
#define STRING_CPY(dst, src) \
	delete [] dst;\
	dst = NULL; \
	if(src != NULL){\
		INT iCplen = STRLEN(src) + 1;\
		dst = new TCHAR[iCplen];\
		STRCPY_S(dst, iCplen, src);\
	} // end #define STRING_CPY

#define STRING_NULL_RETURN(str) if(str==NULL)return _T("")

#define ISLETTER(str)  (str <= _T('Z') && str >= _T('A')) || (str <= _T('z') && str >= _T('a'))
#define ISEMPTYSTR(str) (str==NULL||STRLEN(str)==0)

	typedef std::pair<std::tstring, std::tstring> T_paramPair;
	typedef std::list<T_paramPair>  T_paramList;

	typedef std::pair<CSTRING, CSTRING> T_paramPairNew;
	
	/** ISSURE: Error	C2338	The C++ Standard forbids containers of const elements because allocator<const T> is ill-formed.
	typedef std::list<const T_paramPairNew>  T_paramListNew; */
	typedef const std::list<T_paramPairNew>  T_paramListNew;

	typedef std::map<INT, T_paramPair>  T_paramMap;

	static const T_paramPair EMPTY_PARAM_PAIR(_T(""), _T(""));

	class Locale
	{
	private:
		std::tstring _strLanguage;
		std::tstring _strCountry;
	public:
		Locale(std::string lan, std::string cty)
		{
#ifdef UNICODE
			_strLanguage.assign(lan.begin(), lan.end());
			_strCountry.assign(cty.begin(), cty.end());
#else
			this->_strLanguage = lan;
			this->_strCountry = cty;

#endif
		}
		Locale(std::wstring lan, std::wstring cty)
		{
#ifdef UNICODE
			this->_strLanguage = lan;
			this->_strCountry = cty;
#else
			_strLanguage.assign(lan.begin(), lan.end());
			_strCountry.assign(cty.begin(), cty.end());
#endif
		}
		std::tstring getLanguage(){return _strLanguage;}
		std::tstring getCountry(){return _strCountry;}
	};

	class Configuration;
	class TaskExecutor;
	class System
	{
	public:	
		static LONGLONG now();						// get current time in milliseconds
		static std::tstring getWorkPath();			//@wangyh26: ��ȡ����ǰ����Ŀ¼��
		static std::tstring getAppPath();
		static LONG fileLength(CSTRING);
		static std::tstring getMacAddress(INT i);	// i start from zero	
		static std::tstring getMacAddressNew();		// get the first available adapter
		static std::tstring getOsVersion();
		static std::tstring getOsVersion(OSVERSIONINFOEX& osvi);
		static void getDesktopResolution(int& horizontal, int& vertical);
		static Locale getDefaultLocale();
		static INT initDir(CSTRING dir);			//@wangyh26: ����Ŀ¼
		static BOOLEAN System::WMIQuery(CSTRING query, LPCWSTR key, std::wstring &value);
		static std::tstring getDeviceModel();
		static std::tstring getManufacturer();
		static std::tstring getProxyByIESetting(CSTRING dstUrl);
		static void asyncIESetting(Configuration *conf, TaskExecutor &exe);
		static std::tstring proxyUrlPostProcess(std::wstring proxyUrl);
	};

	class IoUtil
	{
	public:
		static void puti(std::tostream&, INT);
		static void putll(std::tostream&, LONGLONG);
		static INT geti(std::tistream &is);
		static LONGLONG getll(std::tistream &is);
	};

	// java style properties class
	class Properties
	{
	private:
		std::tstring _strFileName;
		std::map<std::tstring, std::tstring> _mapProps;
		void load();
	public:
		Properties() {};
		Properties(CSTRING fileName, CSTRING dir);
		std::tstring getProperty(CSTRING key);
		std::tstring getProperty(CSTRING key, CSTRING def);
	};

	//Lock
	class Lock;
	class LockBase
	{
		friend class Lock;
	
		//for windows xp
		HANDLE _mutex;				//@wangyh26: ������
		HANDLE _condition;			//@wangyh26: �¼�����
		BYTE	_byteWakeFlag;		//@wangyh26: ���ѱ�־λ

		static INT ID;				//@wangyh26: ��̬������ͳ������LockBase���������
		INT _iId;					//@wangyh26: ʵ�����������澲̬����ID�Ľ��������LockBase���������
		UINT	_iAwaitNum;			//@wangyh26: ʵ��������ͳ�Ƶ���LockBase������ʹ�øö���ļ���

		//@wangyh26: ����
		static const BYTE WAKE_FLAG_ALL = 0x01;
		static const BYTE WAKE_FLAG_ONE = 0x10;
	public:
		LockBase();					//@wangyh26: ͳ������LockBase������������Լ�����LockBase������ʹ�øö���ļ����������������塱�������̲߳�ӵ������Ĭ��Ϊ���źţ��͡��¼����󡱣��Զ���λ<�ͷŵ�1��wait�����߳�>����ʼ״̬���źţ�
		~LockBase();				//@wangyh26: �������ѱ�־λ��_byteWakeFlag���ϻ������У�ʹ���¼��������źţ��رա������塱�͡��¼�����
		void await(ULONG waitMs);	//@wangyh26: ͨ��_mutex������ķ���_condition���ȴ��䣨_condition�����źţ�wake()��wakeAll()��
		void wake();				//@wangyh26: �������ѱ�־λ��_byteWakeFlag���ϻ������У�ʹ���¼��������ź�
		void wakeAll();				//@wangyh26: �������ѱ�־λ��_byteWakeFlag���ϻ������У�ʹ���¼��������ź�
		void lock();				//@wangyh26: ��ȡ�������塱
		void unlock();				//@wangyh26: �ͷš������塱
	
				
		/**
		//for windows 7 and upper
		CRITICAL_SECTION _cs;
		CONDITION_VARIABLE _condition;
	public:
		LockBase(){_iId = ++ID;::InitializeCriticalSection(&this->_cs);::InitializeConditionVariable(&this->_condition);}
		~LockBase(){
			//this->wakeAll();
			::DeleteCriticalSection(&this->_cs);}

		void await(ULONG waitMs){::SleepConditionVariableCS (&this->_condition, &this->_cs, waitMs);}
		void wake(){::WakeConditionVariable(&this->_condition);}
		void wakeAll(){::WakeAllConditionVariable(&this->_condition);}

		void lock(){::EnterCriticalSection(&this->_cs);}
		void unlock(){::LeaveCriticalSection(&this->_cs);}
		**/
	};

	class Lock
	{
	private:
		LockBase *_pObj;
	public:
		//@wawngyh26: ��ʼ������LockBase��������_pObj��������lock������ȡ�������塱
		Lock(LockBase *p)
		{
			_pObj = p;
			if(_pObj != NULL)
				_pObj->lock();
		}

		//@wangyh26: �ͷ�LockBase����_pObj�ġ������塱
		~Lock()
		{
			if(_pObj != NULL)
				_pObj->unlock();
		}
	};

}
