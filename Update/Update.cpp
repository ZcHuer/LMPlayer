// Update.cpp : 定义 DLL 应用程序的导出函数。
#include "stdafx.h"
#include "Update.h"
#include "UpdateInstance.h"
#include "QwBase.h"
#include "../include/json/json.h"
#include "../SDK/http/Http.h"
using namespace LeConnect;

CUpdateInstance g_Update;

const std::wstring BASE_URL = L"https://sus.lex.lenovo.com.cn/api/list?";

// 这是已导出类的构造函数。
// 有关类定义的信息，请参阅 Update.h
CUpdate::CUpdate()
{
}

CUpdate::~CUpdate()
{
}

bool CUpdate::IsDownLoading()
{
	return g_Update.IsDownLoading();
}

bool CUpdate::GetUpdateConfig(sUpdateInfo& sInfo)
{
	bool bSucc = false;
	wstring wsUrl = BASE_URL;
	wsUrl.append(L"tvSign=lenovo_win_lmp");
	wsUrl.append(L"&sn=");
	string strMac = QwBase::GetMacString();
	wsUrl.append(QwBase::s2ws(strMac));
	HttpRequest req(wsUrl.c_str(), NULL, NULL);
	bSucc = req.doGet();

	if (bSucc)
	{
		string sData = req.getStream().str();
		sData = QwBase::ConvertUtf8ToGBK(sData);
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

bool CUpdate::DownloadAndUnzip(LPCSTR lpcstr_url, LPCSTR lpcstr_vername, bool bThread/* = false*/, HWND hWnd/* = NULL*/)
{
	return g_Update.DownloadAndUnzip(lpcstr_url, lpcstr_vername, bThread, hWnd);
}

void CUpdate::CancelDownload()
{
	return g_Update.CancelDownload();
}

bool CUpdate::SetCallBackHwnd(HWND hWnd)
{
	return g_Update.SetCallBackHwnd(hWnd);
}
