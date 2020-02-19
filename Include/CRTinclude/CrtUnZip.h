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

MIDL_INTERFACE("683D4476-3B40-4b0f-B08A-C0F82B726095")
IUnZipBuffer : public IUnknown
{
	STDMETHOD_(long,GetZipType)()PURE;
	STDMETHOD_(BYTE*,GetData)()PURE;
	STDMETHOD_(long,GetLength)()PURE;
	STDMETHOD(Allocate)(long iBufferSize)PURE;
	STDMETHOD(GetXMLString)(BSTR* ppString)PURE;
	STDMETHOD_(HGLOBAL,GetHandle)()PURE;
};

MIDL_INTERFACE("132CEF4F-0599-49f7-9E2E-5521911B7748")
IUnZipFileName : public IUnknown
{
	STDMETHOD_(long,GetCount)()PURE;
	STDMETHOD(GetFileName)(long iIndex, BSTR* ppString)PURE;
	STDMETHOD(GetFilePath)(long iIndex, BSTR* ppString)PURE;
};

//��ѹ֧��Zip, cab, rar
MIDL_INTERFACE("683D4476-3B40-4b0f-B08A-C0F82B726095")
ICrtUnZip : public IUnknown
{
	STDMETHOD(ExtractToDirectory)(LPCWSTR lpZipFile, LPCWSTR lpOutputPath, IUnZipFileName** ppZipName,BOOL bExistOverwrite)PURE;//��ȡ�ļ��б�,ֱ�ӽ�ѹ��������OPEN��
	STDMETHOD(GetFileNameList)(LPCWSTR lpZipFile, IUnZipFileName** ppZipName)PURE;
	STDMETHOD(Open)(LPCWSTR lpZipFile)PURE;
	STDMETHOD(Close)()PURE;
	STDMETHOD(ExtractToBuffer)(LPCWSTR lpFileName, IUnZipBuffer** ppBuffer)PURE;
	STDMETHOD(ExtractToFile)(LPCWSTR lpFileName, LPCWSTR lpLocalFile)PURE;
	STDMETHOD(ExtractToXMLString)(LPCWSTR lpFileName, BSTR* ppString)PURE;
};
