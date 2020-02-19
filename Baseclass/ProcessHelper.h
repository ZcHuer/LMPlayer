#ifndef __PROCESS_HELPER_H__
#define __PROCESS_HELPER_H__
#pragma once

#include <string>
#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>


#include <wtypes.h>

namespace process
{
    bool ExecuteExe(LPCTSTR lpAppPath, LPCTSTR lpcsCmd, LPCTSTR lpOperation = L"open", bool bShow = true, bool bWaitTillComplete = false);
    bool IsRunning(LPCTSTR lpProcName);
    bool Terminate(LPCTSTR lpAppPath);

    bool IsAppExist(LPCTSTR lpAppGUID);
    HANDLE CreateAppMutex(LPCTSTR lpAppGUID);
}
#endif
