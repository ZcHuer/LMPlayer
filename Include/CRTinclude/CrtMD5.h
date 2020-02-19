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

#define MAX_MD5_BUFFER			16
#define MAX_MD5_STRING			42

//ע��MD5�����16�ֽ����ݳ���
MIDL_INTERFACE("1F3F2EB0-BE5C-4e19-8BB0-86D08C86C4CC")
ICrtMD5 : public IUnknown
{
	STDMETHOD(GetMD5)( BYTE* pData, int nDataSize, BYTE* pMD5Value, int nBufferSize)PURE;
	STDMETHOD(GetMD5String)( BYTE* pData, int nLen, WCHAR* pMD5String, int cchMax )PURE;

	STDMETHOD(GetFileMD5String)(LPCWSTR lpFile, WCHAR* pMD5String, int cchMax)PURE;
	STDMETHOD(GetFileMD5)(LPCWSTR pFile, BYTE* pMD5Value, int nBufferSize)PURE;		//nBufferSize>=16

	STDMETHOD(GetVideoMD5String)(LPCWSTR pVideoFile, WCHAR* pMD5String, int cchMax)PURE;
	STDMETHOD(GetVideoMD5)(LPCWSTR pVideoFile, BYTE* pMD5Value, int nBufferSize)PURE;		//nBufferSize>=16
	
	//����CRC����
};
