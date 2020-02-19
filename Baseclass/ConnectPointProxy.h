#pragma once
#include <objidl.h>

#define CP_PROXY_CLASS_BEGIN(cls, iface) template<class T, class I = iface> class cls { public:
#define CP_PROXY_CLASS_END() };

#define CP_PROXY_CLASS_CPENUM_BEGIN(p)	CComPtr<IConnectionPoint> _spCP; \
										T* _pT = static_cast<T*>(this); \
										if(_pT) { \
										HRESULT _hr = _pT->FindConnectionPoint(__uuidof(I), &_spCP); \
										if(SUCCEEDED(_hr)) \
										{	CComPtr<IEnumConnections> _spEC; \
											_hr = _spCP->EnumConnections(&_spEC); \
											if(SUCCEEDED(_hr)) \
											{	ULONG _uFetched = 0; \
												CONNECTDATA _cd; \
												while (S_OK == _spEC->Next(1, &_cd, &_uFetched)) \
												{ CComQIPtr<I> (p) = _cd.pUnk; \
													if(p) { 

#define CP_PROXY_CLASS_CPENUM_END()		/*if(p)*/} /*while()*/} /*if(_hr)*/} /*if(_hr)*/} /*if(_pT)*/}

#define CPENUM_BEGIN(p)		CComPtr<IConnectionPoint> _spCP; \
							HRESULT _hr = FindConnectionPoint(__uuidof(I), &_spCP); \
							if(SUCCEEDED(_hr)) \
							{	CComPtr<IEnumConnections> _spEC; \
								_hr = _spCP->EnumConnections(&_spEC); \
								if(SUCCEEDED(_hr)) \
								{	ULONG _uFetched = 0; \
									CONNECTDATA _cd; \
									while (S_OK == _spEC->Next(1, &_cd, &_uFetched)) \
									{ CComQIPtr<I> (p) = _cd.pUnk; \
										if(p) { 

#define CPENUM_END()		/*if(p)*/} /*while()*/} /*if(_hr)*/} /*if(_hr)*/}


#define CP_ENUM_BEGIN_MEMBER(p, m, I)	CComPtr<IConnectionPoint> _spCP; \
									HRESULT _hr = (m)->FindConnectionPoint(__uuidof(I), &_spCP); \
									if(SUCCEEDED(_hr)) \
									{	CComPtr<IEnumConnections> _spEC; \
										_hr = _spCP->EnumConnections(&_spEC); \
										if(SUCCEEDED(_hr)) \
											{	ULONG _uFetched = 0; \
												CONNECTDATA _cd; \
												while (S_OK == _spEC->Next(1, &_cd, &_uFetched)) \
													{ CComPtr<IUnknown> _spUnk; _spUnk.Attach(_cd.pUnk); CComQIPtr<I> (p) = _spUnk; \
														if(p) { 

#define CP_ENUM_END_MEMBER()		/*if(p)*/} /*while()*/} /*if(_hr)*/} /*if(_hr)*/}