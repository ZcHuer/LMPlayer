//////////////////////////////////////////////////////////////////////////
// �汾��V1.0
// ������2010.8.24
// ���ߣ���ɭ��
//------------------------------------------------------------------------
// ���ܣ�����VC�����ڴ�й¶���
//------------------------------------------------------------------------
// Copyright (c) www.baofeng.com All Rights Reserved
//////////////////////////////////////////////////////////////////////////
#pragma once

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#endif

#include <stdlib.h>
#ifdef _DEBUG
	#include <crtdbg.h> 
	#define EnableMemLeakCheck		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF )
#else
	#define EnableMemLeakCheck	
#endif


