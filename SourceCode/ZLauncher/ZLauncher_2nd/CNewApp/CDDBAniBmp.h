#ifndef _INCLUDED_CDDBANIBMP_
#define _INCLUDED_CDDBANIBMP_

#include "CDDB.h"

class CDDBAniBmp : public CStatic
{
public:
	CDDBAniBmp();
	virtual ~CDDBAniBmp();

	BOOL InitDDBAniBmp(UINT nIDResource, UINT nNumOfScene);

	void EnableAnimate(BOOL fAnimate);
	BOOL SetAnimateScene(UINT nStartSceneNum, UINT nEndtSceneNum);

	BOOL ShowScene(UINT nSceneNum);
	UINT GetCurrentScene();
	void SetLoop(BOOL fLoop);

	void SetTimerValue(UINT nTimerValue);
	void SetTimerValue(UINT nSceneNum, UINT nTimerValue);
	void SetTimerValue(UINT nStartSceneNum, UINT nEndSceneNum, UINT nTimerValue);
	void SetTimerValue(UINT *nTimerValue);

protected:
	virtual void PreSubclassWindow();
protected:
	CDDB ddbSkin;
	CDDB ddbRendering;

	UINT m_nWidth;
	UINT m_nHeight;

	UINT m_nNumOfScene;
	UINT m_nCurrentScene;
	UINT m_nStartSceneNum;
	UINT m_nEndSceneNum;

	UINT m_nTimerValue[50];

	BOOL m_fAnimate;
	BOOL m_fLoop;

	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	DECLARE_MESSAGE_MAP()
};

#endif
