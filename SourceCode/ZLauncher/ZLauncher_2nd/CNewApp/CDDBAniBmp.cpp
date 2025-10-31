#include "stdafx.h"
#include "CDDBAniBmp.h"

#define TIMER_ANIMATE		1

CDDBAniBmp::CDDBAniBmp()
{
	m_nNumOfScene = 0;
	m_nCurrentScene = 0;
	m_fAnimate = false;
	m_fLoop = true;
}

CDDBAniBmp::~CDDBAniBmp()
{
	ddbSkin.DeleteDDB();
	ddbRendering.DeleteDDB();
}

BEGIN_MESSAGE_MAP(CDDBAniBmp, CStatic)
	ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()

BOOL CDDBAniBmp::InitDDBAniBmp(UINT nIDResource, UINT nNumOfScene)
{
	if (nNumOfScene < 1)
		return false;

	KillTimer(TIMER_ANIMATE);

	ddbSkin.DeleteDDB();
	ddbRendering.DeleteDDB();

	if (!ddbSkin.CreateDDB(GetDC(), nIDResource))
		return false;
	m_nWidth = (ddbSkin.GetWidth() - nNumOfScene + 1) / nNumOfScene;
	m_nHeight = ddbSkin.GetHeight();
	if (!ddbRendering.CreateDDB(GetDC(), m_nWidth, m_nHeight, RGB(255, 255, 255)))
		return false;

	for (UINT i = 0; i < 50; i++)
		m_nTimerValue[i] = 1000;
	m_nNumOfScene = nNumOfScene;
	m_nStartSceneNum = 0;
	m_nEndSceneNum = nNumOfScene - 1;

	CRect parent_rect, rect;
	GetParent()->GetWindowRect(&parent_rect);
	GetWindowRect(&rect);
	rect.left = rect.left - parent_rect.left;
	rect.top = rect.top - parent_rect.top;
	rect.right = rect.left + m_nWidth;
	rect.bottom = rect.top + m_nHeight;
	MoveWindow(rect);

	ShowScene(0);

	return true;
}

void CDDBAniBmp::EnableAnimate(BOOL fAnimate)
{
	if (m_fAnimate == fAnimate)
		return;

	if (fAnimate)
	{
		ShowScene(m_nStartSceneNum);
		SetTimer(TIMER_ANIMATE, m_nTimerValue[m_nStartSceneNum], NULL);
	}
	else
		KillTimer(TIMER_ANIMATE);

	m_fAnimate = fAnimate;
}

BOOL CDDBAniBmp::SetAnimateScene(UINT nStartSceneNum, UINT nEndtSceneNum)
{
	if ((nStartSceneNum >= m_nNumOfScene) || (nEndtSceneNum >= m_nNumOfScene))
		return false;
	if (nStartSceneNum > m_nNumOfScene)
		return false;

	KillTimer(TIMER_ANIMATE);

	m_nStartSceneNum = nStartSceneNum;
	m_nEndSceneNum = nEndtSceneNum;

	if (m_fAnimate)
	{
		ShowScene(m_nStartSceneNum);
		SetTimer(TIMER_ANIMATE, m_nTimerValue[m_nStartSceneNum], NULL);
	}

	return true;
}

BOOL CDDBAniBmp::ShowScene(UINT nSceneNum)
{
	if (nSceneNum > m_nNumOfScene)
		return false;

	UINT pos;
	if (nSceneNum == 0)
		pos = 0;
	else
		pos = m_nWidth * nSceneNum + nSceneNum;
	ddbRendering.PutBitmap(0, 0, ddbSkin.GetDC(), pos, 0, m_nWidth, m_nHeight, SRCCOPY);

	CDC* pDC = GetDC();
	ddbRendering.Rendering(pDC);
	ReleaseDC(pDC);
	m_nCurrentScene = nSceneNum;

	if (m_fAnimate)
		SetTimer(TIMER_ANIMATE, m_nTimerValue[nSceneNum], NULL);

	return true;
}

UINT CDDBAniBmp::GetCurrentScene()
{
	return m_nCurrentScene;
}

void CDDBAniBmp::SetLoop(BOOL fLoop)
{
	m_fLoop = fLoop;
}

void CDDBAniBmp::SetTimerValue(UINT nTimerValue)
{
	for (UINT i = 0; i < m_nNumOfScene; i++)
		m_nTimerValue[i] = nTimerValue;
}

void CDDBAniBmp::SetTimerValue(UINT nSceneNum, UINT nTimerValue)
{
	m_nTimerValue[nSceneNum] = nTimerValue;
}

void CDDBAniBmp::SetTimerValue(UINT nStartSceneNum, UINT nEndSceneNum, UINT nTimerValue)
{
	for (UINT i = nStartSceneNum; i <= nEndSceneNum; i++)
		m_nTimerValue[i] = nTimerValue;
}

void CDDBAniBmp::SetTimerValue(UINT *nTimerValue)
{
	for (UINT i = 0; i < m_nNumOfScene; i++)
		m_nTimerValue[i] = *(nTimerValue + i);
}

void CDDBAniBmp::PreSubclassWindow()
{
	ModifyStyle(0, BS_OWNERDRAW);

	CStatic::PreSubclassWindow();
}

void CDDBAniBmp::OnPaint()
{
	CPaintDC dc(this);
	CDC* pDC = GetDC();

	ddbRendering.Rendering(pDC);

	ReleaseDC(pDC);
}

void CDDBAniBmp::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == TIMER_ANIMATE)
	{
		KillTimer(TIMER_ANIMATE);

		BOOL bLoop = true;
		m_nCurrentScene++;
		if ((m_nCurrentScene > m_nEndSceneNum) || (m_nCurrentScene == m_nNumOfScene))
		{
			m_nCurrentScene = m_nStartSceneNum;

			if (!m_fLoop)
				bLoop = false;
		}

		ShowScene(m_nCurrentScene);

		if (bLoop)
			SetTimer(TIMER_ANIMATE, m_nTimerValue[m_nCurrentScene], NULL);
	}

	CStatic::OnTimer(nIDEvent);
}