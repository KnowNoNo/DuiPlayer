//// demo.cpp : �������̨Ӧ�ó������ڵ㡣
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
//// Ψһ��Ӧ�ó������
//CWinApp theApp;
//
//using namespace std;
//
//int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
//{
//	int nRetCode = 0;
//
//	// ��ʼ�� MFC ����ʧ��ʱ��ʾ����
//	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
//	{
//		// TODO: ���Ĵ�������Է���������Ҫ
//		_tprintf(_T("����: MFC ��ʼ��ʧ��\n"));
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
