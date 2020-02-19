#include "stdafx.h"
#include "LeDBInstance.h"
#include "sqlite3\sqlite3.h"
#include "../SDK/Flog/FileLog.h"
#include <stdlib.h>
#include <ShlObj.h>

#ifndef SAFE_DELETE
#define SAFE_DELETE(ptr) do{ if(ptr) {delete ptr; ptr=NULL;} }while(0)
#endif 
#ifndef SAFE_CREATELP
#define SAFE_CREATELP(a,b,c) a *b = new a[c + 1]; memset(b,0,sizeof(a) * (c + 1))
#endif

CLeDBInstance* CLeDBInstance::m_pInstance = NULL;

int CLeDBInstance::RecodResultEx(void * NotUsed, int argc, char ** argv, char ** azColName)
{
	vector<sNetVideoItem> *vec = (vector<sNetVideoItem>*)NotUsed;

	sNetVideoItem rec;
	rec.nType = atol(argv[0]);								// 片源类型									
	rec.conAlbumId = atol(argv[1]);							// 剧集ID									
	rec.lCurrPlayTime = atol(argv[2]);						// 当前播放时间
	rec.lTotalTime = atol(argv[3]);							// 总时长
	rec.orderNumber = atol(argv[4]);
	//rec.contentName = new char;
	//rec.contentName = CLeDBInstance::ConvertUtf8ToGBK(argv[5]).c_str();
	rec.time = atol(argv[6]);
	//rec.url = CLeDBInstance::ConvertUtf8ToGBK(argv[7]);
	//rec.remarks == CLeDBInstance::ConvertUtf8ToGBK(argv[8]);
	vec->push_back(rec);

	return 0;
}

int CLeDBInstance::LocalResult(void *NotUsed, int argc, char **argv, char **azColName)
{
	vector<sLocalFileItem*> *vec = (vector<sLocalFileItem*>*)NotUsed;
	sLocalFileItem* rec = new sLocalFileItem;	

	rec->sMd5 = CLeDBInstance::ConvertUtf8ToGBK(argv[0]);			// 播放地址md5
	rec->sPath = CLeDBInstance::ConvertUtf8ToGBK(argv[1]);			// 播放地址
	rec->sName = CLeDBInstance::ConvertUtf8ToGBK(argv[2]);			// 剧集名称
	rec->lCurrPlayTime = atol(argv[3]);								// 当前播放时间
	rec->lTotalTime = atol(argv[4]);								// 总时长	
	rec->bError = (0 == atoi(argv[5]))? false : true;				// 是否无效视频

	(*vec).push_back(rec);

	return 0;
}

int CLeDBInstance::RecodResult(void *NotUsed, int argc, char **argv, char **azColName)
{
	vector<sNetVideoItem*> *vec = (vector<sNetVideoItem*>*)NotUsed;
	sNetVideoItem* rec = new sNetVideoItem;
	rec->nType = atol(argv[0]);								// 片源类型		
	rec->scpid = atol(argv[1]);
	rec->conAlbumId = atol(argv[2]);							// 剧集ID									
	rec->lCurrPlayTime = atol(argv[3]);						// 当前播放时间
	rec->lTotalTime = atol(argv[4]);							// 总时长
	rec->orderNumber = atol(argv[5]);
	rec->contentName = CLeDBInstance::ConvertUtf8ToGBK(argv[6]);
	rec->time = atol(argv[7]);
	rec->url = CLeDBInstance::ConvertUtf8ToGBK(argv[8]);
	rec->remarks == CLeDBInstance::ConvertUtf8ToGBK(argv[9]);
	vec->push_back(rec);

	return 0;
}

int CLeDBInstance::PathResult(void *NotUsed, int argc, char **argv, char **azColName)
{
	BYTE* pPath = (BYTE*)NotUsed;
	char* p = argv[0];
	memcpy_s(pPath, strnlen_s(p, MAX_PATH), p, strnlen_s(p, MAX_PATH));
	return 0;
}

int CLeDBInstance::SearchResult(void *NotUsed, int argc, char **argv, char **azColName)
{
	vector<string> *vec = (vector<string>*)NotUsed;
	string name = CLeDBInstance::ConvertUtf8ToGBK(argv[0]);
	vec->push_back(name);
	return 0;
}

int CLeDBInstance::SingleSetResult(void *NotUsed, int argc, char **argv, char **azColName)
{
	string* sPath = (string*)NotUsed;
	*sPath = (string)argv[0];
	return 0;
}

int CLeDBInstance::ResultMd5s(void *NotUsed, int argc, char **argv, char **azColName)
{
	string* sMd5s = (string*)NotUsed;
	if (sMd5s->length()) {
		sMd5s->append(",");
	}
	sMd5s->append(argv[0]);
	return 0;
}

int CLeDBInstance::CurrTimeResult(void *NotUsed, int argc, char **argv, char **azColName)
{
	long* lCurrTime = (long*)NotUsed;
	*lCurrTime = atol(argv[0]);
	return 0;
}

int CLeDBInstance::HotkeyResult(void *NotUsed, int argc, char **argv, char **azColName)
{
	_HotKey* ht = (_HotKey*)NotUsed;
	switch (atol(argv[1]))
	{
	case VK_MENU:ht->m_bAlt = true; break;
	case VK_CONTROL:ht->m_bCtrl = true; break;
	case VK_SHIFT:ht->m_bShift = true; break;
	default:
		break;
	}
	ht->m_nKey = atol(argv[2]);
	return 0;
}

int CLeDBInstance::OneResult(void *NotUsed, int argc, char **argv, char **azColName)
{
	string* sData = (string*)NotUsed;
	*sData = argv[0];
	return 0;
}

string CLeDBInstance::Int2Str(long l)
{
	char cBuff[128] = { 0 };
	_ltoa_s(l, cBuff, 127, 10);
	return cBuff;
}

string CLeDBInstance::Gbk2Utf8(string str)
{
	if (str.length() == 0) 
		return "";
	string strOutUTF8 = "";
	WCHAR * str1;
	int n = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
	str1 = new WCHAR[n];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, str1, n);
	n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
	char * str2 = new char[n];
	WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);
	strOutUTF8 = str2;
	delete[]str1;
	str1 = NULL;
	delete[]str2;
	str2 = NULL;
	return strOutUTF8;
}

std::wstring CLeDBInstance::s2ws(string str)
{
	wstring ws = L"";
	if (!str.empty())
	{
		int nStrLen = str.length();
		SAFE_CREATELP(WCHAR, lpBuff, nStrLen);
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), nStrLen, lpBuff, nStrLen);
		ws = lpBuff;
		SAFE_DELETE(lpBuff);
	}
	return ws;
}

string CLeDBInstance::ConvertUtf8ToGBK(string strUtf8)
{
	if (strUtf8.length() == 0) 
		return "";
	int len = MultiByteToWideChar(CP_UTF8, 0, strUtf8.c_str(), -1, NULL, 0);
	wchar_t * wszGBK = new wchar_t[len];
	memset(wszGBK, 0, len);
	MultiByteToWideChar(CP_UTF8, 0, strUtf8.c_str(), -1, wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char *szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	string strOut = szGBK;
	delete[] szGBK;
	delete[] wszGBK;
	return strOut;
}

CLeDBInstance* CLeDBInstance::GetInstance()
{
	if (NULL == m_pInstance)
		m_pInstance = new CLeDBInstance;

	return m_pInstance;
}

void CLeDBInstance::Release()
{
	if (NULL != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

CLeDBInstance::~CLeDBInstance()
{
	if (NULL != m_pLocal)
	{
		delete m_pLocal;
		m_pLocal = NULL;
	}
	if (m_pNet)
	{
		delete m_pNet;
		m_pNet = NULL;
	}
	if (m_pSearch)
	{
		delete m_pSearch;
		m_pSearch = NULL;
	}
	if (m_pSet)
	{
		delete m_pSet;
		m_pSet = NULL;
	}
	if (m_pHotKey)
	{
		delete m_pHotKey;
		m_pHotKey = NULL;
	}
	if (m_pFileAss)
	{
		delete m_pFileAss;
		m_pFileAss = NULL;
	}
	if (m_pDB)
		sqlite3_close(m_pDB);
	m_pDB = NULL;
}

// 获取数据库保存路径
void GetDBPath(string &str)
{
	CHAR szBuffer[MAX_PATH] = { 0 };
	SHGetSpecialFolderPathA(NULL, szBuffer, CSIDL_APPDATA, FALSE);
	string tmp = szBuffer;
	tmp += "\\Lenovo\\lmp";
	if (!PathFileExistsA(tmp.c_str()))
	{
		string tmp_lenovo = szBuffer;
		tmp_lenovo += "\\Lenovo";
		CreateDirectoryA(tmp_lenovo.c_str(), NULL);
		tmp_lenovo += "\\lmp";
		CreateDirectoryA(tmp_lenovo.c_str(), NULL);
	}

	str = szBuffer;
	str += "\\Lenovo\\lmp\\lmpsql";
	return;
}

bool CLeDBInstance::InitSql()
{
	string strdb;
	GetDBPath(strdb);
	string path = CLeDBInstance::Gbk2Utf8(strdb);
	int nRes = sqlite3_open(path.c_str(), &m_pDB);
	if (nRes != SQLITE_OK)
		return false;
	return true;
}

bool CLeDBInstance::CreateTable()
{
	sqlite3* pDB = GetSQL();
	if (NULL == pDB)
		return false;

	// 本地播放列表
	if (Local())
		m_pLocal->CreateTable();
	//在线播放记录
	if (Net())
		m_pNet->CreateTable();
	//搜索历史
	if (Search())
		m_pSearch->CreateTable();	
	//设置记录
	if (Set())
	{
		m_pSet->CreateTable();
		m_pSet->InitSetTable();
	}		
	//快捷键设置
	if (HotKey())
	{
		m_pHotKey->CreateTable();
		m_pHotKey->InitHotKey();
	}
	//文件关联
	if (FileAss())
		m_pFileAss->CreateTable();

	return true;
}

bool CLeDBInstance::CreateIndex()
{
	sqlite3* pDB = GetSQL();
	if (NULL == pDB)
		return false;

	string strSql = "create index PlayRecodIndex on PlayRecod(playDay desc);";
	char* cErrMsg;
	int nRes = sqlite3_exec(pDB, strSql.c_str(), 0, 0, &cErrMsg);
	if (nRes != SQLITE_OK)
	{
		if (cErrMsg&&StrStrA(cErrMsg, "already") == NULL)
			return false;
	}

	strSql = "create index CollectionIndex on Collection(time desc);";
	nRes = sqlite3_exec(pDB, strSql.c_str(), 0, 0, &cErrMsg);
	if (nRes != SQLITE_OK)
	{
		if (cErrMsg&&StrStrA(cErrMsg, "already") == NULL)
			return FALSE;
	}
	return true;
}

bool CLeDBInstance::ClearDel()
{
	sqlite3* pDB = GetSQL();
	if (NULL == pDB)
		return false;

	string strSql = "delete from Collection where delFlag=1";
	char* cErrMsg;
	int res = sqlite3_exec(pDB, strSql.c_str(), 0, 0, &cErrMsg);
	strSql = "delete from PlayRecod where delFlag=1";
	res = sqlite3_exec(pDB, strSql.c_str(), 0, 0, &cErrMsg);

	return true;
}

void CLeDBInstance::ThingBegin()
{
	sqlite3* pDB = GetSQL();
	if (NULL == pDB)
		return;

	sqlite3_exec(pDB, "begin;", 0, 0, 0);
}

void CLeDBInstance::ThingEnd()
{
	sqlite3* pDB = GetSQL();
	if (NULL == pDB)
		return;

	sqlite3_exec(pDB, "commit;", 0, 0, 0);
}

ILeDB_Local* CLeDBInstance::Local()
{
	if (NULL == m_pLocal)
		m_pLocal = new CLeDB_Local;
	return m_pLocal;
}

ILeDB_Net* CLeDBInstance::Net()
{
	if (NULL == m_pNet)
		m_pNet = new CLeDB_Net;
	return m_pNet;
	
}

ILeDB_Search* CLeDBInstance::Search()
{
	if (NULL == m_pSearch)
		m_pSearch = new CLeDB_Search;
	return m_pSearch;
}

ILeDB_Set* CLeDBInstance::Set()
{
	if (NULL == m_pSet)
		m_pSet = new CLeDB_Set;
	return m_pSet;
}

ILeDB_HotKey* CLeDBInstance::HotKey()
{
	if (NULL == m_pHotKey)
		m_pHotKey = new CLeDB_HotKey;
	return m_pHotKey;
}

ILeDB_FileAss* CLeDBInstance::FileAss()
{
	if (NULL == m_pFileAss)
		m_pFileAss = new CLeDB_FileAss;
	return m_pFileAss;
}

sqlite3* CLeDBInstance::GetSQL()
{
	return m_pDB;
}

CLeDBInstance::CLeDBInstance(void)
{
	m_pDB = NULL;
	m_pLocal = NULL;
	m_pNet = NULL;
	m_pSearch = NULL;
	m_pSet = NULL;
	m_pHotKey = NULL;
	m_pFileAss = NULL;
}

