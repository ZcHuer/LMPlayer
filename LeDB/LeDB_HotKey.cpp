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
		FLOG(L"HotKey ����ʧ�ܣ�");
	}
	return false;
}

void CLeDB_HotKey::InitHotKey()
{
	CLeDBInstance::GetInstance()->ThingBegin();
	//���ļ�
	InsertKotKey("hk_openfile", VK_CONTROL, 'O');
	//���ļ���
	InsertKotKey("hk_openfolder", VK_CONTROL, 'F');
	//������ͣ
	InsertKotKey("hk_pause", 0, VK_SPACE);
	//ֹͣ
	InsertKotKey("hk_stop", VK_CONTROL, 'S');
	//�˳�
	InsertKotKey("hk_quit", VK_CONTROL, 'W');
	//���
	InsertKotKey("hk_forward", 0, VK_RIGHT);
	//����
	InsertKotKey("hk_rewind", 0, VK_LEFT);
	//���30��
	InsertKotKey("hk_forward30", VK_CONTROL, VK_RIGHT);
	//����30��
	InsertKotKey("hk_rewind30", VK_CONTROL, VK_LEFT);
	//�������
	InsertKotKey("hk_upvolume", 0, VK_UP);
	//��������
	InsertKotKey("hk_dowmvolume", 0, VK_DOWN);
	//����
	InsertKotKey("hk_mute", VK_CONTROL, 'M');
	//ȫ���л�
	InsertKotKey("hk_full", VK_CONTROL, VK_RETURN);
	CLeDBInstance::GetInstance()->ThingEnd();
	return;
}