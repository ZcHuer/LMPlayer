#pragma once
#include <vector>
#include <string>
#include <map>
#include <atltime.h>

using namespace std;

//����������Ҫ�ز���Ϣ
struct sAdvElement
{
	bool	bLoop;											//�Ƿ�ѭ��
	int		nCycle;											//�ܼ�ѭ��
	string	sShowtime;										//����ʱ��
};
struct sAdvSt
{
	vector<sAdvElement *> sElement;
};

struct _DoMain
{
	string epgDomain;
	string bssDomain;
	string searchDomain;
	string bigDataDomain;
	string foundPageURL;
	string relativeId;
	bool isForceUpdate;
	std::map<string, string> para2;
	string para;
	_DoMain()
	{
		epgDomain = "https://epg.vgs.lenovo.com.cn/";
		bssDomain = "https://bss.vgs.lenovo.com.cn";
		searchDomain = "https://search.vgs.lenovo.com.cn/";
		bigDataDomain = "https://collect.vgs.lenovo.com.cn/";
		relativeId = "62";
		isForceUpdate = false;
		para = "cpId=118,119&lSrc=pc&lVer=1&relativeId=62";
	}
};
//��ʾ��������ϸ��Ϣ
struct sAdvStCtrl
{
	vector<sAdvSt *> vAdvVT;				//���ģ��+Ҫ��ʾ�����ز�
	string sParam;							//���������в���
};
class CPopAdvCtrl
{
public:
	CPopAdvCtrl();
	~CPopAdvCtrl();
	void PopAdvCtrl();

private:
	void GetAdvInfo();
	bool CheckAdvTime();

	//system ����UI����
	DWORD INTER_GetExplorerToken(OUT PHANDLE phExplorerToken);
	//��ȡ������Ҫ�������в���
	string GetDesParameStr(int nAdvIndex,vector<int> vdes);
	wchar_t m_currentPath[MAX_PATH];
public:
	void StartPopSe(string parame);
	
private:
	bool isNeedRepired();
	bool CurrentIsLanchPath(string & currentPath);
	void GetMainUrl();


private:
	CTime m_timeLocation;
	sAdvStCtrl m_sAdvCtrl;
	int m_nTick;
	_DoMain m_doMain;
public:
	int ReleaseVec();
	string GetRandPopTime(string showtime);
};

