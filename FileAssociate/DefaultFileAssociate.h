#pragma once
class CDefaultFileAssociate
{
public:
	CDefaultFileAssociate(void);
	virtual ~CDefaultFileAssociate(void);

public:
	static LPCWSTR GetDefaultFileAssociate(LPCWSTR lpExt);		//注意：不能有.,正确是=avi

public:
	static LPCWSTR GetDefault_XP(LPCWSTR lpExt);
	static LPCWSTR GetDefault_Win7(LPCWSTR lpExt);

};

