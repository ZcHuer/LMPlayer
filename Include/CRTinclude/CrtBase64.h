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

//ѹ��֧��Zip
MIDL_INTERFACE("549A893A-5C44-4BD4-803A-B783A4803186")
ICrtBase64 : public IUnknown
{
public:
	//��ȡ�Ľ���������FreeResult�ͷ��ڴ�
	STDMETHOD_(unsigned   char*, Base64Encode)( IN const char *pSrc, IN int nLen );
	STDMETHOD_(unsigned   char*, Base64Decode)( IN const unsigned char *pSrc, IN int nLen,   OUT int *pReturn );

	STDMETHOD(FreeResult)(unsigned   char* pResult)PURE;
};
