//////////////////////////////////////////////////////////////////////////
// �汾��V1.0
// ������2010.4.11
// ���ߣ���ɭ��
//------------------------------------------------------------------------
// ���ܣ�
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
	STDMETHOD(Uncompress)(BYTE* pBuffer, long nBufferSize, IRamBuffer ** ppOutData)PURE;	//��ѹ��
	STDMETHOD(Compress)(BYTE* pBuffer, long nBufferSize, IRamBuffer ** ppOutData)PURE;	//ѹ��
};

//ѹ���ڴ�
MIDL_INTERFACE("10606F54-9201-4104-9F14-0A0510DF4725")
ICrtZipBuffer : public IUnknown
{
	STDMETHOD(ZipToBuffer)(BYTE* pBuffer, long nBufferSize, IRamBuffer ** ppOutData)PURE;	//��ѹ��
	STDMETHOD(BufferToZip)(BYTE* pBuffer, long nBufferSize, IRamBuffer ** ppOutData)PURE;	//ѹ��
	STDMETHOD(UncompressFileData)(LPCWCH pInFile, LPCWCH pOutFile) PURE;	//ѹ��������ݴ洢���ļ������ļ��ж�������ѹ�������ļ�
};