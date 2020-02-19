#pragma once

#include <windows.h>
#include <tchar.h>
#include <ole2.h>

#include "CrtType.h"

#include "CrtCPU.h"
#include "CrtLZMA.h"
#include "CrtMD5.h"
#include "CrtUnZip.h"
#include "CrtFileVer.h"
#include "CrtZip.h"
#include "HttpType.h"

WORD __cdecl CrtVersion();
void __cdecl CrtInitialize(DWORD dwEvent);
void __cdecl CrtUninitialize(DWORD dwEvent);
STDAPI CrtCreateObject(int iObjectType, void** ppObject);
//////////////////////////////////////////////////////////////////////////
//操作系统判断
//////////////////////////////////////////////////////////////////////////
bool __cdecl IsWindow2000();
bool __cdecl IsWindowXP();
bool __cdecl IsWindowVista();
bool __cdecl IsWindow7();
bool __cdecl IsWindow8();
bool __cdecl IsWindow7Latter();
bool __cdecl IsWindowVistaLatter();
bool __cdecl IsWindow8Latter();
bool __cdecl IsWindow64();
bool __cdecl IsWindowServer();
BOOL __cdecl Is64bitDLL(LPCWSTR lpwsFile);
//////////////////////////////////////////////////////////////////////////
//注册表辅助函数
//////////////////////////////////////////////////////////////////////////
bool __cdecl  CrtRegGetBOOL(HKEY hkey, LPCWSTR  pszSubKey, LPCWSTR  pszValue, bool bDefValue);
bool __cdecl  CrtRegSetBOOL(HKEY hkey, LPCWSTR  pszSubKey, LPCWSTR  pszValue, BOOL bValue);
DWORD __cdecl  CrtRegGetDWORD(HKEY hkey, LPCWSTR  pszSubKey, LPCWSTR  pszValue, DWORD dwDefValue);
bool __cdecl  CrtRegSetDWORD(HKEY hkey, LPCWSTR  pszSubKey, LPCWSTR  pszValue, DWORD dwValue);
//CrtRegGetString : 注意最后一个参数是字节数
bool __cdecl  CrtRegGetString(HKEY hkey, LPCWSTR  pszSubKey, LPCWSTR  pszValue, WCHAR* pString, DWORD dwByteSize);
bool __cdecl  CrtRegSetString(HKEY hkey, LPCWSTR  pszSubKey, LPCWSTR  pszValue, LPCWSTR lpString );
bool __cdecl CrtRegDeleteKeyValue(HKEY hKey, LPCWSTR lpwsSubKey, LPCWSTR lpwsValue);
bool __cdecl CrtRegIsKeyExist( HKEY hRootKey, LPCWSTR  pszSubKey );
//////////////////////////////////////////////////////////////////////////
//用户配置：读/写
//////////////////////////////////////////////////////////////////////////
BOOL __cdecl ConfigGetBOOL(LPCWSTR lpName, BOOL bDefault, CrtConfigType cfgType = ConfigType_UserConfig);
BOOL __cdecl ConfigSetBOOL(LPCWSTR lpName, BOOL bValue, CrtConfigType cfgType = ConfigType_UserConfig);
DWORD __cdecl ConfigGetDWORD(LPCWSTR lpName, DWORD dwDefault, CrtConfigType cfgType = ConfigType_UserConfig);
BOOL __cdecl ConfigSetDWORD(LPCWSTR lpName, DWORD dwValue, CrtConfigType cfgType = ConfigType_UserConfig);
BOOL __cdecl ConfigGetString(LPCWSTR lpName, LPWSTR pValue, long cchMax, CrtConfigType cfgType = ConfigType_UserConfig);
BOOL __cdecl ConfigGetStringEx(LPCWSTR lpName, BSTR* pbstrValue, CrtConfigType cfgType = ConfigType_UserConfig);
BOOL __cdecl ConfigSetString(LPCWSTR lpName, LPCWSTR lpValue, CrtConfigType cfgType = ConfigType_UserConfig);
BOOL __cdecl ConfigGetBinary(LPCWSTR lpName, BYTE* pData, LPDWORD pcbData, CrtConfigType cfgType = ConfigType_UserConfig);
BOOL __cdecl ConfigSetBinary(LPCWSTR lpName, const BYTE * lpValue, DWORD cbData, CrtConfigType cfgType = ConfigType_UserConfig);
BOOL __cdecl ConfigDeleteValue(LPCWSTR lpName, CrtConfigType cfgType = ConfigType_UserConfig);
LPCWSTR __cdecl ConfigGetRegSubkey(CrtConfigType cfgType, HKEY* pRootKey );
//////////////////////////////////////////////////////////////////////////
//全局配置信息: 读/写
//////////////////////////////////////////////////////////////////////////
// GetConfigFilePath 
//		获取指定类型的目录，并加上使用者传进来的文件名lpFileName，组合成一个文件的完整路径
//		此函数，作为暴5整套平台，获取常用目录路径 和 常用文件名的统一入口函数
//		PathType: 目录类型, 见枚举值 PathType_None
//		lpSubDirecotry: 仅当PathType为CurrentPath_Customize时, 才使用lpSubDirecotry
bool __cdecl GetConfigFilePath( LPCWSTR lpFileName , WCHAR* pFile, int cchMax, CrtPathType PathType = PathType_None, bool bCreateDirectory = false, LPCWSTR lpSubDirecotry = NULL );
//GetCurrentFilePath
//		bUseCrtModule: 如果为true, 忽略第一个参数hInstance, 使用crt.dll的hinstance
bool __cdecl GetCurrentFilePath( HINSTANCE hInstance, LPCWSTR lpFileName, WCHAR* pFile, int cchMax, CrtCurrentPath CurPath = CurrentPath_None, LPCWSTR lpSubDirecotry = NULL, bool bUseCrtModule = false );

//Config.ini
bool __cdecl CrtGetGlobalConfig( LPCWSTR lpAppName, LPCWSTR lpKeyName, LPWSTR lpReturnedString, DWORD cchMax, CrtConfigScope scope = ConfigScope_Default );
bool __cdecl CrtSetGlobalConfig(LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpString, CrtConfigScope scope = ConfigScope_Default );
DWORD __cdecl CrtGetGlobalConfigDWORD( LPCWSTR lpAppName, LPCWSTR lpKeyName, DWORD dwDefault, CrtConfigScope scope = ConfigScope_Default  );
bool __cdecl CrtSetGlobalConfigDWORD(LPCWSTR lpAppName, LPCWSTR lpKeyName, DWORD dwValue, CrtConfigScope scope = ConfigScope_Default );

//普通.ini
DWORD WINAPI CrtGetProfileString( LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault,
	LPWSTR lpReturnedString, DWORD nSize, LPCWSTR lpFileName);
BOOL WINAPI CrtWriteProfileString( LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpString, LPCWSTR lpFileName);
BOOL WINAPI CrtWritePrivateProfileSection(  LPCWSTR lpAppName, LPCWSTR lpString, LPCWSTR lpFileName );

bool __cdecl GetProductVersionString( WCHAR* pVerString, int cchMax, bool bUseCacheVer = true);
bool __cdecl GetProductVersion(PCRTVER pVer, bool bUseCacheVer = true );
bool __cdecl GetProductConfigFilePath( WCHAR* pFile, int cchMax);

bool __cdecl GetProductSubVersion(long nSubProduct, PCRTVER pVer);
bool __cdecl GetProductSubVersionString(long nSubProduct, WCHAR* pVerString, int cchMax);

bool __cdecl GetDBVersionString( WCHAR* pVerString, int cchMax, bool bUseCacheVer = true);
bool __cdecl GetDBVersion(PCRTVER pVer, bool bUseCacheVer = true );
bool __cdecl GetDBFilePath( WCHAR* pFile, int cchMax);

bool __cdecl CrtDllGetVersion(LPCWSTR lpDllFile, PCRTVER pVer );
//CrtDllGetVersionString
//		bBaofengFormat : 是否按暴风规范的版本串
bool __cdecl CrtDllGetVersionString(LPCWSTR lpDllFile, WCHAR* pVerString, int cchMax, bool bBaofengFormat = true );
bool __cdecl GetFileVersionString( LPCWSTR lpFile, WCHAR* pVerString, int cchMax);
bool __cdecl GetFileVersionStringEx( LPCWSTR lpFile, LPCWSTR lpFormat, WCHAR* pVerString, int cchMax );

bool __cdecl GetPartnerId(WCHAR* pszId, int cchMax );
bool __cdecl GetProductId(WCHAR* pszId, int cchMax );
bool __cdecl GetUserId(WCHAR* pszId, int cchMax );
bool __cdecl GetADChanelId(WCHAR* pszId, int cchMax );
bool __cdecl GetMachineId(WCHAR* pMID, int cchMax);
bool __cdecl GetMediaTypeString(GUID* pGuid, GUID* pMajorType, WCHAR* pName, int cchMax);

//////////////////////////////////////////////////////////////////////////
//日志操作
//////////////////////////////////////////////////////////////////////////
//到期日期格式：“2010.02.01”，lpExpireDate=NULL表示永久有效
//产品版本格式: “5.01.1126.1111“，lpOnlyProductVer=NULL表示所有版本
HRESULT __cdecl CrtLogPostDailyCount(LPCWSTR lpDaily, long lValue = 1, DWORD lCategory = 0, LPCWSTR lpExpireDate = NULL, LPCWSTR lpOnlyProductVer = NULL );
HRESULT __cdecl CrtLogPostUniqueCount(LPCWSTR lpDaily, long lValue = 1, DWORD lCategory = 0, LPCWSTR lpExpireDate = NULL, LPCWSTR lpOnlyProductVer = NULL);
HRESULT __cdecl CrtLogPostDailyBOOL(LPCWSTR lpDaily, BOOL bValue, DWORD lCategory = 0, LPCWSTR lpExpireDate = NULL, LPCWSTR lpOnlyProductVer = NULL );
//格式：@属性名=值|@属性名=值, ，lpExpireDate为空表示永久有效
HRESULT __cdecl CrtLogPostXMLValue(LPCWSTR lpNode, LPCWSTR lpValue, LPCWSTR lpExpireDate = NULL, LPCWSTR lpOnlyProductVer = NULL );

void __cdecl CrtReportActiveLive(long nType, bool bThread);
typedef void (WINAPI *PFNReportCallback)(BOOL bSucceed, long nType, PVOID pParam);
HRESULT __cdecl CrtReportActiveLiveEx(long nType, bool bThread = false, PFNReportCallback pCallback = NULL, PVOID pParam = NULL );
BOOL __cdecl IsReportedActiveLive(long nType);

void __cdecl CrtGetInstallDate(WCHAR* lpwsDate, DWORD dwSize);
void __cdecl CrtGetInstallDaysCount(WCHAR* lpwsDaysCount, DWORD dwSize);
void __cdecl CrtSetInstallDate(WCHAR* lpwsDate);

bool __cdecl CrtIsTodaySendLog(LPCWSTR lpKeyName);
void __cdecl CrtLogSendDate(LPCWSTR lpKeyName);

//////////////////////////////////////////////////////////////////////////
//文件及文件夹操作
//////////////////////////////////////////////////////////////////////////
HANDLE __stdcall CrtCreateFileW(
	LPCWSTR lpFileName,
	DWORD dwDesiredAccess,
	DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile = NULL,
	bool bAutoCreateDirectory = true
	);
BOOL __cdecl CrtCreateDirectoryW(LPCWSTR lpPathName, bool bAutoCreateDirectory = true);

#ifdef UNICODE
#define CrtCreateFile  CrtCreateFileW
#define CrtCreateDirectory  CrtCreateDirectoryW
#endif // !UNICODE

BOOL __cdecl CrtCopyFile( LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName, BOOL bFailIfExists);
BOOL __cdecl CrtMoveFile( LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName );
BOOL __cdecl CrtMoveFileEx( LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName, DWORD dwFlags );


//创建路径串中的各级目录
void __cdecl PathCreateDirectory(LPCWSTR lpPath);
void __cdecl PathCreateDirectoryFromFile(LPCWSTR lpFile);

//删除路径串中的各级目录
#define RECURSE_REMOVE_ALL	0
#define RECURSE_REMOVE_FILE	1
#define RECURSE_REMOVE_DIR	2
bool __cdecl RecurseRemoveDirectory(LPCWSTR strDir, int nType = RECURSE_REMOVE_ALL );
//////////////////////////////////////////////////////////////////////////
//网络相关
//////////////////////////////////////////////////////////////////////////
bool __cdecl IsHasInternetConnection();

//添加连网通知 - [注意：pCallback 是在回调线程锁区间，请不要在回调做太多事。]
UINT WINAPI CrtAddInternetConnectionChangedEvent( HWND hWnd, UINT uMsg, PFNInternetConnectionChangedEvent pCallback = NULL, PVOID pParam = NULL,
	UINT uCheckType = Check_Internet_Connected, BOOL bAutoRemove = TRUE );
void WINAPI CrtRemoveInternetConnectionChangedEvent( UINT uEventId );

//IsHasInternetConnectionEx
//		基本同IsHasInternetConnection, 额外增加对dnstest.baofeng.com域名的连通性判断
bool __cdecl IsHasInternetConnectionEx();
DLState __cdecl CrtDownloadFile(LPCWSTR lpUrl, LPCWSTR lpFile, HTTPDOWNLOADPARAM* pParam = NULL, HTTPDOWNLOADINFO* pDownloadInfo = NULL );
//CrtCloseDownloadFile
//		关闭异步下载模式的句柄
void __cdecl CrtCloseDownloadFile(HTTPDOWNLOADINFO* pDownloadInfo );

//CrtNavigateHttpUrl 会涉及cookie管理, 推荐使用CrtNavigateHttpUrl_Direct
bool __cdecl CrtNavigateHttpUrl( LPCWSTR lpUrl, bool bUseThread = false );
bool __cdecl CrtNavigateHttpUrl_Direct( LPCWSTR lpUrl, bool bUseThread );

int __cdecl CrtUrlEncodeW(LPCWSTR  lpUrl,  LPWSTR pNewUrl, int cchMax);
int __cdecl CrtUrlEncodeA(LPCSTR  lpUrl,  LPSTR pNewUrl, int cchMax);

//////////////////////////////////////////////////////////////////////////
//常用
//////////////////////////////////////////////////////////////////////////
bool __cdecl CrtGetVideoMD5String(LPCWSTR lpVideoFile, WCHAR* pMD5String, int cchMax);
bool __cdecl CrtGetFileMD5String(LPCWSTR lpFile, WCHAR* pMD5String, int cchMax);
DWORD __cdecl CrtGetCRC32(const BYTE* pData, DWORD dwSize);
bool __cdecl CrtGetStringMD5W( LPCWSTR lpString, int nStringLength, WCHAR* pMD5String, int cchMax );
bool __cdecl CrtGetStringMD5A( LPCSTR lpString, int nStringLength, WCHAR* pMD5String, int cchMax );

//CrtExecuteApp
//		pCommandLine如果不为空,请确保该指针指向的是一个缓冲区!
bool __cdecl CrtExecuteApp( LPCWSTR lpFile, LPWSTR pCommandLine, DWORD dwWaitTime = 0);
bool __cdecl CrtExecuteAppWithUAC( LPCWSTR lpFile, LPCWSTR lpParam , DWORD dwWaitTime );
bool __cdecl CrtIERunUrl(LPCWSTR lpUrl, bool bIEPriority = false);

DWORD __cdecl CrtGetResourceSize( HINSTANCE hInstance, UINT nID, LPCWSTR lpType );
LPVOID __cdecl CrtGetResource( HINSTANCE hInstance, UINT nID, LPCWSTR lpType, DWORD* pdwResSize );
bool __cdecl ExtractResourceToFile( LPCWSTR lpFile, HINSTANCE hInstance, UINT nID, LPCWSTR lpType );
bool __cdecl ExtractResourceToXMLString( BSTR* ppXMLString, HINSTANCE hInstance, UINT nID, LPCWSTR lpType );

//////////////////////////////////////////////////////////////////////////
//Win7相关辅助API
//////////////////////////////////////////////////////////////////////////
BOOL __cdecl IsSystemUACEnable();
bool __cdecl IsHasUAC();
bool __cdecl IsProcessVirtualized();
bool __cdecl EnableVirtualized(bool bEnable );
BOOL __cdecl UACAllowWindowMeesage(UINT uMessageID, BOOL bAllow);
bool __cdecl UACExecuteApp( LPCWSTR lpFile, LPCWSTR lpParam, DWORD dwWaitTime );
bool __cdecl FileAddAccessRights(LPCWSTR lpszFileName, LPCWSTR lpszAccountName = L"Users",  DWORD dwAccessMask = GENERIC_ALL);

//////////////////////////////////////////////////////////////////////////
//杂而有用的API
//////////////////////////////////////////////////////////////////////////
//UnZipFileToDirectory
//		bExistOverwrit 【 chenjindu 】文件存在时是否覆盖
HRESULT __stdcall UnZipFileToDirectory( LPCWSTR lpZipFile, LPCWSTR lpOutputPath, BOOL bExistOverwrite = TRUE );  
bool __cdecl CrtRegDll( LPCWSTR lpDll, bool bRegDll );
void __cdecl MinimizeMemory();
bool __cdecl IsFileWritable(LPCWSTR lpFileName);
bool __cdecl CrtGetFileSize(LPCWSTR lpFileName, LONGLONG* pFileSize);
bool __cdecl GetFileTimeString( LPCWSTR lpFle, bool bOnlyDate,  WCHAR* pString, int nStringLength );
bool __cdecl TimeStringToFileTime(LPCWSTR lpTimeString, FILETIME* pFileTime);
bool __cdecl IsConnectProjector();
bool __cdecl GetDebugPrivilege();
bool __cdecl GetDllPathForClsid( LPCWSTR lpClsid, WCHAR* pFile, int cchMax );
bool __cdecl ShutdownWindow();
bool __cdecl GetFileInfomationString( LPCWSTR lpFile, WCHAR* pDescription, int cchDecsMax, WCHAR* pCompanyName, int cchCompMax);

void __cdecl CrtLogEncrypt(LPCSTR lpData, long nLength, LPSTR pEncData );
void __cdecl CrtLogDecrypt(LPCSTR lpData, long nLength, LPSTR pDncData );


#define MAKELONGLONG(a, b)      ((LONGLONG)(((DWORD)(((DWORD_PTR)(a)) & 0xffffffff)) | ((LONGLONG)((DWORD)(((DWORD_PTR)(b)) & 0xffffffff))) << 32))
#define SAFEDELETE(puk) if ( puk ){ delete puk; puk = NULL; }



//////////////////////////////////////////////////////////////////////////
//杂而有用的API
//////////////////////////////////////////////////////////////////////////
int __cdecl CrtGetGlobalServiceControl(LPCWSTR lpKey, int iDefValue );


//////////////////////////////////////////////////////////////////////////
//最有用调试API
//////////////////////////////////////////////////////////////////////////
void __cdecl CrtOutputLogStringA(long lCata, long id, const char *pfmt,...);
void __cdecl CrtOutputLogStringW(long lCata, long id, const wchar_t *pfmt,...);
void __cdecl EnableCrtOutputLog(wchar_t *pszDll,bool bEnable);

void __cdecl CrtTraceA(const char *pfmt,...);
void __cdecl CrtTraceW(const wchar_t *pfmt,...);

#ifdef UNICODE
#define CrtOutputDebugString  CrtOutputLogStringW
#define CrtTrace			CrtTraceW
#endif // !UNICODE

//////////////////////////////////////////////////////////////////////////
//从文件中加载COM及管理
//////////////////////////////////////////////////////////////////////////
//支持相对路径，如 CrtCreateInstance( L"http.dll",...);
HRESULT WINAPI CrtCreateInstance(LPCWSTR lpFile, REFCLSID clsid, REFIID iid, void** ppObject );
int WINAPI CrtFreeLibrary(HMODULE hDLLModule);


//////////////////////////////////////////////////////////////////////////
//线程辅助API
//////////////////////////////////////////////////////////////////////////
HANDLE WINAPI CrtCreateThread( PFNCrtThreadProc pThreadProc, void *pParam, OUT UINT *pThreadId, LPCWSTR lpName = NULL, ComInitType ci = Thread_Init_Com, UINT uInitFlag = 0 );
