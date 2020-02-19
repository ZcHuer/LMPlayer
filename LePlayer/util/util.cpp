#include "util.h"
#include <windows.h>
#include <string.h>
#include <tchar.h>
#include <vector>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <Shlwapi.h>
#include <Shlobj.h>
//#include <openssl/md5.h>
//#include <openssl/aes.h>
//#include <openssl/rsa.h>
//#include <openssl/pem.h>
//#include <openssl/err.h>
//
//
//
//#include <curlpp/curl_easy.h>
//#include <curlpp/curl_exception.h>
//#include <curlpp/curl_ios.h>
//#include <curlpp/curl_sender.h>
//#include <curlpp/curl_receiver.h>
//#include <curlpp/curl_form.h>



//#pragma comment(lib,"libeay32.lib")
//#pragma comment(lib,"ssleay32.lib")
//
//#pragma warning (push)
//#pragma warning(disable:4996)


std::string CUtil::UnicodeToUTF8(const std::wstring& str) {
	int len = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, NULL, 0, NULL, NULL);
	std::vector<char> utf8(len);
	WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, &utf8[0], len, NULL, NULL);
	return std::string(&utf8[0]);
}

std::wstring CUtil::UTF8ToUnicode(const std::string& str) {
	int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
	std::vector<wchar_t> unicode(len);
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &unicode[0], len);
	return std::wstring(&unicode[0]);
}

std::string CUtil::AnsiToUTF8(const std::string& str) {
	return UnicodeToUTF8(AnsiToUnicode(str));
}

std::string CUtil::UTF8ToAnsi(const std::string& str) {
	return UnicodeToAnsi(UTF8ToUnicode(str));
}

std::wstring CUtil::AnsiToUnicode(const std::string& str) {
	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
	std::vector<wchar_t> unicode(len);
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, &unicode[0], len);
	return std::wstring(&unicode[0]);
}

std::string CUtil::UnicodeToAnsi(const std::wstring& str) {
	int len = WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, NULL, 0, NULL, NULL);
	std::vector<char> ansi(len);
	WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, &ansi[0], len, NULL, NULL);

	return std::string(&ansi[0]);
}

int CUtil::StrlenUTF8(const std::string& str) {
	return wcslen(UTF8ToUnicode(str).c_str());
}

std::string CUtil::TimestampToString(int64_t ts) {
	char now[64] = { 0 };
	time_t tt = ts;
	struct tm * ttime = localtime(&tt);
	strftime(now, 64, "%Y-%m-%d %H:%M:%S", ttime);
	std::string ret = now;
	return ret;
}

std::string  CUtil::TimestampToString(int64_t ts, const std::string& strFormat) {
	char now[64] = { 0 };
	time_t tt = ts;
	struct tm * ttime = localtime(&tt);
	strftime(now, 64, strFormat.c_str(), ttime);
	std::string ret = now;
	return ret;
}

std::string  CUtil::SystemTimestampToString() {
	SYSTEMTIME st;
	GetLocalTime(&st);
	wchar_t szFileName[128] = { 0 };
	wsprintf(szFileName, L"%04d%02d%02d%02d%02d%02d%04d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

	return UnicodeToAnsi(szFileName);
}

std::wstring CUtil::PrettySize(uint64_t n) {

	const double k = 1024;
	const double m = k*k;
	const double g = m*m;
	const double t = g*g;

	std::wstringstream ss;
	ss << std::fixed << std::setprecision(2);
	if (n >= t) {
		ss << double(n) / double(t) << L"TB";
	}else if (n >= g) {
		ss << double(n) / double(g) << L"GB";
	}else if (n >= m) {
		ss << double(n) / double(m) << L"MB";
	}else if (n >= k) {
		ss << double(n) / double(k) << L"KB";
	}else{
		ss << n << L"B";
	}

	return ss.str();
}

std::string& CUtil::trim(std::string &s)
{
	if (s.empty())
	{
		return s;
	}

	s.erase(0, s.find_first_not_of(" "));
	s.erase(s.find_last_not_of(" ") + 1);
	return s;
}

//std::string CUtil::CalcMD5(const std::string& str) {
//	return CalcMD5((void*)str.c_str(), str.size());
//}
//
//std::string CUtil::CalcMD5(const std::wstring& str) {
//	return CalcMD5((void*)str.c_str(), str.size()*2);
//}
//
//std::string CUtil::CalcMD5(const void* pBuf, int nSize) {
//	MD5_CTX c;
//	unsigned char md5[17] = { 0 };
//	MD5_Init(&c);
//	MD5_Update(&c, pBuf, nSize);
//	MD5_Final(md5, &c);
//
//
//	std::stringstream ss;
//	for (int i = 0; i < 16; i++)
//		ss << std::hex << std::setw(2) << std::setfill('0') << (int)md5[i];
//	
//	return ss.str();
//}
//
//std::string CUtil::CalcMD5(const Buffer& buf) {
//	return CalcMD5(buf.data(), buf.size());
//}
//
//
//std::string CUtil::CalcFileMD5(const std::string& strFile) {
//	return CalcFileMD5(CUtil::AnsiToUnicode(strFile));
//}
//
//std::string CUtil::CalcFileMD5(const std::wstring& strFile) {
//	std::string strMD5;
//	Buffer buf;
//	if (!ReadFileToBuffer(strFile, buf))
//		return strMD5;
//
//	return CalcMD5(buf);
//
//}
//
//void CUtil::SetBuffer(Buffer& buf, void* pData, int nSize) {
//	if (nSize <= 0) {
//		buf.resize(0);
//	} else {
//		buf.resize(nSize);
//		memcpy(buf.data(), pData, nSize);
//	}
//}
//
//
//void CUtil::EncryptAES(const std::string& strkey, void* pData, int nSize) {
//	AES_KEY aeskey;
//	AES_set_encrypt_key((const unsigned char*)strkey.c_str(), 256, &aeskey);
//	AES_encrypt((const unsigned char*)pData, (unsigned char*)pData, &aeskey);
//}
//
//void CUtil::DecryptAES(const std::string& strkey, void* pData, int nSize) {
//	AES_KEY aeskey;
//	AES_set_decrypt_key((const unsigned char*)strkey.c_str(), 256, &aeskey);
//	AES_decrypt((const unsigned char*)pData, (unsigned char*)pData, &aeskey);
//}
//
//
//bool CUtil::EncryptAESFull(const std::string& strkey, void* pData, int nSize) {
//
//	AES_KEY aeskey;
//	if (AES_set_encrypt_key((const unsigned char*)strkey.c_str(), 256, &aeskey) < 0)
//		return false;
//
//	Buffer in, out;
//	SetBuffer(in, pData, nSize);
//	SetBuffer(out, pData, nSize);
//
//	int len = in.size(), en_len = 0;
//	char* pIN = (char*)in.data();
//	char* pOUT = (char*)out.data();
//	while (en_len < len)//输入输出字符串够长，并且是AES_BLOCK_SIZE的整数倍，需要严格限制  
//	{
//		AES_encrypt((unsigned char*)pIN, (unsigned char*)pOUT, &aeskey);
//		pIN += AES_BLOCK_SIZE;
//		pOUT += AES_BLOCK_SIZE;
//		en_len += AES_BLOCK_SIZE;
//	}
//
//	if (en_len == 0)
//		return false;
//
//	memcpy(pData, out.data(), nSize);
//
//	return true;
//
//}
//
//bool CUtil::DecryptAESFull(const std::string& strkey, void* pData, int nSize) {
//	AES_KEY aeskey;
//	if (AES_set_decrypt_key((const unsigned char*)strkey.c_str(), 256, &aeskey) < 0)
//		return false;
//
//	Buffer in, out;
//	SetBuffer(in, pData, nSize);
//	SetBuffer(out, pData, nSize);
//
//	out.assign(in.begin(), in.end());
//
//	int len = in.size(), de_len = 0;
//	char* pIN = (char*)in.data();
//	char* pOUT = (char*)out.data();
//	while (de_len < len)//输入输出字符串够长，并且是AES_BLOCK_SIZE的整数倍，需要严格限制  
//	{
//		AES_decrypt((unsigned char*)pIN, (unsigned char*)pOUT, &aeskey);
//		pIN += AES_BLOCK_SIZE;
//		pOUT += AES_BLOCK_SIZE;
//		de_len += AES_BLOCK_SIZE;
//	}
//
//	if (de_len == 0)
//		return false;
//
//	memcpy(pData, out.data(), nSize);
//
//	return true;
//}
//
//
//bool CUtil::HttpRequestGet(const std::string& strURL,std::string& strRet) {
//
//	// Let's declare a stream
//	std::ostringstream str;
//	// Create a curl_ios object, passing the stream object.
//	curl::curl_ios<std::ostringstream> writer(str);
//
//	// Pass the writer to the easy constructor and watch the content returned in that variable!
//	curl::curl_easy easy(writer);
//	// Add some option to the easy handle
//	easy.add<CURLOPT_URL>(strURL.c_str());
//	easy.add<CURLOPT_FOLLOWLOCATION>(1L);
//	try {
//		// Execute the request.
//		easy.perform();
//	}
//	catch (curl::curl_easy_exception error) {
//		// If you want to get the entire error stack we can do:
//		curl::curlcpp_traceback errors = error.get_traceback();
//		// Otherwise we could print the stack like this:
//		error.print_traceback();
//		// Note that the printing the stack will erase it
//
//		return false;
//	}
//
//
//	strRet = str.str();
//	return true;
//}
//
//bool CUtil::HttpRequestGetAsBuffer(const std::string& strURL, Buffer& bufRet) {
//
//	std::string strRet;
//	if (!HttpRequestGet(strURL, strRet))
//		return false;
//
//	SetBuffer(bufRet, (void*)strRet.c_str(), strRet.size());
//	return true;
//}
//
//
//bool CUtil::HttpRequestPost(const std::string& strURL, const std::string& strFilename, std::map<std::string, std::string>* pMapFields, std::string& strRet)
//{
//
//	try {
//
//		struct curl_httppost *post = NULL;
//		struct curl_httppost *last = NULL;
//
//		curl::curl_form form;
//		if (pMapFields) {
//			std::map<std::string, std::string>::const_iterator it = pMapFields->begin();
//			while (it != pMapFields->end()) {
//				curl_formadd(&post, &last, CURLFORM_COPYNAME, it->first.c_str(), CURLFORM_COPYCONTENTS, it->second.c_str(), CURLFORM_END);
//				++it;
//			}
//		}
//
//
//		Buffer buf;
//		
//		//添加文件
//		if (strFilename.length() > 0) {
//			
//			if (!CUtil::ReadFileToBuffer(strFilename, buf))
//				return false;
//
//			curl_formadd(&post, &last,
//				CURLFORM_COPYNAME, "uploadfile",
//				CURLFORM_BUFFER, "uploadfile",
//				CURLFORM_BUFFERPTR, buf.data(),
//				CURLFORM_BUFFERLENGTH, buf.size(),
//				CURLFORM_END);
//		}
//
//
//		// Let's declare a stream
//		std::ostringstream str;
//		// Create a curl_ios object, passing the stream object.
//		curl::curl_ios<std::ostringstream> writer(str);
//
//		// Pass the writer to the easy constructor and watch the content returned in that variable!
//		curl::curl_easy easy(writer);
//
//		// Add some option to the easy handle
//		easy.add<CURLOPT_URL>(strURL.c_str());
//		//easy.add<CURLOPT_FOLLOWLOCATION>(1L);
//		curl_easy_setopt(easy.get_curl(), CURLOPT_HEADER, 0);
//		curl_easy_setopt(easy.get_curl(), CURLOPT_HTTPPOST, post);
//		curl_easy_setopt(easy.get_curl(), CURLOPT_TIMEOUT, 15);
//
//		// Execute the request.
//		easy.perform();
//
//		strRet = str.str();
//		return true;
//	}
//	catch (curl::curl_easy_exception error) {
//		// If you want to get the entire error stack we can do:
//		curl::curlcpp_traceback errors = error.get_traceback();
//		// Otherwise we could print the stack like this:
//		error.print_traceback();
//		// Note that the printing the stack will erase it
//
//		return false;
//	}
//	catch (curl::curl_exception &e) {
//		curl::curlcpp_traceback errors = e.get_traceback();
//		e.print_traceback();
//
//		return false;
//	}
//
//
//	
//}
//
//
//bool CUtil::HttpRequestPostFormFile(const std::string& strURL, const std::string& strKFileName, const std::string& strVFilename, std::map<std::string, std::string>* pMapFields, std::string& strRet)
//{
//
//	try {
//
//
//		struct curl_httppost *post = NULL;
//		struct curl_httppost *last = NULL;
//
//		curl::curl_form form;
//		if (pMapFields) {
//			std::map<std::string, std::string>::const_iterator it = pMapFields->begin();
//			while (it != pMapFields->end()) {
//				curl_formadd(&post, &last, CURLFORM_COPYNAME, it->first.c_str(), CURLFORM_COPYCONTENTS, it->second.c_str(), CURLFORM_END);
//				++it;
//			}
//		}
//
//
//		
//		Buffer buf;
//
//		//添加文件
//		if (strKFileName.length() > 0) {
//
//			if (!CUtil::ReadFileToBuffer(strVFilename, buf))
//				return false;
//
//			curl_formadd(&post, &last,
//				CURLFORM_COPYNAME, strKFileName.c_str(),
//				CURLFORM_BUFFER, strKFileName.c_str(),
//				CURLFORM_BUFFERPTR, buf.data(),
//				CURLFORM_BUFFERLENGTH, buf.size(),
//				CURLFORM_END);
//		}
//
//
//		// Let's declare a stream
//		std::ostringstream str;
//		// Create a curl_ios object, passing the stream object.
//		curl::curl_ios<std::ostringstream> writer(str);
//
//		// Pass the writer to the easy constructor and watch the content returned in that variable!
//		curl::curl_easy easy(writer);
//
//		// Add some option to the easy handle
//		easy.add<CURLOPT_URL>(strURL.c_str());
//		//easy.add<CURLOPT_FOLLOWLOCATION>(1L);
//		curl_easy_setopt(easy.get_curl(), CURLOPT_HEADER, 0);
//		curl_easy_setopt(easy.get_curl(), CURLOPT_HTTPPOST, post);
//		curl_easy_setopt(easy.get_curl(), CURLOPT_TIMEOUT, 15);
//
//		// Execute the request.
//		easy.perform();
//
//		strRet = str.str();
//		return true;
//	}
//	catch (curl::curl_easy_exception error) {
//		// If you want to get the entire error stack we can do:
//		curl::curlcpp_traceback errors = error.get_traceback();
//		// Otherwise we could print the stack like this:
//		error.print_traceback();
//		// Note that the printing the stack will erase it
//
//		return false;
//	}
//	catch (curl::curl_exception &e) {
//		curl::curlcpp_traceback errors = e.get_traceback();
//		e.print_traceback();
//
//		return false;
//	}
//
//
//
//}
//
//
//bool CUtil::HttpRequestPost(const std::string& strURL, std::map<std::string, std::string>* pMapFields, std::string& strRet)
//{
//
//	try {
//		curl::curl_form form;
//		if (pMapFields) {
//			std::map<std::string, std::string>::const_iterator it = pMapFields->begin();
//			while (it != pMapFields->end()) {
//
//				curl::curl_pair<CURLformoption, std::string> k(CURLFORM_COPYNAME, it->first);
//				curl::curl_pair<CURLformoption, std::string> v(CURLFORM_COPYCONTENTS, it->second);
//				form.add(k, v);
//				++it;
//			}
//		}
//
//
//		// Let's declare a stream
//		std::ostringstream str;
//		// Create a curl_ios object, passing the stream object.
//		curl::curl_ios<std::ostringstream> writer(str);
//
//		// Pass the writer to the easy constructor and watch the content returned in that variable!
//		curl::curl_easy easy(writer);
//
//		// Add some option to the easy handle
//		easy.add<CURLOPT_URL>(strURL.c_str());
//		easy.add<CURLOPT_HTTPPOST>(form.get());
//
//		// Execute the request.
//		easy.perform();
//
//		strRet = str.str();
//		return true;
//	}
//	catch (curl::curl_easy_exception error) {
//		// If you want to get the entire error stack we can do:
//		curl::curlcpp_traceback errors = error.get_traceback();
//		// Otherwise we could print the stack like this:
//		error.print_traceback();
//		// Note that the printing the stack will erase it
//
//		return false;
//	}
//	catch (curl::curl_exception &e) {
//		curl::curlcpp_traceback errors = e.get_traceback();
//		e.print_traceback();
//
//		return false;
//	}
//
//}

typedef unsigned char BYTE;

//BYTE CUtil::toHex(const BYTE &x)
//{
//	return x > 9 ? x - 10 + 'A' : x + '0';
//}
//
//BYTE CUtil::fromHex(const BYTE &x)
//{
//	return isdigit(x) ? x - '0' : x - 'A' + 10;
//}
//
//std::string CUtil::URLEncode(const std::string &sIn)
//{
//	std::string sOut;
//	for (size_t ix = 0; ix < sIn.size(); ix++)
//	{
//		BYTE buf[4];
//		memset(buf, 0, 4);
//		if (isalnum((BYTE)sIn[ix]))
//		{
//			buf[0] = sIn[ix];
//		}
//		//else if ( isspace( (BYTE)sIn[ix] ) ) //貌似把空格编码成%20或者+都可以  
//		//{  
//		//    buf[0] = '+';  
//		//}  
//		else
//		{
//			buf[0] = '%';
//			buf[1] = toHex((BYTE)sIn[ix] >> 4);
//			buf[2] = toHex((BYTE)sIn[ix] % 16);
//		}
//		sOut += (char *)buf;
//	}
//	return sOut;
//};
//
//std::string CUtil::URLDecode(const std::string &sIn)
//{
//	std::string sOut;
//	for (size_t ix = 0; ix < sIn.size(); ix++)
//	{
//		BYTE ch = 0;
//		if (sIn[ix] == '%')
//		{
//			ch = (fromHex(sIn[ix + 1]) << 4);
//			ch |= fromHex(sIn[ix + 2]);
//			ix += 2;
//		}
//		else if (sIn[ix] == '+')
//		{
//			ch = ' ';
//		}
//		else
//		{
//			ch = sIn[ix];
//		}
//		sOut += (char)ch;
//	}
//	return sOut;
//}


bool CUtil::CompareVersion(const std::wstring& strVer1, const std::wstring& strVer2,int &nResult) {
	// 参数安全性检查
	if (strVer1.length() == 0 || strVer2.length()==0)	
		return false;

	// 参数有效性检查
	for (size_t i = 0; i < strVer1.length(); i++)
	{
		// 如果指定版本号中的当前字符在0到9范围以外，并且不是分割点，就认为是无效的
		if ((strVer1[i] < L'0' || strVer1[i] > L'9') && strVer1[i] != L'.')
			return false;
	}
	for (size_t i = 0; i < strVer2.length(); i++)
	{
		// 如果指定版本号中的当前字符在0到9范围以外，并且不是分割点，就认为是无效的
		if ((strVer2[i] < L'0' || strVer2[i] > L'9') && strVer2[i] != L'.')
			return false;
	}

	// _tcstok_s函数会修改输入，所以先把版本字符串复制一份
	// 根据版本号的通常表示法，不必根据源版本字符串的大小从堆上申请空间，直接定义即可满足绝大多数需求，也能提高效率
	const short nMAX_VER_LEN = 64;
	wchar_t szVer1[nMAX_VER_LEN] = { 0 }, szVer2[nMAX_VER_LEN] = { 0 };
	wcsncpy(szVer1, strVer1.c_str(), nMAX_VER_LEN - 1);
	wcsncpy(szVer2, strVer2.c_str(), nMAX_VER_LEN - 1);

	const wchar_t szSeps[] = L".";
	wchar_t* lpszToken1 = NULL, *lpszToken2 = NULL;
	wchar_t* lpszNextToken1 = NULL, *lpszNextToken2 = NULL;

	lpszToken1 = wcstok_s(szVer1, szSeps, &lpszNextToken1);
	lpszToken2 = wcstok_s(szVer2, szSeps, &lpszNextToken2);

	// 进行逐级拆分
	while ((lpszToken1 != NULL) || (lpszToken2 != NULL))
	{
		int nNum1 = 0, nNum2 = 0;
		// 检查拆分结果
		if (lpszToken1 != NULL)
		{
			nNum1 = _wtoi(lpszToken1);
			lpszToken1 = _tcstok_s(NULL, szSeps, &lpszNextToken1);
		}
		if (lpszToken2 != NULL)
		{
			nNum2 = _wtoi(lpszToken2);
			lpszToken2 = _tcstok_s(NULL, szSeps, &lpszNextToken2);
		}

		// 执行比较
		if (nNum1 > nNum2)
		{
			nResult = 1;
			break;
		}
		else if (nNum1 < nNum2)
		{
			nResult = -1;
			break;
		}
		else
		{
			nResult = 0;
		}
	}

	return true;
}


std::wstring CUtil::GetAppDir() {

	static std::wstring strAppDir;
	if (strAppDir.length() > 0)
		return strAppDir;

#ifdef _DEBUG
	TCHAR appDir[MAX_PATH + 1] = { 0 };
	GetCurrentDirectory(MAX_PATH, appDir);
	strAppDir = appDir;
#else
	TCHAR appFullPath[MAX_PATH + 1] = { 0 };
	GetModuleFileName(NULL, appFullPath, MAX_PATH);
	::PathRemoveFileSpec(appFullPath);
	strAppDir = appFullPath;
	

#endif//

	return strAppDir;
}


std::wstring CUtil::GetAppPath() {
	TCHAR appFullPath[MAX_PATH + 1] = { 0 };
	GetModuleFileName(NULL, appFullPath, MAX_PATH);
	std::wstring strRet = appFullPath;
	return strRet;
}

std::wstring CUtil::GetAppPathDir() {
	static std::wstring strAppPathDir;
	if (strAppPathDir.length() > 0)
		return strAppPathDir;

	TCHAR appFullPath[MAX_PATH + 1] = { 0 };
	GetModuleFileName(NULL, appFullPath, MAX_PATH);
	::PathRemoveFileSpec(appFullPath);
	strAppPathDir = appFullPath;

	return strAppPathDir;
}


std::wstring CUtil::GetCurrentDir() {

	static std::wstring strCurrentDir;
	if (strCurrentDir.length() > 0)
		return strCurrentDir;

	TCHAR appDir[MAX_PATH + 1] = { 0 };
	GetCurrentDirectory(MAX_PATH, appDir);
	strCurrentDir = appDir;

	return strCurrentDir;
}

std::wstring CUtil::GetAppdataDir() {

	static std::wstring strAppdataDir;
	if (strAppdataDir.length() > 0)
		return strAppdataDir;

	wchar_t m_lpszDefaultDir[MAX_PATH];
	wchar_t szDocument[MAX_PATH] = { 0 };
	memset(m_lpszDefaultDir, 0, _MAX_PATH);

	LPITEMIDLIST pidl = NULL;
	SHGetSpecialFolderLocation(NULL, CSIDL_APPDATA, &pidl);
	if (pidl && SHGetPathFromIDList(pidl, szDocument)){
		GetShortPathName(szDocument, m_lpszDefaultDir, _MAX_PATH);
	}

	strAppdataDir = m_lpszDefaultDir;

	return strAppdataDir;
}

std::wstring CUtil::GetAppdataAppDir(const std::wstring& strApp) {
	std::wstring path = GetAppdataDir();
	path.append(L"\\");
	path.append(strApp);

	if (_waccess(path.c_str(), 0) == -1){
		_wmkdir(path.c_str());
	}

	return path;
}


std::wstring CUtil::GetAppLBE() {

	std::wstring strDir = L"LBE";
	if (ExistsFile(L"./inhouse")) {
		strDir += L"_inhouse";
	}

	return strDir;

}

bool CUtil::CreateFullDir(const std::wstring& strDir) {
	if (ERROR_SUCCESS == SHCreateDirectory(NULL, strDir.c_str()))
		return true;
	else
		return false;
}

bool CUtil::DirectoryIsExists(const std::wstring& strDir) {
	DWORD dwAttributes = GetFileAttributes(strDir.c_str());
	if (dwAttributes == 0xFFFFFFFF)
	{
		return false;
	}

	if ((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
	{
		return true;
	}
	return false;
}

bool CUtil::CreateFileWithName(const std::wstring& strFileName) {

	if (strFileName.length() == 0)
		return false;

	FILE *fp = fopen(UnicodeToAnsi(strFileName).c_str(), "wb+");
	if (!fp)
		return false;
	fclose(fp);
	return true;
}

std::wstring CUtil::GetTempFile() {

	wchar_t tfp[MAX_PATH+1] = { 0 };
	GetTempPath(256, tfp);
	wchar_t szTempfile[MAX_PATH + 1];

	GetTempFileName(tfp, L"LojoyBook_", 0, szTempfile);
	std::wstring strFileName = szTempfile;
	return strFileName;
}


bool CUtil::ExistsFile(const std::wstring& strFile) {
	if (::PathFileExists(strFile.c_str()))
		return true;
	else
		return false;
}


bool CUtil::RemoveFile(const std::wstring& strFile) {
	if (::DeleteFile(strFile.c_str()))
		return true;
	
	std::wstring strTempFile= GetTempFile();
	if (::MoveFile(strFile.c_str(), strTempFile.c_str()))
		return true;

	if (::MoveFileEx(strFile.c_str(), strTempFile.c_str(), MOVEFILE_COPY_ALLOWED))
		return true;

	return false;

}


bool CUtil::RemoveFile(const std::string& strFile) {
	return RemoveFile(CUtil::AnsiToUnicode(strFile));
}

bool CUtil::RenameFile(const std::string& oldFileName, const std::string& newFileName) {

	if (::MoveFile((CUtil::AnsiToUnicode(oldFileName)).c_str(), (CUtil::AnsiToUnicode(newFileName)).c_str()))
		return true;
	return false;
}


int CUtil::GetFileSize(const std::wstring& strFile) {
	FILE *fp = fopen(UnicodeToAnsi(strFile).c_str(), "rb");
	if (!fp)
		return 0;

	fseek(fp, 0, SEEK_END);
	int nSize = (int)ftell(fp);
	fclose(fp);

	return nSize;
}

int CUtil::GetFileSize(const std::string& strFile) {
	return GetFileSize(UTF8ToUnicode(strFile));
}

//std::string CUtil::GetFileMD5(const std::wstring& strFile) {
//
//	std::string strMD5;
//
//	Buffer vecBuffer;
//	if (!ReadFileToBuffer(strFile, vecBuffer)) {
//		return strMD5;
//	}
//
//	strMD5 = CalcMD5((void*)&vecBuffer[0], (int)vecBuffer.size());
//	return strMD5;
//}

bool CUtil::WriteFileData(const std::wstring& strFile, const void* pData, int nSize) {
	FILE *fp = fopen(UnicodeToAnsi(strFile).c_str(), "wb+");
	if (!fp)
		return false;

	int nResult = (int)fwrite(pData, 1, nSize, fp);
	fclose(fp);
	if (nResult == nSize)
		return true;

	return false;
}

bool CUtil::WriteFileData(const std::wstring& strFile, const Buffer &buf) {
	return WriteFileData(strFile, buf.data(), buf.size());
}

bool CUtil::ReadFileData(const std::wstring& strFile, void* pData, int nSize) {
	FILE *fp = fopen(UnicodeToAnsi(strFile).c_str(), "rb");
	if (!fp)
		return false;

	int nResult = (int)fread(pData, 1, nSize, fp);
	fclose(fp);
	if (nResult == nSize)
		return true;

	return false;
}

bool CUtil::ReadFileToBuffer(const std::string& strFile, Buffer &vecBuf) {
	return ReadFileToBuffer(AnsiToUnicode(strFile), vecBuf);
}

bool CUtil::ReadFileToBuffer(const std::wstring& strFile, Buffer &vecBuf) {
	FILE *fp = fopen(UnicodeToAnsi(strFile).c_str(), "rb");
	if (!fp)
		return false;

	fseek(fp, 0, SEEK_END);
	int nSize = (int)ftell(fp);
	fseek(fp, 0, SEEK_SET);

	vecBuf.reserve(nSize);
	vecBuf.resize(nSize);


	if (nSize == 0 ) {
		fclose(fp);
		return true;
	}
	
	void* pData = (void*)&vecBuf[0];
	int nResult = (int)fread(pData, 1, nSize, fp);
	fclose(fp);
	if (nResult == nSize)
		return true;

	return false;
}

bool CUtil::ReadFileToString(const std::wstring& strFile, std::string &str) {
	Buffer buf;
	if (!ReadFileToBuffer(strFile, buf))
		return false;

	char* _str = buf.data();
	str.assign(_str, buf.size());
	return true;
}

bool CUtil::ReadFileToString(const std::wstring& strFile, std::wstring &str) {
	Buffer buf;
	if (!ReadFileToBuffer(strFile, buf))
		return false;

	char* _str = (char*)buf.data();
	_str[buf.size()] = 0;
	str = (wchar_t*)_str;

	int len = buf.size() / 2 + (buf.size() % 2 == 0?0:1);
	str.assign((wchar_t*)_str, len);

	return true;
}

#pragma warning (pop)