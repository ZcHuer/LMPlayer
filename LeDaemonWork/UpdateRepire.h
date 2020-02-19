#pragma once
#include <string>
using namespace std;

#define Error_Download  0
#define Error_Unzip		1
#define Error_File		2

// 向窗口发送的消息
enum eUpdateMsg
{
	eUPDATE_Progress = WM_USER + 1,		// 下载进度
	eUPDATE_Success,					// 成功
	eUPDATE_NetError,					// 网络错误
	eUPDATE_NetRepair,					// 网络恢复
	eUPDATE_Error_Download,				// 下载失败
	eUPDATE_Error_Unzip,				// 下载失败
	eUPDATE_Error_File,					// 文件错误
};

struct sUpdateInfo
{
	int id;
	int tvInfoId;
	string appName;
	string appPackage;
	string appSize;
	string appLoadUrl;
	string versionCode;
	string versionName;
	string versionDesc;
	int statusCode;
	int statusCount;
	string miniVersion;
	string updateType;
	long createTime;
	long updateTime;
	string md5;
};

class CUpdateRepire
{
public:
	CUpdateRepire();
	~CUpdateRepire();

	// 查询是否正在升级
	bool IsDownLoading();
	// 查询升级配置
	bool GetUpdateConfig(sUpdateInfo& sInfo);
	// 下载升级包，如果需要开线程，则不能依赖函数的返回值，默认不开线程，因为没有回调的窗口句柄，只能依赖函数返回值
	bool DownloadAndUnzip(LPCSTR lpcstr_url, LPCSTR lpcstr_vername, bool bThread = false, HWND hWnd = NULL);
	// 绑定回调句柄
	bool SetCallBackHwnd(HWND hWnd);
	//
	bool DownloadAndRepire(LPCSTR lpcstr_url, LPCSTR lpcstr_vername, bool bThread = false, HWND hWnd = NULL);
};

