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
	string m_sLenovoID;//用户的唯一标识（登录后带）
	string m_sVipID;//会员的唯一标识
	string m_sDeviceID;//唯一的设备标识
	string m_sMac;
	const string m_sAppID = "2";
	string m_sAppVersion;//应用版本
	const string m_sAppChannel = "1";//标识终端类型，比如 3(tv)、2(pad)、1(pc)
	string m_sOsType;//操作系统 比如 Android 4.5、windows7
	string m_sDeviceStyle;//机型 精确到系列 比如 小新系列

	BOOL m_bIsWifi;
	CAes_encryptor m_aesEncry;
private:
	void InitPublicData();
	void GetReportUrl(string &url);
	void GetEventPagePublicString(Json::Value& jvData, BOOL isMigu, string sEventId, string sEventType, const string sEventDes);
private:
	static CLePlayerDataRep * m_instance;
};


