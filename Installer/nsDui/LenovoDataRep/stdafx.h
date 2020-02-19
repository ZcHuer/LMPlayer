// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"



#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料

// Windows 头文件: 
//#include <windows.h>
#include "WMI/WMIInfo.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

#include <atlbase.h>
#include <atlstr.h>

#include <time.h>

// TODO:  在此处引用程序需要的其他头文件
using namespace std;
#include <iostream>
#include <sstream>

#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")

#include "AesEncode/Aes_encryptor.h"
#include "base64/base641.h"
#include "MD5/md5.h"
#include "MD5Hex.h"
