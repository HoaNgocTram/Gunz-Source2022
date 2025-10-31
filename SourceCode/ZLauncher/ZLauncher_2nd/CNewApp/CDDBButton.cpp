#include "stdafx.h"
#include "CDDBButton.h"

CDDBButton::CDDBButton()
{
	m_nButtonState = BBS_UP;
	m_nMouseState = BMS_LEAVE;
	m_bTrackMouseEvent = FALSE;
	m_bBlanking = FALSE;
	m_bBlanked = FALSE;
}

CDDBButton::~CDDBButton()
{
}

BEGIN_MESSAGE_MAP(CDDBButton, CButton)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL CDDBButton::InitDDBButton(UINT nIDSkinResource, UINT nIDWavResource)
{
	ddbSkin.DeleteDDB();
	ddbRendering.DeleteDDB();

	if (!ddbSkin.CreateDDB(GetDC(), nIDSkinResource))
		return false;

	m_nWidth = (ddbSkin.GetWidth() - 3) / 4;
	m_nHeight = ddbSkin.GetHeight();

	if (!ddbRendering.CreateDDB(GetDC(), ddbSkin.GetWidth(), m_nHeight, 0))
		return false;

	ddbRendering.PutBitmap(0, 0, ddbSkin.GetDC(), 0, 0, ddbSkin.GetWidth(), m_nHeight, SRCCOPY);
	CString str;
	GetWindowText(str);
	SetWindowText(str);

	m_nIDWavResource = nIDWavResource;

	SetWindowPos(NULL, 0, 0, m_nWidth, m_nHeight, SWP_NOMOVE | SWP_NOOWNERZORDER);

	return true;
}

BOOL CDDBButton::InitDDBButton(CDC* pDC, int xSrc, int ySrc, int width, int height, UINT nIDWavResource)
{
	ddbSkin.DeleteDDB();
	ddbRendering.DeleteDDB();

	if (!ddbSkin.CreateDDB(GetDC(), width * 4 + 3, height, 0))
		return false;
	ddbSkin.PutBitmap(0, 0, pDC, xSrc, ySrc, width * 4 + 3, height, SRCCOPY);

	m_nWidth = width;
	m_nHeight = height;

	if (!ddbRendering.CreateDDB(GetDC(), ddbSkin.GetWidth(), height, RGB(255, 255, 255)))
		return false;

	ddbRendering.PutBitmap(0, 0, ddbSkin.GetDC(), 0, 0, ddbSkin.GetWidth(), m_nHeight, SRCCOPY);
	CString str;
	GetWindowText(str);
	SetWindowText(str);

	m_nIDWavResource = nIDWavResource;

	SetWindowPos(NULL, 0, 0, m_nWidth, m_nHeight, SWP_NOMOVE | SWP_NOOWNERZORDER);

	return true;
}

void CDDBButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (lpDrawItemStruct->itemState & ODS_SELECTED)
		m_nButtonState = BBS_DOWN;

	else if (lpDrawItemStruct->itemState & ODS_DISABLED)
		m_nButtonState = BBS_DISABLE;

	else
	{
		if (m_nMouseState == BMS_OVER)
			m_nButtonState = BBS_FOCUS;

		else
			m_nButtonState = BBS_UP;
	}

	DrawButton(m_nButtonState);
}

void CDDBButton::DrawButton(UINT nButtonState)
{
	CDC* pDC = GetDC();
	ddbRendering.Rendering(pDC, 0, 0, m_nWidth*nButtonState + nButtonState, 0, m_nWidth, m_nHeight);
	ReleaseDC(pDC);
}

void CDDBButton::PreSubclassWindow()
{
	ModifyStyle(0, BS_OWNERDRAW);

	CButton::PreSubclassWindow();
}

void CDDBButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bTrackMouseEvent == false)
	{
		SetClassLong(m_hWnd, GCL_HCURSOR, (LONG)(AfxGetApp()->LoadStandardCursor(IDC_ARROW)));

		TRACKMOUSEEVENT trackMouseEvent;
		trackMouseEvent.cbSize = sizeof(trackMouseEvent);
		trackMouseEvent.hwndTrack = m_hWnd;
		trackMouseEvent.dwFlags = TME_LEAVE | TME_HOVER;
		trackMouseEvent.dwHoverTime = 1;
		m_bTrackMouseEvent = _TrackMouseEvent(&trackMouseEvent);
	}

	CButton::OnMouseMove(nFlags, point);
}

LRESULT CDDBButton::OnMouseHover(WPARAM wParam, LPARAM lParam)
{
	m_nMouseState = BMS_OVER;

	Invalidate();

	return 0;
}

LRESULT CDDBButton::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	m_bTrackMouseEvent = false;
	m_nMouseState = BMS_LEAVE;

	Invalidate();

	return 0;
}

void CDDBButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_nIDWavResource != NULL)
		m_sndSound.PlayWavSound(m_nIDWavResource);

	CButton::OnLButtonDown(nFlags, point);
}

void CDDBButton::SetWindowText(LPCTSTR lpszString)
{
	ddbRendering.PutBitmap(0, 0, ddbSkin.GetDC(), 0, 0, ddbSkin.GetWidth(), m_nHeight, SRCCOPY);

	COLORREF color[] = { m_WindowText.nTextUp,      m_WindowText.nTextShadowUp,
						 m_WindowText.nTextFocus,   m_WindowText.nTextShadowFocus,
						 m_WindowText.nTextDown,    m_WindowText.nTextShadowDown,
						 m_WindowText.nTextDisable, m_WindowText.nTextShadowDisable };
	for (int i = 0; i < 4; i++)
	{
		ddbRendering.PutText(m_nWidth*i + i + (i == 2 ? 1 : 0), 1 + (i == 2 ? 1 : 0), m_nWidth, m_nHeight - 2,
			m_WindowText.nTextSize, m_WindowText.lpszFont, lpszString,
			color[i * 2 + 1], TS_BOLD, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		ddbRendering.PutText(m_nWidth*i + i + 1 + (i == 2 ? 1 : 0), 1 + (i == 2 ? 1 : 0), m_nWidth, m_nHeight - 2,
			m_WindowText.nTextSize, m_WindowText.lpszFont, lpszString,
			color[i * 2 + 1], TS_BOLD, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		ddbRendering.PutText(m_nWidth*i + i + (i == 2 ? 1 : 0), (i == 2 ? 1 : 0), m_nWidth, m_nHeight - 2,
			m_WindowText.nTextSize, m_WindowText.lpszFont, lpszString,
			color[i * 2], TS_BOLD, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
	}

	::SetWindowText(m_hWnd, lpszString);
}

void CDDBButton::MoveWindow(UINT x, UINT y)
{
	::MoveWindow(m_hWnd, x, y, m_nWidth, m_nHeight, true);
}

UINT CDDBButton::GetWidth()
{
	return m_nWidth;
}

UINT CDDBButton::GetHeight()
{
	return m_nHeight;
}

void CDDBButton::SetBlanking(BOOL bBlanking, DWORD dwBlankingTimer)
{
	if (m_bBlanking == bBlanking)
		return;

	m_bBlanking = bBlanking;
	m_bBlanked = FALSE;

	if (bBlanking == TRUE)
		SetTimer(0, dwBlankingTimer, NULL);
	else
	{
		KillTimer(0);

		DrawButton(BBS_UP);
	}
}

BOOL CDDBButton::GetBlanking()
{
	return m_bBlanking;
}

void CDDBButton::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == 0)
	{
		if (m_bBlanking == TRUE)
		{
			if (m_nButtonState == BBS_UP)
			{
				if (m_bBlanked == FALSE)
				{
					DrawButton(BBS_FOCUS);
					m_bBlanked = TRUE;
				}
				else
				{
					DrawButton(BBS_UP);
					m_bBlanked = FALSE;
				}
			}

			else
				m_bBlanked = FALSE;
		}
	}

	CButton::OnTimer(nIDEvent);
}