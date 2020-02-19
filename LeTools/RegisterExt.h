#pragma once
#include <Windows.h>
#include <string>
#include <ShlObj.h>

#define EXT_NUM 102

char* chArray_Ext[] =
{
	".asf",		".avi",		".wmv",		".wmp",		".dvrms",	".rmvb",	".ram",		".rm",		".rpm",		".scm",
	".rt",		".smil",	".mov",		".qt",		".dat",		".m1v",		".m2v",		".m2p",		".m2ts",	".gsm"
	".mp2v",	".mpe",		".mpeg",	".mpeg1",	".mpeg2",	".mpg",		".mpv2",	".pss",		".pva",		".tp",
	".trp",		".ts",		".mp4",		".m4b",		".m4r",		".m4v",		".mpeg4",	".3g2",		".3gp",		".3gp2",
	".3gpp",	".flv",		".f4v",		".swf",		".vob",		".ifo",		".amv",		".csf",		".divx",	".evo",
	".mkv",		".mod",		".pmp",		".vp6",		".bik",		".mts",		".xvx",		".xv",		".xlmv",	".ogm",
	".ogv",		".ogx",		".m2t",		".h264",	".k3g",		".nsv",		".pmf",		".tod",		".vp7",		".webm",
	".wtv",		".aac",		".ac3",		".acc",		".aiff",	".amr",		".ape",		".au",		".cda",		".dts",
	".flac",	".m4a",		".mka",		".mp2",		".mp3",		".mpc",		".ra",		".tta",		".wav",		".wma",
	".wv",		".mid",		".oga",		".rmi",		".aif",		".fli",		".flc",		".arm",		".aifc",	".caf",
	".opus",	".spx"
};

namespace RegExt
{
    //检测文件关联情况
    //strExt: 要检测的扩展名(例如: ".txt")
    //strAppKey: ExeName扩展名在注册表中的键值(例如: "txtfile")
    //返回TRUE: 表示已关联，FALSE: 表示未关联
	void DelReg(char* sExt, char*sAppKey);
	void DelRegister(char* sExt);

    BOOL CheckFileRelation(const char *strExt, const char *strAppKey)
    {
        int nRet = FALSE;
        HKEY hExtKey;
        char szPath[_MAX_PATH];
        DWORD dwSize = sizeof(szPath);
        if (RegOpenKeyA(HKEY_CLASSES_ROOT, strExt, &hExtKey) == ERROR_SUCCESS)
        {
            RegQueryValueExA(hExtKey, NULL, NULL, NULL, (LPBYTE)szPath, &dwSize);
            if (_stricmp(szPath, strAppKey) == 0) {
                nRet = TRUE;
            }
            RegCloseKey(hExtKey);
            return nRet;
        }
        return nRet;
    }

    BOOL DelKeyValues(HKEY hKey,const char* lpSubKey)
    {
        HKEY hSubKey = NULL;
        RegOpenKeyA(hKey, lpSubKey,&hSubKey);
        if (!hSubKey) return FALSE;

        DWORD dwValues = 0;
        RegQueryInfoKeyA(hSubKey,nullptr,nullptr, nullptr, nullptr, nullptr, nullptr,&dwValues, nullptr, nullptr, nullptr, nullptr);
        
        DWORD dwType;
        for (size_t i = 0; i < dwValues; i++)
        {
            char cName[128] = { 0 };
            DWORD dwLength = 128;
            LONG lResult = RegEnumValueA(hSubKey, 0,cName, &dwLength, NULL, &dwType, NULL, NULL);
            if (lResult != ERROR_SUCCESS)
                return FALSE;
            RegDeleteValueA(hSubKey, cName);
        }
        RegCloseKey(hSubKey);
        return TRUE;
    }

    BOOL DelKey(HKEY hKey, const char* lpSubKey,const char* lpName)
    {
        HKEY hSubKey = NULL;
        char cSubKey[1024] = { 0 };
        sprintf_s(cSubKey, "%s\\%s", lpSubKey, lpName);
        RegOpenKeyA(hKey, cSubKey, &hSubKey);
        if (!hSubKey) return FALSE;

        DWORD dwKeyCount = 0;
        RegQueryInfoKeyA(hSubKey, nullptr, nullptr, nullptr, &dwKeyCount, nullptr, nullptr,
            nullptr, nullptr, nullptr, nullptr, nullptr);
        for (size_t i = 0; i < dwKeyCount; i++)
        {
            char cName[MAX_PATH] = { 0 };
            DWORD dwLength = MAX_PATH;
            LONG lResult = RegEnumKeyA(hSubKey,0, cName,dwLength);
            if (lResult != ERROR_SUCCESS)
                return FALSE;
            DelKey(hKey, cSubKey, cName);
        }
        RegDeleteKeyA(hKey, cSubKey);
        RegCloseKey(hSubKey);
        return TRUE;
    }

    BOOL SetKeyValue(HKEY hKey,const char* lpSubKey,const char* lpKey,DWORD dwType,const char* lpValue,DWORD dwLen)
    {
        HKEY hSubKey = NULL;
        RegOpenKeyA(hKey, lpSubKey,&hSubKey);
        if (!hSubKey) return FALSE;

        if (RegSetValueExA(hSubKey, lpKey,0, dwType, (BYTE*)lpValue, dwLen) != ERROR_SUCCESS) return FALSE;
       
        return TRUE;
    }

    void RegisterExtWin7(char *sExt, char *sAppKey, char *sAppName)
    {
        char cKey[_MAX_PATH] = { 0 };
        HKEY hKey;
        RegCreateKeyA(HKEY_CLASSES_ROOT, sExt, &hKey);
        RegSetValueA(hKey, "", REG_SZ, sAppName, strlen(sAppName) + 1);
        RegCloseKey(hKey);

        sprintf_s(cKey, "%s\\Shell\\Open\\Command", sAppKey);
        RegCreateKeyA(HKEY_CLASSES_ROOT, cKey, &hKey);
        RegSetValueA(hKey, "", REG_SZ, sAppName, strlen(sAppName) + 1);
        RegCloseKey(hKey);

        sprintf_s(cKey, "%s\\Shell", sAppKey);
        RegCreateKeyA(HKEY_CLASSES_ROOT, cKey, &hKey);
        RegSetValueA(hKey, "", REG_SZ, "Open", strlen("Open") + 1);
        RegCloseKey(hKey);

        sprintf_s(cKey, "%s\\Shell\\Open\\Command", sAppKey);
        RegCreateKeyA(HKEY_CLASSES_ROOT, cKey, &hKey);
        sprintf_s(cKey, "\"%s\" \"%%1\"", sAppName);
        RegSetValueA(hKey, "", REG_SZ, cKey, strlen(cKey) + 1);
        RegCloseKey(hKey);
    }

    //---------------------------------------------------------------------------
    //注册文件关联
    //strExe:要检测的扩展名(例如:".txt")
    //strAppName:要关联的应用程序名(例如:"C:\MyApp\MyApp.exe")
    //strAppKey:ExeName扩展名在注册表中的键值(例如:"txtfile")
    //strDefaultIcon:扩展名为strAppName的图标文件(例如:"C:\MyApp\MyApp.exe,0")
    //strDescribe:文件类型描述  
    void RegisterFileRelation(char *strExt, char *strAppName, char *strAppKey, char *strDefaultIcon, char *strDescribe)
    {
        char strTemp[_MAX_PATH];
        HKEY hKey;
        LSTATUS lst;
        lst = RegCreateKeyA(HKEY_CLASSES_ROOT, strExt, &hKey);
        lst = RegSetValueA(hKey, "", REG_SZ, strAppKey, strlen(strAppKey) + 1);
        RegCloseKey(hKey);

        lst = RegCreateKeyA(HKEY_CLASSES_ROOT, strAppKey, &hKey);
        lst = RegSetValueA(hKey, "", REG_SZ, strDescribe, strlen(strDescribe) + 1);
        RegCloseKey(hKey);

        sprintf_s(strTemp, "%s\\DefaultIcon", strAppKey);
        lst = RegCreateKeyA(HKEY_CLASSES_ROOT, strTemp, &hKey);
        lst = RegSetValueA(hKey, "", REG_SZ, strDefaultIcon, strlen(strDefaultIcon) + 1);
        RegCloseKey(hKey);

        sprintf_s(strTemp, "%s\\Shell", strAppKey);
        lst = RegCreateKeyA(HKEY_CLASSES_ROOT, strTemp, &hKey);
        lst = RegSetValueA(hKey, "", REG_SZ, "Open", strlen("Open") + 1);
        RegCloseKey(hKey);

        sprintf_s(strTemp, "%s\\Shell\\Open\\Command", strAppKey);
        lst = RegCreateKeyA(HKEY_CLASSES_ROOT, strTemp, &hKey);
        sprintf_s(strTemp, "%s \"%%1\"", strAppName);
        lst = RegSetValueA(hKey, "", REG_SZ, strTemp, strlen(strTemp) + 1);
        RegCloseKey(hKey);
    }

	//---------------------------------------------------------------------------
	//注册文件关联
	//strExe:要检测的扩展名(例如:".txt")
	//strAppName:要关联的应用程序名(例如:"C:\MyApp\MyApp.exe")
	//strAppKey:ExeName扩展名在注册表中的键值(例如:"txtfile")
	//strDefaultIcon:扩展名为strAppName的图标文件(例如:"C:\MyApp\MyApp.exe,0")
	//strDescribe:文件类型描述  
	void UnRegisterFileRelation(char *strExt, char *strAppKey)
	{
		RegExt::DelRegister(strExt);
		RegExt::DelReg(strExt, strAppKey);
		RegDeleteKeyA(HKEY_CLASSES_ROOT, strAppKey);
	}

    void RegisterExtWin10(char* sExt, char* sAppKey)
    {
        char cTemp[512] = { 0 };
        memset(cTemp, 0, 512);

        sprintf_s(cTemp, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\%s\\", sExt);
        DelKey(HKEY_CURRENT_USER, cTemp, "UserChoice");

        memset(cTemp, 0, 512);
        sprintf_s(cTemp, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\%s\\OpenWithList", sExt);
        LSTATUS lst;
        lst = SetKeyValue(HKEY_CURRENT_USER, cTemp, "a", REG_SZ, sAppKey, strlen(sAppKey) + 1);
        lst = SetKeyValue(HKEY_CURRENT_USER, cTemp, "MRUList", REG_SZ, "a", 2);

        memset(cTemp, 0, 512);
        sprintf_s(cTemp, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\%s\\OpenWithProgids", sExt);
        lst = SetKeyValue(HKEY_CURRENT_USER, cTemp, sAppKey, REG_NONE, 0, 0);

    }

	
    void DelReg(char* sExt, char*sAppKey)
    {
        HKEY hExtKey;
        char szPath[_MAX_PATH];
        DWORD dwSize = sizeof(szPath);
        if (RegOpenKeyA(HKEY_CLASSES_ROOT, sExt, &hExtKey) == ERROR_SUCCESS)
        {
            RegQueryValueExA(hExtKey, NULL, NULL, NULL, (LPBYTE)szPath, &dwSize);
            if (_stricmp(szPath, sAppKey) == 0)
            {
                char cName[_MAX_PATH] = { 0 };
                dwSize = _MAX_PATH;
                DWORD dwType;
                RegEnumValueA(hExtKey, 0, cName, &dwSize, NULL, &dwType, NULL, NULL);
                RegDeleteValueA(hExtKey, cName);
            }
            RegCloseKey(hExtKey);
        }
    }
	

    void DelRegister(char* sExt)
    {
        char cKey[512] = { 0 };
        sprintf_s(cKey, "%s\\OpenWithProgids", sExt);
        DelKeyValues(HKEY_CLASSES_ROOT, cKey);

        sprintf_s(cKey, "SOFTWARE\\Classes\\%s\\OpenWithProgids", sExt);
        DelKeyValues(HKEY_CURRENT_USER, cKey);

        sprintf_s(cKey, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\%s\\OpenWithList\\", sExt);
        DelKeyValues(HKEY_CURRENT_USER, cKey);

        sprintf_s(cKey, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\%s\\OpenWithProgids\\", sExt);
        DelKeyValues(HKEY_CURRENT_USER, cKey);

        sprintf_s(cKey, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\%s\\", sExt);
        DelKey(HKEY_CURRENT_USER, cKey,"UserChoice");
    }

    void DelDefalut()
    {
        HKEY hKey = NULL;
        RegOpenKeyA(HKEY_CURRENT_USER, "SOFTWARE\\Classes\\AppX6eg8h5sxqq90pv53845wmnbewywdqq5h", &hKey);
        if (hKey)
		{
            RegCloseKey(hKey);
            DelKey(HKEY_CURRENT_USER, "SOFTWARE\\Classes","AppX6eg8h5sxqq90pv53845wmnbewywdqq5h");
        }
    }

    // 是否是win8前的系统
    BOOL IsSystemAfterWin8()
    {
		OSVERSIONINFOEX os;
		os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
#pragma warning(disable:4996)
		if (GetVersionEx((OSVERSIONINFO *)&os))
        {
            if (os.dwMajorVersion>6 || (os.dwMajorVersion == 6 && os.dwMinorVersion>1)) {
                return TRUE;
            }
        }
        return FALSE;
    }

    void RegisterExt(char *sExt, char *sAppKey, char *sAppName, BOOL bBeforeWin8/*=TRUE*/)
    {
        DelRegister(sExt);
        RegisterFileRelation(sExt, sAppName, sAppKey, sAppName,"");
        RegisterExtWin10(sExt, sAppKey);
    }
	void UnRegisterExt(char *sExt, char *sAppKey)
	{
		DelRegister(sExt);
		UnRegisterFileRelation(sExt, sAppKey);
	}
	void UnRegisterExtError()
	{
		HKEY hExtKey;
		char szPath[_MAX_PATH];
		DWORD dwSize = sizeof(szPath);
		if (RegOpenKeyA(HKEY_CLASSES_ROOT, NULL, &hExtKey) == ERROR_SUCCESS)
		{
			FILETIME ftLastWriteTime;
			int retCode = 0;
			int i = 0;
			char  tcKeyName[128];    //用于保存每个子键的名字的内存空间
			DWORD dwKeyNameSize = 128;
			do 
			{
				retCode = RegEnumKeyExA(hExtKey, i, tcKeyName, &dwKeyNameSize,
					NULL, NULL, NULL, &ftLastWriteTime);
				if (retCode == ERROR_NO_MORE_ITEMS)
				{   //如果获取结束则跳出循环
					break;
				}
				string tmp = tcKeyName;
				if (tmp.find("LMPlayer")!=-1)
				{
					RegDeleteKeyA(HKEY_CLASSES_ROOT, tcKeyName);
				}
				i++;
			} while (1);
			RegCloseKey(hExtKey);
		}
		
	}

}