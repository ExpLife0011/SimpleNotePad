#pragma once
#include "Common.h"
#include "afxwin.h"

// CFormatConvertDlg �Ի���

class CFormatConvertDlg : public CDialog
{
	DECLARE_DYNAMIC(CFormatConvertDlg)

public:
	CFormatConvertDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CFormatConvertDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORMAT_CONVERT_DIALOG };
#endif

protected:
	CodeType m_input_format{};		//����ı����ʽ
	CodeType m_output_format{};		//����ı����ʽ

	vector<wstring> m_file_list;	//Ҫת�����ļ��б�
	wstring m_output_path;			//���·��

	CComboBox m_input_box;
	CComboBox m_output_box;
	CListBox m_list_box;

	string m_input_string;		//�����ļ��е��ı�
	wstring m_temp_string;		//ת����Unicode֮����ı�
	string m_output_string;		//ת����Ŀ���ʽ������������ı�

	void ShowFileList();

	void JudgeCode();
	bool OpenFile(LPCTSTR file_path);
	bool SaveFile(LPCTSTR file_path);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedAddButton();
public:
	afx_msg void OnBnClickedRemoveButton();
	afx_msg void OnBnClickedClearButton();
	afx_msg void OnBnClickedBrowseButton();
	afx_msg void OnBnClickedConvertButton();
	afx_msg void OnDropFiles(HDROP hDropInfo);
};
