#include "stdafx.h"
#include "Recorder.h"
#include <ShlObj.h>
#include <Shlwapi.h>
#pragma comment(lib,"Shlwapi.lib")

CRecorder* CRecorder::m_pInstance = NULL;
CRecorder::CRecorder()
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
 
 	m_strIni = szBuffer;
 	m_strIni += "\\Lenovo\\lmp\\SoftData";

	return;
}


CRecorder* CRecorder::GetInstance()
{
	if (NULL == m_pInstance)
	{
		m_pInstance = new CRecorder;
	}
	return m_pInstance;
}

void CRecorder::Release()
{
	if (NULL != m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

CRecorder::~CRecorder()
{
}

void CRecorder::SetTime(string strTime)
{
	WritePrivateProfileStringA("logtime", "time", strTime.c_str(), m_strIni.c_str());
	return;
}

int CRecorder::GetTime()
{
	return GetPrivateProfileIntA("logtime", "time", 0, m_strIni.c_str());
}

void CRecorder::AddCount(int nType, int nAdd)
{
	CHAR wcType1[16] = { 0 };
	_itoa_s(nType, wcType1, 16, 10);
	string strKeyName = wcType1;

	int nValue = GetPrivateProfileIntA("daily_count", strKeyName.c_str(), 0, m_strIni.c_str());
	nValue += nAdd;

	CHAR wcType2[16] = { 0 };
	_itoa_s(nValue, wcType2, 16, 10);
	string strValue = wcType2;
	WritePrivateProfileStringA("daily_count", strKeyName.c_str(), strValue.c_str(), m_strIni.c_str());
	return;
}

void CRecorder::EnumCount(map<int, int>& map_Count)
{
	CHAR strKeyNameTemp[2048];//对应每个AppName的所有KeyName的返回值
	CHAR strReturnTemp[1024];//返回值	

	int nKeyNameSize = GetPrivateProfileStringA("daily_count", NULL, NULL, strKeyNameTemp, 1024, m_strIni.c_str());
	if (nKeyNameSize > 0)
	{
		CHAR *pKeyName = new CHAR[nKeyNameSize];
		int nKeyNameLen = 0;    //每个KeyName的长度
		for (int j = 0; j < nKeyNameSize; j++)
		{
			pKeyName[nKeyNameLen++] = strKeyNameTemp[j];
			if (strKeyNameTemp[j] == '\0')
			{
				GetPrivateProfileStringA("daily_count", pKeyName, NULL, strReturnTemp, 1024, m_strIni.c_str());

				int nKey = atoi(pKeyName);
				int iTemp = atoi(strReturnTemp);

				map_Count.insert(make_pair(nKey, iTemp));
				memset(pKeyName, 0, nKeyNameSize);
				nKeyNameLen = 0;
			}
		}
		delete[]pKeyName;
	}

	return;
}

void CRecorder::SetBool(int nType, bool bValue)
{
	CHAR wcType1[16] = { 0 };
	_itoa_s(nType, wcType1, 16, 10);
	string strKeyName = wcType1;

	CHAR wcType2[16] = { 0 };
	_itoa_s((int)bValue, wcType2, 16, 10);
	string strValue = wcType2;

	WritePrivateProfileStringA("daily_bool", strKeyName.c_str(), strValue.c_str(), m_strIni.c_str());
	return;
}

void CRecorder::EnumBool(map<int, bool>& map_Bool)
{
	CHAR strKeyNameTemp[2048];//对应每个AppName的所有KeyName的返回值
	CHAR strReturnTemp[1024];//返回值	

	int nKeyNameSize = GetPrivateProfileStringA("daily_bool", NULL, NULL, strKeyNameTemp, 1024, m_strIni.c_str());
	if (nKeyNameSize > 0)
	{
		CHAR *pKeyName = new CHAR[nKeyNameSize];
		int nKeyNameLen = 0;    //每个KeyName的长度
		for (int j = 0; j < nKeyNameSize; j++)
		{
			pKeyName[nKeyNameLen++] = strKeyNameTemp[j];
			if (strKeyNameTemp[j] == '\0')
			{
				GetPrivateProfileStringA("daily_bool", pKeyName, NULL, strReturnTemp, 1024, m_strIni.c_str());

				int nKey = atoi(pKeyName);
				int iTemp = atoi(strReturnTemp);
				bool bTemp = (1 == iTemp) ? true : false;
				map_Bool.insert(make_pair(nKey, bTemp));
				memset(pKeyName, 0, nKeyNameSize);
				nKeyNameLen = 0;
			}
		}
		delete[]pKeyName;
	}

	return;
}

void CRecorder::AddString(int nType, const char* pchValue)
{
	CHAR wcType1[16] = { 0 };
	_itoa_s(nType, wcType1, 16, 10);
	string strKeyName = wcType1;

	CHAR wcType2[1024] = { 0 };
	GetPrivateProfileStringA("daily_string", strKeyName.c_str(), NULL, wcType2, 1024, m_strIni.c_str());
	string strTmp = wcType2;
	if (-1 != strTmp.find(pchValue))
		return;
	strTmp.append(",");
	strTmp.append(pchValue);

	WritePrivateProfileStringA("daily_string", strKeyName.c_str(), strTmp.c_str(), m_strIni.c_str());
	return;
}

void CRecorder::EnumString(map<int, string>& map_String)
{
	CHAR strKeyNameTemp[2048];//对应每个AppName的所有KeyName的返回值
	CHAR strReturnTemp[1024];//返回值	

	int nKeyNameSize = GetPrivateProfileStringA("daily_string", NULL, NULL, strKeyNameTemp, 1024, m_strIni.c_str());
	if (nKeyNameSize > 0)
	{
		CHAR *pKeyName = new CHAR[nKeyNameSize];
		int nKeyNameLen = 0;    //每个KeyName的长度
		for (int j = 0; j < nKeyNameSize; j++)
		{
			pKeyName[nKeyNameLen++] = strKeyNameTemp[j];
			if (strKeyNameTemp[j] == '\0')
			{
				GetPrivateProfileStringA("daily_string", pKeyName, NULL, strReturnTemp, 1024, m_strIni.c_str());
				int nKey = atoi(pKeyName);
				map_String.insert(make_pair(nKey, strReturnTemp));
				memset(pKeyName, 0, nKeyNameSize);
				nKeyNameLen = 0;
			}
		}
		delete[]pKeyName;
	}

	return;
}

void CRecorder::RemoveDailyFile()
{
	remove(m_strIni.c_str());
}
