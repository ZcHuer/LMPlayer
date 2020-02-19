#pragma once
#include <string>
#include <map>
#include "CommonDefine.h"
using namespace std;

// 数据上报字段
struct sReportData
{
	string channelId;
	string moduleId;
	string moduleIndex;
	string moduleType;
	string elementId;
	string elementType;
	string activeId;
	string videoId;
	string ifConsume;
	string categoryId;
	string categoryId2;
	string episodeId;
	string playTime;			// 观看时长
	string ifSuccess="";
	string bannerId;
	string columnId;
	string albumId;
	string thematicId;
	string category1;
	string lcpId;
	string sScreen;
	void _Copy(sReportData old)
	{
		channelId = old.channelId;
		moduleId = old.moduleId;
		moduleIndex = old.moduleIndex;
		moduleType = old.moduleType;
		elementId = old.elementId;
		elementType = old.elementType;
		activeId = old.activeId;
		videoId = old.videoId;
		ifConsume = old.ifConsume;
		categoryId = old.categoryId;
		categoryId2 = old.categoryId2;
		episodeId = old.episodeId;
		playTime = old.playTime;
		ifSuccess = old.ifSuccess;
		bannerId = old.bannerId;
		columnId = old.columnId;
		albumId = old.albumId;
		thematicId = old.thematicId;
		category1 = old.category1;
		lcpId = old.lcpId;
		sScreen = old.sScreen;
	}
};

class sPlayData
{
public:
	sPlayData()
	{
		ReSet();
	}
	wstring strSource;			// 播放地址 本地文件可能包括汉字，所以一定用宽字符，不然在宽窄转换几次会丢失
	wstring sMd5;					// MD5
	wstring strName;				// 片名
	eSourceType eSrcType;				// 源类型
	bool bNoHistory;					// 是否已经添加过播放记录
	eSignalType eSignal;				// 源来源，从播放列表/播放记录中点击的，不再添加播放列表
	int nNum;							// 当前序号
	int nTotal;							// 总集数
	bool bautoPlay;						// 是否要自动播放下一集
	long currPlayTime;					// 当前播放时间
	long lTotalTime;					// 总时长
	bool bOnlyAddList;					// 是否只添加到播放列表，不播放	
	bool bShort;						// 是否短视频
	bool bError;						// 是否无效视频
	bool bFavorite;						// 是否收藏
	bool bDel;							// 是否删除
	long conAlbumId;
	long conContentId;
	wstring outConAlbumIdYouKu;
	wstring outConContentIdYouKu;
	wstring outConAlbumId;
	wstring outConContentId;
	wstring ifConsume = _T("2");
	long orderNumber;					// 剧集编号
	string contentName;					// 子剧集名称	
	long time;							// 节点产生的系统时间
	long WatchDuration;					// 观看时长
	sReportData sRD;					// 数据上报的字段

	void ReSet()
	{
		strSource = L"";
		sMd5 = L"";
		strName = L"";
		eSrcType = eSourceType_Unknown;
		bNoHistory = false;
		eSignal = eSignalType_UnKnown;
		nNum = 0;
		nTotal = 0;
		bautoPlay = false;
		currPlayTime = 0;
		lTotalTime = 0;
		bOnlyAddList = false;
		bShort = false;
		bError = false;
		bFavorite = false;
		bDel = false;
		conAlbumId = false;
		conContentId = false;
		outConAlbumIdYouKu = L"";
		outConContentIdYouKu = L"";
		outConAlbumId = L"";
		outConContentId = L"";
		ifConsume = _T("2");
		orderNumber = 0;
		contentName = "";
		time = 0;
		WatchDuration = GetTickCount();
	}

	void Copy(sPlayData sData)
	{
		strSource = sData.strSource;
		sMd5 = sData.sMd5;
		strName = sData.strName;
		eSrcType = sData.eSrcType;
		bNoHistory = sData.bNoHistory;
		eSignal = sData.eSignal;
		nNum = sData.nNum;
		nTotal = sData.nTotal;
		bautoPlay = sData.bautoPlay;
		currPlayTime = sData.currPlayTime;
		lTotalTime = sData.lTotalTime;
		bOnlyAddList = sData.bOnlyAddList;
		bShort = sData.bShort;
		bError = sData.bError;
		bFavorite = sData.bFavorite;
		bDel = sData.bDel;
		conAlbumId = sData.conAlbumId;
		conContentId = sData.conContentId;
		outConAlbumIdYouKu = sData.outConAlbumIdYouKu;
		outConContentIdYouKu = sData.outConContentIdYouKu;
		outConAlbumId = sData.outConAlbumId;
		outConContentId = sData.outConContentId;
		ifConsume = sData.ifConsume;
		orderNumber = sData.orderNumber;
		contentName = sData.contentName;
		time = sData.time;
		WatchDuration = sData.WatchDuration;
		sRD._Copy(sData.sRD);
	}
};

struct _DoMain
{
	string epgDomain;
	string bssDomain;
	string searchDomain;
	string bigDataDomain;
	string foundPageURL;
	bool isForceUpdate;
	std::map<string, string> para2;
	string para;
	_DoMain()
	{
		epgDomain = "https://epg.vgs.lenovo.com.cn/";
		bssDomain = "https://bss.vgs.lenovo.com.cn";
		searchDomain = "https://search.vgs.lenovo.com.cn/";
		bigDataDomain = "https://collect.vgs.lenovo.com.cn/";
		isForceUpdate = false;
		para = "cpId=118,119&lSrc=pc&lVer=1&relativeId=62";
	}
};

struct AdStruct
{
	string channelId;
	string moduleId;
	string advLoc;
	string advId;
	string advType;
	string mediaType;
	string clickurl;
	string advCo;
	void _Copy(AdStruct old)
	{
		channelId = old.channelId;
		moduleId = old.moduleId;
		advLoc = old.advLoc;
		advId = old.advId;
		advType = old.advType;
		mediaType = old.mediaType;
		clickurl = old.clickurl;
		advCo = old.advCo;
	}
};

struct _BtnTJ
{
	string sID;
	string enentType;
	string sDes;

	//page
	string sPageId;
	string stoptime;
	string sinto;

	string sVideoId;
	string sIsVip;
	string sFormat;

	//播放
	sReportData sPlay;
	//login
	string sPhone;
	string sMail;
	string sName;
	//adv
	AdStruct sAdv;
	//close
	__int64 sUseTime;
	int nSourceType;
	void _Copy(_BtnTJ old)
	{
		sID = old.sID;
		enentType = old.enentType;
		sDes = old.sDes;

		sPhone = old.sPhone;
		sMail = old.sMail;
		sName = old.sName;


		sPageId = old.sPageId;
		sinto = old.sinto;
		stoptime = old.stoptime;

		sUseTime = old.sUseTime;

		sVideoId = old.sVideoId;
		sIsVip = old.sIsVip;
		sFormat = old.sFormat;
		nSourceType = old.nSourceType;

		//播放
		sPlay._Copy(old.sPlay);
		//adv
		sAdv._Copy(old.sAdv);
	}
};
struct _PageTJ
{
	string sPageId;
	string sPageLevel;
	string sStopTime;
	string sDes;
	int nSourceType;

	void _Copy(_PageTJ old)
	{
		sPageId = old.sPageId;
		sPageLevel = old.sPageLevel;
		sStopTime = old.sStopTime;
		sDes = old.sDes;
		nSourceType = old.nSourceType;
	}
};
struct _PlayTJ
{
	string sVideID;
	string sEpisodeID;
	long lPlaytime;
	string isSucess;
	string sEvendID;
	string isVip;
	int nSourceType;
	void _Copy(_PlayTJ old)
	{
		sVideID = old.sVideID;
		sEpisodeID = old.sEpisodeID;
		lPlaytime = old.lPlaytime;
		isSucess = old.isSucess;
		sEvendID = old.sEvendID;
		isVip = old.isVip;
		nSourceType = old.nSourceType;
	}
};
struct _UserTJ
{
	string sPhone;
	string sName;
	string eventId;
	string type;
	string status;
};