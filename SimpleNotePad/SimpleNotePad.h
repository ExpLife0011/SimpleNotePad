
// SimpleNotePad.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CSimpleNotePadApp: 
// �йش����ʵ�֣������ SimpleNotePad.cpp
//

class CSimpleNotePadApp : public CWinApp
{
public:
	CSimpleNotePadApp();

	wstring m_config_path;		//�����ļ����ڵ�·��

	bool m_compare_dialog_exit;	//����ָʾ�ļ��Ƚ϶Ի����Ƿ����˳�

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CSimpleNotePadApp theApp;