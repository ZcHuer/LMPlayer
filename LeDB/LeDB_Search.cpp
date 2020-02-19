#include "stdafx.h"
#include "LeDB_Search.h"
#include "../SDK/Flog/FileLog.h"
#include "sqlite3\sqlite3.h"
#include "LeDBInstance.h"

CLeDB_Search::CLeDB_Search()
{

}

CLeDB_Search::~CLeDB_Search()
{

}

bool CLeDB_Search::InsertSearch(const char* pchName)
{
	sqlite3* pDB = CLeDBInstance::GetInstance()->GetSQL();
	if (NULL == pDB)
		return false;

	string strSql = "insert into SearchHistory(name)";
	strSql.append(" values('");
	strSql.append(CLeDBInstance::Gbk2Utf8(pchName));
	strSql.append("');");

	char* cErrMsg;
	int nRes = sqlite3_exec(pDB, strSql.c_str(), 0, 0, &cErrMsg);
	if (nRes != SQLITE_OK)
	{
		if (cErrMsg) {
			string sErr = "SearchHistory insert error: ";
			sErr.append(cErrMsg);
			FLOG(CLeDBInstance::s2ws(sErr).c_str());
		}
		return false;
	}
	return true;
}

bool CLeDB_Search::SelectSearch(void** pVoid_String)
{
	sqlite3* pDB = CLeDBInstance::GetInstance()->GetSQL();
	if (NULL == pDB)
		return false;

	string strSql = "select * from SearchHistory ORDER by time desc limit 5;";
	char* cErrMsg;
	vector<string>* pVec_String = new vector<string>;
	int res = sqlite3_exec(pDB, strSql.c_str(), CLeDBInstance::SearchResult, pVec_String, &cErrMsg);
	if (res != SQLITE_OK)
		return false;
	*pVoid_String = pVec_String;
	return true;
}

bool CLeDB_Search::DelSearch(const char* pchName)
{
	sqlite3* pDB = CLeDBInstance::GetInstance()->GetSQL();
	if (NULL == pDB)
		return false;

	string strSql = "delete from SearchHistory where name='";
	strSql.append(CLeDBInstance::Gbk2Utf8(pchName));
	strSql.append("';");

	char* cErrMsg;
	int res = sqlite3_exec(pDB, strSql.c_str(), 0, 0, &cErrMsg);
	if (res != SQLITE_OK)
		return false;
	return true;
}

bool CLeDB_Search::DelAllSearch()
{
	sqlite3* pDB = CLeDBInstance::GetInstance()->GetSQL();
	if (NULL == pDB)
		return false;

	string strSql = "delete from SearchHistory;";

	char* cErrMsg;
	int res = sqlite3_exec(pDB, strSql.c_str(), 0, 0, &cErrMsg);
	if (res != SQLITE_OK)
		return false;
	return true;
}

bool CLeDB_Search::CreateTable()
{
	sqlite3* pDB = CLeDBInstance::GetInstance()->GetSQL();
	if (NULL == pDB)
		return false;

	string strSql = "create table SearchHistory(";
	strSql.append("name varchar primary key,");
	strSql.append("time TimeStamp NOT NULL DEFAULT (datetime('now','localtime')));");
	char* cErrMsg4;
	int nRes = sqlite3_exec(pDB, strSql.c_str(), 0, 0, &cErrMsg4);
	if (SQLITE_OK == nRes)
		return true;

	if (cErrMsg4&&StrStrA(cErrMsg4, "already") == NULL)
	{
		sqlite3_close(pDB);
		pDB = NULL;
		FLOG(L"SearchHistory ±í´´½¨Ê§°Ü£¡");
	}
	return false;
}