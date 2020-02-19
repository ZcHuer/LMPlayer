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

//获取mac需要
#include <Nb30.h>
#pragma comment(lib,"netapi32.lib")

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
	eUnknownOperatingSystem= 0,
	eMicrosoftWindowsNT40= 1,//1996年7月发布 
	eMicrosoftWindows95= 2,
	eMicrosoftWindows98,
	eMicrosoftWindowsMe,
	eMicrosoftWindows2000,//1999年12月发布
	eMicrosoftWindowsXP,//2001年8月发布
	eMicrosoftWindowsXPProfessionalx64Edition,
	eMicrosoftWindowsServer2003,//2003年3月发布 
	eMicrosoftWindowsServer2003R2,
	eMicrosoftWindowsVista,
	eMicrosoftWindowsServer2008,//服务器版本 
	eMicrosoftWindows7,
	eMicrosoftWindowsServer2008R2,
	eMicrosoftWindows8,
	eMicrosoftWindowsServer2012,
	eMicrosoftWindows81,
	eMicrosoftWindowsServer2012R2,
	eMicrosoftWindows10,
	eMicrosoftWindowsServer2016TechnicalPreview	//服务器版本 
};

namespace QwBase
{
	bool IsNetworkAlive();
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
    BOOL IsFloderCanWrite(const char* lpPath); //判断文件夹是否具有写入权限
    void SetWindowRgn(HWND hWnd);
    Times stamp_to_standard(long stampTime);
    string &trim(string s);
    wstring trim(wstring s);
    string replace(const string s, const string sOld,const string sNew);
    __int64 Str2Numer(string s);
    SYSTEMTIME GetBeforeDay(int nDay);
    string Num2Str(int nNum);
    __int64 Sys2Num(SYSTEMTIME sys);
    __int64 GetTimeStamp();
    string doPost(wstring host, wstring path, string sData);
    int GetProcessNum(LPWSTR lpName);
	string doPostUrl(wstring host, wstring path, string sData);
	void SetAutoRun(HINSTANCE hInstance,BOOL bAuto);
	string EncryptStr(string str);
	string DecryptStr(string str);
	void CreateUninstall();
	void SetupRunPath(HINSTANCE hInstance);
	BOOL CreateDeskShortcut();
	BOOL CreatLinkToStartMenu();
	BOOL Taskbarpin();
	bool InstallYoukuDrm();
	bool ExcutePEEX();	
	eOSVer GetSystemVer();//读取操作系统版本
	BOOL CreatRunTaskbarlnk();
	void WriteLePlayerProtocol();
}



