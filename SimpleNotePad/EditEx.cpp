// EditEx.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "EditEx.h"


// CEditEx

IMPLEMENT_DYNAMIC(CEditEx, CEdit)

CEditEx::CEditEx()
{

}

CEditEx::~CEditEx()
{
}


BEGIN_MESSAGE_MAP(CEditEx, CEdit)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()



// CEditEx ��Ϣ�������




BOOL CEditEx::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	//��Ctrl+Aȫѡ
	if ((GetKeyState(VK_CONTROL) & 0x80) && (pMsg->wParam == 'A'))
	{
		SetSel(0, -1);
		return TRUE;
	}
	return CEdit::PreTranslateMessage(pMsg);
}


void CEditEx::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CString edit_str;
	GetWindowText(edit_str);
	int length{ edit_str.GetLength() };
	int cursor_pos;				//���λ��
	int not_used;
	GetSel(cursor_pos, not_used);		//��ȡ��ǰ����λ�ã�����˫��ʱ������ѡ���κ��ı������Դ˺���������������ֵ�ض�һ��
	int start_pos{ cursor_pos };		//ѡ�����ʼλ��
	int end_pos{ cursor_pos };			//ѡ��Ľ���λ��
	while (start_pos >= 0 && CCommon::IsValidNameChar(edit_str[start_pos]))
	{
		start_pos--;
	}
	while (end_pos < length && CCommon::IsValidNameChar(edit_str[end_pos]))
	{
		end_pos++;
	}
	SetSel(start_pos + 1, end_pos);
	//CEdit::OnLButtonDblClk(nFlags, point);
}
