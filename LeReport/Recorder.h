#pragma once
#include <map>
using namespace std;

class CRecorder
{
public:
	static CRecorder* GetInstance();
	static void Release();
	~CRecorder();	
	
	void SetTime(string strTime);
	int GetTime();
	void AddCount(int nType, int nAdd);
	void EnumCount(map<int, int>& map_Count);
	void SetBool(int nType, bool bValue);
	void EnumBool(map<int, bool>& map_Bool);
	void AddString(int nType, const char* pchValue);
	void EnumString(map<int, string>& map_String);

	void RemoveDailyFile();
	
private:
	CRecorder();
	static CRecorder* m_pInstance;
	string m_strIni;

private:
	void EnumIniFile(LPCWSTR strFilePath, LPCWSTR strAppName, map<int, int>& map_Key);
};

