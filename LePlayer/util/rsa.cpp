//#include "rsa.h"
//
//#include <openssl/err.h>
//#include <openssl/pem.h>
//
//extern "C"
//{
//#include <openssl/applink.c>
//}
//
//#include <iostream>
//
//#include <cstring>
//#include <cassert>
//
//
//#include "core/core.h"
//
//CRsa::CRsa() {
//
//	m_bio = NULL;
//	m_rsa = NULL;
//}
//
//CRsa::~CRsa() {
//	Clear();
//}
//
//void CRsa::Clear() {
//	if (m_rsa) {
//		RSA_free(m_rsa);
//		m_rsa = NULL;
//	}
//
//	if (m_bio) {
//		BIO_free(m_bio);
//		m_bio = NULL;
//	}
//}
//
//Buffer& CRsa::GetOutput() {
//	return m_Output;
//}
//
//char* CRsa::GetOutputData() {
//	return m_Output.data();
//}
//
//size_t CRsa::GetOutputLength() {
//	return m_Output.size();
//}
//
//
//
//bool CRsa::SetPublicKey(const std::string& strKey) {
//
//	Clear();
//
//	if (strKey.empty())
//		return false;
//
//	BIO *bio = BIO_new_mem_buf((void*)strKey.c_str(), -1);
//	if (!bio)
//		return false;
//
//	RSA *rsa = PEM_read_bio_RSA_PUBKEY(bio, NULL, NULL, NULL);
//	if (!rsa) {
//		BIO_free(bio);
//		return false;
//	}
//
//	m_bio = bio;
//	m_rsa = rsa;
//
//	return true;
//}
//
//bool CRsa::SetPrivateKey(const std::string& strKey) {
//	Clear();
//
//	if (strKey.empty())
//		return false;
//
//	BIO *bio = BIO_new_mem_buf((void*)strKey.c_str(), -1);
//	if (!bio)
//		return false;
//	RSA *rsa = PEM_read_bio_RSAPrivateKey(bio, NULL, NULL, NULL);
//	if (!rsa) {
//		BIO_free(bio);
//		return false;
//	}
//
//	m_bio = bio;
//	m_rsa = rsa;
//
//	return true;
//}
//
//
//bool CRsa::Encrypt(const void* data, size_t len) {
//
//	if (!m_rsa || len == 0 || !data)
//		return false;
//
//	int nLen = RSA_size(m_rsa);
//	char* pEncode = new char[nLen + 1];
//	int ret = RSA_public_encrypt(len, (const unsigned char*)data, (unsigned char*)pEncode, m_rsa, RSA_PKCS1_PADDING);
//	if (ret >= 0) {
//		SetBuffer(m_Output, pEncode, ret);
//	}
//	delete[] pEncode;
//
//	if (ret >= 0)
//		return true;
//	else
//		return false;
//}
//
//bool CRsa::Decrypt(const void* data, size_t len) {
//	if (!m_rsa || len == 0 || !data)
//		return false;
//
//	int nLen = RSA_size(m_rsa);
//	char* pDecode = new char[nLen + 1];
//
//	int ret = RSA_private_decrypt(len, (const unsigned char*)data, (unsigned char*)pDecode, m_rsa, RSA_PKCS1_PADDING);
//	if (ret >= 0)
//	{
//		SetBuffer(m_Output, (char*)pDecode, ret);
//	}
//	delete[] pDecode;
//
//	if (ret >= 0)
//		return true;
//	else
//		return false;
//}
//
//
