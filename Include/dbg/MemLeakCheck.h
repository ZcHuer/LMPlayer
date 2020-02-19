//////////////////////////////////////////////////////////////////////////
// 版本：V1.0
// 创建：2010.8.24
// 作者：黄森堂
//------------------------------------------------------------------------
// 功能：开启VC内置内存泄露检测
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


