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
		FLOG(L"LocalSet ����ʧ�ܣ�");
	}
	return false;
}

bool CLeDB_Set::InitSetTable()
{
	CLeDBInstance::GetInstance()->ThingBegin();
	//����
	InitSetItem("passWord", "");
	//�û���
	InitSetItem("userName", "");
	//�Զ���¼
	InitSetItem("autoLogin", "1");
	InitSetItem("remPWD", "1");
	//�ر�ʱ 0:��С�� 1:�ر� Ĭ�ϣ�0
	InitSetItem("quit", "0");
	//��Ƶ���ڷ�� 0��������Ӧ��Ƶ 1����Ƶ��Ӧ���� 2���Զ�ȫ�� Ĭ�ϣ�1
	InitSetItem("wndStyle", "1");
	//�����ͣ���� 0������ͣ 1����ͣ Ĭ�ϣ�1
	InitSetItem("clickPause", "1");
	//��Ƶ��С��ʱ�Ƿ���ͣ 0������ͣ 1����ͣ Ĭ�ϣ�1
	InitSetItem("minPause", "1");
	//����ٶ�
	InitSetItem("forwardSpeed", "10");
	//�����ٶ�
	InitSetItem("rewindSpeed", "10");
	//��Ļ�� 0:ԭʼ 1��16:9 2��4:3  3������ Ĭ��ԭʼ 
	InitSetItem("screenRatio", "0");
	//���� 0:�������� 1��������  2�������� 3����ת�������� 4�����һ������
	InitSetItem("channel", "0");
	//�˳�ʱ��ղ��ż�¼
	InitSetItem("quitClear", "0");
	//�޺۲���ģʽ
	InitSetItem("noMark", "0");
	//����ģʽ 0���ֶ����� 1���Զ�����
	InitSetItem("autoUpdate", "0");

	//����˳�� 0��˳�� 1����� 2������ 3������ѭ�� 4���б�ѭ��
	InitSetItem("playOrder", "0");
	//�Ƿ��ö���ʾ,0���Ӳ� 1��ʼ�� 2������ʱ
	InitSetItem("topmost", "0");
	//����
	InitSetItem("volume", "30");
	InitSetItem("auto", "0");
	CLeDBInstance::GetInstance()->ThingEnd();

	return true;
}