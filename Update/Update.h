// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 UPDATE_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// UPDATE_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef UPDATE_EXPORTS
#define UPDATE_API __declspec(dllexport)
#else
#define UPDATE_API __declspec(dllimport)
#endif

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

// 此类是从 Update.dll 导出的
class UPDATE_API CUpdate 
{
public:
	CUpdate();
	~CUpdate();
	// 查询是否正在升级
	bool IsDownLoading();
	// 查询升级配置
	bool GetUpdateConfig(sUpdateInfo& sInfo);
	// 下载升级包，如果需要开线程，则不能依赖函数的返回值，默认不开线程，因为没有回调的窗口句柄，只能依赖函数返回值
	bool DownloadAndUnzip(LPCSTR lpcstr_url, LPCSTR lpcstr_vername, bool bThread = false, HWND hWnd = NULL);
	// 取消下载
	void CancelDownload();
	// 绑定回调句柄
	bool SetCallBackHwnd(HWND hWnd);
};