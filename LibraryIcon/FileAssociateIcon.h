#pragma once

class CFileAssociateIcon
{
public:
	CFileAssociateIcon(void);
	virtual ~CFileAssociateIcon(void);

public:
	static UINT    GetIconId(LPCWSTR lpExt);
	static HICON   GetIcon(LPCWSTR lpExt);
	static BOOL    IsDefaultAss(LPCWSTR lpExt); //查看某扩展名是否缺省被关联
	static LPCWSTR EnumExt(DWORD dwIndex);

};
