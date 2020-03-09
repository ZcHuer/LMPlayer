/************************************************************************/
/*
此类用于解析命令行，并将命令行分类输出；
兼容已有的未规范的命令行，多为单一命令；
支持多个命令行的指令输入，需要符合命令行的规范。

// 已有的不规范的指令列表
/lmplay				乐窗调用的播放指令
/active				乐窗激活指令
/activeS			乐窗激活指令
/activeSS			乐窗激活指令
-pcm				悬浮球调用启动
-desktop.lnk		桌面快捷方式启动
-startMenu.lnk		开始菜单启动
-content			内容快捷方式启动
/AutoRun			开机自启动
zhLenovo://cid=		原有咪咕播放协议
http://
fsp
lenovo-ai:play		智能识别命令行
lenovo-ai:pause		智能识别命令行
lenovo-ai:			智能识别命令行 因为后跟数字，已废弃

// 命令行指令的规范格式和类型，支持4种类型
lv_r:***			运行指令，方便以后增加后直接透传给服务器
lv_p:***			播放指令，处理播放时，再判断是那种播放协议
lv_f:***			功能指令，播放、暂停等
lv_u:***			打开指令，打开页面等等
可以传入多个，以空格区分，但每种类型，只允许传入一个指令
示例：
LMPlayer.exe lv_r:-pcm lv_p:http://aa.mp4 lv_f:play lv_u:www.baidu.com
*/
/************************************************************************/
#pragma once
#include <string>
#include <vector>
using namespace std;

// 已有命令行字符串宏定义，防止到处copy出错
#define CMD_RUN_NULL			L""					// 文件启动
#define CMD_RUN_ACTIVE			L"/active:"			// 激活启动
#define CMD_RUN_DESKTOP			L"-desktop.lnk"		// 桌面快捷方式启动
#define CMD_RUN_STARTMENU		L"-startMenu.lnk"	// 开始菜单启动
#define CMD_RUN_TASKBAR			L"-taskbar.lnk"		// 任务栏启动
#define CMD_RUN_AUTORUN			L"/AutoRun"			// 开机自启动
#define CMD_RUN_PCM				L"-pcm"				// 悬浮球启动
#define CMD_RUN_CONTENT			L"-content"			// 内容快捷方式启动
#define CMC_RUN_FILERUN			L"FileRun"			// 文件关联启动
#define CMD_PLAY_LM				L"/lmplay"			// 乐窗调起播放
#define CMD_FUNC_PLAY			L"lenovo-ai:play"	// AI播放
#define CMD_FUNC_PAUSE			L"lenovo-ai:pause"	// AI暂停
#define CMD_RUN_RUNNEW			L"runnew"			// 启动新版本


class CCMDParser
{
public:
	CCMDParser();
	~CCMDParser();

	// 解析命令行串
	void Parse(wstring wstrCmd);
	// 解析命令行串，兼容已有的不规范指令
	void ParseOldCmd(wstring wstrCmd);
	// 默认解析播放串
	bool ParsePlayJson(wstring wstrCmd);

	// 判断命令行类型
	bool IsRun();	
	bool IsPlay();	
	bool IsFunc();
	bool IsOpenUrl();

	// 获取命令行字段
	wstring GetRunCmd();
	wstring GetPlayCmd();
	wstring GetFunCCmd();
	wstring GetUrlCmd();

private:
	bool m_bRun;
	bool m_bPlay;
	bool m_bFunc;
	bool m_bUrl;
	wstring m_wstrCmd_Run;		// 启动指令，只允许一个
	wstring m_wstrCmd_Play;		// 播放串，只允许一个
	wstring m_wstrCmd_Func;		// 功能指令，只允许一个
	wstring m_wstrCmd_Url;		// 打开指令，只允许一个
};

