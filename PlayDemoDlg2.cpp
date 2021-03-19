// PlayDemoDlg2.cpp : 实现文件
//

#include "stdafx.h"
#include "PlayDemo.h"
#include "PlayDemoDlg2.h"
#include "dui/KDetectMemoryLeak.h"

// CPlayDemoDlg2 对话框

IMPLEMENT_DYNAMIC(CPlayDemoDlg2, CDialog)

CPlayDemoDlg2::CPlayDemoDlg2(CWnd* pParent /*=NULL*/)
	: CDialog(CPlayDemoDlg2::IDD, pParent)
{
	//m_pDuiFrameWnd = NULL;
}

CPlayDemoDlg2::~CPlayDemoDlg2()
{
	//delete m_pDuiFrameWnd;
	//::CoUninitialize();
}

void CPlayDemoDlg2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPlayDemoDlg2, CDialog)
END_MESSAGE_MAP()


// CPlayDemoDlg2 消息处理程序

BOOL CPlayDemoDlg2::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	//atexit(CheckOnExit);
	//::CoInitialize(NULL);
	//CPaintManagerUI::SetInstance(AfxGetInstanceHandle());
	//CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skin"));

	//m_pDuiFrameWnd = new CDuiFrameWnd(_T("DUI.xml"));
	//m_pDuiFrameWnd->Create(m_hWnd, _T("DUIWnd"), UI_WNDSTYLE_CHILD, 0, 0, 0, 800, 600);
	//m_pDuiFrameWnd->ShowWindow(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}