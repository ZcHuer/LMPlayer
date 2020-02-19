#pragma once
#include <string>
using namespace std;
/*
{
"code": 200,
"data" : {
		"exeEndTime": "17:30",				// 执行时间-末
		"exeStartTime" : "17:00",			// 执行时间-始
		"genShortcut" : 1,					// 生成快捷方式
		"genStartMenu" : 0,					// 生成开始菜单
		"inactiveThreshold" : 4,			// 不活跃的天数-露出用
		"inactiveThresholdForDelay" : 0,	// 不活跃的天数-延时自启用
		"pollingInterval" : 60,				// 拉取的间隔，单位：分钟
		"runDelayMinutes" : 0,				// 延时时长，小于0标识关闭，单位：分钟
		"runOnStartup" : 1					// 是否改写开机自启动
},
"success": true,
"message" : "访问成功",
"status" : "success"
}
*/

struct sRemoteCtrlConfig
{
	bool b_genShortcut;				// 是否生成桌面快捷方式
	bool b_genStartMenu;			// 是否生成开始菜单
	bool b_runDelayMinutes;			// 是否延时启动
	bool b_runOnStartup;			// 是否改写开机自启
	int n_runDelayMinutes;			// 延时时长，单位：分钟
	int n_exeStartTime;				// 执行时间-开始-从00:00开始的分钟
	int n_exeEndTime;				// 执行时间-结束-从00:00开始的分钟
	int ninactiveThreshold;			// 不活跃的天数-露出用		单位：天
	int ninactiveThresholdForDelay;	// 不活跃的天数-延时自启用	单位：天
	int npollingInterval;			// 拉取时间间隔			单位：分钟
};

class CRemoteCtrl
{
public:
	CRemoteCtrl();
	~CRemoteCtrl();

	// 远程控制业务工作函数
	void RemoteCtrl();
	// 从服务端获取配置
	bool GetConfigFromServer();
	// 获取上一次启动的时间到现在的天数
	int GetDaysOfLastRun();
	// 设置运行时间
	void SetRunTime();
	// 根据服务器返回的字符串计算出整形时间
	int CalcTime(string strTm);
	//system 调用UI交互
	DWORD INTER_GetExplorerToken(OUT PHANDLE phExplorerToken);

	void SetAutoRunAsExe();

	sRemoteCtrlConfig m_sConfig;
	string m_str_Config_File;
};

