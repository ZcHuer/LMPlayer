/************************************************************************/
/*
�������ڽ��������У����������з��������
�������е�δ�淶�������У���Ϊ��һ���
֧�ֶ�������е�ָ�����룬��Ҫ���������еĹ淶��

// ���еĲ��淶��ָ���б�
/lmplay				�ִ����õĲ���ָ��
/active				�ִ�����ָ��
/activeS			�ִ�����ָ��
/activeSS			�ִ�����ָ��
-pcm				�������������
-desktop.lnk		�����ݷ�ʽ����
-startMenu.lnk		��ʼ�˵�����
-content			���ݿ�ݷ�ʽ����
/AutoRun			����������
zhLenovo://cid=		ԭ���乾����Э��
http://
fsp
lenovo-ai:play		����ʶ��������
lenovo-ai:pause		����ʶ��������
lenovo-ai:			����ʶ�������� ��Ϊ������֣��ѷ���

// ������ָ��Ĺ淶��ʽ�����ͣ�֧��4������
lv_r:***			����ָ������Ժ����Ӻ�ֱ��͸����������
lv_p:***			����ָ�������ʱ�����ж������ֲ���Э��
lv_f:***			����ָ����š���ͣ��
lv_u:***			��ָ���ҳ��ȵ�
���Դ��������Կո����֣���ÿ�����ͣ�ֻ������һ��ָ��
ʾ����
LMPlayer.exe lv_r:-pcm lv_p:http://aa.mp4 lv_f:play lv_u:www.baidu.com
*/
/************************************************************************/
#pragma once
#include <string>
#include <vector>
using namespace std;

// �����������ַ����궨�壬��ֹ����copy����
#define CMD_RUN_NULL			L""					// �ļ�����
#define CMD_RUN_ACTIVE			L"/active:"			// ��������
#define CMD_RUN_DESKTOP			L"-desktop.lnk"		// �����ݷ�ʽ����
#define CMD_RUN_STARTMENU		L"-startMenu.lnk"	// ��ʼ�˵�����
#define CMD_RUN_TASKBAR			L"-taskbar.lnk"		// ����������
#define CMD_RUN_AUTORUN			L"/AutoRun"			// ����������
#define CMD_RUN_PCM				L"-pcm"				// ����������
#define CMD_RUN_CONTENT			L"-content"			// ���ݿ�ݷ�ʽ����
#define CMC_RUN_FILERUN			L"FileRun"			// �ļ���������
#define CMD_PLAY_LM				L"/lmplay"			// �ִ����𲥷�
#define CMD_FUNC_PLAY			L"lenovo-ai:play"	// AI����
#define CMD_FUNC_PAUSE			L"lenovo-ai:pause"	// AI��ͣ
#define CMD_RUN_RUNNEW			L"runnew"			// �����°汾


class CCMDParser
{
public:
	CCMDParser();
	~CCMDParser();

	// ���������д�
	void Parse(wstring wstrCmd);
	// ���������д����������еĲ��淶ָ��
	void ParseOldCmd(wstring wstrCmd);
	// Ĭ�Ͻ������Ŵ�
	bool ParsePlayJson(wstring wstrCmd);

	// �ж�����������
	bool IsRun();	
	bool IsPlay();	
	bool IsFunc();
	bool IsOpenUrl();

	// ��ȡ�������ֶ�
	wstring GetRunCmd();
	wstring GetPlayCmd();
	wstring GetFunCCmd();
	wstring GetUrlCmd();

private:
	bool m_bRun;
	bool m_bPlay;
	bool m_bFunc;
	bool m_bUrl;
	wstring m_wstrCmd_Run;		// ����ָ�ֻ����һ��
	wstring m_wstrCmd_Play;		// ���Ŵ���ֻ����һ��
	wstring m_wstrCmd_Func;		// ����ָ�ֻ����һ��
	wstring m_wstrCmd_Url;		// ��ָ�ֻ����һ��
};

