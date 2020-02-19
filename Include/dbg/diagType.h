//////////////////////////////////////////////////////////////////////////
// �汾��V1.0
// ������2010.8.21
// ���ߣ���ɭ��
//------------------------------------------------------------------------
// ���ܣ�
//------------------------------------------------------------------------
// Copyright (c) www.baofeng.com All Rights Reserved
//////////////////////////////////////////////////////////////////////////
#pragma  once


typedef enum{
	Diag_Scope_Global,
	Diag_Scope_Media
}DiagScope;

typedef enum{
	Diag_Msg_Default,
	Diag_Msg_Error,
	Diag_Msg_Exception,
	Diag_Msg_UI,
	Diag_Msg_Core,
	Diag_Msg_Timer,
	Diag_Msg_Memory,
}DiagMsgType;

#define Diag_Cmd_InitInstance						1		//����ʼ
#define Diag_Cmd_ExitInstance						2		//�����˳�

#define Diag_Cmd_PlayMedia						3		//pParam=Url;
#define Diag_Cmd_CloseMedia						4

typedef enum{
	Diag_Timer_None,
	Diag_Timer_Start,
	Diag_Timer_Stop,
}DiagTimerType;

#define Diag_Timer_ID_App									1		//�������ʱ
#define Diag_Timer_ID_Skin									2		//Ƥ�������ʱ
#define Diag_Timer_ID_Render								3		//��Ⱦ��ʱ
#define Diag_Timer_ID_DSSource							4		//������
#define Diag_Timer_ID_DLL									5		//DLL
#define Diag_Timer_ID_CheckByte							6		//ȡ��Checkbyte
#define Diag_Timer_ID_QueryInterface					7		//�ӿڲ�ѯ
#define Diag_Timer_ID_MatchCodec						8		//ƥ��������ٶ�
#define Diag_Timer_ID_MeeDB								9		//Mee����
#define Diag_Timer_ID_Playlist								10		//�����б�
#define Diag_Timer_ID_LoadAudio							11		//���������Ƶ
#define Diag_Timer_ID_CreateFilter								12		//�����˾���ʱ
#define Diag_Timer_ID_RenderChain								13		//��Ⱦ����ʱ
#define Diag_Timer_ID_AD                                 14    //���ģ����ؼ�ʱ
#define Diag_Timer_ID_CloseCore                              15    //�رպ���


typedef enum{
	Diag_Mem_None,
	Diag_Mem_Start,
	Diag_Mem_Stop,
}DiagMemType;

#define Diag_Mem_ID_App									1		//�������ڴ�ռ��
#define Diag_Mem_ID_Skin									2		//Ƥ�������ڴ�ռ��
#define Diag_Mem_ID_Render								3		//��Ⱦ�ڴ�ռ��
#define Diag_Mem_ID_DSSource							4		//������

//: �������ü�����ؽṹ
typedef enum {
	Diag_Object_None,
	Diag_Object_Com,
	Diag_Object_Class,
	Diag_Object_Dll,
}DiagObject;
