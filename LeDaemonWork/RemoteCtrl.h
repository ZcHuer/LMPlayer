#pragma once
#include <string>
using namespace std;
/*
{
"code": 200,
"data" : {
		"exeEndTime": "17:30",				// ִ��ʱ��-ĩ
		"exeStartTime" : "17:00",			// ִ��ʱ��-ʼ
		"genShortcut" : 1,					// ���ɿ�ݷ�ʽ
		"genStartMenu" : 0,					// ���ɿ�ʼ�˵�
		"inactiveThreshold" : 4,			// ����Ծ������-¶����
		"inactiveThresholdForDelay" : 0,	// ����Ծ������-��ʱ������
		"pollingInterval" : 60,				// ��ȡ�ļ������λ������
		"runDelayMinutes" : 0,				// ��ʱʱ����С��0��ʶ�رգ���λ������
		"runOnStartup" : 1					// �Ƿ��д����������
},
"success": true,
"message" : "���ʳɹ�",
"status" : "success"
}
*/

struct sRemoteCtrlConfig
{
	bool b_genShortcut;				// �Ƿ����������ݷ�ʽ
	bool b_genStartMenu;			// �Ƿ����ɿ�ʼ�˵�
	bool b_runDelayMinutes;			// �Ƿ���ʱ����
	bool b_runOnStartup;			// �Ƿ��д��������
	int n_runDelayMinutes;			// ��ʱʱ������λ������
	int n_exeStartTime;				// ִ��ʱ��-��ʼ-��00:00��ʼ�ķ���
	int n_exeEndTime;				// ִ��ʱ��-����-��00:00��ʼ�ķ���
	int ninactiveThreshold;			// ����Ծ������-¶����		��λ����
	int ninactiveThresholdForDelay;	// ����Ծ������-��ʱ������	��λ����
	int npollingInterval;			// ��ȡʱ����			��λ������
};

class CRemoteCtrl
{
public:
	CRemoteCtrl();
	~CRemoteCtrl();

	// Զ�̿���ҵ��������
	void RemoteCtrl();
	// �ӷ���˻�ȡ����
	bool GetConfigFromServer();
	// ��ȡ��һ��������ʱ�䵽���ڵ�����
	int GetDaysOfLastRun();
	// ��������ʱ��
	void SetRunTime();
	// ���ݷ��������ص��ַ������������ʱ��
	int CalcTime(string strTm);
	//system ����UI����
	DWORD INTER_GetExplorerToken(OUT PHANDLE phExplorerToken);

	void SetAutoRunAsExe();

	sRemoteCtrlConfig m_sConfig;
	string m_str_Config_File;
};

