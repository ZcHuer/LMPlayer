#pragma once

#define _COMMA_PREFIX(x)		, x			

#define _APPEND_0(x)						  
#define _APPEND_1(x)						   x##1
#define _APPEND_2(x)			_APPEND_1(x) , x##2
#define _APPEND_3(x)			_APPEND_2(x) , x##3
#define _APPEND_4(x)			_APPEND_3(x) , x##4
#define _APPEND_5(x)			_APPEND_4(x) , x##5
#define _APPEND_6(x)			_APPEND_5(x) , x##6
#define _APPEND_7(x)			_APPEND_6(x) , x##7
#define _APPEND_8(x)			_APPEND_7(x) , x##8

#define _APPEND_XY_0(x,y)							
#define _APPEND_XY_1(x,y)							x##1 y##1
#define _APPEND_XY_2(x,y)		_APPEND_XY_1(x,y) , x##2 y##2
#define _APPEND_XY_3(x,y)		_APPEND_XY_2(x,y) , x##3 y##3
#define _APPEND_XY_4(x,y)		_APPEND_XY_3(x,y) , x##4 y##4
#define _APPEND_XY_5(x,y)		_APPEND_XY_4(x,y) , x##5 y##5
#define _APPEND_XY_6(x,y)		_APPEND_XY_5(x,y) , x##6 y##6
#define _APPEND_XY_7(x,y)		_APPEND_XY_6(x,y) , x##7 y##7
#define _APPEND_XY_8(x,y)		_APPEND_XY_7(x,y) , x##8 y##8


#define _APPEND_Y_0(x,y)			
#define _APPEND_Y_1(x,y)						   x y##1
#define _APPEND_Y_2(x,y)		_APPEND_Y_1(x,y) , x y##2
#define _APPEND_Y_3(x,y)		_APPEND_Y_2(x,y) , x y##3
#define _APPEND_Y_4(x,y)		_APPEND_Y_3(x,y) , x y##4
#define _APPEND_Y_5(x,y)		_APPEND_Y_4(x,y) , x y##5
#define _APPEND_Y_6(x,y)		_APPEND_Y_5(x,y) , x y##6
#define _APPEND_Y_7(x,y)		_APPEND_Y_6(x,y) , x y##7
#define _APPEND_Y_8(x,y)		_APPEND_Y_7(x,y) , x y##8

//=================================================================================================

#define MFUNC_THISCALL(n)		CTHISCALL_MFUNC_##n
#define MFUNC_STDCALL(n)		CSTDCALL_MFUNC_##n
#define FUNC_DEFCALL(n)			CDEFCALL_FUNC_##n
#define FUNC_STDCALL(n)			CSTDCALL_FUNC_##n
#define FBINDT(f)				CFBindT_##f

#ifndef _FC_CC_DEF
#define _FC_CC_DEF				__cdecl
#endif //#ifndef _FC_CC_DEF
#define _FC_CC_THIS				__thiscall
#define _FC_CC_STD				__stdcall

/*																eg. R T::F(A)
_FC_CN			FCall Class Name								MFUNC_THISCALL(1)
_FC_CC			FCall Function Calling Convention				_FC_CC_THIS
_FC_TA			FCall Class Template Arguments					_COMMA_PREFIX(_APPEND_Y_1(class, A))			, class A1, class A2
_FC_AL			FCall Function Arguments List					_APPEND_XY_1(A,a)								A1 a1, A2 a2
_FC_AVL			FCall Function Call Arguments List				_APPEND_1(a)									a1, a2
*/
#define FCALL_CLASS_DECLARE_MEMBER(_FC_CN, _FC_CC, _FC_TA, _FC_AL, _FC_AVL) \
template<class R, class T _FC_TA> class _FC_CN \
{ \
public: \
	typedef R (_FC_CC T::*F) (_FC_AL); \
	explicit _FC_CN(F f) : f_(f) {} \
	R operator()(T * p, _FC_AL) const \
	{ \
		return (p->*f_)(_FC_AVL); \
	} \
private: \
	F f_; \
};

#define FCALL_CLASS_DECLARE(_FC_CN, _FC_CC, _FC_TA, _FC_AL, _FC_AVL) \
template<class R _FC_TA> class _FC_CN \
{ \
public: \
	typedef R (_FC_CC *F) (_FC_AL); \
	explicit _FC_CN(F f) : f_(f) {} \
	R operator()(_FC_AL) const \
	{ \
	return (*f_)(_FC_AVL); \
	} \
private: \
	F f_; \
};

/*																eg. R T::F(A)
_FC_CN			FCall Class Name								MFUNC_THISCALL(1)
_FC_CC			FCall Function Calling Convention				_FC_CC_THIS
_FC_TA			FCall Class Template Arguments					_COMMA_PREFIX(_APPEND_Y_1(class, A))			, class A1, class A2
_FC_AL			FCall Function Arguments List					_APPEND_XY_1(A,a)								A1 a1, A2 a2
_FC_AVL			FCall Function Call Arguments List				_APPEND_1(a)									a1, a2

_FB_CN			FBind Class Name								MFUNC_THISCALL(1)						_FC_CN
_FB_CC			FBind Function Calling Covention				_FC_CC_THIS								_FC_CC
_FB_TA			FBind Class Template Arguments					_COMMA_PREFIX(_APPEND_Y_1(class, A))	_FC_TA
_FB_AL			FCall Function Arguments List					_APPEND_XY_1(A,a)						_FC_AL
_FB_AVL			FCall Function Call Arguments List				_APPEND_1(a)							_FC_AVL
_FB_ATL			FCall Function Arguments Type List				_APPEND_1(A)									A1, A2
_FB_ATCN		FBind Arguments Template Class Name				TA1
_FB_AVL_TAn		FBind Operator() Arguments						_APPEND_1(_a)									_a1, _a2
*/
#define FBIND_CLASS_DECLARE_MEMBER(_FB_CN, _FB_CC, _FB_TA, _FB_AL, _FB_AVL, _FB_ATL, _FB_ATCN, _FB_AVL_TAn) \
template<class R, class T _FB_TA> \
class FBINDT(_FB_CN) : public FCall , public _FB_ATCN < _FB_ATL > \
{ \
public: \
	typedef R (_FB_CC T::*F) (_FB_ATL); \
	FBINDT(_FB_CN)(F f, T* pT, _FB_AL) : _f(f), _pT(pT), _FB_ATCN < _FB_ATL >( _FB_AVL ) {} \
	R operator()() { return _f(_pT, _FB_AVL_TAn); } \
	template<class R, class T _FB_TA> \
	FBINDT(_FB_CN)(FBINDT(_FB_CN)<R, T, _FB_ATL>& ref) : _f(ref._f), _pT(ref._f), _FB_ATCN < _FB_ATL >(ref) {} \
	FBINDT(_FB_CN)<R, T, _FB_ATL>& operator=(FBINDT(_FB_CN)<R, T, _FB_ATL>& ref) \
	{ \
		if(this != &ref) \
		{ \
			_f = ref._f; \
			_pT = ref._pT; \
			_FB_ATCN < _FB_ATL >::operator=(ref); \
		} \
		return *this; \
	} \
public: \
	virtual bool Call() { (*this)(); return true; } \
private: \
	_FB_CN<R, T, _FB_ATL> _f; \
	T* _pT; \
};

#define FBIND_CLASS_DECLARE(_FB_CN, _FB_CC, _FB_TA, _FB_AL, _FB_AVL, _FB_ATL, _FB_ATCN, _FB_AVL_TAn) \
template<class R _FB_TA> \
class FBINDT(_FB_CN) : public FCall , public _FB_ATCN < _FB_ATL > \
{ \
public: \
	typedef R (_FB_CC *F) (_FB_ATL); \
	FBINDT(_FB_CN)(F f, _FB_AL) : _f(f), _FB_ATCN < _FB_ATL >( _FB_AVL ) {} \
	R operator()() { return _f(_FB_AVL_TAn); } \
	template<class R _FB_TA> \
	FBINDT(_FB_CN)(FBINDT(_FB_CN)<R, _FB_ATL>& ref) : _f(ref._f), _FB_ATCN < _FB_ATL >(ref) {} \
	FBINDT(_FB_CN)<R, _FB_ATL>& operator=(FBINDT(_FB_CN)<R, _FB_ATL>& ref) \
	{ \
	if(this != &ref) \
		{ \
		_f = ref._f; \
		_FB_ATCN < _FB_ATL >::operator=(ref); \
		} \
		return *this; \
	} \
public: \
	virtual bool Call() { (*this)(); return true; } \
private: \
	_FB_CN<R, _FB_ATL> _f; \
};

/*																eg. R T::F(A)
_FC_CN			FCall Class Name								MFUNC_THISCALL(1)
_FC_CC			FCall Function Calling Convention				_FC_CC_THIS
_FC_TA			FCall Class Template Arguments					_COMMA_PREFIX(_APPEND_Y_1(class, A))			, class A1, class A2
_FC_AL			FCall Function Arguments List					_APPEND_XY_1(A,a)								A1 a1, A2 a2
_FC_AVL			FCall Function Call Arguments List				_APPEND_1(a)									a1, a2

_FB_CN			FBind Class Name								MFUNC_THISCALL(1)						_FC_CN
_FB_CC			FBind Function Calling Covention				_FC_CC_THIS								_FC_CC
_FB_TA			FBind Class Template Arguments					_COMMA_PREFIX(_APPEND_Y_1(class, A))	_FC_TA
_FB_AL			FCall Function Arguments List					_APPEND_XY_1(A,a)						_FC_AL
_FB_AVL			FCall Function Call Arguments List				_APPEND_1(a)							_FC_AVL
_FB_ATL			FCall Function Arguments Type List				_APPEND_1(A)									A1, A2
_FB_ATCN		FBind Arguments Template Class Name				TA1
_FB_AVL_TAn		FBind Operator() Arguments						_APPEND_1(_a)									_a1, _a2

_FBF_CN			FBind Class Name								MFUNC_THISCALL(1)					_FC_CN/_FB_CN
_FBF_CC			FBind Function Calling Covention				_FC_CC_THIS							_FC_CC/_FB_CC
_FBF_TA			FBind Class Template Arguments					_COMMA_PREFIX(_APPEND_Y_1(A))		_FC_TA/_FB_TA
_FBF_AL			FBind Function Arguments List					_APPEND_XY_1(A,a)					_FC_AL/_FB_AL
_FBF_AVL		FBind Function Call Arguments List				_APPEND_1(a)						_FC_AVL/_FB_AVL
_FBF_ATL		FBind Function Arguments Type List				_APPEND_1(A)						_FB_ATL		
*/
#define FBIND_FUNC_DECLARE_MEMBER(_FBF_CN, _FBF_CC, _FBF_TA, _FBF_AL, _FBF_AVL, _FBF_ATL) \
template<class R, class T _FBF_TA> \
FCall* bind(R (_FBF_CC T::* f)(_FBF_ATL), T* pT, _FBF_AL) \
{ \
	return dynamic_cast<FCall*>(new FBINDT(_FBF_CN)<R, T, _FBF_ATL>(f, pT, _FBF_AVL)); \
}

#define FBIND_FUNC_DECLARE(_FBF_CN, _FBF_CC, _FBF_TA, _FBF_AL, _FBF_AVL, _FBF_ATL) \
template<class R _FBF_TA> \
FCall* bind(R (_FBF_CC * f)(_FBF_ATL), _FBF_AL) \
{ \
	return dynamic_cast<FCall*>(new FBINDT(_FBF_CN)<R, _FBF_ATL>(f, _FBF_AVL)); \
}

/*																eg. R T::F(A)
_FC_CN			FCall Class Name								MFUNC_THISCALL(1)
_FC_CC			FCall Function Calling Convention				_FC_CC_THIS
_FC_TA			FCall Class Template Arguments					_COMMA_PREFIX(_APPEND_Y_1(class, A))			, class A1, class A2
_FC_AL			FCall Function Arguments List					_APPEND_XY_1(A,a)								A1 a1, A2 a2
_FC_AVL			FCall Function Call Arguments List				_APPEND_1(a)									a1, a2

_FB_CN			FBind Class Name								MFUNC_THISCALL(1)						_FC_CN
_FB_CC			FBind Function Calling Covention				_FC_CC_THIS								_FC_CC
_FB_TA			FBind Class Template Arguments					_COMMA_PREFIX(_APPEND_Y_1(class, A))	_FC_TA
_FB_AL			FCall Function Arguments List					_APPEND_XY_1(A,a)						_FC_AL
_FB_AVL			FCall Function Call Arguments List				_APPEND_1(a)							_FC_AVL
_FB_ATL			FCall Function Arguments Type List				_APPEND_1(A)									A1, A2
_FB_ATCN		FBind Arguments Template Class Name				TA1
_FB_AVL_TAn		FBind Operator() Arguments						_APPEND_1(_a)									_a1, _a2

_FBF_CN			FBind Class Name								MFUNC_THISCALL(1)					_FC_CN/_FB_CN
_FBF_CC			FBind Function Calling Covention				_FC_CC_THIS							_FC_CC/_FB_CC
_FBF_TA			FBind Class Template Arguments					_COMMA_PREFIX(_APPEND_Y_1(A))		_FC_TA/_FB_TA
_FBF_AL			FBind Function Arguments List					_APPEND_XY_1(A,a)					_FC_AL/_FB_AL
_FBF_AVL		FBind Function Call Arguments List				_APPEND_1(a)						_FC_AVL/_FB_AVL
_FBF_ATL		FBind Function Arguments Type List				_APPEND_1(A)						_FB_ATL		
*/
#define FCALL_DECLARE_MEMBER(_FB_CN, _FB_CC, _FB_TA, _FB_AL, _FB_AVL, _FB_ATL, _FB_ATCN, _FB_AVL_TAn) \
	FCALL_CLASS_DECLARE_MEMBER(_FB_CN, _FB_CC, _FB_TA, _FB_AL, _FB_AVL) \
	FBIND_CLASS_DECLARE_MEMBER(_FB_CN, _FB_CC, _FB_TA, _FB_AL, _FB_AVL, _FB_ATL, _FB_ATCN, _FB_AVL_TAn) \
	FBIND_FUNC_DECLARE_MEMBER(_FB_CN, _FB_CC, _FB_TA, _FB_AL, _FB_AVL, _FB_ATL)

#define FCALL_DECLARE(_FB_CN, _FB_CC, _FB_TA, _FB_AL, _FB_AVL, _FB_ATL, _FB_ATCN, _FB_AVL_TAn) \
	FCALL_CLASS_DECLARE(_FB_CN, _FB_CC, _FB_TA, _FB_AL, _FB_AVL) \
	FBIND_CLASS_DECLARE(_FB_CN, _FB_CC, _FB_TA, _FB_AL, _FB_AVL, _FB_ATL, _FB_ATCN, _FB_AVL_TAn) \
	FBIND_FUNC_DECLARE(_FB_CN, _FB_CC, _FB_TA, _FB_AL, _FB_AVL, _FB_ATL)

namespace FBind2
{
//=================================================================================================
template<class A0>
class TA0
{
public:
	TA0(){}
	TA0(TA0& ref)
	{

	}
	TA0& operator=(TA0& ref)
	{
		return *this;
	}
};

template<class A1>
class TA1
{
public:
	TA1(){}
	TA1(A1 a1) : _a1(a1){}
	TA1(TA1& ref) : _a1(ref._a1)
	{

	}
	TA1& operator=(TA1& ref)
	{
		if(this != &ref)
		{
			_a1 = ref._a1;
		}
		return *this;
	}
	A1 _a1;
};

template<class A1, class A2>
class TA2 : public TA1<A1>
{
public:
	TA2(){}
	TA2(A1 a1, A2 a2) : TA1<A1>(a1), _a2(a2){}
	TA2(TA2& ref) : _a2(ref._a2), TA1<A1>(ref){}
	TA2& operator=(TA2& ref)
	{
		if(this != &ref)
		{
			_a2 = ref._a2;
			TA1<A1>::operator =(ref);
		}
		return *this;
	}
	A2 _a2;
};

template<class A1, class A2, class A3>
class TA3 : public TA2<A1, A2>
{
public:
	TA3(){}
	TA3(A1 a1, A2 a2, A3 a3) : TA2<A1, A2>(a1, a2), _a3(a3){}
	TA3(TA3& ref) : _a3(ref._a3), TA2<A1, A2>(ref){}
	TA3& operator=(TA3& ref)
	{
		if(this != &ref)
		{
			_a3 = ref._a3;
			TA2<A1, A2>::operator =(ref);
		}
		return *this;
	}
	A3 _a3;
};

template<class A1, class A2, class A3, class A4>
class TA4 : public TA3<A1,A2,A3>
{
public:
	TA4(){}
	TA4(A1 a1, A2 a2, A3 a3, A4 a4) : TA3<A1,A2,A3>(a1,a2,a3),_a4(a4){}
	TA4(TA4& ref) : _a4(ref._a4), TA3<A1,A2,A3>(ref)
	{
	}
	TA4& operator=(TA4& ref)
	{
		if(this != &ref)
		{
			_a4 = ref._a4;
			TA3<A1,A2,A3>::operator=(ref);
		}
		return *this;
	}
	A4 _a4;
};

template<class A1, class A2, class A3, class A4, class A5>
class TA5 : public TA4<A1,A2,A3,A4>
{
public:
	TA5(){}
	TA5(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) : TA4<A1,A2,A3,A4>(a1,a2,a3,a4),_a5(a5){}
	TA5(TA5& ref) : _a5(ref._a5), TA3<A1,A2,A3,A4>(ref)
	{
	}
	TA5& operator=(TA5& ref)
	{
		if(this != &ref)
		{
			_a5 = ref._a5;
			TA4<A1,A2,A3,A4>::operator=(ref);
		}
		return *this;
	}
	A5 _a5;
};

template<class A1, class A2, class A3, class A4, class A5, class A6>
class TA6 : public TA5<A1,A2,A3,A4,A5>
{
public:
	TA6(){}
	TA6(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) : TA5<A1,A2,A3,A4,A5>(a1,a2,a3,a4,a5),_a6(a6){}
	TA6(TA6& ref) : _a6(ref._a6), TA5<A1,A2,A3,A4,A5>(ref)
	{
	}
	TA6& operator=(TA6& ref)
	{
		if(this != &ref)
		{
			_a6 = ref._a6;
			TA5<A1,A2,A3,A4,A5>::operator=(ref);
		}
		return *this;
	}
	A6 _a6;
};

template<class A1, class A2, class A3, class A4, class A5, class A6, class A7>
class TA7 : public TA6<A1,A2,A3,A4,A5,A6>
{
public:
	TA7(){}
	TA7(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) : TA6<A1,A2,A3,A4,A5,A6>(a1,a2,a3,a4,a5,a6),_a7(a7){}
	TA7(TA7& ref) : _a7(ref._a7), TA6<A1,A2,A3,A4,A5,A6>(ref)
	{
	}
	TA7& operator=(TA7& ref)
	{
		if(this != &ref)
		{
			_a7 = ref._a7;
			TA6<A1,A2,A3,A4,A5,A6>::operator=(ref);
		}
		return *this;
	}
	A7 _a7;
};

template<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
class TA8 : public TA7<A1,A2,A3,A4,A5,A6,A7>
{
public:
	TA8(){}
	TA8(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) : TA7<A1,A2,A3,A4,A5,A6,A7>(a1,a2,a3,a4,a5,a6,a7),_a8(a8){}
	TA8(TA8& ref) : _a8(ref._a8), TA7<A1,A2,A3,A4,A5,A6,A7>(ref)
	{
	}
	TA8& operator=(TA8& ref)
	{
		if(this != &ref)
		{
			_a8 = ref._a8;
			TA7<A1,A2,A3,A4,A5,A6,A7>::operator=(ref);
		}
		return *this;
	}
	A8 _a8;
};


//=================================================================================================
class FCall
{
public:
	FCall(){}
	virtual ~FCall(){}
	virtual bool Call(){return false;}
	virtual FCall* Clone(){return NULL;}
};

//=================================================================================================
// Member Function Call , __thiscall
//0 arg
template<class R, class T> class CTHISCALL_MFUNC_0 { public: typedef R (__thiscall T::*F) (); explicit CTHISCALL_MFUNC_0(F f) : f_(f) {} R operator()(T * p) const { return (p->*f_)(); } private: F f_; }; template<class R, class T> class CFBindT_CTHISCALL_MFUNC_0 : public FCall{ public: typedef R (__thiscall T::*F) (); CFBindT_CTHISCALL_MFUNC_0(F f, T* pT) : _f(f), _pT(pT){} R operator()() { return _f(_pT); } template<class R, class T > CFBindT_CTHISCALL_MFUNC_0(CFBindT_CTHISCALL_MFUNC_0<R, T>& ref) : _f(ref._f), _pT(ref._f){} CFBindT_CTHISCALL_MFUNC_0<R, T>& operator=(CFBindT_CTHISCALL_MFUNC_0<R, T>& ref) { if(this != &ref) { _f = ref._f; _pT = ref._pT; } return *this; } public: virtual bool Call() { (*this)(); return true; } private: CTHISCALL_MFUNC_0<R, T> _f; T* _pT; }; template<class R, class T> FCall* bind(R (__thiscall T::* f)(), T* pT) { return dynamic_cast<FCall*>(new CFBindT_CTHISCALL_MFUNC_0<R, T>(f, pT)); }
//1 arg
FCALL_DECLARE_MEMBER(MFUNC_THISCALL(1), _FC_CC_THIS, _COMMA_PREFIX(_APPEND_Y_1(class, A)), _APPEND_XY_1(A,a), _APPEND_1(a), _APPEND_1(A), TA1, _APPEND_1(_a))
//2 args
FCALL_DECLARE_MEMBER(MFUNC_THISCALL(2), _FC_CC_THIS, _COMMA_PREFIX(_APPEND_Y_2(class, A)), _APPEND_XY_2(A,a), _APPEND_2(a), _APPEND_2(A), TA2, _APPEND_2(_a))
//3 args
FCALL_DECLARE_MEMBER(MFUNC_THISCALL(3), _FC_CC_THIS, _COMMA_PREFIX(_APPEND_Y_3(class, A)), _APPEND_XY_3(A,a), _APPEND_3(a), _APPEND_3(A), TA3, _APPEND_3(_a))
//4 args
FCALL_DECLARE_MEMBER(MFUNC_THISCALL(4), _FC_CC_THIS, _COMMA_PREFIX(_APPEND_Y_4(class, A)), _APPEND_XY_4(A,a), _APPEND_4(a), _APPEND_4(A), TA4, _APPEND_4(_a))
//5 args
FCALL_DECLARE_MEMBER(MFUNC_THISCALL(5), _FC_CC_THIS, _COMMA_PREFIX(_APPEND_Y_5(class, A)), _APPEND_XY_5(A,a), _APPEND_5(a), _APPEND_5(A), TA5, _APPEND_5(_a))
//6 args
FCALL_DECLARE_MEMBER(MFUNC_THISCALL(6), _FC_CC_THIS, _COMMA_PREFIX(_APPEND_Y_6(class, A)), _APPEND_XY_6(A,a), _APPEND_6(a), _APPEND_6(A), TA6, _APPEND_6(_a))
//7 args
FCALL_DECLARE_MEMBER(MFUNC_THISCALL(7), _FC_CC_THIS, _COMMA_PREFIX(_APPEND_Y_7(class, A)), _APPEND_XY_7(A,a), _APPEND_7(a), _APPEND_7(A), TA7, _APPEND_7(_a))
//8 args
FCALL_DECLARE_MEMBER(MFUNC_THISCALL(8), _FC_CC_THIS, _COMMA_PREFIX(_APPEND_Y_8(class, A)), _APPEND_XY_8(A,a), _APPEND_8(a), _APPEND_8(A), TA8, _APPEND_8(_a))

//Member Function Call, __stdcall 
//0 arg
template<class R, class T> class CSTDCALL_MFUNC_0 { public: typedef R (__stdcall T::*F) (); explicit CSTDCALL_MFUNC_0(F f) : f_(f) {} R operator()(T * p) const { return (p->*f_)(); } private: F f_; }; template<class R, class T > class CFBindT_CSTDCALL_MFUNC_0 : public FCall { public: typedef R (__stdcall T::*F) (); CFBindT_CSTDCALL_MFUNC_0(F f, T* pT) : _f(f), _pT(pT){} R operator()() { return _f(_pT); } template<class R, class T> CFBindT_CSTDCALL_MFUNC_0(CFBindT_CSTDCALL_MFUNC_0<R, T>& ref) : _f(ref._f), _pT(ref._f) {} CFBindT_CSTDCALL_MFUNC_0<R, T>& operator=(CFBindT_CSTDCALL_MFUNC_0<R, T>& ref) { if(this != &ref) { _f = ref._f; _pT = ref._pT; } return *this; } public: virtual bool Call() { (*this)(); return true; } private: CSTDCALL_MFUNC_0<R, T> _f; T* _pT; }; template<class R, class T> FCall* bind(R (__stdcall T::* f)(), T* pT) { return dynamic_cast<FCall*>(new CFBindT_CSTDCALL_MFUNC_0<R, T>(f, pT)); }
//1 arg
FCALL_DECLARE_MEMBER(MFUNC_STDCALL(1), _FC_CC_STD, _COMMA_PREFIX(_APPEND_Y_1(class, A)), _APPEND_XY_1(A,a), _APPEND_1(a), _APPEND_1(A), TA1, _APPEND_1(_a))
//2 args
FCALL_DECLARE_MEMBER(MFUNC_STDCALL(2), _FC_CC_STD, _COMMA_PREFIX(_APPEND_Y_2(class, A)), _APPEND_XY_2(A,a), _APPEND_2(a), _APPEND_2(A), TA2, _APPEND_2(_a))
//3 args
FCALL_DECLARE_MEMBER(MFUNC_STDCALL(3), _FC_CC_STD, _COMMA_PREFIX(_APPEND_Y_3(class, A)), _APPEND_XY_3(A,a), _APPEND_3(a), _APPEND_3(A), TA3, _APPEND_3(_a))
//4 args
FCALL_DECLARE_MEMBER(MFUNC_STDCALL(4), _FC_CC_STD, _COMMA_PREFIX(_APPEND_Y_4(class, A)), _APPEND_XY_4(A,a), _APPEND_4(a), _APPEND_4(A), TA4, _APPEND_4(_a))
//5 args
FCALL_DECLARE_MEMBER(MFUNC_STDCALL(5), _FC_CC_STD, _COMMA_PREFIX(_APPEND_Y_5(class, A)), _APPEND_XY_5(A,a), _APPEND_5(a), _APPEND_5(A), TA5, _APPEND_5(_a))
//6 args
FCALL_DECLARE_MEMBER(MFUNC_STDCALL(6), _FC_CC_STD, _COMMA_PREFIX(_APPEND_Y_6(class, A)), _APPEND_XY_6(A,a), _APPEND_6(a), _APPEND_6(A), TA6, _APPEND_6(_a))
//7 args
FCALL_DECLARE_MEMBER(MFUNC_STDCALL(7), _FC_CC_STD, _COMMA_PREFIX(_APPEND_Y_7(class, A)), _APPEND_XY_7(A,a), _APPEND_7(a), _APPEND_7(A), TA7, _APPEND_7(_a))
//8 args
FCALL_DECLARE_MEMBER(MFUNC_STDCALL(8), _FC_CC_STD, _COMMA_PREFIX(_APPEND_Y_8(class, A)), _APPEND_XY_8(A,a), _APPEND_8(a), _APPEND_8(A), TA8, _APPEND_8(_a))


//=================================================================================================
// Global Function Call , NO Calling Covention
//0 arg
template<class R> class CDEFCALL_FUNC_0 { public: typedef R (__cdecl *F) (); explicit CDEFCALL_FUNC_0(F f) : f_(f) {} R operator()() const { return (*f_)(); } private: F f_; }; template<class R> class CFBindT_CDEFCALL_FUNC_0 : public FCall{ public: typedef R (__cdecl *F) (); CFBindT_CDEFCALL_FUNC_0(F f) : _f(f){} R operator()() { return _f(); } template<class R> CFBindT_CDEFCALL_FUNC_0(CFBindT_CDEFCALL_FUNC_0<R>& ref) : _f(ref._f){} CFBindT_CDEFCALL_FUNC_0<R>& operator=(CFBindT_CDEFCALL_FUNC_0<R>& ref) { if(this != &ref) { _f = ref._f; } return *this; } public: virtual bool Call() { (*this)(); return true; } private: CDEFCALL_FUNC_0<R> _f; }; template<class R> FCall* bind(R (__cdecl * f)()) { return dynamic_cast<FCall*>(new CFBindT_CDEFCALL_FUNC_0<R>(f)); }
//1 arg
FCALL_DECLARE(FUNC_DEFCALL(1), _FC_CC_DEF, _COMMA_PREFIX(_APPEND_Y_1(class, A)), _APPEND_XY_1(A,a), _APPEND_1(a), _APPEND_1(A), TA1, _APPEND_1(_a))
//2 args
FCALL_DECLARE(FUNC_DEFCALL(2), _FC_CC_DEF, _COMMA_PREFIX(_APPEND_Y_2(class, A)), _APPEND_XY_2(A,a), _APPEND_2(a), _APPEND_2(A), TA2, _APPEND_2(_a))
//3 args
FCALL_DECLARE(FUNC_DEFCALL(3), _FC_CC_DEF, _COMMA_PREFIX(_APPEND_Y_3(class, A)), _APPEND_XY_3(A,a), _APPEND_3(a), _APPEND_3(A), TA3, _APPEND_3(_a))
//4 args
FCALL_DECLARE(FUNC_DEFCALL(4), _FC_CC_DEF, _COMMA_PREFIX(_APPEND_Y_4(class, A)), _APPEND_XY_4(A,a), _APPEND_4(a), _APPEND_4(A), TA4, _APPEND_4(_a))
//5 args
FCALL_DECLARE(FUNC_DEFCALL(5), _FC_CC_DEF, _COMMA_PREFIX(_APPEND_Y_5(class, A)), _APPEND_XY_5(A,a), _APPEND_5(a), _APPEND_5(A), TA5, _APPEND_5(_a))
//6 args
FCALL_DECLARE(FUNC_DEFCALL(6), _FC_CC_DEF, _COMMA_PREFIX(_APPEND_Y_6(class, A)), _APPEND_XY_6(A,a), _APPEND_6(a), _APPEND_6(A), TA6, _APPEND_6(_a))
//7 args
FCALL_DECLARE(FUNC_DEFCALL(7), _FC_CC_DEF, _COMMA_PREFIX(_APPEND_Y_7(class, A)), _APPEND_XY_7(A,a), _APPEND_7(a), _APPEND_7(A), TA7, _APPEND_7(_a))
//8 args
FCALL_DECLARE(FUNC_DEFCALL(8), _FC_CC_DEF, _COMMA_PREFIX(_APPEND_Y_8(class, A)), _APPEND_XY_8(A,a), _APPEND_8(a), _APPEND_8(A), TA8, _APPEND_8(_a))

//Global Function Call, __stdcall 
//0 arg
template<class R> class CSTDCALL_FUNC_0 { public: typedef R (__stdcall *F) (); explicit CSTDCALL_FUNC_0(F f) : f_(f) {} R operator()() const { return (*f_)(); } private: F f_; }; template<class R> class CFBindT_CSTDCALL_FUNC_0 : public FCall { public: typedef R (__stdcall *F) (); CFBindT_CSTDCALL_FUNC_0(F f) : _f(f) {} R operator()() { return _f(); } template<class R> CFBindT_CSTDCALL_FUNC_0(CFBindT_CSTDCALL_FUNC_0<R>& ref) : _f(ref._f){} CFBindT_CSTDCALL_FUNC_0<R>& operator=(CFBindT_CSTDCALL_FUNC_0<R>& ref) { if(this != &ref) { _f = ref._f; } return *this; } public: virtual bool Call() { (*this)(); return true; } private: CSTDCALL_FUNC_0<R> _f; }; template<class R> FCall* bind(R (__stdcall * f)()) { return dynamic_cast<FCall*>(new CFBindT_CSTDCALL_FUNC_0<R>(f)); }
//1 arg
FCALL_DECLARE(FUNC_STDCALL(1), _FC_CC_STD, _COMMA_PREFIX(_APPEND_Y_1(class, A)), _APPEND_XY_1(A,a), _APPEND_1(a), _APPEND_1(A), TA1, _APPEND_1(_a))
//2 args
FCALL_DECLARE(FUNC_STDCALL(2), _FC_CC_STD, _COMMA_PREFIX(_APPEND_Y_2(class, A)), _APPEND_XY_2(A,a), _APPEND_2(a), _APPEND_2(A), TA2, _APPEND_2(_a))
//3 args
FCALL_DECLARE(FUNC_STDCALL(3), _FC_CC_STD, _COMMA_PREFIX(_APPEND_Y_3(class, A)), _APPEND_XY_3(A,a), _APPEND_3(a), _APPEND_3(A), TA3, _APPEND_3(_a))
//4 args
FCALL_DECLARE(FUNC_STDCALL(4), _FC_CC_STD, _COMMA_PREFIX(_APPEND_Y_4(class, A)), _APPEND_XY_4(A,a), _APPEND_4(a), _APPEND_4(A), TA4, _APPEND_4(_a))
//5 args
FCALL_DECLARE(FUNC_STDCALL(5), _FC_CC_STD, _COMMA_PREFIX(_APPEND_Y_5(class, A)), _APPEND_XY_5(A,a), _APPEND_5(a), _APPEND_5(A), TA5, _APPEND_5(_a))
//6 args
FCALL_DECLARE(FUNC_STDCALL(6), _FC_CC_STD, _COMMA_PREFIX(_APPEND_Y_6(class, A)), _APPEND_XY_6(A,a), _APPEND_6(a), _APPEND_6(A), TA6, _APPEND_6(_a))
//7 args
FCALL_DECLARE(FUNC_STDCALL(7), _FC_CC_STD, _COMMA_PREFIX(_APPEND_Y_7(class, A)), _APPEND_XY_7(A,a), _APPEND_7(a), _APPEND_7(A), TA7, _APPEND_7(_a))
//8 args
FCALL_DECLARE(FUNC_STDCALL(8), _FC_CC_STD, _COMMA_PREFIX(_APPEND_Y_8(class, A)), _APPEND_XY_8(A,a), _APPEND_8(a), _APPEND_8(A), TA8, _APPEND_8(_a))

}//namespace FBind2
