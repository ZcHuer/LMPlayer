// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�: 
#include <windows.h>

// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include <atlbase.h>
#include <atlwin.h>
#include <atlapp.h>
#include <atlframe.h>
#include <GdiPlus.h>
using namespace Gdiplus;
#pragma comment(lib,"gdiplus.lib")

// �򴰿ڷ��͵Ĳ��Ž�����Ϣ
enum ePlayerMsg
{
	EPLAYERMSG_START = WM_USER + 1,		// ���ſ�ʼ
	EPLAYERMSG_PLAYING,					// ���Ž�����
	EPLAYERMSG_END,						// ���Ž���
	EPLAYERMSG_FILE_INVALID,			// �ļ���Ч
};