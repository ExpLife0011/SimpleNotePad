#pragma once
#include "afxcmn.h"
#include "Common.h"

// CFileCompareDlg �Ի���

class CFileCompareDlg : public CDialog
{
	DECLARE_DYNAMIC(CFileCompareDlg)

public:
	CFileCompareDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CFileCompareDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILE_COMPARE_DIALOG };
#endif

#define WM_COMPARE_COMPLATE (WM_USER+101)		//�ļ��Ƚ������Ϣ
#define WM_COMPARE_PROGRESS (WM_USER+102)		//�ļ��ȽϽ�����Ϣ

protected:
	//struct ResultItem
	//{
	//	unsigned int address;	//��ַ
	//	unsigned char data1;	//�ļ�1�иõ�ַ�ϵ�����
	//	unsigned char data2;	//�ļ�2�иõ�ַ�ϵ�����
	//	bool no_data1;		//�ļ�1�иõ�ַ��û������
	//	bool no_data2;		//�ļ�2�иõ�ַ��û������
	//	//���캯��
	//	ResultItem(){}
	//	ResultItem(unsigned int _address, unsigned char _data1, unsigned char _data2, bool _no_data1, bool _no_data2) :
	//		address{ _address }, data1{ _data1 }, data2{ _data2 }, no_data1{ _no_data1 }, no_data2{ _no_data2 }
	//	{}
	//};
	//vector<ResultItem> m_compare_result;

	//�������ļ��ȽϹ����̴߳�����Ϣ�Ľṹ��
	struct CompareThreadInfo
	{
		HWND hwnd;		//�Ի���ľ��
		string* file1;		//Ҫ�Ƚϵ��ļ�1��ָ��
		string* file2;		//Ҫ�Ƚϵ��ļ�2��ָ��
		CString out_info;		//����ȽϽ���������Ϣ
		int result_count;		//���治ͬ���ֽ���
		//HWND edit_handle;		//Edit�ؼ��ľ��
		//int complete_ratio;		//��ɵı�����ȡֵΪ0~1000
	};

	static UINT FileCompareThreadFun(LPVOID lpParam);

	CompareThreadInfo m_thread_info;
	CWinThread* m_pFileCompareThread;		//�ļ��Ƚϵ��߳�

	CListCtrl m_result_info;
	CProgressCtrl m_progress_ctrl;

	string m_file1;		//�ļ�1������
	string m_file2;		//�ļ�2������

	wstring m_file_path1;	//�ļ�1��·��
	wstring m_file_path2;	//�ļ�2��·��

	void OpenFile(LPCTSTR file_path,string& file);

	void _OnExit();

	void EnableButtons(bool enable = true);
	void ClearCompareResult();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOpenButton1();
	afx_msg void OnBnClickedOpenButton2();
	afx_msg void OnBnClickedCompareButton();
	afx_msg void OnDropFiles(HDROP hDropInfo);
public:
	virtual BOOL OnInitDialog();
protected:
	afx_msg LRESULT OnCompareComplate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCompareProgress(WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
	virtual void OnCancel();
};
