#pragma once

#include "resource.h"
#include "dui/DuiFrameWnd.h"
// CPlayDemoDlg2 �Ի���

class CPlayDemoDlg2 : public CDialog
{
	DECLARE_DYNAMIC(CPlayDemoDlg2)

public:
	CPlayDemoDlg2(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPlayDemoDlg2();

// �Ի�������
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

public:
	//CDuiFrameWnd*	m_pDuiFrameWnd;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
