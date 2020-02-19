#include "stdafx.h"
#include "Aes_encryptor.h"
#include <base64.h>
#include <sstream>

CAes_encryptor::CAes_encryptor()
{
    memset(m_key, 0x00, CryptoPP::AES::DEFAULT_KEYLENGTH);
    memset(m_iv, 0x00, CryptoPP::AES::BLOCKSIZE);
}

CAes_encryptor::~CAes_encryptor()
{
}

void CAes_encryptor::InitKey(const char * key, const char * iv)
{
    for (int j = 0; j < CryptoPP::AES::DEFAULT_KEYLENGTH; ++j)
        m_key[j] = key[j];

    for (int i = 0; i < CryptoPP::AES::BLOCKSIZE; ++i)
        m_iv[i] = iv[i];
}

string CAes_encryptor::encrypt(string src, BOOL bHex)
{
    string des;
    CryptoPP::AES::Encryption aesEncryption(m_key, CryptoPP::AES::DEFAULT_KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Encryption cbcEncryption(aesEncryption, m_iv);
    CryptoPP::StreamTransformationFilter stfEncryptor(cbcEncryption, new CryptoPP::StringSink(des));
    stfEncryptor.Put(reinterpret_cast<const unsigned char*>(src.c_str()), src.length());
    stfEncryptor.MessageEnd();

    string out = des;
    if (bHex)
    {
        for (int i = 0; i < des.size(); i++)
        {
            char ch[3] = { 0 };
            sprintf_s(ch, "%02x", static_cast<BYTE>(des[i]));
            out += ch;
        }
    }
    else
    {
        /*
        CryptoPP::Base64Encoder encoder;
        encoder.Put(reinterpret_cast<const unsigned char*>(des.c_str()), des.length());
        encoder.MessageEnd();
        long size = encoder.MaxRetrievable();
        if (size)
        {
            out.resize(size);
            encoder.Get((byte*)&out[0], out.size());
        }
        */
    }
    return out;
}

string CAes_encryptor::decrypt(string src)
{
    string cipherText;
    string decryptedText;

    int i = 0;
    while (true)
    {
        char c;
        int x;
        stringstream ss;
        ss << hex << src.substr(i, 2).c_str();
        ss >> x;
        c = (char)x;
        cipherText += c;
        if (i >= src.length() - 2)break;
        i += 2;
    }

    CryptoPP::AES::Decryption aesDecryption(m_key, CryptoPP::AES::DEFAULT_KEYLENGTH);
    CryptoPP::CBC_Mode_ExternalCipher::Decryption cbcDecryption(aesDecryption, m_iv);
    CryptoPP::StreamTransformationFilter stfDecryptor(cbcDecryption, new CryptoPP::StringSink(decryptedText));
    stfDecryptor.Put(reinterpret_cast<const unsigned char*>(cipherText.c_str()), cipherText.size());

    stfDecryptor.MessageEnd();

    return decryptedText;
}
