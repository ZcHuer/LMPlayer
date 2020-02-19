#pragma once
#include <vector>
#include <objidl.h>
#include <ObjectArray.h>
#include "Base.h"

class CObjectCollectionImpl : public IObjectCollection
{
public:
	CObjectCollectionImpl(void);
	~CObjectCollectionImpl(void);

	static HRESULT CreateObjectCollection(IObjectCollection** lppObj)
	{
		if(NULL == lppObj)
			return E_INVALIDARG;

		IUnknownImpl2<CObjectCollectionImpl, IObjectCollection, IObjectArray>* 
			pObj = new IUnknownImpl2<CObjectCollectionImpl, IObjectCollection, IObjectArray>();
		if(NULL == pObj)
			return E_FAIL;

		HRESULT hr = pObj->QueryInterface(__uuidof(IObjectCollection), (void**)lppObj);
		if(FAILED(hr))
			delete pObj;
		return hr;
	}

public://IObjectArray
	virtual HRESULT STDMETHODCALLTYPE GetCount( 
		/* [out] */ __RPC__out UINT *pcObjects);

	virtual HRESULT STDMETHODCALLTYPE GetAt( 
		/* [in] */ UINT uiIndex,
		/* [in] */ __RPC__in REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out_opt void **ppv);

public://IObjectCollection
	virtual HRESULT STDMETHODCALLTYPE AddObject( 
		/* [in] */ __RPC__in_opt IUnknown *punk);

	virtual HRESULT STDMETHODCALLTYPE AddFromArray( 
		/* [in] */ __RPC__in_opt IObjectArray *poaSource);

	virtual HRESULT STDMETHODCALLTYPE RemoveObjectAt( 
		/* [in] */ UINT uiIndex);

	virtual HRESULT STDMETHODCALLTYPE Clear( void);

protected:
	std::vector<IUnknown*> m_vecOjbects;
};


class CStreamImpl : public IStream
{
public:
	CStreamImpl();
	virtual ~CStreamImpl();

public:


public://ISequentialStream
	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Read( 
		/* [annotation] */ 
		__out_bcount_part(cb, *pcbRead)  void *pv,
		/* [in] */ ULONG cb,
		/* [annotation] */ 
		__out_opt  ULONG *pcbRead);

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Write( 
		/* [annotation] */ 
		__in_bcount(cb)  const void *pv,
		/* [in] */ ULONG cb,
		/* [annotation] */ 
		__out_opt  ULONG *pcbWritten);

public://IStream
	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Seek( 
		/* [in] */ LARGE_INTEGER dlibMove,
		/* [in] */ DWORD dwOrigin,
		/* [annotation] */ 
		__out_opt  ULARGE_INTEGER *plibNewPosition);

	virtual HRESULT STDMETHODCALLTYPE SetSize( 
		/* [in] */ ULARGE_INTEGER libNewSize) = 0;

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE CopyTo( 
		/* [unique][in] */ IStream *pstm,
		/* [in] */ ULARGE_INTEGER cb,
		/* [annotation] */ 
		__out_opt  ULARGE_INTEGER *pcbRead,
		/* [annotation] */ 
		__out_opt  ULARGE_INTEGER *pcbWritten);

	virtual HRESULT STDMETHODCALLTYPE Commit( 
		/* [in] */ DWORD grfCommitFlags) = 0;

	virtual HRESULT STDMETHODCALLTYPE Revert( void);

	virtual HRESULT STDMETHODCALLTYPE LockRegion( 
		/* [in] */ ULARGE_INTEGER libOffset,
		/* [in] */ ULARGE_INTEGER cb,
		/* [in] */ DWORD dwLockType);

	virtual HRESULT STDMETHODCALLTYPE UnlockRegion( 
		/* [in] */ ULARGE_INTEGER libOffset,
		/* [in] */ ULARGE_INTEGER cb,
		/* [in] */ DWORD dwLockType);

	virtual HRESULT STDMETHODCALLTYPE Stat( 
		/* [out] */ __RPC__out STATSTG *pstatstg,
		/* [in] */ DWORD grfStatFlag);

	virtual HRESULT STDMETHODCALLTYPE Clone( 
		/* [out] */ __RPC__deref_out_opt IStream **ppstm);
};
