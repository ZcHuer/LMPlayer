#ifndef RingBuffer_h__
#define RingBuffer_h__

#include <mutex>
#include <memory>

/* ����BUFFER�� ����һ�߶���һ��д�� �����������ݿ��ܻḲ��ǰ�������*/
class CRingBuffer
{
public:
	CRingBuffer(DWORD dwInitSize, DWORD dwMaxSize)
		: m_spBuffer(new char[dwInitSize], std::default_delete<char[]>())
		, m_dwBufferLen(dwInitSize)
		, m_dwMaxSize(dwMaxSize)
		, m_dwReadOffset(0)
		, m_dwWriteOffset(0)
		, m_dwDataLen(0)

	{
		ATLASSERT(m_dwBufferLen > 0);
		ATLASSERT(m_dwMaxSize >= m_dwBufferLen);
		ATLASSERT(m_spBuffer);
	}

	~CRingBuffer() {};

	int Read(char* szBuffer, DWORD dwLen)
	{
		std::lock_guard<std::mutex> lk(m_lckBuffer);
		int nRet = ReadUnLock(szBuffer, dwLen);
		if (nRet < 0)
			return -1;

		// ���¶�ָ������ݳ���
		m_dwReadOffset = nRet;
		m_dwDataLen = m_dwDataLen - dwLen;
		return 0;
	}

	int ReadWithNoMove(char* szBuffer, DWORD dwLen)
	{
		std::lock_guard<std::mutex> lk(m_lckBuffer);
		return ReadUnLock(szBuffer, dwLen);
	}

	int Write(const char* szBuffer, DWORD dwLen)
	{
		std::lock_guard<std::mutex> lk(m_lckBuffer);

		// ���ݳ����������
		if (NULL == szBuffer || 0 == dwLen || m_dwDataLen + dwLen > m_dwMaxSize)
			return -1;

		if (m_dwDataLen + dwLen > m_dwBufferLen)
		{
			// ���ݳ�����ǰBUFFER����
			DWORD dwNewBuffenLen = m_dwBufferLen;
			while (m_dwDataLen + dwLen > dwNewBuffenLen)
			{	 	
				dwNewBuffenLen = dwNewBuffenLen * 2;
			}

			if (dwNewBuffenLen > m_dwMaxSize)
				dwNewBuffenLen = m_dwMaxSize;

			// ���·��������ڴ�ռ䣬����ȡʣ�����ݵ��»���
			std::shared_ptr<char> spNewBuffer(new char[dwNewBuffenLen], std::default_delete<char[]>());
			if (!spNewBuffer)
				return -1;

			if(m_dwDataLen > 0)
			{
				int nRet = ReadUnLock(spNewBuffer.get(), m_dwDataLen);
				if (nRet < 0)
					return -1;
			}

			m_spBuffer = spNewBuffer;
			m_dwBufferLen = dwNewBuffenLen;
			m_dwReadOffset = 0;
			m_dwWriteOffset = m_dwDataLen;
		}

		// ����дָ������ݳ���
		if (m_dwWriteOffset + dwLen > m_dwBufferLen)
		{
			// дָ���ұߵĿռ䲻����ʣ����ֽڸ��ǻ������������
			DWORD dwWriteRight = m_dwBufferLen - m_dwWriteOffset;
			DWORD dwWriteLeft = dwLen - dwWriteRight;
			if(dwWriteRight > 0)
				memcpy(m_spBuffer.get() + m_dwWriteOffset, szBuffer, dwWriteRight);
			memcpy(m_spBuffer.get(), szBuffer+dwWriteRight, dwWriteLeft);
			m_dwWriteOffset = dwWriteLeft;
		}
		else
		{
			memcpy(m_spBuffer.get() + m_dwWriteOffset, szBuffer, dwLen);
			m_dwWriteOffset += dwLen;
		}
		m_dwDataLen += dwLen;
		return 0;
	}

private:
	int ReadUnLock(char* szBuffer, DWORD dwLen)
	{
		// ���ݲ�����
		if (NULL == szBuffer || 0 == dwLen || !m_spBuffer || m_dwDataLen < dwLen)
			return -1;

		int nOffset = -1;
		if (m_dwReadOffset + dwLen > m_dwBufferLen)
		{
			// ��ָ���ұߵ����ݲ�����
			DWORD dwReadRight = m_dwBufferLen - m_dwReadOffset;
			DWORD dwReadLeft = dwLen - dwReadRight;
			if(dwReadRight > 0)
				memcpy(szBuffer, m_spBuffer.get() + m_dwReadOffset, dwReadRight);
			memcpy(szBuffer+dwReadRight, m_spBuffer.get(), dwReadLeft);
			nOffset = dwReadLeft;
		}
		else
		{
			memcpy(szBuffer, m_spBuffer.get() + m_dwReadOffset, dwLen);
			nOffset = m_dwReadOffset + dwLen;
		}
		return nOffset;
	}

private:
	DWORD m_dwMaxSize;
	DWORD m_dwReadOffset;
	DWORD m_dwWriteOffset;
	DWORD  m_dwDataLen;
	DWORD m_dwBufferLen;
	std::mutex  m_lckBuffer;
	std::shared_ptr<char> m_spBuffer;
};

#endif // RingBuffer_h__
