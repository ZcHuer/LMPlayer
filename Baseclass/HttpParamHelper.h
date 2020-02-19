#ifndef __HTTP_PARAM_HELPER_H__
#define __HTTP_PARAM_HELPER_H__
#pragma once
#include <string>
namespace base {
//     std::wstring GeneralUrl(const std::wstring & strHost, std::map<std::wstring , std::wstring >& params);
//     std::wstring  GeneralLogUrl(const std::wstring & strHost, std::map<std::wstring , std::wstring >& params);

    std::wstring GeneralParamString(const std::map<std::wstring, std::wstring>& params);    // ���ݲ����б����ɶ�Ӧ���ַ���
    std::wstring GeneralEncodedParamString(const std::map<std::wstring, std::wstring>& params); // ���ݲ����б����ɶ�Ӧ��urlencode�����ַ���
    void SigniturePhpParam(std::map<std::wstring, std::wstring>& params);   // ����ͨPHP�ӿڲ�������ǩ��
    void SignitureLogParam(std::map<std::wstring, std::wstring>& params);   // ����ͨLOG�ӿڲ�������ǩ��
}

#endif
