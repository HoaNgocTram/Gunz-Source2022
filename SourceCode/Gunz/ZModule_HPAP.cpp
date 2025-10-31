#include "stdafx.h"
#include "ZModule_HPAP.h"
#include "ZGame.h"
#include "ZApplication.h"
#include "ZActorWithFSM.h"
#include "CheckReturnCallStack.h"

ZModule_HPAP::ZModule_HPAP() : m_LastAttacker(MUID(0,0)), m_bRealDamage_DebugRegister(false), m_fAccumulationDamage(0.f), m_bAccumulationDamage(false)
{
	float f = float(rand() % 1000); 
	f += float(rand() % 100) * 0.01f;
	m_fMask.Set_MakeCrc(f);

	m_bRealDamage.Set_MakeCrc(false);
	m_fHP.Set_MakeCrc(1000.f);
	m_fAP.Set_MakeCrc(1000.f);
	m_fMaxHP.Set_MakeCrc(100);
	m_fMaxAP.Set_MakeCrc(100);
}



ZModule_HPAP::~ZModule_HPAP()
{
}

float ZModule_HPAP::GetHP() 
{
	// Custom: Modified
	float fHP = m_fHP.Ref();

	if (floor(fHP) <= 0.f)
		fHP = 0.f;

	return  fHP - GetMask();
}

float	ZModule_HPAP::GetAP() 
{
	// Custom: Modified
	float fAP = m_fAP.Ref();

	if (floor(fAP) <= 0.f)
		fAP = 0.f;

	return  fAP - GetMask();
}

void ZModule_HPAP::OnDamage(MUID uidAttacker, float fDamage, float fRatio, int Type)
{
	_ASSERT(fDamage >= 0);
	if (fDamage < 0) fDamage *= -1.f;	// ÇØÅ·À¸·Î À½¼ö´ë¹ÌÁö¸¦ ³Ö¾î¼­ È¸º¹ÇÒ ¼ö ÀÖÀ¸¹Ç·Î..


	m_LastAttacker = uidAttacker;

	// Äù½ºÆ® Å×½ºÆ®¿ë Ä¡Æ® Ã¼Å©
#ifndef _PUBLISH
	if (CheckQuestCheet() == true) return;
#endif
	//jintriple3 µð¹ö±× ·¹Áö½ºÅÍ ÇÙ ¹æÁö ÄÚµå
	//³» Ä³¸¯ÅÍ°¡ ¾Æ´Ï°Å³ª npc°¡ ¾Æ´Ï¸é....
	//m_bRealDamageÀÇ °æ¿ì ³» Å¬¶óÀÌ¾ðÆ®¿¡¼­ ½ÇÁ¦·Î µ¥¹ÌÁö¸¦ Àû¿ëÇÏ³ª ¾ÈÇÏ³ª¸¦ Ã¼Å©ÇÏ´Â º¯¼ö.
	//MyCharacter ÀÌ°Å³ª NPC¸¸ ³» Å¬¶óÀÌ¾ðÆ®¿¡¼­ µ¥¹ÌÁö Àû¿ë.

	ZCharacter* pCharacter = ZGetGame()->m_pMyCharacter;
	if (pCharacter == NULL) return;

	if(!m_bRealDamage.Ref())	
		PROTECT_DEBUG_REGISTER(!m_bRealDamage_DebugRegister)	//ÀÌ°É À§ÇØ »õ·Î¿î º¯¼ö ÇÏ³ª ´õ »ç¿ë.
			return;

	// NPCÀÇ ³­ÀÌµµ Á¶Àý°è¼ö¶§¹®¿¡ ³ÖÀ½
	ZObject* pAttacker = ZGetObjectManager()->GetObject(uidAttacker);
	if ((pAttacker) && (!IsPlayerObject(pAttacker)))
	{
		if (MIsExactlyClass(ZActor, pAttacker))
		{
			ZActor* pActor = (ZActor*)pAttacker;
			//fDamage = (int)(fDamage * (pActor->GetTC()));
			fDamage = fDamage * (pActor->GetQL() * 0.2f + 1);
			fDamage = pActor->ModifyAttackDamage(fDamage);
		}
		if (MIsExactlyClass(ZActorWithFSM, pAttacker))
		{
			//do nothing for now.
		}
	}
	if (!ZGetGame()->m_pMyCharacter->IsObserverTarget() && !ZGetGame()->GetMatch()->IsQuestDrived() && !ZGetGame()->m_pMyCharacter->IsDie() && !ZGetGameClient()->IsDuelTournamentGame())
	{
		if (!pAttacker || !ZGetGame()->m_pMyCharacter) return;
		if (pAttacker->GetUID() != ZGetGame()->m_pMyCharacter->GetUID())
		{
#ifdef _BOXLEAD
			if (ZGetGameClient()->GetMatchStageSetting()->IsLead() == false && (ZDAMAGETYPE)Type != ZD_BULLET && (ZDAMAGETYPE)Type != ZD_BULLET_HEADSHOT)
			{
#endif
				if (ZGetGame()->DmgCounter->count(pAttacker->GetUID()) == 1)
				{
					DamageCounterMap::iterator itr = ZGetGame()->DmgCounter->find(pAttacker->GetUID());
					itr->second += fDamage;
				}
				else {
					ZGetGame()->DmgCounter->insert(DamageCounterMap::value_type(pAttacker->GetUID(), fDamage));
				}
			}
		}
#ifdef _BOXLEAD
	}
#endif
	if (!ZGetGame()->m_pMyCharacter->IsDie())
	{
		ZGetGame()->m_pMyCharacter->GetStatus().CheckCrc();
		ZGetGame()->m_pMyCharacter->GetStatus().Ref().nTakenDamage += fDamage;
		ZGetGame()->m_pMyCharacter->GetStatus().Ref().nRoundTakenDamage += fDamage;
		ZGetGame()->m_pMyCharacter->GetStatus().MakeCrc();
	}
	if(IsAccumulationDamage())
	{
		AccumulateDamage(fDamage);
#ifndef _PUBLISH	// ³»ºÎºôµå¿¡¼­ ´©Àû ´ë¹ÌÁö Á¤º¸ Ãâ·Â
		char szAccumulationDamagePrint[256];
		sprintf(szAccumulationDamagePrint, "ÇÇÇØ ´çÇÑ ´©Àû´ë¹ÌÁö[%2.1f]", GetAccumulationDamage());
		ZChatOutput(MCOLOR(255, 200, 100), szAccumulationDamagePrint);
#endif
	}


	float fHPDamage = (float)((float)fDamage * fRatio);
	float fAPDamage = fDamage - fHPDamage;

	if ((GetAP() - fAPDamage) < 0)
	{
		fHPDamage += (fAPDamage - GetAP());
		fAPDamage -= (fAPDamage - GetAP());
	}

	SetHP(GetHP() - fHPDamage);
	SetAP(GetAP() - fAPDamage);

	return;
}


float ZModule_HPAP::OnDamageCalc(MUID uidAttacker, float fDamage, float fRatio)
{
	_ASSERT(fDamage >= 0);
	if (fDamage < 0) fDamage *= -1.f;

	m_LastAttacker = uidAttacker;

	ZCharacter* pCharacter = ZGetGame()->m_pMyCharacter;

	if (pCharacter == NULL) return 0;

	if (!m_bRealDamage.Ref())
		PROTECT_DEBUG_REGISTER(!m_bRealDamage_DebugRegister)
		return 0;

	ZObject* pAttacker = ZGetObjectManager()->GetObject(uidAttacker);
	if ((pAttacker) && (!IsPlayerObject(pAttacker)))
	{
		ZActor* pActor = (ZActor*)pAttacker;
		//fDamage = (int)(fDamage * (pActor->GetTC()));
		fDamage = fDamage * (pActor->GetQL() * 0.2f + 1);
	}

	if (IsAccumulationDamage())
	{
		AccumulateDamage(fDamage);
		/*
		#ifndef _PUBLISH
				char szAccumulationDamagePrint[256];
				sprintf(szAccumulationDamagePrint, "???? ???? ?????????[%2.1f]", GetAccumulationDamage());
				ZChatOutput(MCOLOR(255, 200, 100), szAccumulationDamagePrint);
		#endif*/
	}

	//TODO: modify attakdamage for zcharacter (For blitzkrieg);


	float fHPDamage = (float)((float)fDamage * fRatio);
	float fAPDamage = fDamage - fHPDamage;

	if ((GetAP() - fAPDamage) < 0)
	{
		fHPDamage += (fAPDamage - GetAP());
		fAPDamage -= (fAPDamage - GetAP());
	}

	SetHP(GetHP() - fHPDamage);
	SetAP(GetAP() - fAPDamage);

	if (!ZGetGame()->GetMatch()->IsQuestDrived() && !ZGetGameTypeManager()->IsQuestDerived(ZGetGameClient()->GetMatchStageSetting()->GetGameType()))
	{
		ZPostDamage(pAttacker->GetUID(), fHPDamage);
	}

	return fHPDamage + fAPDamage;
}



void ZModule_HPAP::InitStatus()
{
	m_LastAttacker = MUID(0,0);
}

bool ZModule_HPAP::CheckQuestCheet()
{
#ifdef _PUBLISH
	return false;
#endif

	// Äù½ºÆ® Å×½ºÆ®¿ë Ä¡Æ® Ã¼Å©
	if (IsMyCharacter((ZObject*)m_pContainer))
	{
		// Custom: Quest/Clan Server
		if ((ZIsLaunchDevelop()) /*&& (ZGetGameClient()->GetServerMode() == MSM_TEST)*/)
		{
			//if (ZGetGameTypeManager()->IsQuestDerived(ZGetGameClient()->GetMatchStageSetting()->GetGameType()))
			//{
				if (ZGetQuest()->GetCheet(ZQUEST_CHEET_GOD) == true) return true;
			//}
		}
	}

	return false;
}

void ZModule_HPAP::ShiftFugitiveValues()
{
	m_fMask.ShiftHeapPos_CheckCrc();

	m_fMaxHP.ShiftHeapPos_CheckCrc();
	m_fMaxAP.ShiftHeapPos_CheckCrc();

	m_fHP.ShiftHeapPos_CheckCrc();
	m_fAP.ShiftHeapPos_CheckCrc();

	m_bRealDamage.ShiftHeapPos_CheckCrc();
}