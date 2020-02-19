#include "stdafx.h"
#include "JsonParse2.h"
#include <assert.h>

BOOL ReadJson(INetStream* lpStream, Json::Value& root)
{
	if (NULL == lpStream || lpStream->GetValidSize() == 0)
		return FALSE;

	BOOL bRtn = FALSE;
	try
	{
		Json::Reader reader;
		if (reader.parse((const char*)lpStream->GetBuffer(0), (const char*)lpStream->GetBuffer(lpStream->GetValidSize()), root))
			bRtn = TRUE;
	}
	catch (...)
	{
		assert(false);
	}
	return bRtn;
}

BOOL ReadJson(INetStream* lpStream, Json::Value& root, LONG* lplError, CString* lpstrErrMsg)
{
	if (NULL == lpStream || lpStream->GetValidSize() == 0)
		return FALSE;

	if (!ReadJson(lpStream, root))
		return FALSE;

	if (lpstrErrMsg)
	{
		Json::Value vMsg;
		if (GetJsonNode(root, "res", vMsg))
		{
			GetJsonStringUTF16(vMsg, "remindMsg", *lpstrErrMsg);
		}
	}

	if (lplError)
	{
		LONG lError = 0;
		if (!GetJsonLong(root, "code", lError))
			return FALSE;

		*lplError = lError;
		if (lError != DEF_ERRORCODE_OK)
			return FALSE;
	}
	return TRUE;
}

void ConvertJsonArrayToVector(Json::Value& array, std::vector<ULONGLONG>& vec)
{
    if (array.type() != Json::arrayValue)
    {
        assert(false);
        return;
    }

    Json::ArrayIndex size = array.size();
    for (Json::ArrayIndex i = 0; i < size; i++)
    {
        ULONGLONG v = 0;
        if (GetJsonULongLong(array[i], v)) {
            vec.push_back(v);
        }
        else {
            assert(false);
        }
    }
}

void ConvertVectorToJsonArray(const std::vector<ULONGLONG>& vec, Json::Value& array)
{
    if(array.type() == Json::arrayValue || array.type() == Json::nullValue) {
        for (const ULONGLONG& id : vec)
        {
            CStringA temp;
            temp.Format("%I64u", id);
            array.append((LPCSTR)temp);
        }
    }
    else {
        assert(false);
    }
}

//=================================================================================================

LONGLONG GetLongLong(Json::Value& root, LPCSTR lpsKey, LONGLONG defaultValue/* = 0*/)
{
    LONGLONG value = defaultValue;
    if (!GetJsonLongLong(root, lpsKey, value))
    {
        assert(false);
    }
    return value;
}

LONG GetLong(Json::Value& root, LPCSTR lpsKey, LONG defaultValue/* = 0*/)
{
    LONG value = defaultValue;
    if (!GetJsonLong(root, lpsKey, value))
    {
        assert(false);
    }
    return value;
}

BOOL GetJsonString(Json::Value& root, CString& strValue)
{
    Json::FastWriter writer;
	writer.omitEndingLineFeed();
    std::string doc = writer.write(root);
    CUTFString utf(doc.c_str());
    strValue = utf.UTF16();
    strValue.Remove(L'\r');
    strValue.Remove(L'\n');
    return TRUE;
}

LONGLONG GetLongLong(Json::Value& root, LONGLONG defaultValue/* = 0*/)
{
    LONGLONG value = defaultValue;
    if (!GetJsonLongLong(root, value))
    {
        assert(false);
    }
    return value;
}

LONG GetLong(Json::Value& root, LONG defaultValue/* = 0*/)
{
    LONG value = defaultValue;
    if (!GetJsonLong(root, value))
    {
        assert(false);
    }
    return value;
}
