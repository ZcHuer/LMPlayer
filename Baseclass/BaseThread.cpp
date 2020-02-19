#pragma once
#include "stdafx.h"
#include "BaseThread.h"

namespace {
    DWORD g_dwMainThreadId = 0;
}

void UpdateMainThreadId()
{
    g_dwMainThreadId = GetCurrentThreadId();
}

bool IsInMainThread()
{
    return GetCurrentThreadId() == g_dwMainThreadId;
}
