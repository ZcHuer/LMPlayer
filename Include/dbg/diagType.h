//////////////////////////////////////////////////////////////////////////
// 版本：V1.0
// 创建：2010.8.21
// 作者：黄森堂
//------------------------------------------------------------------------
// 功能：
//------------------------------------------------------------------------
// Copyright (c) www.baofeng.com All Rights Reserved
//////////////////////////////////////////////////////////////////////////
#pragma  once


typedef enum{
	Diag_Scope_Global,
	Diag_Scope_Media
}DiagScope;

typedef enum{
	Diag_Msg_Default,
	Diag_Msg_Error,
	Diag_Msg_Exception,
	Diag_Msg_UI,
	Diag_Msg_Core,
	Diag_Msg_Timer,
	Diag_Msg_Memory,
}DiagMsgType;

#define Diag_Cmd_InitInstance						1		//程序开始
#define Diag_Cmd_ExitInstance						2		//程序退出

#define Diag_Cmd_PlayMedia						3		//pParam=Url;
#define Diag_Cmd_CloseMedia						4

typedef enum{
	Diag_Timer_None,
	Diag_Timer_Start,
	Diag_Timer_Stop,
}DiagTimerType;

#define Diag_Timer_ID_App									1		//主程序记时
#define Diag_Timer_ID_Skin									2		//皮肤引擎记时
#define Diag_Timer_ID_Render								3		//渲染记时
#define Diag_Timer_ID_DSSource							4		//分离器
#define Diag_Timer_ID_DLL									5		//DLL
#define Diag_Timer_ID_CheckByte							6		//取得Checkbyte
#define Diag_Timer_ID_QueryInterface					7		//接口查询
#define Diag_Timer_ID_MatchCodec						8		//匹配解码器速度
#define Diag_Timer_ID_MeeDB								9		//Mee加载
#define Diag_Timer_ID_Playlist								10		//播放列表
#define Diag_Timer_ID_LoadAudio							11		//加载外挂音频
#define Diag_Timer_ID_CreateFilter								12		//创建滤镜记时
#define Diag_Timer_ID_RenderChain								13		//渲染链记时
#define Diag_Timer_ID_AD                                 14    //广告模块加载记时
#define Diag_Timer_ID_CloseCore                              15    //关闭核心


typedef enum{
	Diag_Mem_None,
	Diag_Mem_Start,
	Diag_Mem_Stop,
}DiagMemType;

#define Diag_Mem_ID_App									1		//主程序内存占用
#define Diag_Mem_ID_Skin									2		//皮肤引擎内存占用
#define Diag_Mem_ID_Render								3		//渲染内存占用
#define Diag_Mem_ID_DSSource							4		//分离器

//: 对象引用计数监控结构
typedef enum {
	Diag_Object_None,
	Diag_Object_Com,
	Diag_Object_Class,
	Diag_Object_Dll,
}DiagObject;
