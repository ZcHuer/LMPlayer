#pragma once

#ifndef LSF_EXPORTS_C_STYLE
#define LSF_EXPORTS_C_STYLE
#endif

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the LSF_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// LSF_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef LSF_EXPORTS
#define LSF_API __declspec(dllexport)
#else
#define LSF_API __declspec(dllimport)
#endif

#define LSF_ONLINE  1
#define LSF_OFFLINE 0
#define LSF_SUCCESS 1
#define LSF_FAILED  0
#define ERROR_LSF_BASE 20000

enum EPError
{
	// custom error code
	ERROR_LSF_NOT_INIT = ERROR_LSF_BASE + 1,
	ERROR_LSF_SERVICE_UNAVAILABLE,
	ERROR_LSF_NOT_LOGIN,
	ERROR_LSF_ALREADY_LOGON,
	ERROR_LSF_VERSION_LOW,
	ERROR_LSF_NOT_GET_APPNAME,
	ERROR_PARAM_PROCESSNAME_NULL,
	ERROR_PARAM_PROCESSNAME_LOGININFO,
};

class ILoginListener
{
public:
	virtual void OnLoginResult(const char* st, int errorCode = 0) = 0;
};

class ILogoutListener
{
public:
	virtual void OnLogoutResult(int errorCode = 0) = 0;
};

typedef struct _LoginOptions
{
	int portal;
	const TCHAR *customPhotoFullPath;
	const TCHAR *loginTxt;
	const TCHAR *realmImg;
	const TCHAR *mainColor;
	const TCHAR *hideqq;
	const TCHAR *hidesina;
	const TCHAR *hidewechat;
	const TCHAR *userType;
	const TCHAR *uAgreementTxt;
	const TCHAR *uAgreementUrl;
	int countdown; //选择已登录用户界面上的倒计时秒数
	int countdown_default_action; //倒计时结束时的默认动作。0(默认)自动登录 1弹出登录界面
	LANGID lang; //选择用户语言
	VOID* preserve1;
	VOID* preserve2;
	VOID* preserve3;
	int version;
} LoginOptions;

class LSF_API LsfSdk
{
public:
	/**
	* Constructor.
	*/
	LsfSdk(void);

	/**
	* Destructor.
	*/
	~LsfSdk(void);

public:
	int Init(const char* realm = NULL, ILoginListener* login_listener = NULL, ILogoutListener* logout_listener = NULL);

	int GetLoginStatus(const char* realm);
	int Login(const char* realm, ILoginListener* listener = NULL, const char* customPhotoFullPath = "", const char* loginTxt = "", const char* realmImg = "",
		const char* mainColor = "", const char* hideqq = "", const char* hidesina = "",const char* hidewechat="", const char* userType = "", const char* uAgreementTxt = "", const char* uAgreementUrl = "");
	int LoginEx(const char* realm, const HWND parentWindow = NULL, ILoginListener* listener = NULL, LoginOptions* loginOptions = NULL);
	int LoginBySt(const char* realm,const char* st,const char* loginToken, ILoginListener* listener);
	int Logout(const char* realm, ILogoutListener* listener = NULL);
	const char* GetST(const char* realm,bool usecache);
	const char* GetUserName(const char* realm);
	const char* GetUserId(const char* realm);

	int GetLoginStatusByGroup(const char* realm);
	int LoginByGroup(const char* realm, ILoginListener* listener, const char* customPhotoFullPath = "", const char* loginTxt = "", const char* realmImg = "",
		const char* mainColor = "", const char* hideqq = "", const char* hidesina = "",const char* hidewechat="", const char* userType = "", const char* uAgreementTxt = "", const char* uAgreementUrl = "");
	int LoginByGroupEx(const char* realm, const HWND parentWindow = NULL, ILoginListener* listener = NULL, LoginOptions* loginOptions = NULL);
	int LoginByStAndGroup(const char* realm, const char* st, const char* loginToken, ILoginListener* listener);
	int LogoutByGroup(const char* realm, ILogoutListener* listener);
	const char* GetSTByGroup(const char* realm, bool usecache, const char* groupName);
	const char* GetUserNameByGroup(const char* realm);
	const char* GetUserIdByGroup(const char* realm);

	int OpenUserCenter();
	int Close(const char* realm);
	const char* GetLastError();

	int IsExistLoginUser(const char* processName = NULL);
	int CheckAccount(const char* realm, const char* account, const char* password, const char* st);
	int ReLogin(const char * realm, const char * st, const char * loginToken, ILoginListener* listener_login = NULL);
	int isHalfAccount(const char * realm, const char *Token);
	//int AutoLoginNoUi(const char* realm,const char* processName,ILoginListener* listener);
	//int LogoutByProcessName(const char* processName,ILogoutListener* listener);

	//int GroupLogin(const char* realm, ILoginListener* listener);
	//int GroupLogout(ILogoutListener* listener);
};

#ifdef LSF_EXPORTS_C_STYLE
extern "C" {
	namespace Lsf
	{
		typedef void(__stdcall *LOGIN_CALLBACK)(const char* st, int errorCode);
		typedef void(__stdcall *LOGOUT_CALLBACK)(int errorCode);

		LSF_API int __stdcall Init(const char* realm = NULL, LOGIN_CALLBACK login_listener = NULL, LOGOUT_CALLBACK logout_listener = NULL);
		LSF_API int __stdcall GetLoginStatus(const char* realm);
		LSF_API int __stdcall Login(const char* realm, const HWND parentWindow, LOGIN_CALLBACK listener, LoginOptions* loginOptions = NULL);
		LSF_API int __stdcall LoginBySt(const char* realm, const char* st, const char* loginToken, LOGIN_CALLBACK listener);
		LSF_API int __stdcall Logout(const char* realm, LOGOUT_CALLBACK listener);
		LSF_API const char* __stdcall GetST(const char* realm,bool usecache);
		LSF_API const char* __stdcall GetLsfUserName(const char* realm);
		LSF_API const char* __stdcall GetLsfUserId(const char* realm);

		LSF_API int __stdcall GetLoginStatusByGroup(const char* realm);
		LSF_API int __stdcall LoginByGroup(const char* realm, const HWND parentWindow, LOGIN_CALLBACK listener, LoginOptions* loginOptions = NULL);
		LSF_API int __stdcall LoginByStAndGroup(const char* realm, const char* st, const char* loginToken, LOGIN_CALLBACK listener);
		LSF_API int __stdcall LogoutByGroup(const char* realm, LOGOUT_CALLBACK listener);
		LSF_API const char* __stdcall GetSTByGroup(const char* realm,bool usecache, const char* groupName);
		LSF_API const char* __stdcall GetLsfUserNameByGroup(const char* realm);
		LSF_API const char* __stdcall GetLsfUserIdByGroup(const char* realm);

		LSF_API int __stdcall OpenUserCenter();
		LSF_API int __stdcall Close(const char* realm);
		LSF_API const char* __stdcall GetLsfLastError();

		LSF_API int __stdcall IsExistLoginUser(const char* processName = NULL);
		LSF_API int __stdcall CheckAccount(const char* realm, const char* account, const char* password, const char* st);
		LSF_API int __stdcall ReLogin(const char * realm, const char * st, const char * loginToken, LOGIN_CALLBACK listener_login = NULL);
		LSF_API int __stdcall isHalfAccount(const char * realm,const char *Token);
		//LSF_API int __stdcall AutoLoginNoUi(const char* realm,const char* processName,ILoginListener* listener);
		//LSF_API int __stdcall LogoutByProcessName(const char* processName,ILogoutListener* listener);

		//LSF_API int __stdcall GroupLogin(const char* realm, LOGIN_CALLBACK* listener);
		//LSF_API int __stdcall GroupLogout(LOGIN_CALLBACK* listener);
	}
}
#endif
