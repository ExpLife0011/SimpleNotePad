// FormatConvertDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SimpleNotePad.h"
#include "FormatConvertDlg.h"
#include "afxdialogex.h"


// CFormatConvertDlg �Ի���

IMPLEMENT_DYNAMIC(CFormatConvertDlg, CDialog)

CFormatConvertDlg::CFormatConvertDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_FORMAT_CONVERT_DIALOG, pParent)
{

}

CFormatConvertDlg::~CFormatConvertDlg()
{
}

void CFormatConvertDlg::ShowFileList()
{
	m_list_box.ResetContent();
	for (const auto& item : m_file_list)
	{
		m_list_box.AddString(item.c_str());
	}
	CCommon::AdjustListWidth(m_list_box);
}


void CFormatConvertDlg::JudgeCode()
{
	if (m_input_string.size() >= 3 && m_input_string[0] == -17 && m_input_string[1] == -69 && m_input_string[2] == -65)
		m_input_format = CodeType::UTF8;
	else if (m_input_string.size() >= 2 && m_input_string[0] == -1 && m_input_string[1] == -2)
		m_input_format = CodeType::UTF16;
	else if (CCommon::IsUTF8Bytes(m_input_string.c_str()))
		m_input_format = CodeType::UTF8_NO_BOM;
	else m_input_format = CodeType::ANSI;
}

bool CFormatConvertDlg::OpenFile(LPCTSTR file_path)
{
	m_input_string.clear();
	ifstream file{ file_path, std::ios::binary };
	if (file.fail())
	{
		CString info;
		info.Format(_T("�޷����ļ���%s����"), file_path);
		MessageBox(info, NULL, MB_OK | MB_ICONSTOP);
		return false;
	}
	while (!file.eof())
	{
		m_input_string.push_back(file.get());
	}
	m_input_string.pop_back();

	if (m_input_format != CodeType::AUTO)
	{
		m_temp_string = CCommon::StrToUnicode(m_input_string, m_input_format);	//ת����Unicode
	}
	else		//��������ʽΪ���Զ���ʱ���Զ��жϱ�������
	{
		JudgeCode();											//�жϱ�������
		m_temp_string = CCommon::StrToUnicode(m_input_string, m_input_format);	//ת����Unicode
		if (m_temp_string.size() < m_input_string.size() / 4)		//������Զ�ʶ��ĸ�ʽת����Unicode��Unicode�ַ����ĳ���С�ڶ��ֽ��ַ������ȵ�1/4�����ı��ı����ʽ������UTF16
		{
			m_input_format = CodeType::UTF16;
			m_temp_string = CCommon::StrToUnicode(m_input_string, m_input_format);	//����ת����Unicode
		}
	}
	return true;
}

bool CFormatConvertDlg::SaveFile(LPCTSTR file_path)
{
	bool char_connot_convert;
	m_output_string = CCommon::UnicodeToStr(m_temp_string, char_connot_convert, m_output_format);
	if (char_connot_convert)	//���ļ��а���Unicode�ַ�ʱ����ʾ��Щ�ַ������޷�ת��
	{
		CString info;
		info.Format(_T("���棺%s�ļ��д����޷�ת�����ַ�����Щ�ַ��Ѷ�ʧ��Ҫ������Щ�ַ���Ӧ��ѡ��ת����һ��Unicode��ʽ�ı��롣"), file_path);
		MessageBox(info, NULL, MB_ICONWARNING);
	}
	ofstream file{ file_path, std::ios::binary };
	if (file.fail())
	{
		CString info;
		info.Format(_T("��%s�ļ��޷����棬�������·���Ƿ���ȷ����"), file_path);
		MessageBox(info, NULL, MB_OK | MB_ICONSTOP);
		return false;
	}
	file << m_output_string;
	return true;
}



void CFormatConvertDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_INPUT_COMBO, m_input_box);
	DDX_Control(pDX, IDC_OUTPUT_COMBO, m_output_box);
	DDX_Control(pDX, IDC_FILE_LIST, m_list_box);
}


BEGIN_MESSAGE_MAP(CFormatConvertDlg, CDialog)
	ON_BN_CLICKED(IDC_ADD_BUTTON, &CFormatConvertDlg::OnBnClickedAddButton)
	ON_BN_CLICKED(IDC_REMOVE_BUTTON, &CFormatConvertDlg::OnBnClickedRemoveButton)
	ON_BN_CLICKED(IDC_CLEAR_BUTTON, &CFormatConvertDlg::OnBnClickedClearButton)
	ON_BN_CLICKED(IDC_BROWSE_BUTTON, &CFormatConvertDlg::OnBnClickedBrowseButton)
	ON_BN_CLICKED(IDC_CONVERT_BUTTON, &CFormatConvertDlg::OnBnClickedConvertButton)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


// CFormatConvertDlg ��Ϣ�������


BOOL CFormatConvertDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	//���øöԻ�������������ʾ
	ModifyStyleEx(0, WS_EX_APPWINDOW);

	m_input_box.AddString(_T("�Զ�ʶ��"));
	m_input_box.AddString(_T("ANSI"));
	m_input_box.AddString(_T("UTF8"));
	m_input_box.AddString(_T("UTF16"));
	m_input_box.SetCurSel(0);

	m_output_box.AddString(_T("ANSI"));
	m_output_box.AddString(_T("UTF8"));
	m_output_box.AddString(_T("UTF16"));
	m_output_box.SetCurSel(1);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


void CFormatConvertDlg::OnBnClickedAddButton()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//���ù�����
	LPCTSTR szFilter = _T("�ı��ļ�(*.txt)|*.txt|�����ļ�(*.*)|*.*||");
	//������ļ��Ի���
	CFileDialog fileDlg(TRUE, NULL, NULL, OFN_ALLOWMULTISELECT, szFilter, this);
	//��ʾ���ļ��Ի���
	if (IDOK == fileDlg.DoModal())
	{
		POSITION posFile = fileDlg.GetStartPosition();
		while (posFile != NULL)
		{
			m_file_list.push_back(fileDlg.GetNextPathName(posFile).GetString());
		}
		ShowFileList();
	}
}


void CFormatConvertDlg::OnBnClickedRemoveButton()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int select;
	select = m_list_box.GetCurSel();		//��ȡ��ǰѡ�������
	if (select >= 0 && select < m_file_list.size())
	{
		m_file_list.erase(m_file_list.begin() + select);	//ɾ��ѡ����
		ShowFileList();
	}
}


void CFormatConvertDlg::OnBnClickedClearButton()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_file_list.clear();
	ShowFileList();
}


void CFormatConvertDlg::OnBnClickedBrowseButton()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CFolderPickerDialog folderPickerDlg;
	if (folderPickerDlg.DoModal() == IDOK)
	{
		m_output_path = folderPickerDlg.GetPathName();
		SetDlgItemText(IDC_FOLDER_EDIT, m_output_path.c_str());
	}
}


void CFormatConvertDlg::OnBnClickedConvertButton()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_file_list.empty())
	{
		MessageBox(_T("�����Ҫת�����ļ���"), NULL, MB_ICONWARNING);
		return;
	}
	if (m_output_path.empty())
	{
		MessageBox(_T("��ѡ������ļ��У�"), NULL, MB_ICONWARNING);
		return;
	}

	CWaitCursor wait_cursor;
	switch (m_output_box.GetCurSel())
	{
	case 0: m_output_format = CodeType::ANSI; break;
	case 1: m_output_format = CodeType::UTF8; break;
	case 2: m_output_format = CodeType::UTF16; break;
	default:
		break;
	}

	int convert_cnt{};
	for (const auto& item : m_file_list)
	{
		switch (m_input_box.GetCurSel())
		{
		case 0: m_input_format = CodeType::AUTO; break;
		case 1: m_input_format = CodeType::ANSI; break;
		case 2: m_input_format = CodeType::UTF8; break;
		case 3: m_input_format = CodeType::UTF16; break;
		default: break;
		}
		if(!OpenFile(item.c_str())) continue;		//�����ǰ�ļ��޷��򿪣���������
		
		wstring file_name;
		size_t index;
		index = item.find_last_of(L'\\');
		if (index == string::npos) continue;
		file_name = item.substr(index);
		if(!SaveFile((m_output_path + file_name).c_str())) continue;
		convert_cnt++;
	}

	CString info;
	info.Format(_T("ת����ɣ���ת��%d���ļ���"), convert_cnt);
	MessageBox(info, NULL, MB_ICONINFORMATION);
}


void CFormatConvertDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	TCHAR file_path[MAX_PATH];
	int drop_count = DragQueryFile(hDropInfo, -1, NULL, 0);		//ȡ�ñ��϶��ļ�����Ŀ
	for (int i{}; i < drop_count; i++)
	{
		DragQueryFile(hDropInfo, i, file_path, MAX_PATH);
		m_file_list.emplace_back(file_path);
	}
	ShowFileList();
	CDialog::OnDropFiles(hDropInfo);
}
