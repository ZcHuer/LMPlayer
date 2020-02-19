#ifndef __RECT_HELPER_H__
#define __RECT_HELPER_H__
#pragma once

#include <windef.h>

namespace base {
    bool IsValidRect(const RECT& rc);
    bool IsEmptyRect(const RECT& rc);
    void OffSetRect(RECT& rc, const POINT& pt);
}

#endif
