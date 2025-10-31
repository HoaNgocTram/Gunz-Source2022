#include "stdafx.h"
#include "ZRuleQuestChallenge.h"
#include "ZMatch.h"
#include "MActorDef.h"
#include "ZFSMManager.h"
#include "ZActorActionManager.h"
#include "ZActorAction.h"
#include "ZActorWithFSM.h"
#include "ZApplication.h"
#include "ZFSMManager.h"
#include "ZFSMParser.h"
#include "ZFSM.h"
#include "ZScreenEffectManager.h"
#include "ZBmNumLabel.h"

ZRuleQuestChallenge::ZRuleQuestChallenge(ZMatch* pMatch) : IRuleNewQuest(pMatch)
, m_pScenario(NULL)
, m_nCurrSector(0)
, m_nRewardXP(0)
, m_nRewardBP(0)
{
	m_uidBoss.SetZero();
	Init();
	m_nNPC = 0;
	m_szElapsedTime = "";
	m_fElapsedTime = ZGetGame()->GetTime();
	m_OurCQRewards.clear();
	m_FadeBGTime = 150;
	m_bLateJoin = false;
}

ZRuleQuestChallenge::~ZRuleQuestChallenge()
{
	ZGetScreenEffectManager()->DestroyQuestRes();
}

bool ZRuleQuestChallenge::LoadScenarioMap(const char* szScenarioName)
{
	MNewQuestScenarioManager* pScenarioMgr = ZApplication::GetStageInterface()->GetChallengeQuestScenario();
	if (!pScenarioMgr) { _ASSERT(0); return false; }
	MNewQuestScenario* pScenario = pScenarioMgr->GetScenario(szScenarioName);
	if (!pScenario) { _ASSERT(0); return false; }

	int numSector = pScenario->GetNumSector();
	for (int i=0; i<numSector; ++i)
	{
		MNewQuestSector* pSector = pScenario->GetSector(i);
		if (!pSector)  { _ASSERT(0); return false; }

		const char* szMapName = pSector->GetMapName();
		_ASSERT(0 != strlen(szMapName));
		ZGetWorldManager()->AddWorld(szMapName);
	}

	return true;
}

DWORD ZRuleQuestChallenge::GetCurrTime()
{
	return m_fElapsedTime;
}

int ZRuleQuestChallenge::GetRoundMax()
{
	if (!m_pScenario) {
		_ASSERT(0);
		return 1;
	}

	return m_pScenario->GetNumSector();
}

int ZRuleQuestChallenge::GetCurrSector()
{
	if (!m_pScenario) {
		_ASSERT(0);
		return 1;
	}

	return m_nCurrSector;
}

int ZRuleQuestChallenge::GetNPC()
{
	if (!m_pScenario) {
		_ASSERT(0);
		return 1;
	}

	return m_nNPC;
}

bool ZRuleQuestChallenge::Init()
{
	//todok quest 게임을 할때마다 새로 로딩하는 것을 개선하자.

	if (!m_pActorActionMgr->ReadXml(ZApplication::GetFileSystem(), "system/zactoraction.xml"))
	{
		mlog("Error while Load actoraction.\n");
		return false;
	}
	vector<ZFSM*> vecFSM;
	ZFSMParser fsmParser;
	if (!fsmParser.ReadXml(ZApplication::GetFileSystem(), "system/aifsm.xml", vecFSM, m_pActorActionMgr))
	{
		mlog("Error while Load aifsm.\n");
		return false;
	}
	m_pFsmManager->AddFsm(&vecFSM[0], (unsigned int)vecFSM.size());

	if (ZGetNpcMeshMgr()->LoadXmlList("model/npc2.xml") == -1)
	{
		mlog("Error while Load NPC Models.\n");
		return false;
	}
	mlog("npc2 model has loaded successfully! \n");
	if (!m_pActorDefMgr->ReadXml(ZApplication::GetFileSystem(), "system/npc2.xml"))
	{
		mlog("Error while Read npc2.\n");
		return false;
	}
	mlog("npc2 has loaded successfully! \n");
	MNewQuestScenarioManager* pScenarioMgr = ZApplication::GetStageInterface()->GetChallengeQuestScenario();
	m_pScenario = pScenarioMgr->GetScenario(ZGetGameClient()->GetMatchStageSetting()->GetMapName());

	m_nCurrSector = 0;
	m_nNPC = 0;
	ZGetScreenEffectManager()->CreateQuestRes();
	return true;
}

void ZRuleQuestChallenge::OnUpdate( float fDelta )
{
	//IRuleNewQuest::OnUpdate(fDelta);

	if (ZGetGame() == NULL) return;

	if (ZGetCombatInterface() && !ZGetCombatInterface()->IsShowResult())
	{
		m_fElapsedTime = ZGetGame()->GetTime();
		DWORD dwTime = m_fElapsedTime;
		char szText[128];
		sprintf(szText, "%s : %d:%02d", "Time Passed", dwTime / 60, dwTime % 60);
		m_szElapsedTime = szText;
	}

}

void ZRuleQuestChallenge::OnSetRoundState( MMATCH_ROUNDSTATE roundState )
{
	switch(roundState) 
	{
	case MMATCH_ROUNDSTATE_PREPARE: 
		{
		}
		break;
	case MMATCH_ROUNDSTATE_PRE_COUNTDOWN:
		{
		}
		break;
	case MMATCH_ROUNDSTATE_COUNTDOWN : 
		{
		}
		break;
	case MMATCH_ROUNDSTATE_PLAY:
		{
		}
		break;
	case MMATCH_ROUNDSTATE_FINISH:
		{
		}
		break;
	case MMATCH_ROUNDSTATE_EXIT:
		{
		}
		break;
	case MMATCH_ROUNDSTATE_FREE:
		{
		}
		break;
	};
}

bool ZRuleQuestChallenge::OnCommand( MCommand* pCommand )
{
	if (!ZGetGame())
		return false;

	switch (pCommand->GetID())
	{
	case MC_NEWQUEST_MOVE_TO_NEXT_SECTOR:
		{
			MoveToNextSector();
			return true;
		}break;

	case MC_QUEST_SECTOR_BONUS:
		{
			OnSectorBonus(pCommand);
			return true;
		}break;
	case MC_MATCH_LATEJOIN_CQ:
	{
		MUID uidPlayer;
		int currSector;
		UINT currTime;
		pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
		pCommand->GetParameter(&currSector, 1, MPT_INT);
		pCommand->GetParameter(&currTime, 2, MPT_UINT);

		ZCharacter* pCharacter = dynamic_cast<ZCharacter*>(ZGetCharacterManager()->Find(uidPlayer));
		pCharacter->ForceDie();
		if (uidPlayer == ZGetGame()->m_pMyCharacter->GetUID())
		{
			SetElapsedTime(currTime); //1 second time offset?
			MoveToRealSector(currSector);
		}
		return true;
		}break;
	case MC_NEWQUEST_NPC_SPAWN:
	{
		MUID uidController;
		MUID uidNpc;
		char szActorDefName[128];
		unsigned char nCustomSpawnTypeIndex, nSpawnIndex;
		pCommand->GetParameter(&uidController, 0, MPT_UID);
		pCommand->GetParameter(&uidNpc, 1, MPT_UID);
		pCommand->GetParameter(szActorDefName, 2, MPT_STR, sizeof(szActorDefName));
		pCommand->GetParameter(&nCustomSpawnTypeIndex, 3, MPT_UCHAR);
		pCommand->GetParameter(&nSpawnIndex, 4, MPT_UCHAR);

		bool mycontrol = false;
		if (ZGetGame()->m_pMyCharacter->IsAdminName())
			mycontrol = true;
		SpawnActor(szActorDefName, uidNpc, nCustomSpawnTypeIndex, nSpawnIndex, ZGetMyUID() == uidController);
		return true;
	}break;

	case MC_NEWQUEST_NPC_DEAD:
	{
		MUID uidKiller, uidNPC;

		pCommand->GetParameter(&uidKiller, 0, MPT_UID);
		pCommand->GetParameter(&uidNPC, 1, MPT_UID);

		ZActorWithFSM* pActor = (ZActorWithFSM*)ZGetObjectManager()->GetNPCObject(uidNPC);
		if (pActor)
		{
			ZGetObjectManager()->Delete(pActor);

			//todok 아래 주석 구현해줘야 함
			//m_GameInfo.IncreaseNPCKilled();

			ZCharacter* pCharacter = (ZCharacter*)ZGetCharacterManager()->Find(uidKiller);
			if (pCharacter)
			{
				pCharacter->GetStatus().Ref().nKills += 1;
				if (uidKiller == ZGetGame()->m_pMyCharacter->GetUID())
					ZGetScreenEffectManager()->AddKO();

				pCharacter->GetStatus().MakeCrc();
			}
			m_nNPC--;
		}
		return true;
	}
	break;

	case MC_NEWQUEST_PEER_NPC_BASICINFO:
	{
		MCommandParameter* pParam = pCommand->GetParameter(0);
		if (pParam->GetType() != MPT_BLOB)
		{
			_ASSERT(0); break;
		}

		ZACTOR_WITHFSM_BASICINFO* pbi = (ZACTOR_WITHFSM_BASICINFO*)pParam->GetPointer();
		ZActorWithFSM* pActor = (ZActorWithFSM*)ZGetObjectManager()->GetNPCObject(pbi->uidNPC);
		if (pActor)
		{
			pActor->OnBasicInfo(pbi);
		}

		return true;
	}break;
	case MC_NEWQUEST_PEER_NPC_ACTION_EXECUTE:
	{
		MUID uidNpc;
		int nActionIndex;
		pCommand->GetParameter(&uidNpc, 0, MPT_UID);
		pCommand->GetParameter(&nActionIndex, 1, MPT_INT, sizeof(nActionIndex));

		ZActorBase* pActor = ZGetObjectManager()->GetNPCObject(uidNpc);
		ZActorWithFSM* pActorWithFSM = MDynamicCast(ZActorWithFSM, pActor);
		if (pActorWithFSM)
		{
			pActorWithFSM->OnPeerActionExecute(nActionIndex);
		}

		return true;
	}break;
	case MC_NEWQUEST_REQUEST_NPC_SPAWN:
	{
		MUID uidController;
		MUID uidNpc;
		char szActorDefName[128];
		int num, route;
		MShortVector dir, pos;
		pCommand->GetParameter(&uidController, 0, MPT_UID);
		pCommand->GetParameter(&uidNpc, 1, MPT_UID);
		pCommand->GetParameter(&num, 2, MPT_INT);
		pCommand->GetParameter(szActorDefName, 3, MPT_STR, sizeof(szActorDefName));
		pCommand->GetParameter(&pos, 4, MPT_SVECTOR);
		pCommand->GetParameter(&dir, 5, MPT_SVECTOR);
		pCommand->GetParameter(&route, 6, MPT_INT);


		bool mycontrol = false;
		if (ZGetGame()->m_pMyCharacter->IsAdminName())
			mycontrol = true;
		SpawnActorSummon(szActorDefName, uidNpc, num, ZGetMyUID() == uidController, pos, dir, route);
		return true;
	}break;
	case MC_NEWQUEST_NEW_CONTROLLER:
	{
		MUID newController, npcUID;
		pCommand->GetParameter(&newController, 0, MPT_UID);
		pCommand->GetParameter(&npcUID, 1, MPT_UID);
		SetNewNpcController(newController, npcUID);
		return true;
	}break;
	case MC_MATCH_QUEST_PLAYER_DEAD:
	{
		MUID uidVictim;
		pCommand->GetParameter(&uidVictim, 0, MPT_UID);

		ZCharacter* pVictim = (ZCharacter*)ZGetCharacterManager()->Find(uidVictim);

		if (pVictim)
		{
			if (pVictim != ZGetGame()->m_pMyCharacter)
			{
				pVictim->Die();		
			}

			pVictim->GetStatus().CheckCrc();
			pVictim->GetStatus().Ref().AddDeaths();
			if (pVictim->GetStatus().Ref().nLife > 0)
				pVictim->GetStatus().Ref().nLife--;
			pVictim->GetStatus().MakeCrc();
		}
		return true;
	}break;
	case MC_NEWQUEST_LATE_NPC_SPAWN:
	{
		MUID uidLateJoiner;
		MUID uidNpc;
		char szActorDefName[128];
		unsigned char nCustomSpawnTypeIndex, nSpawnIndex;
		pCommand->GetParameter(&uidLateJoiner, 0, MPT_UID);
		pCommand->GetParameter(&uidNpc, 1, MPT_UID);
		pCommand->GetParameter(szActorDefName, 2, MPT_STR, sizeof(szActorDefName));

		if (uidLateJoiner == ZGetMyUID())
			SpawnLateJoinNpc(szActorDefName, uidNpc, nCustomSpawnTypeIndex, nSpawnIndex);
		return true;
	}break;

	break;
	default:
		break;
	}

	return false;
}



void ZRuleQuestChallenge::MoveToNextSector()
{
	ZCharacter* pMyChar = ZGetGame()->m_pMyCharacter;
	pMyChar->InitStatus();
	pMyChar->SetVisible(false);

	ZGetEffectManager()->Clear();
	ZGetGame()->m_WeaponManager.Clear();

	++m_nCurrSector;
	ZGetWorldManager()->SetCurrent(m_nCurrSector);

}

void ZRuleQuestChallenge::MoveToRealSector(int currSector)
{
	ZCharacter* pMyChar = ZGetGame()->m_pMyCharacter;
	ZGetCombatInterface()->SetObserverMode(true);
	pMyChar->InitStatus();
	pMyChar->SetVisible(false);
	pMyChar->ForceDie();
	pMyChar->Die();
	m_nCurrSector = currSector;
	ZGetWorldManager()->SetCurrent(currSector);
}

bool ZRuleQuestChallenge::OnSectorBonus(MCommand* pCommand)
{
	MUID uidPlayer;
	unsigned long int nRewardXP = 0;
	unsigned long int nRewardBP = 0;
	unsigned long int nLevelPct = 0;
	pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
	pCommand->GetParameter(&nRewardXP, 1, MPT_UINT);
	pCommand->GetParameter(&nRewardBP, 2, MPT_UINT);
	pCommand->GetParameter(&nLevelPct, 3, MPT_UINT);

	if (ZGetCharacterManager()->Find(uidPlayer) == ZGetGame()->m_pMyCharacter)
	{
		char szText[256];
		ZGetMyInfo()->SetXP(ZGetMyInfo()->GetXP() + nRewardXP);
		ZGetMyInfo()->SetLevelPercent(nLevelPct);
		ZGetScreenEffectManager()->SetGaugeExpFromMyInfo();
		ZGetMyInfo()->SetBP(ZGetMyInfo()->GetBP() + nRewardBP);
		m_nRewardXP += nRewardXP;
		m_nRewardBP += nRewardBP;
		ZGetScreenEffectManager()->AddExpEffect(nRewardXP);		// 획득 경험치 표시 없애기
		//sprintf(szText, "%s %d", ZMsg(MSG_WORD_EXPBONUSWON), nRewardXP, nRewardBP);
		sprintf(szText, "You have won %d XP and %d BP for clearing the round", nRewardXP, nRewardBP);
		ZChatOutput(ZCOLOR_CHAT_SYSTEM, szText);
	}

	return true;
}

ZActorWithFSM* ZRuleQuestChallenge::SpawnActor(const char* szActorDefName, MUID& uid, int nCustomSpawnTypeIndex, int nSpawnIndex, bool bMyControl)
{
	ZActorWithFSM* pActor =  IRuleNewQuest::SpawnActor(szActorDefName, uid, nCustomSpawnTypeIndex, nSpawnIndex, bMyControl);
	if (pActor == nullptr)
	{
		return nullptr;
	}
	if (pActor->GetActorDef()->IsBoss())
	{
		m_uidBoss = pActor->GetUID();
		if (StrStr(pActor->GetActorDef()->GetName(), "tower"))
			pActor->SetStationary(true);
	}
	return pActor;
}

ZActorWithFSM* ZRuleQuestChallenge::SpawnActorSummon(const char* szActorName, MUID& uid, int nNum, bool bMyControl,
	MShortVector vec, MShortVector vec2, int route)
{
	return IRuleNewQuest::SpawnActorSummon(szActorName, uid, nNum, bMyControl, vec, vec2, route);
}

ZActorWithFSM* ZRuleQuestChallenge::SpawnLateJoinNpc(const char* szActorDefName, MUID& uid, int nCustomSpawnTypeIndex, int nSpawnIndex, bool bMyControl)
{
	ZActorWithFSM* lateJoinNPC = IRuleNewQuest::SpawnLateJoinNpc(szActorDefName, uid, nCustomSpawnTypeIndex, nSpawnIndex, bMyControl);
	ZGetEffectManager()->AddReBirthEffect(lateJoinNPC->GetPosition());

	return lateJoinNPC;
}
