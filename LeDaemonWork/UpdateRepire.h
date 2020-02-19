#pragma once
#include <string>
using namespace std;

#define Error_Download  0
#define Error_Unzip		1
#define Error_File		2

// �򴰿ڷ��͵���Ϣ
enum eUpdateMsg
{
	eUPDATE_Progress = WM_USER + 1,		// ���ؽ���
	eUPDATE_Success,					// �ɹ�
	eUPDATE_NetError,					// �������
	eUPDATE_NetRepair,					// ����ָ�
	eUPDATE_Error_Download,				// ����ʧ��
	eUPDATE_Error_Unzip,				// ����ʧ��
	eUPDATE_Error_File,					// �ļ�����
};

struct sUpdateInfo
{
	int id;
	int tvInfoId;
	string appName;
	string appPackage;
	string appSize;
	string appLoadUrl;
	string versionCode;
	string versionName;
	string versionDesc;
	int statusCode;
	int statusCount;
	string miniVersion;
	string updateType;
	long createTime;
	long updateTime;
	string md5;
};

class CUpdateRepire
{
public:
	CUpdateRepire();
	~CUpdateRepire();

	// ��ѯ�Ƿ���������
	bool IsDownLoading();
	// ��ѯ��������
	bool GetUpdateConfig(sUpdateInfo& sInfo);
	// �����������������Ҫ���̣߳��������������ķ���ֵ��Ĭ�ϲ����̣߳���Ϊû�лص��Ĵ��ھ����ֻ��������������ֵ
	bool DownloadAndUnzip(LPCSTR lpcstr_url, LPCSTR lpcstr_vername, bool bThread = false, HWND hWnd = NULL);
	// �󶨻ص����
	bool SetCallBackHwnd(HWND hWnd);
	//
	bool DownloadAndRepire(LPCSTR lpcstr_url, LPCSTR lpcstr_vername, bool bThread = false, HWND hWnd = NULL);
};

