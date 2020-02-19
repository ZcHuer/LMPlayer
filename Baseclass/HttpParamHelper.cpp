#include "stdafx.h"
#include "StringHelper.h"
#include "..\..\baseclass\urlencode.h"

namespace base {
//     std::wstring  GeneralUrl(const std::wstring & strHost, std::map<std::wstring , std::wstring >& params) {
//         SigniturePhpParam(params);
// 
//         std::wstring  strParam = GeneralParamString(params);
//         return strHost + L"?" + strParam;
//     }
// 
//     std::wstring  GeneralLogUrl(const std::wstring & strHost, std::map<std::wstring , std::wstring >& params)
//     {
//         params[L"sign"] = L"pSvzv4*GERmDnj6N";
//         std::wstring  strParam = GeneralEncodedParamString(params);
//         return strHost + L"?" + strParam;
//     }

    std::wstring GeneralParamString(const std::map<std::wstring, std::wstring>& params) {
        std::wstring strParam;
        bool bFirst = true;
        {
            for (auto pair : params)
            {
                if (!bFirst) {
                    strParam += L"&";
                }
                bFirst = false;
                strParam += pair.first;
                strParam += L"=";
                strParam += pair.second;
            }
        }
        return strParam;
    }

    std::wstring GeneralEncodedParamString(const std::map<std::wstring, std::wstring>& params) {
        std::string tempUrl;
        bool bFirst = true;
        {
            for (auto pair : params)
            {
                if (!bFirst) {
                    tempUrl += "&";
                }
                bFirst = false;

                string strFirst = CURLEncode::EncodeURL(pair.first.c_str(), pair.first.length(), CP_UTF8);
                tempUrl += strFirst;
                tempUrl += "=";

                string strSecond = CURLEncode::EncodeURL(pair.second.c_str(), pair.second.length(), CP_UTF8);
                tempUrl += strSecond;
            }
        }

        CUTFString utfUrl(tempUrl.c_str());
        return utfUrl.UTF16();
    }

    void SigniturePhpParam(std::map<std::wstring, std::wstring>& params) {
        std::wstring strParam = GeneralParamString(params);
        strParam += WEBLOGSIGN_KEY;

        CUTFString str(strParam.c_str());
        std::string md5 = CMD5Checksum::GetMD5((LPBYTE)str.UTF8(), str.LengthUTF8());
        params[L"sign"] = CUTFString(md5.c_str()).UTF16();
    }

    void SignitureLogParam(std::map<std::wstring, std::wstring>& params) {
        params[L"sign"] = L"pSvzv4*GERmDnj6N";
    }
}

