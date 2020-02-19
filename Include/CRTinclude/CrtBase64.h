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
MIDL_INTERFACE("549A893A-5C44-4BD4-803A-B783A4803186")
ICrtBase64 : public IUnknown
{
public:
	//获取的结果必须调用FreeResult释放内存
	STDMETHOD_(unsigned   char*, Base64Encode)( IN const char *pSrc, IN int nLen );
	STDMETHOD_(unsigned   char*, Base64Decode)( IN const unsigned char *pSrc, IN int nLen,   OUT int *pReturn );

	STDMETHOD(FreeResult)(unsigned   char* pResult)PURE;
};
