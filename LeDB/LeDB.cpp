// LeDB.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "LeDBInstance.h"
#include "../SDK/Flog/FileLog.h"

CLeDB* CLeDB::m_pInstance = NULL;
CLeDB* CLeDB::GetInstance()
{
	if (NULL == m_pInstance)
		m_pInstance = new CLeDB;

	return m_pInstance;
}

void CLeDB::Release()
{
	CLeDBInstance::GetInstance()->Release();
	if (NULL != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

CLeDB::~CLeDB()
{

}

bool CLeDB::InitSql()
{
	return CLeDBInstance::GetInstance()->InitSql();
}

bool CLeDB::CreateTable()
{
	return CLeDBInstance::GetInstance()->CreateTable();
}

bool CLeDB::CreateIndex()
{	
	return CLeDBInstance::GetInstance()->CreateIndex();
}

bool CLeDB::ClearDel()
{
	return CLeDBInstance::GetInstance()->ClearDel();
}

void CLeDB::ThingBegin()
{
	return CLeDBInstance::GetInstance()->ThingBegin();
}

void CLeDB::ThingEnd()
{
	return CLeDBInstance::GetInstance()->ThingEnd();
}

void CLeDB::ReleaseStr(char* pch)
{
	delete pch;
	pch = NULL;
	return;
}

void CLeDB::ReleaseVecLocal(void* pVoid_Local)
{
	vector<sLocalFileItem*>* pVec_Local = (vector<sLocalFileItem*>*)pVoid_Local;
	for (vector<sLocalFileItem*>::iterator it = pVec_Local->begin(); it != pVec_Local->end(); it++)
	{
		sLocalFileItem* pinfo = *it;
		delete pinfo;
		pinfo = NULL;
	}
	delete pVoid_Local;
	pVoid_Local = NULL;
	pVoid_Local = NULL;
	return;
}

void CLeDB::ReleaseVecNet(void* pVoid_Net)
{
	vector<sNetVideoItem*>* pVec_Net = (vector<sNetVideoItem*>*)pVoid_Net;
	for (vector<sNetVideoItem*>::iterator it = pVec_Net->begin(); it != pVec_Net->end(); it++)
	{
		sNetVideoItem* pinfo = *it;
		delete pinfo;
		pinfo = NULL;
	}
	delete pVec_Net;
	pVec_Net = NULL;
	pVoid_Net = NULL;
	return;
}

void CLeDB::ReleaseVecString(void* pVoid_String)
{
	vector<string>* pVec_String = (vector<string>*)pVoid_String;
	delete pVec_String;
	pVec_String = NULL;
	pVoid_String = NULL;
	return;
}

ILeDB_Local* CLeDB::Local()
{
	return CLeDBInstance::GetInstance()->Local();
}

ILeDB_Net* CLeDB::Net()
{
	return CLeDBInstance::GetInstance()->Net();
}

ILeDB_Search* CLeDB::Search()
{
	return CLeDBInstance::GetInstance()->Search();
}

ILeDB_Set* CLeDB::Set()
{
	return CLeDBInstance::GetInstance()->Set();
}

ILeDB_HotKey* CLeDB::HotKey()
{
	return CLeDBInstance::GetInstance()->HotKey();
}

ILeDB_FileAss* CLeDB::FileAss()
{
	return CLeDBInstance::GetInstance()->FileAss();
}

// 这是已导出类的构造函数。
// 有关类定义的信息，请参阅 LeDB.h
CLeDB::CLeDB()
{
}
