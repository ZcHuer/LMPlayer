#include "stdafx.h"
#include "ObjcectArray.h"

CObjectCollectionImpl::CObjectCollectionImpl(void)
{

}

CObjectCollectionImpl::~CObjectCollectionImpl(void)
{
	Clear();
}

HRESULT STDMETHODCALLTYPE CObjectCollectionImpl::GetCount( 
	/* [out] */ __RPC__out UINT *pcObjects)
{
	if(NULL == pcObjects)
		return E_INVALIDARG;

	size_t nCount = m_vecOjbects.size();
	*pcObjects = (UINT)nCount;
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CObjectCollectionImpl::GetAt( 
	/* [in] */ UINT uiIndex,
	/* [in] */ __RPC__in REFIID riid,
	/* [iid_is][out] */ __RPC__deref_out_opt void **ppv)
{
	size_t nIndex = (size_t)uiIndex;
	if(nIndex >= m_vecOjbects.size())
		return E_INVALIDARG;

	IUnknown* pUnk = m_vecOjbects.at(nIndex);
	if(NULL == pUnk)
		return E_FAIL;

	return pUnk->QueryInterface(riid, ppv);
}

HRESULT STDMETHODCALLTYPE CObjectCollectionImpl::AddObject( 
		/* [in] */ __RPC__in_opt IUnknown *punk)
{
	if(NULL== punk)
		return E_INVALIDARG;

	punk->AddRef();
	m_vecOjbects.push_back(punk);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CObjectCollectionImpl::AddFromArray( 
		/* [in] */ __RPC__in_opt IObjectArray *poaSource)
{
	if(NULL == poaSource)
		return E_INVALIDARG;

	UINT uCount = 0;
	HRESULT hr = poaSource->GetCount(&uCount);
	if(FAILED(hr) || uCount == 0)
		return hr;

	for(UINT i = 0; i < uCount; ++i)
	{
		IUnknown* pUnk = NULL;
		hr = poaSource->GetAt(i, __uuidof(IUnknown), (LPVOID*)&pUnk);
		if(FAILED(hr))
			return hr;

		if(pUnk)
		{
			pUnk->AddRef();
			m_vecOjbects.push_back(pUnk);
		}
		else
		{
			ATLASSERT(FALSE);
		}
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE CObjectCollectionImpl::RemoveObjectAt( 
		/* [in] */ UINT uiIndex)
{
	UINT i = 0; 
	std::vector<IUnknown*>::iterator it = m_vecOjbects.begin();
	for( ; i < uiIndex && it != m_vecOjbects.end(); ++it,++i)
	{
		//DoNothing
	}
	if(it != m_vecOjbects.end())
	{
		if(*it)
			(*it)->Release();
		m_vecOjbects.erase(it);
		return S_OK;
	}
	return E_FAIL;
}

HRESULT STDMETHODCALLTYPE CObjectCollectionImpl::Clear( void)
{
	std::vector<IUnknown*>::iterator it = m_vecOjbects.begin();
	for( ; it != m_vecOjbects.end(); ++it)
	{
		if(*it)
		{
			(*it)->Release();
		}
	}
	m_vecOjbects.clear();
	return S_OK;
}