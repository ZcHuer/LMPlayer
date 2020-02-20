// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "cefclient/browser/client_app_browser.h"

#include "include/base/cef_logging.h"
#include "include/cef_cookie.h"
#include "cefclient/common/client_switches.h"

#include <string>
using namespace std;

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

namespace client {

ClientAppBrowser::ClientAppBrowser() {
  CreateDelegates(delegates_);
}

void ClientAppBrowser::OnBeforeCommandLineProcessing(
    const CefString& process_type,
    CefRefPtr<CefCommandLine> command_line) {
  // Pass additional command-line flags to the browser process.
  if (process_type.empty()) {

	  OSVERSIONINFO osvi;
	  ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	  GetVersionEx(&osvi);
	  if (osvi.dwMajorVersion == 5)
	  {
		  command_line->AppendSwitch("disable-gpu");
		  command_line->AppendSwitch("disable-gpu-compositing");
	  }

	  	TCHAR szFilePath[MAX_PATH] = {0};
		GetModuleFileName(NULL, szFilePath, MAX_PATH);
		PathRemoveFileSpec(szFilePath);

		wstring strFlashPath = szFilePath;
		strFlashPath = strFlashPath + L"\\PepperFlash\\pepflashplayer.dll";
		const CefString strCefFlash = strFlashPath;

		command_line->AppendSwitchWithValue("--ppapi-flash-path", strCefFlash);
		command_line->AppendSwitchWithValue("--ppapi-flash-version", "21.0.0.216");
		command_line->AppendSwitch("--enable-media-stream");
		command_line->AppendSwitch("--force-wave-audio");
        //command_line->AppendSwitch("--disable-gpu-watchdog");
		
    // Pass additional command-line flags when off-screen rendering is enabled.
    if (command_line->HasSwitch(switches::kOffScreenRenderingEnabled)) {
      // If the PDF extension is enabled then cc Surfaces must be disabled for
      // PDFs to render correctly.
      // See https://bitbucket.org/chromiumembedded/cef/issues/1689 for details.
      if (!command_line->HasSwitch("disable-extensions") &&
          !command_line->HasSwitch("disable-pdf-extension")) {
        command_line->AppendSwitch("disable-surfaces");
      }

      // Use software rendering and compositing (disable GPU) for increased FPS
      // and decreased CPU usage. This will also disable WebGL so remove these
      // switches if you need that capability.
      // See https://bitbucket.org/chromiumembedded/cef/issues/1257 for details.
      if (!command_line->HasSwitch(switches::kEnableGPU)) {
        command_line->AppendSwitch("disable-gpu");
        command_line->AppendSwitch("disable-gpu-compositing");
      }

      // Synchronize the frame rate between all processes. This results in
      // decreased CPU usage by avoiding the generation of extra frames that
      // would otherwise be discarded. The frame rate can be set at browser
      // creation time via CefBrowserSettings.windowless_frame_rate or changed
      // dynamically using CefBrowserHost::SetWindowlessFrameRate. In cefclient
      // it can be set via the command-line using `--off-screen-frame-rate=XX`.
      // See https://bitbucket.org/chromiumembedded/cef/issues/1368 for details.
      command_line->AppendSwitch("enable-begin-frame-scheduling");
    }

    DelegateSet::iterator it = delegates_.begin();
    for (; it != delegates_.end(); ++it)
      (*it)->OnBeforeCommandLineProcessing(this, command_line);
  }
}

void ClientAppBrowser::OnContextInitialized() {
  // Register cookieable schemes with the global cookie manager.
  CefRefPtr<CefCookieManager> manager =
      CefCookieManager::GetGlobalManager(NULL);
  DCHECK(manager.get());
  manager->SetSupportedSchemes(cookieable_schemes_, NULL);

  print_handler_ = CreatePrintHandler();

  DelegateSet::iterator it = delegates_.begin();
  for (; it != delegates_.end(); ++it)
    (*it)->OnContextInitialized(this);
}

void ClientAppBrowser::OnBeforeChildProcessLaunch(
      CefRefPtr<CefCommandLine> command_line) {
  DelegateSet::iterator it = delegates_.begin();
  for (; it != delegates_.end(); ++it)
    (*it)->OnBeforeChildProcessLaunch(this, command_line);
}

void ClientAppBrowser::OnRenderProcessThreadCreated(
    CefRefPtr<CefListValue> extra_info) {
  DelegateSet::iterator it = delegates_.begin();
  for (; it != delegates_.end(); ++it)
    (*it)->OnRenderProcessThreadCreated(this, extra_info);
}

}  // namespace client
