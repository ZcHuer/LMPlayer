//////////////////////////////////////////////////////////////////////////
// �汾��V1.0
// ������2010.8.29
// ���ߣ���ɭ��
//------------------------------------------------------------------------
// ���ܣ�
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
			{ CM_Opening, L"���ڴ�", },
			{ CM_Playing, L"���ڲ���", },
			{ CM_Pause, L"��ͣ", },
			{ CM_Stoped, L"ֹͣ", },
			{ CM_Completed, L"�������", },
			{ CM_Closed, L"�Ѿ��ر�", },
			{ CM_OpenFailed, L"��ʧ��", },
			{ CM_SwitchFailed, L"�л�ʧ��", },
			{ CM_CloudPlay, L"ʹ��������", },
			{ CM_Upload, L"�ϴ�", },
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
		TraceDS( L"[PlayState]%s", L"δ֪" );
	}

	void OnCoreStatusChange(LPCWSTR lpCoreName, KRPLAYERSTATUS eStatus)
	{
		static STATESTRING States[] =
		{
			{ PS_Opening, L"���ڴ�", },
			{ PS_Opened, L"�����", },
			{ PS_Playing, L"���ڲ���", },
			{ PS_Pause, L"��ͣ", },
			{ PS_Seeking, L"seeking", },
			{ PS_Seeked, L"seek���", },
			{ PS_Stoping, L"����ֹͣ", },
			{ PS_Stoped, L"ֹͣ", },
			{ PS_Completed, L"�������", },
			{ PS_Closing, L"���ڹر�", },
			{ PS_Closed, L"�Ѿ��ر�", },
			{ PS_OpenFailed, L"��ʧ��", },
			{ PS_SwitchFailed, L"���л�ʧ��", },
			{ PS_LoadAudio_Start, L"��ʼ���������Ƶ", },
			{ PS_LoadAudio_OK, L"���������Ƶ�ɹ�", },
			{ PS_LoadAudio_Fail, L"���������Ƶʧ��", },
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
		TraceDS( L"[%s][CoreState]%s", lpCoreName, L"δ֪" );
	}

	void OnMEESwitch(MEE_TYPE TypeSrc, MEE_TYPE TypeDst)
	{
		TraceDS(L"�л�MEE: %s==>%s", GetMEE_Name(TypeSrc), GetMEE_Name(TypeDst));
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
		TraceDS(L"���ź����б�[%d]: %s", cnt, sCoreArray);
	}

	void NotifyCMStatusChange(CMPLAYSTATUS cmStatus)
	{

	}

};