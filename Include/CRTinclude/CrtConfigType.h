//////////////////////////////////////////////////////////////////////////
// 版本：V1.0
// 创建：2012.5.19
// 作者：黄森堂
//------------------------------------------------------------------------
// 功能：CRT配置接口
//------------------------------------------------------------------------
// Copyright (c) www.baofeng.com All Rights Reserved
//////////////////////////////////////////////////////////////////////////
#pragma once

//改变CRT一个设置内容
enum CrtConfigValue
{
	CrtConfigValue_Default_PathName = 0,	//默认路径名，默认是StormPlayer
	CrtConfigValue_Default_RegKeyName,		//默认注册表键值
	CrtConfigValue_Url_SendLog,				//发送日志的URL
	CrtConfigValue_Url_ActiveLive,			//发送报活链接
	CrtConfigValue_Url_CrashReport,			//崩溃报数链接
	CrtConfigValue_Url_CrashUpload,			//崩溃文件上传链接
	CrtConfigValue_ProductName,				//产品名称(取版本号时会用到这个名称对应的文件, 即名称.ver)
	CrtConfigValue_UUID,					//UUID值
	CrtConfigValue_GlobalServiceControl,	//全局在线控制，主要是崩溃等控制
};