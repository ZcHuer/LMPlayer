//////////////////////////////////////////////////////////////////////////
// 版本：V1.0
// 创建：2010.4.11
// 作者：黄森堂
//------------------------------------------------------------------------
// 功能：
//------------------------------------------------------------------------
// Copyright (c) www.baofeng.com All Rights Reserved
//////////////////////////////////////////////////////////////////////////
#pragma once

//压缩支持Zip
MIDL_INTERFACE("4B78A538-8256-41be-AB33-B4CA3DD592D6")
ICrtZip : public IUnknown
{
	STDMETHOD(Create)(LPCWSTR lpZipFile)PURE;
	STDMETHOD(Open)(LPCWSTR lpZipFile)PURE;
	STDMETHOD(Close)()PURE;
	STDMETHOD(Append)(LPCWSTR lpFile)PURE;
};
