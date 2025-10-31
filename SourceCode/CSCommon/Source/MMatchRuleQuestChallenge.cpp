#include "stdafx.h"
#include "MMatchRuleQuestChallenge.h"
#include "MNewQuestNpcManager.h"
#include "MNewQuestPlayerManager.h"
#include "MSharedCommandTable.h"
#include "MMatchWorldItemDesc.h"
#include "MMatchFormula.h"

#define FILENAME_NPC2_XML		"NewQuest/npc2.xml"
#define FILENAME_SCENARIO2_XML	"NewQuest/scenario2.xml"
MNewQuestScenarioManager MMatchRuleQuestChallenge::ms_scenarioMgr;
void MMatchRuleQuestChallenge::InitActorDefMgr()
{
	if (!ms_actorDefMgr.ReadXml(NULL, FILENAME_NPC2_XML))
	{
		mlog("Read NPC2 %s Failed\n", FILENAME_NPC2_XML);
		_ASSERT(0); return;
	}
	mlog("Read NPC 2 (Challenge Quest).\n");
}
void MMatchRuleQuestChallenge::InitScenarioMgr()
{
	if (!ms_scenarioMgr.ReadXml(NULL, FILENAME_SCENARIO2_XML, MMatchServer::GetInstance()->GetQuest()->GetDropTable()))
	{
		mlog("Read new quest file %s Failed\n", FILENAME_SCENARIO2_XML);
		_ASSERT(0); return;
	}
	mlog("Read Scenario (Challenge Quest).\n");
}

MNewQuestScenarioManager* MMatchRuleQuestChallenge::GetScenarioMgr()
{
	return &ms_scenarioMgr;
}


MMatchRuleQuestChallenge::MMatchRuleQuestChallenge(MMatchStage* pStage)
: IMatchRuleNewQuest(pStage)
, m_pScenario(NULL)
, m_nCurrSector(0)
{
	//LoadTempINI();
}

MMatchRuleQuestChallenge::~MMatchRuleQuestChallenge()
{
}

// 모든 라운드가 끝나면 false 반환
bool MMatchRuleQuestChallenge::RoundCount()
{
	if (!m_pScenario)
	{
		_ASSERT(0);
		return false;
	}

	if (m_nCurrSector < m_pScenario->GetNumSector())
	{
		++m_nCurrSector;
		return true;
	}

	return false;
}

void MMatchRuleQuestChallenge::OnBegin()
{
	if (m_pPlayerMgr)
	{
		delete m_pPlayerMgr;
		m_pPlayerMgr = 0;
	}
	m_pPlayerMgr = new MNewQuestPlayerManager;
	if (m_pNpcMgr)
	{
		delete m_pNpcMgr;
		m_pNpcMgr = 0;
	}
	m_pNpcMgr = new MNewQuestNpcManager;
	m_nCurrSector = 0;

	//todok 플레이어 접속 종료 및 방 나갈때 제거해줘야 한다

	// 시나리오 세팅
	m_pScenario = ms_scenarioMgr.GetScenario(m_pStage->GetMapName());
	if (!m_pScenario)
		m_pScenario = ms_scenarioMgr.GetScenario(ms_scenarioMgr.GetDefaultScenarioName());

	// 게임에 존재하는 플레이어 추가
	for (MUIDRefCache::iterator it = m_pStage->GetObjBegin(); it != m_pStage->GetObjEnd(); ++it)
	{
		MUID uidChar = it->first;

		MMatchObject* pObj = MMatchServer::GetInstance()->GetObject(uidChar);
		if (IsAdminGrade(pObj) && pObj->GetAccountInfo()->m_nUGrade == MMUG_EVENTTEAM
			&& pObj->CheckPlayerFlags(MTD_PlayerFlags_AdminHide)) continue;

		m_pPlayerMgr->AddPlayer(pObj);
	}
	MGetMatchServer()->OnStartCQ(m_pStage, m_pScenario->GetNumSector());
//	CollectStartingQuestChallengeGameLogInfo();
	//	m_dwCurrTime = MMatchServer::GetInstance()->GetTickTime();
}

void MMatchRuleQuestChallenge::OnEnd()
{

	for (MUIDRefCache::iterator i = m_pStage->GetObjBegin(); i != m_pStage->GetObjEnd(); i++)
	{
		MMatchObject* pObj = NULL;

		pObj = (MMatchObject*)(*i).second;
		if (IsEnabledObject(pObj))
			MGetMatchServer()->ResponseCharacterItemList(pObj->GetUID());
	}
	//requires user to successfully complete the game itself
	//if (m_nCurrSector == m_pScenario->GetNumSector() && m_pNpcMgr->GetNumNpc() == 0)
	//{
	//	PostInsertQuestChallengeGameLogAsyncJob();
	//}

	if (m_pNpcMgr)
	{
		delete m_pNpcMgr;
		m_pNpcMgr = 0;
	}
	if (m_pPlayerMgr)
	{
		delete m_pPlayerMgr;
		m_pPlayerMgr = 0;
	}
}
//void MMatchRuleQuestChallenge::CollectStartingQuestChallengeGameLogInfo()
//{
//	// 수집하기전에 이전의 정보를 반드시 지워야 함.
//	m_QuestChallengeGameLogInfoMgr.Clear();
//
//	//if( QuestTestServer() ) 
//	//{
//	// Master CID
//	MMatchObject* pMaster = MMatchServer::GetInstance()->GetObject(GetStage()->GetMasterUID());
//	if (IsEnabledObject(pMaster))
//		m_QuestChallengeGameLogInfoMgr.SetMasterCID(pMaster->GetCharInfo()->m_nCID);
//
//	m_QuestChallengeGameLogInfoMgr.SetScenarioID(MGetStringResManager()->GetStringFromXml(m_pScenario->GetName()));
//
//	// Stage name 저장.
////	m_QuestChallengeGameLogInfoMgr.SetStageName( GetStage()->GetName() );
//
//	// 시작할때의 유저 정보를 저장함.
//	for (MUIDRefCache::iterator itor = m_pStage->GetObjBegin(); itor != m_pStage->GetObjEnd(); ++itor)
//	{
//		MMatchObject* pObj = (MMatchObject*)(*itor).second;
//		m_QuestChallengeGameLogInfoMgr.AddQuestPlayer(pObj->GetUID(), pObj);
//	}
//
//	m_QuestChallengeGameLogInfoMgr.SetStartTime(timeGetTime());
//	//}
//}
//void MMatchRuleQuestChallenge::CollectEndQuestChallengeGameLogInfo()
//{
//	m_QuestChallengeGameLogInfoMgr.SetEndTime(timeGetTime());
//}
void MMatchRuleQuestChallenge::OnRoundBegin()
{
	// PLAY 상태로 넘어갈때 전부 스폰 시킨다
	ProcessNpcSpawning();

	MMatchRule::OnRoundBegin();
}

void MMatchRuleQuestChallenge::OnRoundEnd()
{
	if (m_pStage)
		m_pStage->m_WorldItemManager.CleanUpCQItems();
	if (m_nCurrSector < m_pScenario->GetNumSector())
	{
		RouteXpBpBonus();
		++m_nCurrSector;
		RouteMoveToNextSector(m_nCurrSector);
	}

	MMatchRule::OnRoundEnd();
}

void MMatchRuleQuestChallenge::OnSectorBonus(MMatchObject* pObj, const unsigned int nAddedXP, const unsigned int nAddedBP)
{
	MCommand* pCmd = MGetMatchServer()->CreateCommand(MC_QUEST_SECTOR_BONUS, MUID(0, 0));
	pCmd->AddParameter(new MCmdParamUID(pObj->GetUID()));
	pCmd->AddParameter(new MCmdParamUInt(nAddedXP));
	pCmd->AddParameter(new MCmdParamUInt(nAddedBP));

	MGetMatchServer()->RouteToListener(pObj, pCmd);
}

//oid MMatchRuleQuestChallenge::RouteExpToPlayers()
void MMatchRuleQuestChallenge::RouteXpBpBonus()
{
	for (MUIDRefCache::iterator it = m_pStage->GetObjBegin(); it != m_pStage->GetObjEnd(); ++it)
	{
		MUID uidChar = it->first;

		MMatchObject* pObj = MMatchServer::GetInstance()->GetObject(uidChar);
		if (IsAdminGrade(pObj) && pObj->GetAccountInfo()->m_nUGrade == MMUG_EVENTTEAM &&
			pObj->CheckPlayerFlags(MTD_PlayerFlags_AdminHide)) continue;
		if ((!IsEnabledObject(pObj)) || (!pObj->CheckAlive())) continue;

		//EXP
		int nXP = 0;
		int nBP = 0;

		MNewQuestSector* pCurrSector = NULL;
		if (m_pScenario)
		{
			MNewQuestSector* pCurrSector = m_pScenario->GetSector(m_nCurrSector);
			if (pCurrSector)
			{
				nXP = pCurrSector->GetRewardXp();
				nBP = pCurrSector->GetRewardBp();
			}
		}

		const float fXPBonusRatio = MMatchFormula::CalcXPBonusRatio(pObj, MIBT_QUEST);
		const float fBPBonusRatio = MMatchFormula::CalcBPBounsRatio(pObj, MIBT_QUEST);


		int nExpBonus = (int)(nXP * fXPBonusRatio);
		nXP += nExpBonus;

		int nBPBonus = nBP * fBPBonusRatio;
		nBP += nBPBonus;

		MGetMatchServer()->ProcessPlayerXPBP(m_pStage, pObj, nXP, nBP);


		int nExpPercent = MMatchFormula::GetLevelPercent(pObj->GetCharInfo()->m_nXP,
			pObj->GetCharInfo()->m_nLevel);

		MCommand* pNewCmd = MMatchServer::GetInstance()->CreateCommand(MC_QUEST_SECTOR_BONUS, MUID(0, 0));
		pNewCmd->AddParameter(new MCmdParamUID(pObj->GetUID()));
		pNewCmd->AddParameter(new MCmdParamUInt(nXP));
		pNewCmd->AddParameter(new MCmdParamUInt(nBP));
		pNewCmd->AddParameter(new MCmdParamUInt(nExpPercent));
		MMatchServer::GetInstance()->RouteToListener(pObj, pNewCmd);
	}
}

bool MMatchRuleQuestChallenge::OnRun()
{
	DWORD nClock = MMatchServer::GetInstance()->GetGlobalClockCount();

	switch (GetRoundState())
	{
	case MMATCH_ROUNDSTATE_PREPARE:
	{
		if (GetStage()->CheckBattleEntry() == true)
		{
			SetRoundState(MMATCH_ROUNDSTATE_COUNTDOWN);
			return true;
		}
		return true;
	}
	case MMATCH_ROUNDSTATE_COUNTDOWN:
	{
		if (nClock - GetRoundStateTimer() > 3 * 1000)
		{
			SetRoundState(MMATCH_ROUNDSTATE_PLAY);
			return true;
		}
		return true;
	}
	case MMATCH_ROUNDSTATE_PLAY:
	{
		if (!OnCheckEnableBattleCondition())
		{
			SetRoundState(MMATCH_ROUNDSTATE_FREE);
		}
		if (OnCheckRoundFinish())
		{
			SetRoundState(MMATCH_ROUNDSTATE_FINISH);
		}
		if (!CheckPlayersAlive())
		{
			SetRoundState(MMATCH_ROUNDSTATE_EXIT);
		}
		return true;
	}
	case MMATCH_ROUNDSTATE_FINISH:
	{
		if (nClock - GetRoundStateTimer() > 1000)
		{
			if (m_nCurrSector < m_pScenario->GetNumSector())
			{
				if (nClock - GetRoundStateTimer() > 6000)
				{
					SetRoundState(MMATCH_ROUNDSTATE_PREPARE);
				}
				return true;
			}
			else
			{
				SetRoundState(MMATCH_ROUNDSTATE_EXIT);
				return true;
			}
		}
		return true;
	}
	/*	case MMATCH_ROUNDSTATE_FREE:
		{
			if (OnCheckEnableBattleCondition())
			{
				SetRoundState(MMATCH_ROUNDSTATE_PREPARE);
			}
			return true;
		}*/
	case MMATCH_ROUNDSTATE_EXIT:
	{
		return false;
	}
	}
	return false;
}

bool MMatchRuleQuestChallenge::OnCheckRoundFinish()
{
	if (m_pNpcMgr->GetNumNpc() == 0)
	{
		for (MUIDRefCache::iterator i = m_pStage->GetObjBegin(); i != m_pStage->GetObjEnd(); i++)
		{
			MUID uidChar = i->first;
			if (!m_pPlayerMgr->IsPlayerInMap(uidChar))
			{
				MMatchObject* pObj = MMatchServer::GetInstance()->GetObject(uidChar);
				if (!IsEnabledObject(pObj)) continue;
				if (pObj->GetPlace() == MMP_BATTLE) // Custom: Make sure the user is actually in the game
				{
					m_pPlayerMgr->AddPlayer(pObj);
				}
			}
		}
		return true;
	}
	return false;
}
bool MMatchRuleQuestChallenge::OnCheckEnableBattleCondition()
{
	for (MUIDRefCache::iterator i = m_pStage->GetObjBegin(); i != m_pStage->GetObjEnd(); i++)
	{
		MMatchObject* pObj = (MMatchObject*)(*i).second;
		if (!IsEnabledObject(pObj)) continue;
		if (pObj->GetEnterBattle() == false) continue;
		if (IsAdminGrade(pObj) && pObj->GetAccountInfo()->m_nUGrade == MMUG_EVENTTEAM
			&& pObj->CheckPlayerFlags(MTD_PlayerFlags_AdminHide)) continue;
	}
	{
	if (!GetStage()->CheckBattleEntry())
			return false;

		return true;
	}
	return true;
}

bool MMatchRuleQuestChallenge::CheckPlayersAlive()
{
	int nAliveCount = 0;
	MMatchObject* pObj;
	for (MUIDRefCache::iterator i = m_pStage->GetObjBegin(); i != m_pStage->GetObjEnd(); i++)
	{
		pObj = (MMatchObject*)(*i).second;
		if (pObj->GetEnterBattle() == false) continue;	// 배틀참가하고 있는 플레이어만 체크
		if (IsAdminGrade(pObj) && pObj->CheckPlayerFlags(MTD_PlayerFlags_AdminHide)) continue;

		if (pObj->CheckAlive() == true)
		{
			++nAliveCount;
		}
	}

	// 모두 죽었으면 리셋
	if (nAliveCount == 0) return false;

	return true;
}


void MMatchRuleQuestChallenge::ProcessNpcSpawning()
{
	// todok 요건 모든 스폰포인트에서 액터를 스폰하는 것 // 기획 나오고 구현을 픽스할 때는 코드 정리 좀 하자 if문 대박이네
	MNewQuestSector* pCurrSector = NULL;
	if (m_pScenario)
	{
		MNewQuestSector* pCurrSector = m_pScenario->GetSector(m_nCurrSector);
		if (pCurrSector)
		{
			int numSpawnType = pCurrSector->GetNumSpawnType();
			for (int i=0; i<numSpawnType; ++i)
			{
				MNewQuestSpawn* pSpawn = pCurrSector->GetSpawnByIndex(i);
				if (pSpawn)
				{
					for (int k=0; k<pSpawn->GetNum(); ++k)
					{
						// drop item 결정
						MQuestDropItem item;
						MMatchServer::GetInstance()->GetQuest()->GetDropTable()->Roll(item, pSpawn->GetDropTableId(), 0);

						SpawnNpc(pSpawn->GetActor(), i, k, item.nID);
					}
				}
			}
		}
	}
}


MUID MMatchRuleQuestChallenge::SpawnNpc(const char* szActorDef, int nCustomSpawnTypeIndex, int nSpawnIndex, int nDropItemId)
{
	return IMatchRuleNewQuest::SpawnNpc(szActorDef, nCustomSpawnTypeIndex, nSpawnIndex, nDropItemId);
}

void MMatchRuleQuestChallenge::RouteSpawnNpc(MUID uidNPC, MUID uidController, const char* szNpcDefName, int nCustomSpawnTypeIndex, int nSpawnIndex)
{
	IMatchRuleNewQuest::RouteSpawnNpc(uidNPC,uidController,szNpcDefName,nCustomSpawnTypeIndex,nSpawnIndex);
}

void MMatchRuleQuestChallenge::SpawnNpcSummon(const char* szActorDef, int num, MShortVector nSpawnPos, MShortVector nSpawnDir,const int route)
{
	IMatchRuleNewQuest::SpawnNpcSummon(szActorDef, num, nSpawnPos, nSpawnDir,route);
}

void MMatchRuleQuestChallenge::RouteSpawnNpcSummon(MUID uidNpc, MUID uidController, int Num, const char* szNpcDefName, MShortVector pos, MShortVector dir,
	const int route)
{
	IMatchRuleNewQuest::RouteSpawnNpcSummon(uidNpc, uidController, Num, szNpcDefName, pos, dir,route);
}
void MMatchRuleQuestChallenge::RouteNpcDead(MUID uidNPC, MUID uidKiller)
{
	MCommand* pNew = MMatchServer::GetInstance()->CreateCommand(MC_NEWQUEST_NPC_DEAD, MUID(0, 0));
	pNew->AddParameter(new MCmdParamUID(uidKiller));
	pNew->AddParameter(new MCmdParamUID(uidNPC));
	MMatchServer::GetInstance()->RouteToBattle(m_pStage->GetUID(), pNew);
}
void MMatchRuleQuestChallenge::RouteMoveToNextSector(int nSectorID)
{
	MCommand* pNew = MMatchServer::GetInstance()->CreateCommand(MC_NEWQUEST_MOVE_TO_NEXT_SECTOR, MUID(0,0));
	pNew->AddParameter(new MCmdParamInt(nSectorID));
	pNew->AddParameter(new MCmdParamBool(false));
	MMatchServer::GetInstance()->RouteToBattle(m_pStage->GetUID(), pNew);
}
void MMatchRuleQuestChallenge::RouteMoveToNextSectorSingle(MUID uidListener, int nSectorID)
{
	MMatchObject* pObj = MMatchServer::GetInstance()->GetObject(uidListener);
	if (!IsEnabledObject(pObj)) return;

	MCommand* pNew = MMatchServer::GetInstance()->CreateCommand(MC_NEWQUEST_MOVE_TO_NEXT_SECTOR, MUID(0, 0));
	pNew->AddParameter(new MCmdParamInt(nSectorID));
	pNew->AddParameter(new MCmdParamBool(true));
	MMatchServer::GetInstance()->RouteToListener(pObj, pNew);
}
void MMatchRuleQuestChallenge::OnCommand(MCommand* pCommand)
{
	switch (pCommand->GetID())
	{
	case MC_NEWQUEST_REQUEST_NPC_DEAD:
	{
		MUID uidSender = pCommand->GetSenderUID();
		MUID uidKiller, uidNPC;
		MShortVector s_pos;
		pCommand->GetParameter(&uidKiller, 0, MPT_UID);
		pCommand->GetParameter(&uidNPC, 1, MPT_UID);
		pCommand->GetParameter(&s_pos, 2, MPT_SVECTOR);
		MVector pos = MVector((float)s_pos.x, (float)s_pos.y, (float)s_pos.z);

		MNewQuestNpcObject* pNpc = m_pNpcMgr->GetNpc(uidNPC);
		if (!pNpc)
		{
			_ASSERT(0); break;
		}

		if (pNpc->GetController() != uidSender)
		{
			_ASSERT(0); break;
		}

		DropItemByNpcDead(uidKiller, pNpc->GetDropItemId(), pos);

		m_pNpcMgr->DeleteNpcObject(uidNPC);
		m_pPlayerMgr->DecreaseNpcControl(uidSender);

		RouteNpcDead(uidNPC, uidKiller);
	}
	break;
	case MC_NEWQUEST_REQUEST_NPC_SPAWN:
	{
		MUID uidController;
		MUID uidNpc;
		char szActorDefName[128];
		int num, route;
		MShortVector s_pos, s_dir;

		if (!m_pNpcMgr || !m_pPlayerMgr)
			break;

		if (GetRoundState() != MMATCH_ROUNDSTATE_PLAY)
			break;

		pCommand->GetParameter(&uidController, 0, MPT_UID);
		pCommand->GetParameter(&uidNpc, 1, MPT_UID);
		pCommand->GetParameter(&num, 2, MPT_INT);
		pCommand->GetParameter(szActorDefName, 3, MPT_STR, sizeof(szActorDefName));
		pCommand->GetParameter(&s_pos, 4, MPT_SVECTOR);
		pCommand->GetParameter(&s_dir, 5, MPT_SVECTOR);
		pCommand->GetParameter(&route, 6, MPT_INT);

		MNewQuestNpcObject* pNpc = m_pNpcMgr->GetNpc(uidNpc);
		if (!pNpc)
		{
			mlog("not npc uid\n");
			break;
		}

		if (pNpc->GetController() != pCommand->GetSenderUID())
			break;

		SpawnNpcSummon(szActorDefName, num, s_pos, s_dir, route);
		break;
	}
	}
}

void MMatchRuleQuestChallenge::DropItemByNpcDead(const MUID& uidKiller, int nWorldItemId, const MVector& pos)
{
	// 월드아이템인지 검사하자
	if (!MGetMatchWorldItemDescMgr()->GetItemDesc(nWorldItemId))
	{
		MMatchObject* pPlayer = MMatchServer::GetInstance()->GetObject(uidKiller);
		if (!IsEnabledObject(pPlayer) || nWorldItemId == 0)
			return;
		int nSpawnItemID = QUEST_WORLDITEM_ITEMBOX_ID;
		int nQuestItemID = nWorldItemId;
		int nRentPeriodHour = RENT_MINUTE_PERIOD_UNLIMITED;

		int nWorldItemExtraValues[WORLDITEM_EXTRAVALUE_NUM];
		nWorldItemExtraValues[0] = nQuestItemID;
		nWorldItemExtraValues[1] = nRentPeriodHour;

		m_pStage->SpawnServerSideWorldItem(pPlayer, nSpawnItemID, pos.x, pos.y, pos.z, QUEST_DYNAMIC_WORLDITEM_LIFETIME, nWorldItemExtraValues);
	}
	else
	{
		MMatchObject* pPlayer = MMatchServer::GetInstance()->GetObject(uidKiller);
		if (!pPlayer) return;

		int nWorldItemExtraValues[WORLDITEM_EXTRAVALUE_NUM];
		for (int i = 0; i < WORLDITEM_EXTRAVALUE_NUM; i++)
			nWorldItemExtraValues[i] = -1;


		m_pStage->SpawnServerSideWorldItem(pPlayer, nWorldItemId, pos.x, pos.y, pos.z,
			QUEST_DYNAMIC_WORLDITEM_LIFETIME, nWorldItemExtraValues);
	}
}

void MMatchRuleQuestChallenge::OnEnterBattle(MUID& uidChar)
{
	//latejoin
	MMatchObject* pObj = MGetMatchServer()->GetObject(uidChar);
	if (!pObj)
		return;

	//only use latejoin info is player isn't in the ptr
	if (!m_pPlayerMgr->FindPlayer(uidChar))
	{
		IMatchRuleNewQuest::OnEnterBattle(uidChar);

		///Added timesync to get the times close to matching when player latejoins
		UINT currTime = MMatchServer::GetInstance()->GetGlobalClockCount() - GetRoundStateTimer();

		MCommand* pCmd = MGetMatchServer()->CreateCommand(MC_MATCH_LATEJOIN_CQ, MUID(0, 0));
		pCmd->AddParameter(new MCmdParamUID(uidChar));
		pCmd->AddParameter(new MCmdParamInt(m_nCurrSector));
		pCmd->AddParameter(new MCmdParamUInt(currTime));

		MGetMatchServer()->RouteToListener(pObj, pCmd);
	}
}

void MMatchRuleQuestChallenge::OnLeaveBattle(MUID& uidChar)
{
	IMatchRuleNewQuest::OnLeaveBattle(uidChar);
}

void MMatchRuleQuestChallenge::OnGameKill(const MUID& uidAttacker, const MUID& uidVictim)
{

}

void MMatchRuleQuestChallenge::OnRequestPlayerDead(const MUID& uidPlayer)
{
	IMatchRuleNewQuest::OnRequestPlayerDead(uidPlayer);
}

void MMatchRuleQuestChallenge::RouteSpawnLateJoinNpc(MUID uidLateJoiner, MUID uidNpc, const char* szNpcDefName, int nCustomSpawnTypeIndex, int nSpawnIndex)
{
	IMatchRuleNewQuest::RouteSpawnLateJoinNpc(uidLateJoiner, uidNpc, szNpcDefName, nCustomSpawnTypeIndex, nSpawnIndex);
}

//Refresh best time????right now ust for min/maxplayers
void MMatchRuleQuestChallenge::RefreshStageGameInfo()
{
	//MakeStageGameInfo();
	//RouteStageGameInfo();
}

void MMatchRuleQuestChallenge::MakeStageGameInfo()
{

}

void MMatchRuleQuestChallenge::RouteStageGameInfo(int const& maxPlayers, int const& minLevel)
{

}
//void MMatchRuleQuestChallenge::PostInsertQuestChallengeGameLogAsyncJob()
//{
//	// Custom: More than 4 in quest.
//	// Custom: Quest/Clan Server
//	//if( MSM_TEST == MGetServerConfig()->GetServerMode() ) 
//	//{
//	CollectEndQuestChallengeGameLogInfo();
//	m_QuestChallengeGameLogInfoMgr.PostInsertQuestChallengeGameLog();
//	//}
//}
bool MMatchRuleQuestChallenge::OnObtainWorldItemRandom(int nItemID, int* pnExtraValues)
{
	int nQuestItemID = pnExtraValues[0];
	int nRentPeriodHour = pnExtraValues[1];

	if (nQuestItemID == -1) //eat it in this case.
		return false;


	MMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);
	if (pItemDesc == NULL) return false;

	std::vector<MMatchObject*> vObjects;

	for (MUIDRefCache::iterator i = m_pStage->GetObjBegin(); i != m_pStage->GetObjEnd(); i++)
	{
		MMatchObject* pObj = (MMatchObject*)(*i).second;
		if (!IsEnabledObject(pObj)) continue;

		if (pObj->GetCharInfo()->m_ItemList.GetCount() >= MAX_ITEM_COUNT)
		{
			continue;
		}
		if (m_pPlayerMgr->IsPlayerInMap(pObj->GetUID()))
		{
			if (pItemDesc->m_nResSex.Ref() == -1 || pObj->GetCharInfo() && (int)pObj->GetCharInfo()->m_nSex == pItemDesc->m_nResSex.Ref())
				vObjects.push_back(pObj);
		}
	}

	if (vObjects.empty())
		return false;

	const unsigned long n = vObjects.size();

	if (n == 0)
		return false;


	int randomIndex = rand() % n;

	MMatchObject* pObj = NULL;
	if (randomIndex < vObjects.size())
		pObj = vObjects[randomIndex];

	if (!pObj)
		return false;
	return true;
}
void MMatchRuleQuestChallenge::OnObtainWorldItem(MMatchObject* pObj, int nItemID, int* pnExtraValues)
{
	if (!IsEnabledObject(pObj))
		return;

	int nQuestItemID = pnExtraValues[0];
	int nRentPeriodHour = pnExtraValues[1];

	if (nQuestItemID == -1) //eat it in this case.
		return;

	if (pObj->GetCharInfo()->m_ItemList.GetCount() >= MAX_ITEM_COUNT)
	{
		char sztext[48];
		sprintf_s(sztext, 48, "You have too many items to receive a reward");
		MGetMatchServer()->Announce(pObj, sztext);
		OnObtainWorldItemRandom(nItemID, pnExtraValues);
		//still need to return here otherwise we fall thru
		return;
	}

	MMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);
	if (pItemDesc == NULL) return;

	if (pItemDesc->m_nResSex.Ref() != -1)
	{
		if (pObj->GetCharInfo() && (int)pObj->GetCharInfo()->m_nSex != pItemDesc->m_nResSex.Ref())
		{
			OnObtainWorldItemRandom(nItemID, pnExtraValues);
			return;
		}
		return;
	}
}
//void MMatchRuleQuestChallenge::PostCQGameLog()
//{
//
//
//}