#pragma once
#include "UpdateDlg.h"

class CUpdatehelper
{
public:
	static CUpdatehelper* GetInstance();
	static void Release();
	
	~CUpdatehelper();

	// 启动后例行升级检查，每次启动都会做一次，获取服务端升级配置并比较是否需要升级
	bool CheckUpdate();
	// 手动检查更新
	bool ManualCheck();	
	
	// 查询服务端升级配置
	bool GetUpdateInfo();
	// 比较版本判断是否需要升级
	bool IsNewVersion(const char * version_Net, const char * version_Local);
	// 获取升级类型
	string GetUpdateType();

	// 静默升级
	bool SilentInstall(string strDownLoadUrl, string strSvrVer);
	// 强制升级
	bool InstallNow();
	// 用户选择是否立即升级
	bool UserChoiceUpdate(bool bUnKnownCP = false);

	// 设置是否开机启动的方式
	void SetLeplayerAutoRun(bool bAutoRun);
	// 判断是否是开机启动的方式
	bool IsLeplayerAutoRun();
	// 判断是否升级进行中
	bool IsUpdateing();
	// 取消下载
	void CancelDownload();

	// 向配置中写版本信息，新版本
	void WriteVersionToCfg_New(string strVer);
	// 向配置中写版本信息，旧版本
	void WriteVersionToCfg_Old(string strVer);
	// 向升级后台发送升级结果
	void ReportDowsloadResult(bool bRet);
	// 切换到新版本
	bool SwitchVerNow();
	// 用户选择是否立即切换
	bool UserChoiceSwitch();
	// 当主程序激活时
	void WhenLeplayerActivation();

private:
	CUpdatehelper();
	static CUpdatehelper* m_pInstance;
	// 升级组件
	CUpdate m_cUpdate;
	// 服务端升级配置
	sUpdateInfo m_sUpdateInfo;
	bool m_bLePlayerAutoRun;
	bool m_bUpdateWork;	
};