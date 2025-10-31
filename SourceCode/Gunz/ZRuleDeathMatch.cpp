#include "stdafx.h"
#include "ZRuleDeathMatch.h"
#include "ZScreenEffectManager.h"
#include "MMatchUtil.h"

ZRuleSoloDeathMatch::ZRuleSoloDeathMatch(ZMatch* pMatch) : ZRule(pMatch)
{

}

ZRuleSoloDeathMatch::~ZRuleSoloDeathMatch()
{





}
/////////////////////////////////////////////////////////////////////////////////////////

ZRuleTeamDeathMatch::ZRuleTeamDeathMatch(ZMatch* pMatch) : ZRule(pMatch)
{

}

ZRuleTeamDeathMatch::~ZRuleTeamDeathMatch()
{

}


/////////////////////////////////////////////////////////////////////////////////////////

ZRuleTeamDeathMatch2::ZRuleTeamDeathMatch2(ZMatch* pMatch) : ZRule(pMatch)
{

}

ZRuleTeamDeathMatch2::~ZRuleTeamDeathMatch2()
{

}

bool ZRuleTeamDeathMatch2::OnCommand(MCommand* pCommand)
{
	if (!ZGetGame()) return false;

	switch (pCommand->GetID())
	{

	case MC_MATCH_GAME_DEAD:
		{
			MUID uidAttacker, uidVictim;

			pCommand->GetParameter(&uidAttacker, 0, MPT_UID);
			pCommand->GetParameter(&uidVictim, 2, MPT_UID);

			ZCharacter* pAttacker = (ZCharacter*) ZGetGame()->m_CharacterManager.Find(uidAttacker);
			ZCharacter* pVictim = (ZCharacter*) ZGetGame()->m_CharacterManager.Find(uidVictim);
			
			m_pMatch->AddTeamKills(pVictim->GetTeamID() == MMT_BLUE ? MMT_RED : MMT_BLUE);
		}
		break;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////

ZRuleTeamCTF::ZRuleTeamCTF(ZMatch* pMatch) : ZRule(pMatch)
{
	ZeroMemory(m_BlueFlagPos, sizeof(m_BlueFlagPos));
	ZeroMemory(m_RedFlagPos, sizeof(m_RedFlagPos));
	m_uidRedFlagHolder = MUID( 0, 0 );
	m_uidBlueFlagHolder = MUID( 0, 0 );
}

ZRuleTeamCTF::~ZRuleTeamCTF()
{

}

bool ZRuleTeamCTF::OnCommand(MCommand* pCommand)
{
	if (!ZGetGame()) return false;

	switch (pCommand->GetID())
	{
	case MC_MATCH_FLAG_STATE:
		{
			int nItemID;
			MShortVector s_pos;
			int IsGone;
			MUID Carrier;
			pCommand->GetParameter(&nItemID, 0, MPT_INT);
			pCommand->GetParameter(&s_pos, 1, MPT_SVECTOR);
			pCommand->GetParameter(&IsGone, 2, MPT_INT);
			pCommand->GetParameter(&Carrier, 3, MPT_UID);
			ZCharacter* pCharacter = (ZCharacter*)ZGetCharacterManager()->Find(Carrier);
			if (!pCharacter)
				break;

			switch(nItemID)
			{
			case CTF_RED_ITEM_ID:
				{
					SetRedCarrier(Carrier);
					SetRedFlagState(IsGone);
					SetRedFlagPos(rvector(s_pos.x, s_pos.y, s_pos.z));
				}
				break;
			case CTF_BLUE_ITEM_ID:
				{
					SetBlueCarrier(Carrier);
					SetBlueFlagState(IsGone);
					SetBlueFlagPos(rvector(s_pos.x, s_pos.y, s_pos.z));
				}
				break;
			}
		}
		break;
	case MC_MATCH_FLAG_CAP:
		{
			MMatchTeam nTeam;
			pCommand->GetParameter(&nTeam, 0, MPT_INT);
			m_pMatch->AddTeamKills(nTeam == MMT_BLUE ? MMT_BLUE : MMT_RED);

			if(nTeam == MMT_RED)
			{
				ZGetScreenEffectManager()->AddCTFEffect(nTeam, 2);
				//ZGetSoundEngine()->PlaySound( "scored" );
				//ZGetGameInterface()->PlayVoiceSound( VOICE_RED_TEAM_SCORE, 2000);
				//ZGetScreenEffectManager()->AddScreenEffect("ctf_score_r");
				SetBlueFlagState(false);
				SetRedCarrier(MUID(0,0));
			}
			else if(nTeam == MMT_BLUE)
			{
				ZGetScreenEffectManager()->AddCTFEffect(nTeam, 2);
				//ZGetSoundEngine()->PlaySound( "scored" );
				//ZGetGameInterface()->PlayVoiceSound( VOICE_BLUE_TEAM_SCORE, 2000);
				//ZGetScreenEffectManager()->AddScreenEffect("ctf_score_b");
				SetRedFlagState(false); // oops??? (secrets set to true)
				SetBlueCarrier(MUID(0,0));
			}

			for (ZCharacterManager::iterator itor = ZGetGame()->m_CharacterManager.begin();
			itor != ZGetGame()->m_CharacterManager.end(); ++itor)
			{
				ZCharacter* pCharacter = (ZCharacter*)(*itor).second;
				if(pCharacter)
				{
					if(pCharacter->GetTeamID() == nTeam)
					{
						pCharacter->SetTagger(false);
					}
				}
			}
		}
		break;
	case MC_MATCH_FLAG_EFFECT:
		{
			MUID uidOwner;
			int nTeam;
			pCommand->GetParameter(&uidOwner,		0, MPT_UID);
			pCommand->GetParameter(&nTeam,		1, MPT_INT);

			ZCharacter* pCapper = (ZCharacter*)ZGetGame()->m_CharacterManager.Find(uidOwner);
			if(pCapper)
			{
				if(!pCapper->IsDie())
				{
					AssignFlagEffect(uidOwner, nTeam);
				}

				else
				{
					for (ZCharacterManager::iterator itor = ZGetGame()->m_CharacterManager.begin();
					itor != ZGetGame()->m_CharacterManager.end(); ++itor)
					{
						ZCharacter* pCharacter = (ZCharacter*)(*itor).second;
						if(pCharacter)
						{
							if(pCharacter->GetTeamID() == nTeam)
							{
								pCharacter->SetTagger(false);
							}
						}
					}
					if(nTeam == MMT_RED)
					{
						ZGetScreenEffectManager()->AddCTFEffect((MMatchTeam)nTeam, 0);
						SetRedCarrier(MUID(0,0));
						SetBlueFlagState(false);
						//ZGetScreenEffectManager()->AddScreenEffect("ctf_flagdrop_b");
						//ZGetSoundEngine()->PlaySound( "ctf_flagreturn" );
						//ZGetGameInterface()->PlayVoiceSound( VOICE_BLUE_FLAG_RETURN, 1600);
					}
					else if(nTeam == MMT_BLUE)
					{
						ZGetScreenEffectManager()->AddCTFEffect((MMatchTeam)nTeam, 0);
						SetBlueCarrier(MUID(0,0));
						SetRedFlagState(false);
						//ZGetScreenEffectManager()->AddScreenEffect("ctf_flagdrop_r");
						//ZGetSoundEngine()->PlaySound( "ctf_flagreturn" );
						//ZGetGameInterface()->PlayVoiceSound( VOICE_RED_FLAG_RETURN, 1600);
					}
				}
			}
			else
			{
				for (ZCharacterManager::iterator itor = ZGetGame()->m_CharacterManager.begin();
					itor != ZGetGame()->m_CharacterManager.end(); ++itor)
				{
					ZCharacter* pCharacter = (ZCharacter*)(*itor).second;
					if(pCharacter)
					{
						if(pCharacter->GetTeamID() == nTeam)
						{
							pCharacter->SetTagger(false);
						}
					}
				}
				if(nTeam == MMT_RED)
				{
					ZGetScreenEffectManager()->AddCTFEffect((MMatchTeam)nTeam, 0);
					SetRedCarrier(MUID(0,0));
					SetBlueFlagState(false);
					//ZGetScreenEffectManager()->AddScreenEffect("ctf_flagdrop_b");
					//ZGetSoundEngine()->PlaySound( "ctf_flagreturn" );
					//ZGetGameInterface()->PlayVoiceSound( VOICE_BLUE_FLAG_RETURN, 1600);
				}
				else if(nTeam == MMT_BLUE)
				{
					ZGetScreenEffectManager()->AddCTFEffect((MMatchTeam)nTeam, 0);
					SetBlueCarrier(MUID(0,0));
					SetRedFlagState(false);
					//ZGetScreenEffectManager()->AddScreenEffect("ctf_flagdrop_r");
					//ZGetSoundEngine()->PlaySound( "ctf_flagreturn" );
					//ZGetGameInterface()->PlayVoiceSound( VOICE_RED_FLAG_RETURN, 1600);
				}
			}
		}
		break;
	}

	return false;
}

void ZRuleTeamCTF::AssignFlagEffect(MUID& uidOwner, int nTeam)
{
	if (!ZGetGame()) return;

	for (ZCharacterManager::iterator itor = ZGetGame()->m_CharacterManager.begin();
	itor != ZGetGame()->m_CharacterManager.end(); ++itor)
	{
		ZCharacter* pCharacter = (ZCharacter*)(*itor).second;
		if(pCharacter)
		{
			if(pCharacter->GetTeamID() == nTeam)
			{
				pCharacter->SetTagger(false);
			}
		}
	}

	ZCharacter* pFlagChar = (ZCharacter*)ZGetGame()->m_CharacterManager.Find(uidOwner);
	if (pFlagChar)
	{
		// can't trust that nTeam variable D:
		ZGetEffectManager()->AddCTFEffect(pFlagChar, pFlagChar->GetTeamID());
		ZGetEffectManager()->AddBerserkerIcon(pFlagChar);
		pFlagChar->SetTagger(true);
		
		if(nTeam == MMT_BLUE)
		{
			SetBlueCarrier(uidOwner);
			SetRedFlagState(true);
			ZGetScreenEffectManager()->AddCTFEffect((MMatchTeam)nTeam, 1);
			//ZGetScreenEffectManager()->AddScreenEffect("ctf_taken_r");
			//ZGetSoundEngine()->PlaySound( "ctf_flagtaken" );
			//ZGetGameInterface()->PlayVoiceSound( VOICE_BLUE_HAS_FLAG, 1600);
		}
		else if (nTeam == MMT_RED)
		{
			SetRedCarrier(uidOwner);
			SetBlueFlagState(true);
			ZGetScreenEffectManager()->AddCTFEffect((MMatchTeam)nTeam, 1);
			//ZGetScreenEffectManager()->AddScreenEffect("ctf_taken_b");
			//ZGetSoundEngine()->PlaySound( "ctf_flagtaken" );
			//ZGetGameInterface()->PlayVoiceSound( VOICE_RED_HAS_FLAG, 1600);
		}
		if (nTeam == MMT_BLUE)
		{
			ZGetEffectManager()->AddBerserkerIcon(pFlagChar);
			ZGetEffectManager()->AddRedFlagIcon(pFlagChar);
			pFlagChar->SetTagger(true);
		}
		else if (nTeam == MMT_RED)
		{
			ZGetEffectManager()->AddBerserkerIcon(pFlagChar);
			ZGetEffectManager()->AddBlueFlagIcon(pFlagChar);
			pFlagChar->SetTagger(true);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////

ZRuleTeamInfected::ZRuleTeamInfected(ZMatch* pMatch) : ZRule(pMatch)
{
	m_bLastFive = false;
}

ZRuleTeamInfected::~ZRuleTeamInfected()
{

}

bool ZRuleTeamInfected::OnCommand(MCommand* pCommand)
{
	if (!ZGetGame()) return false;

	switch (pCommand->GetID())
	{
	case MC_MATCH_ASSIGN_COMMANDER:
		{
			MUID uidPlayer;

			pCommand->GetParameter(&uidPlayer,		0, MPT_UID);
			//pCommand->GetParameter(&uidBlueCommander,	1, MPT_UID);

			OnInfected(uidPlayer, true);
		}
		break;

	case MC_MATCH_INFECT:
		{
			MUID uidPlayer;

			pCommand->GetParameter(&uidPlayer, 0, MPT_UID);

			OnInfected(uidPlayer, false);
		}
		break;

	case MC_MATCH_LASTSURVIVOR:
		{
			MUID uidPlayer;
			pCommand->GetParameter(&uidPlayer, 0, MPT_UID);

			if (uidPlayer.IsInvalid())
				break;

			ZCharacter* pChar = (ZCharacter*) ZGetGame()->m_CharacterManager.Find(uidPlayer);
			if (pChar)
			{
				char szInfectedMsg[128];
				memset( szInfectedMsg, 0, sizeof( szInfectedMsg ) );

				if (pChar == ZGetGame()->m_pMyCharacter && !ZGetGameInterface()->GetCombatInterface()->GetObserverMode())
				{
					sprintf_s( szInfectedMsg, "You are the last survivor." );
					ZGetGameInterface()->GetCombatInterface()->UpdateCTFMsg( szInfectedMsg );
				}
				else
				{
					sprintf_s( szInfectedMsg, "'%s' is the last survivor.", pChar->GetUserName() );
					ZGetGameInterface()->GetCombatInterface()->UpdateCTFMsg( szInfectedMsg );
				}
			}

			ZGetSoundEngine()->PlaySound( "last_survivor" );
		}
		break;
	}

	return false;
}

void ZRuleTeamInfected::OnInfected(const MUID& uidPlayer, bool bFirst)
{
	if (!ZGetGame()) return;

	if (uidPlayer.IsInvalid()) return;

	char szTest[128];
	memset( szTest, 0, sizeof( szTest ) );

	//sprintf_s (szTest, "player [%u-%u] - infect call start", uidPlayer.High, uidPlayer.Low);
	//ZChatOutput(szTest, ZChat::CMT_SYSTEM);

	if (bFirst)
	{
		for (ZCharacterManager::iterator itor = ZGetGame()->m_CharacterManager.begin(); itor != ZGetGame()->m_CharacterManager.end(); ++itor)
		{
			ZGetEffectManager()->Clear();
			ZCharacter* pCharacter = (ZCharacter*) (*itor).second;
			pCharacter->m_dwStatusBitPackingValue.Ref().m_bCommander = false;
		}
	}

	ZCharacter* pChar = (ZCharacter*) ZGetGame()->m_CharacterManager.Find(uidPlayer);

	if(pChar)
	{
		if (pChar->m_bInfected)
		{
			//sprintf_s (szTest, "player [%u-%u] - infect FAIL (done)", uidPlayer.High, uidPlayer.Low);
			//ZChatOutput(szTest, ZChat::CMT_SYSTEM);
			return;
		}

		if (!pChar->GetInitialized() || pChar->IsAdminHide())
			return;

		//sprintf_s (szTest, "player [%u-%u] - infect OK", uidPlayer.High, uidPlayer.Low);
		//ZChatOutput(szTest, ZChat::CMT_SYSTEM);

		pChar->SetVisible(true);
		pChar->Revival();
		pChar->InfectCharacter(bFirst);
		pChar->OnScream();

		// because revival() resets the commander flag
		if (bFirst)
		{
			ZGetEffectManager()->AddCommanderIcon(pChar, 0);
			pChar->m_dwStatusBitPackingValue.Ref().m_bCommander = true;
			m_uidPatientZero = pChar->GetUID();
		}
		else
		{
			ZGetSoundEngine()->PlaySound( "scored" );
			ZCharacter* pMaster = (ZCharacter*) ZGetGame()->m_CharacterManager.Find(m_uidPatientZero);
			
			if (pMaster && pMaster == ZGetGame()->m_pMyCharacter)
			{
				pMaster->SetHP(500.f); // ZOMBIE_HP
			}
		}

		int nRandId = rand() % 14 + 1;

		char szSndName[128];
		memset( szSndName, 0, sizeof( szSndName ) );
		sprintf_s( szSndName, "zombie_voice_idle%d", nRandId );

		ZGetSoundEngine()->PlaySound(szSndName/*, pChar->GetPosition(), pChar->IsObserverTarget()*/);

		if ((!ZGetGameInterface()->GetCombatInterface()->GetObserverMode() && pChar == ZGetGame()->m_pMyCharacter) || (ZGetGameInterface()->GetCombatInterface()->GetObserverMode() && pChar == ZGetGameInterface()->GetCombatInterface()->GetObserver()->GetTargetCharacter()))
		{
			// don't release
			if (pChar == ZGetGame()->m_pMyCharacter)
				ZGetGame()->ReleaseObserver();

			ZGetCamera()->Shock(2000.0f, .7f, rvector(0.0f, 0.0f, -1.0f)); // Vibrate camera

			char szInfectedMsg[128];
			memset( szInfectedMsg, 0, sizeof( szInfectedMsg ) );

			sprintf_s( szInfectedMsg, "YOU are Infected. KILL THEM ALL!" );

			ZGetGameInterface()->GetCombatInterface()->UpdateCTFMsg( szInfectedMsg );
		}
	}
}

void ZRuleTeamInfected::OnSetRoundState( MMATCH_ROUNDSTATE roundState )
{
	switch (roundState)
	{
	case MMATCH_ROUNDSTATE_PRE_COUNTDOWN:
		{
			ZGetGameInterface()->GetCombatInterface()->UpdateCTFMsg("Zombies will spawn soon. HIDE!");
			m_bLastFive = false;
			m_uidPatientZero = MUID( 0, 0 );
			//ZChatOutput("HIDE NOW", ZChat::CMT_SYSTEM);
		}
		break;
	case MMATCH_ROUNDSTATE_COUNTDOWN:
		{
			//ZChatOutput("", ZChat::CMT_SYSTEM);
		}
		break;
	case MMATCH_ROUNDSTATE_PLAY:
		{
			if (ZGetGame()->IsReplay() && m_uidPatientZero.IsValid() && ZGetGame()->m_pMyCharacter->GetUID() == m_uidPatientZero && !ZGetGame()->m_pMyCharacter->m_bInfected)
				ZGetGame()->m_pMyCharacter->InfectCharacter(true);

			if (ZGetGame()->IsReplay() && m_uidPatientZero.IsValid() && ZGetGame()->m_pMyCharacter->GetTeamID() == MMT_RED && !ZGetGame()->m_pMyCharacter->m_bInfected)
				ZGetGame()->m_pMyCharacter->InfectCharacter(false);

			ZGetGameInterface()->GetCombatInterface()->UpdateCTFMsg("RUN!");
			ZChatOutput("RUN!", ZChat::CMT_SYSTEM);
		}
		break;

	case MMATCH_ROUNDSTATE_FREE:
		{
			ZGetGameInterface()->GetCombatInterface()->UpdateCTFMsg("4 or more players are required to start the game.");
			m_bLastFive = false;
			m_uidPatientZero = MUID( 0, 0 );
			//ZChatOutput("RS -> (Free)", ZChat::CMT_SYSTEM);
		}
		break;
	}
}

void ZRuleTeamInfected::OnResponseRuleInfo(MTD_RuleInfo* pInfo)
{
	MTD_RuleInfo_Assassinate* pAssassinateRule = (MTD_RuleInfo_Assassinate*)pInfo;
	OnInfected(pAssassinateRule->uidRedCommander, true);
}

void ZRuleTeamInfected::OnUpdate(float fDelta)
{
	if ( ZGetGame()->GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_PLAY)
	{
		DWORD dwTime = ZGetGame()->GetMatch()->GetRemaindTime();
		DWORD dwLimitTime = ZGetGameClient()->GetMatchStageSetting()->GetStageSetting()->nLimitTime;
		if ( dwLimitTime != 99999)
		{
			dwLimitTime *= 60000;
			if ( dwTime <= dwLimitTime)
			{
				dwTime = (dwLimitTime - dwTime) / 1000;

				if ((dwTime / 60) == 0 && (dwTime % 60) == 5 && !m_bLastFive)
				{
					m_bLastFive = true;
					ZGetSoundEngine()->PlaySound( "time_tick_last5" );
				}
			}
		}
	}

}

/////////////////////////////////////////////////////////////////////////////////////////
bool CompareGunGameChar(ZCharacter* a,ZCharacter* b) {
	if(a->m_nGunGameWeaponLevel < b->m_nGunGameWeaponLevel) return true;
	if(a->m_nGunGameWeaponLevel > b->m_nGunGameWeaponLevel) return false;

	return false;
}

ZRuleGunGame::ZRuleGunGame(ZMatch* pMatch) : ZRule(pMatch)
{
	m_nWeaponMaxLevel = 0;
	m_vPlayerData.clear();
}

ZRuleGunGame::~ZRuleGunGame()
{
	m_vPlayerData.clear();
}

bool ZRuleGunGame::OnCommand(MCommand* pCommand)
{
	if (!ZGetGame()) return false;

	switch (pCommand->GetID())
	{
	case MC_MATCH_GUNGAME_WEAPONDATA:
		{
			int nWeaponMaxLevel;
			pCommand->GetParameter(&nWeaponMaxLevel, 0, MPT_INT);

			m_nWeaponMaxLevel = nWeaponMaxLevel;

			MCommandParameter* pParam = pCommand->GetParameter(1);
			if(pParam->GetType()!=MPT_BLOB) break;
			void* pBlob = pParam->GetPointer();
			int nCount = MGetBlobArrayCount(pBlob);

			if (MGetBlobArraySize(pBlob) != (sizeof(MTD_GunGameWeaponInfo) * nCount) + (sizeof(int) * 2))
				break;

			for (int i = 0; i < nCount; ++i)
			{
				MTD_GunGameWeaponInfo* pInfo = (MTD_GunGameWeaponInfo*)MGetBlobArrayElement(pBlob, i);

				if (pInfo->uidPlayer.IsInvalid())
					continue;
				ZCharacter* pCharacter = (ZCharacter*) ZGetGame()->GetCharacterMgr()->Find(pInfo->uidPlayer);

				if (!pCharacter)
					continue;

				pCharacter->m_nGunGameWeaponLevel = pInfo->nWeaponLevel;

				unsigned long nEquipedItemDesc[MMCIP_END], nEquipedItemCount[MMCIP_END];
				memset(nEquipedItemDesc, 0, sizeof( nEquipedItemDesc ));
				memset(nEquipedItemCount, 0, sizeof( nEquipedItemCount ));

				// copy item list
				for (int i = 0; i < MMCIP_END; ++i)
				{
					ZItem* pItem = pCharacter->m_Items.GetItem((MMatchCharItemParts)i);
					if (pItem)
					{
						nEquipedItemDesc[i] = pItem->GetDescID();
						nEquipedItemCount[i] = pItem->GetItemCount();
					}
				}

				nEquipedItemDesc[MMCIP_MELEE] = pInfo->nWeaponID[0];
				nEquipedItemDesc[MMCIP_PRIMARY] = pInfo->nWeaponID[1];
				nEquipedItemDesc[MMCIP_SECONDARY] = pInfo->nWeaponID[2];

				memcpy(pCharacter->m_nGunGameWeaponID, pInfo->nWeaponID, sizeof(pCharacter->m_nGunGameWeaponID));

				nEquipedItemCount[MMCIP_MELEE] = pInfo->nWeaponID[0] > 0 ? 1 : 0;
				nEquipedItemCount[MMCIP_PRIMARY] = pInfo->nWeaponID[1] > 0 ? 1 : 0;
				nEquipedItemCount[MMCIP_SECONDARY] = pInfo->nWeaponID[2] > 0 ? 1 : 0;

				int nTotalItems = nEquipedItemCount[MMCIP_MELEE] + nEquipedItemCount[MMCIP_PRIMARY] + nEquipedItemCount[MMCIP_SECONDARY];

				if (!pCharacter->IsAdminHide() && pCharacter->m_nGunGameWeaponLevel > 1)
				{
					char szBuf[128];
					memset( szBuf, 0, sizeof(szBuf) );

					if (pCharacter == ZGetGame()->m_pMyCharacter)
					{
						if (pCharacter->m_nGunGameWeaponLevel == m_nWeaponMaxLevel)
						{
							sprintf_s(szBuf, "You are on the Final Level!");
						}
						else
							sprintf_s(szBuf, "You are on Level %d, with %d items", pCharacter->m_nGunGameWeaponLevel, nTotalItems);
					}
					else
					{
						if (pCharacter->m_nGunGameWeaponLevel == m_nWeaponMaxLevel)
						{
							sprintf_s(szBuf, "%s is on the Final Level!", pCharacter->GetUserAndClanName());
						}
						else
						{
							if (pCharacter->m_nGunGameWeaponLevel % 5 == 0)
								sprintf_s(szBuf, "%s is on Level %d", pCharacter->GetUserAndClanName(), pCharacter->m_nGunGameWeaponLevel);
						}
					}

					if (strlen(szBuf) > 0)
						ZChatOutput( MCOLOR(ZCOLOR_CHAT_BROADCAST), szBuf );
				}

				ZChangeCharParts(pCharacter->m_pVMesh, pCharacter->GetProperty()->nSex, pCharacter->GetProperty()->nHair, pCharacter->GetProperty()->nFace, nEquipedItemDesc);

				for (int i = 0; i < MMCIP_END; i++)
				{
					pCharacter->m_Items.EquipItem(MMatchCharItemParts(i), nEquipedItemDesc[i], nEquipedItemCount[i]);
				}

				pCharacter->ChangeWeapon(MMCIP_MELEE);
				MMatchItemDesc* pSelectedItemDesc = pCharacter->m_Items.GetSelectedWeapon()->GetDesc();

				if (pSelectedItemDesc==NULL)
				{
					pCharacter->m_Items.SelectWeapon(MMCIP_MELEE);
				}
				else
					pCharacter->OnChangeWeapon(pSelectedItemDesc->m_pMItemName->Ref().m_szMeshName);

				pCharacter->InitItemBullet();

				// lol stupid impl
				pCharacter->ChangeWeapon(MMCIP_PRIMARY);
				pSelectedItemDesc = pCharacter->m_Items.GetSelectedWeapon()->GetDesc();

				if (pSelectedItemDesc==NULL)
				{
					pCharacter->m_Items.SelectWeapon(MMCIP_PRIMARY);
				}
				else
					pCharacter->OnChangeWeapon(pSelectedItemDesc->m_pMItemName->Ref().m_szMeshName);
			}

			//list<ZCharacter*> chars;
			//for (ZCharacterManager::iterator itor = ZGetGame()->m_CharacterManager.begin(); itor != ZGetGame()->m_CharacterManager.end(); ++itor)
			//{
			//	ZCharacter* pCharacter = (ZCharacter*)(*itor).second;
			//	if(pCharacter)
			//	{
			//		if (!pCharacter->GetInitialized() || pCharacter->IsAdminHide())
			//			continue;

			//		chars.push_back(pCharacter);
			//	}
			//}

			//chars.sort(CompareGunGameChar);

			//if (!chars.empty() && chars.front() != NULL && chars.front()->m_nGunGameWeaponLevel > 1)
			//{
			//	char szBuf[128];
			//	sprintf_s(szBuf, "%s is leading the race!", chars.front()->GetUserAndClanName());
			//	ZChatOutput( MCOLOR(ZCOLOR_CHAT_BROADCAST), szBuf );
			//}
		}
		break;
	}

	return false;
}

ZRuleRun::ZRuleRun(ZMatch* pMatch) : ZRule(pMatch)
{

}

ZRuleRun::~ZRuleRun()
{

}

void ZRuleRun::OnSetRoundState(MMATCH_ROUNDSTATE roundstate)
{
	switch (roundstate)
	{
		case MMATCH_ROUNDSTATE_FINISH:
		{
		}break;
		case MMATCH_ROUNDSTATE_PREPARE:
		{
		}
		case MMATCH_ROUNDSTATE_PLAY:
		{

		}
		case MMATCH_ROUNDSTATE_COUNTDOWN://_FREE for singlep testing
		{
		}
	}
}
ZRuleTurbo::ZRuleTurbo(ZMatch* pMatch) : ZRule(pMatch)
{

}

ZRuleTurbo::~ZRuleTurbo()
{

}
//void ZRuleTurbo::OnUpdate(float fElapsed)
//{
//	if (ZGetGame()->m_pMyCharacter->GetPosition().z <= -2400.f)
//	{
//		ZMapSpawnData* spawnData = ZGetGame()->GetWorld()->GetDesc()->GetSpawnManager()->GetSoloRandomData();
//		ZGetGame()->m_pMyCharacter->SetPosition(spawnData->m_Pos);
//
//		ZGetEffectManager()->AddReBirthEffect(ZGetGame()->m_pMyCharacter->GetPosition());
//	}
//}