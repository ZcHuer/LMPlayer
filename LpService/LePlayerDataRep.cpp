#include <string.h>
#include <tchar.h>
#include "WMIInfo\WMIInfo.h"
#include "QwBase.h"
#include "base64\base641.h"
#include "http\Http.h"
#include <sstream>
using namespace std;
using namespace LeConnect;
#include "LePlayerDataRep.h"
#define MAINURL _T("http://lebo.vgs.lenovo.com.cn")
CLePlayerDataRep * CLePlayerDataRep::m_instance = nullptr;
CLePlayerDataRep::CLePlayerDataRep()
{
	m_sLenovoID = "";
	m_sVipID = "";
	m_sDeviceID = "";
	m_sAppVersion = "1.0.1.0";
	m_sOsType = "";
	m_sMac = "";
	m_sDeviceStyle = "";
	m_bIsWifi = FALSE;
	m_aesEncry.InitKey("1102130405061708", "1102130405061708");
}
CLePlayerDataRep::~CLePlayerDataRep()
{
}
void CLePlayerDataRep::GetReportUrl(string &url)
{
	wstring wsUrl =MAINURL;
	wsUrl.append(L"/back/urls/leplayer2/1");
	wsUrl.append(L"?mac=");
	wsUrl.append(QwBase::s2ws(QwBase::GetMacString()));
	HttpRequest req(wsUrl.c_str(), NULL, NULL);
	BOOL bSucc = req.doGet();
	if (bSucc)
	{
		string sResult = req.getStream().str();
		sResult = QwBase::ConvertUtf8ToGBK(sResult);
		Json::Reader jr;
		Json::Value jvData;
		if (jr.parse(sResult, jvData))
		{
			if (jvData["code"].asInt() == 0)
			{
				Json::Value jv = jvData["data"];
				url = jv["bigDataDomain"].asString();
			}
		}
	}

}

void CLePlayerDataRep::InitPublicData()
{
	//设备标识，PC使用MAC
	//m_aesEncry.InitKey("1102130405061708", "1102130405061708");
	m_sDeviceID = QwBase::GetMac();
	m_sMac = m_sDeviceID;
	CWmiInfo wmi;
	wmi.InitWmi();
	CString sResu;
	m_bIsWifi = wmi.IsWifi();
	//机型 精确到系列 比如 联想
	CString sItems[] = { L"Name", L"Vendor" };
	wmi.GetGroupItemInfo(L"Win32_ComputerSystemProduct", sItems, 2, sResu);
	string sDeviceStyle = QwBase::ws2s(sResu.GetBuffer(0));

	m_sDeviceStyle = "";
	while (sDeviceStyle.find("\t") != -1)
	{
		m_sDeviceStyle.append(sDeviceStyle.substr(0, sDeviceStyle.find("\t")));
		m_sDeviceStyle.append(" ");
		sDeviceStyle = sDeviceStyle.substr(sDeviceStyle.find("\t") + 1);
	}

	m_sDeviceStyle = QwBase::trim(m_sDeviceStyle);
	m_sDeviceStyle.append(" ");
	m_sDeviceStyle.append(sDeviceStyle);

	sDeviceStyle = m_sDeviceStyle;
	m_sDeviceStyle = "";
	while (sDeviceStyle.find("\r\n") != -1)
	{
		m_sDeviceStyle.append(sDeviceStyle.substr(0, sDeviceStyle.find("\r\n")));
		sDeviceStyle = sDeviceStyle.substr(sDeviceStyle.find("\r\n") + 2);
	}
	m_sDeviceStyle.append(sDeviceStyle);

	//获取操作系统版本
	wmi.GetSingleItemInfo(L"Win32_OperatingSystem", L"Caption", sResu);
	m_sOsType = QwBase::ws2s(sResu.GetBuffer(0));

}
void CLePlayerDataRep::GetEventPagePublicString(Json::Value& jvData, BOOL isMigu, string sEventId, string sEventType, const string sEventDes)
{
	jvData["lenovoId"] = m_sLenovoID;
	jvData["vipId"] = m_sVipID;
	jvData["deviceId"] = m_sDeviceID;
	jvData["appId"] = m_sAppID;
	jvData["appVersion"] = m_sAppVersion;
	jvData["appChannel"] = m_sAppChannel;
	jvData["osType"] = m_sOsType;
	jvData["deviceStyle"] = m_sDeviceStyle;
	jvData["mac"] = m_sMac;
	jvData["source"] = "lenovo";
	jvData["actionTime"] = QwBase::GetTimeStamp();
	jvData["eventId"] = sEventId;
	jvData["eventType"] = sEventType;
	jvData["eventDes"] = sEventDes;

	Json::FastWriter writer;
	string sData = writer.write(jvData);
	sData = QwBase::Gbk2Utf8(sData);

}
void CLePlayerDataRep::RepInstall()
{
	//服务程序安装数据上报
	InitPublicData();
	string bigDomain;
	GetReportUrl(bigDomain);
	Json::Value jvData;
	//获取公共上报数据
	//GetPublicString(jvData, tj.bIsMigu);
	//GetEventPagePublicString(jvData, false, "47", "1", "service install");
	//tianjia

	jvData["lenovoId"] = m_sLenovoID;
	jvData["vipId"] = m_sVipID;
	jvData["deviceId"] = m_sDeviceID;
	jvData["appId"] = m_sAppID;
	jvData["appVersion"] = m_sAppVersion;
	jvData["appChannel"] = m_sAppChannel;
	jvData["osType"] = m_sOsType;
	jvData["deviceStyle"] = m_sDeviceStyle;
	jvData["mac"] = m_sMac;
	jvData["source"] = "lenovo";
	jvData["actionTime"] = QwBase::GetTimeStamp();
	jvData["eventId"] = "52";
	jvData["eventType"] = "1";
	jvData["eventDes"] = "service install";

	Json::FastWriter writer;
	string sData = writer.write(jvData);
	sData = QwBase::Gbk2Utf8(sData);
	
	string sEncrypt = m_aesEncry.encrypt(sData);
	DWORD dwSize = sEncrypt.length() * 2;
	unsigned char* cDes = (unsigned char*)malloc(sEncrypt.length() * 2);
	memset(cDes, 0, dwSize);
	base64_encode((unsigned char*)sEncrypt.c_str(), sEncrypt.length(), cDes, &dwSize);
	sData = (char*)cDes;
	free(cDes);

	Json::Value jsPost;
	jsPost["encrypt"] = sData;
	sData = writer.write(jsPost);
	wstring wbigDomain =QwBase::s2ws(bigDomain);

	string sResult = QwBase::doPost(wbigDomain,
		_T("log/event"), sData);
	sResult = QwBase::ConvertUtf8ToGBK(sResult);
	return ;
	
}

void CLePlayerDataRep::RepUnInstall()
{
	//服务程序安装数据上报
	InitPublicData();
	string bigDomain;
	GetReportUrl(bigDomain);
	Json::Value jvData;
	//获取公共上报数据
	//GetPublicString(jvData, tj.bIsMigu);
	//GetEventPagePublicString(jvData, false, "47", "1", "service install");
	//tianjia

	jvData["lenovoId"] = m_sLenovoID;
	jvData["vipId"] = m_sVipID;
	jvData["deviceId"] = m_sDeviceID;
	jvData["appId"] = m_sAppID;
	jvData["appVersion"] = m_sAppVersion;
	jvData["appChannel"] = m_sAppChannel;
	jvData["osType"] = m_sOsType;
	jvData["deviceStyle"] = m_sDeviceStyle;
	jvData["mac"] = m_sMac;
	jvData["source"] = "lenovo";
	jvData["actionTime"] = QwBase::GetTimeStamp();
	jvData["eventId"] = "53";
	jvData["eventType"] = "1";
	jvData["eventDes"] = "service uninstall";

	Json::FastWriter writer;
	string sData = writer.write(jvData);
	sData = QwBase::Gbk2Utf8(sData);

	string sEncrypt = m_aesEncry.encrypt(sData);
	DWORD dwSize = sEncrypt.length() * 2;
	unsigned char* cDes = (unsigned char*)malloc(sEncrypt.length() * 2);
	memset(cDes, 0, dwSize);
	base64_encode((unsigned char*)sEncrypt.c_str(), sEncrypt.length(), cDes, &dwSize);
	sData = (char*)cDes;
	free(cDes);

	Json::Value jsPost;
	jsPost["encrypt"] = sData;
	sData = writer.write(jsPost);
	wstring wbigDomain = QwBase::s2ws(bigDomain);

	string sResult = QwBase::doPost(wbigDomain,
		_T("log/event"), sData);
	sResult = QwBase::ConvertUtf8ToGBK(sResult);
	return;
}
