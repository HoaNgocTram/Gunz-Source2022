#pragma once

#include "resource.h"
#include "CDDBDialog.h"
#include "CDDBAniBmp.h"
#include "explorer.h"
#include "ZUpdate.h"
#include "ZFileTransfer.h"

#define	WM_FIND_PATCHINFO				(WM_USER + 1)
#define	WM_UPDATE_COMPLETE				(WM_USER + 2)
#define	WM_UPDATE_ERROR					(WM_USER + 3)

enum STATUSID
{
	STATUS_NONE = 0,
	STATUS_READY,
	STATUS_ANALYSE_PATCHINFO,
	STATUS_CANNOT_FIND_PATCHINFO,
	STATUS_FIND_NEW_FILE,
	STATUS_UPDATING,
	STATUS_UPDATE_FAIL,
	STATUS_UPDATE_COMPLETE,
	STATUS_STARTGAME,
};

enum TIMERID
{
	TIMER_UPDATE = 0,
};

class CNewAppDlg : public CDDBDialog
{
protected:
	bool			m_bReservedQuit;

	char			m_szClientVersion[25];

	char			m_szLauncherVersion[25];

	int				m_nMajorVersion;

	bool			m_bSkipUpdate;

	bool			m_bIsAlpha;

	bool			m_bNavigationComplete;

	CExplorer		m_cExplorer;
	CDDBAniBmp		m_cAnimation;

public:
	bool			m_bUpdateComplete;

	CDDBButton		m_cStartGame;
	CDDBButton		m_cStop;
	int				m_nStatus;

	ZUpdate			m_Update;

	bool			m_bUpdateCanceled;

	CWinThread*		m_pThreadUpdate;
	bool			m_bThreadContinue;

public:
	CNewAppDlg(CWnd* pParent = NULL);
	bool IsOpen(char* ip, int port);

	void UpdateStartUI();

	void UpdateWebLoading();

	void UpdateInfo();

	void OnDestroyDialog();

	void StartGame();

	void GetRunInfo();

	void RunExternalUpdater();

	void GetFileVersion(const char* pszFileName, char* pszVersion);

	int GetOSVersion(char* pszBuff);

	void DocumentCompleteExplorer(LPDISPATCH pDisp, VARIANT* URL);

	bool ShowLicenseDlg();

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBnClickedStartgame();
	afx_msg void OnBnClickedStop();
	afx_msg LRESULT OnFindPatchInfo(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateComplete(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateError(WPARAM wParam, LPARAM lParam);
	afx_msg void OnAbout();
	afx_msg void OnOK();
	afx_msg void OnCancel();
	afx_msg void OnQuit();
	DECLARE_MESSAGE_MAP()

	DECLARE_EVENTSINK_MAP()
};
