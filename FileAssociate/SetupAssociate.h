//////////////////////////////////////////////////////////////////////////
// 版本：V1.0
// 创建：
// 作者：
//------------------------------------------------------------------------
// 功能：安装中的文件关联。保存到临时注册表
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

	//调用此函数时 是一定有权限的
	static void FileAssociateFromTempWithPower();
};
