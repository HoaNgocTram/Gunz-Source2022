#ifndef _INCLUDED_CDDBDLG_
#define _INCLUDED_CDDBDLG_

#include "CDDB.h"
#include "CDDBButton.h"
#include "CSound.h"
#include <afxcmn.h>

#define WM_MOUSEDRAG		(WM_USER + 100)
#define WM_TRAYICON_NOTIFY	(WM_USER + 101)
#define IDC_ABOUTBOX		(WM_USER + 200)
#define IDC_MINIMIZE		(WM_USER + 201)
#define IDC_MAXIMIZE		(WM_USER + 202)
#define IDC_QUIT			(WM_USER + 203)

class CDDBDialog : public CDialog
{
public:
	struct CDDBDLGINFO
	{
		UINT		nWidth;
		UINT		nHeight;
		UINT		IDSkinResource;
		UINT		IDIconResource;
		UINT		IDWaveResource;
		COLORREF	nTextColor;
		COLORREF	nTextBkColor;

		BOOL		bUseTitlebar;
		BOOL		bUseBmpTitle;
		BOOL		bUseIconSystemMenu;

		BOOL		bUseAboutboxButton;
		BOOL		bUseMinimizeButton;
		BOOL		bUseMaximizeButton;
		BOOL		bUseQuitButton;

		BOOL		bEnableMoveDlg;
		BOOL		bEnableDragAndDrop;
		BOOL		bEnableRenderingLayer;

		CDDBDLGINFO()
		{
			nWidth = 320;
			nHeight = 240;
			IDSkinResource = NULL;
			IDIconResource = NULL;
			IDWaveResource = NULL;
			nTextColor = RGB(61, 99, 184);
			nTextBkColor = RGB(255, 255, 255);
			bUseTitlebar = false;
			bUseBmpTitle = false;
			bUseIconSystemMenu = false;
			bUseAboutboxButton = false;
			bUseMinimizeButton = false;
			bUseMaximizeButton = false;
			bUseQuitButton = false;
			bEnableMoveDlg = false;
			bEnableDragAndDrop = false;
			bEnableRenderingLayer = false;
		}
	};

private:
	CDDB			m_Screen,
		m_Skin,
		m_Render;

	CSound			m_Sound;

	CDDBButton		m_cButtonAbout,
		m_cButtonMinimize,
		m_cButtonMaximize,
		m_cButtonQuit;

	CDDBDLGINFO		m_DlgInfo;

	HICON			m_hIcon;
	CBrush			m_brushCtlColor;
	CRgn			m_Rgn;
	CToolTipCtrl	m_Tooltip;

	bool			m_bInitialized;

	BOOL			m_bMouseLButton;
	CPoint			m_nMouseLBtnDownPos;
	NOTIFYICONDATA	m_TrayIcon;

public:
	CDDBDialog(UINT nIDTemplate, CWnd* pParentWnd);
	virtual ~CDDBDialog();

	void SetWindowText(LPCTSTR lpszString);

	UINT GetWidth() { return m_DlgInfo.nWidth; }
	UINT GetHeight() { return m_DlgInfo.nHeight; }

	COLORREF GetTextColor() { return m_DlgInfo.nTextColor; }
	COLORREF GetTextBkColor() { return m_DlgInfo.nTextBkColor; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void DrawDialogWindow();
	virtual void DrawPanel(int x, int y, int width, int height, LPCTSTR lpszTitle = NULL);
	virtual void CreateAboutButton(int x, int y);
	virtual void CreateMaximizeButton(int x, int y);
	virtual void CreateMinimizeButton(int x, int y);
	virtual void CreateQuitButton(int x, int y);
	virtual void DropFiles(LPCTSTR szFileName);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	void Rendering();
	void Rendering(int x, int y, int width, int height);

	void GetDialogInfo(CDDBDLGINFO* DlgInfo) { *DlgInfo = m_DlgInfo; }
	void SetDialogInfo(CDDBDLGINFO DlgInfo) { if (!m_bInitialized)  m_DlgInfo = DlgInfo; }

	CDDB* GetScreen() { return &m_Screen; }
	CDDB* GetSkin() { return &m_Skin; }
	CDDB* GetRender() { return &m_Render; }

	CDC* GetScreenDC() { return m_Screen.GetDC(); }
	CDC* GetSkinDC() { return m_Skin.GetDC(); }
	CDC* GetRenderDC() { return m_Render.GetDC(); }

	void AddTooltip(CWnd* pWnd, LPCTSTR lpszMsg) { if (!m_bInitialized)  m_Tooltip.AddTool(pWnd, _T(lpszMsg)); }

	void CreateTrayicon(LPCTSTR lpszTooltip);
	void DeleteTrayicon();

	void PlayEffectSound(UINT IDWaveResource) { m_Sound.PlayWavSound(IDWaveResource); }

protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseDrag(WPARAM wParam, LPARAM lParam);
	afx_msg void OnAboutbox();
	afx_msg void OnMaximize();
	afx_msg void OnMinimize();
	afx_msg void OnQuit();
	afx_msg LRESULT OnTrayNotification(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	DECLARE_MESSAGE_MAP()
};

#endif
