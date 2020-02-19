#pragma once
#include <windows.h>
#include <string.h>
using namespace std;
#include <aes.h>
#include <filters.h>
#include <modes.h>
#include <string.h>


#define RSA_SIGNKEY "L36p1u4K02IaHG5_hs4vDjKSKyuikMlcTHmVoMql_5YzzbxVOc7JG29BXlFtYp6EA0TPt9oo4lUjU0s0VQx3EdUUokH6oFoOK-d2VnmM_ewfUYUhDItL8e8YRrEwWv4NBn1fxjNrZH_g03me-VmWi-9x9lZFGoEgNWRxjhl-t3E"

class CAes_encryptor
{
public:
    CAes_encryptor();
    ~CAes_encryptor();

    BYTE m_key[CryptoPP::AES::DEFAULT_KEYLENGTH];
    BYTE m_iv[CryptoPP::AES::BLOCKSIZE];
    void InitKey(const char* key, const char* iv);
    string encrypt(string src,BOOL bHex=false);
    string decrypt(string src);
};

