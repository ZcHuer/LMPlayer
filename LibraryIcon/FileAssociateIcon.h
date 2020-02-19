#pragma once

class CFileAssociateIcon
{
public:
	CFileAssociateIcon(void);
	virtual ~CFileAssociateIcon(void);

public:
	static UINT    GetIconId(LPCWSTR lpExt);
	static HICON   GetIcon(LPCWSTR lpExt);
	static BOOL    IsDefaultAss(LPCWSTR lpExt); //�鿴ĳ��չ���Ƿ�ȱʡ������
	static LPCWSTR EnumExt(DWORD dwIndex);

};
