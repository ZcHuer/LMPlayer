#pragma once

#include "crttype.h"

MIDL_INTERFACE("1AE16C58-92AB-4e9d-82BF-33267296976F")
ICrtFileVer : public IUnknown
{
	STDMETHOD(Open)(LPCWSTR lpFile)PURE;
	STDMETHOD(Close)()PURE;
	STDMETHOD(GetVer)(PCRTVER pVer)PURE;
	STDMETHOD(GetVersionString)(WCHAR* pString, int cchMax)PURE;
	STDMETHOD(GetVersionStringEx)(LPCWSTR lpFormat, WCHAR* pString, int cchMax)PURE;//自定义格式
	STDMETHOD_(long,CompareFile)(LPCWSTR lpSrcFile)PURE;
	STDMETHOD_(long,CompareFileByVer)(PCRTVER pVer)PURE;
	STDMETHOD_(long,CompareFileByVerString)(LPCWSTR lpSrcVer)PURE;
};
