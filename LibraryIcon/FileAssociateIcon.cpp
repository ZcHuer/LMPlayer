#include "stdafx.h"
#include "FileAssociateIcon.h"
#include "resource.h"

CFileAssociateIcon::CFileAssociateIcon(void)
{

}

CFileAssociateIcon::~CFileAssociateIcon(void)
{

}

struct FileExtInfo
{
	LPCWSTR lpExt;
	UINT    uIconId;
	BOOL    bSelected;
};

FileExtInfo g_ExtInfoTable[] = 
{
	{_T("asf"), IDI_ASF,TRUE},
	{_T("wm"),  IDI_WM,TRUE},
	{_T("wmp"), IDI_WMP,TRUE},
	{_T("wmv"), IDI_WMV,TRUE},
	{_T("wma"), IDI_WMA,TRUE},

	{_T("real"),IDI_REAL,FALSE},
	{_T("ram"), IDI_REAL,TRUE},
	{_T("rm"),  IDI_REAL,TRUE},
	{_T("rmvb"),IDI_REAL,TRUE},
	{_T("rpm"), IDI_REAL,TRUE},
	{_T("scm"), IDI_REAL,FALSE},
	{_T("ra"),  IDI_REAL,TRUE},
	{_T("rp"),  IDI_REAL,FALSE},


	{_T("rp"),  IDI_REAL,FALSE},
	{_T("rt"),  IDI_REAL,TRUE},
	{_T("smi"),  IDI_REAL,TRUE},
	{_T("smil"),  IDI_REAL,TRUE},

	{_T("dat"), IDI_DAT,FALSE},
	{_T("evo"), IDI_MPEG,TRUE},
	/* {_T("ifo"), IDI_MPEG,TRUE}, */

	{_T("mov"), IDI_QT,TRUE},
	{_T("qt"),  IDI_QT,TRUE},
	{_T("aif"), IDI_QT,TRUE},
	{_T("aifc"), IDI_QT,FALSE},
	{_T("aiff"), IDI_QT,TRUE},

	{_T("3g2"), IDI_QT,TRUE},
	{_T("3gp"), IDI_QT,TRUE},
	{_T("3gp2"),IDI_QT,TRUE},
	{_T("3gpp"),IDI_QT,TRUE},
	{_T("amr"), IDI_QT,TRUE},

	{_T("amv"), IDI_DEFAULT,TRUE},
	{_T("avi"), IDI_AVI,TRUE},
	{_T("bik"), IDI_DEFAULT,TRUE},
	{_T("csf"), IDI_DEFAULT,TRUE},
	{_T("d2v"), IDI_DEFAULT,TRUE},
	{_T("dsm"), IDI_DEFAULT,TRUE},
	{_T("dsf"), IDI_DEFAULT,FALSE},
	{_T("dsv"), IDI_DEFAULT,TRUE},
	{_T("ivf"), IDI_DEFAULT,TRUE},
	{_T("m1v"), IDI_DEFAULT,TRUE},
	{_T("m2p"), IDI_DEFAULT,TRUE},
	{_T("asm"), IDI_DEFAULT,TRUE},

	{_T("avsts"), IDI_DEFAULT,TRUE},
	{_T("divx"), IDI_DEFAULT,TRUE},
	{_T("webm"), IDI_DEFAULT,TRUE},


	{_T("m2ts"), IDI_MPEG,TRUE},
	{_T("m2v"), IDI_DEFAULT,TRUE},

	{_T("m4b"), IDI_M4B,TRUE},
	{_T("m4p"), IDI_M4P,TRUE},
	{_T("m4v"), IDI_M4V,TRUE},
	{_T("mkv"), IDI_MKV,TRUE},
	{_T("mp4"), IDI_MP4,TRUE},
	{_T("mpe"), IDI_DVD,TRUE},

	{_T("mpeg"), IDI_MPEG,TRUE},
	{_T("mpg"),  IDI_MPG,TRUE},
	{_T("mts"),  IDI_DEFAULT,TRUE},
	{_T("ogm"),  IDI_OGM,TRUE},
	{_T("pmp"),  IDI_DEFAULT,TRUE},
	{_T("pmp2"), IDI_DEFAULT,TRUE},
	{_T("pss"),  IDI_PSS,TRUE},
	{_T("pva"),  IDI_PVA,TRUE},

	{_T("ratDVD"), IDI_DEFAULT,TRUE},
	{_T("smk"), IDI_DEFAULT,TRUE},
	{_T("tp"),  IDI_TP,TRUE},
	{_T("tpr"), IDI_TPR,TRUE},
	{_T("ts"),  IDI_TS,TRUE},
	{_T("vg2"), IDI_DEFAULT,TRUE},
	{_T("vid"), IDI_VID,FALSE},

	{_T("vob"), IDI_VOB,TRUE},
	{_T("vp6"), IDI_DEFAULT,TRUE},
	{_T("vp7"), IDI_DEFAULT,TRUE},
	{_T("wv"),  IDI_MPEG,TRUE},

	{_T("aac"), IDI_AUDIO,FALSE},
	{_T("ac3"), IDI_DEFAULT,TRUE},
	{_T("acc"), IDI_DEFAULT,TRUE},
	{_T("act"), IDI_DEFAULT,TRUE},
	{_T("ape"), IDI_AUDIO,FALSE},
	{_T("au"),  IDI_DEFAULT,TRUE},
	{_T("cda"), IDI_AUDIO,FALSE},
	{_T("dsa"), IDI_DEFAULT,TRUE},
	{_T("dss"), IDI_DEFAULT,TRUE},

	{_T("dts"),  IDI_DEFAULT,TRUE},
	{_T("flac"), IDI_AUDIO,FALSE},
	{_T("m4a"),  IDI_AUDIO,FALSE},
	{_T("mac"),  IDI_AUDIO,FALSE},
	{_T("mid"),  IDI_DEFAULT,FALSE},
	{_T("midi"), IDI_DEFAULT,FALSE},
	{_T("mp2"),  IDI_DEFAULT,TRUE},
	{_T("mp3"),  IDI_MP3,FALSE},
	{_T("mp5"),  IDI_DEFAULT,FALSE},
	{_T("mpa"),  IDI_DEFAULT,FALSE},
	{_T("mpga"), IDI_DEFAULT,FALSE},
	{_T("mod"), IDI_DEFAULT,TRUE},
	{_T("ogg"), IDI_DEFAULT,FALSE},
	{_T("ofr"), IDI_AUDIO,FALSE},
	{_T("rmi"), IDI_DEFAULT,FALSE},
	{_T("tak"), IDI_AUDIO,FALSE},
	{_T("tta"), IDI_AUDIO,FALSE},
	{_T("wav"), IDI_AUDIO,FALSE},

	{_T("swf"), IDI_FLASH,FALSE},
	{_T("flv"), IDI_FLASH,TRUE},
	{_T("flic"), IDI_DEFAULT,TRUE},
	{_T("fli"), IDI_DEFAULT,TRUE},
	{_T("flc"), IDI_DEFAULT,TRUE},
		
	{_T("LISI"),IDI_SUPL,FALSE},
	{_T("asx"), IDI_ASX,TRUE},
	{_T("m3u"), IDI_M3U,TRUE},
	{_T("mpcpl"), IDI_MPCPL,TRUE},
	{_T("pls"), IDI_SUPL,TRUE},
	{_T("smpl"), IDI_SMPL,TRUE},
	{_T("wax"), IDI_WAX,TRUE},
	{_T("wmx"), IDI_WMX,TRUE},
	{_T("wvx"), IDI_WVX,TRUE},

	{_T("ass"), IDI_ASS,TRUE},
	{_T("cue"), IDI_CUE,FALSE},  
	{_T("idx"), IDI_IDX,FALSE},  
	{_T("srt"), IDI_SRT,TRUE},  
	{_T("ssa"), IDI_SSA,TRUE},  
	{_T("sub"), IDI_SUB,FALSE},
	{_T("supl"),IDI_SUPL,FALSE},

	{_T("bf"),IDI_DEFAULT,FALSE},		//chenjindu 加了一个缺省图标

	{_T("bhd"),IDI_BHD,TRUE},
	{_T("ghd"),IDI_GHD,TRUE},
	{_T("divx"),IDI_DIVX,TRUE},
	{_T("ifo"),IDI_IFO,TRUE},

	{_T("bov"),IDI_BOV,TRUE},
	{_T("bfp"),IDI_BFP,TRUE},
	{_T("sup"),IDI_SUP,TRUE},
};

UINT CFileAssociateIcon::GetIconId(LPCWSTR lpExt)
{
	int iCnt = ARRAYSIZE(g_ExtInfoTable);
	for(int i=0; i<iCnt; i++ )
	{
		if( _tcsicmp(g_ExtInfoTable[i].lpExt, lpExt) == 0 )
		{
			return g_ExtInfoTable[i].uIconId;
		}
	}

	return 0;
}

HICON CFileAssociateIcon::GetIcon(LPCWSTR lpExt)
{
	return NULL;

	int iCnt = ARRAYSIZE(g_ExtInfoTable);
	for(int i=0; i<iCnt; i++ )
	{
		if( _tcsicmp(g_ExtInfoTable[i].lpExt, lpExt) == 0 )
		{
			return LoadIcon( _hInstance, MAKEINTRESOURCE(g_ExtInfoTable[i].uIconId) );
		}
	}

	return NULL;
}

BOOL CFileAssociateIcon::IsDefaultAss(LPCWSTR lpExt)
{
	int iCnt = ARRAYSIZE(g_ExtInfoTable);
	for(int i=0; i<iCnt; i++ )
	{
		if( _tcsicmp(g_ExtInfoTable[i].lpExt, lpExt) == 0 )
		{
			return g_ExtInfoTable[i].bSelected;
		}
	}
	return FALSE;
}

LPCWSTR CFileAssociateIcon::EnumExt(DWORD dwIndex)
{
	DWORD iCnt = ARRAYSIZE(g_ExtInfoTable);
	if( dwIndex < iCnt )
		return g_ExtInfoTable[dwIndex].lpExt;
	return NULL;
}

