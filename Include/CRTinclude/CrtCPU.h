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

#define CPU_VENDOR_UNKNOWN		0
#define CPU_VENDOR_INTEL				1
#define CPU_VENDOR_AMD					2
#define CPU_VENDOR_VIA					3

#define CPU_SET_MMX							0x00000001
#define CPU_SET_SSE							0x00000002
#define CPU_SET_SSE2							0x00000004
#define CPU_SET_SSE3							0x00000008
#define CPU_SET_SSSE3						0x00000010
#define CPU_SET_SSE41						0x00000020
#define CPU_SET_SSE42						0x00000040


MIDL_INTERFACE("0DFE94D7-1F0E-4fa0-B8FF-684600142FFE")
ICrtCPU : public IUnknown
{
	STDMETHOD(IsSupportSet)(DWORD dwSet)PURE;
	STDMETHOD(GetName)(WCHAR* pCPUName,  int cchMax)PURE;
	STDMETHOD(GetVendorName)(WCHAR* pName,  int cchMax)PURE;
	STDMETHOD_(long, GetVendor)()PURE;
	STDMETHOD_(long, GetCoreCount)()PURE;
};

//////////////////////////////////////////////////////////////////////////
#define GPU_VENDOR_UNKNOWN		0
#define GPU_VENDOR_INTEL				1
#define GPU_VENDOR_AMD					2
#define GPU_VENDOR_NVIDIA				3
#define GPU_VENDOR_VIA					4

#define GPU_HDCHIP_BROADCOM		1
#define GPU_HDCHIP_Quartics				2

MIDL_INTERFACE("A9D7FE56-5A30-4c42-8C90-CD71F4B56F3A")
ICrtGPU : public IUnknown
{
	STDMETHOD(GetName)(WCHAR* pGPUName, int cchMax)PURE;
	STDMETHOD_(long, GetVendor)()PURE;
	STDMETHOD(GetVendorString)(WCHAR* pString, int cchMax)PURE;
	STDMETHOD_(bool, IsIntegrateGPU)()PURE;
	STDMETHOD_(bool, IsDirectDraw)()PURE;
	STDMETHOD_(bool, IsUseMicrosoftDriver)()PURE;
	STDMETHOD(GetDriverString)(WCHAR* pString, int cchMax)PURE;
	STDMETHOD(GetDriverVer)(WCHAR* pString, int cchMax)PURE;
	STDMETHOD_(bool, IsSupportCUDA)()PURE;
};

//////////////////////////////////////////////////////////////////////////
#define DXVA_TYPE_Unknown				0
#define DXVA_TYPE_MPEG2					1
#define DXVA_TYPE_H264					2
#define DXVA_TYPE_WVC1					4

#define DXVA_Level_PostProc				1
#define DXVA_Level_MoComp				2
#define DXVA_Level_IDCT						3
#define DXVA_Level_VLD						4
#define DXVA_Level_NoFGT					5
#define DXVA_Level_FGT						6


MIDL_INTERFACE("6B9B2DBF-12BE-42c1-8FEB-A6C3DB9A29E7")
ICrtDXVA : public IUnknown
{
	STDMETHOD(Initialize)()PURE;
	STDMETHOD(UnInitialize)()PURE;
	STDMETHOD_(UINT,GetGuidCount)()PURE;
	STDMETHOD(Enum)(long iIndex, GUID* pGuid)PURE;
	STDMETHOD(GetGuid)(GUID* pGuidArray)PURE;
	STDMETHOD_(LPCWSTR,GetString)(GUID* pGuid)PURE;
	STDMETHOD_(bool,IsDXVAGuid)(GUID* pGuid)PURE;
	STDMETHOD_(long, GetType)(GUID* pGuid)PURE;
	STDMETHOD_(DWORD, GetLevel)(GUID* pGuid)PURE;
	STDMETHOD_(bool, IsSupportHDVideo)()PURE;
	STDMETHOD_(DWORD, GetGPUDXVA)()PURE;
};
