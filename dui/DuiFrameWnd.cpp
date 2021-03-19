#include "StdAfx.h"
#include "DuiFrameWnd.h"
#include <windowsx.h>
#include <algorithm>
#include <time.h>
#include <comutil.h>
#include "../Player.h"
#pragma comment(lib, "comsupp.lib")
#include "MenuWnd.h"
#include "../resource.h"
const TCHAR STR_PATH_PLAYLIST[] = _T("Playlist.m3u");
const TCHAR STR_PATH_CONFIG[]   = _T("XMP.cfg");

// �ļ�ɸѡ
const TCHAR STR_FILE_FILTER[] =
_T("All Files(*.*)\0*.*\0")
_T("Movie Files(*.rmvb,*.mpeg,etc)\0*.rm;*.rmvb;*.flv;*.f4v;*.avi;*.3gp;*.mp4;*.wmv;*.mpeg;*.mpga;*.asf;*.dat;*.mov;*.dv;*.mkv;*.mpg;*.trp;*.ts;*.vob;*.xv;*.m4v;*.dpg;\0")
_T("Music Files(*.mp3,*.wma,etc)\0*.mp3;*.wma;*.wav;*.mid;*.rmi;*.aac;*.ac3;*.aiff;*.amr;*.m4a;*.mka;*.mp2;*.ogg;*.ra;*.au;*.flac;\0");

// �ļ����� 
// ����STR_FILE_FILTER��\0�����Բ������ڲ��ң������\0����|
const TCHAR STR_FILE_MOVIE[] =
_T("Movie Files(*.rmvb,*.mpeg,etc)|*.rm;*.rmvb;*.flv;*.f4v;*.avi;*.3gp;*.mp4;*.wmv;*.mpeg;*.mpga;*.asf;*.dat;*.mov;*.dv;*.mkv;*.mpg;*.trp;*.ts;*.vob;*.xv;*.m4v;*.dpg;|");

const TCHAR STR_FILE_MUSIC[] =
_T("Music Files(*.mp3,*.wma,etc)|*.mp3;*.wma;*.wav;*.mid;*.rmi;*.aac;*.ac3;*.aiff;*.amr;*.m4a;*.mka;*.mp2;*.ogg;*.ra;*.au;*.flac;|");

const TCHAR STR_FILE_DAV[] =
_T("DAV Files(*.dav)|*.dav;|");

// TreeView�ؼ������б���ļ�·���ڵ㣬��tag������ΪU_TAG_PLAYLIST
const UINT_PTR U_TAG_PLAYLIST = 1; 

#define WM_USER_PLAYING         WM_USER + 1     // ��ʼ�����ļ�
#define WM_USER_POS_CHANGED     WM_USER + 2     // �ļ�����λ�øı�
#define WM_USER_END_REACHED     WM_USER + 3     // �������


bool FindFileExt(LPCTSTR pstrPath, LPCTSTR pstrExtFilter)
{
	if (! pstrPath || ! pstrExtFilter)
	{
		return false;
	}

	TCHAR szExt[_MAX_EXT] = _T("");

	_tsplitpath_s(pstrPath, NULL, 0, NULL, 0, NULL, 0, szExt, _MAX_EXT);
	_tcslwr_s(szExt, _MAX_EXT);

	if(_tcslen(szExt))  
	{
		_tcscat_s(szExt, _MAX_EXT, _T(";"));    // .mo���������������ڻ�ƥ�䵽.mov�������ں�����ϡ�;�����ж��Ƿ���ȫƥ��
		return NULL != _tcsstr(pstrExtFilter, szExt);
	}

	return false;
}

bool IsMusicFile(LPCTSTR pstrPath)
{
	return FindFileExt(pstrPath, STR_FILE_MUSIC);
}

bool IsMovieFile(LPCTSTR pstrPath)
{
	return FindFileExt(pstrPath, STR_FILE_MOVIE);
}

bool IsDavFile(LPCTSTR pstrPath)
{
	return FindFileExt(pstrPath, STR_FILE_DAV);
}

bool IsWantedFile(LPCTSTR pstrPath)
{
	return IsDavFile(pstrPath);
}

void EnumerateFiles(std::vector<string_t> &vctString)
{
	WIN32_FIND_DATA fd;
	HANDLE hFind = ::FindFirstFile(_T("*.*"), &fd);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do 
		{
			// ���ΪĿ¼
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (_tcscmp(fd.cFileName, _T(".")) && _tcscmp(fd.cFileName, _T("..")))
				{
					::SetCurrentDirectory(fd.cFileName);    
					EnumerateFiles(vctString);              
					::SetCurrentDirectory(_T(".."));        
				}
			}
			// ���Ϊ�ļ�
			else
			{
				CDuiString strDir;
				TCHAR      lpDir[MAX_PATH];

				::GetCurrentDirectory(MAX_PATH, lpDir);
				strDir = lpDir;
				if ( strDir.Right(1) != _T("\\") )
				{
					strDir += _T("\\");
				}
				strDir += fd.cFileName;

				vctString.push_back(strDir.GetData());
			}
		} while (::FindNextFile(hFind, &fd));

		::FindClose(hFind);
	}
}

// ����uRandNum������ͬ�������������ӵ�queRandĩβ
void Rand(std::deque<unsigned int> &queRand, unsigned int uRandNum)
{
	if (uRandNum <= 0)
	{
		return;
	}

	unsigned uSizeOld = queRand.size();
	unsigned uSizeNew = uSizeOld + uRandNum;
	queRand.resize(uSizeNew);
	srand(unsigned(time(NULL)));

	for(unsigned i = uSizeOld; i < uSizeNew; i++)
	{
		queRand[i] = i;
	}

	for(unsigned i = uSizeOld; i < uSizeNew; i++)
	{
		std::swap(queRand[i], queRand[rand() % uSizeNew]);
	}
}

void CallbackPlayer(void *data, UINT uMsg)
{
	CDisplayWnd *pAVPlayer = (CDisplayWnd *) data;

	if (pAVPlayer)
	{
		HWND hWnd = pAVPlayer->GetSafeHwnd();

		if (::IsWindow(hWnd) && ::IsWindow(::GetParent(hWnd)))
		{
			::PostMessage(::GetParent(hWnd), uMsg, (WPARAM)data, 0);
		}
	}
}

void CallbackPlaying(void *data)
{
	CallbackPlayer(data, WM_USER_PLAYING);
}

void CallbackPosChanged(void *data)
{
	CallbackPlayer(data, WM_USER_POS_CHANGED);
}

void CallbackEndReached(void *data)
{
	CallbackPlayer(data, WM_USER_END_REACHED);
}

std::string UnicodeConvert(const std::wstring& strWide, UINT uCodePage)
{
	std::string strANSI;
	int iLen = ::WideCharToMultiByte(uCodePage, 0, strWide.c_str(), -1, NULL, 0, NULL, NULL);

	if (iLen > 1)
	{ 
		strANSI.resize(iLen-1);
		::WideCharToMultiByte(uCodePage, 0, strWide.c_str(), -1, &strANSI[0], iLen, NULL, NULL);
	}

	return strANSI;
}

std::string UnicodeToUTF8(const std::wstring& strWide)
{
	return UnicodeConvert(strWide, CP_UTF8);
}

std::string UnicodeToANSI(const std::wstring& strWide)
{
	return UnicodeConvert(strWide, CP_ACP);
}


CDuiFrameWnd::CDuiFrameWnd( LPCTSTR pszXMLName )
: CXMLWnd(pszXMLName),
m_pStatic(NULL),
m_DisplayAngle(NULL),
m_pSliderPlay(NULL),
m_bFullScreenMode(FALSE),
m_iPlaylistIndex(-1),
m_emPlayMode(EM_PLAY_MODE_SEQUENCE)
{
	ReadConfig(CPaintManagerUI::GetInstancePath() + STR_PATH_CONFIG);
	m_cPlayList.ReadFile((CPaintManagerUI::GetInstancePath() + STR_PATH_PLAYLIST).GetData());
}

CDuiFrameWnd::~CDuiFrameWnd()
{
	WriteConfig(CPaintManagerUI::GetInstancePath() + STR_PATH_CONFIG);
	m_cPlayList.WriteFile((CPaintManagerUI::GetInstancePath() + STR_PATH_PLAYLIST).GetData());
	CloseFile();
}

DUI_BEGIN_MESSAGE_MAP(CDuiFrameWnd, CNotifyPump)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK,OnClick)
DUI_END_MESSAGE_MAP()

void CDuiFrameWnd::InitWindow()
{
	//SetIcon(IDR_MAINFRAME);

	// ���ݷֱ����Զ����ڴ��ڴ�С
	//MONITORINFO oMonitor = {};
	//oMonitor.cbSize = sizeof(oMonitor);
	//::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTONEAREST), &oMonitor);
	//AdaptWindowSize(oMonitor.rcMonitor.right - oMonitor.rcMonitor.left);
	//::GetWindowPlacement(*this, &m_OldWndPlacement);

	// ��ʼ��CActiveXUI�ؼ�
	std::vector<CDuiString> vctName;
	CActiveXUI* pActiveXUI;

	vctName.push_back(_T("ActiveXCloud"));

	for (UINT i = 0; i < vctName.size(); i++)
	{
		pActiveXUI = static_cast<CActiveXUI*>(m_PaintManager.FindControl(vctName[i]));

		if(pActiveXUI) 
		{
			pActiveXUI->SetDelayCreate(false);                     
			pActiveXUI->CreateControl(CLSID_WebBrowser);    
		}
	}

	// �������ÿؼ���Ϊ��Ա����
	CSliderUI* pSilderVol = static_cast<CSliderUI*>(m_PaintManager.FindControl(_T("sliderVol")));
	m_pSliderPlay = static_cast<CSliderUI*>(m_PaintManager.FindControl(_T("sliderPlay")));
	m_pLabelTime  = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("labelPlayTime")));

	if (! pSilderVol || ! m_pSliderPlay || ! m_pLabelTime)
	{
		return;
	}

	pSilderVol->OnNotify    += MakeDelegate(this, &CDuiFrameWnd::OnVolumeChanged);
	m_pSliderPlay->OnNotify += MakeDelegate(this, &CDuiFrameWnd::OnPosChanged);

	// ����m3u�����б�
	AddFiles(m_cPlayList.GetPlaylist(), true);   

	// ѡ����һ�β����ļ���λ��
	CTreeViewUI *pTree = static_cast<CTreeViewUI*>(m_PaintManager.FindControl(_T("treePlaylist")));
	if (pTree)
	{
		pTree->SelectItem(m_iPlaylistIndex, true);
	}

	//	����Ƶ����
	m_dlgDisplay.ShowWindow(SW_SHOW);
}

CControlUI* CDuiFrameWnd::CreateControl( LPCTSTR pstrClassName )
{
	CDuiString     strXML;
	CDialogBuilder builder;

	if (_tcsicmp(pstrClassName, _T("Caption")) == 0)
	{
		strXML = _T("Caption.xml");
	}
	else if (_tcsicmp(pstrClassName, _T("PlayPanel")) == 0)
	{
		strXML = _T("PlayPanel.xml");
	}
	else if (_tcsicmp(pstrClassName, _T("Playlist")) == 0)
	{
		strXML = _T("Playlist.xml");
	}
	else if (_tcsicmp(pstrClassName, _T("WndMediaDisplay")) == 0)
	{
		CWndUI *pUI = new CWndUI;   
		//HWND   hWnd = CreateWindow(_T("Static"), _T("WndMediaDisplay"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, m_PaintManager.GetPaintWindow(), (HMENU)0, NULL, NULL);
		m_pStatic = new CStatic;
		m_pStatic->Create(L"WndMediaDisplay" , WS_VISIBLE|WS_CHILD|SS_BITMAP|SS_CENTERIMAGE, CRect(0,0,600,600),CWnd::FromHandle(m_PaintManager.GetPaintWindow()),0);
		//������Ƶ����
		m_dlgDisplay.Create(IDD_DIALOG_DISPLAY, m_pStatic);
		pUI->Attach(m_pStatic->GetSafeHwnd(),m_dlgDisplay.GetSafeHwnd());  
		return pUI;
	}

	if (! strXML.IsEmpty())
	{
		CControlUI* pUI = builder.Create(strXML.GetData(), NULL, NULL, &m_PaintManager, NULL); // ������봫��m_PaintManager����Ȼ��XML����ʹ��Ĭ�Ϲ���������Ϣ��
		return pUI;
	}

	return NULL;
}

void CDuiFrameWnd::OnClick( TNotifyUI& msg )
{
	if( msg.pSender->GetName() == _T("btnPlaylistShow") ) 
	{
		ShowPlaylist(true);
	}
	else if( msg.pSender->GetName() == _T("btnPlaylistHide") ) 
	{
		ShowPlaylist(false);
	}
	else if( msg.pSender->GetName() == _T("btnFastBackward") ) 
	{
		FastBackward();
	}
	else if( msg.pSender->GetName() == _T("btnFastForward") ) 
	{
		FastForward();
	}
	else if( msg.pSender->GetName() == _T("btnPrevious") ) 
	{
		Play(GetNextPath(false));
	}
	else if( msg.pSender->GetName() == _T("btnNext") ) 
	{
		Play(GetNextPath(true));
	}
	else if( msg.pSender->GetName() == _T("btnPlay") ) 
	{
		Play(true);
	}
	else if( msg.pSender->GetName() == _T("btnPause") ) 
	{
		Play(false);
	}
	else if( msg.pSender->GetName() == _T("btnStop") ) 
	{
		Stop();
	}
	else if( msg.pSender->GetName() == _T("btnOpen") || msg.pSender->GetName() == _T("btnOpenMini") /*|| msg.pSender->GetName() == _T("tabPlay")*/) 
	{
		OpenFileDialog(); 
	}
	else if( msg.pSender->GetName() == _T("btnRefresh") ) 
	{
		CEditUI* pUI = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editURL")));
		Play(pUI->GetText());
	}
	else if( msg.pSender->GetName() == _T("btnScreenFull") ) 
	{
		FullScreen(true);
	}
	else if( msg.pSender->GetName() == _T("btnScreenNormal") ) 
	{
		FullScreen(false);
	}
	else if( msg.pSender->GetName() == _T("btnVolume") ) 
	{
		CPlayer::Instance()->SetAudioVolume(0);
		m_PaintManager.FindControl(_T("btnVolumeZero"))->SetVisible(true);
		msg.pSender->SetVisible(false);
	}
	else if( msg.pSender->GetName() == _T("btnVolumeZero") ) 
	{
		CSliderUI* pUI = static_cast<CSliderUI*>(m_PaintManager.FindControl(_T("sliderVol")));
		CPlayer::Instance()->SetAudioVolume(pUI->GetValue());
		m_PaintManager.FindControl(_T("btnVolume"))->SetVisible(true);
		msg.pSender->SetVisible(false);
	}
	else if( msg.pSender->GetName() == _T("btnPlayMode") ) 
	{
		CMenuWnd *pMenu = new CMenuWnd(_T("menu.xml"));
		POINT    pt = {msg.ptMouse.x, msg.ptMouse.y};
		CDuiRect rc = msg.pSender->GetPos();

		pt.x = rc.left;
		pt.y = rc.bottom;
		pMenu->Init(&m_PaintManager, pt);
		pMenu->ShowWindow(TRUE);
	} if( msg.pSender->GetName() == _T("btnAdd") )
	{
		OpenFileDialog();
	}
	else if(msg.pSender->GetName() == _T("btnDelete"))
	{
		DeleteFile();
	}
	else {}//add btn ...

	__super::OnClick(msg);
}

void CDuiFrameWnd::Notify( TNotifyUI& msg )
{
	if( msg.sType == DUI_MSGTYPE_ITEMACTIVATE )   
	{
		CTreeViewUI* pTree = static_cast<CTreeViewUI*>(m_PaintManager.FindControl(_T("treePlaylist")));

		if(pTree && -1 != pTree->GetItemIndex(msg.pSender) && U_TAG_PLAYLIST == msg.pSender->GetTag())
		{
			m_iPlaylistIndex = pTree->GetItemIndex(msg.pSender);
			Play(m_cPlayList.GetPlaylist(GetNodePlaylistIndex(m_iPlaylistIndex)).c_str());  //(static_cast<CTreeNodeUI*> (msg.pSender))->GetItemText();
		}
	}
	else if(msg.sType == DUI_MSGTYPE_SELECTCHANGED)
	{
		CDuiString    strName = msg.pSender->GetName();
		CTabLayoutUI* pTab    = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("tabCaption")));
		std::vector<CDuiString> vctString;
		
		vctString.push_back(_T("tabPlay"));
		vctString.push_back(_T("tabCloud"));

		std::vector<CDuiString>::iterator it = std::find(vctString.begin(), vctString.end(), strName);
		if (vctString.end() != it)
		{
			int iIndex = it - vctString.begin();
			pTab->SelectItem(iIndex);

			// ������ҳ
			// ���ڼ�����ҳ��ĺܶ��ڴ棬��������ѡ��̬����
			if (iIndex > 0)
			{
				std::vector<CDuiString> vctName, vctURL;
				CActiveXUI* pActiveXUI;

				vctName.push_back(_T("ActiveXCloud"));

				vctURL.push_back(_T("http://www.hhkj.cn/"));

				iIndex--;
				pActiveXUI = static_cast<CActiveXUI*>(m_PaintManager.FindControl(vctName[iIndex]));

				if(pActiveXUI) 
				{
					IWebBrowser2* pWebBrowser = NULL;
					pActiveXUI->GetControl(IID_IWebBrowser2, (void**)&pWebBrowser);

					if(pWebBrowser) 
					{
						_bstr_t bstrTmp;
						BSTR    bstr;

						pWebBrowser->get_LocationURL(&bstr);
						bstrTmp.Attach(bstr);

						if (! bstrTmp.length())
						{
							pWebBrowser->Navigate(_bstr_t(vctURL[iIndex]), NULL,NULL,NULL,NULL);
							pWebBrowser->Release();
						}
					}
				}
			}
		}
	}
	else if(msg.sType == DUI_MSGTYPE_ITEMCLICK)
	{
		CDuiString strName = msg.pSender->GetName();
		CLabelUI*  pLabelPlayMode = static_cast<CLabelUI*>(m_PaintManager.FindControl(L"labPlayMode"));

		if (strName == _T("menuSequence"))
		{
			m_emPlayMode = EM_PLAY_MODE_SEQUENCE;
			if(pLabelPlayMode)pLabelPlayMode->SetText(L"˳�򲥷�");
		} 
		else if (strName == _T("menuRandom"))
		{
			m_emPlayMode = EM_PLAY_MODE_RANDOM;
			if(pLabelPlayMode)pLabelPlayMode->SetText(L"�������");
		}
		else if (strName == _T("menuSingleCircle"))
		{
			m_emPlayMode = EM_PLAY_MODE_SINGLE_CIRCLE;
			if(pLabelPlayMode)pLabelPlayMode->SetText(L"��������");
		}
	}
	else if( msg.sType == DUI_MSGTYPE_DBCLICK )   
	{
		if (IsInStaticControl(msg.pSender))
		{
			// ����ᴫ�����ܶ��˫����Ϣ������ֻ��ȡ���ȿؼ�����Ϣ
			if (! msg.pSender->GetParent())
			{
				FullScreen(! m_bFullScreenMode);
			}
		}
	}

	__super::Notify(msg);
}

LRESULT CDuiFrameWnd::HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LRESULT lRes = __super::HandleMessage(uMsg, wParam, lParam);

	switch (uMsg)
	{
		HANDLE_MSG (*this, WM_DROPFILES, OnDropFiles);
		HANDLE_MSG (*this, WM_DISPLAYCHANGE, OnDisplayChange);
		HANDLE_MSG (*this, WM_GETMINMAXINFO, OnGetMinMaxInfo);
		HANDLE_MSG (*this, WM_SIZE, OnSize);

	case WM_USER_PLAYING:
		return OnPlaying(*this, wParam, lParam);
	case WM_USER_POS_CHANGED:
		return OnPosChanged(*this, wParam, lParam);
	case WM_USER_END_REACHED:
		return OnEndReached(*this, wParam, lParam);    
	}

	return lRes;
}

LRESULT CDuiFrameWnd::OnNcHitTest( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	//if (m_bFullScreenMode)
	//{
		return HTCLIENT;//�ͻ�������Ӧ����¼�
	//}

	//return __super::OnNcHitTest(uMsg, wParam, lParam, bHandled);
}

LRESULT CDuiFrameWnd::ResponseDefaultKeyEvent(WPARAM wParam)
{
	if (wParam == VK_ESCAPE)
	{
		if (m_bFullScreenMode)
		{
			FullScreen(false);
		}
	}

	return __super::ResponseDefaultKeyEvent(wParam);
}

void CDuiFrameWnd::AdaptWindowSize( UINT cxScreen )
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

	CControlUI *pctnPlaylist = m_PaintManager.FindControl(_T("ctnPlaylist"));
	CControlUI *peditSearch  = m_PaintManager.FindControl(_T("editSearch"));
	CControlUI *pbtnSearch   = m_PaintManager.FindControl(_T("btnSearch"));
	if (pctnPlaylist && peditSearch && pbtnSearch)
	{
		pctnPlaylist->SetFixedWidth(iWidthList);
		peditSearch->SetFixedWidth(iWidthSearchEdit);
		pbtnSearch->SetFixedXY(szFixSearchBtn);
	}

	::SetWindowPos(m_PaintManager.GetPaintWindow(), NULL, 0, 0, iX, iY, SWP_FRAMECHANGED|SWP_NOZORDER|SWP_NOACTIVATE);
	CenterWindow();
}

void CDuiFrameWnd::OnDisplayChange( HWND hwnd, UINT bitsPerPixel, UINT cxScreen, UINT cyScreen )
{
	AdaptWindowSize(cxScreen);
}

void CDuiFrameWnd::OnGetMinMaxInfo( HWND hwnd, LPMINMAXINFO lpMinMaxInfo )
{     
	if (m_bFullScreenMode)
	{
		lpMinMaxInfo->ptMaxTrackSize.x = GetSystemMetrics(SM_CXSCREEN) + 2 * (GetSystemMetrics(SM_CXFIXEDFRAME) + GetSystemMetrics(SM_CXBORDER));
		lpMinMaxInfo->ptMaxTrackSize.y = GetSystemMetrics(SM_CYSCREEN) + 2 * (GetSystemMetrics(SM_CYFIXEDFRAME) + GetSystemMetrics(SM_CYBORDER));
	}
}

void CDuiFrameWnd::OnDropFiles( HWND hwnd, HDROP hDropInfo )
{
	UINT  nFileCount = ::DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);
	TCHAR szFileName[_MAX_PATH] = _T("");
	DWORD dwAttribute;
	std::vector<string_t> vctString;

	// ��ȡ��ק�����ļ����ļ���
	for (UINT i = 0; i < nFileCount; i++)
	{
		::DragQueryFile(hDropInfo, i, szFileName, sizeof(szFileName));
		dwAttribute = ::GetFileAttributes(szFileName);

		// �Ƿ�Ϊ�ļ���
		if ( dwAttribute & FILE_ATTRIBUTE_DIRECTORY )
		{          
			::SetCurrentDirectory(szFileName);       
			EnumerateFiles(vctString);	      
		}
		else
		{
			vctString.push_back(szFileName);
		}
	}

	AddFiles(vctString);
	::DragFinish(hDropInfo);
}

void CDuiFrameWnd::ShowPlaylist( bool bShow )
{
	CControlUI *pctnPlaylist = m_PaintManager.FindControl(_T("ctnPlaylist"));
	CControlUI *pbtnHide     = m_PaintManager.FindControl(_T("btnPlaylistHide"));
	CControlUI *pbtnShow     = m_PaintManager.FindControl(_T("btnPlaylistShow"));

	if (pctnPlaylist && pbtnHide && pbtnShow)
	{
		pctnPlaylist->SetVisible(bShow);
		pbtnHide->SetVisible(bShow);
		pbtnShow->SetVisible(! bShow);
	}
}

void CDuiFrameWnd::ShowPlayButton( bool bShow )
{
	CControlUI *pbtnPlay  = m_PaintManager.FindControl(_T("btnPlay"));
	CControlUI *pbtnPause = m_PaintManager.FindControl(_T("btnPause"));

	if (pbtnPlay && pbtnPause)
	{
		pbtnPlay->SetVisible(bShow);
		pbtnPause->SetVisible(! bShow);
	}
}

void CDuiFrameWnd::ShowPlayWnd( bool bShow )
{
	CControlUI *pbtnWnd     = m_PaintManager.FindControl(_T("wndMedia"));
	CControlUI *pbtnStop    = m_PaintManager.FindControl(_T("btnStop"));
	CControlUI *pbtnScreen  = m_PaintManager.FindControl(_T("btnScreenFull"));
	CControlUI *pctnURL     = m_PaintManager.FindControl(_T("ctnURL"));
	CControlUI *pctnClient  = m_PaintManager.FindControl(_T("ctnClient"));
	CControlUI *pctnMusic   = m_PaintManager.FindControl(_T("ctnMusic"));
	CControlUI *pctnSlider  = m_PaintManager.FindControl(_T("ctnSlider"));

	if (pbtnWnd && pbtnStop && pbtnScreen && pctnURL && pctnClient && pctnMusic && pctnSlider)
	{
		pbtnStop->SetEnabled(bShow);
		pbtnScreen->SetEnabled(bShow);
		pctnURL->SetVisible(! bShow);
		pctnClient->SetVisible(! bShow);
		pctnSlider->SetVisible(bShow);

		// ���ļ�ʱ
		if (bShow)  
		{
			if (IsMusicFile(m_strPath))
			{
				pbtnWnd->SetVisible(! bShow);
				pctnMusic->SetVisible(bShow);
			} 
			else
			{
				pbtnWnd->SetVisible(bShow);
				pctnMusic->SetVisible(! bShow);
			}
		}
		// �ر��ļ�ʱ
		else        
		{
			pctnMusic->SetVisible(false);
			pbtnWnd->SetVisible(false);
		}
	}
}

void CDuiFrameWnd::ShowControlsForPlay( bool bShow )
{
	m_pLabelTime->SetText(_T(""));
	ShowPlayWnd(bShow);
	ShowPlaylist( bShow);
	ShowPlayButton(! bShow);
}

void CDuiFrameWnd::AddFiles( const std::vector<string_t> &vctString, bool bInit )
{
	CTreeNodeUI *pNodeTmp, *pNodePlaylist;
	CTreeViewUI *pTreePlayList;
	CDuiString  strTmp;
	TCHAR       szName[_MAX_FNAME];
	TCHAR       szExt[_MAX_EXT];
	unsigned    i, uWantedCount;
	int			nFileIndex;

	pNodePlaylist = static_cast<CTreeNodeUI*>(m_PaintManager.FindControl(_T("nodePlaylist")));
	pTreePlayList = static_cast<CTreeViewUI*>(m_PaintManager.FindControl(_T("treePlaylist")));

	if (! pNodePlaylist || !pTreePlayList)
	{
		return;
	}

	for(i = 0, uWantedCount = 0; i < vctString.size(); i++)
	{
		if (IsWantedFile(vctString[i].c_str()))
		{
			//�����������ظ�
			if(!bInit && (nFileIndex = m_cPlayList.FindFile(vctString[i].c_str())) >= 0)
			{
				pTreePlayList->SelectItem(GetTreePlaylistIndex(nFileIndex),TRUE);
				continue;
			}
			_tsplitpath_s(vctString[i].c_str(), NULL, 0, NULL, 0, szName, _MAX_FNAME, szExt, _MAX_EXT);
			strTmp.Format(_T("%s%s"), szName, szExt);   // �ļ���

			pNodeTmp = new CTreeNodeUI; 
			pNodeTmp->SetItemTextColor(0xFFC8C6CB);
			pNodeTmp->SetItemHotTextColor(0xFFC8C6CB);
			pNodeTmp->SetSelItemTextColor(0x008080);
			pNodeTmp->SetTag(U_TAG_PLAYLIST);
			pNodeTmp->SetItemText(strTmp);  
			pNodeTmp->SetAttribute(_T("Tooltip"), szName);
			pNodeTmp->SetAttribute(_T("height"), _T("22"));
			pNodeTmp->SetAttribute(_T("inset"), _T("7,0,0,0"));
			pNodeTmp->SetAttribute(_T("itemattr"), _T("valign=\"vcenter\" font=\"4\""));
			pNodeTmp->SetAttribute(_T("folderattr"), _T("width=\"0\" float=\"true\""));
			pNodePlaylist->Add(pNodeTmp);
			uWantedCount++;

			if (! bInit)
			{
				m_cPlayList.Add(vctString[i]);          // ����·��
			}
		}        
	}

	Rand(m_queRand, uWantedCount);
	ShowPlaylist(true);
}

void CDuiFrameWnd::Play( bool bPlay )
{
	if( PlayerOperator(bPlay? PLAY_STATE::Play:PLAY_STATE::Pause) )
	{
		ShowPlayButton(! bPlay);
	}
}

void CDuiFrameWnd::Play( LPCTSTR pszPath )
{
	if(!_tcscmp(pszPath,L"") || !OpenFile(pszPath))
	{	
		//AfxMessageBox(L"���ļ�ʧ��");
		return;
	}

	if(PlayerOperator(PLAY_STATE::Play) )
	{
		//ShowControlsForPlay(false);	
		std::vector<string_t> vctString(1, pszPath);
		AddHistoryFiles(vctString);
		ShowControlsForPlay(true);
	}
}


void CDuiFrameWnd::Stop()
{
	PlayerOperator(PLAY_STATE::Stop);
    ShowControlsForPlay(false);
}	

void CDuiFrameWnd::OpenFileDialog()
{
	CString strFilePath;
	CFileDialog fileDlg(TRUE , NULL , NULL , OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_ALLOWMULTISELECT , STR_FILE_DAV ,CWnd::FromHandle(m_PaintManager.GetPaintWindow()));


	if(IDOK == fileDlg.DoModal())
	{	
		POSITION pos = fileDlg.GetStartPosition();
		while(pos)
		{
			strFilePath = fileDlg.GetNextPathName(pos);
			if(!strFilePath.IsEmpty())
			{
				std::vector<string_t> vctString(1, strFilePath.GetBuffer(0));
				AddFiles(vctString);
			}
				
		}
	}
}

void CDuiFrameWnd::FullScreen( bool bFull )
{
	CControlUI* pbtnFull   = m_PaintManager.FindControl(_T("btnScreenFull"));
	CControlUI* pbtnNormal = m_PaintManager.FindControl(_T("btnScreenNormal"));
	CControlUI* pUICaption = m_PaintManager.FindControl(_T("ctnCaption"));
	int iBorderX = GetSystemMetrics(SM_CXFIXEDFRAME) + GetSystemMetrics(SM_CXBORDER);
	int iBorderY = GetSystemMetrics(SM_CYFIXEDFRAME) + GetSystemMetrics(SM_CYBORDER);

	if (pbtnFull && pbtnNormal && pUICaption)
	{
		m_bFullScreenMode = bFull;

		if (bFull)
		{
			::GetWindowPlacement(*this, &m_OldWndPlacement);

			if (::IsWindow(this->GetHWND()) && ::IsZoomed(*this))
			{
				::ShowWindow(*this, SW_SHOWDEFAULT);
			}

			::SetWindowPos(*this, HWND_TOPMOST, -iBorderX, -iBorderY, GetSystemMetrics(SM_CXSCREEN) + 2 * iBorderX, GetSystemMetrics(SM_CYSCREEN) + 2 * iBorderY, 0);
			ShowPlaylist(false);
		} 
		else
		{
			::SetWindowPlacement(*this, &m_OldWndPlacement);
			::SetWindowPos(*this, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		}

		pbtnNormal->SetVisible(bFull);
		pUICaption->SetVisible(! bFull);
		pbtnFull->SetVisible(! bFull);
	}
}

LRESULT CDuiFrameWnd::OnPlaying(HWND hwnd, WPARAM wParam, LPARAM lParam )
{
	return TRUE;
}

LRESULT CDuiFrameWnd::OnPosChanged(HWND hwnd, WPARAM wParam, LPARAM lParam )
{
	CDuiString  strTime;
	struct tm   tmTotal, tmCurrent;
	time_t      timeTotal   =  CPlayer::Instance()->GetTotalTime() / 1000;
	time_t      timeCurrent =	CPlayer::Instance()->GetPlayedTime() / 1000;
	TCHAR       szTotal[MAX_PATH], szCurrent[MAX_PATH];

	gmtime_s(&tmTotal, &timeTotal);
	gmtime_s(&tmCurrent, &timeCurrent);
	_tcsftime(szTotal,   MAX_PATH, _T("%X"), &tmTotal);
	_tcsftime(szCurrent, MAX_PATH, _T("%X"), &tmCurrent);
	strTime.Format(_T("%s / %s"), szCurrent, szTotal);

	m_pLabelTime->SetText(strTime);
	m_pSliderPlay->SetValue(CPlayer::Instance()->GetProcess());
	return TRUE;
}

LRESULT CDuiFrameWnd::OnEndReached(HWND hwnd, WPARAM wParam, LPARAM lParam )
{
	Play(GetNextPath(true));
	return TRUE;
}

bool CDuiFrameWnd::OnPosChanged( void* param )
{
	TNotifyUI* pMsg = (TNotifyUI*)param;

	if( pMsg->sType == _T("valuechanged") )
	{
		CPlayer::Instance()->Seek((static_cast<CSliderUI*>(pMsg->pSender))->GetValue() + 1);// ��ȡ��ֵ����1���������õ�ֵҲ����1����������+1
	}
	return true;
}

bool CDuiFrameWnd::OnVolumeChanged( void* param )
{
	TNotifyUI* pMsg = (TNotifyUI*)param;

	if( pMsg->sType == _T("valuechanged") )
	{
		CPlayer::Instance()->SetAudioVolume((static_cast<CSliderUI*>(pMsg->pSender))->GetValue());
	}

	return true;
}

void CDuiFrameWnd::GetPlaylistInfo(int &iIndexTreeStart, int &iFileCount)
{
	iIndexTreeStart = -1;
	iFileCount      = 0;

	// ����·���ļ�����ʼ�ڵ㣬ĿǰΪnodePlaylist��3���ӽڵ�
	CTreeNodeUI *pNodeTmp; 
	CTreeNodeUI *pNodePlaylist = static_cast<CTreeNodeUI*>(m_PaintManager.FindControl(_T("nodePlaylist")));
	int iNodeTotal = pNodePlaylist->GetCountChild();
	int i;

	for(i = 0; i < iNodeTotal; i++)
	{
		pNodeTmp = pNodePlaylist->GetChildNode(i);

		if (U_TAG_PLAYLIST == pNodeTmp->GetTag())
		{
			break;
		}
	}

	// δ�ҵ�U_TAG_PLAYLIST���ӽڵ㣬˵��û���ļ�����
	if (i == iNodeTotal)
	{      
		return;
	}

	// ���ҵ���ʼ�ڵ㣬��ʼ����һ���ļ�����һ���ļ�
	CTreeViewUI* pTree = static_cast<CTreeViewUI*>(m_PaintManager.FindControl(_T("treePlaylist")));
	if (pTree)
	{
		iIndexTreeStart = pTree->GetItemIndex(pNodeTmp); 
		iFileCount      = iNodeTotal - i;
	}
}

int CDuiFrameWnd::GetNodePlaylistIndex( int iIndexTree )
{
	int iNodeStart = -1; // ��һ���ļ���tree�±�
	int iFileCount = 0;
	
	GetPlaylistInfo(iNodeStart, iFileCount); 
	
	if ( iFileCount <= 0 || (iIndexTree - iNodeStart) >= iFileCount || (iIndexTree - iNodeStart) < 0)
	{
		return -1;
	}

	return iIndexTree - iNodeStart;
}

int CDuiFrameWnd::GetTreePlaylistIndex( int iIndexNode )
{
	int iNodeStart = -1; // ��һ���ļ���tree�±�
	int iFileCount = 0;

	GetPlaylistInfo(iNodeStart, iFileCount); 

	if ( iFileCount <= iIndexNode)
	{
		return -1;
	}

	return iNodeStart + iIndexNode;
}

DuiLib::CDuiString CDuiFrameWnd::GetNextPath( bool bNext )
{
	int iNodeStart = -1; // ��һ���ļ���tree�±�
	int iFileCount = 0;
	int iIndexPlay = m_iPlaylistIndex;  // ������GetCurSelActivateҲ���Ի�ȡ˫�����±꣬�������˫���˷��ļ����У��ͻ�����һ�У�������Treeֻ�Ǹ����һ��Demo�ο�������Ƕ��ListBox�������ļ�·����

	GetPlaylistInfo(iNodeStart, iFileCount);
	if (iFileCount <= 0)
	{
		return _T("");
	}

	if (-1 == iIndexPlay)
	{
		iIndexPlay = iNodeStart;
	}

	if (EM_PLAY_MODE_RANDOM == m_emPlayMode)
	{
		if (! m_queRand.size())
		{
			Rand(m_queRand, iFileCount);
		}

		iIndexPlay = iNodeStart + m_queRand.front();
		m_queRand.pop_front();
	} 
	else if (EM_PLAY_MODE_SEQUENCE == m_emPlayMode)
	{
		if (bNext)
		{
			iIndexPlay++;

			if (iIndexPlay >= iNodeStart + iFileCount)
			{
				iIndexPlay = iNodeStart;
			} 
		} 
		else
		{
			iIndexPlay--;

			if (iIndexPlay < iNodeStart)
			{
				iIndexPlay = iNodeStart + iFileCount - 1;
			} 
		}
	}

	CTreeViewUI *pTree = static_cast<CTreeViewUI*>(m_PaintManager.FindControl(_T("treePlaylist")));
	if(pTree)
	{
		pTree->SelectItem(iIndexPlay, true);
	}
	m_iPlaylistIndex = iIndexPlay;
	//return static_cast<CTreeNodeUI*>(pTree->GetItemAt(iIndexPlay))->GetItemText();  // �ļ���
	return m_cPlayList.GetPlaylist(iIndexPlay - iNodeStart).c_str();                  // ����·��
}

void CDuiFrameWnd::ReadConfig(LPCTSTR pszPath)
{
	if (! pszPath)
	{
		return; 
	}

	DWORD  dwSize;
	HANDLE hFile = (HANDLE)CreateFile(pszPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); 
	if (hFile == INVALID_HANDLE_VALUE) 
	{ 
		return; 
	} 

	ReadFile(hFile, &m_emPlayMode, sizeof(m_emPlayMode), &dwSize, NULL); 
	ReadFile(hFile, &m_iPlaylistIndex, sizeof(m_iPlaylistIndex), &dwSize, NULL);   
	CloseHandle(hFile);
}

void CDuiFrameWnd::WriteConfig(LPCTSTR pszPath)
{
	if (! pszPath)
	{
		return; 
	}

	DWORD  dwSize;
	HANDLE hFile = (HANDLE)CreateFile(pszPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL); 
	if (hFile == INVALID_HANDLE_VALUE) 
	{ 
		return; 
	} 

	WriteFile(hFile, &m_emPlayMode, sizeof(m_emPlayMode), &dwSize, NULL); 
	WriteFile(hFile, &m_iPlaylistIndex, sizeof(m_iPlaylistIndex), &dwSize, NULL); 
	CloseHandle(hFile);
}

BOOL CDuiFrameWnd::OpenFile(CString strPath)
{
	CloseFile();
	/**/
	CPlayer::Instance()->stateTable[0][5] = 0;

	/*Open player*/
	if( CPlayer::Instance()->Open(CPlayer::TYPE(0), 
		strPath.GetBuffer(0), m_dlgDisplay.m_hWnd, m_PaintManager.GetPaintWindow()) <= 0 )
	{
		CPlayer::Instance()->Close();
		MessageBox(NULL,L"Open file failed!",L"Fail",NULL);
		return false;
	}
	
	//
	CPlayer::Instance()->openTable;

	MediaInit();
	
	SetThrowMode(1);
	
	m_dlgDisplay.OpenFile();
	return TRUE;
}

int CDuiFrameWnd::MediaInit()
{
	//DecodeType type = (DecodeType)m_dlgDecodeEngine.GetDecodeType();
	DecodeType type = DECODE_HW_FAST_D3D11;
	//if(type == DECODE_HW_FAST_D3D11)
		PLAY_SetEngine(0,type,RENDER_NOTSET);
	//else
	//	PLAY_SetEngine(0,type,RENDER_D3D11);

	ChangeAngle(0);
	/*Audio is on by default.*/
	OpenAudio();
	CPlayer::Instance()->SetEngineType(type);
	return 0;
}

void CDuiFrameWnd::ChangeAngle(int type)
{
	if(type == m_DisplayAngle)
		return;
	if(type == 4)
		m_DisplayAngle = 0;
	else if(type == -1)
		m_DisplayAngle = 3;
	else
		m_DisplayAngle = type;
	PLAY_SetRotateAngle(0,m_DisplayAngle);
	
}

int CDuiFrameWnd::OpenAudio()
{
	CPlayer::Instance()->OpenSound(TRUE);

	CPlayer::Instance()->SetAudioWave(50);


	return 1;
}

void CDuiFrameWnd::SetThrowMode(UINT ID)
{
	int nThrowFlags = -1;
	switch(ID)
	{
	case 1:
		nThrowFlags = 0;
		//m_nThrowMode = ID_THROW_NO;
		CPlayer::Instance()->EnableLargePicAdjustment(0); //0Ϊ����֡
		break;
	case 2:
		nThrowFlags = 1;
		//m_nThrowMode = ID_THROW_DEFAULT;
		CPlayer::Instance()->EnableLargePicAdjustment(1); // 1ΪĬ�ϲ��Գ�֡
		break;
	case 3:
		nThrowFlags = 3;
		//m_nThrowMode = ID_THROW_ADAPTION;
		CPlayer::Instance()->EnableLargePicAdjustment(3); //3Ϊ����Ӧ��֡
		break;
	default:
		break;
	}
}

void CDuiFrameWnd::ChangeUIState(PLAY_STATE nState)
{
	CControlUI *pbtnPlay  = m_PaintManager.FindControl(_T("btnPlay"));
	CControlUI *pbtnPause = m_PaintManager.FindControl(_T("btnPause"));
	CControlUI *pbtnStop  = m_PaintManager.FindControl(_T("btnStop"));
	CControlUI *pbtnPrevious = m_PaintManager.FindControl(_T("btnPrevious"));
	CControlUI *pbtnNext  = m_PaintManager.FindControl(_T("btnNext"));
	CControlUI *pbtnFastBackward = m_PaintManager.FindControl(_T("btnFastBackward"));    
	CControlUI *pbtnFastForward  = m_PaintManager.FindControl(_T("btnFastForward"));
	CControlUI *psliderPlay  = m_PaintManager.FindControl(_T("sliderPlay"));

	//if(nState == Init)
	//{
	//	pbtnPlay->SetVisible(false);
	//	pbtnPause->SetVisible(false);
	//	pbtnStop->SetVisible(false);
	//	pbtnPrevious->SetVisible(false);
	//	pbtnNext->SetVisible(false);
	//	pbtnFastBackward->SetVisible(false);
	//	pbtnFastForward->SetVisible(false);
	//	psliderPlay->SetVisible(false);
	//}


}

BOOL CDuiFrameWnd::PlayerOperator(PLAY_STATE uID)
{
	//PLAY_STATE nState = PLAY_STATE(uID);
	
	/*change player status*/
	int a = CPlayer::Instance()->ChangeState(uID);

	if( CPlayer::Instance()->Do() <= 0)
	{
		//ChangeSingleUIState(nState, FALSE);
		return FALSE;
	}
	
	if(uID == PLAY_STATE::Stop)
	{
		CloseFile();
		ShowControlsForPlay(false);
	}

	return TRUE;
	
}

void CDuiFrameWnd::CloseFile()
{
	CPlayer::Instance()->OpenSound(FALSE);
	CPlayer::Instance()->Close();
	if(::IsWindow(m_dlgDisplay.m_hWnd))
		m_dlgDisplay.CloseFile();
}

void CDuiFrameWnd::FastForward()
{
	PlayerOperator(PLAY_STATE::Fast);
}

void CDuiFrameWnd::FastBackward()
{
	PlayerOperator(PLAY_STATE::Slow);
}

void CDuiFrameWnd::DeleteFile()
{
	CTreeViewUI* pTree = static_cast<CTreeViewUI*>(m_PaintManager.FindControl(_T("treePlaylist")));
	CTreeNodeUI* pNodePlaylist = static_cast<CTreeNodeUI*>(m_PaintManager.FindControl(_T("nodePlaylist")));
	int	nTreeNodeIndex,nTreeViewSelIndex;
	if (! pNodePlaylist || !pTree )
	{
		return;
	}
	nTreeViewSelIndex = pTree->GetCurSel();
	//ȷ����ѡ���
	nTreeNodeIndex = GetNodePlaylistIndex(nTreeViewSelIndex);
	if( nTreeNodeIndex < 0)
	{
		TRACE(L"��ѡɾ�����nodeplaylist��");
		return;
	}
	
	//���ɾ���ǵ�ǰ����
	if(m_iPlaylistIndex == nTreeViewSelIndex)
	{
		int* piRetval;
		if( MessageBox(NULL,L"ɾ��Ϊ��ǰ���ţ�ȷ��ɾ��?",L"WARN",MB_YESNO) != IDYES)
			return;

		PlayerOperator(PLAY_STATE::Stop);
		pTree->SelectItem(nTreeViewSelIndex-1);
		m_iPlaylistIndex--;
	}

	CControlUI* pNodeSel =  pTree->GetItemAt(nTreeViewSelIndex);//pNodePlaylist->GetItemAt(nTreeNodeIndex+1);
	pNodePlaylist->Remove(pNodeSel);
	m_cPlayList.Erase(nTreeNodeIndex);
}

void CDuiFrameWnd::AddHistoryFiles(const std::vector<string_t> &vctString)
{
	CTreeNodeUI *pNodeTmp, *pNodeHistorylist;
	CDuiString  strTmp;
	TCHAR       szName[_MAX_FNAME];
	TCHAR       szExt[_MAX_EXT];
	unsigned    i, uNodeCount;

	pNodeHistorylist = static_cast<CTreeNodeUI*>(m_PaintManager.FindControl(_T("nodeHistorylist")));
	if (! pNodeHistorylist)
	{
		return;
	}

	for(i = 0; i < vctString.size(); i++)
	{
		if (_tcscmp(vctString[i].c_str(),L""))
		{
			_tsplitpath_s(vctString[i].c_str(), NULL, 0, NULL, 0, szName, _MAX_FNAME, szExt, _MAX_EXT);
			strTmp.Format(_T("%s%s"), szName, szExt);   // �ļ���

			pNodeTmp = new CTreeNodeUI; 
			pNodeTmp->SetItemTextColor(0xFFC8C6CB);
			pNodeTmp->SetItemHotTextColor(0xFFC8C6CB);
			pNodeTmp->SetSelItemTextColor(0x008080);
			pNodeTmp->SetTag(U_TAG_PLAYLIST+1);
			pNodeTmp->SetItemText(strTmp);  
			pNodeTmp->SetAttribute(_T("height"), _T("22"));
			pNodeTmp->SetAttribute(_T("inset"), _T("7,0,0,0"));
			pNodeTmp->SetAttribute(_T("itemattr"), _T("valign=\"vcenter\" font=\"4\""));
			pNodeTmp->SetAttribute(_T("folderattr"), _T("width=\"0\" float=\"true\""));
			pNodeHistorylist->Add(pNodeTmp);
		}
	}
}

void CDuiFrameWnd::OnSize(HWND hwnd, UINT nType, int cx, int cy)
{
	   ::SetWindowPos(m_PaintManager.GetPaintWindow(), NULL, 0, 0, cx, cy, SWP_FRAMECHANGED|SWP_NOZORDER|SWP_NOACTIVATE);
}
