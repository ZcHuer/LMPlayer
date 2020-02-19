#include "../stdafx.h"
#include "ImageHeader.h"
namespace SOUI
{
	CImageHeader::CImageHeader()
	{
		m_pMaskSkin1 = NULL;
	}

	CImageHeader::~CImageHeader()
	{

	}

	

	HRESULT CImageHeader::OnAttrMask(const SStringW & strValue, BOOL bLoading)
	{
		SStringW strChannel = strValue.Right(2);
		m_iMaskChannel = -1;
		if (strChannel == L".a")
			m_iMaskChannel = 3;
		else if (strChannel == L".r")
			m_iMaskChannel = 0;
		else if (strChannel == L".g")
			m_iMaskChannel = 1;
		else if (strChannel == L".b")
			m_iMaskChannel = 2;

		IBitmap *pImg = NULL;
		if (m_iMaskChannel == -1)
		{//use alpha channel as default
			m_iMaskChannel = 0;
			pImg = LOADIMAGE2(strValue);
		}
		else
		{
			pImg = LOADIMAGE2(strValue.Left(strValue.GetLength() - 2));
		}
		if (!pImg)
		{
			return E_FAIL;
		}
		m_bmpMask = pImg;
		pImg->Release();

		m_bmpCache = NULL;
		GETRENDERFACTORY->CreateBitmap(&m_bmpCache);
		m_bmpCache->Init(m_bmpMask->Width(), m_bmpMask->Height());

		if (!m_strSkin.IsEmpty()&&!m_pImg)
		{
			//ISkinObj * pSkin = GETSKIN(m_strSkin, GetScale());
			//if (pSkin) MakeCacheApha(pSkin);
		}
		return S_OK;
	}

	HRESULT CImageHeader::OnAttrImage(const SStringW & strValue, BOOL bLoading)
	{
		if (m_bmpCache)
		{
			ISkinObj * pSkin = GETSKIN(strValue, GetScale());
			if (pSkin) MakeCacheApha(pSkin);
		}
		else
		{
			m_strSkin = strValue;
		}
		return bLoading ? S_OK : S_FALSE;
	}

	void CImageHeader::OnPaint(IRenderTarget * pRT)
	{
		CRect rcWnd = GetWindowRect();
		if (m_pImg)
		{
			int img_x, img_y;
			int img_w, img_h;
			img_w = m_pImg->Width();
			img_h = m_pImg->Height();
			img_x = img_w<img_h?0:(img_w-img_h)/2;
			img_y = img_w < img_h ? (img_h - img_w) / 2 : 0;
			img_h = img_w < img_h ? img_w : img_h;
			img_w = img_h;
			CRect rcImg(img_x, img_y, img_w, img_h);
			pRT->DrawBitmapEx(rcWnd, m_pImg, &rcImg, MAKELONG(EM_STRETCH, m_fl));
		}
		else
			m_pSkin->Draw(pRT, rcWnd, 0);
		CRect rcClient = GetClientRect();
		if (m_pMaskSkin1)
		{
			m_pMaskSkin1->Draw(pRT, rcClient,0);
			//pRT->DrawBitmap(rcClient, m_bmpMask, m_bmpMask->Width(), m_bmpMask->Height(), EM_STRETCH);
		}
		return;

	}
	void CImageHeader::MakeCacheApha()
	{
		SASSERT(m_bmpMask && m_bmpCache &&m_pImg);
		CRect rcClient = GetClientRect();
		CAutoRefPtr<IRenderTarget> pRTDst;
		GETRENDERFACTORY->CreateRenderTarget(&pRTDst, rcClient.Width(), rcClient.Height());
		CAutoRefPtr<IRenderObj> pOldBmp;
		pRTDst->SelectObject(m_bmpCache, &pOldBmp);
		CRect rc(CPoint(0, 0), m_bmpCache->Size());
		//pSkin->Draw(pRTDst, &rc, 0);
		pRTDst->DrawBitmap(rc,m_pImg, m_pImg->Width(),m_pImg->Height());
		pRTDst->SelectObject(pOldBmp);
		//return;
		//从mask的指定channel中获得alpha通道
		LPBYTE pBitCache = (LPBYTE)m_bmpCache->LockPixelBits();
		LPBYTE pBitMask = (LPBYTE)m_bmpMask->LockPixelBits();
		LPBYTE pDst = pBitCache;
		LPBYTE pSrc = pBitMask + m_iMaskChannel;
		int nPixels = m_bmpCache->Width()*m_bmpCache->Height();
		for (int i = 0; i < nPixels; i++)
		{
			BYTE byAlpha = *pSrc;
			pSrc += 4;
			//源半透明，mask不透明时使用源的半透明属性
			if (pDst[3] == 0xff || (pDst[3] != 0xFF && byAlpha == 0))
			{//源不透明,或者mask全透明
				*pDst++ = ((*pDst) * byAlpha) >> 8;//做premultiply
				*pDst++ = ((*pDst) * byAlpha) >> 8;//做premultiply
				*pDst++ = ((*pDst) * byAlpha) >> 8;//做premultiply
				*pDst++ = byAlpha;
			}
		}
		m_bmpCache->UnlockPixelBits(pBitCache);
		m_bmpMask->UnlockPixelBits(pBitMask);

	}
	void CImageHeader::MakeCacheApha(ISkinObj *pSkin)
	{
		return;
		SASSERT(m_bmpMask && m_bmpCache);
		CAutoRefPtr<IRenderTarget> pRTDst;
		GETRENDERFACTORY->CreateRenderTarget(&pRTDst, 0, 0);
		CAutoRefPtr<IRenderObj> pOldBmp;
		pRTDst->SelectObject(m_bmpCache, &pOldBmp);
		CRect rc(CPoint(0, 0), m_bmpCache->Size());
		pSkin->Draw(pRTDst, &rc, 0);
		pRTDst->SelectObject(pOldBmp);


		//从mask的指定channel中获得alpha通道
		LPBYTE pBitCache = (LPBYTE)m_bmpCache->LockPixelBits();
		LPBYTE pBitMask = (LPBYTE)m_bmpMask->LockPixelBits();
		LPBYTE pDst = pBitCache;
		LPBYTE pSrc = pBitMask + m_iMaskChannel;
		int nPixels = m_bmpCache->Width()*m_bmpCache->Height();
		for (int i = 0; i < nPixels; i++)
		{
			BYTE byAlpha = *pSrc;
			pSrc += 4;
			//源半透明，mask不透明时使用源的半透明属性
			if (pDst[3] == 0xff || (pDst[3] != 0xFF && byAlpha == 0))
			{//源不透明,或者mask全透明
				*pDst++ = ((*pDst) * byAlpha) >> 8;//做premultiply
				*pDst++ = ((*pDst) * byAlpha) >> 8;//做premultiply
				*pDst++ = ((*pDst) * byAlpha) >> 8;//做premultiply
				*pDst++ = byAlpha;
			}
		}
		m_bmpCache->UnlockPixelBits(pBitCache);
		m_bmpMask->UnlockPixelBits(pBitMask);

	}

}


