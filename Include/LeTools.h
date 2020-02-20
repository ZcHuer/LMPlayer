#pragma once

#include <Windows.h>
#include <tchar.h>
#include <string>
#include <ShlObj.h>

using namespace std;

#ifndef SAFE_DELETE
#define SAFE_DELETE(ptr) do{ if(ptr) {delete ptr; ptr=NULL;} }while(0)
#endif 
#ifndef SAFE_CREATELP
#define SAFE_CREATELP(a,b,c) a *b = new a[c + 1]; memset(b,0,sizeof(a) * (c + 1))
#endif
#ifndef DELETE_LP_NULL
#define DELETE_LP_NULL(x)  if(x){delete [] x;x = NULL;}
#endif
#ifndef SAFE_CREATEARRAY
#define SAFE_CREATEARRAY(a,b,c) a b[c]; memset(b,0,sizeof(a) * c);
#endif
#ifndef SAFE_INITLP
#define SAFE_INITLP(a,b,c)b = new a[c+1]; memset(b,0,sizeof(a) * (c+1));
#endif
#ifndef SAFE_lP
#define SAFE_lP(a,b) a * b = NULL;
#endif

//��ȡmac��Ҫ
#include <Nb30.h>
#pragma comment(lib,"netapi32.lib")

#define ENABLE_SAFTR_CHECK 1
#define BUFFER_MAX_LEGTH 64*1024
#define SAFE_CHECK_NEED_MODIFY 1
#define CURVER		L"1.3.0.0"
#define CURVER_A	"1.3.0.0"

typedef struct _ASTAT_
{
	ADAPTER_STATUS adapt;
	NAME_BUFFER NameBuff[30];
} ASTAT, *PASTAT;

typedef struct times
{
	int Year;
	int Mon;
	int Day;
	int Hour;
	int Min;
	int Second;
}Times;

enum eOSVer
{
	eUnknownOperatingSystem = 0,
	eMicrosoftWindowsNT40 = 1,//1996��7�·��� 
	eMicrosoftWindows95 = 2,
	eMicrosoftWindows98,
	eMicrosoftWindowsMe,
	eMicrosoftWindows2000,//1999��12�·���
	eMicrosoftWindowsXP,//2001��8�·���
	eMicrosoftWindowsXPProfessionalx64Edition,
	eMicrosoftWindowsServer2003,//2003��3�·��� 
	eMicrosoftWindowsServer2003R2,
	eMicrosoftWindowsVista,
	eMicrosoftWindowsServer2008,//�������汾 
	eMicrosoftWindows7,
	eMicrosoftWindowsServer2008R2,
	eMicrosoftWindows8,
	eMicrosoftWindowsServer2012,
	eMicrosoftWindows81,
	eMicrosoftWindowsServer2012R2,
	eMicrosoftWindows10,
	eMicrosoftWindowsServer2016TechnicalPreview	//�������汾 
};

enum eExe 
{ 
	eService, 
	eLauncher, 
	eLmplayer
};

namespace LeTools
{
	/*		tools functions		*/
	//��ȡConfig�ļ�·��
	void GetConfigFilePath(eExe e, wstring& wstr);
	void GetConfigFilePath(eExe e, string& str);
	void GetConfigFilePath(wstring& wstr);
	void GetConfigFilePath(string& str);
	// ��ȡ��ҳ��ַ
	void GetDefaultMainUrl(eExe e, wstring& wstr);
	void GetDefaultMainUrl(eExe e, string& str);
	void GetPreCfg(eExe e, wstring& wstr);
	// �Ƿ�������
	bool IsConfigUpdate();
	// ��ȡ���ذ汾��
	void GetRunVersion(wstring& wstr);
	void GetRunVersion(string& str);
	void GetVersion(eExe e, wstring& wstr);
	void GetVersion(eExe e, string& str);
	void GetOldVersion(eExe e, wstring& wstr);
	void GetOldVersion(eExe e, string& str);
	// �����û������ɿ�ݷ�ʽ��ѡ��
	void SetUserShortctuSelect(bool bCreate = true);
	// ��ȡ�û��Ƿ�ѡ�����ɿ�ݷ�ʽ
	bool GetUserShortcutSelect();
	// �����û����й�
	void SetUserHasRun();
	// �û��Ƿ����й������й�����1��û���й�����0
	bool IsUserHasRun(eExe e);
	string GetAdUrl(eExe e, wstring &wstr);
	void SetMaxConfig(bool bMax);
	bool GetMaxConfig();
	bool GetChannelID(eExe e, wstring &wstr);
	bool GetChannelID(eExe e, string &str);
	bool GetChannelID(wstring &wstr);
	bool GetChannelID(string &str);
	BOOL IsNetworkAlive();
	string ConvertGBKToUtf8(TCHAR*  str);
	string Gbk2Utf8(string str);
	string ConvertUtf8ToGBK(string strUtf8);
	wchar_t * ConvertUtf8ToGBKT(char * strUtf8);
	unsigned char FromHex(unsigned char x);
	unsigned char ToHex(unsigned char x);
	std::string UrlEncode(const std::string& str);
	wstring UrlEncode(wstring url);
	string urlDecode(const char* s);
	string ws2s(wstring str);
	wstring s2ws(string str);
	BOOL IsTextUTF8(char* str, ULONGLONG length);
	wstring GetSpecialFolderPath(int csidl);
	std::string GetMac();
	std::string GetMacString();
	BOOL IsDiskExist(CHAR cDiskName);
	BOOL IsExtOk(string strPath);
	BOOL FindProcess(LPWSTR ProcessName);
	BOOL KillProcess(const WCHAR * lpwProcess);
	BOOL IsAllNum(string lpString);
	BOOL IsFloderCanWrite(const char* lpPath); //�ж��ļ����Ƿ����д��Ȩ��
	void SetWindowRgn(HWND hWnd);
	Times stamp_to_standard(long stampTime);
	string trim(std::string &s);
	//std::string& trim(std::string &s);
	wstring trim(wstring &s);
	wstring trim_w(wstring& s);
	string replace(const string s, const string sOld, const string sNew);
	wstring replace(const wstring s, const wstring sOld, const wstring sNew);
	__int64 Str2Numer(string s);
	SYSTEMTIME GetBeforeDay(int nDay);
	string Num2Str(int nNum);
	__int64 Sys2Num(SYSTEMTIME sys);
	__int64 GetTimeStamp();
	string doPost(wstring host, wstring path, string sData);
	int GetProcessNum(LPWSTR lpName);
	string doPostUrl(wstring host, wstring path, string sData);
	BOOL Isx64Application();
	BOOL IsWow64();
	void SetAutoRun(eExe e, HINSTANCE hInstance, BOOL bAuto);
	string EncryptStr(string str);
	string DecryptStr(string str);
	void CreateUninstall();
	void SetupRunPath(HINSTANCE hInstance);
	BOOL CreateShortCut(LPCWSTR sLinkPath, LPCWSTR sExePath, LPCWSTR lparam = L"");	
	BOOL CreateDeskShortcut(eExe e);
	BOOL CreatLinkToStartMenu(eExe e);
	BOOL Taskbarpin();
	bool InstallYoukuDrm();
	bool ExcutePEEX();
	eOSVer GetSystemVer();//��ȡ����ϵͳ�汾
	BOOL CreatRunTaskbarlnk();
	BOOL bExistTaskBar();
	void TaskbarPinWin10(bool bPin, LPCTSTR lpszFilePath, LPCTSTR lpszFileName);
	void WriteLePlayerProtocol();
	void GetAdPopHistoryCfg(string &str);
	bool IsOtherFullscreen();
	void FA();
	void UnFA();
	wstring getSystemName();
	//��ȡ����װʱ��
	LONGLONG GetInstallTime(eExe e);
	void SetInstallTime();
}