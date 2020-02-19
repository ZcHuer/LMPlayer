#include "stdafx.h"

#include "Reporter.h"
#include "Recorder.h"
#include "../include/LeTools.h"
#include "../include/json/json.h"
#include <ShlObj.h>

//获取mac需要
#include <Nb30.h>
#include "../LePlayer/base641.h"
#include "../Include/CommonStruct.h"

CReporter* CReporter::m_pInstance = NULL;

mutex CReporter::m_mtxBtn;
list<_BtnTJ> CReporter::m_lsBtnTJ;

HANDLE CReporter::m_hBtnWork;

string CReporter::GetMac()
{
	string mac;
	char* pchMac = new char[33];
	memset(pchMac, 0, 33);
	ASTAT Adapter;
	NCB Ncb;
	UCHAR uRetCode;
	LANA_ENUM lenum;
	int i = 0;
	memset(&Ncb, 0, sizeof(Ncb));
	Ncb.ncb_command = NCBENUM;
	Ncb.ncb_buffer = (UCHAR *)&lenum;
	Ncb.ncb_length = sizeof(lenum);
	uRetCode = Netbios(&Ncb);
	for (i = 0; i < lenum.length; i++)
	{
		memset(&Ncb, 0, sizeof(Ncb));
		Ncb.ncb_command = NCBRESET;
		Ncb.ncb_lana_num = lenum.lana[i];
		uRetCode = Netbios(&Ncb);
		memset(&Ncb, 0, sizeof(Ncb));
		Ncb.ncb_command = NCBASTAT;
		Ncb.ncb_lana_num = lenum.lana[i];
		strcpy_s((char *)Ncb.ncb_callname, NCBNAMSZ, "* ");
		Ncb.ncb_buffer = (unsigned char *)&Adapter;
		Ncb.ncb_length = sizeof(Adapter);
		uRetCode = Netbios(&Ncb);
		if (uRetCode == 0)
		{
			sprintf_s(pchMac, 32, "%02X%02X%02X%02X%02X%02X",
				Adapter.adapt.adapter_address[0],
				Adapter.adapt.adapter_address[1],
				Adapter.adapt.adapter_address[2],
				Adapter.adapt.adapter_address[3],
				Adapter.adapt.adapter_address[4],
				Adapter.adapt.adapter_address[5]
			);
			break;
		}
	}
	mac = pchMac;
	delete pchMac;
	pchMac = NULL;
	return mac;
}

string CReporter::Num2Str(int nNum)
{
	char cBuff[120] = { 0 };
	_itoa_s(nNum, cBuff, 120, 10);
	return cBuff;
}

void CReporter::InitPublic()
{
	m_sDeviceID = LeTools::GetMacString();
	m_sMac = m_sDeviceID;
	CWmiInfo wmi;
	wmi.InitWmi();
	CString sResu;
	m_bIsWifi = wmi.IsWifi();
	//机型 精确到系列 比如 联想
	CString sItems[] = { L"Name", L"Vendor" };
	wmi.GetGroupItemInfo(L"Win32_ComputerSystemProduct", sItems, 2, sResu);
	string sDeviceStyle = LeTools::ws2s(sResu.GetString());

	m_sDeviceStyle = "";
	while (sDeviceStyle.find("\t") != -1)
	{
		m_sDeviceStyle.append(sDeviceStyle.substr(0, sDeviceStyle.find("\t")));
		m_sDeviceStyle.append(" ");
		sDeviceStyle = sDeviceStyle.substr(sDeviceStyle.find("\t") + 1);
	}

	LeTools::trim(m_sDeviceStyle);
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
	m_sOsType = LeTools::ws2s(sResu.GetString());

	//开启按钮上报事件
	thread thBtn = thread(&CReporter::DoBtnSend, this);
	thBtn.detach();
}

string CReporter::Gbk2Utf8(string str)
{
	if (str.length() == 0) 
		return "";
	string strOutUTF8 = "";
	WCHAR * str1;
	int n = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
	str1 = new WCHAR[n];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, str1, n);
	n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
	char * str2 = new char[n];
	WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);
	strOutUTF8 = str2;
	delete[]str1;
	str1 = NULL;
	delete[]str2;
	str2 = NULL;
	return strOutUTF8;
}

void CReporter::DoBtnSend()
{
	while (!m_bQuit)
	{
		//等待任务执行
		WaitForSingleObject(m_hBtnWork, INFINITE);
		BOOL bWork = FALSE;
		m_mtxBtn.lock();
		bWork = m_lsBtnTJ.size();
		m_mtxBtn.unlock();
		if (!bWork) {
			//当前无任务可以执行，恢复等待状态
			ResetEvent(m_hBtnWork);
			continue;
		}

		m_mtxBtn.lock();
		int nCount = m_lsBtnTJ.size();
		m_mtxBtn.unlock();
		while (nCount)
		{
			_BtnTJ btnTj;
			m_mtxBtn.lock();
			list<_BtnTJ>::iterator itor = m_lsBtnTJ.begin();
			if (itor == m_lsBtnTJ.end()) {
				m_mtxBtn.unlock();
				//当前任务已执行完，恢复等待状态
				ResetEvent(m_hBtnWork);
				break;
			}
			//获取当前执行任务
			btnTj._Copy(*itor);
			//重列表中删除当前任务
			m_lsBtnTJ.erase(itor);
			m_mtxBtn.unlock();
			//上报数据
			BOOL bOk = SendBtnTJ(btnTj);
			//未上报成功重新放入列表中继续上报
			if (!bOk)
			{
				m_mtxBtn.lock();
				m_lsBtnTJ.push_back(btnTj);
				m_mtxBtn.unlock();
			}
			//获取当前等待任务数
			m_mtxBtn.lock();
			nCount = m_lsBtnTJ.size();
			m_mtxBtn.unlock();
		}
		//当前无任务可以执行，恢复等待状态
		ResetEvent(m_hBtnWork);
	}
	SetEvent(m_hQuit);

}

BOOL CReporter::SendBtnTJ(_BtnTJ tj)
{
	Json::Value jvData;
	//获取公共上报数据
	//GetPublicString(jvData, tj.bIsMigu);
	GetBtnEventPublicStr(jvData, tj.nSourceType, tj.sID, tj.enentType, tj.sDes);

	//根据 事件ID 
	if (tj.sID == "40")
	{
		//客户端page
		jvData["pageId"] = tj.sPageId;
		jvData["stopTime"] = tj.stoptime;
		jvData["inOut"] = tj.sinto;
	}
	else if (tj.sID == "20" || tj.sID == "46")
	{
		//关闭播放器
		jvData["useTime"] = tj.sUseTime;
	}
	else if (tj.sID == "16" || tj.sID == "17" || tj.sID == "55")
	{
		//登录注册
		jvData["phone"] = tj.sPhone;
		jvData["mail"] = tj.sMail;
		jvData["name"] = tj.sName;
	}
	else if (tj.sID == "1" || tj.sID == "2" || tj.sID == "3" || tj.sID == "4" || tj.sID == "19" || tj.sID == "30" || tj.sID == "47" || tj.sID == "61")
	{
		if (tj.sID == "1" || tj.sID == "65" || tj.sID == "65")
		{
			//添加//广告
			jvData["channelId"] = tj.sAdv.channelId;
			jvData["moduleId"] = tj.sAdv.moduleId;
			jvData["advLoc"] = tj.sAdv.advLoc;
			jvData["advId"] = tj.sAdv.advId;
			jvData["advType"] = tj.sAdv.advType;
			jvData["mediaType"] = tj.sAdv.mediaType;
			jvData["clickurl"] = tj.sAdv.clickurl;
			jvData["advCo"] = tj.sAdv.advCo;
		}
		//播放，点击，收藏
		jvData["channelId"] = tj.sPlay.channelId;
		jvData["moduleId"] = tj.sPlay.moduleId;
		jvData["moduleIndex"] = tj.sPlay.moduleIndex;
		jvData["mouduleType"] = tj.sPlay.moduleType;
		jvData["elementId"] = tj.sPlay.elementId;
		jvData["elementType"] = tj.sPlay.elementType;
		jvData["activeId"] = tj.sPlay.activeId;
		jvData["videoId"] = tj.sPlay.videoId;
		jvData["ifConsume"] = tj.sPlay.ifConsume;
		jvData["categoryId"] = tj.sPlay.categoryId;
		jvData["categoryId2"] = tj.sPlay.categoryId2;
		jvData["episodeId"] = tj.sPlay.episodeId;
		jvData["playTime"] = tj.sPlay.playTime;
		jvData["ifSuccess"] = tj.sPlay.ifSuccess;
		jvData["bannerId"] = tj.sPlay.bannerId;
		jvData["columnId"] = tj.sPlay.columnId;
		jvData["albumId"] = tj.sPlay.albumId;
		jvData["thematicId"] = tj.sPlay.thematicId;
		jvData["apkId"] = "";
		jvData["speed"] = "x1";
		jvData["clarity"] = "标清";
		jvData["screen"] = tj.sPlay.sScreen;

	}
	jvData["channelId"] = tj.sAdv.channelId;
	jvData["moduleId"] = tj.sAdv.moduleId;
	jvData["advLoc"] = tj.sAdv.advLoc;
	jvData["advId"] = tj.sAdv.advId;
	jvData["advType"] = tj.sAdv.advType;
	jvData["mediaType"] = tj.sAdv.mediaType;
	jvData["clickurl"] = tj.sAdv.clickurl;
	jvData["advCo"] = tj.sAdv.advCo;

	Json::FastWriter writer;
	string sData = writer.write(jvData);
	sData = LeTools::Gbk2Utf8(sData);

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
	wstring bigDataHTTP = L"https://collect.vgs.lenovo.com.cn/";
	wstring bigDataPaht = L"log/event";
	string sResult =LeTools::doPost(bigDataHTTP,
		bigDataPaht, sData);
	sResult = LeTools::ConvertUtf8ToGBK(sResult);
	return TRUE;

}

void CReporter::GetBtnEventPublicStr(Json::Value& jvData, int nSourceType, string sEventId, string sEventType, const string sEventDes)
{
	jvData["lenovoId"] = m_sLenovoID;
	jvData["ifVip"] = m_sifVip;
	jvData["deviceId"] = LeTools::GetMacString();;
	jvData["appId"] = m_sAppID;
	jvData["appVersion"] = m_strVer;
	jvData["appChannel"] = m_sAppChannel;
	jvData["channel"] = m_channelID;
	jvData["osType"] = m_sOsType;
	jvData["deviceStyle"] = m_sDeviceStyle;
	jvData["mac"] = LeTools::GetMacString();;
	jvData["cpid"] = m_sCpid;
	/*
	eSourceType_Unknown,	// 未知
	eSourceType_Local,		// 本地
	eSourceType_Migu,		// 咪咕
	eSourceType_FunTv,		// 风行
	eSourceType_FunTvWeb,	// 风行web版
	eSourceType_Youku		// 优酷
	*/
	if (3 == nSourceType )
		jvData["source"] = "funtv";
	else if (5 == nSourceType)
		jvData["source"] = "youku";
	else if(6 ==nSourceType)
		jvData["source"] = "knews";
	else
		jvData["source"] = "lenovo";

	jvData["actionTime"] = LeTools::GetTimeStamp();
	jvData["eventId"] = sEventId;
	jvData["eventType"] = sEventType;
	jvData["eventDes"] = sEventDes;

	Json::FastWriter writer;
	string sData = writer.write(jvData);
	sData = LeTools::Gbk2Utf8(sData);
}

bool CReporter::SendRTD_Eeventsync(const string sEventID, const string sEvnetype, const string sEventDes, string videoId, string format, int nSourceType, __int64 nUserTime)
{
	//FLOG(L"SendReport begein");
	bool bres = false;
	_BtnTJ tj;
	//必传字段
	tj.sID = sEventID;
	tj.enentType = sEvnetype;
	tj.sDes = sEventDes;

	tj.sUseTime = nUserTime;
	tj.sVideoId = videoId;
	tj.sIsVip = m_sifVip;
	tj.sFormat = format;
	tj.nSourceType = nSourceType;
	Json::Value jvData;
	//获取公共上报数据
	//GetPublicString(jvData, tj.bIsMigu);
	GetBtnEventPublicStr(jvData, tj.nSourceType, tj.sID, tj.enentType, tj.sDes);
	//tianjia
	Json::FastWriter writer;
	string sData = writer.write(jvData);
	sData = LeTools::Gbk2Utf8(sData);

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
	wstring bigDataHTTP = L"https://collect.vgs.lenovo.com.cn/";
	wstring bigDataPaht = L"log/event";
	
	string sResult = LeTools::doPost(bigDataHTTP,
		bigDataPaht, sData);
	sResult = LeTools::ConvertUtf8ToGBK(sResult);
	//FLOG(L"sResult:%s", LeTools::s2ws(sResult).c_str());
	Json::Reader jr;
	//Json::Value jvData;
	if (jr.parse(sResult, jvData))
	{
		//FLOG(L"sResult:%s",LeTools::s2ws(sResult).c_str());
		if (jvData["resultCode"].asString() == "00")
		{
			bres = true;
		}
	}
	return bres;
}

CReporter* CReporter::GetInstance()
{
	if (NULL == m_pInstance)
	{
		m_pInstance = new CReporter;

	}

	return m_pInstance;
}

void CReporter::Release()
{
	if (NULL != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

CReporter::~CReporter()
{

}

void CReporter::SetDataServer(const char* pchServer, const char* pchInterFace)
{
	m_strServer = pchServer;
	m_strInterFace = pchInterFace;
	return;
}

void CReporter::SetVersion(const char* pchVer)
{
	m_strVer = pchVer;
	return;
}
void CReporter::SetchannelID(const string channelID)
{
	m_channelID = channelID;
}

void CReporter::SetIfVip(string sVip)
{
	m_sifVip = sVip;
}

void CReporter::SetLenovoId(const char* pchLenovoId)
{
	m_sLenovoID = pchLenovoId;
	return;
}

void CReporter::SendCollectData()
{
	// 比较时间，判断今天是否上传过
	int nDate_ini = CRecorder::GetInstance()->GetTime();
	SYSTEMTIME tm;
	::GetSystemTime(&tm);
	int nDate_Now = tm.wYear * 10000 + tm.wMonth * 100 + tm.wDay;
	if (nDate_Now == nDate_ini)
		return;

	// 遍历读取本地保存的三类数据
	map<int, int> map_Count;
	CRecorder::GetInstance()->EnumCount(map_Count);
	map<int, bool> map_bool;
	CRecorder::GetInstance()->EnumBool(map_bool);
	map<int, string> map_String;
	CRecorder::GetInstance()->EnumString(map_String);

#ifndef _DEBUG
	if (0 == map_Count.size() && 0 == map_bool.size() && 0 == map_String.size())
		return;
#endif

	// 组装公共字段
	Json::Value jvData;
	jvData["appId"] = "2";
	jvData["appVersion"] = m_strVer.c_str();

	//机型 精确到系列 比如 联想
	jvData["deviceStyle"] = "thinkpad";
	jvData["appChannel"] = "1";
	jvData["channel"] = m_channelID.c_str();
	jvData["deviceId"] = GetMac().c_str();
	jvData["lenovoId"] = m_sLenovoID.c_str();
	
	// 拼接三类数据
	int i = 0;
	for (map<int, int>::iterator it = map_Count.begin(); it != map_Count.end(); it++)
	{
		Json::Value jvData_KeyValue;
		jvData_KeyValue["eventId"]		= Num2Str(it->first).c_str();
		jvData_KeyValue["eventCount"]	= Num2Str(it->second).c_str();
		jvData["events"][i] = jvData_KeyValue;
		i++;
	}
	for (map<int, bool>::iterator it = map_bool.begin(); it != map_bool.end(); it++)
	{
		Json::Value jvData_KeyValue;
		jvData_KeyValue["eventId"] = Num2Str(it->first).c_str();
		jvData_KeyValue["eventCount"] = Num2Str(it->second).c_str();
		jvData["events"][i] = jvData_KeyValue;
		i++;
	}
	for (map<int, string>::iterator it = map_String.begin(); it != map_String.end(); it++)
	{
		Json::Value jvData_KeyValue;
		jvData_KeyValue["eventId"] = Num2Str(it->first).c_str();
		jvData_KeyValue["eventCount"] = it->second.c_str();
		jvData["events"][i] = jvData_KeyValue;
		i++;
	}
	// 发送
	Json::FastWriter writer;
	string sData = writer.write(jvData);
	sData = Gbk2Utf8(sData);

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
	
	string sResult = LeTools::doPost(LeTools::s2ws(m_strServer), LeTools::s2ws(m_strInterFace), sData);
	sResult = LeTools::ConvertUtf8ToGBK(sResult);
		
	Json::Reader jr;
	Json::Value jvData_Result;
	if (jr.parse(sResult, jvData_Result))
	{
		if (jvData_Result["code"].asInt() == 0)
		{
			// 判断成功后，删除文件
			CRecorder::GetInstance()->RemoveDailyFile();
			SYSTEMTIME tm;
			::GetSystemTime(&tm);
			int nDate_Now = tm.wYear * 10000 + tm.wMonth * 100 + tm.wDay;
			string strTime = LeTools::Num2Str(nDate_Now);
			CRecorder::GetInstance()->SetTime(strTime);
		}
	}
	
	return;
}

string CReporter::GetOsType()
{
	return m_sOsType;
}

string CReporter::GetDeviceStyle()
{
	return m_sDeviceStyle;
}

void CReporter::SendRTD_Eevent(const string sEventID, const string sEvnetype, const string sEventDes, string videoId /*= ""*/, string format /*= ""*/, int nSourceType /*= 0*/, __int64 nUserTime /*= 0*/)
{
	_BtnTJ btn;
	//必传字段
	btn.sID = sEventID;
	btn.enentType = sEvnetype;
	btn.sDes = sEventDes;

	btn.sUseTime = nUserTime;
	btn.sVideoId = videoId;
	btn.sIsVip = m_sifVip;
	btn.sFormat = format;
	btn.nSourceType = nSourceType;

	m_mtxBtn.lock();
	m_lsBtnTJ.push_back(btn);
	m_mtxBtn.unlock();
	//开启按钮上报工作
	SetEvent(m_hBtnWork);
}
void CReporter::SendRTD_Page(const string pageId, const string pageLevel,
	long stopTime, int binto, int nSourceType, string sDes)
{
	_BtnTJ btn;
	//必传字段
	btn.sID = "40";
	btn.enentType = "2";
	btn.sDes = sDes;

	btn.sPageId = pageId;
	btn.sinto = LeTools::Num2Str(binto);
	long tmp = stopTime * 1000;
	btn.stoptime = LeTools::Num2Str(tmp);

	m_mtxBtn.lock();
	m_lsBtnTJ.push_back(btn);
	m_mtxBtn.unlock();
	//开启按钮上报工作
	SetEvent(m_hBtnWork);
}

void CReporter::SendRTD_Play(const string sEventID, const string sEvnetype, const string sEventDes, enum eSourceType etype, sReportData &old)
{
	_BtnTJ btn;
	//必传字段
	btn.sID = sEventID;
	btn.enentType = sEvnetype;
	btn.sDes = sEventDes;
	btn.nSourceType = etype;
	btn.sPlay._Copy(old);

	m_mtxBtn.lock();
	m_lsBtnTJ.push_back(btn);
	m_mtxBtn.unlock();
	//开启按钮上报工作
	SetEvent(m_hBtnWork);
}

void CReporter::SendRTD_Login(const string sEventID, const string sEvnetype, const string sEventDes, const string sPhone, const string mail, const string sname)
{
	_BtnTJ btn;
	//必传字段
	btn.sID = sEventID;
	btn.enentType = sEvnetype;
	btn.sDes = sEventDes;

	btn.sPhone = sPhone;
	btn.sMail = mail;
	btn.sName = sname;

	m_mtxBtn.lock();
	m_lsBtnTJ.push_back(btn);
	m_mtxBtn.unlock();
	//开启按钮上报工作
	SetEvent(m_hBtnWork);
}

void CReporter::SendRTD_Adv(const string sEventID, const string sEvnetype, const string sEventDes, AdStruct &sAdv, enum eSourceType etype, struct sReportData &old)
{
	_BtnTJ btn;
	//必传字段
	btn.sID = sEventID;
	btn.enentType = sEvnetype;
	btn.sDes = sEventDes;
	btn.nSourceType = etype;
	btn.sAdv._Copy(sAdv);
	btn.sPlay._Copy(old);

	m_mtxBtn.lock();
	m_lsBtnTJ.push_back(btn);
	m_mtxBtn.unlock();
	//开启按钮上报工作
	SetEvent(m_hBtnWork);

}

void CReporter::QuitThread()
{
	m_bQuit = true;
	WaitForSingleObject(m_hQuit, INFINITE);
}

CReporter::CReporter()
{
	m_strServer = "";
	m_strInterFace = "";
	m_bQuit = false;
	m_strVer = "1.2.0.26";
	m_aesEncry.InitKey("1102130405061708", "1102130405061708");
	m_hBtnWork = CreateEvent(0, TRUE, FALSE, 0);
	m_RepUrl = "https://collect.vgs.lenovo.com.cn/";
	InitPublic();
	m_hQuit = CreateEvent(0, TRUE, FALSE, 0);

}
