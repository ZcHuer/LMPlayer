#ifndef __UTIL_H__
#define __UTIL_H__

#include <string>
#include <vector>
#include <map>


#include "Buffer.h"

class CUtil {
public:

	static std::string	UnicodeToUTF8(const std::wstring& str);
	static std::wstring UTF8ToUnicode(const std::string& str);

	static std::string	AnsiToUTF8(const std::string& str);
	static std::string	UTF8ToAnsi(const std::string& str);

	static std::wstring	AnsiToUnicode(const std::string& str);
	static std::string	UnicodeToAnsi(const std::wstring& str);

	static int			StrlenUTF8(const std::string& str);

public:

	static std::string  TimestampToString(int64_t ts);
	static std::string  TimestampToString(int64_t ts, const std::string& strFormat);
	static std::string  SystemTimestampToString();
	static std::wstring PrettySize(uint64_t n);

//	static std::string  CalcMD5(const std::string& str);
//	static std::string  CalcMD5(const std::wstring& str);
//	static std::string  CalcMD5(const void* pBuf,int nSize);
//	static std::string  CalcMD5(const Buffer& buf);
//
//	static std::string  CalcFileMD5(const std::string& strFile);
//	static std::string  CalcFileMD5(const std::wstring& strFile);
//
//
//	static void			SetBuffer(Buffer& buf, void* pData, int nSize);
//
//	static void			EncryptAES(const std::string& strkey, void* pData, int nSize);
//	static void			DecryptAES(const std::string& strkey, void* pData, int nSize);
//
//	static bool			EncryptAESFull(const std::string& strkey, void* pData, int nSize);
//	static bool			DecryptAESFull(const std::string& strkey, void* pData, int nSize);
//
//
//public:
//
//	static bool			HttpRequestGet(const std::string& strURL, std::string& strRet);
//	static bool			HttpRequestGetAsBuffer(const std::string& strURL, Buffer& bufRet);
//
//	static bool			HttpRequestPost(const std::string& strURL, const std::string& strFilename, std::map<std::string,std::string>* pMapFields,std::string& strRet);
//	static bool			HttpRequestPostFormFile(const std::string& strURL, const std::string& strKFileName, const std::string& strVFilename, std::map<std::string, std::string>* pMapFields, std::string& strRet);
//	static bool			HttpRequestPost(const std::string& strURL, std::map<std::string, std::string>* pMapFields, std::string& strRet);
//
//	//static BYTE			toHex(const BYTE &x);
//	//static BYTE			fromHex(const BYTE &x);
//	//static std::string	URLEncode(const std::string &sIn);
//	//static std::string	URLDecode(const std::string &sIn);

public:

	static bool			CompareVersion(const std::wstring& strVer1, const std::wstring& strVer2, int &nResult);

	static std::wstring	GetAppDir();
	static std::wstring GetAppPath();
	static std::wstring GetAppPathDir();
	static std::wstring GetCurrentDir();
	static std::wstring GetAppdataDir();
	static std::wstring GetAppdataAppDir(const std::wstring& strApp);

	static std::wstring GetAppLBE();

	static bool			CreateFullDir(const std::wstring& strDir);
	static bool			DirectoryIsExists(const std::wstring& strDir);
	static bool			CreateFileWithName(const std::wstring& strFileName);

	static std::wstring GetTempFile();

	static bool			ExistsFile(const std::wstring& strFile);
	static bool			RemoveFile(const std::wstring& strFile);
	static bool			RemoveFile(const std::string& strFile);

	static bool			RenameFile(const std::string& oldFileName, const std::string& newFileName);

	static int			GetFileSize(const std::wstring& strFile);
	static int			GetFileSize(const std::string& strFile);
	//static std::string  GetFileMD5(const std::wstring& strFile);

	static bool			WriteFileData(const std::wstring& strFile, const void* pData, int nSize);
	static bool			WriteFileData(const std::wstring& strFile, const Buffer&buf );

	static bool			ReadFileData(const std::wstring& strFile,void* pData,int nSize);
	static bool			ReadFileToBuffer(const std::string& strFile, Buffer &vecBuf);
	static bool			ReadFileToBuffer(const std::wstring& strFile, Buffer &vecBuf);
	static bool			ReadFileToString(const std::wstring& strFile, std::string &str);
	static bool			ReadFileToString(const std::wstring& strFile, std::wstring &str);
	static std::string& trim(std::string &s);
};



#endif//__UTIL_H__