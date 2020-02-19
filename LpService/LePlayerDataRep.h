#pragma once

#include "json\json.h"


#include "Aes_encryptor\Aes_encryptor.h"
//using namespace LeConnect;
class CLePlayerDataRep
{
public:
	CLePlayerDataRep();
	~CLePlayerDataRep();

	static CLePlayerDataRep * getInstance()
	{
		if (m_instance == nullptr)
		{
			m_instance = new CLePlayerDataRep;
		}
		return m_instance;
	}
	static void delInstance()
	{
		if (m_instance)
		{
			delete m_instance;
			m_instance = nullptr;
		}
	}

	void RepInstall();
	void RepUnInstall();
private:
	string m_sLenovoID;//�û���Ψһ��ʶ����¼�����
	string m_sVipID;//��Ա��Ψһ��ʶ
	string m_sDeviceID;//Ψһ���豸��ʶ
	string m_sMac;
	const string m_sAppID = "2";
	string m_sAppVersion;//Ӧ�ð汾
	const string m_sAppChannel = "1";//��ʶ�ն����ͣ����� 3(tv)��2(pad)��1(pc)
	string m_sOsType;//����ϵͳ ���� Android 4.5��windows7
	string m_sDeviceStyle;//���� ��ȷ��ϵ�� ���� С��ϵ��

	BOOL m_bIsWifi;
	CAes_encryptor m_aesEncry;
private:
	void InitPublicData();
	void GetReportUrl(string &url);
	void GetEventPagePublicString(Json::Value& jvData, BOOL isMigu, string sEventId, string sEventType, const string sEventDes);
private:
	static CLePlayerDataRep * m_instance;
};


