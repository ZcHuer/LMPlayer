#ifndef __HTTP_PARAM_HELPER_H__
#define __HTTP_PARAM_HELPER_H__
#pragma once
#include <string>
namespace base {
//     std::wstring GeneralUrl(const std::wstring & strHost, std::map<std::wstring , std::wstring >& params);
//     std::wstring  GeneralLogUrl(const std::wstring & strHost, std::map<std::wstring , std::wstring >& params);

    std::wstring GeneralParamString(const std::map<std::wstring, std::wstring>& params);    // 根据参数列表生成对应的字符串
    std::wstring GeneralEncodedParamString(const std::map<std::wstring, std::wstring>& params); // 根据参数列表生成对应的urlencode过的字符串
    void SigniturePhpParam(std::map<std::wstring, std::wstring>& params);   // 对普通PHP接口参数进行签名
    void SignitureLogParam(std::map<std::wstring, std::wstring>& params);   // 对普通LOG接口参数进行签名
}

#endif
