//////////////////////////////////////////////////////////////////////////
// �汾��V1.0
// ������2012.5.19
// ���ߣ���ɭ��
//------------------------------------------------------------------------
// ���ܣ�CRT���ýӿ�
//------------------------------------------------------------------------
// Copyright (c) www.baofeng.com All Rights Reserved
//////////////////////////////////////////////////////////////////////////
#pragma once

//�ı�CRTһ����������
enum CrtConfigValue
{
	CrtConfigValue_Default_PathName = 0,	//Ĭ��·������Ĭ����StormPlayer
	CrtConfigValue_Default_RegKeyName,		//Ĭ��ע����ֵ
	CrtConfigValue_Url_SendLog,				//������־��URL
	CrtConfigValue_Url_ActiveLive,			//���ͱ�������
	CrtConfigValue_Url_CrashReport,			//������������
	CrtConfigValue_Url_CrashUpload,			//�����ļ��ϴ�����
	CrtConfigValue_ProductName,				//��Ʒ����(ȡ�汾��ʱ���õ�������ƶ�Ӧ���ļ�, ������.ver)
	CrtConfigValue_UUID,					//UUIDֵ
	CrtConfigValue_GlobalServiceControl,	//ȫ�����߿��ƣ���Ҫ�Ǳ����ȿ���
};