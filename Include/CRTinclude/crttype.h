//////////////////////////////////////////////////////////////////////////
// �汾��V1.0
// ������2010.4.13
// ���ߣ���ɭ��
//------------------------------------------------------------------------
// ���ܣ�CRT�����������
//------------------------------------------------------------------------
// Copyright (c) www.baofeng.com All Rights Reserved
//////////////////////////////////////////////////////////////////////////
#pragma once

#define CRT_INIT_BASIC			0x00000001		//һ����Ҫ���صġ�
#define CRT_INIT_READY			0x00000002		//ҵ���ʼ����ʱ��

#define CRT_UNINIT_DESTORY		0x00000001		//����Ҫ�ͷ�
#define CRT_UNINIT_FINAL		0x00000002		//����
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
typedef struct _tagCrtVer
{
	WORD dwMajorHigh;
	WORD dwMajorLow;
	WORD dwMinorHigh;
	WORD dwMinorLow;
}CRTVER,*PCRTVER;

enum ComInitType
{
	Thread_Init_None = 0,					//
	Thread_Init_Com,						//CoInitialize
	Thread_Init_Com_Ex,						//CoInitializeEx
	Thread_Init_OLE,						//OleInitialize
};

//////////////////////////////////////////////////////////////////////////
//Thread
//////////////////////////////////////////////////////////////////////////
typedef UINT (WINAPI *PFNCrtThreadProc) (void * pParam);

//����֪ͨ
#define Check_Internet_Connected		0x00000001
#define Check_Internet_Disconnected		0x00000002

typedef void (WINAPI *PFNInternetConnectionChangedEvent)(BOOL bHasInternet, PVOID pThis);

//////////////////////////////////////////////////////////////////////////
//��������
#define CRT_Object_Type_UnZip						1
#define CRT_Object_Type_LZMA						2
#define CRT_Object_Type_MD5							3
#define CRT_Object_Type_CPU							4
#define CRT_Object_Type_GPU							5
#define CRT_Object_Type_DXVA						6
#define CRT_Object_Type_FileVer						7
#define CRT_Object_Type_Zip							8
#define CRT_Object_Type_Http						9
#define CRT_Object_Type_Http_Download				10
#define CRT_Object_Type_Http_Upload					11
#define CRT_Object_Type_Http_MultiThreadDownload	12
#define CRT_Object_Type_Base64						13
#define CRT_Object_Type_ZipBuffer                   14

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////
//·����Χ
enum CrtPathScope
{
	PathScope_Global,				//ȫ������
	PathScope_User,					//�û�����
};

// ���Ŀ¼���ļ���·��ʱ�����õ��� Ŀ¼���� 
// ��CRT���� GetConfigFilePath 
enum CrtPathType
{
	PathType_None = 0,
	PathType_Customize,		//�Զ���·��
	PathType_Profiles,
	PathType_Profiles_Vod,  //loading�����Ĵ��Ŀ¼
	PathType_Log,
	PathType_Ad,
	PathType_Ad_Temp,
	PathType_Ad_Material,      //�������Ŀ¼
	PathType_Ad_Consult,       //���Э�̺��XML���Ŀ¼
	PathType_Ad_TxtAdLink,     //���������
	PathType_Package,
	PathType_Codecs,
	PathType_Unzip,
	PathType_MovieList,			//Ŀ¼��
	PathType_P2P,
	PathType_Update,
	PathType_Temp,
	PathType_Crash,
	PathType_Skin,				//Ƥ��
	PathType_Exception,			//������־
	PathType_ZM,				//ת��
};

enum CrtCurrentPath
{
	CurrentPath_None = 0,
	CurrentPath_Customize,		//�Զ���
	CurrentPath_Skin,			//Ƥ��
	CurrentPath_Core,			//���ź���
	CurrentPath_Bin,			//����
	CurrentPath_P2P,			//P2P
	CurrentPath_ZM,				//ת��
};

//ȫ�����÷�Χ: config.ini ��Server.ini
enum CrtConfigScope
{
	ConfigScope_Default = 0,		//Ĭ��ȫ������
	ConfigScope_Control,			//ÿ�ո��µĿ�������
};

//�û���������(ע���)��
enum CrtConfigType
{
	ConfigType_UserConfig = 0,			//�û���������
	ConfigType_Config,						//������������
	ConfigType_Debug,						//Debug����
	ConfigType_FileAssociate,				//�ļ���������
	ConfigType_Hotkey,						//�ȼ�����
	ConfigType_DirectShow,				//DirectShow����
	ConfigType_qstp,                       //p2p��������
	ConfigType_Subscribe,				//: ���2��
	
	ConfigType_Root,					//��
	ConfigType_End,						//�����ֻ�ܷ�������棬�����������ٷ��ж�������ɭ�á�
};
