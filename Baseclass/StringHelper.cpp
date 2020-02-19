#include "stdafx.h"
#include "StringHelper.h"

namespace base {

    CString N2S(int num)
    {
        CString str;
        str.Format(L"%d", num);
        return str;
    }

    CString N2S(LONG num)
    {
        CString str;
        str.Format(L"%d", num);
        return str;
    }

    CString N2S(ULONG num)
    {
        CString str;
        str.Format(L"%u", num);
        return str;
    }

    CString N2S(ULONGLONG num)
    {
        CString str;
        str.Format(L"%I64u", num);
        return str;
    }
}

