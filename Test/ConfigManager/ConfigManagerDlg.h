
// ConfigManagerDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CConfigManagerDlg �Ի���
class CConfigManagerDlg : public CDialogEx
{
// ����
public:
	CConfigManagerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CONFIGMANAGER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
