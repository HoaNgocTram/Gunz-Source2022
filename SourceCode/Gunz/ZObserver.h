#ifndef _ZOBSERVER_H
#define _ZOBSERVER_H

#include "ZPrerequisites.h"
#include "MDrawContext.h"

class ZCharacter;
class ZCamera;
class ZIDLResource;

enum ZObserverType
{
	ZOM_NONE = 0,

	ZOM_ANYONE,
	ZOM_BLUE,
	ZOM_RED,

	ZOM_MAX
};

#define ZFREEOBSERVER_RADIUS	30.f	
#define OBSERVER_QUICK_TAGGER_TARGET_KEY			'0'	

class ZObserverQuickTarget {
protected:
	MUID	m_arrayPlayers[10];

public:
	ZObserverQuickTarget()	{ Clear(); }
	~ZObserverQuickTarget()	{ Clear(); }
	void Clear() {
		for (int i=0; i<10; i++)
			m_arrayPlayers[i] = MUID(0,0);
	}

	bool ConvertKeyToIndex(char nKey, int* nIndex);
	void StoreTarget(int nIndex, MUID uidChar);
	MUID GetTarget(int nIndex);
};

class ZObserver
{
private:
protected:
	float					m_fDelay;	
	bool					m_bVisible;
	ZObserverType			m_nType;
	ZCharacter*				m_pTargetCharacter;
	ZCamera*				m_pCamera;
	ZIDLResource*			m_pIDLResource;
	rvector					m_FreeLookTarget;
	ZObserverQuickTarget	m_QuickTarget;

	void ShowInfo(bool bShow);
	void SetTarget(ZCharacter* pCharacter);
	bool IsVisibleSetTarget(ZCharacter* pCharacter);	
	void CheckDeadTarget();	
public:
	ZObserver();
	virtual ~ZObserver();
	bool Create(ZCamera* pCamera, ZIDLResource*	pIDLResource);
	void Destroy();
	void ChangeToNextTarget();
	bool SetFirstTarget();
	void SetTarget(MUID muid);

	bool IsVisible() { return m_bVisible; }
	void Show(bool bVisible);
	void OnDraw(MDrawContext* pDC);
	void DrawPlayerDuelHPAPBar(MDrawContext* pDC);
	float GetDelay() { return m_fDelay; }


	void SetType(ZObserverType nType);
	ZObserverType GetType() { return m_nType; }
	ZCharacter* GetTargetCharacter() { return m_pTargetCharacter; }

	void SetFreeLookTarget(rvector& tar) { m_FreeLookTarget = tar; }
	rvector* GetFreeLookTarget() { return &m_FreeLookTarget; }

	void NextLookMode();

	bool OnKeyEvent(bool bCtrl, char nKey);
};

#define ZOBSERVER_DEFAULT_DELAY_TIME		0.2f

#endif