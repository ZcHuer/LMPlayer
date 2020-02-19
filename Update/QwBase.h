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

namespace QwBase
{
	string ConvertGBKToUtf8(TCHAR*  str);
    string ConvertUtf8ToGBK(string strUtf8);
    string ws2s(wstring str);
    wstring s2ws(string str);
    BOOL IsTextUTF8(char* str, ULONGLONG length);
    wstring GetSpecialFolderPath(int csidl);
    std::string GetMac();
    std::string GetMacString();
	BOOL IsDiskExist(CHAR cDiskName);
	string doPost(wstring host, wstring path, string sData);
	std::string UrlEncode(const std::string& str);
	string Num2Str(int nNum);
	string doPostUrl(wstring host, wstring path, string sData);
	BOOL KillProcess(const WCHAR * lpwProcess);
}



