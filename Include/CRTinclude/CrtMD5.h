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

#define MAX_MD5_BUFFER			16
#define MAX_MD5_STRING			42

//注：MD5结果是16字节数据长度
MIDL_INTERFACE("1F3F2EB0-BE5C-4e19-8BB0-86D08C86C4CC")
ICrtMD5 : public IUnknown
{
	STDMETHOD(GetMD5)( BYTE* pData, int nDataSize, BYTE* pMD5Value, int nBufferSize)PURE;
	STDMETHOD(GetMD5String)( BYTE* pData, int nLen, WCHAR* pMD5String, int cchMax )PURE;

	STDMETHOD(GetFileMD5String)(LPCWSTR lpFile, WCHAR* pMD5String, int cchMax)PURE;
	STDMETHOD(GetFileMD5)(LPCWSTR pFile, BYTE* pMD5Value, int nBufferSize)PURE;		//nBufferSize>=16

	STDMETHOD(GetVideoMD5String)(LPCWSTR pVideoFile, WCHAR* pMD5String, int cchMax)PURE;
	STDMETHOD(GetVideoMD5)(LPCWSTR pVideoFile, BYTE* pMD5Value, int nBufferSize)PURE;		//nBufferSize>=16
	
	//加入CRC处理
};
