#pragma once
#include "duilib.h"
#include "PlayList.h"
#include "../DisplayWnd.h"
#include "../Player.h"
#include <deque>
enum enum_PlayMode
{
	EM_PLAY_MODE_SEQUENCE,      // 顺序播放
	EM_PLAY_MODE_RANDOM,        // 随机播放
	EM_PLAY_MODE_SINGLE_CIRCLE  // 单曲循环
};

class CDuiFrameWnd: public CXMLWnd
{
public:
	explicit CDuiFrameWnd(LPCTSTR pszXMLName);
	~CDuiFrameWnd();

	DUI_DECLARE_MESSAGE_MAP()
	virtual void InitWindow();
	virtual CControlUI* CreateControl(LPCTSTR pstrClassName);
	virtual void Notify(TNotifyUI& msg);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void OnClick(TNotifyUI& msg);
	virtual LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT ResponseDefaultKeyEvent(WPARAM wParam); 

	void OnDropFiles(HWND hwnd, HDROP hDropInfo);
	void OnDisplayChange(HWND hwnd, UINT bitsPerPixel, UINT cxScreen, UINT cyScreen);
	void OnGetMinMaxInfo(HWND hwnd, LPMINMAXINFO lpMinMaxInfo);
	void OnSize(HWND hwnd,  UINT nType, int cx, int cy);

	LRESULT OnPlaying(HWND hwnd, WPARAM wParam, LPARAM lParam);     // 文件头读取完毕，开始播放
	LRESULT OnPosChanged(HWND hwnd, WPARAM wParam, LPARAM lParam);  // 进度改变，播放器传回来的进度
	LRESULT OnEndReached(HWND hwnd, WPARAM wParam, LPARAM lParam);  // 文件播放完毕
	bool    OnPosChanged(void* param);                              // 进度改变，用户主动改变进度
	bool    OnVolumeChanged(void* param);                           // 音量改变

	void Play(LPCTSTR pszPath);                                     // 播放路径为pszPath的文件
	void Play(bool bPlay);                                          // 播放或暂停
	void Stop();                                                    // 停止                                        // 打开文件窗口
	void FastForward();										
	void FastBackward();
	void ShowPlaylist(bool bShow);                                  // 显示播放列表
	CDuiString GetNextPath(bool bNext);                             // 获取下一个播放路径，bNext为true代表下一个，为false代表上一个
	void AddFiles(const std::vector<string_t> &vctString, bool bInit = false);    // 添加文件到播放列表
	void AddHistoryFiles(const std::vector<string_t> &vctString);	// 添加文件到历史列表,注意文件TREEVIEWUI 不能插入到最后一部分
	void DeleteFile();												// 删除播放列表当前所选项
private:
	CStatic			*m_pStatic;			// mfc控件, static作为视频画面
	//CDlgOpenFile	m_dlgOpenFile;		// mfc控件, openfile窗口
	CDisplayWnd     m_dlgDisplay;		// MFC控件，播放视频窗口
	int				m_DisplayAngle;
	CDuiString      m_strPath;          // 当前文件的路径
	CSliderUI       *m_pSliderPlay;     // 文件播放进度
	CLabelUI        *m_pLabelTime;      // 文件播放时间
	WINDOWPLACEMENT m_OldWndPlacement;  // 保存窗口原来的位置
	bool            m_bFullScreenMode;  // 是否在全屏模式
	int             m_iPlaylistIndex;   // 当前播放文件在tree控件里的下标  treeview控件目前很不完善，所以用m_iPlaylistIndex来辅助标记
	CPlaylist       m_cPlayList;        // 存储播放列表的路径
	enum_PlayMode   m_emPlayMode;       // 播放模式
	std::deque<unsigned>   m_queRand;   // 随机播放模式的文件序号

	void ReadConfig(LPCTSTR pszPath);
	void WriteConfig(LPCTSTR pszPath);

	void ShowPlayButton(bool bShow);                                // 显示播放按钮
	void ShowPlayWnd(bool bShow);                                   // 显示播放窗口
	void ShowControlsForPlay(bool bShow);                           // 当开始播放或停止时，显示或隐藏一些控件

	void AdaptWindowSize(UINT cxScreen);                            // 根据屏幕分辨率自动调整窗口大小
	void FullScreen(bool bFull);                                    // 全屏
	void GetPlaylistInfo(int &iIndexTreeStart, int &iFileCount);    // 获取播放列表在Tree控件中的起始位置(如果没有文件，则返回-1),以及文件数量
	int  GetNodePlaylistIndex(int iIndexTree);						// 根据Tree控件的下标计算在Playlist中的下标
	int  GetTreePlaylistIndex(int iIndexNode);						// 根据Node控件的下标计算在Playlist中的下标
	////
	BOOL OpenFile(CString strPath);									// 打开并加载dav文件
	void CloseFile();												// 关闭dav文件											
	void OpenFileDialog();  
	int  MediaInit();
	void ChangeAngle(int type);
	int  OpenAudio();
	void SetThrowMode(UINT ID);
	BOOL PlayerOperator(PLAY_STATE uID);
	void ChangeUIState(PLAY_STATE nState);
};