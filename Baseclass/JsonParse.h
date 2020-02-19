#pragma once
#include "..\3rdparty\jsoncpp\include\json\json.h"

/*
从文件读取Json数据
文件必须以 UTF8 无BOM头方式保存
文件内容必须小于5M，否则返回错误
内部捕获c++异常
*/
BOOL ReadJsonFile(LPCWSTR lpwsJsonFile, Json::Value& root);

/*
将以UTF16编码的Json串转换为UTF8串后再解析
内部捕获c++异常
*/
BOOL ReadJson(LPCWSTR lpwsJson, Json::Value& root);

/*
获取当前结点的对应类型值
注：获取数字类型值时，内部会做字符到数字的转换
内部捕获c++异常
*/
BOOL GetJsonStringUTF8(Json::Value& node, CStringA& strValue);
BOOL GetJsonStringUTF16(Json::Value& node, CString& strValue);	//内部做UTF8到UTF16转换
BOOL GetJsonLong(Json::Value& node, LONG& lValue);
BOOL GetJsonULong(Json::Value& node, ULONG& lValue);
BOOL GetJsonLongLong(Json::Value& node, LONGLONG& llValue);
BOOL GetJsonULongLong(Json::Value& node, ULONGLONG& llValue);
BOOL GetJsonBool(Json::Value& node, BOOL& bValue);//内部处理字符串true/false, 数字0或非0

/*
获取当前字符串类型结点的对应数字类型值
内部会做字符到数字的转换
内部捕获c++异常
*/
BOOL GetJsonLongFromString(Json::Value& node, LONG& lValue);
BOOL GetJsonULongFromString(Json::Value& node, ULONG& lValue);
BOOL GetJsonLongLongFromString(Json::Value& node, LONGLONG& llValue);
BOOL GetJsonULongLongFromString(Json::Value& node, ULONGLONG& llValue);
BOOL GetJsonBoolFromString(Json::Value& node, BOOL& bValue);

/*
获取当前结点的命名子结点对应类型值
支持多级子结点,如： subnode1\subnode2\subnode3
获取数字类型值时，内部会做字符到数字的转换
内部捕获c++异常
*/
BOOL GetJsonStringUTF8(Json::Value& node, LPCSTR lpsKey, CStringA& strValue);
BOOL GetJsonStringUTF16(Json::Value& node, LPCSTR lpsKey, CString& strValue);
BOOL GetJsonLong(Json::Value& node, LPCSTR lpsKey, LONG& lValue);
BOOL GetJsonULong(Json::Value& node, LPCSTR lpsKey, ULONG& lValue);
BOOL GetJsonLongLong(Json::Value& node, LPCSTR lpsKey, LONGLONG& llValue);
BOOL GetJsonULongLong(Json::Value& node, LPCSTR lpsKey, ULONGLONG& llValue);
BOOL GetJsonBool(Json::Value& node, LPCSTR lpsKey, BOOL& bValue);

/*
获取当前字符串类型结点的命名子结点的对应数字类型值
支持多级子结点,如： subnode1\subnode2\subnode3
内部会做字符到数字的转换
内部捕获c++异常
*/
BOOL GetJsonLongFromString(Json::Value& node, LPCSTR lpsKey, LONG& lValue);
BOOL GetJsonULongFromString(Json::Value& node, LPCSTR lpsKey, ULONG& lValue);
BOOL GetJsonLongLongFromString(Json::Value& node, LPCSTR lpsKey, LONGLONG& llValue);
BOOL GetJsonULongLongFromString(Json::Value& node, LPCSTR lpsKey, ULONGLONG& llValue);

/*
获取子结点，支持多级子结点
内部捕获c++异常
如： subnode1\subnode2\subnode3
*/
BOOL GetJsonNode(const Json::Value& root, LPCSTR lpsPath, Json::Value& node);

/*
获取子结点，支持多级子结点，支持数组子结点
内部捕获c++异常
XPath 示例:
nodename
[index]
nodename1\nodename2\nodename3
[index]\nodename1\nodename2
nodename1\nodename2[index]\nodename3
*/
BOOL GetJsonNodeEx(const Json::Value& root, LPCSTR lpsXPath, Json::Value& node);

/*
获得当前对象结点的Json串
内部捕获c++异常
*/
BOOL GetJsonNodeStringUTF8(const Json::Value& root, CStringA& strValue);
BOOL GetJsonNodeStringUTF16(const Json::Value& root, CString& strValue);

/*
获得当前结点的对象类型子结点的Json串
支持多级子结点
内部捕获c++异常
*/
BOOL GetJsonNodeStringUTF8(const Json::Value& root, LPCSTR lpsKey, CStringA& strValue);
BOOL GetJsonNodeStringUTF16(const Json::Value& root, LPCSTR lpsKey, CString& strValue);

/*
设置结点值
支持将数字转换为字符串，支持\uXXXX编码
内部捕获c++异常
*/
BOOL SetJsonValue(Json::Value& node, LPCSTR lpsKey, LONG lValue, BOOL bAsString = FALSE);
BOOL SetJsonValue(Json::Value& node, LPCSTR lpsKey, ULONG lValue, BOOL bAsString = FALSE);
BOOL SetJsonValue(Json::Value& node, LPCSTR lpsKey, LONGLONG llValue, BOOL bAsString = FALSE);
BOOL SetJsonValue(Json::Value& node, LPCSTR lpsKey, ULONGLONG llValue, BOOL bAsString = FALSE);
BOOL SetJsonValue(Json::Value& node, LPCSTR lpsKey, LPCSTR lpsValue, BOOL bEncode = FALSE);
BOOL SetJsonValue(Json::Value& node, LPCSTR lpsKey, LPCWSTR lpwsValue, BOOL bEncode = FALSE);


/*
格式化Json串
内部捕获c++异常
*/

CUTFString RootToString(const Json::Value& root);
BOOL JsonWriteToStringUTF8( const Json::Value& root, CStringA& strJsonString );
BOOL JsonWriteToStringUTF16( const Json::Value& root, CString& strJsonString );