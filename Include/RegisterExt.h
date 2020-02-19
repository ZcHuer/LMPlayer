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
}

CString getSystemName()
{
	SYSTEM_INFO info;                //用SYSTEM_INFO结构判断64位AMD处理器
	GetSystemInfo(&info);            //调用GetSystemInfo函数填充结构
	OSVERSIONINFOEX os;
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
#pragma warning(disable:4996)
	if (GetVersionEx((OSVERSIONINFO *)&os))
	{
		CString vname;

		//下面根据版本信息判断操作系统名称
		switch (os.dwMajorVersion) {                        //判断主版本号
		case 4:
			switch (os.dwMinorVersion) {                //判断次版本号
			case 0:
				if (os.dwPlatformId == VER_PLATFORM_WIN32_NT)
					vname = _T("Microsoft Windows NT 4.0");  //1996年7月发布
				else if (os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
					vname = _T("Microsoft Windows 95");
				break;
			case 10:
				vname = _T("Microsoft Windows 98");
				break;
			case 90:
				vname = _T("Microsoft Windows Me");
				break;
			}
			break;
		case 5:
			switch (os.dwMinorVersion) {               //再比较dwMinorVersion的值
			case 0:
				vname = _T("Microsoft Windows 2000");    //1999年12月发布
				break;
			case 1:
				vname = _T("Microsoft Windows XP");      //2001年8月发布
				break;
			case 2:
				if (os.wProductType == VER_NT_WORKSTATION &&
					info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
					vname = _T("Microsoft Windows XP Professional x64 Edition");
				else if (GetSystemMetrics(SM_SERVERR2) == 0)
					vname = _T("Microsoft Windows Server 2003");   //2003年3月发布
				else if (GetSystemMetrics(SM_SERVERR2) != 0)
					vname = _T("Microsoft Windows Server 2003 R2");
				break;
			}
			break;
		case 6:
			switch (os.dwMinorVersion) {
			case 0:
				if (os.wProductType == VER_NT_WORKSTATION)
					vname = _T("Microsoft Windows Vista");
				else
					vname = _T("Microsoft Windows Server 2008");   //服务器版本
				break;
			case 1:
				if (os.wProductType == VER_NT_WORKSTATION)
					vname = _T("Microsoft Windows 7");
				else
					vname = _T("Microsoft Windows Server 2008 R2");
				break;
			case 2:
				vname = _T("Microsoft Windows 7");
				break;
			}
			break;
		default:
			vname = _T("未知操作系统");
		}
		return vname;
	}
	else
		return _T("");
}
