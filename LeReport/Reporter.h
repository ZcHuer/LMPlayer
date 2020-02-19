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

	/* 对外接口函数 */
	void SetDataServer(const char* pchServer, const char* pchInterFace);		// 设置上报地址
	void SetVersion(const char* pchVer);			// 版本
	void SetLenovoId(const char* pchLenovoId);		// 用户id
	void SetchannelID(const string channelID);
	void SetIfVip(string sVip);
	// 发送本地收集的程序数据
	void SendCollectData();
	// 实时发送数据
	
	string GetOsType();
	string GetDeviceStyle();

	void SendRTD_Eevent(const string sEventID, const string sEvnetype, const string sEventDes,
		string videoId = "", string format = "", int nSourceType = 0, __int64 nUserTime = 0);
	bool SendRTD_Eeventsync(const string sEventID, const string sEvnetype, const string sEventDes,
			string videoId = "", string format = "", int nSourceType = 0, __int64 nUserTime = 0);
	void SendRTD_Page(const string pageId, const string pageLevel, long stopTime, int binto = 1, int nSourceType = 0, string sDes = "页面浏览");
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
	
	string m_sLenovoID;//用户的唯一标识（登录后带）
	string m_sVipID;   //会员的唯一标识
	string m_sifVip;	//1 :非VIP 2：VIP
	string m_sDeviceID;//唯一的设备标识
	string m_sMac;
	string m_sCpid;
	string m_sOsType;
	const string m_sAppID = "2";
	const string m_sAppChannel = "1";//标识终端类型，比如 3(tv)、2(pad)、1(pc)

	string m_sSource;//来源 aiyiqi migu
	BOOL m_bIsWifi;
	string m_sDeviceStyle;

	CAes_encryptor m_aesEncry;
	string m_RepUrl;
	string m_strServer;		// 服务器地址
	string m_strInterFace;	// 服务器接口
	string m_strVer;		// 版本
	string m_channelID;

	HANDLE m_hQuit;
	bool m_bQuit;
public:
	static mutex m_mtxBtn;
	static list<_BtnTJ> m_lsBtnTJ;
	static HANDLE m_hBtnWork;//按钮事件上报
};