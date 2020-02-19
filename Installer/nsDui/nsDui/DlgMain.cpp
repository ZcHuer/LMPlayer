#include "DlgMain.h"


CDlgMain::CDlgMain()
{
	CDuiString strResourcePath = CPaintManagerUI::GetResourcePath();
}

CDlgMain::~CDlgMain()
{

}

//CDuiString CDlgMain::GetSkinFile() { 
//    TCHAR file[128] = { 0 };
//    LANGID id = GetUserDefaultUILanguage();
//    switch (id&0xffff){
//    case 0x0804://简体中文
//    case 0x0404://繁体中文台湾 0c04 香港澳门
//        break;
//    default:
//        id = 0x0409;//english
//        break;
//    }
//    _stprintf_s(file, sizeof(file) / sizeof(file[0]), _T("install%04X.xml"), id);
//    return file;
//}

void CDlgMain::Notify( TNotifyUI& msg )
{
    std::map<CDuiString, int >::iterator iter = m_controlCallbackMap.find( msg.pSender->GetName() );
    if( _tcsicmp( msg.sType, _T("click") ) == 0 ){
        if( iter != m_controlCallbackMap.end() )
            g_pluginParms->ExecuteCodeSegment( iter->second - 1, 0 );
    }
    else if( _tcsicmp( msg.sType, _T("textchanged") ) == 0 ){
        if( iter != m_controlCallbackMap.end() )
            g_pluginParms->ExecuteCodeSegment( iter->second - 1, 0 );
    } else {
        WindowImplBase::Notify(msg);
    }
}

LRESULT CDlgMain::HandleCustomMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	if (WM_NCLBUTTONDBLCLK == uMsg) {
		bHandled = TRUE;
		return S_OK;
	}
	bHandled = FALSE;
	return 0;
}


void CDlgMain::SetMyIcon(bool binstall)
{
	if (binstall)
	{
		SetIcon(IDI_ICON_INSTALL);
	}
	else
	{
		SetIcon(IDI_ICON_UNINTALL);
	}
}
void CDlgMain::InitWindow()
{
	SetIcon(IDI_ICON_INSTALL);
    CRichEditUI * pRichEdit = static_cast<CRichEditUI*>(m_PaintManager.FindControl(_T("editLicense")));
    if(pRichEdit) {
        FILE *fp = _wfopen(m_license.GetData(), _T("rb"));
        if (!fp)
            return;
        fseek(fp, 0, SEEK_END);
        long llen = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        unsigned char *lpBuf = (unsigned char*)malloc(llen+1);
        if (!lpBuf)
            goto err;
        lpBuf[llen] = 0;
        if(llen == fread(lpBuf, 1, llen, fp))
            pRichEdit->AppendText((TCHAR*)lpBuf);
    err:
        if (lpBuf)
            free(lpBuf);
        if (fp)
            fclose(fp);
    }

}








