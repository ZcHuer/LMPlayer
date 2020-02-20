// Copyright (c) 2017 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
//
// ---------------------------------------------------------------------------
//
// This file was generated by the CEF translator tool. If making changes by
// hand only do so within the body of existing method and function
// implementations. See the translator.README.txt file in the tools directory
// for more information.
//
// $hash=e61dc433c36e5cdcd658328701b69cfcbb58df38$
//

#include "libcef_dll/cpptoc/get_geolocation_callback_cpptoc.h"

namespace {

// MEMBER FUNCTIONS - Body may be edited by hand.

void CEF_CALLBACK get_geolocation_callback_on_location_update(
    struct _cef_get_geolocation_callback_t* self,
    const struct _cef_geoposition_t* position) {
  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  DCHECK(self);
  if (!self)
    return;
  // Verify param: position; type: struct_byref_const
  DCHECK(position);
  if (!position)
    return;

  // Translate param: position; type: struct_byref_const
  CefGeoposition positionObj;
  if (position)
    positionObj.Set(*position, false);

  // Execute
  CefGetGeolocationCallbackCppToC::Get(self)->OnLocationUpdate(positionObj);
}

}  // namespace

// CONSTRUCTOR - Do not edit by hand.

CefGetGeolocationCallbackCppToC::CefGetGeolocationCallbackCppToC() {
  GetStruct()->on_location_update = get_geolocation_callback_on_location_update;
}

template <>
CefRefPtr<CefGetGeolocationCallback>
CefCppToCRefCounted<CefGetGeolocationCallbackCppToC,
                    CefGetGeolocationCallback,
                    cef_get_geolocation_callback_t>::
    UnwrapDerived(CefWrapperType type, cef_get_geolocation_callback_t* s) {
  NOTREACHED() << "Unexpected class type: " << type;
  return NULL;
}

#if DCHECK_IS_ON()
template <>
base::AtomicRefCount CefCppToCRefCounted<
    CefGetGeolocationCallbackCppToC,
    CefGetGeolocationCallback,
    cef_get_geolocation_callback_t>::DebugObjCt ATOMIC_DECLARATION;
#endif

template <>
CefWrapperType
    CefCppToCRefCounted<CefGetGeolocationCallbackCppToC,
                        CefGetGeolocationCallback,
                        cef_get_geolocation_callback_t>::kWrapperType =
        WT_GET_GEOLOCATION_CALLBACK;
