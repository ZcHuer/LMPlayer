#include "stdafx.h"
#include "../include/LeTools.h"
#include "RegisterExt.h"
#include <winhttp.h>
#include <TlHelp32.h>
#include <chrono>
#include <shlobj.h>
#include <sensapi.h>
#include <shlwapi.h>
#include "..\SOUI\utilities\include\wtl.mini\souimisc.h"
#include "../SDK/Flog/FileLog.h"
#include <comutil.h>
#include <shellapi.h>
#include "base641.h"
#pragma comment(lib,"winhttp.lib")
#pragma comment(lib,"Sensapi.lib")

namespace LeTools
{
	void GetConfigFilePath(eExe e, wstring& wstr)
	{
		wchar_t wpath[MAX_PATH] = { 0 };
		::GetModuleFileNameW(NULL, wpath, MAX_PATH);
		::PathRemoveFileSpecW(wpath);
		if (eLmplayer == e)
			::PathRemoveFileSpecW(wpath);
		PathAppend(wpath, L"\\Config.ini");
		wstr = wpath;
		return;
	}

	void GetConfigFilePath(eExe e, string& str)
	{
		char chPath[MAX_PATH] = { 0 };
		::GetModuleFileNameA(NULL, chPath, MAX_PATH);
		::PathRemoveFileSpecA(chPath);
		if (eLmplayer == e)
			::PathRemoveFileSpecA(chPath);
		PathAppendA(chPath, "\\Config.ini");
		str = chPath;
		return;
	}

	void GetDefaultMainUrl(eExe e, wstring& wstr)
	{
#ifdef CONFIG_OFF
		wstr = L"https://leplayer.vgs.lenovo.com.cn";
		return;
#endif
		wstring wstrCfg;
		GetConfigFilePath(e, wstrCfg);
		WCHAR wBuff[MAX_PATH] = { 0 };
		GetPrivateProfileString(L"Debug", L"DebugUrl", L"", wBuff, 2048, wstrCfg.c_str());
		wstr = wBuff;
		if (wstr.length() == 0)
			wstr = L"https://leplayer.vgs.lenovo.com.cn";
		return;
	}

	void GetDefaultMainUrl(eExe e, string& str)
	{
#ifdef CONFIG_OFF
		str = "https://leplayer.vgs.lenovo.com.cn";
		return;
#endif
		string strCfg;
		GetConfigFilePath(e, strCfg);
		CHAR Buff[MAX_PATH] = { 0 };
		GetPrivateProfileStringA("Debug", "DebugUrl", "", Buff, 2048, strCfg.c_str());
		str = Buff;
		if (str.length() == 0)
			str = "https://leplayer.vgs.lenovo.com.cn";
		return;
	}

	void GetPreCfg(eExe e, wstring& wstr)
	{
#ifdef CONFIG_OFF
		wstr = L"http://lebo.vgs.lenovo.com.cn";
		return;
#endif
		wstring wstrCfg;
		GetConfigFilePath(e, wstrCfg);
		WCHAR wBuff[MAX_PATH] = { 0 };
		GetPrivateProfileString(L"Debug", L"JsPre", L"", wBuff, 2048, wstrCfg.c_str());
		wstr = wBuff;
		return;
	}

	bool IsConfigUpdate()
	{
#ifdef CONFIG_OFF
		return true;
#endif
		wstring wstr;
		GetConfigFilePath(eLmplayer, wstr);
		int bUpdate = GetPrivateProfileInt(L"Update", L"Update", 1, wstr.c_str());
		return (bool)bUpdate;
	}

	void GetVersion(eExe e, wstring& wstr)
	{
		wstring wstrCfg;
		GetConfigFilePath(e, wstrCfg);
		WCHAR wBuff[MAX_PATH] = { 0 };
		GetPrivateProfileString(L"Update", L"NewVer", CURVER, wBuff, 2048, wstrCfg.c_str());
		wstr = wBuff;
		if (wstr.length() == 0)
			wstr = CURVER;
		return;
	}

	void GetVersion(eExe e, string& str)
	{
		string strCfg;
		GetConfigFilePath(e, strCfg);
		CHAR cBuff[MAX_PATH] = { 0 };
		GetPrivateProfileStringA("Update", "NewVer", CURVER_A, cBuff, 2048, strCfg.c_str());
		str = cBuff;
		if (str.length() == 0)
			str = CURVER_A;
		return;
	}

	void GetOldVersion(eExe e, wstring& wstr)
	{
		wstring wstrCfg;
		GetConfigFilePath(e, wstrCfg);
		WCHAR wBuff[MAX_PATH] = { 0 };
		GetPrivateProfileString(L"Update", L"CurVer", CURVER, wBuff, 2048, wstrCfg.c_str());
		wstr = wBuff;
		if (wstr.length() == 0)
			wstr = CURVER;
		return;
	}

	void GetOldVersion(eExe e, string& str)
	{
		string strCfg;
		GetConfigFilePath(e, strCfg);
		CHAR cBuff[MAX_PATH] = { 0 };
		GetPrivateProfileStringA("Update", "CurVer", CURVER_A, cBuff, 2048, strCfg.c_str());
		str = cBuff;
		if (str.length() == 0)
			str = CURVER_A;
		return;
	}
	
	void SetUserShortctuSelect(bool bCreate /*= true*/)
	{
		wstring wstr;
		GetConfigFilePath(eLmplayer, wstr);
		if (bCreate)
			WritePrivateProfileStringW(L"UserSel", L"Shutcut", L"1", wstr.c_str());
		else
			WritePrivateProfileStringW(L"UserSel", L"Shutcut", L"0", wstr.c_str());
	}

	bool GetUserShortcutSelect()
	{
		wstring wstr;
		GetConfigFilePath(eLmplayer, wstr);
		int nCreate = GetPrivateProfileInt(L"UserSel", L"Shutcut", 1, wstr.c_str());
		return (bool)nCreate;
	}

	void SetUserHasRun()
	{
		wchar_t wpath[MAX_PATH] = { 0 };
		::GetModuleFileNameW(NULL, wpath, MAX_PATH);
		::PathRemoveFileSpecW(wpath);
		PathAppend(wpath, L"..\\Config.ini");
		WritePrivateProfileStringW(L"UserSel", L"hasrun", L"1", wpath);
	}

	bool IsUserHasRun(eExe e)
	{
		wchar_t wpath[MAX_PATH] = { 0 };
		::GetModuleFileNameW(NULL, wpath, MAX_PATH);
		::PathRemoveFileSpecW(wpath);
		PathAppend(wpath, L"..\\Config.ini");
		int nhasrun = GetPrivateProfileInt(L"UserSel", L"hasrun", 0, wpath);
		return (bool)nhasrun;
	}

	string GetAdUrl(eExe e, wstring &wstr)
	{
#ifdef CONFIG_OFF
		wstr = L"http://epg.vgs.lenovo.com.cn/getAdvertisement/62?advType=MUM&purpose=";
		return;
#endif
		wstring wstrCfg;
		GetConfigFilePath(e, wstrCfg);
		WCHAR wBuff[MAX_PATH] = { 0 };
		GetPrivateProfileString(L"Debug", L"AdvUrl", L"", wBuff, 2048, wstrCfg.c_str());
		wstr = wBuff;

		return "";
	}
	
	bool GetChannelID(eExe e, wstring &wstr)
	{
		wchar_t wpath[MAX_PATH] = { 0 };
		WCHAR wBuff[MAX_PATH] = { 0 };
		::GetModuleFileNameW(NULL, wpath, MAX_PATH);
		::PathRemoveFileSpecW(wpath);
		if (eLmplayer == e)
			::PathRemoveFileSpecW(wpath);
		PathAppend(wpath, L"\\Config.ini");
		GetPrivateProfileString(L"Channel", L"ChannelID", L"", wBuff, 2048, wpath);
		wstr = wBuff;
		return true;
	}

	bool GetChannelID(eExe e, string &str)
	{
		char chpath[MAX_PATH] = { 0 };
		char Buff[MAX_PATH] = { 0 };
		::GetModuleFileNameA(NULL, chpath, MAX_PATH);
		::PathRemoveFileSpecA(chpath);
		if (eLmplayer == e)
			::PathRemoveFileSpecA(chpath);
		PathAppendA(chpath, "\\Config.ini");
		GetPrivateProfileStringA("Channel", "ChannelID", "", Buff, 2048, chpath);
		str = Buff;
		return true;
	}

	BOOL IsNetworkAlive()
	{
		DWORD dwFlag = NETWORK_ALIVE_INTERNET;
		bool bLive = ::IsNetworkAlive(&dwFlag);
		DWORD dwError = GetLastError();
		if (dwError == 0)
		{
			if (bLive)
				return true;
		}
		return false;

	}

	/*
	Tools Functions
	*/


	string ConvertGBKToUtf8(TCHAR * str)
	{
		if (str == nullptr || _tcslen(str) == 0)return "";
		string strOut = "";
#ifndef  UNICODE
		//获得临时变量的大小
		int len = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
		wchar_t* wstr = new wchar_t[len + 1];
		memset(wstr, 0, len + 1);
		MultiByteToWideChar(CP_ACP, 0, str, -1, wstr, len);
		len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
		char* lpOut = new char[len + 1];
		memset(lpOut, 0, len + 1);
		WideCharToMultiByte(CP_UTF8, 0, wstr, -1, lpOut, len, NULL, NULL);
		if (wstr) delete[] wstr;
		strOut = lpOut;
		if (lpOut) delete[] lpOut;
#else
		//获得临时变量的大小
		int i = WideCharToMultiByte(CP_UTF8, 0, str, -1, NULL, 0, NULL, NULL);
		char* lpOut = new char[i + 1];
		memset(lpOut, 0, i + 1);
		WideCharToMultiByte(CP_UTF8, 0, str, -1, lpOut, i, NULL, NULL);
		strOut = lpOut;
		if (lpOut) delete[] lpOut;
#endif
		return strOut;
	}

	string Gbk2Utf8(string str)
	{
		if (str.length() == 0) return "";
		string strOutUTF8 = "";
		WCHAR * str1;
		int n = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
		str1 = new WCHAR[n];
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, str1, n);
		n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
		char * str2 = new char[n];
		WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);
		strOutUTF8 = str2;
		delete[]str1;
		str1 = NULL;
		delete[]str2;
		str2 = NULL;
		return strOutUTF8;
	}

	string ConvertUtf8ToGBK(string strUtf8)
	{
		if (strUtf8.length() == 0) return "";
		int len = MultiByteToWideChar(CP_UTF8, 0, strUtf8.c_str(), -1, NULL, 0);
		wchar_t * wszGBK = new wchar_t[len];
		memset(wszGBK, 0, len);
		MultiByteToWideChar(CP_UTF8, 0, strUtf8.c_str(), -1, wszGBK, len);
		len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
		char *szGBK = new char[len + 1];
		memset(szGBK, 0, len + 1);
		WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
		string strOut = szGBK;
		delete[] szGBK;
		delete[] wszGBK;
		return strOut;
	}
	wchar_t * ConvertUtf8ToGBKT(char * strUtf8)
	{
		if (strlen(strUtf8) == 0) return L"";
		int len = strlen(strUtf8);
		len = MultiByteToWideChar(CP_UTF8, 0, strUtf8, -1, NULL, 0);
		wchar_t * wszGBK = new wchar_t[len];
		memset(wszGBK, 0, len);
		MultiByteToWideChar(CP_UTF8, 0, strUtf8, -1, wszGBK, len);
		return wszGBK;

	}



	unsigned char FromHex(unsigned char x)
	{
		unsigned char y;
		if (x >= 'A' && x <= 'Z') {
			y = x - 'A' + 10;
		}
		else if (x >= 'a' && x <= 'z') {
			y = x - 'a' + 10;
		}
		else if (x >= '0' && x <= '9') {
			y = x - '0';
		}
		else {
			y = '0';
		}
		return y;
	}

	unsigned char ToHex(unsigned char x)
	{
		return  x > 9 ? x + 55 : x + 48;
	}
	const wchar_t * hexenc[] = {
		L"%00", L"%01", L"%02", L"%03", L"%04", L"%05", L"%06", L"%07",
		L"%08", L"%09", L"%0a", L"%0b", L"%0c", L"%0d", L"%0e", L"%0f",
		L"%10", L"%11", L"%12", L"%13", L"%14", L"%15", L"%16", L"%17",
		L"%18", L"%19", L"%1a", L"%1b", L"%1c", L"%1d", L"%1e", L"%1f",
		L"%20", L"%21", L"%22", L"%23", L"%24", L"%25", L"%26", L"%27",
		L"%28", L"%29", L"%2a", L"%2b", L"%2c", L"%2d", L"%2e", L"%2f",
		L"%30", L"%31", L"%32", L"%33", L"%34", L"%35", L"%36", L"%37",
		L"%38", L"%39", L"%3a", L"%3b", L"%3c", L"%3d", L"%3e", L"%3f",
		L"%40", L"%41", L"%42", L"%43", L"%44", L"%45", L"%46", L"%47",
		L"%48", L"%49", L"%4a", L"%4b", L"%4c", L"%4d", L"%4e", L"%4f",
		L"%50", L"%51", L"%52", L"%53", L"%54", L"%55", L"%56", L"%57",
		L"%58", L"%59", L"%5a", L"%5b", L"%5c", L"%5d", L"%5e", L"%5f",
		L"%60", L"%61", L"%62", L"%63", L"%64", L"%65", L"%66", L"%67",
		L"%68", L"%69", L"%6a", L"%6b", L"%6c", L"%6d", L"%6e", L"%6f",
		L"%70", L"%71", L"%72", L"%73", L"%74", L"%75", L"%76", L"%77",
		L"%78", L"%79", L"%7a", L"%7b", L"%7c", L"%7d", L"%7e", L"%7f",
		L"%80", L"%81", L"%82", L"%83", L"%84", L"%85", L"%86", L"%87",
		L"%88", L"%89", L"%8a", L"%8b", L"%8c", L"%8d", L"%8e", L"%8f",
		L"%90", L"%91", L"%92", L"%93", L"%94", L"%95", L"%96", L"%97",
		L"%98", L"%99", L"%9a", L"%9b", L"%9c", L"%9d", L"%9e", L"%9f",
		L"%a0", L"%a1", L"%a2", L"%a3", L"%a4", L"%a5", L"%a6", L"%a7",
		L"%a8", L"%a9", L"%aa", L"%ab", L"%ac", L"%ad", L"%ae", L"%af",
		L"%b0", L"%b1", L"%b2", L"%b3", L"%b4", L"%b5", L"%b6", L"%b7",
		L"%b8", L"%b9", L"%ba", L"%bb", L"%bc", L"%bd", L"%be", L"%bf",
		L"%c0", L"%c1", L"%c2", L"%c3", L"%c4", L"%c5", L"%c6", L"%c7",
		L"%c8", L"%c9", L"%ca", L"%cb", L"%cc", L"%cd", L"%ce", L"%cf",
		L"%d0", L"%d1", L"%d2", L"%d3", L"%d4", L"%d5", L"%d6", L"%d7",
		L"%d8", L"%d9", L"%da", L"%db", L"%dc", L"%dd", L"%de", L"%df",
		L"%e0", L"%e1", L"%e2", L"%e3", L"%e4", L"%e5", L"%e6", L"%e7",
		L"%e8", L"%e9", L"%ea", L"%eb", L"%ec", L"%ed", L"%ee", L"%ef",
		L"%f0", L"%f1", L"%f2", L"%f3", L"%f4", L"%f5", L"%f6", L"%f7",
		L"%f8", L"%f9", L"%fa", L"%fb", L"%fc", L"%fd", L"%fe", L"%ff"
	};

	wstring UrlEncode(wstring url)
	{
		if (url.length() == 0)return L"";
		wstring text = url;

		size_t len = text.length();
		std::wstring encoded = L"";
		for (size_t i = 0; i < len; i++)
		{
			wchar_t wch = text.at(i);
			if ('A' <= wch && wch <= 'Z') {
				encoded += wch;
			}
			else if ('a' <= wch && wch <= 'z') {
				encoded += wch;
			}
			else if ('0' <= wch && wch <= '9') {
				encoded += wch;
			}
			else if (wch == ' ') {
				encoded += hexenc[wch];
			}
			else if (wch == '-' || wch == '_'
				|| wch == '.' || wch == '!'
				|| wch == '~' || wch == '*'
				|| wch == '\'' || wch == '('
				|| wch == ')') {
				encoded += hexenc[wch];
			}
			else if (wch <= 0x007f) {        // other ASCII
				encoded += hexenc[wch];
			}
			else if (wch <= 0x07FF) {        // non-ASCII <= 0x7FF
				encoded += hexenc[0xc0 | (wch >> 6)];
				encoded += hexenc[0x80 | (wch & 0x3F)];
			}
			else {                    // 0x7FF < ch <= 0xFFFF
				encoded += hexenc[0xe0 | (wch >> 12)];
				encoded += hexenc[0x80 | ((wch >> 6) & 0x3F)];
				encoded += hexenc[0x80 | (wch & 0x3F)];
			}
		}
		return encoded;
	}

	std::string UrlEncode(const std::string& str)
	{
		if (str.length() == 0)return "";
		std::string strTemp = "";
		size_t length = str.length();
		for (size_t i = 0; i < length; i++)
		{
			if (isalnum((unsigned char)str[i]) ||
				(str[i] == '-') ||
				(str[i] == '_') ||
				(str[i] == '.') ||
				(str[i] == '~'))
				strTemp += str[i];
			else if (str[i] == ' ')
				strTemp += "+";
			else
			{
				strTemp += '%';
				strTemp += ToHex((unsigned char)str[i] >> 4);
				strTemp += ToHex((unsigned char)str[i] % 16);
			}
		}
		return strTemp;
	}

	string urlDecode(const char* s)
	{
		if (s == NULL || strlen(s) == 0) {
			return "";
		}
		std::string strTemp = "";
		size_t length = strlen(s);
		for (size_t i = 0; i < length; i++) {
			if (s[i] == '+') {
				strTemp += ' ';
			}
			else if (s[i] == '%') {
				if (i + 2 < length) {
					unsigned char high = FromHex((unsigned char)s[++i]);
					unsigned char low = FromHex((unsigned char)s[++i]);
					strTemp += high * 16 + low;
				}
			}
			else {
				strTemp += s[i];
			}
		}
		return strTemp;
	}

	string ws2s(wstring str)
	{
		string s = "";
		if (!str.empty()) {
			int nStrLen = str.length();
			SAFE_CREATELP(char, lpBuff, nStrLen * 2);
			WideCharToMultiByte(CP_ACP, 0, str.c_str(), nStrLen, lpBuff, nStrLen * 2, 0, 0);
			s = lpBuff;
			SAFE_DELETE(lpBuff);
		}
		return s;
	}

	wstring s2ws(string str)
	{
		wstring ws = L"";
		if (!str.empty())
		{
			int nStrLen = str.length();
			SAFE_CREATELP(WCHAR, lpBuff, nStrLen);
			MultiByteToWideChar(CP_ACP, 0, str.c_str(), nStrLen, lpBuff, nStrLen);
			ws = lpBuff;
			SAFE_DELETE(lpBuff);
		}
		return ws;
	}

	BOOL IsTextUTF8(char* str, ULONGLONG length)
	{
		DWORD nBytes = 0;//UFT8可用1-6个字节编码,ASCII用一个字节  
		UCHAR chr;
		BOOL bAllAscii = TRUE; //如果全部都是ASCII, 说明不是UTF-8  
		for (int i = 0; i<length; ++i)
		{
			chr = *(str + i);
			if ((chr & 0x80) != 0) // 判断是否ASCII编码,如果不是,说明有可能是UTF-8,ASCII用7位编码,但用一个字节存,最高位标记为0,o0xxxxxxx  
				bAllAscii = FALSE;
			if (nBytes == 0) //如果不是ASCII码,应该是多字节符,计算字节数  
			{
				if (chr >= 0x80)
				{
					if (chr >= 0xFC && chr <= 0xFD)
						nBytes = 6;
					else if (chr >= 0xF8)
						nBytes = 5;
					else if (chr >= 0xF0)
						nBytes = 4;
					else if (chr >= 0xE0)
						nBytes = 3;
					else if (chr >= 0xC0)
						nBytes = 2;
					else
						return FALSE;

					nBytes--;
				}
			}
			else //多字节符的非首字节,应为 10xxxxxx  
			{
				if ((chr & 0xC0) != 0x80)
					return FALSE;

				nBytes--;
			}
		}
		if (nBytes > 0) //违返规则  
			return FALSE;
		if (bAllAscii) //如果全部都是ASCII, 说明不是UTF-8  
			return FALSE;

		return TRUE;
	}

	wstring GetSpecialFolderPath(int csidl)
	{
		WCHAR szBuffer[MAX_PATH];
		SHGetSpecialFolderPathW(NULL, szBuffer, csidl, FALSE);
		return szBuffer;
	}

	std::string GetMacString()
	{
		string mac;
		mac = "Mac地址为:";
		SAFE_CREATELP(char, Str, 32);
		ASTAT Adapter;
		NCB Ncb;
		UCHAR uRetCode;
		LANA_ENUM lenum;
		int i = 0;
		memset(&Ncb, 0, sizeof(Ncb));
		Ncb.ncb_command = NCBENUM;
		Ncb.ncb_buffer = (UCHAR *)&lenum;
		Ncb.ncb_length = sizeof(lenum);
		uRetCode = Netbios(&Ncb);
		for (i = 0; i < lenum.length; i++)
		{
			memset(&Ncb, 0, sizeof(Ncb));
			Ncb.ncb_command = NCBRESET;
			Ncb.ncb_lana_num = lenum.lana[i];
			uRetCode = Netbios(&Ncb);
			memset(&Ncb, 0, sizeof(Ncb));
			Ncb.ncb_command = NCBASTAT;
			Ncb.ncb_lana_num = lenum.lana[i];
			strcpy_s((char *)Ncb.ncb_callname, NCBNAMSZ, "* ");
			Ncb.ncb_buffer = (unsigned char *)&Adapter;
			Ncb.ncb_length = sizeof(Adapter);
			uRetCode = Netbios(&Ncb);
			if (uRetCode == 0)
			{
				sprintf_s(Str, 32, "%02X%02X%02X%02X%02X%02X",
					Adapter.adapt.adapter_address[0],
					Adapter.adapt.adapter_address[1],
					Adapter.adapt.adapter_address[2],
					Adapter.adapt.adapter_address[3],
					Adapter.adapt.adapter_address[4],
					Adapter.adapt.adapter_address[5]
				);
				break;
			}
		}
		mac = Str;
		DELETE_LP_NULL(Str);
		return mac;
	}
	BOOL IsDiskExist(CHAR cDiskName)
	{
		DWORD dwDrivers;
		int i = cDiskName - 'A';

		//dwDrivers的每一个二进制位表示对应的驱动器是否存在。  
		dwDrivers = GetLogicalDrives();
		//判断当前位是否有驱动器  
		if ((dwDrivers & (1 << (i))) != 0)
		{
			return TRUE;
		}
		return FALSE;
	}
	BOOL IsExtOk(string strPath)
	{
		LPSTR lpExtenson = PathFindExtensionA(strPath.c_str());
		string sExt = lpExtenson;
		if (sExt.size() <= 1) return FALSE;
		for (size_t i = 1; i < sExt.size(); i++)
		{
			if ((sExt[i]<'0' || sExt[i]>'9') &&
				(sExt[i]<'a' || sExt[i]>'z')
				&& (sExt[i]<'A' || sExt[i]>'Z'))
			{
				return FALSE;
			}
		}
		return TRUE;
	}
	BOOL FindProcess(LPWSTR ProcessName)
	{
		HANDLE hProcessSnap;
		PROCESSENTRY32 pe32;
		// Take a snapshot of all processes in the system.
		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE) {
			return(FALSE);
		}

		pe32.dwSize = sizeof(PROCESSENTRY32);
		if (!Process32First(hProcessSnap, &pe32))
		{
			CloseHandle(hProcessSnap);          // clean the snapshot object
			return(FALSE);
		}

		BOOL bRet = FALSE;
		do
		{
			if (!_wcsicmp(ProcessName, pe32.szExeFile))
			{
				bRet = TRUE;
				break;
			}
		} while (Process32Next(hProcessSnap, &pe32));
		CloseHandle(hProcessSnap);
		return bRet;
	}
	BOOL KillProcess(const WCHAR * lpwProcess)
	{
		LPWSTR File = NULL;
		HANDLE hProcessSnap;
		PROCESSENTRY32 pe32;
		//通过获取进程信息为指定的进程、进程使用的堆[HEAP]、模块[MODULE]、线程[THREAD]建立一个快照[snapshot]
		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		pe32.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hProcessSnap, &pe32))
		{
			do
			{
				File = pe32.szExeFile;
				if (_wcsicmp(File, lpwProcess) == 0)
				{
					HANDLE hProcessHandle = ::OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
					::TerminateProcess(hProcessHandle, 4);
					CloseHandle(hProcessSnap);
					return TRUE;
				}
			} while (Process32Next(hProcessSnap, &pe32));
		}
		CloseHandle(hProcessSnap);
		return FALSE;
	}

	BOOL IsAllNum(string lpString)
	{
		for (int i = 0; i < lpString.size(); i++)
		{
			int tmp = (int)lpString[i];
			if (tmp >= 48 && tmp <= 57) {
				continue;
			}
			else {
				return FALSE;
			}
		}
		return TRUE;
	}
	BOOL IsFloderCanWrite(const char * lpPath)
	{
		if (!lpPath || strlen(lpPath) == 0) return FALSE;
		long lTime = time(NULL);
		string strPath = lpPath;
		strPath.append("\\");
		char cBuff[64] = { 0 };
		_ltoa(lTime, cBuff, 10);
		strPath.append(cBuff);
		strPath.append(".ini");
		FILE* file = fopen(strPath.c_str(), "wb");
		if (file)
		{
			fclose(file);
			::DeleteFileA(strPath.c_str());
			return TRUE;
		}
		return FALSE;
	}

	Times stamp_to_standard(long stampTime)
	{
		time_t tick = (time_t)stampTime;
		struct tm tm;
		char s[100];
		Times standard;

		tm = *localtime(&tick);
		strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S", &tm);
		printf("%d: %s\n", (int)tick, s);

		standard.Year = atoi(s);
		standard.Mon = atoi(s + 5);
		standard.Day = atoi(s + 8);
		standard.Hour = atoi(s + 11);
		standard.Min = atoi(s + 14);
		standard.Second = atoi(s + 17);

		return standard;
	}
	string trim(string& s)
	{
		if (s.empty()) {
			return s;
		}
		s.erase(0, s.find_first_not_of(" "));
		s.erase(s.find_last_not_of(" ") + 1);
		return s;
	}
	wstring trim_w(wstring& s)
	{
		if (s.empty()) {
			return s;
		}
		s.erase(0, s.find_first_not_of(L" "));
		s.erase(s.find_last_not_of(L" ") + 1);
		return s;
	}
	string replace(const string s, const string sOld, const string sNew)
	{
		string sDes = s;
		string sTmp = "";
		while (sDes.find(sOld) != -1)
		{
			sTmp = sDes.substr(0, sDes.find(sOld));
			sTmp.append(sNew);
			sTmp.append(sDes.substr(sDes.find(sOld) + sOld.length()));
			sDes = sTmp;
		}
		return sDes;
	}
	wstring replace(const wstring s, const wstring sOld, const wstring sNew)
	{
		wstring sDes = s;
		wstring sTmp = L"";
		while (sDes.find(sOld) != -1)
		{
			sTmp = sDes.substr(0, sDes.find(sOld));
			sTmp.append(sNew);
			sTmp.append(sDes.substr(sDes.find(sOld) + sOld.length()));
			sDes = sTmp;
		}
		return sDes;
	}
	__int64 Str2Numer(string s)
	{
		string sStr = s;
		__int64 lNumber = 0;
		for (size_t i = 0; i < sStr.length(); i++)
			lNumber = lNumber * 10 + sStr[i] - '0';
		return lNumber;
	}
	SYSTEMTIME GetBeforeDay(int nDay)
	{
		time_t t;
		struct tm *m;
		t = time(NULL) - nDay * 24 * 3600;
		tm temptm = *localtime(&t);
		SYSTEMTIME st = { 1900 + temptm.tm_year,
			1 + temptm.tm_mon,
			temptm.tm_wday,
			temptm.tm_mday,
			temptm.tm_hour,
			temptm.tm_min,
			temptm.tm_sec,
			0 };
		return st;
	}
	string Num2Str(int nNum)
	{
		char cBuff[120] = { 0 };
		_itoa(nNum, cBuff, 10);
		return cBuff;
	}
	__int64 Sys2Num(SYSTEMTIME sys)
	{
		__int64 llNum = 0;
		llNum += sys.wYear;
		llNum *= 100;
		llNum += sys.wMonth;
		llNum *= 100;
		llNum += sys.wDay;
		llNum *= 100;
		llNum += sys.wHour;
		llNum *= 100;
		llNum += sys.wMinute;
		llNum *= 100;
		llNum += sys.wSecond;
		return llNum;
	}
	__int64 GetTimeStamp()
	{
		std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
		auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch());
		time_t timestamp = tmp.count();
		return timestamp;
	}

	string doPost(wstring host, wstring path, string sData)
	{
		DWORD dwSize = 0;
		DWORD dwDownloaded = 0;
		LPSTR pszOutBuffer = NULL;
		HINTERNET  hSession = NULL, hConnect = NULL, hRequest = NULL;

		BOOL  bResults = FALSE;

		//初始化，为应用程序，使用WinHTTP的功能，并返回一个WinHTTP的会话句柄。
		hSession = WinHttpOpen(L"User-Agent", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);

		if (hSession)
		{
			host = host.substr(host.find(L"//") + 2);
			host = host.substr(0, host.find(L"/"));
			//指定一个HTTP请求的初始目标服务器并返回一个HINTERNET连接句柄该初始目标的HTTP会话。
			hConnect = WinHttpConnect(hSession,
				host.c_str(), INTERNET_DEFAULT_HTTP_PORT, 0);
		}

		if (hConnect)
		{
			//创建一个HTTP请求句柄。
			hRequest = WinHttpOpenRequest(hConnect, L"POST",
				path.c_str(), NULL,
				WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
		}
		if (hRequest)
		{
			WinHttpAddRequestHeaders(hRequest,
				L"Content-Type: application/json",
				wcslen(L"Content-Type: application/json"),
				WINHTTP_ADDREQ_FLAG_ADD);

			std::string data = sData;//"name=6000000000&password=60000000&timestamp=1497333252384";

			const void *ss = (const char *)data.c_str();

			bResults = WinHttpSendRequest(hRequest, 0, 0, const_cast<void *>(ss),
				data.length(), data.length(), 0);
		}

		if (bResults)
		{
			bResults = WinHttpReceiveResponse(hRequest, NULL);

		}
		string sResult = "";
		if (bResults)
		{
			do
			{
				// Check for available data.
				dwSize = 0;
				if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
				{
					printf("Error %u in WinHttpQueryDataAvailable.\n", GetLastError());
					break;
				}
				if (!dwSize)
					break;
				pszOutBuffer = new char[dwSize + 1];
				if (!pszOutBuffer)
				{
					printf("Out of memory\n");
					break;
				}
				ZeroMemory(pszOutBuffer, dwSize + 1);
				if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded))
				{
					printf("Error %u in WinHttpReadData.\n", GetLastError());
				}
				else
				{
					printf("%s", pszOutBuffer);
				}
				sResult += pszOutBuffer;
				delete[] pszOutBuffer;

				if (!dwDownloaded)
					break;

			} while (dwSize > 0);
		}
		if (hRequest) WinHttpCloseHandle(hRequest);
		if (hConnect) WinHttpCloseHandle(hConnect);
		if (hSession) WinHttpCloseHandle(hSession);
		return sResult;
	}

	int GetProcessNum(LPWSTR lpName)
	{
		HANDLE hProcessSnap;
		PROCESSENTRY32 pe32;
		// Take a snapshot of all processes in the system.
		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE) {
			return 0;
		}

		pe32.dwSize = sizeof(PROCESSENTRY32);
		if (!Process32First(hProcessSnap, &pe32))
		{
			CloseHandle(hProcessSnap);          // clean the snapshot object
			return 0;
		}
		int nCount = 0;
		do
		{
			if (!_wcsicmp(lpName, pe32.szExeFile))
			{
				nCount++;
			}
		} while (Process32Next(hProcessSnap, &pe32));
		CloseHandle(hProcessSnap);
		return nCount;
	}

	string doPostUrl(wstring host, wstring path, string sData)
	{
		DWORD dwSize = 0;
		DWORD dwDownloaded = 0;
		LPSTR pszOutBuffer = NULL;
		HINTERNET  hSession = NULL, hConnect = NULL, hRequest = NULL;

		BOOL  bResults = FALSE;

		//初始化，为应用程序，使用WinHTTP的功能，并返回一个WinHTTP的会话句柄。
		hSession = WinHttpOpen(L"User-Agent", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);

		if (hSession)
		{
			host = host.substr(host.find(L"//") + 2);
			host = host.substr(0, host.find(L"/"));
			//指定一个HTTP请求的初始目标服务器并返回一个HINTERNET连接句柄该初始目标的HTTP会话。
			hConnect = WinHttpConnect(hSession,
				host.c_str(), INTERNET_DEFAULT_HTTP_PORT, 0);
		}

		if (hConnect)
		{
			//创建一个HTTP请求句柄。
			hRequest = WinHttpOpenRequest(hConnect, L"POST",
				path.c_str(), NULL,
				WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
		}
		if (hRequest)
		{
			WinHttpAddRequestHeaders(hRequest,
				L"Content-Type: application/x-www-form-urlencoded",
				wcslen(L"Content-Type: application/x-www-form-urlencoded"),
				WINHTTP_ADDREQ_FLAG_ADD);

			std::string data = sData;//"name=6000000000&password=60000000&timestamp=1497333252384";

			const void *ss = (const char *)data.c_str();

			bResults = WinHttpSendRequest(hRequest, 0, 0, const_cast<void *>(ss),
				data.length(), data.length(), 0);
		}

		if (bResults)
		{
			bResults = WinHttpReceiveResponse(hRequest, NULL);

		}
		string sResult = "";
		if (bResults)
		{
			do
			{
				// Check for available data.

				dwSize = 0;

				if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
				{
					printf("Error %u in WinHttpQueryDataAvailable.\n", GetLastError());

					break;
				}

				if (!dwSize)
					break;

				pszOutBuffer = new char[dwSize + 1];

				if (!pszOutBuffer)
				{
					printf("Out of memory\n");
					break;
				}

				ZeroMemory(pszOutBuffer, dwSize + 1);

				if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded))
				{
					printf("Error %u in WinHttpReadData.\n", GetLastError());
				}
				else
				{
					printf("%s", pszOutBuffer);
				}
				sResult = pszOutBuffer;
				delete[] pszOutBuffer;

				if (!dwDownloaded)
					break;

			} while (dwSize > 0);
		}
		if (hRequest) WinHttpCloseHandle(hRequest);
		if (hConnect) WinHttpCloseHandle(hConnect);
		if (hSession) WinHttpCloseHandle(hSession);
		return sResult;
	}



	typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

	/**
	* Don't use the function IsWow64Process as a static function,
	* you should load it by function GetProcAddress, because
	* it is not available on all version of Windows.
	*/
	LPFN_ISWOW64PROCESS fnIsWow64Process = NULL;

	/**
	* This function tells if your application is a x64 program.
	*/
	BOOL Isx64Application()
	{
		return (sizeof(LPFN_ISWOW64PROCESS) == 8) ? TRUE : FALSE;
	}

	/**
	* This function tells if you're under Windows x64.
	*/
	BOOL IsWow64()
	{
		BOOL bIsWow64 = FALSE;
		if (!fnIsWow64Process)
			fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "IsWow64Process");
		if (fnIsWow64Process)
		{
			if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64))
				return FALSE;
		}

		return bIsWow64;
	}

	void SetAutoRun(eExe e, HINSTANCE hInstance, BOOL bAuto)
	{
		FLOG(_T("SetAutoRun begin"));
		LONG lret = ERROR_SUCCESS;
		char pvData[256] = { 0 };
		DWORD dwDataSize = sizeof(pvData);
		lret = RegGetValueA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", "LePlayer", RRF_RT_REG_SZ, NULL, pvData, &dwDataSize);
		if (lret == ERROR_SUCCESS)
		{
			FLOG(L"SetAutoRun lret == ERROR_SUCCESS");
			FLOG(L"%s", LeTools::s2ws(pvData).c_str());
			//成功
			if (FALSE == bAuto)
			{
				RegDeleteKeyValueA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", "LePlayer");
			}
			return;
		}
		// 程序路径
		CHAR chPath_Run[MAX_PATH] = { 0 };
		::GetModuleFileNameA(hInstance, chPath_Run, MAX_PATH);
		if (e == eLmplayer)
		{
			PathRemoveFileSpecA(chPath_Run);
			PathRemoveFileSpecA(chPath_Run);
			PathAppendA(chPath_Run, "\\LMPlayer.exe /AutoRun-lmp");
		}
		else if (e == eLauncher)
		{
			PathRemoveFileSpecA(chPath_Run);
			PathAppendA(chPath_Run, "\\LMPlayer.exe /AutoRun-LMPlayer");
		}
		else if (e == eService)
		{
			PathRemoveFileSpecA(chPath_Run);
			PathAppendA(chPath_Run, "\\LMPlayer.exe /AutoRun-Service");
		}

		HKEY hKey = NULL;
		REGSAM samDesiredOpen = KEY_ALL_ACCESS;
		// 判断系统是32位还是64位，调用不同的注册表函数
		if (IsWow64())
		{
			FLOG(_T("IsWow64"));
			samDesiredOpen |= KEY_WOW64_64KEY;
		}

		lret = RegCreateKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, L"", 0, samDesiredOpen, NULL, &hKey, 0);
		if (lret != ERROR_SUCCESS)
			FLOG(_T("SetAutoRun create failed return %d", lret));		

		lret = RegSetValueExA(hKey, "LMPlayer", 0, REG_SZ, (BYTE*)chPath_Run, strlen(chPath_Run));
		if (lret != ERROR_SUCCESS)
		{
			FLOG(_T("SetAutoRun set failed return %d", lret));
			return;
		}

		//RegEnableReflectionKey(HKEY_LOCAL_MACHINE);
		//RegDisableReflectionKey(HKEY_LOCAL_MACHINE);
		if (hKey)
			RegCloseKey(hKey);

		FLOG(_T("SetAutoRun end"));
		return;
	}


	string EncryptStr(string str)
	{
		DWORD dwSize = str.length() * 2;
		unsigned char* cDes = (unsigned char*)malloc(dwSize + 1);
		memset(cDes, 0, dwSize + 1);
		base64_encode((unsigned char*)str.c_str(), str.length(), cDes, &dwSize);

		unsigned char* cP = cDes;
		for (int i = 0; i < dwSize; i++)
		{
			*cP = (*cP) ^ 0x01;
			cP++;
		}

		str = (char*)cDes;
		free(cDes);

		return str;

	}

	string DecryptStr(string str)
	{
		DWORD dwSize = str.length();
		unsigned char* cSrc = (unsigned char*)malloc(dwSize + 1);
		memset(cSrc, 0, dwSize + 1);
		memcpy(cSrc, str.c_str(), dwSize);

		unsigned char* cP = cSrc;
		for (int i = 0; i < dwSize; i++)
		{
			*cP = (*cP) ^ 0x01;
			cP++;
		}

		unsigned char* cDes = (unsigned char*)malloc(dwSize);
		memset(cDes, 0, dwSize);
		base64_decode(cSrc, str.length(), cDes, &dwSize);

		str = (char*)cDes;
		free(cSrc);
		free(cDes);

		return str;
	}

	/*
	string strphone = "`1234567890-=abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ~!@#$%^&*()_+,./;'[]\<>?:{}|";
	string strE = LeTools::EncryptStr(strphone);
	string strD = LeTools::DecryptStr(strE);
	if (0 != strphone.compare(strD.c_str()))
	{
	int a = 1;
	}
	*/

	void CreateUninstall()
	{
		FLOG(_T("CreateUninstall begin"));
		LONG lret = ERROR_SUCCESS;
		char pvData[256] = { 0 };
		DWORD dwDataSize = sizeof(pvData);

		// 程序路径
		CHAR chPath_Run[MAX_PATH] = { 0 };
		::GetModuleFileNameA(NULL, chPath_Run, MAX_PATH);
		PathRemoveFileSpecA(chPath_Run);
		PathRemoveFileSpecA(chPath_Run);
		PathAppendA(chPath_Run, "\\LMPlayer.exe");

		CHAR chUnPath[MAX_PATH] = { 0 };
		::GetModuleFileNameA(NULL, chUnPath, MAX_PATH);
		PathRemoveFileSpecA(chUnPath);
		PathAppendA(chUnPath, "\\uninst.exe");

		CHAR chVersion[MAX_PATH] = { 0 };
		wstring version;
		GetVersion(eLmplayer, version);
		PathAppendA(chVersion, ws2s(version).c_str());

		HKEY hKey = NULL;
		REGSAM samDesiredOpen = KEY_ALL_ACCESS;
		// 判断系统是32位还是64位，调用不同的注册表函数
		if (IsWow64())
		{
			FLOG(_T("IsWow64"));
			samDesiredOpen |= KEY_WOW64_64KEY;
		}
		if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall", 0, samDesiredOpen, &hKey) != ERROR_SUCCESS)
		{
			//\\LePlayer
			FLOG(_T("打开卸载注册表失败"));
			return;
		}
		HKEY lsub;
		if (RegCreateKeyA(hKey, "LMPlayer", &lsub) != ERROR_SUCCESS)
		{
			FLOG(_T("创建失败"));
			return;
		}

		if (RegSetValueExA(lsub, "DisplayIcon", 0, REG_SZ, (BYTE*)chPath_Run, strlen(chPath_Run)) != ERROR_SUCCESS)
		{
			FLOG(_T("写入卸载信息失败"));
			return;
		}
		char playName[120] = "LMPlayer";
		if (RegSetValueExA(lsub, "DisplayName", 0, REG_SZ, (BYTE*)playName, strlen(playName)) != ERROR_SUCCESS)
		{
			FLOG(_T("写入卸载信息失败"));
			return;
		}
		if (RegSetValueExA(lsub, "DisplayIcon", 0, REG_SZ, (BYTE*)chPath_Run, strlen(chPath_Run)) != ERROR_SUCCESS)
		{
			FLOG(_T("写入卸载信息失败"));
			return;
		}

		if (RegSetValueExA(lsub, "DisplayVersion", 0, REG_SZ, (BYTE*)chVersion, strlen(chVersion)) != ERROR_SUCCESS)
		{
			FLOG(_T("写入卸载信息失败"));
			return;
		}
		if (RegSetValueExA(lsub, "UninstallString", 0, REG_SZ, (BYTE*)chUnPath, strlen(chUnPath)) != ERROR_SUCCESS)
		{
			FLOG(_T("写入卸载信息失败"));
			return;
		}
		char chPublisher[120] = "LMPlayer";
		if (RegSetValueExA(lsub, "Publisher", 0, REG_SZ, (BYTE*)chPublisher, strlen(chPublisher)) != ERROR_SUCCESS)
		{
			FLOG(_T("写入卸载信息失败"));
			return;
		}
		char chURLInfoAbout[120] = "https://www.lenovo.com.cn/";
		if (RegSetValueExA(lsub, "URLInfoAbout", 0, REG_SZ, (BYTE*)chURLInfoAbout, strlen(chURLInfoAbout)) != ERROR_SUCCESS)
		{
			FLOG(_T("写入卸载信息失败"));
			return;
		}
		char chInstallLocation[MAX_PATH] = { 0 };
		PathAppendA(chInstallLocation, chUnPath);
		PathRemoveFileSpecA(chInstallLocation);
		PathRemoveFileSpecA(chInstallLocation);

		if (RegSetValueExA(lsub, "InstallLocation", 0, REG_SZ, (BYTE*)chInstallLocation, strlen(chInstallLocation)) != ERROR_SUCCESS)
		{
			FLOG(_T("写入卸载信息失败"));
			return;
		}
		//REG_DWORD
		char chEstimatedSize[MAX_PATH] = "200000";
		if (RegSetValueExA(lsub, "EstimatedSize", 0, REG_DWORD, (BYTE*)chEstimatedSize, strlen(chEstimatedSize)) != ERROR_SUCCESS)
		{
			FLOG(_T("写入卸载信息失败"));
			return;
		}
		char chctext[MAX_PATH] = "LMPlayer";
		if (RegSetValueExA(lsub, "ctext", 0, REG_SZ, (BYTE*)chctext, strlen(chctext)) != ERROR_SUCCESS)
		{
			FLOG(_T("写入卸载信息失败"));
			return;
		}
		char chcurl[120] = "https://www.lenovo.com.cn/";
		if (RegSetValueExA(lsub, "curl", 0, REG_SZ, (BYTE*)chcurl, strlen(chcurl)) != ERROR_SUCCESS)
		{
			FLOG(_T("写入卸载信息失败"));
			return;
		}

		if (hKey)
			RegCloseKey(hKey);

		FLOG(_T("CreateUninstall end"));
		return;

	}

	// 向注册表中设置安装路径
	void SetupRunPath(HINSTANCE hInstance)
	{
		FLOG(_T("SetupRunPath begin"));
		// 程序路径
		CHAR chPath_Run[MAX_PATH] = { 0 };
		::GetModuleFileNameA(hInstance, chPath_Run, MAX_PATH);
		PathRemoveFileSpecA(chPath_Run);
		PathRemoveFileSpecA(chPath_Run);
		PathAppendA(chPath_Run, "\\LMPlayer.exe");

		HKEY hKey = NULL;
		REGSAM samDesiredOpen = KEY_ALL_ACCESS;
		// 判断系统是32位还是64位，调用不同的注册表函数
		if (IsWow64())
		{
			FLOG(_T("IsWow64"));
			samDesiredOpen |= KEY_WOW64_64KEY;
		}
		LSTATUS lRes = RegCreateKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\LMPlayer", 0, "", 0, samDesiredOpen, NULL, &hKey, 0);
		if (lRes != ERROR_SUCCESS)
			FLOG(_T("SetupRunPath create failed"));

		lRes = RegCreateKeyExA(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\LMPlayer", 0, "", 0, samDesiredOpen, NULL, &hKey, 0);
		if (lRes != ERROR_SUCCESS)
			FLOG(_T("SetupRunPath create failed"));

		if (RegSetValueExA(hKey, "LMPlayer", 0, REG_SZ, (BYTE*)chPath_Run, strlen(chPath_Run)) != ERROR_SUCCESS)
			FLOG(_T("SetupRunPath set failed"));

		//RegEnableReflectionKey(HKEY_LOCAL_MACHINE);
		//RegDisableReflectionKey(HKEY_LOCAL_MACHINE);
		if (hKey)
			RegCloseKey(hKey);

		FLOG(_T("SetupRunPath end"));
		return;
	}

	// 创建快捷方式-通用
	BOOL CreateShortCut(LPCWSTR sLinkPath, LPCWSTR sExePath, LPCWSTR lparam /*= L""*/)
	{
		//删除已有的LNK
		::DeleteFileW(sLinkPath);
		BOOL bRet = FALSE;
		HRESULT hr = CoInitialize(NULL);
		if (SUCCEEDED(hr))
		{
			IShellLinkW *pisl;
			hr = CoCreateInstance(CLSID_ShellLink, NULL,
				CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&pisl);
			if (SUCCEEDED(hr))
			{
				IPersistFile* pIPF;
				//////////////////////////////////////////////////////////
				//这里是我们要创建快捷方式的原始文件地址
				pisl->SetPath(sExePath);
				pisl->SetArguments(lparam);
				hr = pisl->QueryInterface(IID_IPersistFile, (void**)&pIPF);
				if (SUCCEEDED(hr))
				{
					//这里是我们要创建快捷方式的目标地址
					//L"C:\\Users\\ChenWang\\Desktop\\记事本.lnk"
					hr = pIPF->Save(sLinkPath, TRUE);
					if (SUCCEEDED(hr)) bRet = TRUE;
					pIPF->Release();
				}
				pisl->Release();
			}
			CoUninitialize();
		}
		return bRet;
	}

	// 创建快捷方式-桌面
	BOOL CreateDeskShortcut(eExe e)
	{
		// 获取模块所在路径
		WCHAR wsExePath[MAX_PATH] = { 0 };
		GetModuleFileNameW(NULL, wsExePath, sizeof(wsExePath));
		if (e == eLmplayer)
		{
			PathRemoveFileSpecW(wsExePath);
			PathRemoveFileSpecW(wsExePath);
		}
		else if (e == eLauncher || e == eService)
		{
			PathRemoveFileSpecW(wsExePath);
		}
		PathAppendW(wsExePath, L"\\LMPlayer.exe");

		// 获取桌面路径
		BOOL bRet = FALSE;
		LPITEMIDLIST pidl;
		LPMALLOC pShellMalloc;
		WCHAR szDir[MAX_PATH] = { 0 };
		if (SUCCEEDED(SHGetMalloc(&pShellMalloc)))
		{
			if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &pidl)))
			{
				// 如果成功返回true
				bRet = SHGetPathFromIDListW(pidl, szDir);
				pShellMalloc->Free(pidl);
			}
			pShellMalloc->Release();
		}
		if (!bRet)
			return bRet;

		WCHAR wPath[MAX_PATH] = { 0 };
		wsprintf(wPath, L"%s\\%s.lnk", szDir, L"LMPlayer");
		bRet = CreateShortCut(wPath, wsExePath, L"-desktop.lnk");
		return bRet;
	}


	// 创建快捷方式-开始菜单
	BOOL CreatLinkToStartMenu(eExe e)
	{
		// 获取模块所在路径
		WCHAR wsExePath[MAX_PATH] = { 0 };
		GetModuleFileNameW(NULL, wsExePath, sizeof(wsExePath));
		if (e == eLmplayer)
		{
			PathRemoveFileSpecW(wsExePath);
			PathRemoveFileSpecW(wsExePath);
		}
		else if (e == eLauncher || e == eService)
		{
			PathRemoveFileSpecW(wsExePath);
		}
		PathAppendW(wsExePath, L"\\LMPlayer.exe");

		BOOL bRet = FALSE;
		WCHAR szStartPath[MAX_PATH] = { 0 };
		SHGetSpecialFolderPath(NULL, szStartPath, CSIDL_PROGRAMS, 0);
		WCHAR wPath[MAX_PATH] = { 0 };
		wsprintf(wPath, L"%s\\%s\\%s.lnk", szStartPath, L"LMPlayer", L"LMPlayer");

		WCHAR wPathDir[MAX_PATH] = { 0 };
		wsprintf(wPathDir, L"%s\\%s", szStartPath, L"LMPlayer");
		CreateDirectory(wPathDir, NULL);
		bRet = CreateShortCut(wPath, wsExePath, L"-startMenu.lnk");
		return bRet;
	}
	BOOL CreatRunTaskbarlnk()
	{
		BOOL bRet = FALSE;
		WCHAR wsExePath[MAX_PATH] = { 0 };
		GetModuleFileNameW(NULL, wsExePath, sizeof(wsExePath));
		PathRemoveFileSpecW(wsExePath);
		PathAppendW(wsExePath, L"\\LMPlayer.exe");

		WCHAR szStartPath[MAX_PATH] = { 0 };
		GetModuleFileNameW(NULL, szStartPath, sizeof(wsExePath));
		PathRemoveFileSpecW(szStartPath);
		WCHAR wPath[MAX_PATH] = { 0 };
		wsprintf(wPath, L"%s\\%s.lnk", szStartPath, L"LMPlayer");
		bRet = CreateShortCut(wPath, wsExePath, L"-taskbar.lnk");
		return bRet;

	}
	BOOL bExistTaskBar()
	{
		BOOL bres = false;
		CHAR szBuffer[MAX_PATH] = { 0 };
		SHGetSpecialFolderPathA(NULL, szBuffer, CSIDL_APPDATA, FALSE);
		string tmp = szBuffer;
		tmp += "\\Microsoft\\Internet Explorer\\Quick Launch\\User Pinned\\TaskBar\\LMPlayer.lnk";
		if (PathFileExistsA(tmp.c_str()))
		{
			bres = true;
		}
		return bres;
	}
	void    TaskbarPinWin10(bool bPin, LPCTSTR lpszFilePath, LPCTSTR lpszFileName)
	{
		WCHAR kPinStrEn[] = L"Pin to Taskbar";
		WCHAR kPinStrCh[] = L"锁定到任务栏";

		WCHAR kUnPinStrCh[] = L"从任务栏取消固定";
		WCHAR kUnPinStrEn[] = L"Pin to Taskbar";
		do
		{
			TCHAR  szVal[MAX_PATH] = { 0 };
			ULONG  uValSize = MAX_PATH;
			CRegKey  regKey;

			if (ERROR_SUCCESS != regKey.Open(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\CommandStore\\shell\\Windows.taskbarpin"), KEY_READ))
			{
				break;
			}
			if (ERROR_SUCCESS != regKey.QueryStringValue(_T("ExplorerCommandHandler"), szVal, &uValSize))
			{
				break;
			}
			regKey.Close();
			if (ERROR_SUCCESS != regKey.Create(HKEY_CURRENT_USER, _T("SOFTWARE\\Classes\\*\\shell\\{:}")))
			{
				break;
			}
			if (ERROR_SUCCESS != regKey.SetStringValue(_T("ExplorerCommandHandler"), szVal))
			{
				break;
			}
			regKey.Close();

			IShellDispatch* pShellDisp = NULL;
			Folder *pFolder;
			FolderItem *pFolderItem;
			FolderItemVerbs *pFolderVers;
			CComBSTR    stitle, str;
			HRESULT hr = S_OK;
			CoInitialize(NULL);
			hr = ::CoCreateInstance(CLSID_Shell, NULL,
				CLSCTX_SERVER, IID_IShellDispatch, (LPVOID*)&pShellDisp);
			if (SUCCEEDED(hr))
			{
				hr = pShellDisp->NameSpace(CComVariant(lpszFilePath), &pFolder);
				hr = pFolder->ParseName(CComBSTR(lpszFileName), &pFolderItem);
				pFolderItem->Verbs(&pFolderVers);
				long count = 0;
				pFolderVers->get_Count(&count);
				for (long i = 0; i < count; ++i)
				{
					HRESULT rv;
					FolderItemVerb* item_verb = NULL;
					rv = pFolderVers->Item(_variant_t(i), &item_verb);
					if (SUCCEEDED(rv) && item_verb)
					{
						CComBSTR name;
						item_verb->get_Name(&name);
						//cout << *name << endl;
						if (bPin)
						{
							if (!bExistTaskBar())
							{
								pFolderItem->InvokeVerb(CComVariant(_T("{:}")));
							}
							break;
						}
						else
						{
							if (_wcsnicmp(name, kUnPinStrCh, wcslen(kUnPinStrCh)) == 0
								|| _wcsnicmp(name, kUnPinStrEn, wcslen(kUnPinStrEn)) == 0)
							{
								item_verb->DoIt();
								//ret = true;
								break;
							}

						}

					}
				}
				hr = pShellDisp->Release();
				pShellDisp = NULL;
			}

			if (ERROR_SUCCESS != regKey.Open(HKEY_CURRENT_USER, _T("SOFTWARE\\Classes\\*\\shell"), KEY_READ | KEY_WRITE))
			{
				break;
			}
			regKey.RecurseDeleteKey(_T("{:}"));
			regKey.Close();
			CoUninitialize();

		} while (FALSE);
	}

	BOOL Taskbarpin()
	{
		OSVERSIONINFO osvi;
		ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&osvi);
		WCHAR wsExePath[MAX_PATH] = { 0 };
		WCHAR path[MAX_PATH] = { 0 };
		WCHAR plnk[MAX_PATH] = { 0 };
		GetModuleFileNameW(NULL, wsExePath, sizeof(wsExePath));
		PathRemoveFileSpecW(wsExePath);
		PathAppendW(plnk, wsExePath);
		PathAppendW(plnk, L"\\LMPlayer.lnk");
		PathAppendW(wsExePath, L"\\LMPlayer.exe");
		CreateShortCut(plnk, wsExePath,L" -taskbar.lnk");
		if (GetSystemVer() >= eMicrosoftWindows8)// win8以上系统才做此操作，win7系统以后解决
		{
			// 获取模块所在路径
			WCHAR wsExePath[MAX_PATH] = { 0 };
			WCHAR path[MAX_PATH] = { 0 };
			GetModuleFileNameW(NULL, wsExePath, sizeof(wsExePath));
			PathRemoveFileSpecW(wsExePath);
			PathAppendW(path, wsExePath);
			TaskbarPinWin10(TRUE, path, L"LMPlayer.lnk");
			return TRUE;
		}
		else
		{
			ShellExecute(NULL, _T("taskbarpin"), plnk, NULL, NULL, 0);
		}
		return FALSE;
	}
	bool InstallYoukuDrm()
	{
		// 判断优酷DRM保镖注册表项
		LONG lret = ERROR_SUCCESS;
		char pvData[256] = { 0 };
		DWORD dwDataSize = sizeof(pvData);
		lret = RegGetValueA(HKEY_LOCAL_MACHINE, "Software\\AlibabaProtect", "ProgramPath", RRF_RT_REG_SZ, NULL, pvData, &dwDataSize);
		if (lret == ERROR_SUCCESS)
		{
			//成功
			return true;
		}
		return ExcutePEEX();
	}

	bool ExcutePEEX()
	{
		DWORD exitCode = 0;

		// 安装优酷DRM保镖
		WCHAR wsExePath[MAX_PATH] = { 0 };
		GetModuleFileNameW(NULL, wsExePath, sizeof(wsExePath));
		PathRemoveFileSpecW(wsExePath);
		PathAppendW(wsExePath, L"\\YouKu\\install_helper.exe");
		SHELLEXECUTEINFO execinfo;
		memset(&execinfo, 0, sizeof(execinfo));
		execinfo.lpFile = wsExePath;
		execinfo.cbSize = sizeof(execinfo);

		OSVERSIONINFO osvi;
		ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&osvi);
		if (osvi.dwMajorVersion > 5)
		{
			execinfo.lpVerb = _T("runas");
		}
		execinfo.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_NO_UI;
		execinfo.nShow = SW_HIDE;
		execinfo.lpParameters = L"--repair_uac";
		if (ShellExecuteEx(&execinfo))
			return true;
		else
			return false;
	}

	eOSVer GetSystemVer()
	{
		SYSTEM_INFO info;        //用SYSTEM_INFO结构判断64位AMD处理器 
		GetSystemInfo(&info);    //调用GetSystemInfo函数填充结构 
		OSVERSIONINFOEX os;
		os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		eOSVer eOv = eUnknownOperatingSystem;
		if (GetVersionEx((OSVERSIONINFO *)&os))
		{
			//下面根据版本信息判断操作系统名称 
			switch (os.dwMajorVersion)//判断主版本号
			{
			case 4:
				switch (os.dwMinorVersion)//判断次版本号 
				{
				case 0:
					if (os.dwPlatformId == VER_PLATFORM_WIN32_NT)
						eOv = eMicrosoftWindowsNT40;
					else if (os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
						eOv = eMicrosoftWindows95;
					break;
				case 10:
					eOv = eMicrosoftWindows98;
					break;
				case 90:
					eOv = eMicrosoftWindowsMe;
					break;
				}
				break;

			case 5:
				switch (os.dwMinorVersion)	//再比较dwMinorVersion的值
				{
				case 0:
					eOv = eMicrosoftWindows2000;//1999年12月发布
					break;

				case 1:
					eOv = eMicrosoftWindowsXP;//2001年8月发布
					break;

				case 2:
					if (os.wProductType == VER_NT_WORKSTATION
						&& info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
					{
						eOv = eMicrosoftWindowsXPProfessionalx64Edition;
					}
					else if (GetSystemMetrics(SM_SERVERR2) == 0)
						eOv = eMicrosoftWindowsServer2003;//2003年3月发布 
					else if (GetSystemMetrics(SM_SERVERR2) != 0)
						eOv = eMicrosoftWindowsServer2003R2;
					break;
				}
				break;

			case 6:
				switch (os.dwMinorVersion)
				{
				case 0:
					if (os.wProductType == VER_NT_WORKSTATION)
						eOv = eMicrosoftWindowsVista;
					else
						eOv = eMicrosoftWindowsServer2008;//服务器版本 
					break;
				case 1:
					if (os.wProductType == VER_NT_WORKSTATION)
						eOv = eMicrosoftWindows7;
					else
						eOv = eMicrosoftWindowsServer2008R2;
					break;
				case 2:
					if (os.wProductType == VER_NT_WORKSTATION)
						eOv = eMicrosoftWindows8;
					else
						eOv = eMicrosoftWindowsServer2012;
					break;
				case 3:
					if (os.wProductType == VER_NT_WORKSTATION)
						eOv = eMicrosoftWindows81;
					else
						eOv = eMicrosoftWindowsServer2012R2;
					break;
				}
				break;

			case 10:
				switch (os.dwMinorVersion)
				{
				case 0:
					if (os.wProductType == VER_NT_WORKSTATION)
						eOv = eMicrosoftWindows10;
					else
						eOv = eMicrosoftWindowsServer2016TechnicalPreview;//服务器版本 
					break;
				}
				break;
			}
		}

		return eOv;
	}

	void WriteLePlayerProtocol()
	{
		FLOG(_T("WriteLePlayerProtocol begin"));
		// 程序路径
		CHAR chPath_Run[MAX_PATH] = { 0 };
		::GetModuleFileNameA(NULL, chPath_Run, MAX_PATH);
		PathRemoveFileSpecA(chPath_Run);
		PathAppendA(chPath_Run, "\\LMPlayer.exe");

		HKEY hKey = NULL;
		REGSAM samDesiredOpen = KEY_ALL_ACCESS;
		// 判断系统是32位还是64位，调用不同的注册表函数
		/*
		;生成协议
		WriteRegStr HKCR "LePlayer" "" "LePlayer Protocol"
		WriteRegStr HKCR "LePlayer" "URL Protocol" ""
		WriteRegStr HKCR "LePlayer\DefaultIcon" "" "$vInitPath\${EXE_NAME},1"
		WriteRegStr HKCR "LePlayer\shell" "" ""
		WriteRegStr HKCR "LePlayer\shell\open" "" ""
		WriteRegStr HKCR "LePlayer\shell\open\command" "" "$vInitPath\${EXE_NAME} %1"
		*/
		LSTATUS lRes = RegCreateKeyExA(HKEY_CLASSES_ROOT, "LMPlayer", 0, "", 0, samDesiredOpen, NULL, &hKey, 0);
		if (lRes != ERROR_SUCCESS)
			FLOG(_T("SetupRunPath create failed"));
		if (RegSetValueExA(hKey, "", 0, REG_SZ, (BYTE*)"LMPlayer", strlen(chPath_Run)) != ERROR_SUCCESS)
			FLOG(_T("SetupRunPath set failed"));
		if (RegSetValueExA(hKey, "URL Protocol", 0, REG_SZ, (BYTE*)"LMPlayer", strlen(chPath_Run)) != ERROR_SUCCESS)
			FLOG(_T("SetupRunPath set failed"));

		hKey = NULL;
		lRes = RegCreateKeyExA(HKEY_CLASSES_ROOT, "LMPlayer\\DefaultIcon", 0, "", 0, samDesiredOpen, NULL, &hKey, 0);
		if (lRes != ERROR_SUCCESS)
			FLOG(_T("SetupRunPath create failed"));
		CHAR chValue[MAX_PATH] = { 0 };
		sprintf_s(chValue, "%s,1", chPath_Run);
		if (RegSetValueExA(hKey, "", 0, REG_SZ, (BYTE*)chValue, strlen(chValue)) != ERROR_SUCCESS)
			FLOG(_T("SetupRunPath set failed"));

		hKey = NULL;
		lRes = RegCreateKeyExA(HKEY_CLASSES_ROOT, "LMPlayer\\shell", 0, "", 0, samDesiredOpen, NULL, &hKey, 0);
		if (lRes != ERROR_SUCCESS)
			FLOG(_T("SetupRunPath create failed"));
		if (RegSetValueExA(hKey, "", 0, REG_SZ, NULL, NULL) != ERROR_SUCCESS)
			FLOG(_T("SetupRunPath set failed"));

		hKey = NULL;
		lRes = RegCreateKeyExA(HKEY_CLASSES_ROOT, "LMPlayer\\shell\\open", 0, "", 0, samDesiredOpen, NULL, &hKey, 0);
		if (lRes != ERROR_SUCCESS)
			FLOG(_T("SetupRunPath create failed"));

		if (RegSetValueExA(hKey, "", 0, REG_SZ, NULL, NULL) != ERROR_SUCCESS)
			FLOG(_T("SetupRunPath set failed"));

		hKey = NULL;
		lRes = RegCreateKeyExA(HKEY_CLASSES_ROOT, "LMPlayer\\shell\\open\\command", 0, "", 0, samDesiredOpen, NULL, &hKey, 0);
		if (lRes != ERROR_SUCCESS)
			FLOG(_T("SetupRunPath create failed"));

		sprintf_s(chValue, "%s %c1", chPath_Run, '%');
		if (RegSetValueExA(hKey, "", 0, REG_SZ, (BYTE*)chValue, strlen(chValue)) != ERROR_SUCCESS)
			FLOG(_T("SetupRunPath set failed"));

		if (hKey)
			RegCloseKey(hKey);

		FLOG(_T("SetupRunPath end"));
		return;

	}

	void GetAdPopHistoryCfg(string &str)
	{
		CHAR szBuffer[MAX_PATH];
		SHGetSpecialFolderPathA(NULL, szBuffer, CSIDL_APPDATA, FALSE);
		string tmp = szBuffer;
		tmp += "\\Lenovo\\lebo";
		if (!PathFileExistsA(tmp.c_str()))
		{
			CreateDirectoryA(tmp.c_str(), NULL);
		}

		str = szBuffer;
		str += "\\Lenovo\\lebo\\Ad.cfg";
		return;
	}

	bool IsOtherFullscreen()
	{
		bool bFullScreen = false;
		HWND hWnd = GetForegroundWindow();
		RECT rcApp, rcDesk;
		GetWindowRect(GetDesktopWindow(), &rcDesk);
		if (hWnd != GetDesktopWindow() && hWnd != GetShellWindow())
		{
			GetWindowRect(hWnd, &rcApp);
			if (rcApp.left <= rcDesk.left && rcApp.top <= rcDesk.top && rcApp.right >= rcDesk.right && rcApp.bottom >= rcDesk.bottom)
				bFullScreen = true;
		}
		return bFullScreen;
	}

	void FA()
	{
		// 因为调用者不同，所以先找lelaunch.exe
		char buff_A[MAX_PATH] = {0};
		GetModuleFileNameA(NULL, buff_A, sizeof(buff_A));		
		PathRemoveFileSpecA(buff_A);
		PathAppendA(buff_A, "\\LMPlayer.exe");
		if (false == PathFileExistsA(buff_A))
		{
			// 如果找不到，就再向上一层文件夹找
			memset(buff_A, 0, MAX_PATH);
			GetModuleFileNameA(NULL, buff_A, sizeof(buff_A));
			PathRemoveFileSpecA(buff_A);
			PathRemoveFileSpecA(buff_A);
			PathAppendA(buff_A, "\\LMPlayer.exe");
		}

		if (PathFileExistsA(buff_A))
		{
			for (int i = 0; i < EXT_NUM; i++)
			{
				char* sExt = chArray_Ext[i];
				char cAppKey[128] = { 0 };
				sprintf_s(cAppKey, "LMPlayer%s", sExt);
				RegExt::RegisterExt(sExt, cAppKey, buff_A, 0);
			}
			SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
		}
		return;
	}

	void UnFA()
	{
		for (int i = 0; i < EXT_NUM; i++)
		{
			char* sExt = chArray_Ext[i];
			char cAppKey[128] = { 0 };
			char cLePlayerKey[128] = {0};
			sprintf_s(cAppKey, "LMPlayer%s", sExt);
			sprintf_s(cLePlayerKey, "leplayer%s", sExt);
			RegExt::UnRegisterExt(sExt, cAppKey);
			RegExt::UnRegisterExt(sExt, cLePlayerKey);
			RegExt::UnRegisterExtError();
		}
		SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);

		return;
	}

	std::wstring getSystemName()
	{
		SYSTEM_INFO info;                //用SYSTEM_INFO结构判断64位AMD处理器
		GetSystemInfo(&info);            //调用GetSystemInfo函数填充结构
		OSVERSIONINFOEX os;
		os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
#pragma warning(disable:4996)
		if (GetVersionEx((OSVERSIONINFO *)&os))
		{
			wstring vname;

			//下面根据版本信息判断操作系统名称
			switch (os.dwMajorVersion) {                        //判断主版本号
			case 4:
				switch (os.dwMinorVersion) {                //判断次版本号
				case 0:
					if (os.dwPlatformId == VER_PLATFORM_WIN32_NT)
						vname = L"Microsoft Windows NT 4.0";  //1996年7月发布
					else if (os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
						vname = L"Microsoft Windows 95";
					break;
				case 10:
					vname = L"Microsoft Windows 98";
					break;
				case 90:
					vname = L"Microsoft Windows Me";
					break;
				}
				break;
			case 5:
				switch (os.dwMinorVersion) {               //再比较dwMinorVersion的值
				case 0:
					vname = L"Microsoft Windows 2000";    //1999年12月发布
					break;
				case 1:
					vname = L"Microsoft Windows XP";      //2001年8月发布
					break;
				case 2:
					if (os.wProductType == VER_NT_WORKSTATION &&
						info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
						vname = L"Microsoft Windows XP Professional x64 Edition";
					else if (GetSystemMetrics(SM_SERVERR2) == 0)
						vname = L"Microsoft Windows Server 2003";   //2003年3月发布
					else if (GetSystemMetrics(SM_SERVERR2) != 0)
						vname = L"Microsoft Windows Server 2003 R2";
					break;
				}
				break;
			case 6:
				switch (os.dwMinorVersion) {
				case 0:
					if (os.wProductType == VER_NT_WORKSTATION)
						vname = L"Microsoft Windows Vista";
					else
						vname = L"Microsoft Windows Server 2008";   //服务器版本
					break;
				case 1:
					if (os.wProductType == VER_NT_WORKSTATION)
						vname = L"Microsoft Windows 7";
					else
						vname = L"Microsoft Windows Server 2008 R2";
					break;
				case 2:
					vname = L"Microsoft Windows 7";
					break;
				}
				break;
			default:
				vname = L"未知操作系统";
			}
			return vname;
		}
		else
			return L"";
	}

}
