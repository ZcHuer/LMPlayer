// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� LEUAC_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// LEUAC_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef LEUAC_EXPORTS
#define LEUAC_API __declspec(dllexport)
#else
#define LEUAC_API __declspec(dllimport)
#endif

// �����Ǵ� LeUac.dll ������
class LEUAC_API CLeUac {
public:
	CLeUac(void);
	// TODO:  �ڴ�������ķ�����
};

extern LEUAC_API int nLeUac;

LEUAC_API int fnLeUac(void);
