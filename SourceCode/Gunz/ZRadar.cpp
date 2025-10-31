#include "stdafx.h"

#include "ZRadar.h"
#include "ZGameInterface.h"
#include "ZGame.h"
#include "ZCharacter.h"
#include "ZApplication.h"
#include "ZConfiguration.h"

ZRadar::ZRadar(const char* szName, MWidget* pParent, MListener* pListener)
	: ZInterface(szName, pParent, pListener)
{
	m_pRadarBitmap = NULL;
	m_pPipBitmap = NULL;

	m_fMaxDistance = 2000.0f;
	m_nMaxShotTime = 3000;
}

ZRadar::~ZRadar()
{

}

bool ZRadar::OnCreate()
{
	m_pRadarBitmap = (MBitmapR2*)MBitmapManager::Get("radar.png");
	m_pPipBitmap = (MBitmapR2*)MBitmapManager::Get("radar_pip.png");
	m_pFriendPipBitmap = (MBitmapR2*)MBitmapManager::Get("radar_pip_friend.png");
	if (!m_pRadarBitmap || !!m_pPipBitmap || !m_pFriendPipBitmap) return false;

	return true;
}

void ZRadar::OnDestroy()
{
}


void rotateVec(rvector& vec, float angle) {
	float ca = cos(angle);
	float sa = sin(angle);

	float rx = vec.x*ca - vec.y*sa;
	float ry = vec.x*sa + vec.y*ca;
	vec.x = rx;
	vec.y = ry;
};

rvector ZRadar::GetTargetPos(rvector& my_pos, rvector& my_dir, rvector& tar_pos, float radar_radius)
{
	rvector target_off = tar_pos - my_pos;

	rvector vector1 = my_dir, vector2 = rvector(0.0f, 1.0f, 0.0f);
	Normalize(vector2);
	vector1.y = -vector1.y;
	vector2.y = -vector2.y;
	float cosAng1 = DotProduct(vector1, vector2);

	float ang;
	if (-vector1.y*vector2.x + vector1.x*vector2.y > 0.0f)
		ang = -(float)(acos(cosAng1));
	else
		ang = (float)(acos(cosAng1));

	rotateVec(target_off, ang);

	if (D3DXVec3Length(&target_off) > m_fMaxDistance) {
		D3DXVec3Scale(&target_off, &target_off, m_fMaxDistance / D3DXVec3Length(&target_off));
	}
	D3DXVec3Scale(&target_off, &target_off, radar_radius / m_fMaxDistance);

	return target_off;
}

void ZRadar::OnDraw(MDrawContext* pDC)
{
	//bool bBlitzKriegs = ZGetGameClient()->IsLadderChannel();

	unsigned long int currentTime = timeGetTime();

	pDC->SetBitmap(m_pRadarBitmap);
	pDC->SetOpacity(0xFF);
	pDC->SetEffect(MDE_NORMAL);

	float left = (660.f / 800.f) * MGetWorkspaceWidth();
	float bottom = 500.f / 600.f * MGetWorkspaceHeight();

	float size = ((790.f / 800.f) * MGetWorkspaceWidth()) - left;
	float r = size / 2;
	float pipSize = size / 12;


	pDC->Draw(left, bottom - size, size, size);

	while ((!m_Nodes.empty()) && ((int)(currentTime - m_Nodes.front().nLastShotTime) > m_nMaxShotTime))
		m_Nodes.pop_front();

	rvector my_dir = ZGetGame()->m_pMyCharacter->m_Direction;
	rvector my_pos = ZGetGame()->m_pMyCharacter->GetPosition();
	my_pos.z = 0.0f;
	my_dir.z = 0.0f;
	Normalize(my_dir);

	if (ZGetGame()->GetMatch()->IsTeamPlay())
	{
		for (ZCharacterManager::iterator itor = ZGetGame()->m_CharacterManager.begin(); itor != ZGetGame()->m_CharacterManager.end(); ++itor)
		{
			rvector pos, screen_pos;
			ZCharacter* pCharacter = (ZCharacter*)(*itor).second;
			if (!pCharacter->IsDie())
			{
				if (pCharacter->GetTeamID() == ZGetGame()->GetMyCharacter()->GetTeamID())
					pDC->SetBitmap(m_pFriendPipBitmap);
				else
					pDC->SetBitmap(m_pPipBitmap);

				rvector friend_pos = pCharacter->GetPosition();
				friend_pos = ZRadar::GetTargetPos(my_pos, my_dir, friend_pos, r);
				pDC->SetOpacity(0xFF);
				pDC->Draw(left + r - friend_pos.x - (pipSize / 2), bottom - r - friend_pos.y - (pipSize / 2), pipSize, pipSize);
			}
		}
	}
	else
	{
		return;
	}
}

bool ZRadar::OnEvent(MEvent* pEvent, MListener* pListener)
{
	return false;
}


void ZRadar::SetMaxDistance(float fDist)
{
	m_fMaxDistance = fDist;
}

void ZRadar::OnAttack(rvector& pAttackerPos, MUID& attackerUID)
{
	bool bDisplayShot = false;

	if (attackerUID != ZGetMyUID()) {
		if (!ZGetGame()->GetMatch()->IsTeamPlay()) {
			ZCharacter* pAttacker = (ZCharacter*)ZGetCharacterManager()->Find(attackerUID);
			if (pAttacker)
				if (!pAttacker->IsTeam(ZGetGame()->m_pMyCharacter))
					bDisplayShot = true;
		}
	}

	if (!bDisplayShot) return;

	rvector attackPos = pAttackerPos;
	attackPos.z = 0.0f;

	if (!m_Nodes.empty()) {
		auto it = m_Nodes.begin();
		for (auto it = m_Nodes.begin(); it != m_Nodes.end(); it++) {
			if (it->uid == attackerUID) {
				it = m_Nodes.erase(it);
				break;
			}
		}
	}

	m_Nodes.push_back(ZRadarNode(attackPos, attackerUID));
}