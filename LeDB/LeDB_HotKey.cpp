#include "stdafx.h"
#include "LeDB_HotKey.h"
#include "../SDK/Flog/FileLog.h"
#include "sqlite3\sqlite3.h"
#include "LeDBInstance.h"

CLeDB_HotKey::CLeDB_HotKey()
{

}

CLeDB_HotKey::~CLeDB_HotKey()
{

}

void CLeDB_HotKey::DefaultHotkey()
{
	sqlite3* pDB = CLeDBInstance::GetInstance()->GetSQL();
	if (NULL == pDB)
		return;

	string strSql = "delete from HotKey";
	char* error;
	sqlite3_exec(pDB, strSql.c_str(), 0, 0, &error);

	InitHotKey();
	return;
}

void CLeDB_HotKey::InsertKotKey(const char* pchName, int key1, int key2)
{
	sqlite3* pDB = CLeDBInstance::GetInstance()->GetSQL();
	if (NULL == pDB)
		return;

	string strSql = "insert INTO HotKey(name,fKey,sKey) values('";
	strSql.append(pchName);
	strSql.append("',");
	strSql.append(CLeDBInstance::Int2Str(key1));
	strSql.append(",");
	strSql.append(CLeDBInstance::Int2Str(key2));
	strSql.append(");");
	sqlite3_exec(pDB, strSql.c_str(), 0, 0, NULL);
	
	return;
}

void CLeDB_HotKey::GetHotkey(const char* pchName, _HotKey& hk)
{
	sqlite3* pDB = CLeDBInstance::GetInstance()->GetSQL();
	if (NULL == pDB)
		return;

	hk.Init();
	string strSql = "select * from HotKey where name='";
	strSql.append(pchName);
	strSql.append("';");
	char* cErrMsg;
	int res = sqlite3_exec(pDB, strSql.c_str(), CLeDBInstance::HotkeyResult, &hk, &cErrMsg);

	return;
}

void CLeDB_HotKey::SetHotkey(const char* pchName, _HotKey hk)
{
	sqlite3* pDB = CLeDBInstance::GetInstance()->GetSQL();
	if (NULL == pDB)
		return;

	UINT nKey1 = 0;
	if (hk.m_bAlt)nKey1 = VK_MENU;
	if (hk.m_bCtrl)nKey1 = VK_CONTROL;
	if (hk.m_bShift)nKey1 = VK_SHIFT;

	string strSql = "update HotKey set fKey=";
	strSql.append(CLeDBInstance::Int2Str(nKey1));
	strSql.append(",sKey=");
	strSql.append(CLeDBInstance::Int2Str(hk.m_nKey));
	strSql.append(" where name='");
	strSql.append(pchName);
	strSql.append("';");
	char* err;
	sqlite3_exec(pDB, strSql.c_str(), 0, 0, &err);

	return;
}

bool CLeDB_HotKey::CreateTable()
{
	sqlite3* pDB = CLeDBInstance::GetInstance()->GetSQL();
	if (NULL == pDB)
		return false;

	string strSql = "create table HotKey(";
	strSql.append("name varchar PRIMARY KEY, fKey integer,sKey integer);");
	char* cErrMsg6;
	int nRes = sqlite3_exec(pDB, strSql.c_str(), 0, 0, &cErrMsg6);
	if (SQLITE_OK == nRes)
		return true;

	if (cErrMsg6&&StrStrA(cErrMsg6, "already") == NULL)
	{
		sqlite3_close(pDB);
		pDB = NULL;
		FLOG(L"HotKey 表创建失败！");
	}
	return false;
}

void CLeDB_HotKey::InitHotKey()
{
	CLeDBInstance::GetInstance()->ThingBegin();
	//打开文件
	InsertKotKey("hk_openfile", VK_CONTROL, 'O');
	//打开文件夹
	InsertKotKey("hk_openfolder", VK_CONTROL, 'F');
	//播放暂停
	InsertKotKey("hk_pause", 0, VK_SPACE);
	//停止
	InsertKotKey("hk_stop", VK_CONTROL, 'S');
	//退出
	InsertKotKey("hk_quit", VK_CONTROL, 'W');
	//快进
	InsertKotKey("hk_forward", 0, VK_RIGHT);
	//快退
	InsertKotKey("hk_rewind", 0, VK_LEFT);
	//快进30秒
	InsertKotKey("hk_forward30", VK_CONTROL, VK_RIGHT);
	//快退30秒
	InsertKotKey("hk_rewind30", VK_CONTROL, VK_LEFT);
	//提高音量
	InsertKotKey("hk_upvolume", 0, VK_UP);
	//降低音量
	InsertKotKey("hk_dowmvolume", 0, VK_DOWN);
	//静音
	InsertKotKey("hk_mute", VK_CONTROL, 'M');
	//全屏切换
	InsertKotKey("hk_full", VK_CONTROL, VK_RETURN);
	CLeDBInstance::GetInstance()->ThingEnd();
	return;
}