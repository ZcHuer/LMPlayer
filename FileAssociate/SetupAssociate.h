//////////////////////////////////////////////////////////////////////////
// �汾��V1.0
// ������
// ���ߣ�
//------------------------------------------------------------------------
// ���ܣ���װ�е��ļ����������浽��ʱע���
//------------------------------------------------------------------------
// Copyright (c) www.baofeng.com All Rights Reserved
//////////////////////////////////////////////////////////////////////////
#pragma once

class CSetupAssociate
{
public:
	CSetupAssociate(void);
	virtual ~CSetupAssociate(void);

public:
	static void SetupFileAssociate();
	static void RemoveAllFileAssociate();

	static void InstallByUpdate();

	static bool SaveAssociateState(LPCWSTR lpExt, bool bAssociate);
	static void CleanAllAssociateState();

	//���ô˺���ʱ ��һ����Ȩ�޵�
	static void FileAssociateFromTempWithPower();
};
