#pragma once

#include "resource.h"
#include "dui/DuiFrameWnd.h"

// CPlayDemoDlg1 对话框

class CPlayDemoDlg1 : public CDialog
{
	DECLARE_DYNAMIC(CPlayDemoDlg1)

public:
	CPlayDemoDlg1(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPlayDemoDlg1();

// 对话框数据
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

public:
	CDuiFrameWnd*	m_pDuiFrameWnd;

protected:
	HICON	m_hIcon;
public:
	void AdaptWindowSize(UINT cxScreen);                            // 根据屏幕分辨率自动调整窗口大小



	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
