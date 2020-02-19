#pragma once
#include <memory>

template<class T>
class CTrackableObjectOwner
{
public:
	CTrackableObjectOwner() { m_bVaild = true; };
	~CTrackableObjectOwner() {};

	void SetInValid() { m_bVaild = false; };
	bool IsValid(){ return m_bVaild; }
private:
	bool m_bVaild;
};

template<class T>
class CTrackableObject
{
public:
	CTrackableObject(T* pRaw, std::shared_ptr<CTrackableObjectOwner<T>> Owner)
	{
		m_pRaw = pRaw;
		m_owner = Owner;
	}
	~CTrackableObject() {};

	CTrackableObject(const CTrackableObject<T> &other)
	{
		m_pRaw = other.m_pRaw;
		m_owner = other.m_owner;
	}

	CTrackableObject<T> & operator=(const CTrackableObject<T> &other)
	{
		m_pRaw = other.m_pRaw;
		m_owner = other.m_owner;
		return *this;
	}

	operator bool()
	{
		return IsValid();
	}

	T *operator->()
	{
		return m_pRaw;
	}

	T* Get()
	{
		return m_pRaw;
	}

	bool IsValid()
	{
		if (m_owner)
			return (m_pRaw != NULL && m_owner->IsValid());
		else
			return (m_pRaw != NULL);
	}
private:
	T* m_pRaw;
	std::shared_ptr<CTrackableObjectOwner<T>> m_owner;
};

template<class T>
class CTrackableObjectFactory
{
public:
	CTrackableObjectFactory(T* pRaw)
	{
		m_pRaw = pRaw;
		m_owner.reset(new CTrackableObjectOwner<T>());
	}

	~CTrackableObjectFactory() 
	{ 
		if(m_owner) 
			m_owner->SetInValid(); 
	}

	CTrackableObject<T> GetTrackObject()
	{
		return CTrackableObject<T>(m_pRaw, m_owner);
	}
private:
	T* m_pRaw;
	std::shared_ptr<CTrackableObjectOwner<T>> m_owner;
};

