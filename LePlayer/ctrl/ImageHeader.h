#pragma once
namespace SOUI
{
	class CImageHeader :
		public SImageWnd
	{
		SOUI_CLASS_NAME(CImageHeader, L"titleheader")
	public:
		CImageHeader();
		~CImageHeader();
		void OnPaint(IRenderTarget *pRT);
		HRESULT OnAttrMask(const SStringW & strValue, BOOL bLoading);
		HRESULT OnAttrImage(const SStringW & strValue, BOOL bLoading);

		void MakeCacheApha(ISkinObj *pSkin);
		void MakeCacheApha();
		SOUI_MSG_MAP_BEGIN()
			MSG_WM_PAINT_EX(OnPaint)
			SOUI_MSG_MAP_END()

			SOUI_ATTRS_BEGIN()
			ATTR_SKIN(L"maskskin", m_pMaskSkin1, TRUE)
			SOUI_ATTRS_END()

		ISkinObj *m_pMaskSkin1;
		SStringW                m_strSkin;
		CAutoRefPtr<IBitmap>    m_bmpCache;
		CAutoRefPtr<IBitmap>    m_bmpMask;
		int                     m_iMaskChannel;

		
	};
}


