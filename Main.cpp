//// demo.cpp : 定义控制台应用程序的入口点。
////
#pragma once
#include "StdAfx.h"
//#include "resource.h"
//#include "dui/DuiFrameWnd.h"
//#include "dui/KDetectMemoryLeak.h"
//
//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif
//
//// 唯一的应用程序对象
//CWinApp theApp;
//
//using namespace std;
//
//int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
//{
//	int nRetCode = 0;
//
//	// 初始化 MFC 并在失败时显示错误
//	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
//	{
//		// TODO: 更改错误代码以符合您的需要
//		_tprintf(_T("错误: MFC 初始化失败\n"));
//		nRetCode = 1;
//	}
//
//	//
//	atexit(CheckOnExit);
//	::CoInitialize(NULL);
//	CPaintManagerUI::SetInstance(hInstance);
//	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skin"));
//
//	CDuiFrameWnd *pFrame = new CDuiFrameWnd(_T("DUI.xml"));
//	pFrame->Create(NULL, _T("hh_duiplayer"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE | WS_EX_ACCEPTFILES);
//	pFrame->ShowModal();
//
//	delete pFrame;
//	::CoUninitialize();
//	return nRetCode;
//}
