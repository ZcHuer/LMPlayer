// LeReport.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "../include/LeReport.h"
#include <map>
#include "Reporter.h"
#include "Recorder.h"
using namespace std;

CLeReport* CLeReport::m_pInstance = NULL;
CLeReport* CLeReport::GetInstance()
{
	if (NULL == m_pInstance)
		m_pInstance = new CLeReport;

	return m_pInstance;
}

void CLeReport::Release()
{
	if (NULL != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

CLeReport::~CLeReport()
{

}

void CLeReport::SetDataServer(const char* pchServer, const char* pchInterFace)
{
	return CReporter::GetInstance()->SetDataServer(pchServer, pchInterFace);
}

void CLeReport::SetVersion(const char* pchVer)
{
	return CReporter::GetInstance()->SetVersion(pchVer);
}

void CLeReport::SetLenovoId(const char* pchLenovoId)
{
	return CReporter::GetInstance()->SetLenovoId(pchLenovoId);
}
void CLeReport::SetchannelID(const string channelID)
{
	return CReporter::GetInstance()->SetchannelID(channelID);
}

void CLeReport::SetIfVip(string sVip)
{
	return CReporter::GetInstance()->SetIfVip(sVip);
}

string CLeReport::GetOsType()
{
	return CReporter::GetInstance()->GetOsType();
}

string CLeReport::GetDeviceStyle()
{
	return CReporter::GetInstance()->GetDeviceStyle();
}

void CLeReport::SendCollectData()
{
	return CReporter::GetInstance()->SendCollectData();
}

void CLeReport::SendRTD_Eevent(const string sEventID, const string sEvnetype, const string sEventDes, string videoId /*= ""*/, string format /*= ""*/, int nSourceType /*= 0*/, __int64 nUserTime /*= 0*/)
{
	return CReporter::GetInstance()->SendRTD_Eevent(sEventID, sEvnetype, sEventDes, videoId, format, nSourceType, nUserTime);
}

bool CLeReport::SendRTD_Eeventsync(const string sEventID, const string sEvnetype, const string sEventDes, string videoId /*= ""*/, string format /*= ""*/, int nSourceType /*= 0*/, __int64 nUserTime /*= 0*/)
{
	return CReporter::GetInstance()->SendRTD_Eeventsync(sEventID, sEvnetype, sEventDes, videoId, format, nSourceType, nUserTime);
}

void CLeReport::SendRTD_Page(const string pageId, const string pageLevel,
	long stopTime, int binto, int nSourceType, string sDes)
{
	return CReporter::GetInstance()->SendRTD_Page(pageId, pageLevel, stopTime, binto,nSourceType,sDes);
}

void CLeReport::SendRTD_Play(const string sEventID, const string sEvnetype, const string sEventDes, enum eSourceType etype, sReportData &old)
{
	return CReporter::GetInstance()->SendRTD_Play(sEventID, sEvnetype, sEventDes,etype, old);
}

void CLeReport::SendRTD_Login(const string sEventID, const string sEvnetype, const string sEventDes, const string sPhone, const string mail, const string sname)
{
	return CReporter::GetInstance()->SendRTD_Login(sEventID, sEvnetype, sEventDes, sPhone, mail, sname);
}

void CLeReport::SendRTD_Adv(const string sEventID, const string sEvnetype, const string sEventDes, AdStruct &sAdv, enum eSourceType etype, struct sReportData &old)
{
	return CReporter::GetInstance()->SendRTD_Adv(sEventID, sEvnetype, sEventDes, sAdv, etype, old);
}

void CLeReport::SaveData_Count(int lType, int lValue /*= 1*/)
{
	return CRecorder::GetInstance()->AddCount(lType, lValue);
}

void CLeReport::SaveData_Bool(int lType, bool bValue)
{
	return CRecorder::GetInstance()->SetBool(lType, bValue);
}

void CLeReport::SaveData_String(int lType, const char* pchValue)
{
	return CRecorder::GetInstance()->AddString(lType, pchValue);
}

void CLeReport::QuitThread()
{
	return CReporter::GetInstance()->QuitThread();
}

// 这是已导出类的构造函数。
// 有关类定义的信息，请参阅 LeReport.h
CLeReport::CLeReport()
{
    return;
}
