//////////////////////////////////////////////////////////////////////////
// 版本：V1.0
// 创建：2010.4.13
// 作者：黄森堂
//------------------------------------------------------------------------
// 功能：CRT最基本的类型
//------------------------------------------------------------------------
// Copyright (c) www.baofeng.com All Rights Reserved
//////////////////////////////////////////////////////////////////////////
#pragma once

#define CRT_INIT_BASIC			0x00000001		//一启动要加载的。
#define CRT_INIT_READY			0x00000002		//业务初始化的时候

#define CRT_UNINIT_DESTORY		0x00000001		//窗口要释放
#define CRT_UNINIT_FINAL		0x00000002		//最终
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
typedef struct _tagCrtVer
{
	WORD dwMajorHigh;
	WORD dwMajorLow;
	WORD dwMinorHigh;
	WORD dwMinorLow;
}CRTVER,*PCRTVER;

enum ComInitType
{
	Thread_Init_None = 0,					//
	Thread_Init_Com,						//CoInitialize
	Thread_Init_Com_Ex,						//CoInitializeEx
	Thread_Init_OLE,						//OleInitialize
};

//////////////////////////////////////////////////////////////////////////
//Thread
//////////////////////////////////////////////////////////////////////////
typedef UINT (WINAPI *PFNCrtThreadProc) (void * pParam);

//连网通知
#define Check_Internet_Connected		0x00000001
#define Check_Internet_Disconnected		0x00000002

typedef void (WINAPI *PFNInternetConnectionChangedEvent)(BOOL bHasInternet, PVOID pThis);

//////////////////////////////////////////////////////////////////////////
//对象类型
#define CRT_Object_Type_UnZip						1
#define CRT_Object_Type_LZMA						2
#define CRT_Object_Type_MD5							3
#define CRT_Object_Type_CPU							4
#define CRT_Object_Type_GPU							5
#define CRT_Object_Type_DXVA						6
#define CRT_Object_Type_FileVer						7
#define CRT_Object_Type_Zip							8
#define CRT_Object_Type_Http						9
#define CRT_Object_Type_Http_Download				10
#define CRT_Object_Type_Http_Upload					11
#define CRT_Object_Type_Http_MultiThreadDownload	12
#define CRT_Object_Type_Base64						13
#define CRT_Object_Type_ZipBuffer                   14

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
//路径范围
enum CrtPathScope
{
	PathScope_Global,				//全局配置
	PathScope_User,					//用户配置
};

// 组合目录或文件名路径时，所用到的 目录类型 
// 见CRT函数 GetConfigFilePath 
enum CrtPathType
{
	PathType_None = 0,
	PathType_Customize,		//自定义路径
	PathType_Profiles,
	PathType_Profiles_Vod,  //loading动画的存放目录
	PathType_Log,
	PathType_Ad,
	PathType_Ad_Temp,
	PathType_Ad_Material,      //广告物料目录
	PathType_Ad_Consult,       //广告协商后的XML存放目录
	PathType_Ad_TxtAdLink,     //广告文字链
	PathType_Package,
	PathType_Codecs,
	PathType_Unzip,
	PathType_MovieList,			//目录树
	PathType_P2P,
	PathType_Update,
	PathType_Temp,
	PathType_Crash,
	PathType_Skin,				//皮肤
	PathType_Exception,			//异退日志
	PathType_ZM,				//转码
};

enum CrtCurrentPath
{
	CurrentPath_None = 0,
	CurrentPath_Customize,		//自定义
	CurrentPath_Skin,			//皮肤
	CurrentPath_Core,			//播放核心
	CurrentPath_Bin,			//功能
	CurrentPath_P2P,			//P2P
	CurrentPath_ZM,				//转码
};

//全局配置范围: config.ini 或Server.ini
enum CrtConfigScope
{
	ConfigScope_Default = 0,		//默认全局配置
	ConfigScope_Control,			//每日更新的控制配置
};

//用户配置类型(注册表)：
enum CrtConfigType
{
	ConfigType_UserConfig = 0,			//用户操作配置
	ConfigType_Config,						//程序运行配置
	ConfigType_Debug,						//Debug配置
	ConfigType_FileAssociate,				//文件关联配置
	ConfigType_Hotkey,						//热键配置
	ConfigType_DirectShow,				//DirectShow配置
	ConfigType_qstp,                       //p2p缓存配置
	ConfigType_Subscribe,				//: 插件2期
	
	ConfigType_Root,					//根
	ConfigType_End,						//这个宏只能放在最后面，不得在下面再放有东东【黄森堂】
};
