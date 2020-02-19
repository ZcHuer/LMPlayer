#include "WmiInfo.h"  

CWmiInfo::CWmiInfo(void)
{
    m_pWbemSvc = NULL;
    m_pWbemLoc = NULL;
    m_pEnumClsObj = NULL;
    m_pWbemClsObj = NULL;
}

CWmiInfo::~CWmiInfo(void)
{
    m_pWbemSvc = NULL;
    m_pWbemLoc = NULL;
    m_pEnumClsObj = NULL;
}

HRESULT CWmiInfo::InitWmi()
{
    HRESULT hr;

    //һ����ʼ��COM���  
    //��ʼ��COM  
    hr = ::CoInitializeEx(0, COINIT_MULTITHREADED);
    if (SUCCEEDED(hr) || RPC_E_CHANGED_MODE == hr)
    {
        //���ý��̵İ�ȫ���𣬣�����COM���ʱ�ڳ�ʼ��COM֮��Ҫ����CoInitializeSecurity���ý��̰�ȫ���𣬷���ᱻϵͳʶ��Ϊ������  
        hr = CoInitializeSecurity(NULL,
            -1,
            NULL,
            NULL,
            RPC_C_AUTHN_LEVEL_PKT,
            RPC_C_IMP_LEVEL_IMPERSONATE,
            NULL,
            EOAC_NONE,
            NULL);
        //VERIFY(SUCCEEDED(hr));  

        //��������һ��WMI�����ռ�����  
        //����һ��CLSID_WbemLocator����  
        hr = CoCreateInstance(CLSID_WbemLocator,
            0,
            CLSCTX_INPROC_SERVER,
            IID_IWbemLocator,
            (LPVOID*)&m_pWbemLoc);
        VERIFY(SUCCEEDED(hr));

        //ʹ��m_pWbemLoc���ӵ�"root\cimv2"������m_pWbemSvc��ָ��  
        hr = m_pWbemLoc->ConnectServer(CComBSTR(L"ROOT\\CIMV2"),
            NULL,
            NULL,
            0,
            NULL,
            0,
            0,
            &m_pWbemSvc);
        VERIFY(SUCCEEDED(hr));

        //��������WMI���ӵİ�ȫ��  
        hr = CoSetProxyBlanket(m_pWbemSvc,
            RPC_C_AUTHN_WINNT,
            RPC_C_AUTHZ_NONE,
            NULL,
            RPC_C_AUTHN_LEVEL_CALL,
            RPC_C_IMP_LEVEL_IMPERSONATE,
            NULL,
            EOAC_NONE);
        VERIFY(SUCCEEDED(hr));

    }
    return(hr);
}

HRESULT CWmiInfo::ReleaseWmi()
{
    HRESULT hr;

    if (NULL != m_pWbemSvc)
    {
        hr = m_pWbemSvc->Release();
    }
    if (NULL != m_pWbemLoc)
    {
        hr = m_pWbemLoc->Release();
    }
    if (NULL != m_pEnumClsObj)
    {
        hr = m_pEnumClsObj->Release();
    }

    ::CoUninitialize();

    return(hr);
}

BOOL CWmiInfo::GetSingleItemInfo(CString ClassName, CString ClassMember, CString &chRetValue)
{
    USES_CONVERSION;

    CComBSTR query("SELECT * FROM ");
    VARIANT vtProp;
    ULONG uReturn;
    HRESULT hr;
    BOOL bRet = FALSE;

    if (NULL != m_pWbemSvc)
    {
        //��ѯ��ClassName�е������ֶ�,���浽m_pEnumClsObj��  
        query += CComBSTR(ClassName);
        hr = m_pWbemSvc->ExecQuery(CComBSTR("WQL"), query, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            0, &m_pEnumClsObj);
        if (SUCCEEDED(hr))
        {
            //��ʼ��vtPropֵ  
            VariantInit(&vtProp);
            uReturn = 0;

            //���شӵ�ǰλ����ĵ�һ������m_pWbemClsObj��  
            hr = m_pEnumClsObj->Next(WBEM_INFINITE, 1, &m_pWbemClsObj, &uReturn);
            if (SUCCEEDED(hr) && uReturn>0)
            {
                //��m_pWbemClsObj���ҳ�ClassMember��ʶ�ĳ�Ա����ֵ,�����浽vtProp������  
                hr = m_pWbemClsObj->Get(CComBSTR(ClassMember), 0, &vtProp, 0, 0);
                if (SUCCEEDED(hr))
                {
                    VariantToString(&vtProp, chRetValue);
                    VariantClear(&vtProp);//���vtProp  
                    bRet = TRUE;
                }
            }
        }
    }
    if (NULL != m_pEnumClsObj)
    {
        hr = m_pEnumClsObj->Release();
        m_pEnumClsObj = NULL;
    }
    if (NULL != m_pWbemClsObj)
    {
        hr = m_pWbemClsObj->Release();
        m_pWbemClsObj = NULL;
    }
    return bRet;
}

BOOL CWmiInfo::GetGroupItemInfo(CString ClassName, CString ClassMember[], int n, CString &chRetValue)
{
    USES_CONVERSION;

    CComBSTR query("SELECT * FROM ");
    CString result, info;
    VARIANT vtProp;
    ULONG uReturn;
    HRESULT hr;
    int i;
    BOOL bRet = FALSE;
    if (NULL != m_pWbemSvc)
    {
        query += CComBSTR(ClassName);
        hr = m_pWbemSvc->ExecQuery(CComBSTR("WQL"), query, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, 0, &m_pEnumClsObj);
        if (SUCCEEDED(hr))
        {
            VariantInit(&vtProp); //��ʼ��vtProp����  
            if (m_pEnumClsObj)
            {
                Sleep(10);
                uReturn = 0;
                hr = m_pEnumClsObj->Next(WBEM_INFINITE, 1, &m_pWbemClsObj, &uReturn);
                if (SUCCEEDED(hr) && uReturn>0)
                {
                    for (i = 0; i<n; ++i)
                    {
                        hr = m_pWbemClsObj->Get(CComBSTR(ClassMember[i]), 0, &vtProp, 0, 0);
                        if (SUCCEEDED(hr))
                        {
                            VariantToString(&vtProp, info);
                            chRetValue += info + _T("\t");
                            VariantClear(&vtProp);
                            bRet = TRUE;
						}
						
                    }
                    chRetValue += _T("\r\n");
                }
            }
        }
    }

    if (NULL != m_pEnumClsObj)
    {
        hr = m_pEnumClsObj->Release();
        m_pEnumClsObj = NULL;
    }
    if (NULL != m_pWbemClsObj)
    {
        hr = m_pWbemClsObj->Release();
        m_pWbemClsObj = NULL;
    }
    return bRet;
}

BOOL CWmiInfo::GetAllInfo(CString ClassName)
{
    USES_CONVERSION;

    CComBSTR query("SELECT * FROM ");
    VARIANT vtProp;
    ULONG uReturn;
    HRESULT hr;
    BOOL bRet = FALSE;

    if (NULL != m_pWbemSvc)
    {
        //��ѯ��ClassName�е������ֶ�,���浽m_pEnumClsObj��  
        query += CComBSTR(ClassName);
        hr = m_pWbemSvc->ExecQuery(CComBSTR("WQL"), query, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            0, &m_pEnumClsObj);
        if (SUCCEEDED(hr))
        {
            //���شӵ�ǰλ����ĵ�һ������m_pWbemClsObj��  
            hr = m_pEnumClsObj->Next(WBEM_INFINITE, 1, &m_pWbemClsObj, &uReturn);
            if (SUCCEEDED(hr) && uReturn > 0)
            {
                SAFEARRAY *pvNames = NULL;
                if (m_pWbemClsObj->GetNames(NULL, WBEM_FLAG_ALWAYS |
                    WBEM_MASK_CONDITION_ORIGIN, NULL, &pvNames) == S_OK)
                {
                    long vbl, vbu;
                    SafeArrayGetLBound(pvNames, 1, &vbl);
                    SafeArrayGetUBound(pvNames, 1, &vbu);
                    for (long i = vbl; i <= vbu; i++)
                    {
                        long index = i;
                        WCHAR *wsName = NULL;
                        CComVariant varValue;
                        SafeArrayGetElement(pvNames, &index, &wsName);
                        CComBSTR bstrName(wsName);
                        if (m_pWbemClsObj->Get(bstrName, 0, &varValue, NULL, 0) == S_OK)
                        {
                            CString str;
                            if (varValue.vt & VT_BSTR)
                            {
                                str = varValue.bstrVal;
                            }
                            else if (varValue.vt & VT_ARRAY)
                            {
                                long iLowBound = 0, iUpBound = 0;
                                SafeArrayGetLBound(varValue.parray, 1, &iLowBound);
                                SafeArrayGetUBound(varValue.parray, 1, &iUpBound);
                                for (long j = iLowBound; j <= iUpBound; j++)
                                {
                                    VARIANT *pvar = NULL;
                                    long temp = j;
                                    if (SafeArrayGetElement(varValue.parray, &temp, pvar) == S_OK &&
                                        pvar)
                                    {
                                        CComVariant varTemp;
                                        if (varTemp.ChangeType(VT_BSTR, pvar) == S_OK)
                                        {
                                            if (!str.IsEmpty())
                                                str += _T(",");
                                            str += varTemp.bstrVal;
                                        }
                                    }
                                }
                            }
                            else
                            {
                                if (varValue.ChangeType(VT_BSTR) == S_OK)
                                    str = varValue.bstrVal;
                            }
                            // ��ӡ��Ӳ����Ϣ
                            TRACE("%s=%s/n", CString(bstrName), str);
                        }
                    }
                }
                if (pvNames){
                    SafeArrayDestroy(pvNames);
                }
            }
        }
    }

    if (NULL != m_pEnumClsObj)
    {
        hr = m_pEnumClsObj->Release();
        m_pEnumClsObj = NULL;
    }
    if (NULL != m_pWbemClsObj)
    {
        hr = m_pWbemClsObj->Release();
        m_pWbemClsObj = NULL;
    }


    return 0;
}

BOOL CWmiInfo::GetNetworkAdapter(CString &chRetValue)
{
    USES_CONVERSION;

    CComBSTR query("SELECT * FROM ");
    VARIANT vtProp;
    ULONG uReturn;
    HRESULT hr;
    BOOL bRet = FALSE;

    if (NULL != m_pWbemSvc)
    {
        //��ѯ��ClassName�е������ֶ�,���浽m_pEnumClsObj��  
        query += CComBSTR("Win32_NetworkAdapterConfiguration where IPEnabled=True");
        hr = m_pWbemSvc->ExecQuery(CComBSTR("WQL"), query, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            0, &m_pEnumClsObj);
        if (SUCCEEDED(hr))
        {
            //��ʼ��vtPropֵ  
            VariantInit(&vtProp);
            uReturn = 0;

            //���شӵ�ǰλ����ĵ�һ������m_pWbemClsObj��  
            hr = m_pEnumClsObj->Next(WBEM_INFINITE, 1, &m_pWbemClsObj, &uReturn);
            if (SUCCEEDED(hr) && uReturn>0)
            {
                //��m_pWbemClsObj���ҳ�ClassMember��ʶ�ĳ�Ա����ֵ,�����浽vtProp������  
                hr = m_pWbemClsObj->Get(CComBSTR("Description"), 0, &vtProp, 0, 0);
                if (SUCCEEDED(hr))
                {
                    VariantToString(&vtProp, chRetValue);
                    VariantClear(&vtProp);//���vtProp  
                    bRet = TRUE;
                }
            }
        }
    }
    if (NULL != m_pEnumClsObj)
    {
        hr = m_pEnumClsObj->Release();
        m_pEnumClsObj = NULL;
    }
    if (NULL != m_pWbemClsObj)
    {
        hr = m_pWbemClsObj->Release();
        m_pWbemClsObj = NULL;
    }
    return bRet;
}

BOOL CWmiInfo::IsWifi()
{
    CString sResult;
    GetNetworkAdapter(sResult);
    if (sResult.Find(L"Wireless") != -1)return TRUE;
    return FALSE;
}

void CWmiInfo::VariantToString(const LPVARIANT pVar, CString &chRetValue) const
{
    USES_CONVERSION;

    CComBSTR HUGEP* pBstr;
    BYTE HUGEP* pBuf;
    LONG low, high, i;
    HRESULT hr;

    switch (pVar->vt)
    {
    case VT_BSTR:
    {
        chRetValue = W2T(pVar->bstrVal);
    }
    break;
    case VT_BOOL:
    {
        if (VARIANT_TRUE == pVar->boolVal)
            chRetValue = "��";
        else
            chRetValue = "��";
    }
    break;
    case VT_I4:
    {
        chRetValue.Format(_T("%d"), pVar->lVal);
    }
    break;
    case VT_UI1:
    {
        chRetValue.Format(_T("%d"), pVar->bVal);
    }
    break;
    case VT_UI4:
    {
        chRetValue.Format(_T("%d"), pVar->ulVal);
    }
    break;

    case VT_BSTR | VT_ARRAY:
    {
        hr = SafeArrayAccessData(pVar->parray, (void HUGEP**)&pBstr);
        hr = SafeArrayUnaccessData(pVar->parray);
        chRetValue = W2T(pBstr->m_str);
    }
    break;

    case VT_I4 | VT_ARRAY:
    {
        SafeArrayGetLBound(pVar->parray, 1, &low);
        SafeArrayGetUBound(pVar->parray, 1, &high);

        hr = SafeArrayAccessData(pVar->parray, (void HUGEP**)&pBuf);
        hr = SafeArrayUnaccessData(pVar->parray);
        CString strTmp;
        high = min(high, MAX_PATH * 2 - 1);
        for (i = low; i <= high; ++i)
        {
            strTmp.Format(_T("%02X"), pBuf[i]);
            chRetValue += strTmp;
        }
    }
    break;
    default:
        break;
    }
}