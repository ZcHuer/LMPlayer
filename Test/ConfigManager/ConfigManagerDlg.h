
// ConfigManagerDlg.h : 头文件
//

#pragma once
#include "afxwin.h"


// CConfigManagerDlg 对话框
class CConfigManagerDlg : public CDialogEx
{
// 构造
public:
	CConfigManagerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CONFIGMANAGER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClicked_Save();
	afx_msg void OnBnClicked_Cancel();	
	BOOL m_bPreView;
	BOOL m_bCloseUpdate;
	BOOL m_bJsDebug;
	CString m_cstrVer_Cur;
	CString m_cstrVer_New;
	afx_msg void OnBnClickedRadioNomal();
	afx_msg void OnBnClickedRadioTest();
	int m_nSel;

	CString m_cstrCfg;
};
