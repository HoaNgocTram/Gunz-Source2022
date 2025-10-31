#ifndef _INCLUDED_CDDBBUTTON_
#define _INCLUDED_CDDBBUTTON_

#include "CDDB.h"
#include "CSound.h"

class CDDBButton : public CButton
{
	struct CDDBBUTTONWINDOWTEXT
	{
		CDDBBUTTONWINDOWTEXT()
		{
			nTextUp = RGB(255, 255, 255);
			nTextShadowUp = RGB(0, 0, 0);
			nTextFocus = RGB(255, 255, 100);
			nTextShadowFocus = RGB(0, 0, 0);
			nTextDown = RGB(255, 255, 100);
			nTextShadowDown = RGB(0, 0, 0);
			nTextDisable = RGB(60, 60, 60);
			nTextShadowDisable = RGB(0, 0, 0);
			lpszFont = "Tahoma";
			nTextSize = 14;
		}

		COLORREF nTextUp;
		COLORREF nTextShadowUp;
		COLORREF nTextFocus;
		COLORREF nTextShadowFocus;
		COLORREF nTextDown;
		COLORREF nTextShadowDown;
		COLORREF nTextDisable;
		COLORREF nTextShadowDisable;
		LPCTSTR lpszFont;
		UINT nTextSize;
	};

public:
	const enum BUTTONSTATE { BBS_UP, BBS_FOCUS, BBS_DOWN, BBS_DISABLE };
	const enum MOUSESTATE { BMS_LEAVE, BMS_OVER };

	CDDBButton();
	virtual ~CDDBButton();
	BOOL InitDDBButton(UINT nIDSkinResource, UINT nIDWavResource = 0);
	BOOL InitDDBButton(CDC* pDC, int xSrc, int ySrc, int width, int height, UINT nIDWavResource = 0);
	void DrawButton(UINT nButtonState);
	void SetWindowText(LPCTSTR lpszString);
	void MoveWindow(UINT x, UINT y);
	UINT GetWidth();
	UINT GetHeight();
	void SetBlanking(BOOL bBlanking = TRUE, DWORD dwBlankingTimer = 1000);
	BOOL GetBlanking();

protected:
	virtual void PreSubclassWindow();
protected:
	CDDB ddbRendering;
	CDDB ddbSkin;
	UINT m_nWidth;
	UINT m_nHeight;
	UINT m_nButtonState;
	UINT m_nMouseState;
	BOOL m_bBlanking;
	BOOL m_bBlanked;
	BOOL m_bTrackMouseEvent;
	UINT m_nIDWavResource;
	CDDBBUTTONWINDOWTEXT m_WindowText;
	CSound m_sndSound;

	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()
};

#endif
