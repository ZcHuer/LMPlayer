#ifndef __DLG_MAIN_H__
#define __DLG_MAIN_H__

#include "UIlib.h"
//
#include "plugin-common.h"
#include "resource.h"


#include "Utils\Utils.h"
#include <atltypes.h>


using namespace DuiLib;

extern extra_parameters* g_pluginParms;

class CDlgMain: public WindowImplBase
{
public:
    CDlgMain();
    ~CDlgMain();
	void SetMyIcon(bool binstall);
    void SetLicenseFilePath(LPCTSTR file){ m_license = file; }
    void SetSkinFilePath(LPCTSTR file){ m_skinfile = file; }
protected:
    virtual LPCTSTR GetWindowClassName() const {return _T("CDlgMain");};
    virtual CDuiString GetSkinFolder() { return _T(""); }
    virtual CDuiString GetSkinFile(){ return m_skinfile; }
    virtual UILIB_RESOURCETYPE GetResourceType() const { return UILIB_FILE; }
    virtual LPCTSTR GetResourceID() const { return MAKEINTRESOURCE(IDR_ZIPRES_SKIN); }

    virtual void Notify(TNotifyUI& msg);
    virtual void InitWindow();
	
    virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
    CDuiString m_license;
    CDuiString m_skinfile;
public:
	CPaintManagerUI m_pm;//与控件绘制和消息处理关联起来
	
};

#endif