#pragma once

#include "resource.h"
#include "dui/DuiFrameWnd.h"
// CPlayDemoDlg2 对话框

class CPlayDemoDlg2 : public CDialog
{
	DECLARE_DYNAMIC(CPlayDemoDlg2)

public:
	CPlayDemoDlg2(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPlayDemoDlg2();

// 对话框数据
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

public:
	//CDuiFrameWnd*	m_pDuiFrameWnd;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
