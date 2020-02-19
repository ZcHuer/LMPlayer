#include "stdafx.h"
#include "PopAdvCtrl.h"
#include <atltime.h>
#include "../SDK/Flog/FileLog.h"
#include <TlHelp32.h>
#include <strsafe.h>
#include "../Include/LeTools.h"
#include <Userenv.h>
#include "../Include/json/json.h"
#include "../SDK/http/Http.h"
#include <winbase.h>
using namespace LeConnect;


CPopAdvCtrl::CPopAdvCtrl()
{
	m_nTick = 0;
	GetMainUrl();
	GetAdvInfo();
}

CPopAdvCtrl::~CPopAdvCtrl()
{
}


void CPopAdvCtrl::PopAdvCtrl()
{
	do 
	{
		m_nTick++;
		CheckAdvTime();
		if(m_nTick==60*60)
		{
			m_nTick = 0;
			GetAdvInfo();
		}
		Sleep(1000);
	} while (TRUE);
}
void CPopAdvCtrl::GetMainUrl()
{
	FLOG(L"GetMainUrl begin");
	wstring wsUrl;
	wstring wsUrlPath;
	LeTools::GetDefaultMainUrl(eService, wsUrl);
	wsUrl.append(L"back/urls/leplayer/1");
	wsUrl.append(L"?mac=");
	wsUrl.append(LeTools::s2ws(LeTools::GetMacString()));
	FLOG(L"mainURL:%s", wsUrl.c_str());
	HttpRequest req(wsUrl.c_str(), NULL, NULL);
	BOOL bSucc = req.doGet();
	if (bSucc)
	{
		string sResult = req.getStream().str();
		//FLOG(L"获取域名：%s", LeTools::s2ws(sResult).c_str());
		sResult = LeTools::ConvertUtf8ToGBK(sResult);
		Json::Reader jr;
		Json::Value jvData;
		if (jr.parse(sResult, jvData))
		{
			if (jvData["code"].asInt() == 0)
			{
				Json::Value jv = jvData["data"];
				m_doMain.epgDomain = jv["epgDomain"].asString();
				m_doMain.bssDomain = jv["bssDomain"].asString();
				m_doMain.searchDomain = jv["searchDomain"].asString();
				m_doMain.bigDataDomain = jv["bigDataDomain"].asString();
				m_doMain.foundPageURL = jv["foundPageURL"].asString();
				m_doMain.isForceUpdate = jv["isForceUpdate"].asBool();
				m_doMain.relativeId = jv["para2"]["relativeId"].asString();
				Json::Value Jv_para2 = jv["para2"];
				Json::Value::Members members = Jv_para2.getMemberNames();
				for (Json::Value::Members::iterator iterMember = members.begin(); iterMember != members.end(); iterMember++)   // 遍历每个key
				{
					string strFirst = *iterMember;
					m_doMain.para2[strFirst] = Jv_para2[strFirst].asString();
				}
				m_doMain.para = jv["para"].asString();

				//获取默认关键字
				FLOG(L"获取域名成功1：%s", LeTools::s2ws(m_doMain.epgDomain).c_str());
				FLOG(L"获取域名成功2：%s", LeTools::s2ws(m_doMain.bssDomain).c_str());
				FLOG(L"获取域名成功3：%s", LeTools::s2ws(m_doMain.searchDomain).c_str());
				FLOG(L"获取域名成功4：%s", LeTools::s2ws(m_doMain.bigDataDomain).c_str());

			}
		}
	}
}

void CPopAdvCtrl::GetAdvInfo()
{
	//获取广告信息到数据接口
	string sUrl = "";
	sUrl = m_doMain.epgDomain;
	//http://epg.vgs.lenovo.com.cn/
	//http://epg.vgs.lenovo.com.cn/getAdvertisement/62?advType=MUM&purpose=0&pre=1
	//purpose=1 卸载；0或者不传；还是原来的逻辑
	sUrl.append("getAdvertisement/");
	sUrl.append(m_doMain.relativeId);
	sUrl.append("?advType=MUM&purpose=");
	sUrl.append(isNeedRepired() ? "1" : "0");
	HttpRequest req(LeTools::s2ws(sUrl).c_str(), NULL, NULL);
	FLOG(L"sUrl:%s", LeTools::s2ws(sUrl));

	BOOL bSucc = req.doGet();
	if (bSucc)
	{
		string sResult = req.getStream().str();
#ifdef DEBUG
		sResult = "{\"code\":0,\"msg\":\"ok\",\"vip\":\"202,2.0.2\",\"datas\":[{\"id\":106,\"relativeId\":62,\"advName\":\"PC优酷客户端-TC样式1\",\"advType\":\"MUM\",\"advMode\":0,\"advElementSize\":\"400*224\",\"advMediaSources\":0,\"thirdPartyId\":0,\"terminal\":\"\",\"advDeviceId\":\"\",\"advStatus\":\"1\",\"advCategory1\":\"\",\"advTag\":\"\",\"advAlbumName\":\"\",\"advAlbumId\":0,\"advSort\":1,\"advRemark\":\"\",\"beginTime\":0,\"channelId\":0,\"moduleId\":0,\"advStyle\":\"TC1\",\"clientVersion\":\"\",\"userType\":\"\",\"downAddress\":\"https://sta-dev.vgs.lenovo.com.cn/popup.html\",\"frameSize\":\"400*248\",\"insertType\":0,\"carouselProgramList\":\"\",\"carouselName\":\"\",\"defaultVolume\":\"\",\"defaultTime\":\"\",\"autoRotation\":\"1\",\"rotationTime\":\"\",\"advElements\":[{\"id\":161,\"advId\":106,\"elementName\":\"PC-TC样式1-测试\",\"launchType\":\"MTG\",\"mediaType\":\"MSD\",\"elementUrl\":\"https://imagecdn.vgs.lenovo.com.cn/cms/2018-11-01/201811011749152978855.jpg\",\"jumpUrl\":\"\",\"elementSort\":1,\"elementDuration\":0,\"advStartDate\":\"2018-11-01 00:00:38\",\"advEndDate\":\"2019-05-31 17:47:39\",\"elementStatus\":\"2\",\"thirdAction\":\"\",\"thirdParameter\":\"\",\"elementRemark\":\"\",\"albumId\":39,\"albumName\":\"建军大业\",\"albumPoster\":\"\",\"focus\":\"新生代演员神还原伟人\",\"cpId\":\"119\",\"elementVideo\":\"\",\"stayTime\":30,\"mainTitle\":\"香蜜\",\"pushDate\":\"11:22:19\",\"islooppush\":1,\"cycle\":\"8\",\"subTitle\":\"哈哈哈啊\",\"vipShow\":0,\"elementMd5\":\"ec853a47405cf897c55fc26538ef90da\",\"silentOpen\":0,\"album\":{\"outAlbumId\":0,\"outTvId\":0,\"displayType\":\"MOVIE\",\"poster\":\"\",\"elementName\":\"建军大业\",\"category\":102,\"episodes\":1,\"focus\":\"新生代演员神还原伟人\",\"playCount\":35263973,\"playCountCn\":\"3526万\",\"isDubo\":0,\"isPurchase\":0,\"duration\":\"02:12:40\",\"upStatusDesc\":\"\",\"jumpId\":\"39\",\"jumpType\":\"detail\",\"description\":\"1927年\",\"category2\":\"历史/剧情/院线\",\"cpId\":119,\"outAlbumIdBst\":\"4a538ac2ecdd11e6b9bb\",\"outTvIdBst\":\"XMzA1MTExMTU4MA==\",\"isMain\":1,\"actors\":\"刘烨\",\"category2Ids\":\",1520,1500,12903,\",\"videoSource\":0,\"videoWidth\":0,\"videoHeight\":0,\"contentId\":\"820\",\"albumStatus\":\"AAB\",\"updateTime\":1533484800000,\"playPlatform\":\",PC,\",\"subtags\":\"\",\"stream\":[],\"advBean\":{\"islooppush\":0,\"elementMd5\":\"\"}}}]},{\"id\":107,\"relativeId\":62,\"advName\":\"PC客户端-TC样式2\",\"advType\":\"MUM\",\"advMode\":0,\"advElementSize\":\"150*208\",\"advMediaSources\":0,\"thirdPartyId\":0,\"terminal\":\"\",\"advDeviceId\":\"\",\"advStatus\":\"1\",\"advCategory1\":\"\",\"advTag\":\"\",\"advAlbumName\":\"\",\"advAlbumId\":0,\"advSort\":2,\"advRemark\":\"\",\"beginTime\":0,\"channelId\":0,\"moduleId\":0,\"advStyle\":\"TC2\",\"clientVersion\":\"\",\"userType\":\"\",\"downAddress\":\"https://sta-dev.vgs.lenovo.com.cn/popup.html\",\"frameSize\":\"400*248\",\"insertType\":0,\"carouselProgramList\":\"\",\"carouselName\":\"\",\"defaultVolume\":\"\",\"defaultTime\":\"\",\"autoRotation\":\"1\",\"rotationTime\":\"\",\"advElements\":[{\"id\":144,\"advId\":107,\"elementName\":\"PC-TC样式2-测试\",\"launchType\":\"MTG\",\"mediaType\":\"MSD\",\"elementUrl\":\"https://imagecdn.vgs.lenovo.com.cn/cms/2018-11-01/201811011446366122366.jpg\",\"jumpUrl\":\"\",\"elementSort\":1,\"elementDuration\":0,\"advStartDate\":\"2018-11-01 14:00:01\",\"advEndDate\":\"2019-05-31 23:59:01\",\"elementStatus\":\"2\",\"thirdAction\":\"\",\"thirdParameter\":\"\",\"elementRemark\":\"\",\"albumId\":13285,\"albumName\":\"香蜜沉沉烬如霜 TV版\",\"albumPoster\":\"\",\"focus\":\"杨紫邓伦上演千年之恋\",\"cpId\":\"119\",\"elementVideo\":\"\",\"stayTime\":5,\"mainTitle\":\"香蜜沉沉烬如霜\",\"pushDate\":\"14:56:52\",\"islooppush\":1,\"cycle\":\"0\",\"subTitle\":\"唉呀妈呀\",\"vipShow\":0,\"elementMd5\":\"8f696041b4ab240dd72bb3c1a705b2d0\",\"silentOpen\":0,\"album\":{\"outAlbumId\":0,\"outTvId\":0,\"displayType\":\"ALBUM\",\"poster\":\"\",\"elementName\":\"香蜜沉沉烬如霜 TV版\",\"category\":101,\"episodes\":63,\"focus\":\"杨紫邓伦上演千年之恋\",\"playCount\":1987441367,\"playCountCn\":\"19亿\",\"isDubo\":0,\"isPurchase\":1,\"duration\":\"00:45:32\",\"upStatusDesc\":\"共63集\",\"jumpId\":\"13285\",\"jumpType\":\"detail\",\"description\":\"永世孤独的天帝。\",\"category2\":\"爱情/古装/仙侠\",\"cpId\":119,\"outAlbumIdBst\":\"69efbfbdefbfbd343a00\",\"outTvIdBst\":\"XMzc1OTA1MDY2MA==\",\"isMain\":1,\"actors\":\"杨紫\",\"category2Ids\":\",1112,1107,5880,\",\"videoSource\":0,\"videoWidth\":0,\"videoHeight\":0,\"contentId\":\"518374\",\"albumStatus\":\"AAB\",\"updateTime\":1536163200000,\"playPlatform\":\",PC,\",\"subtags\":\"\",\"stream\":[],\"advBean\":{\"islooppush\":0,\"elementMd5\":\"\"}}}]}],\"currentTime\":1541992939000,\"isRecommendCount\":0,\"notRecommendCount\":0}";
#endif // DEBUG
		FLOG(_T("sResult：%s"), LeTools::s2ws(sResult));
		sResult = LeTools::ConvertUtf8ToGBK(sResult);

		Json::Reader jr;
		Json::Value jvData;
		if (jr.parse(sResult, jvData)) {

			if (jvData["code"].asInt() == 0)
			{
				ReleaseVec();
				m_sAdvCtrl.vAdvVT.clear();
				FLOG(_T("获取配置成功：%s"), LeTools::s2ws(sResult));
				//成功
				m_sAdvCtrl.vAdvVT.clear();
				m_sAdvCtrl.sParam = "";
				m_sAdvCtrl.sParam = sResult;
				int nSize = jvData["datas"].size();
				size_t i=0;
				FLOG(L"nSize:%d", nSize);
				for (i = 0; i < nSize; i++)
				{
					Json::Value jvAdvData = jvData["datas"];
					int nElement = jvAdvData[i]["advElements"].size();
					sAdvSt* advst =new sAdvSt();
					sAdvElement* sEle = NULL;
					FLOG(L"nElement:%d", nElement);
					for (size_t k= 0; k < nElement; k++)
					{
						sEle = new sAdvElement();;
						Json::Value jvAdvElement = jvAdvData[i]["advElements"];
						//FLOG(L"jvAdvElement.asCString:%s", LeTools::s2ws(jvAdvElement.toStyledString()).c_str());
						sEle->bLoop = jvAdvElement[k]["islooppush"].asBool();
						sEle->nCycle = atoi(jvAdvElement[k]["cycle"].asString().c_str());
						FLOG(L"jvAdvElement[%d][\"cycle\"]:%d", k, atoi(jvAdvElement[k]["cycle"].asString().c_str()));	
						sEle->sShowtime = jvAdvElement[k]["pushDate"].asString();//GetRandPopTime(jvAdvElement[k]["pushDate"].asString());
						FLOG(L"sEle->sShowtime%d:%s", k, LeTools::s2ws(sEle->sShowtime).c_str());
						advst->sElement.push_back(sEle);
					}
					m_sAdvCtrl.vAdvVT.push_back(advst);
					FLOG(L"m_sAdvCtrl.vAdvVT.size：%d", m_sAdvCtrl.vAdvVT.size());
				}
			}
			else
			{
				//失败
				//g_advStr = "";
			}
		}
	}
}

bool CPopAdvCtrl::CheckAdvTime()
{
	int isize = m_sAdvCtrl.vAdvVT.size();
	//FLOG(L"m_sAdvCtrl.vAdvVT:%d", isize);
	vector<int> vDesindex;
	//循环所有的广告信息
	for (int i = 0; i < isize; i++)
	{
		vDesindex.clear();
		//循环所有素材
		sAdvSt* sdata = m_sAdvCtrl.vAdvVT[i];
		int nMaterial = sdata->sElement.size();
		m_timeLocation = CTime::GetCurrentTime();
		int ndesIndex = 0;
		bool bshowAd = FALSE;
		// FLOG(L"sdata.sElement:%d", nMaterial);
		for (size_t j = 0; j < nMaterial; j++)
		{
			sAdvElement* element = sdata->sElement[j];
			//FLOG(L"element.sShowtime:%s",LeTools::s2ws(element->sShowtime));
			string showtime = element->sShowtime;
			BOOL isloop = element->bLoop;
			int showday = element->nCycle;
			//showday = 8;

			int iyear = m_timeLocation.GetYear();
			int month = m_timeLocation.GetMonth();
			int day = m_timeLocation.GetDay();
			//FLOG(L"showtime:%d", showtime.length());
			if (showtime.length()<8)
				continue;
			//服务器时间年，月，年
			int hour = atoi(showtime.substr(0, 2).c_str());
			int minmont = atoi(showtime.substr(3, 2).c_str());
			int sec = atoi(showtime.substr(6, 2).c_str());

			//FLOG(L"%d:%d:%d %d:%d:%d", iyear, month, day, hour, minmont, sec);
			CTime tm1(iyear, month, day, hour, minmont, sec);
			//服务器时间星期
			int weekday = tm1.GetDayOfWeek();
			//1541829918			1541832952907
			//2018/11/10 14:05:18   2018/11/10 14:55:52
			//1541819752,11:15:52

			CTimeSpan spn = tm1 - m_timeLocation;
			//显示器时间和服务器时间的差值
			int subtime = spn.GetTotalSeconds();
			//FLOG(_T("时间差：%d"), subtime);
			if (subtime == 0)
			{

				//FLOG(L"seParame:%s", LeTools::s2ws(seParame));
				//string sSubJson = datas[i]["advElements"][j].toStyledString();
				//FLOG(L"sSubJson:%s", LeTools::ConvertUtf8ToGBK(sSubJson));
				//datas_dest["advElements"][ndesIndex] = datas[i]["advElements"][j];

				
				//FLOG(L"ndesIndex:%d,j:%d", ndesIndex, j);
				FLOG(L"vDesindex1:%d", vDesindex.size());
				vDesindex.push_back(j);
				FLOG(L"vDesindex:%d", vDesindex.size());
				if (isloop)
				{
					/*
					0		1 2 3 4 5 6 7	 8			9
					周6，7		工作日循环	每日	  工作日
					*/
					//FLOG(L"loop");
					if (showday == 0)
					{
						//周末
						if (weekday == 1 || weekday == 7)
						{
							bshowAd = TRUE;
						}
						else
						{
							bshowAd = FALSE;
						}

					}
					else if (showday >= 1 && showday <= 7)
					{
						bshowAd = (weekday == showday) ? TRUE : FALSE;
					}
					else if (showday == 8)
					{
						//每天
						bshowAd = TRUE;
					}
					else if (showday == 9)
					{
						//工作日提醒
						if (2 <= weekday &&weekday <= 6)
						{
							bshowAd = TRUE;
						}
						else
						{
							bshowAd = FALSE;
						}

					}
				}
				else
				{

					bshowAd = TRUE;
					//TagAdFlag(adid);
				}
			}
		}
		//FLOG(L"bshowAd:%d", bshowAd);
		if (bshowAd)
		{
			FLOG(L"start pop adv");
			string seParame = GetDesParameStr(i, vDesindex);
			//FLOG(L"seParame2:%s", LeTools::s2ws(seParame));
			srand(time(0));
			int randtime = rand() % 300;
			FLOG(L"randtime:%d", randtime);
			Sleep(randtime * 1000);
			if (false == LeTools::IsOtherFullscreen())
			{
				FLOG(L"no full");
				StartPopSe(seParame);
			}
			else
			{
				FLOG(L"have full");
			}
				
		}
	}
	return false;
}

void CPopAdvCtrl::StartPopSe(string parame)
{
	TCHAR buff[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, buff, sizeof(buff));
	::PathRemoveFileSpecW(buff);
	StrNCpy(m_currentPath, buff, MAX_PATH);

	FLOG(_T("StartPopSe begin:%s"), LeTools::s2ws(parame));
	FLOG(_T("%s"), m_currentPath);
	//m_adCmdLine = parame;
	HANDLE hPtoken = NULL;
	INTER_GetExplorerToken(&hPtoken);
	PROCESS_INFORMATION pi;
	STARTUPINFOA si = { sizeof(STARTUPINFO),NULL,"",NULL,0,0,0,0,0,0,0,STARTF_USESHOWWINDOW,0,0,NULL,0,0,0 };
	si.wShowWindow = SW_SHOW;
	si.lpDesktop = NULL;
	LPVOID pEnv = NULL;
	CreateEnvironmentBlock(&pEnv, hPtoken, FALSE);

	ZeroMemory(&pi, sizeof(pi));
	wchar_t cmdline[MAX_PATH] = { 0 };
	//StrCpyW(cmdline, g_currentPath);
	StrNCpy(cmdline, m_currentPath, MAX_PATH);
	PathAppendW(cmdline, _T("\\LePopAdSe.exe"));
	FLOG(L"cmdline:%s", cmdline);
	//cmdline.Format("C:\\work\\LenovoPlayer\\Bin\\Debug\\LePopAdSe.exe %s",parame);
	CreateProcessAsUserA(hPtoken, LeTools::ws2s(cmdline).c_str(), (LPSTR)parame.c_str(),
		NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE | NORMAL_PRIORITY_CLASS | CREATE_UNICODE_ENVIRONMENT, pEnv, LeTools::ws2s(m_currentPath).c_str(), &si, &pi);
	//::ShellExecuteA(NULL, "open", "C:\\work\\LenovoPlayer\\Bin\\Debug\\LePopAdSe.exe", parame.c_str(), NULL, SW_SHOW);
	FLOG(L"StartPopSe End");
}

DWORD CPopAdvCtrl::INTER_GetExplorerToken(OUT PHANDLE phExplorerToken)
{
	DWORD       dwStatus = ERROR_FILE_NOT_FOUND;
	BOOL        bRet = FALSE;
	HANDLE      hProcess = NULL;
	HANDLE      hProcessSnap = NULL;
	WCHAR        szExplorerPath[MAX_PATH] = { 0 };
	WCHAR        FileName[MAX_PATH] = { 0 };
	PROCESSENTRY32 pe32 = { 0 };
	//__try
	{
		GetWindowsDirectory(szExplorerPath, MAX_PATH);
		//StrCatW(szExplorerPath, L"\\explorer.exe");
		//StrNCatW(szExplorerPath, L"\\explorer.exe", MAX_PATH);
		StringCchCat(szExplorerPath, MAX_PATH, L"\\explorer.exe");
		string expStr = LeTools::ws2s(szExplorerPath);
		hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (hProcessSnap == INVALID_HANDLE_VALUE)
		{

			dwStatus = GetLastError();
			return dwStatus;
			//__leave;
		}
		pe32.dwSize = sizeof(PROCESSENTRY32);
		if (!Process32First(hProcessSnap, &pe32))
		{
			dwStatus = GetLastError();
			return dwStatus;
		}
		BOOL bres = FALSE;
		do {
			hProcess = OpenProcess(
				PROCESS_ALL_ACCESS,
				FALSE,
				pe32.th32ProcessID);
			if (NULL != hProcess)
			{
				DWORD(__stdcall *GMFNE)(HANDLE hProcess,
					HMODULE hModule,
					LPTSTR lpFilename,
					DWORD nSize);
				HMODULE hPsapi = LoadLibrary(L"PSAPI");
				if (!hPsapi)
				{
					dwStatus = GetLastError();
					break;
				}
				GMFNE = (DWORD(__stdcall *) (HANDLE, HMODULE, LPTSTR, DWORD))GetProcAddress(hPsapi, "GetModuleFileNameExW");
				if (GMFNE(hProcess, NULL, FileName, MAX_PATH))
				{
					string tmp = LeTools::ws2s(FileName);
					if (!_stricmp(expStr.c_str(), tmp.c_str()))
					{
						HANDLE  hToken;
						if (OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken))
						{
							*phExplorerToken = hToken;
							dwStatus = 0;
						}
						break;
					}
				}
				CloseHandle(hProcess);
				hProcess = NULL;
			}
			bres = Process32Next(hProcessSnap, &pe32);
		} while (bres);
	}
	//__finally
	{
		if (NULL != hProcess)
		{
			CloseHandle(hProcess);
		}
		if (NULL != hProcessSnap)
		{
			CloseHandle(hProcessSnap);
		}
	}

	return dwStatus;
}

string CPopAdvCtrl::GetDesParameStr(int nAdvIndex,vector<int> vdes)
{
	FLOG(L"GetDesParameStr begin");
	Json::Reader jr;
	Json::Value jvData;
	Json::Value datas_dest;
	string strRes = "";
	
	if (jr.parse(m_sAdvCtrl.sParam, jvData))
	{
		Json::Value datas = jvData["datas"];
		datas_dest = datas[nAdvIndex];
		datas_dest["advElements"].clear();
		int nsize = vdes.size();
		FLOG(L"vdes:%d", nsize);
		FLOG(L"nAdvIndex:%d", nAdvIndex);
		for (int i=0;i<nsize;i++)
		{
			int nindex = vdes[i];
			FLOG(L"nindex:%d;i:%d", nindex,i);
			datas_dest["advElements"][i] = datas[nAdvIndex]["advElements"][nindex];
			datas_dest["advElements"][i]["album"]["description"] = "";
		}
		FLOG(L"nAdvIndex:%d", nAdvIndex);
		strRes = datas_dest.toStyledString();
	}
	FLOG(L"GetDesParameStr begin");
	return strRes;

}


bool CPopAdvCtrl::isNeedRepired()
{
	//1 卸载；0或者不传
	bool bres = TRUE;
	WCHAR wcMoudlePath[MAX_PATH] = { 0 };
	::GetModuleFileNameW(NULL, wcMoudlePath, MAX_PATH);
	//FLOG(wcMoudlePath);
	string path;
	bool isoldServer = CurrentIsLanchPath(path);
	if (isoldServer)
	{
		PathRemoveFileSpecW(wcMoudlePath);
		PathAppendW(wcMoudlePath, L"\\LMPlayer.exe");
	}
	else
	{
		PathRemoveFileSpecW(wcMoudlePath);
		PathRemoveFileSpecW(wcMoudlePath);
		PathAppendW(wcMoudlePath, L"\\LePlayer\\LMPlayer.exe");
	}

	FLOG(wcMoudlePath);
	if (PathFileExistsW(wcMoudlePath))
	{
		//MessageBox(lpwsContent, L"4",  0);
		bres = FALSE;
	}

	return bres;
}


bool CPopAdvCtrl::CurrentIsLanchPath(string & currentPath)
{
	bool bres = FALSE;
	WCHAR wcPath[MAX_PATH] = { 0 };
	::GetModuleFileNameW(NULL, wcPath, MAX_PATH);
	PathRemoveFileSpecW(wcPath);
	currentPath = LeTools::ws2s(wcPath);
	PathAppendW(wcPath, L"\\LMPlayer.exe");
	if (PathFileExists(wcPath)) bres = TRUE;
	return bres;
}


int CPopAdvCtrl::ReleaseVec()
{
	size_t length = m_sAdvCtrl.vAdvVT.size();
	for (size_t i = 0; i < length; i++)
	{
		sAdvSt *p1 = m_sAdvCtrl.vAdvVT[i];
		size_t nsize = p1->sElement.size();
		for (size_t j = 0;j < nsize; j++)
		{
			sAdvElement * p2 = p1->sElement[j];
			if (p2)
			{
				delete(p2);
				p2 = NULL;
			}
			
		}
		if (p1)
		{
			delete(p1);
			p1 = NULL;
		}


	}
	return 0;
}


string CPopAdvCtrl::GetRandPopTime(string showtime)
{
	string strRes = "";
	int hour = atoi(showtime.substr(0, 2).c_str());
	int minmont = atoi(showtime.substr(3, 2).c_str());
	int sec = atoi(showtime.substr(6, 2).c_str());
	srand(time(nullptr));
	int randtime = rand() % 300;
	int showtick = hour * 60 * 60 + minmont * 60 + sec;
	int randtick = showtick + randtime;
	hour = (randtick / 60 / 60)%24;
	minmont = (randtick / 60) % 60;
	sec = randtick % (60);
	char ctime[24] = { 0 };
	sprintf_s(ctime, "%02d:%02d:%02d", hour, minmont, sec);
	strRes = ctime;
	return strRes;
}
