#pragma once

#include "resource.h"
#include "dui/DuiFrameWnd.h"

// CPlayDemoDlg1 �Ի���

class CPlayDemoDlg1 : public CDialog
{
	DECLARE_DYNAMIC(CPlayDemoDlg1)

public:
	CPlayDemoDlg1(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPlayDemoDlg1();

// �Ի�������
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

public:
	CDuiFrameWnd*	m_pDuiFrameWnd;

protected:
	HICON	m_hIcon;
public:
	void AdaptWindowSize(UINT cxScreen);                            // ������Ļ�ֱ����Զ��������ڴ�С



	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
