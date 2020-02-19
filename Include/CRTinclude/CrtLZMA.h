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

interface IRamBuffer :public IUnknown
{
	STDMETHOD_(UINT ,GetSize)()PURE;
	STDMETHOD_(LPVOID,GetPtr)()PURE;
};

MIDL_INTERFACE("807FDA76-5ACB-4032-8969-2EBED1BD07D6")
ICrtLZMA : public IUnknown
{
	STDMETHOD(Uncompress)(BYTE* pBuffer, long nBufferSize, IRamBuffer ** ppOutData)PURE;	//解压缩
	STDMETHOD(Compress)(BYTE* pBuffer, long nBufferSize, IRamBuffer ** ppOutData)PURE;	//压缩
};

//压缩内存
MIDL_INTERFACE("10606F54-9201-4104-9F14-0A0510DF4725")
ICrtZipBuffer : public IUnknown
{
	STDMETHOD(ZipToBuffer)(BYTE* pBuffer, long nBufferSize, IRamBuffer ** ppOutData)PURE;	//解压缩
	STDMETHOD(BufferToZip)(BYTE* pBuffer, long nBufferSize, IRamBuffer ** ppOutData)PURE;	//压缩
	STDMETHOD(UncompressFileData)(LPCWCH pInFile, LPCWCH pOutFile) PURE;	//压缩后的数据存储到文件，从文件中读出，解压到输入文件
};