#include "stdafx.h"
#include "LeDB_Net.h"
#include "sqlite3\sqlite3.h"
#include "LeDBInstance.h"
#include "../SDK/Flog/FileLog.h"

CLeDB_Net::CLeDB_Net()
{

}

CLeDB_Net::~CLeDB_Net()
{

}

bool CLeDB_Net::InsertPlayRecodEx(int nType, long lcpid, long conAlbumId, long lCurrPlayTime, long lTotalTime, long orderNumber, const char* pchContentName, long time, const char* pchUrl/* = ""*/, const char* pchRemarks/* = ""*/)
{
	sqlite3* pDB = CLeDBInstance::GetInstance()->GetSQL();
	if (NULL == pDB)
		return false;

	string strSql = "insert into NetVideoRecordEx(nType,lcpid, conAlbumId, lCurrPlayTime, lTotalTime, orderNumber, contentName, url, remarks, time)";
	strSql += " values(";
	strSql += CLeDBInstance::Int2Str(nType) + ",";
	strSql += CLeDBInstance::Int2Str(lcpid) + ",";
	strSql += CLeDBInstance::Int2Str(conAlbumId) + ",";
	strSql += CLeDBInstance::Int2Str(lCurrPlayTime) + ",";
	strSql += CLeDBInstance::Int2Str(lTotalTime) + ",";
	strSql += CLeDBInstance::Int2Str(orderNumber) + ",";
	strSql += "'";
	strSql.append(pchContentName);
	strSql += "',";
	strSql += "'";
	strSql.append(pchUrl);
	strSql += "',";
	strSql += "'";
	strSql.append(pchRemarks);
	strSql += "',";
	strSql += CLeDBInstance::Int2Str(time) + ")";

	char* cErrMsg;
	int nRes = sqlite3_exec(pDB, strSql.c_str(), 0, 0, &cErrMsg);
	if (nRes != SQLITE_OK)
	{
		//¼ÇÂ¼ÒÑ´æÔÚ
		if (cErrMsg && _stricmp(cErrMsg, "UNIQUE constraint failed: NetVideoRecordEx.conAlbumId") == 0)
		{
			if (0 == lCurrPlayTime)
				return true;
			return UpdatePlayRecodEx(conAlbumId, lCurrPlayTime, lTotalTime, orderNumber, pchContentName, pchUrl, pchRemarks);
		}
		if (cErrMsg)
		{
			string sErr = "NetVideoRecordEx insert error: ";
			sErr.append(cErrMsg);
			FLOG(CLeDBInstance::s2ws(sErr).c_str());
			return false;
		}
	}
	return true;
}

bool CLeDB_Net::UpdatePlayRecodEx(long conAlbumId, long lCurrPlayTime, long lTotalTime, long orderNumber, const char* pchContentName, const char* pchUrl/* = ""*/, const char* pchRemarks/* = ""*/)
{
	sqlite3* pDB = CLeDBInstance::GetInstance()->GetSQL();
	if (NULL == pDB)
		return false;

	string strSql = "update NetVideoRecordEx set ";
	strSql += "lCurrPlayTime=";
	strSql += CLeDBInstance::Int2Str(lCurrPlayTime) + ",";
	strSql += "lTotalTime=";
	strSql += CLeDBInstance::Int2Str(lTotalTime) + ",";
	strSql += "orderNumber=";
	strSql += CLeDBInstance::Int2Str(orderNumber) + ",";
	strSql += "contentName=";
	strSql += "'";
	strSql.append(pchContentName);
	strSql += "'";	
	strSql += "url=";
	strSql += "'";
	strSql.append(pchUrl);
	strSql += "'";
	strSql += "remarks=";
	strSql += "'";
	strSql.append(pchRemarks);
	strSql += "'";	
	strSql += " where conAlbumId='" + CLeDBInstance::Int2Str(conAlbumId) + "';";

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

bool CLeDB_Net::SelectPlayRecodEx(void** pVoid_Net, bool bLimit /*= false*/)
{
	sqlite3* pDB = CLeDBInstance::GetInstance()->GetSQL();
	if (NULL == pDB)
		return false;

	string strSql = "select * from NetVideoRecordEx where lCurrPlayTime>=0 ORDER by playDay desc limit 10;";
	if (!bLimit)
		strSql = "select * from NetVideoRecordEx where lCurrPlayTime>=0 ORDER by playDay desc;";
	char* cErrMsg;
	vector<sNetVideoItem*>* pVec_Net = new vector<sNetVideoItem *>;
	int res = sqlite3_exec(pDB, strSql.c_str(), CLeDBInstance::RecodResult, pVec_Net, &cErrMsg);
	if (res != SQLITE_OK)
		return false;
	*pVoid_Net = pVec_Net;
	return true;
}

bool CLeDB_Net::DelPlayRecodEx(long conAlbumId)
{
	sqlite3* pDB = CLeDBInstance::GetInstance()->GetSQL();
	if (NULL == pDB)
		return false;

	string strSql = "delete from NetVideoRecordEx where conAlbumId=";
	strSql += CLeDBInstance::Int2Str(conAlbumId);
	strSql += ";";
	//Âß¼­ É¾³ýÍøÂçÊÓÆµ
	char* cErrMsg;
	int res = sqlite3_exec(pDB, strSql.c_str(), 0, 0, &cErrMsg);
	if (res != SQLITE_OK)
		return false;
	return true;
}

long CLeDB_Net::GetPlayProcess_ById_NetVideoEx(long conAlbumId, int nindex)
{
	sqlite3* pDB = CLeDBInstance::GetInstance()->GetSQL();
	if (NULL == pDB)
		return 0;

	long lCurrTime = 0;
	string strSql = "select lCurrPlayTime from NetVideoRecordEx where conAlbumId='";
	string strconAlbumId = CLeDBInstance::Int2Str(conAlbumId);
	strSql.append(strconAlbumId);
	strSql.append("'and orderNumber='");
	string sindex = CLeDBInstance::Int2Str(nindex);
	strSql.append(sindex);
	strSql.append("';");
	char* cErrMsg;
	int res = sqlite3_exec(pDB, strSql.c_str(), CLeDBInstance::CurrTimeResult, &lCurrTime, &cErrMsg);
	if (res != SQLITE_OK)
		return lCurrTime;
	return lCurrTime;
}

bool CLeDB_Net::CreateTable()
{
	sqlite3* pDB = CLeDBInstance::GetInstance()->GetSQL();
	if (NULL == pDB)
		return false;
	
	string strSql = "create table NetVideoRecordEX(";
	strSql.append("nType integer,");
	strSql.append("lcpid integer,");
	strSql.append("conAlbumId integer unique,");
	strSql.append("lCurrPlayTime integer,");
	strSql.append("lTotalTime integer,");
	strSql.append("orderNumber integer,");
	strSql.append("contentName varchar,");
	strSql.append("time integer,");
	strSql.append("url varchar,");
	strSql.append("remarks varchar,");
	strSql.append("playDay TimeStamp NOT NULL DEFAULT (datetime('now','localtime')));");

	char* cErrMsg2;
	int nRes = sqlite3_exec(pDB, strSql.c_str(), 0, 0, &cErrMsg2);
	if (SQLITE_OK == nRes)
		return true;

	if (cErrMsg2&&StrStrA(cErrMsg2, "already") == NULL)
	{
		sqlite3_close(pDB);
		pDB = NULL;
		FLOG(L"NetVideoRecordEX ±í´´½¨Ê§°Ü£¡");
	}
	return false;
}