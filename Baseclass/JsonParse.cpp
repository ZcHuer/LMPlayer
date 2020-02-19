#include "stdafx.h"
#include "JsonParse.h"
#include <assert.h>

BOOL ReadJsonFile(LPCWSTR lpwsJsonFile, Json::Value& root)
{
	if (NULL == lpwsJsonFile || !PathFileExists(lpwsJsonFile))
		return FALSE;

	CHandle hFile(CreateFile(lpwsJsonFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL));
	if (INVALID_HANDLE_VALUE == hFile)
		return FALSE;

	CAutoVectorPtr<BYTE> spBuf;
	BOOL bRtn = FALSE;
	do
	{
		DWORD dwSizeHigh = 0;
		DWORD dwSizeLow = GetFileSize(hFile, &dwSizeHigh);
		if (dwSizeLow == INVALID_FILE_SIZE  //此函数出错时返回INVALID_FILE_SIZE
			|| dwSizeHigh != 0				//非0时，文件超过4GB
			|| dwSizeLow > 5 * 1024 * 1024) //5M大小判断
			break;

		spBuf.Allocate(dwSizeLow + 8);
		if (!ReadFile(hFile, spBuf, dwSizeLow, &dwSizeHigh, NULL)
			|| dwSizeHigh != dwSizeLow)
			break;

		//对读取的数据增加字符串结尾符
		spBuf[dwSizeHigh] = 0;
		spBuf[dwSizeHigh + 1] = 0;

		try
		{
			Json::Reader reader;
			if (reader.parse((const char*)(LPBYTE)spBuf, (const char*)(LPBYTE)spBuf + dwSizeLow, root))
				bRtn = TRUE;
		}
		catch(...)
		{
			assert(false);
		}
	} while (FALSE);

	return bRtn;
}

BOOL ReadJson(LPCWSTR lpwsJson, Json::Value& root)
{
	if (NULL == lpwsJson)
		return FALSE;

	CUTFString utf(lpwsJson);

	BOOL bRtn = FALSE;
	try
	{
		Json::Reader reader;
		if (reader.parse(utf.UTF8(), utf.UTF8() + utf.LengthUTF8(), root))
			bRtn = TRUE;
	}
	catch (...)
	{
		assert(false);
	}

	return bRtn;
}

//=================================================================================================================

BOOL GetJsonStringUTF8(Json::Value& node, CStringA& strValue)
{
	if (!node.isString())
	{
		if (node.isNull())
		{
			strValue.Empty();
			return TRUE;
		}
		return FALSE;
	}

	BOOL bRtn = TRUE;
	try
	{
		strValue = node.asCString();
	}
	catch (...)
	{
		assert(false);
		bRtn = FALSE;
	}
	return bRtn;
}

BOOL GetJsonStringUTF16(Json::Value& node, CString& strValue)
{
	CStringA strUTF8;
	if (!GetJsonStringUTF8(node, strUTF8))
		return FALSE;
	CUTFString utf8((LPCSTR)strUTF8);
	strValue = utf8.UTF16();
	return TRUE;
}

BOOL GetJsonLong(Json::Value& node, LONG& lValue)
{
    if(!node.isInt())
        return GetJsonLongFromString(node, lValue);

	BOOL bRtn = TRUE;
	try
	{
	   lValue = node.asInt();
	}
	catch (...)
	{
		assert(false);
		bRtn = FALSE;
	}
	return bRtn;
}

BOOL GetJsonULong(Json::Value& node, ULONG& lValue)
{
	if (!node.isUInt())
		return GetJsonULongFromString(node, lValue);

	BOOL bRtn = TRUE;
	try
	{
		lValue = node.asUInt();
	}
	catch (...)
	{
		assert(false);
		bRtn = FALSE;
	}
	return bRtn;
}


BOOL GetJsonLongLong(Json::Value& node, LONGLONG& llValue)
{
    if(!node.isInt64())
        return GetJsonLongLongFromString(node, llValue);

	BOOL bRtn = TRUE;
	try
	{
	    llValue = node.asInt64();
	}
	catch (...)
	{
		assert(false);
		bRtn = FALSE;
	}
	return bRtn;
}

BOOL GetJsonULongLong(Json::Value& node, ULONGLONG& llValue)
{
	if (!node.isUInt64())
		return GetJsonULongLongFromString(node, llValue);
	
	BOOL bRtn = TRUE;
	try
	{
		llValue = node.asUInt64();
	}
	catch (...)
	{
		assert(false);
		bRtn = FALSE;
	}
	return bRtn;
}

BOOL GetJsonBool(Json::Value& node, BOOL& bValue)
{
	if (!node.isBool())
	{
		if (node.isString())
			return GetJsonBoolFromString(node, bValue);

		LONG lVal = 0;
		if (GetJsonLong(node, lVal))
			bValue = 0 != lVal;

		return FALSE;
	}

	BOOL bRtn = TRUE;
	try
	{
		bValue = node.asBool() ? TRUE : FALSE;
	}
	catch (...)
	{
		assert(false);
		bRtn = FALSE;
	}
	return bRtn;
}

BOOL GetJsonLongFromString(Json::Value& node, LONG& lValue)
{
	CStringA strNum;
	if (!GetJsonStringUTF8(node, strNum))
		return FALSE;

	lValue = atol(strNum);
	return TRUE;
}

BOOL GetJsonLongLongFromString(Json::Value& node, LONGLONG& llValue)
{
	CStringA strNum;
	if (!GetJsonStringUTF8(node, strNum))
		return FALSE;

	llValue = _atoi64(strNum);
	return TRUE;
}

BOOL GetJsonULongFromString(Json::Value& node, ULONG& lValue)
{
	CStringA strNum;
	if (!GetJsonStringUTF8(node, strNum))
		return FALSE;
	
	lValue = strtoul(strNum, NULL, 10);
	return TRUE;
}

BOOL GetJsonULongLongFromString(Json::Value& node, ULONGLONG& llValue)
{
	CStringA strNum;
	if (!GetJsonStringUTF8(node, strNum))
		return FALSE;

	CUTFString utf8((LPCSTR)strNum);
	llValue = _wcstoui64(utf8.UTF16(), NULL, 10);
	return TRUE;
}

BOOL GetJsonBoolFromString(Json::Value& node, BOOL& bValue)
{
	CStringA strBool;
	if (!GetJsonStringUTF8(node, strBool))
		return FALSE;

	if (strBool.CompareNoCase("true") == 0)
	{
		bValue = TRUE;
	}
	else if (strBool.CompareNoCase("false") == 0)
	{
		bValue = FALSE;
	}
	else
	{
		assert(FALSE);
		return FALSE;
	}
	return TRUE;
}

//=================================================================================================

BOOL GetJsonStringUTF8(Json::Value& node, LPCSTR lpsKey, CStringA& strValue)
{
	if (!node.isObject() || NULL == lpsKey)
		return FALSE;

	Json::Value subnode;
	if (!GetJsonNode(node, lpsKey, subnode))
		return FALSE;

	return GetJsonStringUTF8(subnode, strValue);
}

BOOL GetJsonStringUTF16(Json::Value& node, LPCSTR lpsKey, CString& strValue)
{
	CStringA strUTF8;
	if(!GetJsonStringUTF8(node, lpsKey, strUTF8))
		return FALSE;

	CUTFString utf8((LPCSTR)strUTF8);
	strValue = utf8.UTF16();
	return TRUE;
}

BOOL GetJsonLong(Json::Value& node, LPCSTR lpsKey, LONG& lValue)
{
	if (!node.isObject() || NULL == lpsKey)
		return FALSE;

	Json::Value subnode;
	if (!GetJsonNode(node, lpsKey, subnode))
		return FALSE;

	return GetJsonLong(subnode, lValue);
}

BOOL GetJsonULong(Json::Value& node, LPCSTR lpsKey, ULONG& lValue)
{
	if (!node.isObject() || NULL == lpsKey)
		return FALSE;

	Json::Value subnode;
	if (!GetJsonNode(node, lpsKey, subnode))
		return FALSE;

	return GetJsonULong(subnode, lValue);
}

BOOL GetJsonLongLong(Json::Value& node, LPCSTR lpsKey, LONGLONG& llValue)
{
	if (!node.isObject() || NULL == lpsKey)
		return FALSE;

	Json::Value subnode;
	if (!GetJsonNode(node, lpsKey, subnode))
		return FALSE;

	return GetJsonLongLong(subnode, llValue);
}

BOOL GetJsonULongLong(Json::Value& node, LPCSTR lpsKey, ULONGLONG& llValue)
{
	if (!node.isObject() || NULL == lpsKey)
		return FALSE;

	Json::Value subnode;
	if (!GetJsonNode(node, lpsKey, subnode))
		return FALSE;

	return GetJsonULongLong(subnode, llValue);
}

BOOL GetJsonBool(Json::Value& node, LPCSTR lpsKey, BOOL& bValue)
{
	if (!node.isObject() || NULL == lpsKey)
		return FALSE;

	Json::Value subnode;
	if (!GetJsonNode(node, lpsKey, subnode))
		return FALSE;

	return GetJsonBool(subnode, bValue);	
}

BOOL GetJsonLongFromString(Json::Value& node, LPCSTR lpsKey, LONG& lValue)
{
	if (!node.isObject() || NULL == lpsKey)
		return FALSE;

	Json::Value subnode;
	if (!GetJsonNode(node, lpsKey, subnode))
		return FALSE;

	return GetJsonLongFromString(subnode, lValue);
}

BOOL GetJsonLongLongFromString(Json::Value& node, LPCSTR lpsKey, LONGLONG& llValue)
{
	if (!node.isObject() || NULL == lpsKey)
		return FALSE;

	Json::Value subnode;
	if (!GetJsonNode(node, lpsKey, subnode))
		return FALSE;

	return GetJsonLongLongFromString(subnode, llValue);
}

BOOL GetJsonULongFromString(Json::Value& node, LPCSTR lpsKey, ULONG& lValue)
{
	if (!node.isObject() || NULL == lpsKey)
		return FALSE;

	Json::Value subnode;
	if (!GetJsonNode(node, lpsKey, subnode))
		return FALSE;

	return GetJsonULongFromString(subnode, lValue);
}

BOOL GetJsonULongLongFromString(Json::Value& node, LPCSTR lpsKey, ULONGLONG& llValue)
{
	if (!node.isObject() || NULL == lpsKey)
		return FALSE;

	Json::Value subnode;
	if (!GetJsonNode(node, lpsKey, subnode))
		return FALSE;

	return GetJsonULongLongFromString(subnode, llValue);
}

//=================================================================================================

BOOL SafeGetJsonNode(const Json::Value& root, LPCSTR lpsPath, Json::Value& node)
{
	if(NULL == lpsPath || !root.isObject())
		return FALSE;

	LPCSTR psEnd = StrStrIA(lpsPath, "\\");
	if(psEnd)
	{
		if(!root.isMember(lpsPath, psEnd))
			return FALSE;
		
		Json::Value const* found = root.find(lpsPath, psEnd);
		if (!found) 
			return FALSE;

		const Json::Value& root2 = *found;
		return GetJsonNode(root2, psEnd+1, node);
	}
	if(!root.isMember(lpsPath))
		return FALSE;

	node = root[lpsPath];
	return TRUE;
}

BOOL GetJsonNode(const Json::Value& root, LPCSTR lpsPath, Json::Value& node)
{
	BOOL bRtn = TRUE;
	try
	{
		bRtn = SafeGetJsonNode(root, lpsPath, node);
	}
	catch (...)
	{
		assert(false);
		bRtn = FALSE;
	}
	return bRtn;
}

bool IsNumber(LPCSTR lps)
{
	int nLen = lps ? lstrlenA(lps) : 0;
	for (int i = 0; i < nLen; ++i)
		if (!isdigit((int)(*lps)))
			return false;
	return nLen > 0;
}

BOOL PopFirstPath(const CStringA& strXPath, CStringA& strFirstPath, CStringA& strXPathTail)
{
	if (strXPath.IsEmpty())
		return FALSE;

	int length = strXPath.GetLength();
	int lenBuf = (((length >> 2) + 1) << 2);//4字节对齐

	int scan = _snscanf_s(strXPath, strXPath.GetLength(),
		"%[^\\]\\%s",
		strFirstPath.GetBufferSetLength(lenBuf), length,
		strXPathTail.GetBufferSetLength(lenBuf), length);
	strFirstPath.ReleaseBuffer();
	strXPathTail.ReleaseBuffer();


	if((scan == -1 && strXPath.Find("\\") == -1)
		|| scan == 0)
		strFirstPath = strXPath;//[nodename]

	return !strFirstPath.IsEmpty();
}

enum EArrayPathType
{
	EAPT_Error = 0,
	EAPT_OnlyName = 1,
	EAPT_OnlyIndex = 2,
	EAPT_NameAndIndex = 3,
};

EArrayPathType ParseArrayPath(const CStringA& strArrayPath, CStringA& strPathName, CStringA& strArrayIndex)
{
	if (strArrayPath.IsEmpty())
		return EAPT_Error;

	int length = strArrayPath.GetLength();
	int lenBuf = (((length >> 2) + 1) << 2);//4字节对齐

	CStringA strNodeNameTail;
	int scan = 0;
	if (strArrayPath.GetAt(0) == '[')
	{
		scan = _snscanf_s(strArrayPath, length,
			"[%[^]]]%s",
			strArrayIndex.GetBufferSetLength(lenBuf), length,
			strNodeNameTail.GetBufferSetLength(lenBuf), length);
		strArrayIndex.ReleaseBuffer();
		strNodeNameTail.ReleaseBuffer();

		if (scan == 1 && !strArrayIndex.IsEmpty() && strNodeNameTail.IsEmpty())
			return EAPT_OnlyIndex;
	}
	else if (strArrayPath.Find("[") == -1)
	{
		strPathName = strArrayPath;
		if (!strPathName.IsEmpty())
			return EAPT_OnlyName;
	}
	else
	{
		scan = _snscanf_s(strArrayPath, length,
			"%[^[][%[^]]]%s",
			strPathName.GetBufferSetLength(lenBuf), length,
			strArrayIndex.GetBufferSetLength(lenBuf), length,
			strNodeNameTail.GetBufferSetLength(lenBuf), length);
		strPathName.ReleaseBuffer();
		strArrayIndex.ReleaseBuffer();
		strNodeNameTail.ReleaseBuffer();

		if (scan == 2 && !strPathName.IsEmpty() && !strArrayIndex.IsEmpty() && strNodeNameTail.IsEmpty())
			return EAPT_NameAndIndex;
	}

	return EAPT_Error;
}

BOOL GetJsonNodeEx(const Json::Value& root, LPCSTR lpsXPath, Json::Value& node)
{
	if (NULL == lpsXPath || (!root.isObject() && !root.isArray()))
		return FALSE;

	/*
	XPath :
	nodename
	[index]
	nodename1\nodename2\nodename3
	[index]\nodename1\nodename2
	nodename1\nodename2[index]\nodename3
	*/

	CStringA strXPathTail;
	CStringA strFirstPath;
	if (!PopFirstPath(lpsXPath, strFirstPath, strXPathTail))
	{
		ATLASSERT(FALSE);
		return FALSE;
	}

	CStringA strNodeName;
	CStringA strIndex;
	switch (ParseArrayPath(strFirstPath, strNodeName, strIndex))
	{
	case EAPT_OnlyName:
	{
		if (strXPathTail.IsEmpty())
		{
			return GetJsonNode(root, strNodeName, node);
		}
		else
		{
			Json::Value root2;
			if (GetJsonNode(root, strNodeName, root2))
			{
				return GetJsonNodeEx(root2, strXPathTail, node);
			}
		}
		break;
	}
	case EAPT_NameAndIndex:
	{
		Json::Value root2;
		if (GetJsonNode(root, strNodeName, root2)
			&& root2.isArray()
			&& IsNumber(strIndex))
		{
			Json::ArrayIndex index = (Json::ArrayIndex)atoi(strIndex);
			if (index < root2.size())
			{
				if (strXPathTail.IsEmpty())
				{
					node = root2[index];
					return !node.isNull();
				}
				else
				{
					Json::Value root3 = root2[index];
					return GetJsonNodeEx(root3, strXPathTail, node);
				}
			}
		}
		break;
	}
	case EAPT_OnlyIndex:
	{
		if (root.isArray() && IsNumber(strIndex))
		{
			Json::ArrayIndex index = (Json::ArrayIndex)atoi(strIndex);
			if (index < root.size())
			{
				if (strXPathTail.IsEmpty())
				{
					node = root[index];
					return !node.isNull();
				}
				else
				{
					Json::Value root2 = root[index];
					return GetJsonNodeEx(root2, strXPathTail, node);
				}
			}
		}
		break;
	}
	case EAPT_Error:
	default:
	{
		ATLASSERT(FALSE);
		break;
	}
	}
	return FALSE;
}

//=================================================================================================

BOOL GetJsonNodeStringUTF8(const Json::Value& root, CStringA& strValue)
{
	if (!root.isObject())
		return FALSE;

	BOOL bRtn = TRUE;
	try
	{
		Json::FastWriter writer;
		writer.omitEndingLineFeed();
		std::string doc = writer.write(root);
		strValue = doc.c_str();
	}
	catch (...)
	{
		assert(false);
		bRtn = FALSE;
	}
	return bRtn;
}

BOOL GetJsonNodeStringUTF16(const Json::Value& root, CString& strValue)
{
	CStringA strJson;
	if (!GetJsonNodeStringUTF8(root, strJson))
		return FALSE;

	CUTFString utf((LPCSTR)strJson);
	strValue = utf.UTF16();
	return TRUE;
}

BOOL GetJsonNodeStringUTF8(const Json::Value& node, LPCSTR lpsKey, CStringA& strValue)
{
	Json::Value data;
	if (!GetJsonNode(node, lpsKey, data))
		return FALSE;
	
	return GetJsonNodeStringUTF8(data, strValue);
}

BOOL GetJsonNodeStringUTF16(const Json::Value& node, LPCSTR lpsKey, CString& strValue)
{
	CStringA strJson;
	if (!GetJsonNodeStringUTF8(node, lpsKey, strJson))
		return FALSE;

	CUTFString utf((LPCSTR)strJson);
	strValue = utf.UTF16();
	return TRUE;
}

//=================================================================================================
BOOL SetJsonValue(Json::Value& node, LPCSTR lpsKey, LONG lValue, BOOL bAsString/* = FALSE*/)
{
	if (NULL == lpsKey)
		return FALSE;

	if (bAsString)
	{
		CHAR szBuf[32] = { 0 };
		StringCchPrintfA(szBuf, 30, "%d", lValue);
		return SetJsonValue(node, lpsKey, szBuf);
	}

	BOOL bRtn = TRUE;
	try
	{
		node[lpsKey] = (Json::Int)lValue;
	}
	catch (...)
	{
		assert(false);
		bRtn = FALSE;
	}
	return bRtn;
}

BOOL SetJsonValue(Json::Value& node, LPCSTR lpsKey, ULONG lValue, BOOL bAsString/* = FALSE*/)
{
	if(NULL == lpsKey)
		return FALSE;

	if(bAsString)
	{
		CHAR szBuf[32] = {0};
		StringCchPrintfA(szBuf, 30, "%u", lValue);
		return SetJsonValue(node, lpsKey, szBuf);
	}

	BOOL bRtn = TRUE;
	try
	{
		node[lpsKey] = (Json::UInt)lValue;
	}
	catch (...)
	{
		assert(false);
		bRtn = FALSE;
	}
	return bRtn;
}

BOOL SetJsonValue(Json::Value& node, LPCSTR lpsKey, LONGLONG llValue, BOOL bAsString/* = FALSE*/)
{
	if(NULL == lpsKey)
		return FALSE;

	if(bAsString)
	{
		CHAR szBuf[32] = {0};
		StringCchPrintfA(szBuf, 30, "%I64d", llValue);
		return SetJsonValue(node, lpsKey, szBuf);
	}

	BOOL bRtn = TRUE;
	try
	{
		node[lpsKey] = (Json::Int64)llValue;
	}
	catch (...)
	{
		assert(false);
		bRtn = FALSE;
	}
	return bRtn;
}

BOOL SetJsonValue(Json::Value& node, LPCSTR lpsKey, ULONGLONG llValue, BOOL bAsString/* = FALSE*/)
{
	if(NULL == lpsKey)
		return FALSE;

	if(bAsString)
	{
		CHAR szBuf[32] = {0};
		StringCchPrintfA(szBuf, 30, "%I64u", llValue);
		return SetJsonValue(node, lpsKey, szBuf);
	}

	BOOL bRtn = TRUE;
	try
	{
		node[lpsKey] = (Json::UInt64)llValue;
	}
	catch (...)
	{
		assert(false);
		bRtn = FALSE;
	}
	return bRtn;
}

bool JsonEncode(LPCSTR lpsString, std::string& strValue)
{
	if (NULL == lpsString)
		return false;

	strValue.clear();

	LPCSTR psWork = lpsString;
	LPCSTR psEnd = psWork + lstrlenA(psWork);

	bool bRtn = true;
	while (psWork && psWork < psEnd)
	{
		byte cp = (byte)(*psWork);
		unsigned int uCode = 0;
		if (cp <= 0x7f)
		{
			//DoNothing
			psWork += 1;
		}
		else if (cp <= 0xDF)
		{
			if (psWork + 1 >= psEnd)
			{
				ATLASSERT(FALSE);
				bRtn = false;
				break;
			}

			char cp2 = 0x3F & (*(psWork + 1));
			char cp1 = 0x1F & (*(psWork));
			uCode = (((unsigned int)cp1 << 6) | (unsigned int)cp2);

			psWork += 2;
		}
		else if (cp <= 0xEF)
		{
			if (psWork + 2 >= psEnd)
			{
				ATLASSERT(FALSE);
				bRtn = false;
				break;
			}

			char cp3 = 0x3F & (*(psWork + 2));
			char cp2 = 0x3F & (*(psWork + 1));
			char cp1 = 0x0F & (*(psWork));
			uCode = (((unsigned int)cp1 << 12) | ((unsigned int)cp2 << 6) | cp3);

			psWork += 3;
		}
		else if (cp <= 0xF7)
		{
			if (psWork + 3 >= psEnd)
			{
				ATLASSERT(FALSE);
				bRtn = false;
				break;
			}

			char cp4 = 0x3F & (*(psWork + 3));
			char cp3 = 0x3F & (*(psWork + 2));
			char cp2 = 0x3F & (*(psWork + 1));
			char cp1 = 0x7 & (*(psWork));
			uCode = (((unsigned int)cp1 << 18) | ((unsigned int)cp2 << 12) | ((unsigned int)cp3 << 6) | cp4);

			psWork += 4;
		}
		else
		{
			bRtn = false;
			ATLASSERT(FALSE);
			break;
		}

		char szBuf[16] = { 0 };
		if (uCode == 0)
		{
			szBuf[0] = cp;
			szBuf[1] = '\0';
		}
		else
		{
			sprintf_s(szBuf, 16, "\\u%04X", uCode);
		}
		strValue += szBuf;
	}
	return bRtn;
}

BOOL SetJsonValue(Json::Value& node, LPCSTR lpsKey, LPCSTR lpsValue, BOOL bEncode/* = FALSE*/)
{
	if(NULL == lpsKey)
		return FALSE;

	BOOL bRtn = TRUE;
	try
	{
		if (bEncode)
		{
			std::string strEncode;
			if (!JsonEncode(lpsValue, strEncode))
				bRtn = FALSE;
			else
				node[lpsKey] = Json::Value(strEncode);
		}
		else
		{
			node[lpsKey] = Json::Value(lpsValue);
		}
	}
	catch (...)
	{
		assert(false);
		bRtn = FALSE;
	}
	return bRtn;
}

BOOL SetJsonValue(Json::Value& node, LPCSTR lpsKey, LPCWSTR lpwsValue, BOOL bEncode/* = FALSE*/)
{
	if(NULL == lpsKey)
		return FALSE;

	CUTFString strUTF8((LPCWSTR)lpwsValue);
	return SetJsonValue(node, lpsKey, (LPCSTR)strUTF8.UTF8(), bEncode);
}

//=================================================================================================



CUTFString RootToString(const Json::Value& root)
{
	Json::FastWriter writer;
	writer.omitEndingLineFeed();
	std::string doc = writer.write(root);
	CUTFString utf(doc.c_str());
	return utf;
}

BOOL JsonWriteToStringUTF8( const Json::Value& root, CStringA& strJsonString )
{
	BOOL bRtn = TRUE;
	try
	{
		CUTFString utf = RootToString(root);
		strJsonString = utf.UTF8();
	}
	catch(...)
	{
		assert(false);
		bRtn = FALSE;
	}

	return bRtn;
}

BOOL JsonWriteToStringUTF16( const Json::Value& root, CString& strJsonString )
{
	BOOL bRtn = TRUE;
	try
	{
		CUTFString utf = RootToString(root);
		strJsonString = utf.UTF16();
	}
	catch(...)
	{
		assert(false);
		bRtn = FALSE;
	}
	return bRtn;
}

//=================================================================================================
