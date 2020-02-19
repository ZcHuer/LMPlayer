#pragma once
#include <map>
#include "..\LePlayer\Aes_encryptor.h"
using namespace std;
#include "../include/json/json.h"
#include "..\Include\CommonStruct.h"

class CReporter
{
public:
	static CReporter* GetInstance();
	static void Release();
	~CReporter();

	/* ����ӿں��� */
	void SetDataServer(const char* pchServer, const char* pchInterFace);		// �����ϱ���ַ
	void SetVersion(const char* pchVer);			// �汾
	void SetLenovoId(const char* pchLenovoId);		// �û�id
	void SetchannelID(const string channelID);
	void SetIfVip(string sVip);
	// ���ͱ����ռ��ĳ�������
	void SendCollectData();
	// ʵʱ��������
	
	string GetOsType();
	string GetDeviceStyle();

	void SendRTD_Eevent(const string sEventID, const string sEvnetype, const string sEventDes,
		string videoId = "", string format = "", int nSourceType = 0, __int64 nUserTime = 0);
	bool SendRTD_Eeventsync(const string sEventID, const string sEvnetype, const string sEventDes,
			string videoId = "", string format = "", int nSourceType = 0, __int64 nUserTime = 0);
	void SendRTD_Page(const string pageId, const string pageLevel, long stopTime, int binto = 1, int nSourceType = 0, string sDes = "ҳ�����");
	void SendRTD_Play(const string sEventID, const string sEvnetype, const string sEventDes, enum eSourceType etype, sReportData &old);
	void SendRTD_Login(const string sEventID, const string sEvnetype, const string sEventDes, const string sPhone, const string mail, const string sname);
	void SendRTD_Adv(const string sEventID, const string sEvnetype, const string sEventDes, AdStruct &sAdv, enum eSourceType etype, struct sReportData &old);
	void QuitThread();
private:
	CReporter();
	static CReporter* m_pInstance;

	string GetMac();
	string Num2Str(int nNum);
	void InitPublic();
	string Gbk2Utf8(string str);
	void DoBtnSend();
	BOOL SendBtnTJ(_BtnTJ tj);
	void GetBtnEventPublicStr(Json::Value& jvData, int nSourceType, string sEventId, string sEventType, const string sEventDes);
	
	string m_sLenovoID;//�û���Ψһ��ʶ����¼�����
	string m_sVipID;   //��Ա��Ψһ��ʶ
	string m_sifVip;	//1 :��VIP 2��VIP
	string m_sDeviceID;//Ψһ���豸��ʶ
	string m_sMac;
	string m_sCpid;
	string m_sOsType;
	const string m_sAppID = "2";
	const string m_sAppChannel = "1";//��ʶ�ն����ͣ����� 3(tv)��2(pad)��1(pc)

	string m_sSource;//��Դ aiyiqi migu
	BOOL m_bIsWifi;
	string m_sDeviceStyle;

	CAes_encryptor m_aesEncry;
	string m_RepUrl;
	string m_strServer;		// ��������ַ
	string m_strInterFace;	// �������ӿ�
	string m_strVer;		// �汾
	string m_channelID;

	HANDLE m_hQuit;
	bool m_bQuit;
public:
	static mutex m_mtxBtn;
	static list<_BtnTJ> m_lsBtnTJ;
	static HANDLE m_hBtnWork;//��ť�¼��ϱ�
};