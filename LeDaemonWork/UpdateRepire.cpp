#include "stdafx.h"
#include "UpdateRepire.h"
#include "RepireInstance.h"
CRepireInstance g_Update;

#include "../SDK/http/Http.h"
#include "../Include/json/json.h"
#include "../Include/LeTools.h"
#include "../SDK/Flog/FileLog.h"


using namespace LeConnect;
const wstring BASE_URL = L"https://sus.lex.lenovo.com.cn/api/list?";

CUpdateRepire::CUpdateRepire()
{
}


CUpdateRepire::~CUpdateRepire()
{
}
bool CUpdateRepire::IsDownLoading()
{
	return g_Update.IsDownLoading();
}

bool CUpdateRepire::GetUpdateConfig(sUpdateInfo& sInfo)
{
	bool bSucc = false;
	wstring wsUrl = BASE_URL;
	wsUrl.append(L"tvSign=lenovo_windows");
	wsUrl.append(L"&sn=");
	string strMac = LeTools::GetMacString();
	wsUrl.append(LeTools::s2ws(strMac));
	HttpRequest req(wsUrl.c_str(), NULL, NULL);
	bSucc = req.doGet();

	if (bSucc)
	{
		string sData = req.getStream().str();
		sData = LeTools::ConvertUtf8ToGBK(sData);
		Json::Reader jr;
		Json::Value jv;
		if (jr.parse(sData, jv))
		{
			if (jv["code"].asInt() == 200)
			{
				Json::Value jvData = jv["data"][0];

				sInfo.appLoadUrl = jvData["appLoadUrl"].asString();
				sInfo.appName = jvData["appName"].asString();
				sInfo.appPackage = jvData["appPackage"].asString();
				sInfo.appSize = jvData["appSize"].asString();
				sInfo.createTime = jvData["createTime"].asInt64();
				sInfo.id = jvData["id"].asInt64();
				sInfo.md5 = jvData["md5"].asString();
				sInfo.miniVersion = jvData["miniVersion"].asString();
				sInfo.statusCode = jvData["statusCode"].asInt64();
				sInfo.statusCount = jvData["statusCount"].asInt64();
				sInfo.tvInfoId = jvData["tvInfoId"].asInt64();
				sInfo.updateTime = jvData["updateTime"].asInt64();
				sInfo.updateType = jvData["updateType"].asString();
				sInfo.versionCode = jvData["versionCode"].asString();
				sInfo.versionDesc = jvData["versionDesc"].asString();
				sInfo.versionName = jvData["versionName"].asString();
				return true;
			}
		}
	}
	return false;
}

bool CUpdateRepire::DownloadAndUnzip(LPCSTR lpcstr_url, LPCSTR lpcstr_vername, bool bThread/* = false*/, HWND hWnd/* = NULL*/)
{
	return g_Update.DownloadAndUnzip(lpcstr_url, lpcstr_vername, bThread, hWnd);
}

bool CUpdateRepire::DownloadAndRepire(LPCSTR lpcstr_url, LPCSTR lpcstr_vername, bool bThread/* = false*/, HWND hWnd/* = NULL*/)
{
	return g_Update.DownloadAndRepire(lpcstr_url, lpcstr_vername, bThread, hWnd);
}

bool CUpdateRepire::SetCallBackHwnd(HWND hWnd)
{
	return g_Update.SetCallBackHwnd(hWnd);
}

