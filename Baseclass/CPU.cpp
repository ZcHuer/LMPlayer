#include "stdafx.h"
#include "CPU.h"


//////////////////////////////////////////////////////////////////////////
//CPU相关信息
//////////////////////////////////////////////////////////////////////////
#define CPU_VENDOR_UNKNOWN				0
#define CPU_VENDOR_INTEL				1
#define CPU_VENDOR_AMD					2
#define CPU_VENDOR_VIA					3

#define CPU_SET_MMX							0x00000001
#define CPU_SET_SSE							0x00000002
#define CPU_SET_SSE2						0x00000004
#define CPU_SET_SSE3						0x00000008
#define CPU_SET_SSSE3						0x00000010
#define CPU_SET_SSE41						0x00000020
#define CPU_SET_SSE42						0x00000040

//////////////////////////////////////////////////////////////////////////
//GPU相关信息
//////////////////////////////////////////////////////////////////////////
#define GPU_VENDOR_UNKNOWN				0
#define GPU_VENDOR_INTEL				1
#define GPU_VENDOR_AMD					2
#define GPU_VENDOR_NVIDIA				3
#define GPU_VENDOR_VIA					4


CCPU::CCPU(void)
	:m_eax(0),m_ebx(0),m_ecx(0),m_edx(0)
{
	m_dwCPUSet = 0 ;
	m_bIA64 = false;
	m_dwCPUCount = 0;

	//http://blog.csdn.net/yaoyaofull/archive/2009/11/09/4788842.aspx
	// Intel CPU的CPUID指令
	//http://baike.baidu.com/view/1829765.htm?fr=ala0_1
	Executecpuid(1); // 执行cpuid指令，使用输入参数 eax = 1 
	if( m_edx & (1<<23) )
		m_dwCPUSet |= CPU_SET_MMX;

	if( m_edx & (1<<25) )
		m_dwCPUSet |= CPU_SET_SSE;

	if( m_edx & (1<<26) )
		m_dwCPUSet |= CPU_SET_SSE2;

	if( m_edx & (1<<30) )
		m_bIA64 = true;

	if( m_ecx & 1 )
		m_dwCPUSet |= CPU_SET_SSE3;

	if(  m_ecx & (1<<19) )
		m_dwCPUSet |= CPU_SET_SSE41;

	if(  m_ecx & (1<<20) )
		m_dwCPUSet |= CPU_SET_SSE42;

	//取出CPU个数
	//然后看看EBX的23到16位，这些位代表了逻辑核心数目
	m_dwCPUCount = (m_ebx & 0x00FF0000)>>16;

}


CCPU::~CCPU(void)
{
}

void CCPU::Executecpuid(DWORD veax)
{
	// 因为嵌入式的汇编代码不能识别 类成员变量
	// 所以定义四个临时变量作为过渡
	DWORD deax;
	DWORD debx;
	DWORD decx;
	DWORD dedx;

	__asm
	{
		mov eax, veax
			cpuid 
			mov deax, eax
			mov debx, ebx
			mov decx, ecx
			mov dedx, edx
	}

	m_eax = deax; // 把临时变量中的内容放入类成员变量
	m_ebx = debx;
	m_ecx = decx;
	m_edx = dedx;
}
//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

#define EAX_LEN				4
void CCPU::GetName(WCHAR* pCPUName, int cchMax)
{
	if( pCPUName == NULL )
		return ;

	const DWORD BRANDID = 0x80000002;  // 从0x80000002开始，到0x80000004结束
	char szBrand[EAX_LEN*4*3+1] = {0};    // 用来存储商标字符串，48个字符

	for (DWORD i = 0; i < 3; i++)   // 依次执行3个指令
	{
		Executecpuid(BRANDID + i);  
		// 每次执行结束后，保存四个寄存器里的asc码到数组
		memcpy(szBrand + i*EAX_LEN*4 + EAX_LEN*0, &m_eax, EAX_LEN);
		memcpy(szBrand + i*EAX_LEN*4 + EAX_LEN*1, &m_ebx, EAX_LEN);
		memcpy(szBrand + i*EAX_LEN*4 + EAX_LEN*2, &m_ecx, EAX_LEN);
		memcpy(szBrand + i*EAX_LEN*4 + EAX_LEN*3, &m_edx, EAX_LEN);
	}

	//去掉前面的空格
	const char* pNoBlank = szBrand;
	while(*pNoBlank)
	{
		if( *pNoBlank != ' ')
			break;

		pNoBlank++;
	}

	MultiByteToWideChar( CP_ACP, 0, pNoBlank, strlen(pNoBlank), pCPUName, cchMax );
}

void CCPU::GetVendorName(WCHAR* pName,  int cchMax)
{
	char szVID[EAX_LEN*3+1] = {0};   // 字符串，用来存储制造商信息
	Executecpuid(0);  // 执行cpuid指令，使用输入参数 eax = 0
	memcpy(szVID+EAX_LEN*0, &m_ebx, EAX_LEN); // 复制前四个字符到数组
	memcpy(szVID+EAX_LEN*1, &m_edx, EAX_LEN); // 复制中间四个字符到数组
	memcpy(szVID+EAX_LEN*2, &m_ecx, EAX_LEN); // 复制最后四个字符到数组

	MultiByteToWideChar( CP_ACP, 0, szVID, strlen(szVID), pName, cchMax );
}

long CCPU::GetVendor()
{
	char szVID[EAX_LEN*3+1] = {0};   // 字符串，用来存储制造商信息
	Executecpuid(0);  // 执行cpuid指令，使用输入参数 eax = 0
	memcpy(szVID+EAX_LEN*0, &m_ebx, EAX_LEN); // 复制前四个字符到数组
	memcpy(szVID+EAX_LEN*1, &m_edx, EAX_LEN); // 复制中间四个字符到数组
	memcpy(szVID+EAX_LEN*2, &m_ecx, EAX_LEN); // 复制最后四个字符到数组

	if( strcmp(szVID, "GenuineIntel") == 0 )
		return CPU_VENDOR_INTEL;
	else if( strcmp(szVID, "AuthenticAMD") == 0 )
		return CPU_VENDOR_AMD;
	else if( strcmp(szVID, "VIA VIA VIA") == 0 )
		return CPU_VENDOR_VIA;

	return CPU_VENDOR_UNKNOWN;
}

long CCPU::GetCoreCount()
{
	if( m_dwCPUCount == 0 )
	{
		SYSTEM_INFO  info = {0};

		GetSystemInfo(&info);
		return info.dwNumberOfProcessors;
	}

	return m_dwCPUCount;
}

DWORD CCPU::GetCPUSet()
{
	return m_dwCPUSet;
}

