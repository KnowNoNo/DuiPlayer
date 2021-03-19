// PlayDemoDlg1.cpp : 实现文件
//

#include "stdafx.h"
#include "PlayDemo.h"
#include "PlayDemoDlg1.h"
#include "dui/KDetectMemoryLeak.h"

// CPlayDemoDlg1 对话框

IMPLEMENT_DYNAMIC(CPlayDemoDlg1, CDialog)

CPlayDemoDlg1::CPlayDemoDlg1(CWnd* pParent /*=NULL*/)
	: CDialog(CPlayDemoDlg1::IDD, pParent)
{
	m_pDuiFrameWnd = NULL;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CPlayDemoDlg1::~CPlayDemoDlg1()
{
	delete m_pDuiFrameWnd;
	::CoUninitialize();
}

void CPlayDemoDlg1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPlayDemoDlg1, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CPlayDemoDlg1 消息处理程序

BOOL CPlayDemoDlg1::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetIcon(m_hIcon,TRUE);	// 设置大图标
	SetIcon(m_hIcon,FALSE);	// 小图标
	
	this->SetWindowText(L"PlayDemo");

	//初始化m_pDuiFrameWnd控件信息
	atexit(CheckOnExit);
	::CoInitialize(NULL);
	CPaintManagerUI::SetInstance(AfxGetInstanceHandle());
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skin"));

	m_pDuiFrameWnd = new CDuiFrameWnd(_T("DUI.xml"));
	m_pDuiFrameWnd->Create(m_hWnd, _T("DUIWnd"), UI_WNDSTYLE_CHILD, 0, 0, 0, 800, 600);
	m_pDuiFrameWnd->ShowWindow(TRUE);

	// 根据分辨率自动调节窗口大小
	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTONEAREST), &oMonitor);
	AdaptWindowSize(oMonitor.rcMonitor.right - oMonitor.rcMonitor.left);
	//::GetWindowPlacement(*this, &m_OldWndPlacement);

	

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CPlayDemoDlg1::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if(m_pDuiFrameWnd)
		m_pDuiFrameWnd->SendMessage(WM_SIZE,nType,MAKELPARAM(cx,cy));
}

void CPlayDemoDlg1::AdaptWindowSize(UINT cxScreen)
{
	int iX = 968, iY = 600;
	int iWidthList = 225, iWidthSearchEdit = 193;
	SIZE szFixSearchBtn = {201, 0};

	if(cxScreen <= 1024)      // 800*600  1024*768  
	{
		iX = 775;
		iY = 470;
	} 
	else if(cxScreen <= 1280) // 1152*864  1280*800  1280*960  1280*1024
	{
		iX = 968;
		iY = 600;
	}
	else if(cxScreen <= 1366) // 1360*768 1366*768
	{
		iX = 1058;
		iY = 656;
		iWidthList        += 21;
		iWidthSearchEdit  += 21;
		szFixSearchBtn.cx += 21;
	}
	else                      // 1440*900
	{
		iX = 1224;
		iY = 760;
		iWidthList        += 66;
		iWidthSearchEdit  += 66;
		szFixSearchBtn.cx += 66;
	}

	::SetWindowPos(m_hWnd, NULL, 0, 0, iX, iY, SWP_FRAMECHANGED|SWP_NOZORDER|SWP_NOACTIVATE);
	CenterWindow();
}
