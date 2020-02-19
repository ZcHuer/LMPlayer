// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�:
#include <windows.h>

//�����ڴ�й¶���
#include "../Include/dbg/MemLeakCheck.h"

#include <tchar.h>

#include <atlbase.h>
#include <atlcom.h>
#include <atlstr.h>
//#include "../../../Include/def/KRDef.h"
#include "../Include/CRTinclude/crt.h"

extern HINSTANCE _hInstance;


//ϵͳע����ֵ
#define REG_FILEEXTS						_T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts")
#define REG_ASSOC_SETUP						_T("Software\\Lenovo\\Lebo\\FileAssociateList")					//��ʱ����װʱ�ļ������б�
#define REG_SUBKEY_FILEASSOCIATE			_T("Software\\Lenovo\\Lebo\\UserConfig\\FileAssociate")

//�ļ�����
#define NAME_FileAssoc						_T("LePlayer.")
#define REG_UAC_AutoRun						_T("Software\\Lenovo\\LePlayer\\Config\\UACAutoRun")			//��ʱ��Win7�߼�ѡ���û�ѡ���Ƿ��Զ�����
#define REG_UAC_Associate					_T("Software\\Lenovo\\LePlayer\\Config\\UACAssociate")			//��ʱ��WIN7����UACʱ���ļ������б�
#define REG_UAC_Associate_Module			_T("Software\\Lenovo\\LePlayer\\Config\\UACAssociate\\Module")
#define REG_UAC_Associate_Function			_T("Software\\Lenovo\\LePlayer\\Config\\UACAssociate\\Function")
#define REG_ASSOC_SETUP						_T("Software\\Lenovo\\LePlayer\\FileAssociateList")				//��ʱ����װʱ�ļ������б�
#define NAME_FileAssoc_Bak					_T("LePlayer.backup")
