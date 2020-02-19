#pragma once
#include "UpdateDlg.h"

class CUpdatehelper
{
public:
	static CUpdatehelper* GetInstance();
	static void Release();
	
	~CUpdatehelper();

	// ����������������飬ÿ������������һ�Σ���ȡ������������ò��Ƚ��Ƿ���Ҫ����
	bool CheckUpdate();
	// �ֶ�������
	bool ManualCheck();	
	
	// ��ѯ�������������
	bool GetUpdateInfo();
	// �Ƚϰ汾�ж��Ƿ���Ҫ����
	bool IsNewVersion(const char * version_Net, const char * version_Local);
	// ��ȡ��������
	string GetUpdateType();

	// ��Ĭ����
	bool SilentInstall(string strDownLoadUrl, string strSvrVer);
	// ǿ������
	bool InstallNow();
	// �û�ѡ���Ƿ���������
	bool UserChoiceUpdate(bool bUnKnownCP = false);

	// �����Ƿ񿪻������ķ�ʽ
	void SetLeplayerAutoRun(bool bAutoRun);
	// �ж��Ƿ��ǿ��������ķ�ʽ
	bool IsLeplayerAutoRun();
	// �ж��Ƿ�����������
	bool IsUpdateing();
	// ȡ������
	void CancelDownload();

	// ��������д�汾��Ϣ���°汾
	void WriteVersionToCfg_New(string strVer);
	// ��������д�汾��Ϣ���ɰ汾
	void WriteVersionToCfg_Old(string strVer);
	// ��������̨�����������
	void ReportDowsloadResult(bool bRet);
	// �л����°汾
	bool SwitchVerNow();
	// �û�ѡ���Ƿ������л�
	bool UserChoiceSwitch();
	// �������򼤻�ʱ
	void WhenLeplayerActivation();

private:
	CUpdatehelper();
	static CUpdatehelper* m_pInstance;
	// �������
	CUpdate m_cUpdate;
	// �������������
	sUpdateInfo m_sUpdateInfo;
	bool m_bLePlayerAutoRun;
	bool m_bUpdateWork;	
};