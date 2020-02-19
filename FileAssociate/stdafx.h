// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>

//开启内存泄露检测
#include "../Include/dbg/MemLeakCheck.h"

#include <tchar.h>

#include <atlbase.h>
#include <atlcom.h>
#include <atlstr.h>
//#include "../../../Include/def/KRDef.h"
#include "../Include/CRTinclude/crt.h"

extern HINSTANCE _hInstance;


//系统注册表键值
#define REG_FILEEXTS						_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts")
#define REG_ASSOC_SETUP						_T("Software\\Lenovo\\Lebo\\FileAssociateList")					//临时：安装时文件关联列表
#define REG_SUBKEY_FILEASSOCIATE			_T("Software\\Lenovo\\Lebo\\UserConfig\\FileAssociate")

//文件关联
#define NAME_FileAssoc						_T("LePlayer.")
#define REG_UAC_AutoRun						_T("Software\\Lenovo\\LePlayer\\Config\\UACAutoRun")			//临时：Win7高级选项用户选择是否自动启动
#define REG_UAC_Associate					_T("Software\\Lenovo\\LePlayer\\Config\\UACAssociate")			//临时：WIN7启动UAC时，文件关联列表
#define REG_UAC_Associate_Module			_T("Software\\Lenovo\\LePlayer\\Config\\UACAssociate\\Module")
#define REG_UAC_Associate_Function			_T("Software\\Lenovo\\LePlayer\\Config\\UACAssociate\\Function")
#define REG_ASSOC_SETUP						_T("Software\\Lenovo\\LePlayer\\FileAssociateList")				//临时：安装时文件关联列表
#define NAME_FileAssoc_Bak					_T("LePlayer.backup")
