#include "stdafx.h"
#include "LeDB_Local.h"
#include "sqlite3\sqlite3.h"
#include "../SDK/Flog/FileLog.h"
#include "LeDBInstance.h"

CLeDB_Local::CLeDB_Local()
{

}

CLeDB_Local::~CLeDB_Local()
{

}

bool CLeDB_Local::InsertLocalFileList(const char* pchMd5, const char* pchPath, const char* pchName, long currPlayTime, long totalTime, long bError)
{
	sqlite3* pDB = CLeDBInstance::GetInstance()->GetSQL();
	if (NULL == pDB)
		return false;

	string strSql = "insert into LocalFileList(md5,path,name,currPlayTime,totalTime,isError)";
	strSql += " values(";
	strSql += "'";
	strSql.append(CLeDBInstance::Gbk2Utf8(pchMd5));
	strSql += "',";
	strSql += "'";
	strSql.append(CLeDBInstance::Gbk2Utf8(pchPath));
	strSql += "',";
	strSql += "'";
	strSql.append(CLeDBInstance::Gbk2Utf8(pchName));
	strSql += "',";	
	strSql += CLeDBInstance::Int2Str(currPlayTime) + ",";
	strSql += CLeDBInstance::Int2Str(totalTime) + ",";
	strSql += CLeDBInstance::Int2Str(bError) + ");";

	char* cErrMsg;
	int nRes = sqlite3_exec(pDB, strSql.c_str(), 0, 0, &cErrMsg);
	if (nRes != SQLITE_OK)
	{
		//记录已存在
		if (cErrMsg && _stricmp(cErrMsg, "UNIQUE constraint failed: LocalFileList.md5") == 0)
		{
			if (0 == currPlayTime)
				return true;
			return UpdateLocalFileList(pchMd5, currPlayTime, totalTime, bError);
		}
		if (cErrMsg)
		{
			string sErr = "LocalFileList insert error: ";
			sErr.append(cErrMsg);
			FLOG(CLeDBInstance::s2ws(sErr).c_str());
			return false;
		}
	}
	return true;
}

bool CLeDB_Local::UpdateLocalFileList(const char* pchMd5, long currPlayTime, long totalTime, long bError)
{
	sqlite3* pDB = CLeDBInstance::GetInstance()->GetSQL();
	if (NULL == pDB)
		return false;

	string strSql = "update LocalFileList set ";
	strSql += "currPlayTime=";
	strSql += CLeDBInstance::Int2Str(currPlayTime) + ",";
	strSql += "totalTime=";
	strSql += CLeDBInstance::Int2Str(totalTime) + ",";
	strSql += "isError=";
	strSql += CLeDBInstance::Int2Str(bError);
	strSql += " where md5='";
	strSql.append(pchMd5);
	strSql += "';";

	char* cErrMsg;
	int nRes = sqlite3_exec(pDB, strSql.c_str(), 0, 0, &cErrMsg);
	if (nRes != SQLITE_OK)
	{
		if (cErrMsg)
			FLOG(CLeDBInstance::s2ws(cErrMsg).c_str());
		return false;
	}
	return true;
}

bool CLeDB_Local::SelectLocalFileList(void** pVoid_Local, bool bLimit /*= false*/)
{
	sqlite3* pDB = CLeDBInstance::GetInstance()->GetSQL();
	if (NULL == pDB)
		return false;

	string strSql = "select * from LocalFileList";
	char* cErrMsg;
	vector<sLocalFileItem*>* pVec_Local = new vector<sLocalFileItem *>;
	int res = sqlite3_exec(pDB, strSql.c_str(), CLeDBInstance::LocalResult, pVec_Local, &cErrMsg);
	if (res != SQLITE_OK)
		return false;
	*pVoid_Local = pVec_Local;
	return true;
}

char* CLeDB_Local::SelectPath(const char* pchMd5)
{
	sqlite3* pDB = CLeDBInstance::GetInstance()->GetSQL();
	if (NULL == pDB)
		return "";

	string strSql = "select path from LocalFileList where md5='";
	strSql.append(pchMd5);
	strSql.append("';");
	char* pPath = new char[MAX_PATH];
	memset(pPath, 0, MAX_PATH);
	char* cErrMsg;
	int res = sqlite3_exec(pDB, strSql.c_str(), CLeDBInstance::PathResult, pPath, &cErrMsg);
	if (res != SQLITE_OK)
		return "";
	return pPath;
}

long CLeDB_Local::GetPlayProcess_ByMd5_Local(const char* pchMd5)
{
	sqlite3* pDB = CLeDBInstance::GetInstance()->GetSQL();
	if (NULL == pDB)
		return 0;

	long lCurrTime = 0;
	string strSql = "select currPlayTime from LocalFileList where md5='";
	strSql.append(pchMd5);
	strSql.append("';");
	char* cErrMsg;
	int res = sqlite3_exec(pDB, strSql.c_str(), CLeDBInstance::CurrTimeResult, &lCurrTime, &cErrMsg);
	if (res != SQLITE_OK)
		return lCurrTime;
	return lCurrTime;
}

bool CLeDB_Local::DelLocalFileItem(const char* pchMd5)
{
	sqlite3* pDB = CLeDBInstance::GetInstance()->GetSQL();
	if (NULL == pDB)
		return false;

	string strSql = "delete from LocalFileList where md5='";
	strSql.append(pchMd5);
	strSql.append("';");

	//逻辑 删除本地
	char* cErrMsg;
	int res = sqlite3_exec(pDB, strSql.c_str(), 0, 0, &cErrMsg);
	if (res != SQLITE_OK)
		return false;
	return true;
}

bool CLeDB_Local::DelLocalFileList()
{
	sqlite3* pDB = CLeDBInstance::GetInstance()->GetSQL();
	if (NULL == pDB)
		return false;

	string strSql = "delete from LocalFileList;";

	//逻辑 删除本地
	char* cErrMsg;
	int res = sqlite3_exec(pDB, strSql.c_str(), 0, 0, &cErrMsg);
	if (res != SQLITE_OK)
		return false;
	return true;
}

bool CLeDB_Local::CreateTable()
{
	sqlite3* pDB = CLeDBInstance::GetInstance()->GetSQL();
	if (NULL == pDB)
		return false;

	string strSql = "create table LocalFileList(";
	strSql.append("md5 varchar unique,");
	strSql.append("path varchar,");
	strSql.append("name varchar,");
	strSql.append("currPlayTime integer,");
	strSql.append("totalTime integer,");
	strSql.append("isError char);");

	char* cErrMsg1;
	int nRes = sqlite3_exec(pDB, strSql.c_str(), 0, 0, &cErrMsg1);
	if (SQLITE_OK == nRes)
		return true;

	if (cErrMsg1&&StrStrA(cErrMsg1, "already") == NULL)
	{
		sqlite3_close(pDB);
		pDB = NULL;
		FLOG(L"LocalFileList 表创建失败！");
	}
	return false;
}