// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� LEDAEMONWORK_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// LEDAEMONWORK_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef LEDAEMONWORK_EXPORTS
#define LEDAEMONWORK_API __declspec(dllexport)
#else
#define LEDAEMONWORK_API __declspec(dllimport)
#endif


extern "C"
{
	LEDAEMONWORK_API int fnLeDaemonWork(wchar_t * cPath);
}
