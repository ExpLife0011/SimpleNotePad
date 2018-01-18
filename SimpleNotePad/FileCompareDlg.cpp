// FileCompareDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SimpleNotePad.h"
#include "FileCompareDlg.h"
#include "afxdialogex.h"


// CFileCompareDlg �Ի���

IMPLEMENT_DYNAMIC(CFileCompareDlg, CDialog)

CFileCompareDlg::CFileCompareDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_FILE_COMPARE_DIALOG, pParent)
{

}

CFileCompareDlg::~CFileCompareDlg()
{
}

UINT CFileCompareDlg::FileCompareThreadFun(LPVOID lpParam)
{
	CompareThreadInfo* pInfo = (CompareThreadInfo*)lpParam;
	size_t max_size = (pInfo->file1->size() > pInfo->file2->size() ? pInfo->file1->size() : pInfo->file2->size());
	size_t min_size = (pInfo->file1->size() < pInfo->file2->size() ? pInfo->file1->size() : pInfo->file2->size());
	pInfo->result_count = 0;
	//CString out_info;
	int progress;
	pInfo->out_info.Empty();
	for (unsigned int i{}; i < max_size; i++)
	{
		CString temp;
		if (i < pInfo->file1->size() && i < pInfo->file2->size())
		{
			if (pInfo->file1->at(i) != pInfo->file2->at(i))
			{
				temp.Format(_T("%.8x \t%.2x\t%.2x\r\n"), i, static_cast<unsigned char>(pInfo->file1->at(i)), static_cast<unsigned char>(pInfo->file2->at(i)));
				pInfo->result_count++;
			}
		}
		else if (i >= pInfo->file1->size())
		{
			temp.Format(_T("%.8x \t������\t%.2x\r\n"), i, static_cast<unsigned char>(pInfo->file2->at(i)));
			pInfo->result_count++;
		}
		else
		{
			temp.Format(_T("%.8x \t%.2x\t������\r\n"), i, static_cast<unsigned char>(pInfo->file1->at(i)));
			pInfo->result_count++;
		}
		temp.MakeUpper();
		pInfo->out_info += temp;

		//����ļ��Ƚϴ����Ѿ��˳������˳��߳�
		if (theApp.m_compare_dialog_exit)
			return 0;

		//���½�����
		progress = static_cast<int>(static_cast<__int64>(i) * 1000 / max_size);
		static int last_progress{};
		if (last_progress != progress)
		{
			::SendMessage(pInfo->hwnd, WM_COMPARE_PROGRESS, (WPARAM)progress, 0);		//����ǰ�ȽϽ���ͨ����Ϣ���͸��Ի���
			last_progress = progress;
		}
	}
	if (pInfo->out_info.IsEmpty()) pInfo->out_info = _T("�����ļ���ȫ��ͬ��");
	::PostMessage(pInfo->hwnd, WM_COMPARE_COMPLATE, 0, 0);		//�ļ��Ƚ���ɺ���һ���Ƚ������Ϣ
	return 0;
}

void CFileCompareDlg::OpenFile(LPCTSTR file_path, string & file)
{
	CWaitCursor wait_cursor;
	ifstream open_file{ file_path, std::ios::binary };
	if (open_file.fail())
	{
		CString info;
		info.Format(_T("�޷����ļ���%s����"), file_path);
		MessageBox(info, NULL, MB_OK | MB_ICONSTOP);
		return;
	}
	file.clear();
	while (!open_file.eof())
	{
		file.push_back(open_file.get());
		if (file.size() > MAX_COMPARE_SIZE)
		{
			CString info;
			info.Format(_T("��%s���ļ�̫��ֻ��ȡ��ǰ�� %d ���ֽڣ�"), file_path, MAX_COMPARE_SIZE);
			MessageBox(info, NULL, MB_OK | MB_ICONWARNING);
			//file.clear();

			break;
		}
	}
	file.pop_back();
	ClearCompareResult();
}

void CFileCompareDlg::_OnExit()
{
	theApp.m_compare_dialog_exit = true;		//���ڹر�ʱ����־��Ϊtrue
	if (m_pFileCompareThread != nullptr)
		WaitForSingleObject(m_pFileCompareThread->m_hThread, 1000);	//�ȴ��߳��˳�
}

void CFileCompareDlg::EnableButtons(bool enable)
{
	GetDlgItem(IDC_COMPARE_BUTTON)->EnableWindow(enable);
	GetDlgItem(IDC_OPEN_BUTTON1)->EnableWindow(enable);
	GetDlgItem(IDC_OPEN_BUTTON2)->EnableWindow(enable);
}

void CFileCompareDlg::ClearCompareResult()
{
	SetDlgItemText(IDC_EDIT_RESULT, _T(""));
	for(int i{}; i<=6; i++)
		m_result_info.SetItemText(i, 1, _T(""));
}

void CFileCompareDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_RESULT, m_result_info);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress_ctrl);
}


BEGIN_MESSAGE_MAP(CFileCompareDlg, CDialog)
	ON_BN_CLICKED(IDC_OPEN_BUTTON1, &CFileCompareDlg::OnBnClickedOpenButton1)
	ON_BN_CLICKED(IDC_OPEN_BUTTON2, &CFileCompareDlg::OnBnClickedOpenButton2)
	ON_BN_CLICKED(IDC_COMPARE_BUTTON, &CFileCompareDlg::OnBnClickedCompareButton)
	ON_WM_DROPFILES()
	ON_MESSAGE(WM_COMPARE_COMPLATE, &CFileCompareDlg::OnCompareComplate)
	ON_MESSAGE(WM_COMPARE_PROGRESS, &CFileCompareDlg::OnCompareProgress)
END_MESSAGE_MAP()


BOOL CFileCompareDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_result_info.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	CRect rect;
	m_result_info.GetClientRect(rect);
	size_t width1 = rect.Width() * 3 / 5;		//��1�еĿ�ȣ��б��ȵ�3/5
	size_t width2 = rect.Width() - width1 - (rect.Width() * 20 / 408);	//��2�еĿ�ȣ��б���-��1�еĿ��-��ֱ�������Ŀ��
	m_result_info.InsertColumn(0, _T("��Ŀ"), LVCFMT_LEFT, width1);		//�����1��
	m_result_info.InsertColumn(1, _T("ֵ"), LVCFMT_LEFT, width2);		//�����2��
	m_result_info.InsertItem(0, _T("�ļ�1�ֽ���"));
	m_result_info.InsertItem(1, _T("�ļ�2�ֽ���"));
	m_result_info.InsertItem(2, _T("�����ļ���С����ֽ���"));
	m_result_info.InsertItem(3, _T("��ͬ���ֽ���"));
	m_result_info.InsertItem(4, _T("��ͬ���ֽ���"));
	m_result_info.InsertItem(5, _T("��ͬ���ֽ���ռ�ٷֱ�"));
	m_result_info.InsertItem(6, _T("��ͬ���ֽ���ռ�ٷֱ�"));

	m_progress_ctrl.SetRange(0, 1000);

	theApp.m_compare_dialog_exit = false;

	//���øöԻ�������������ʾ
	ModifyStyleEx(0, WS_EX_APPWINDOW);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}

// CFileCompareDlg ��Ϣ�������


void CFileCompareDlg::OnBnClickedOpenButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//���ù�����
	LPCTSTR szFilter = _T("�����ļ�(*.*)|*.*||");
	//������ļ��Ի���
	CFileDialog fileDlg(TRUE, NULL, NULL, 0, szFilter, this);
	//��ʾ���ļ��Ի���
	if (IDOK == fileDlg.DoModal())
	{
		//CString file_path;
		m_file_path1 = fileDlg.GetPathName();	//��ȡ�򿪵��ļ�·��
		OpenFile(m_file_path1.c_str(), m_file1);		//���ļ�
		SetDlgItemText(IDC_EDIT_OPEN1, m_file_path1.c_str());
	}
}


void CFileCompareDlg::OnBnClickedOpenButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//���ù�����
	LPCTSTR szFilter = _T("�����ļ�(*.*)|*.*||");
	//������ļ��Ի���
	CFileDialog fileDlg(TRUE, NULL, NULL, 0, szFilter, this);
	//��ʾ���ļ��Ի���
	if (IDOK == fileDlg.DoModal())
	{
		//CString file_path;
		m_file_path2 = fileDlg.GetPathName();	//��ȡ�򿪵��ļ�·��
		OpenFile(m_file_path2.c_str(), m_file2);		//���ļ�
		SetDlgItemText(IDC_EDIT_OPEN2, m_file_path2.c_str());
	}
}


void CFileCompareDlg::OnBnClickedCompareButton()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//m_compare_result.clear();
	if (m_file_path1.empty())
	{
		MessageBox(_T("����ļ�1��"), NULL, MB_ICONWARNING);
		return;
	}
	if (m_file_path2.empty())
	{
		MessageBox(_T("����ļ�2��"), NULL, MB_ICONWARNING);
		return;
	}
	if (m_file_path1 == m_file_path2)
	{
		MessageBox(_T("�Ƚϵ���ͬһ���ļ���"), NULL, MB_ICONWARNING);
		return;
	}

	m_thread_info.file1 = &m_file1;
	m_thread_info.file2 = &m_file2;
	m_thread_info.hwnd = m_hWnd;
	m_pFileCompareThread = AfxBeginThread(FileCompareThreadFun, &m_thread_info);
	EnableButtons(false);
	ClearCompareResult();
}


void CFileCompareDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	TCHAR file_path[MAX_PATH];
	int drop_count = DragQueryFile(hDropInfo, -1, NULL, 0);		//ȡ�ñ��϶��ļ�����Ŀ
	if (drop_count <= 1)		//����϶��������ļ�ֻ��1��
	{
		DragQueryFile(hDropInfo, 0, file_path, MAX_PATH);		//��ȡ���ļ�
		if (m_file1.empty())		//�����1���ļ�û�д�
		{
			OpenFile(file_path, m_file1);		//�򿪵�1���ļ�
			SetDlgItemText(IDC_EDIT_OPEN1, file_path);
			m_file_path1 = file_path;
		}
		else
		{
			OpenFile(file_path, m_file2);		//�򿪵�2���ļ�
			SetDlgItemText(IDC_EDIT_OPEN2, file_path);
			m_file_path2 = file_path;
		}
	}
	else		//����϶��������ļ�Ϊ1������
	{
		DragQueryFile(hDropInfo, 0, file_path, MAX_PATH);		//��ȡ��1���ļ�
		OpenFile(file_path, m_file1);		//�򿪵�1���ļ�
		SetDlgItemText(IDC_EDIT_OPEN1, file_path);
		m_file_path1 = file_path;

		DragQueryFile(hDropInfo, 1, file_path, MAX_PATH);		//��ȡ��2���ļ�
		OpenFile(file_path, m_file2);		//�򿪵�2���ļ�
		SetDlgItemText(IDC_EDIT_OPEN2, file_path);
		m_file_path2 = file_path;
	}
	DragFinish(hDropInfo); //�ϷŽ�����,�ͷ��ڴ�

	CDialog::OnDropFiles(hDropInfo);
}




afx_msg LRESULT CFileCompareDlg::OnCompareComplate(WPARAM wParam, LPARAM lParam)
{
	CWaitCursor wait_cursor;
	SetDlgItemText(IDC_PROGRESS_STATIC, _T("�Ƚ���ɣ����������������Ժ򡭡�"));
	const int MAX_LENGTH{ 50000000 };
	if (m_thread_info.out_info.GetLength() > MAX_LENGTH)
	{
		m_thread_info.out_info = m_thread_info.out_info.Left(MAX_LENGTH);
		m_thread_info.out_info += _T("\r\n������̫���������޷���ȫ��ʾ��");
	}

	SetDlgItemText(IDC_EDIT_RESULT, m_thread_info.out_info);

	size_t max_size = (m_file1.size() > m_file2.size() ? m_file1.size() : m_file2.size());
	size_t min_size = (m_file1.size() < m_file2.size() ? m_file1.size() : m_file2.size());
	int size_difference;
	size_difference = abs(static_cast<int>(m_file1.size() - m_file2.size()));

	CString str;
	//��ӵ�0������"�ļ�1�ֽ���"
	str.Format(_T("%u"), m_file1.size());
	m_result_info.SetItemText(0, 1, str);
	//��ӵ�1������"�ļ�2�ֽ���"
	str.Format(_T("%u"), m_file2.size());
	m_result_info.SetItemText(1, 1, str);
	//��ӵ�2������"�����ļ���С����ֽ���"
	str.Format(_T("%d"), size_difference);
	m_result_info.SetItemText(2, 1, str);
	//��ӵ�3������"��ͬ���ֽ���"
	str.Format(_T("%u"), max_size - m_thread_info.result_count);
	m_result_info.SetItemText(3, 1, str);
	//��ӵ�4������"��ͬ���ֽ���"
	str.Format(_T("%u"), m_thread_info.result_count - size_difference);
	m_result_info.SetItemText(4, 1, str);
	//��ӵ�5������"��ͬ���ֽ���ռ�ٷֱ�"
	str.Format(_T("%.2f%%"), (max_size - m_thread_info.result_count) * 100 / static_cast<float>(min_size));
	m_result_info.SetItemText(5, 1, str);
	//��ӵ�6������"��ͬ���ֽ���ռ�ٷֱ�"
	str.Format(_T("%.2f%%"), (m_thread_info.result_count - size_difference) * 100 / static_cast<float>(min_size));
	m_result_info.SetItemText(6, 1, str);
	SetDlgItemText(IDC_PROGRESS_STATIC, _T("�Ƚ���ɡ�"));
	EnableButtons(true);
	return 0;
}


afx_msg LRESULT CFileCompareDlg::OnCompareProgress(WPARAM wParam, LPARAM lParam)
{
	int progress = wParam;
	m_progress_ctrl.SetPos(progress);
	CString info;
	info.Format(_T("���ڱȽϣ����Ժ������%.1f%%"), static_cast<float>(progress) / 10.0);
	SetDlgItemText(IDC_PROGRESS_STATIC, info);
	return 0;
}


void CFileCompareDlg::OnOK()
{
	// TODO: �ڴ����ר�ô����/����û���
	_OnExit();
	CDialog::OnOK();
}


void CFileCompareDlg::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���
	_OnExit();
	CDialog::OnCancel();
}
