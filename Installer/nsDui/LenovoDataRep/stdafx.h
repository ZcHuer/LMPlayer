// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"



#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����

// Windows ͷ�ļ�: 
//#include <windows.h>
#include "WMI/WMIInfo.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

#include <atlbase.h>
#include <atlstr.h>

#include <time.h>

// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
using namespace std;
#include <iostream>
#include <sstream>

#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")

#include "AesEncode/Aes_encryptor.h"
#include "base64/base641.h"
#include "MD5/md5.h"
#include "MD5Hex.h"
