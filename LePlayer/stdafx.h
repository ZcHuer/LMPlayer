// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//  are changed infrequently
//

#pragma once

#define WINVER 0x0501
#define  _CRT_SECURE_NO_WARNINGS
#define	 DLL_SOUI
#include <assert.h>
#include <souistd.h>
#include <core/SHostDialog.h>
#include <control/SMessageBox.h>
#include <control/souictrls.h>
#include <res.mgr/sobjdefattr.h>
#include <com-cfg.h>
#include "resource.h"
#include "../SDK/http/Http.h"
#define R_IN_CPP	//定义这个开关来
#include "res\resource.h"
using namespace SOUI;
using namespace std;

#include <helper\SMenuEx.h>
#include <helper\SMenu.h>
#include <control\SCmnCtrl.h>
#include "ctrl\SImageButton1.h"
#include "ctrl\SFlyWnd.h"
#include "ctrl\SListTitle.h"
#include "ctrl\SListWnd.h"
#include "ctrl\SFadeFrame.h"
#include "ctrl\SAdapterBase.h"
#include "ctrl\gif\SSkinGif.h"
#include "ctrl\gif\SGifPlayer.h"
#include "ctrl\gif\SSkinAPNG.h"
#include "ctrl\SAreaButton.h"
#include "ctrl\SLineCtrl.h"
#include "ctrl\SCheckBoxEx.h"
#include "ctrl\SWndBtn.h"
#include "ctrl\SItemWnd.h"
#include "md5.h"
#include <map>
#include <list>
#include <vector>
#include <mutex>
#include "json\json.h"
#include "../include/LeTools.h"
#include "CommonDefine.h"
#include "CommonStruct.h"
#include "../LeDB/LeDB.h"
#include "../include/Data_OffLine.h"
#include "../include/Data_RealTime.h"
#include "Aes_encryptor.h"
#include "../SDK/Flog/FileLog.h"
#include "download\Downloader.h"
#include ".\xml\tinyxml.h"
#include <GdiPlus.h>
#include "CefAdapter/LeCef.h"

using namespace Gdiplus;
#pragma comment(lib,"gdiplus.lib")
using namespace LeConnect;
#include <UrlMon.h>
#pragma comment(lib,"urlmon.lib")

