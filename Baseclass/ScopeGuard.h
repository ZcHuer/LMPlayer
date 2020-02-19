#ifndef __SCOPE_GUARD_H__
#define __SCOPE_GUARD_H__

#pragma once

#include <functional>


class ScopeGuard
{
public:
	ScopeGuard(std::function<void(void)> func) : m_func(func), m_dismissed(false) {};
	~ScopeGuard() {
		if (!m_dismissed && m_func)
		{
			m_func();
		}
	}

	void Dismiss() const {
		m_dismissed = true;
	}
private:
	mutable bool m_dismissed;
	std::function<void(void)> m_func;
};

#endif