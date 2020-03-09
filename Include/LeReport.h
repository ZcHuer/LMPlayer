#include <string>
using namespace std;
// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 LEREPORT_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// LEREPORT_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef LEREPORT_EXPORTS
#define LEREPORT_API __declspec(dllexport)
#else
#define LEREPORT_API __declspec(dllimport)
#endif

// 此类是从 LeReport.dll 导出的
class  CLeReport
{
public:
	// TODO:  在此添加您的方法。
	static CLeReport* GetInstance();
	static void Release();
	~CLeReport();

	/* 对外接口函数 */
	void SetDataServer(const char* pchServer, const char* pchInterFace);		// 设置上报地址
	void SetVersion(const char* pchVer);										// 版本
	void SetLenovoId(const char* pchLenovoId);									// 用户id
	void SetchannelID(const string channelID);									// 设置渠道ID
	void SetIfVip(string sVip);
	void SetLaunchFrom(string sLaunch);
	void SetExternRd(string sCmdRd);

	string GetOsType();
	string GetDeviceStyle();
	// 发送本地收集的程序数据
	void SendCollectData();
	// 实时发送数据
	void SendRTD_Eevent(const string sEventID, const string sEvnetype, const string sEventDes,
		string videoId = "", string format = "", int nSourceType = 0, __int64 nUserTime = 0);
	bool SendRTD_Eeventsync(const string sEventID, const string sEvnetype, const string sEventDes,
		string videoId = "", string format = "", int nSourceType = 0, __int64 nUserTime = 0);

	void SendRTD_Page(const string pageId, const string pageLevel, long stopTime, int binto = 1, int nSourceType = 0, string sDes = "页面浏览");
	void SendRTD_Play(const string sEventID, const string sEvnetype, const string sEventDes, enum eSourceType etype, struct sReportData &old);
	void SendRTD_Login(const string sEventID, const string sEvnetype, const string sEventDes, const string sPhone, const string mail, const string sname);
	void SendRTD_Adv(const string sEventID, const string sEvnetype, const string sEventDes, struct AdStruct &sAdv, enum eSourceType etype, struct sReportData &old);
	// 记录程序数据到本地-计数
	void SaveData_Count(int lType, int lValue = 1);
	void SaveData_Bool(int lType, bool bValue);
	void SaveData_String(int lType, const char* pchValue);
	
	void QuitThread();

private:
	CLeReport(void);
	static CLeReport* m_pInstance;	
};