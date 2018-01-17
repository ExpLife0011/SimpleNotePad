#pragma once
//#define MAX_STR_SIZE 1024;
using std::wstring;
using std::string;

enum class CodeType
{
	ANSI,
	UTF8,
	UTF8_NO_BOM,
	UTF16,
	AUTO
};


class CCommon
{
public:
	CCommon();
	~CCommon();

	//��string���͵��ַ���ת����Unicode�����wstring�ַ���
	static wstring StrToUnicode(const string& str, CodeType code_type = CodeType::ANSI);

	//��Unicode�����wstring�ַ���ת����string���͵��ַ���(���������һ���ַ��޷�ת������char_cannot_convert��Ϊtrue)
	static string UnicodeToStr(const wstring& wstr, bool& char_cannot_convert, CodeType code_type = CodeType::ANSI);

	//�ж��ַ����Ƿ�UTF8����
	static bool IsUTF8Bytes(const char* data);

	static void WritePrivateProfileInt(const wchar_t* AppName, const wchar_t* KeyName, int value, const wchar_t* Path);

	//static bool FileExist(const wchar_t* file);

	//��ȡ��ǰexe�ļ���·���������ļ�����
	static wstring GetCurrentPath();

	//����CListBox�ؼ��Ŀ��
	static void AdjustListWidth(CListBox & list);

	//�޸��ַ����ĳ��ȣ�����µĳ��ȱ�ԭ���Ķ̣������ұߵ��ַ�������µĳ��ȱ�ԭ���ĳ�������������ָ�����ַ�
	static void ChangeStringLength(CString& str, int length, TCHAR ch);

	//��һ��Edit box����ַ���
	static void EditAppendString(const CString& str, HWND hWnd);
};

