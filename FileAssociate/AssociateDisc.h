#pragma once

class CAssociateDisc
{
public:
	CAssociateDisc(void);
	virtual ~CAssociateDisc(void);

public:
	static bool AssociateDVD(bool bAssociate);
	static bool AssociateDVDAutoplay(bool bAssociate);
	static bool AssociateUsbAutoplay(bool bAssociate);
};
