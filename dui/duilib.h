
#pragma once
#include <E:\duilib1\duilib\UIlib.h>
using namespace DuiLib;

#ifdef _DEBUG
#   ifdef _UNICODE
#       pragma comment(lib, "DuiLib_ud.lib")
#   else
#       pragma comment(lib, "DuiLib_d.lib")
#   endif
#else
#   ifdef _UNICODE
#       pragma comment(lib, "DuiLib_u.lib")
#   else
#       pragma comment(lib, "DuiLib.lib")
#   endif
#endif


// 以XML生成界面的窗口基类
class CXMLWnd : public WindowImplBase
{
public:
	explicit CXMLWnd(LPCTSTR pszXMLName) 
		: m_strXMLName(pszXMLName){}

public:
	virtual LPCTSTR GetWindowClassName() const
	{
		return _T("XMLWnd");
	}

	virtual CDuiString GetSkinFile()
	{
		return m_strXMLName;
	}

	virtual CDuiString GetSkinFolder()
	{
		return _T("");
	}

protected:
	CDuiString m_strXMLName;    // XML的名字
};


// 将HWND显示到CControlUI上面
class CWndUI: public CControlUI
{
public:
	CWndUI(): m_hWnd(NULL){}

	virtual void SetVisible(bool bVisible = true)
	{
		__super::SetVisible(bVisible);
		::ShowWindow(m_hWnd, bVisible);
	}

	virtual void SetInternVisible(bool bVisible = true)
	{
		__super::SetInternVisible(bVisible);
		::ShowWindow(m_hWnd, bVisible);
	}

	virtual void SetPos(RECT rc)
	{
		__super::SetPos(rc);
		::SetWindowPos(m_hWnd, NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOACTIVATE);
		//::SetWindowPos(m_hWndDlg, NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER | SWP_NOACTIVATE);
	}

	BOOL Attach(HWND hWndNew,HWND hWndNewDlg = NULL)
	{
		if (! ::IsWindow(hWndNew) || (hWndNewDlg && ! ::IsWindow(hWndNewDlg)) )
		{
			return FALSE;
		}

		m_hWnd = hWndNew;
		m_hWndDlg = hWndNewDlg;
		return TRUE;
	}

	HWND Detach()
	{
		HWND hWnd = m_hWnd;
		m_hWnd = NULL;
		m_hWndDlg = NULL;
		return hWnd;
	}

	HWND GetHWND()
	{
		return m_hWnd;
	}

protected:
	HWND m_hWnd;
	HWND m_hWndDlg;// 窗口播放用 一般不用此句柄
};
