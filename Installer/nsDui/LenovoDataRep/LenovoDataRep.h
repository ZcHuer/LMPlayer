// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� LENOVODATAREP_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// LENOVODATAREP_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef LENOVODATAREP_EXPORTS
#define LENOVODATAREP_API __declspec(dllexport)
#else
#define LENOVODATAREP_API __declspec(dllimport)
#endif

// �����Ǵ� LenovoDataRep.dll ������
class LENOVODATAREP_API CLenovoDataRep {
public:
	CLenovoDataRep(void);
	// TODO:  �ڴ�������ķ�����
};

extern LENOVODATAREP_API int nLenovoDataRep;

LENOVODATAREP_API int fnLenovoDataRep(void);
