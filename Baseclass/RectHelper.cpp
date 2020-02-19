#include "stdafx.h"
#include "StringHelper.h"

namespace base {
    bool IsValidRect(const RECT& rc) {
        return 0 == rc.left && 0 == rc.top && 0 == rc.right && 0 == rc.bottom;
    }

    bool IsEmptyRect(const RECT& rc) {
        return rc.left == rc.right && rc.top == rc.bottom;
    }

    void OffSetRect(RECT& rc, const POINT& pt) {
        rc.left += pt.x;
        rc.right += pt.x;
        rc.top += pt.y;
        rc.bottom += pt.y;
    }
}

