// Copyright (c) 2016 Marshall A. Greenblatt. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//    * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//    * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//    * Neither the name of Google Inc. nor the name Chromium Embedded
// Framework nor the names of its contributors may be used to endorse
// or promote products derived from this software without specific prior
// written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// ---------------------------------------------------------------------------
//
// This file was generated by the CEF translator tool and should not edited
// by hand. See the translator.README.txt file in the tools directory for
// more information.
//

#ifndef CEF_INCLUDE_CAPI_CEF_WEB_PLUGIN_CAPI_H_
#define CEF_INCLUDE_CAPI_CEF_WEB_PLUGIN_CAPI_H_
#pragma once

#include "include/capi/cef_base_capi.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _cef_browser_t;

///
// Information about a specific web plugin.
///
typedef struct _cef_web_plugin_info_t {
  ///
  // Base structure.
  ///
  cef_base_t base;

  ///
  // Returns the plugin name (i.e. Flash).
  ///
  // The resulting string must be freed by calling cef_string_userfree_free().
  cef_string_userfree_t (CEF_CALLBACK *get_name)(
      struct _cef_web_plugin_info_t* self);

  ///
  // Returns the plugin file path (DLL/bundle/library).
  ///
  // The resulting string must be freed by calling cef_string_userfree_free().
  cef_string_userfree_t (CEF_CALLBACK *get_path)(
      struct _cef_web_plugin_info_t* self);

  ///
  // Returns the version of the plugin (may be OS-specific).
  ///
  // The resulting string must be freed by calling cef_string_userfree_free().
  cef_string_userfree_t (CEF_CALLBACK *get_version)(
      struct _cef_web_plugin_info_t* self);

  ///
  // Returns a description of the plugin from the version information.
  ///
  // The resulting string must be freed by calling cef_string_userfree_free().
  cef_string_userfree_t (CEF_CALLBACK *get_description)(
      struct _cef_web_plugin_info_t* self);
} cef_web_plugin_info_t;


///
// Structure to implement for visiting web plugin information. The functions of
// this structure will be called on the browser process UI thread.
///
typedef struct _cef_web_plugin_info_visitor_t {
  ///
  // Base structure.
  ///
  cef_base_t base;

  ///
  // Method that will be called once for each plugin. |count| is the 0-based
  // index for the current plugin. |total| is the total number of plugins.
  // Return false (0) to stop visiting plugins. This function may never be
  // called if no plugins are found.
  ///
  int (CEF_CALLBACK *visit)(struct _cef_web_plugin_info_visitor_t* self,
      struct _cef_web_plugin_info_t* info, int count, int total);
} cef_web_plugin_info_visitor_t;


///
// Structure to implement for receiving unstable plugin information. The
// functions of this structure will be called on the browser process IO thread.
///
typedef struct _cef_web_plugin_unstable_callback_t {
  ///
  // Base structure.
  ///
  cef_base_t base;

  ///
  // Method that will be called for the requested plugin. |unstable| will be
  // true (1) if the plugin has reached the crash count threshold of 3 times in
  // 120 seconds.
  ///
  void (CEF_CALLBACK *is_unstable)(
      struct _cef_web_plugin_unstable_callback_t* self,
      const cef_string_t* path, int unstable);
} cef_web_plugin_unstable_callback_t;


///
// Visit web plugin information. Can be called on any thread in the browser
// process.
///
CEF_EXPORT void cef_visit_web_plugin_info(
    cef_web_plugin_info_visitor_t* visitor);

///
// Cause the plugin list to refresh the next time it is accessed regardless of
// whether it has already been loaded. Can be called on any thread in the
// browser process.
///
CEF_EXPORT void cef_refresh_web_plugins();

///
// Unregister an internal plugin. This may be undone the next time
// cef_refresh_web_plugins() is called. Can be called on any thread in the
// browser process.
///
CEF_EXPORT void cef_unregister_internal_web_plugin(const cef_string_t* path);

///
// Register a plugin crash. Can be called on any thread in the browser process
// but will be executed on the IO thread.
///
CEF_EXPORT void cef_register_web_plugin_crash(const cef_string_t* path);

///
// Query if a plugin is unstable. Can be called on any thread in the browser
// process.
///
CEF_EXPORT void cef_is_web_plugin_unstable(const cef_string_t* path,
    cef_web_plugin_unstable_callback_t* callback);

#ifdef __cplusplus
}
#endif

#endif  // CEF_INCLUDE_CAPI_CEF_WEB_PLUGIN_CAPI_H_
