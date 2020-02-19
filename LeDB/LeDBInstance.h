#pragma once
#include "LeDB.h"
#include "LeDB_Local.h"
#include "LeDB_Net.h"
#include "LeDB_Search.h"
#include "LeDB_Set.h"
#include "LeDB_HotKey.h"
#include "LeDB_FileAss.h"
#include <string>
#include <vector>
#include "sqlite3\sqlite3.h"
using namespace std;

class CLeDBInstance
{
public:
	static CLeDBInstance* GetInstance();
	static void Release();
	~CLeDBInstance();

	bool InitSql();
	bool CreateTable();
	bool CreateIndex();
	bool ClearDel();//清理已删除记录
	void ThingBegin();
	void ThingEnd();

	ILeDB_Local* Local();
	ILeDB_Net* Net();
	ILeDB_Search* Search();
	ILeDB_Set* Set();
	ILeDB_HotKey* HotKey();
	ILeDB_FileAss* FileAss();
	sqlite3* GetSQL();
	static int RecodResultEx(void * NotUsed, int argc, char ** argv, char ** azColName);
	static int LocalResult(void *NotUsed, int argc, char **argv, char **azColName);
	static int RecodResult(void *NotUsed, int argc, char **argv, char **azColName);
	//static int CollectResult(void *NotUsed, int argc, char **argv, char **azColName);
	static int PathResult(void *NotUsed, int argc, char **argv, char **azColName);
	static int SearchResult(void *NotUsed, int argc, char **argv, char **azColName);
	static int SingleSetResult(void *NotUsed, int argc, char **argv, char **azColName);
	static int ResultMd5s(void *NotUsed, int argc, char **argv, char **azColName);
	static int CurrTimeResult(void *NotUsed, int argc, char **argv, char **azColName);
	static int HotkeyResult(void *NotUsed, int argc, char **argv, char **azColName);
	static int OneResult(void *NotUsed, int argc, char **argv, char **azColName);
	static string Int2Str(long l);
	static string Gbk2Utf8(string str);
	static wstring s2ws(string str);
	static string ConvertUtf8ToGBK(string strUtf8);
	sqlite3* m_pDB;

private:
	CLeDBInstance(void);
	static CLeDBInstance* m_pInstance;
	CLeDB_Local* m_pLocal;
	CLeDB_Net* m_pNet;
	CLeDB_Search* m_pSearch;
	CLeDB_Set* m_pSet;
	CLeDB_HotKey* m_pHotKey;
	CLeDB_FileAss* m_pFileAss;
};