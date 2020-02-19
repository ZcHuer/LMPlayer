#include "stdafx.h"
#include "LeDB_Set.h"
#include "../SDK/Flog/FileLog.h"
#include "sqlite3\sqlite3.h"
#include "LeDBInstance.h"

CLeDB_Set::CLeDB_Set()
{

}

CLeDB_Set::~CLeDB_Set()
{

}

void CLeDB_Set::InitSetItem(const char* pchKey, const char* pchValue)
{
	sqlite3* pDB = CLeDBInstance::GetInstance()->GetSQL();
	if (NULL == pDB)
		return;

	string strSql = "insert INTO LocalSet(Name,Value) values('";
	strSql.append(pchKey);
	strSql.append("','");
	strSql.append(pchValue);
	strSql.append("');");
	sqlite3_exec(pDB, strSql.c_str(), 0, 0, NULL);

	return;
}

bool CLeDB_Set::UpdateSingleSet(const char* pchKey, const char* pchValue)
{
	sqlite3* pDB = CLeDBInstance::GetInstance()->GetSQL();
	if (NULL == pDB)
		return false;

	string strSql = "update LocalSet set Value='";
	strSql.append(CLeDBInstance::Gbk2Utf8(pchValue));
	strSql.append("'");
	strSql.append(" where Name='");
	strSql.append(CLeDBInstance::Gbk2Utf8(pchKey));
	strSql.append("';");

	char* cErrMsg;
	int nRes = sqlite3_exec(pDB, strSql.c_str(), 0, 0, &cErrMsg);
	if (nRes != SQLITE_OK)
	{
		if (cErrMsg) {
			string sErr = "LocalSet update error: ";
			sErr.append(cErrMsg);
			sErr.append("---");
			sErr.append(pchKey);
			FLOG(CLeDBInstance::s2ws(sErr).c_str());
		}
		return false;
	}
	return true;
}

char* CLeDB_Set::GetSingleSet(const char* pchKey)
{
	sqlite3* pDB = CLeDBInstance::GetInstance()->GetSQL();
	if (NULL == pDB)
		return NULL;

	string strSql = "select Value from LocalSet where Name='";
	strSql.append(pchKey);
	strSql.append("';");
	char* cErrMsg;
	string strValue = "a";
	int res = sqlite3_exec(pDB, strSql.c_str(), CLeDBInstance::SingleSetResult, &strValue, &cErrMsg);
	if (SQLITE_OK == res)
	{
		int nL = strValue.length();
		char* pchChar = new char[nL+1];
		memset(pchChar, 0, nL + 1);
		memcpy_s(pchChar, nL, strValue.c_str(), nL);
		return pchChar;
	}
	return NULL;
}

bool CLeDB_Set::CreateTable()
{
	sqlite3* pDB = CLeDBInstance::GetInstance()->GetSQL();
	if (NULL == pDB)
		return false;

	string strSql = "create table LocalSet(";
	strSql.append("Name varchar PRIMARY KEY, Value varchar);");
	char* cErrMsg5;
	int nRes = sqlite3_exec(pDB, strSql.c_str(), 0, 0, &cErrMsg5);
	if (SQLITE_OK == nRes)
		return true;

	if (cErrMsg5&&StrStrA(cErrMsg5, "already") == NULL)
	{
		sqlite3_close(pDB);
		pDB = NULL;
		FLOG(L"LocalSet 表创建失败！");
	}
	return false;
}

bool CLeDB_Set::InitSetTable()
{
	CLeDBInstance::GetInstance()->ThingBegin();
	//密码
	InitSetItem("passWord", "");
	//用户名
	InitSetItem("userName", "");
	//自动登录
	InitSetItem("autoLogin", "1");
	InitSetItem("remPWD", "1");
	//关闭时 0:最小化 1:关闭 默认：0
	InitSetItem("quit", "0");
	//视频窗口风格 0：窗口适应视频 1：视频适应窗口 2：自动全屏 默认：1
	InitSetItem("wndStyle", "1");
	//点击暂停播放 0：不暂停 1：暂停 默认：1
	InitSetItem("clickPause", "1");
	//视频最小化时是否暂停 0：不暂停 1：暂停 默认：1
	InitSetItem("minPause", "1");
	//快进速度
	InitSetItem("forwardSpeed", "10");
	//快退速度
	InitSetItem("rewindSpeed", "10");
	//屏幕比 0:原始 1：16:9 2：4:3  3：铺满 默认原始 
	InitSetItem("screenRatio", "0");
	//声道 0:立体声道 1：左声道  2：右声道 3：反转立体声道 4：左右混合声道
	InitSetItem("channel", "0");
	//退出时清空播放记录
	InitSetItem("quitClear", "0");
	//无痕播放模式
	InitSetItem("noMark", "0");
	//更新模式 0：手动更新 1：自动更新
	InitSetItem("autoUpdate", "0");

	//播放顺序 0：顺序 1：随机 2：单个 3：单个循环 4：列表循环
	InitSetItem("playOrder", "0");
	//是否置顶显示,0：从不 1：始终 2：播放时
	InitSetItem("topmost", "0");
	//音量
	InitSetItem("volume", "30");
	InitSetItem("auto", "0");
	CLeDBInstance::GetInstance()->ThingEnd();

	return true;
}