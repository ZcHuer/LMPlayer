
// ConfigManager.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CConfigManagerApp: 
// �йش����ʵ�֣������ ConfigManager.cpp
//

class CConfigManagerApp : public CWinApp
{
public:
	CConfigManagerApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CConfigManagerApp theApp;