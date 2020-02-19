//////////////////////////////////////////////////////////////////////////
// 版本：V1.0
// 创建：2010.8.29
// 作者：黄森堂
//------------------------------------------------------------------------
// 功能：
//------------------------------------------------------------------------
// Copyright (c) www.baofeng.com All Rights Reserved
//////////////////////////////////////////////////////////////////////////
#pragma once
#include "../../DllHelper/DiagHelper.h"
#include "../type/KRType.h"
#include "../core/KRSwitchSolution.h"
#include "../KRMeeDB.h"

typedef struct _tagStateString
{
	KRPLAYERSTATUS eStatus;
	LPCWSTR lpString;
}STATESTRING,*PSTATESTRING;

typedef struct _tagCMStateString
{
	CMPLAYSTATUS cmStatus;
	LPCWSTR lpString;
}CMSTATESTRING,*PCMSTATESTRING;

typedef struct _tagCoreString
{
	CLSID coreClsid;
	LPCWSTR lpString;
}CORESTRING,*PCORESTRING;

class CDiagCore : public CDiagHelper
{
public:
	CDiagCore()
	{

	}
	virtual ~CDiagCore()
	{

	}

public:
	void OnPlayStatusChange(KRPLAYERSTATUS eStatus)
	{
		static STATESTRING States[] =
		{
			{ CM_Opening, L"正在打开", },
			{ CM_Playing, L"正在播放", },
			{ CM_Pause, L"暂停", },
			{ CM_Stoped, L"停止", },
			{ CM_Completed, L"播放完成", },
			{ CM_Closed, L"已经关闭", },
			{ CM_OpenFailed, L"打开失败", },
			{ CM_SwitchFailed, L"切换失败", },
			{ CM_CloudPlay, L"使用云万能", },
			{ CM_Upload, L"上传", },
		};
		int iCnt = ARRAYSIZE(States);
		for( int i=0; i<iCnt; ++i )
		{
			if( States[i].eStatus == eStatus )
			{
				TraceDS( L"[PlayState]%s", States[i].lpString );
				return;
			}
		}
		TraceDS( L"[PlayState]%s", L"未知" );
	}

	void OnCoreStatusChange(LPCWSTR lpCoreName, KRPLAYERSTATUS eStatus)
	{
		static STATESTRING States[] =
		{
			{ PS_Opening, L"正在打开", },
			{ PS_Opened, L"打开完成", },
			{ PS_Playing, L"正在播放", },
			{ PS_Pause, L"暂停", },
			{ PS_Seeking, L"seeking", },
			{ PS_Seeked, L"seek完成", },
			{ PS_Stoping, L"正在停止", },
			{ PS_Stoped, L"停止", },
			{ PS_Completed, L"播放完成", },
			{ PS_Closing, L"正在关闭", },
			{ PS_Closed, L"已经关闭", },
			{ PS_OpenFailed, L"打开失败", },
			{ PS_SwitchFailed, L"链切换失败", },
			{ PS_LoadAudio_Start, L"开始加载外挂音频", },
			{ PS_LoadAudio_OK, L"加载外挂音频成功", },
			{ PS_LoadAudio_Fail, L"加载外挂音频失败", },
		};
		int iCnt = ARRAYSIZE(States);
		for( int i=0; i<iCnt; ++i )
		{
			if( States[i].eStatus == eStatus )
			{
				TraceDS( L"[%s][CoreState]%s", lpCoreName, States[i].lpString );
				return;
			}
		}
		TraceDS( L"[%s][CoreState]%s", lpCoreName, L"未知" );
	}

	void OnMEESwitch(MEE_TYPE TypeSrc, MEE_TYPE TypeDst)
	{
		TraceDS(L"切换MEE: %s==>%s", GetMEE_Name(TypeSrc), GetMEE_Name(TypeDst));
	}

	void OnParserCore(IKRPlayerCoreColl* pCoreInfoColl)
	{
		int cnt = 0;
		CString sCoreArray;
		if (pCoreInfoColl)
		{
			for (int i =0 ; i < pCoreInfoColl->GetPlayCoreCnt(); i++)
			{
				CComPtr<IKRPlayerCore>spCore;
				pCoreInfoColl->EnumPlayCore(i, &spCore);
				if (spCore)
				{
					cnt++;
					sCoreArray += spCore->GetName();
					sCoreArray += L"|";
				}
			}
		}
		TraceDS(L"播放核心列表[%d]: %s", cnt, sCoreArray);
	}

	void NotifyCMStatusChange(CMPLAYSTATUS cmStatus)
	{

	}

};