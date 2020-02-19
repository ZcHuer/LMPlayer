// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件: 
#include <windows.h>

// TODO:  在此处引用程序需要的其他头文件
#include <atlbase.h>
#include <atlwin.h>
#include <atlapp.h>
#include <atlframe.h>
#include <GdiPlus.h>
using namespace Gdiplus;
#pragma comment(lib,"gdiplus.lib")

// 向窗口发送的播放进度消息
enum ePlayerMsg
{
	EPLAYERMSG_START = WM_USER + 1,		// 播放开始
	EPLAYERMSG_PLAYING,					// 播放进行中
	EPLAYERMSG_END,						// 播放结束
	EPLAYERMSG_FILE_INVALID,			// 文件无效
};