#ifndef _ZRADAR_H
#define _ZRADAR_H

#include "ZInterface.h"
#include "MPicture.h"

#include <deque>

struct ZRadarNode
{
	unsigned long	nLastShotTime;
	rvector			attackPos;
	MUID				uid;

	ZRadarNode(rvector _attackPos, MUID _uid) {
		attackPos = _attackPos;
		uid = _uid;
		nLastShotTime = timeGetTime();
	}
};
class ZRadar : public ZInterface
{
protected:
	MBitmapR2*			m_pRadarBitmap;
	MBitmapR2*			m_pPipBitmap;
	MBitmapR2*			m_pFriendPipBitmap;
	deque<ZRadarNode>	m_Nodes;

	float				m_fMaxDistance;
	int					m_nMaxShotTime;

	rvector GetTargetPos(rvector& my_pos, rvector& my_dir, rvector& tar_pos, float radar_radius);
public:
	ZRadar(const char* szName = NULL, MWidget* pParent = NULL, MListener* pListener = NULL);
	virtual ~ZRadar();
	virtual bool OnCreate();
	virtual void OnDestroy();
	virtual void OnDraw(MDrawContext* pDC);
	virtual bool OnEvent(MEvent* pEvent, MListener* pListener);
	void OnAttack(rvector& pAttackerPos, MUID& attackerUID);

	void SetMaxDistance(float fDist);
	float GetMaxDistance() { return m_fMaxDistance; }
};

#endif