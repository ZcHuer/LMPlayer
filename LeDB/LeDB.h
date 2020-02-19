// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� LEDB_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// LEDB_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#pragma once
#ifdef LEDB_EXPORTS
#define LEDB_API __declspec(dllexport)
#else
#define LEDB_API __declspec(dllimport)
#endif
#include <string>

using namespace std;

// ���ز����б�ڵ�
struct sLocalFileItem
{
	string sMd5;
	string sPath;
	string sName;
	long lCurrPlayTime;
	long lTotalTime;
	bool bError;
};
// ���ز����б�ӿ���
class ILeDB_Local
{
public:
	virtual bool InsertLocalFileList(const char* pchMd5, const char* pchPath, const char* pchName, long currPlayTime, long totalTime, long bError) = 0;
	virtual bool UpdateLocalFileList(const char* pchMd5, long currPlayTime, long totalTime, long bError) = 0;
	virtual bool SelectLocalFileList(void** pVoid_Local, bool bLimit = false) = 0;
	virtual char* SelectPath(const char* pchMd5) = 0;
	virtual long GetPlayProcess_ByMd5_Local(const char* pchMd5) = 0;
	virtual bool DelLocalFileItem(const char* pchMd5) = 0;
	virtual bool DelLocalFileList() = 0;
};
// ������Ƶ���ż�¼�ڵ�
struct sNetVideoItem
{
	int nType;					// ƬԴ����
	long scpid;					//
	long conAlbumId;			// �缯ID	
	long lCurrPlayTime;			// ��ǰ����ʱ��								
	long lTotalTime;			// ��ʱ��
	long orderNumber;			// �缯���										
	string contentName;			// �Ӿ缯����
	long time;					// ����ʱ�� 
	string url;					//����URL
	string remarks;				//��չ�ַ���remarks
};
// ������Ƶ��¼�ӿ���
class ILeDB_Net
{
public:
	virtual bool InsertPlayRecodEx(int nType, long lcpid, long conAlbumId, long lCurrPlayTime, long lTotalTime, long orderNumber, const char* pchContentName, long time, const char* pchUrl = "", const char* pchRemarks = "") = 0;
	virtual bool UpdatePlayRecodEx(long conAlbumId, long lCurrPlayTime, long lTotalTime, long orderNumber, const char* pchContentName, const char* pchUrl = "", const char* pchRemarks = "") = 0;
	virtual bool SelectPlayRecodEx(void** pVoid_Net, bool bLimit = false) = 0;
	virtual bool DelPlayRecodEx(long conAlbumId) = 0;
	virtual long GetPlayProcess_ById_NetVideoEx(long conAlbumId, int nindex) = 0;
};
// ������ʷ�ӿ���
class ILeDB_Search
{
public:
	virtual bool InsertSearch(const char* pchName) = 0;
	virtual bool SelectSearch(void** pVoid_String) = 0;
	virtual bool DelSearch(const char* pchName) = 0;
	virtual bool DelAllSearch() = 0;
};
// ���ýӿ���
class ILeDB_Set
{
public:
	virtual void InitSetItem(const char* pchKey, const char* pchValue) = 0;
	virtual bool UpdateSingleSet(const char* pchKey, const char* pchValue) = 0;
	virtual char* GetSingleSet(const char* pchKey) = 0;
};
// �ȼ��ṹ��
struct _HotKey
{
	bool m_bCtrl;
	bool m_bAlt;
	bool m_bShift;
	unsigned int m_nKey;
	_HotKey()
	{
		m_bCtrl = false;
		m_bAlt = false;
		m_bShift = false;
		m_nKey = -1;
	}
	void Init()
	{
		m_bCtrl = false;
		m_bAlt = false;
		m_bShift = false;
		m_nKey = -1;
	}

	void SetKey(bool bCtrl, bool bAlt, bool bShift, unsigned int nKey)
	{
		m_bCtrl = bCtrl;
		m_bAlt = bAlt;
		m_bShift = bShift;
		m_nKey = nKey;
	}

	bool IsSame(_HotKey hot)
	{
		if (hot.m_bCtrl == m_bCtrl&&hot.m_bAlt == m_bAlt&&hot.m_bShift == m_bShift&&hot.m_nKey == m_nKey)
		{
			return true;
		}
		return false;
	}

	void _Copy(_HotKey hk)
	{
		m_bCtrl = hk.m_bCtrl;
		m_bAlt = hk.m_bAlt;
		m_bShift = hk.m_bShift;
		m_nKey = hk.m_nKey;
	}

	bool IsHotKeyDown(bool bCtrl, bool bAlt, bool bShift, unsigned int nKey)
	{
		if (m_bCtrl == bCtrl
			&&m_bAlt == bAlt
			&&m_bShift == bShift
			&&m_nKey == nKey)
		{
			return true;
		}
		return false;
	}

	wstring GetWString()
	{
		wstring wsKey = L"";
		if (48 <= m_nKey && 90 >= m_nKey)
		{
			wchar_t wBuff[64] = { 0 };
			wBuff[0] = m_nKey;
			_wcsupr_s(wBuff,2);
			_wcsupr_s(wBuff, 2);
			wsKey = wBuff;
		}
		if (96 <= m_nKey && 105 >= m_nKey)
		{
			wchar_t wBuff[64] = { 0 };
			wBuff[0] = m_nKey - 48;
			_wcsupr_s(wBuff, 2);
			wsKey = wBuff;
		}
		switch (m_nKey)
		{
		case VK_MULTIPLY:wsKey = L"*"; break;
		case VK_ADD:wsKey = L"+"; break;
		case VK_SUBTRACT:wsKey = L"-"; break;
		case VK_DECIMAL:wsKey = L"."; break;
		case VK_DIVIDE:wsKey = L"/"; break;
		case VK_F1:wsKey = L"F1"; break;
		case VK_F2:wsKey = L"F2"; break;
		case VK_F3:wsKey = L"F3"; break;
		case VK_F4:wsKey = L"F4"; break;
		case VK_F5:wsKey = L"F5"; break;
		case VK_F6:wsKey = L"F6"; break;
		case VK_F7:wsKey = L"F7"; break;
		case VK_F8:wsKey = L"F8"; break;
		case VK_F9:wsKey = L"F9"; break;
		case VK_F10:wsKey = L"F10"; break;
		case VK_F11:wsKey = L"F11"; break;
		case VK_F12:wsKey = L"F12"; break;
		case VK_TAB:wsKey = L"Tab"; break;
		case VK_ESCAPE:wsKey = L"Esc"; break;
		case VK_CAPITAL:wsKey = L"Caps lock"; break;
		case VK_BACK:wsKey = L"Backspace"; break;
		case VK_OEM_PLUS:wsKey = L"="; break;
		case VK_OEM_MINUS:wsKey = L"-"; break;
		case VK_SPACE:wsKey = L"�ո�"; break;
		case VK_LEFT:wsKey = L"��"; break;
		case VK_RIGHT:wsKey = L"��"; break;
		case VK_UP:wsKey = L"��"; break;
		case VK_DOWN:wsKey = L"��"; break;
		case VK_RETURN:wsKey = L"Enter"; break;
		default:break;
		}
		wstring wsSubKey = L"";
		if (m_bCtrl) {
			wsSubKey = L"Ctrl+";
		}
		if (m_bAlt) {
			wsSubKey += L"Atl+";
		}
		if (m_bShift) {
			wsSubKey += L"Shift+";
		}
		if (wsKey.length() == 0) 
		{
			return L"";
		}
		wsKey = wsSubKey + wsKey;
		return wsKey;
	}
};
// �ȼ��ӿ���
class ILeDB_HotKey
{
public:
	virtual void DefaultHotkey() = 0;
	virtual void InsertKotKey(const char* pchName, int key1, int key2) = 0;
	virtual void GetHotkey(const char* pchName, _HotKey& hk) = 0;
	virtual void SetHotkey(const char* pchName, _HotKey hk) = 0;
};
// �ļ������ӿ���
class ILeDB_FileAss
{
public:	
	//��ʼ������
	virtual void InitInsert() = 0;
	virtual bool InsertAssociation(int id, const char* pchName, int flag) = 0;
	virtual bool GetAssociation(int id) = 0;
	virtual bool GetAssociation(const char* pchName) = 0;
	virtual char* GetExtName(int id) = 0;
	virtual void DefaultAss(int flag) = 0;
	virtual void SetAssociation(int id, int flag) = 0;
	virtual void SetAssociation(const char* pchName, int flag) = 0;
};

// �����Ǵ� LeDB.dll ������
class LEDB_API CLeDB 
{
public:
	static CLeDB* GetInstance();
	static void Release();
	~CLeDB();

	bool InitSql();
	bool CreateTable();
	bool CreateIndex();
	//������ɾ����¼
	bool ClearDel();
	void ThingBegin();
	void ThingEnd();

	void ReleaseStr(char* pch);
	//void ReleaseString(string* pch);
	void ReleaseVecLocal(void* pVoid_Local);
	void ReleaseVecNet(void* pVoid_Net);
	void ReleaseVecString(void* pVoid_String);

	ILeDB_Local* Local();
	ILeDB_Net* Net();
	ILeDB_Search* Search();
	ILeDB_Set* Set();
	ILeDB_HotKey* HotKey();
	ILeDB_FileAss* FileAss();

private:
	CLeDB(void);
	static CLeDB* m_pInstance;
};