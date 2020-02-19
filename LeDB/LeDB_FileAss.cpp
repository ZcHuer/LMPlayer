#include "stdafx.h"
#include "LeDB_FileAss.h"
#include "sqlite3\sqlite3.h"
#include "LeDBInstance.h"
#include "../include/FileAssDefine.h"
#include "../SDK/Flog/FileLog.h"

CLeDB_FileAss::CLeDB_FileAss()
{

}

CLeDB_FileAss::~CLeDB_FileAss()
{

}

void CLeDB_FileAss::InitInsert()
{
	CLeDBInstance::GetInstance()->ThingBegin();
	int i = 0;
	while (AllExt[i].ext != nullptr)
	{
		InsertAssociation(AllExt[i].nExt, (char*)(AllExt[i].ext), 1);
		i++;
	}
	CLeDBInstance::GetInstance()->ThingEnd();
}

bool CLeDB_FileAss::InsertAssociation(int id, const char* pchName, int flag)
{
	sqlite3* pDB = CLeDBInstance::GetInstance()->GetSQL();
	if (NULL == pDB)
		return false;

	string strSql = "insert INTO FileAssociation(id,name,flag) values(";
	strSql.append(CLeDBInstance::Int2Str(id));
	strSql.append(",'");
	strSql.append(pchName);
	strSql.append("',");
	strSql.append(CLeDBInstance::Int2Str(flag));
	strSql.append(");");
	int nRes = sqlite3_exec(pDB, strSql.c_str(), 0, 0, NULL);
	if (nRes == SQLITE_OK)
		return true;

	return false;
}

bool CLeDB_FileAss::GetAssociation(int id)
{
	sqlite3* pDB = CLeDBInstance::GetInstance()->GetSQL();
	if (NULL == pDB)
		return false;

	bool bAss = false;	
	string strSql = "select flag from FileAssociation where id=";
	strSql.append(CLeDBInstance::Int2Str(id));
	string sData = "";
	char* cErrorMsg;
	int nRes = sqlite3_exec(pDB, strSql.c_str(), CLeDBInstance::OneResult, &sData, &cErrorMsg);
	if (nRes == SQLITE_OK) 
	{
		bAss = (0 == atoi(sData.c_str()))? false : true;
	}
	return bAss;
}

bool CLeDB_FileAss::GetAssociation(const char* pchName)
{
	sqlite3* pDB = CLeDBInstance::GetInstance()->GetSQL();
	if (NULL == pDB)
		return false;

	bool bAss = false;	
	string strSql = "select flag from FileAssociation where name='";
	strSql.append(pchName);
	strSql.append("';");

	string sData = "";
	char* cErrorMsg;
	int nRes = sqlite3_exec(pDB, strSql.c_str(), CLeDBInstance::OneResult, &sData, &cErrorMsg);
	if (nRes == SQLITE_OK) 
	{
		bAss = (0 == atoi(sData.c_str())) ? false : true;
	}
	return bAss;
}

char* CLeDB_FileAss::GetExtName(int id)
{
	sqlite3* pDB = CLeDBInstance::GetInstance()->GetSQL();
	if (NULL == pDB)
		return "";
	
	string strSql = "select name from FileAssociation where id=";
	strSql.append(CLeDBInstance::Int2Str(id));
	strSql.append(";");

	string sExtName = "";
	char* cErrorMsg;
	int nRes = sqlite3_exec(pDB, strSql.c_str(), CLeDBInstance::OneResult, &sExtName, &cErrorMsg);
	if (SQLITE_OK == nRes)
	{
		int nL = sExtName.length();
		char* pchChar = new char[nL + 1];
		memset(pchChar, 0, nL + 1);
		memcpy_s(pchChar, nL, sExtName.c_str(), nL);
		return pchChar;
	}
	return NULL;
}

void CLeDB_FileAss::DefaultAss(int flag)
{
	sqlite3* pDB = CLeDBInstance::GetInstance()->GetSQL();
	if (NULL == pDB)
		return;

	string strSql = "update FileAssociation set flag=";
	strSql.append(CLeDBInstance::Int2Str(flag));
	sqlite3_exec(pDB, strSql.c_str(), 0, 0, NULL);

	return;
}

void CLeDB_FileAss::SetAssociation(int id, int flag)
{
	sqlite3* pDB = CLeDBInstance::GetInstance()->GetSQL();
	if (NULL == pDB)
		return;

	string strSql = "update FileAssociation set flag=";
	strSql.append(CLeDBInstance::Int2Str(flag));
	strSql.append(" where id=");
	strSql.append(CLeDBInstance::Int2Str(id));
	strSql.append(";");

	string sData = "";
	char* cErrorMsg;
	int nRes = sqlite3_exec(pDB, strSql.c_str(), 0, 0, &cErrorMsg);

	return;
}

void CLeDB_FileAss::SetAssociation(const char* pchName, int flag)
{
	sqlite3* pDB = CLeDBInstance::GetInstance()->GetSQL();
	if (NULL == pDB)
		return;

	string strSql = "update FileAssociation set flag=";
	strSql.append(CLeDBInstance::Int2Str(flag));
	strSql.append(" where name='");
	strSql.append(pchName);
	strSql.append("';");

	string sData = "";
	char* cErrorMsg;
	int nRes = sqlite3_exec(pDB, strSql.c_str(), 0, 0, &cErrorMsg);

	return;
}

bool CLeDB_FileAss::CreateTable()
{
	sqlite3* pDB = CLeDBInstance::GetInstance()->GetSQL();
	if (NULL == pDB)
		return false;

	//文件关联
	string strSql = "create table FileAssociation(";
	strSql.append("id integer PRIMARY KEY,");
	strSql.append("name varchar unique,");
	strSql.append("flag integer);");
	char* cErrMsg;
	int nRes = sqlite3_exec(pDB, strSql.c_str(), 0, 0, &cErrMsg);
	if (SQLITE_OK == nRes)
	{
		//初始化插入数据
		InitInsert();
		return true;
	}

	if (cErrMsg&&StrStrA(cErrMsg, "already") == NULL) 
	{
		sqlite3_close(pDB);
		pDB = NULL;
		FLOG(L"Collection 表创建失败！");
	}

	return false;
}