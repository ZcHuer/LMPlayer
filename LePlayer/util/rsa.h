//#ifndef __UTIL_RSA_H__
//#define __UTIL_RSA_H__
//
//#include "buffer.h"
//#include <string>
//#include <openssl/rsa.h>
//
//class CRsa 
//{
//public:
//
//	CRsa();
//	~CRsa();
//
//	void Clear();
//
//public:
//
//	Buffer& GetOutput();
//
//	char*   GetOutputData();
//	size_t  GetOutputLength();
//
//public:
//
//	bool SetPublicKey(const std::string& strKey);
//	bool SetPrivateKey(const std::string& strKey);
//
//	bool Encrypt(const void* data, size_t len);
//	bool Decrypt(const void* data, size_t len);
//
//private:
//
//	Buffer m_Output;
//
//	BIO * m_bio;
//	RSA * m_rsa;
//};
//
//#endif//__UTIL_RSA_H__