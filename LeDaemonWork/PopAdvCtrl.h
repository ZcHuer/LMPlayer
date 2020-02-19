#pragma once
#include <vector>
#include <string>
#include <map>
#include <atltime.h>

using namespace std;

//弹窗程序需要素材信息
struct sAdvElement
{
	bool	bLoop;											//是否循环
	int		nCycle;											//周几循环
	string	sShowtime;										//弹出时间
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
//显示弹窗数据细信息
struct sAdvStCtrl
{
	vector<sAdvSt *> vAdvVT;				//广告模板+要显示广告的素材
	string sParam;							//弹窗命令行参数
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

	//system 调用UI交互
	DWORD INTER_GetExplorerToken(OUT PHANDLE phExplorerToken);
	//获取弹窗需要的命令行参数
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

