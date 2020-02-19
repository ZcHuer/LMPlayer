#ifndef IObserver_h__
#define IObserver_h__

#include <list>
#include "Base.h"

template <typename T>
struct IPublisher
{
    virtual void Attach(T* pObserver) = 0;
    virtual void Detach(T* pObserver) = 0;
};

template <typename P>
struct IObserver1
{
    virtual void OnUpdate1(const P& data) = 0;
};

template <typename P1, typename P2>
struct IObserver2
{
    virtual void OnUpdate2(const P1& p1, const P2& p2) = 0;
};

template <typename P1, typename P2, typename P3>
struct IObserver3
{
    virtual void OnUpdate3(const P1& p1, const P2& p2, const P3& p3) = 0;
};

template <typename P1, typename P2, typename P3, typename P4>
struct IObserver4
{
    virtual void OnUpdate4(const P1& p1, const P2& p2, const P3& p3, const P4& p4) = 0;
};


template <typename T, typename mutex = CLockNo>
struct PublisherBase
{
public:
    virtual void Attach(T* pObserver)
    {
        CAutoLock lg(m_mutex);
#ifdef DEBUG
        if (m_observers.end() != std::find(m_observers.begin(), m_observers.end(), pObserver))
        {
            assert(false);
        }
#endif // DEBUG
        m_observers.push_back(pObserver);
    }

    virtual void Detach(T* pObserver)
    {
		CAutoLock lg(m_mutex);
		auto it = std::find(m_observers.begin(), m_observers.end(), pObserver);
		if (it != m_observers.end())
		{
			m_observers.erase(it);
		}
		else
		{
			assert(false);
		}
	}

protected:
    template <typename P>
    void UpdateImpl1(const P& data)
    {
        CAutoLock lg(m_mutex);
        for (T* observer : m_observers)
        {
			observer->OnUpdate1(data);
		}
    }

	template <typename P>
	void UpdateImpl1(P& data)
	{
		CAutoLock lg(m_mutex);
		for (T* observer : m_observers)
		{
			observer->OnUpdate1(data);
		}
	}

    template <typename P1, typename P2>
    void UpdateImpl2(const P1& p1, const P2& p2)
    {
        CAutoLock lg(m_mutex);
		for (T* observer : m_observers)
		{
			observer->OnUpdate2(p1, p2);
		}
    }

	template <typename P1, typename P2>
	void UpdateImpl2(P1& p1, P2& p2)
	{
		CAutoLock lg(m_mutex);
		for (T* observer : m_observers)
		{
			observer->OnUpdate2(p1, p2);
		}
	}

    template <typename P1, typename P2, typename P3>
    void UpdateImpl3(const P1& p1, const P2& p2, const P3& p3)
    {
        CAutoLock lg(m_mutex);
        for (T* observer : m_observers)
        {
            observer->OnUpdate3(p1, p2, p3);
        }
    }

    template <typename P1, typename P2, typename P3>
    void UpdateImpl3(P1& p1, P2& p2, P3& p3)
    {
        CAutoLock lg(m_mutex);
        for (T* observer : m_observers)
        {
            observer->OnUpdate3(p1, p2, p3);
        }
    }

    template <typename P1, typename P2, typename P3, typename P4>
    void UpdateImpl4(const P1& p1, const P2& p2, const P3& p3, const P4& p4)
    {
        CAutoLock lg(m_mutex);
        for (T* observer : m_observers)
        {
            observer->OnUpdate4(p1, p2, p3, p4);
        }
    }

    template <typename P1, typename P2, typename P3, typename P4>
    void UpdateImpl4(P1& p1, P2& p2, P3& p3, P4& p4)
    {
        CAutoLock lg(m_mutex);
        for (T* observer : m_observers)
        {
            observer->OnUpdate4(p1, p2, p3, p4);
        }
    }
protected:
	mutex			m_mutex;
	std::list<T*>	m_observers;
};

#endif // IObserver_h__
