#include "stdafx.h"
#include "MMatchServer.h"
#include "MMatchConfig.h"
#include "MLadderMgr.h"
#include "MLadderPicker.h"
#include "MMatchGlobal.h"
#include "MThread.h"
#include "MSharedCommandTable.h"
#include "MMatchUtil.h"
#include "MTeamGameStrategy.h"
#include "MCommandCommunicator.h"
#include "Msg.h"
#include "MDebug.h"


MLadderGroupMap* MLadderMgr::GetWaitGroupContainer(MLADDERTYPE nLadderType)
{
	if ((nLadderType < 0) || (nLadderType >= MLADDERTYPE_MAX))
	{
		_ASSERT("UNKNOWN LADDERMAP");
		return NULL;
	}

	return &m_WaitingMaps[(int)nLadderType];
}

MLadderGroup* MLadderMgr::CreateLadderGroup()
{
	return new MLadderGroup(MMatchServer::GetInstance()->GetTickTime());
}

MLadderGroup* MLadderMgr::FindLadderGroup(int nGroupID)
{
	MLadderGroup* pGroup = NULL;

	for (int i = 0; i < MLADDERTYPE_MAX; i++)
	{
		if (pGroup=m_WaitingMaps[i].Find(nGroupID))
			return pGroup;
	}
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////////
bool MLadderMgr::Init()
{
	m_Stat.Init();

	return true;
}

void MLadderMgr::AddGroup(MLADDERTYPE nLadderType, MLadderGroup* pGroup)
{
	pGroup->SetLadderType(nLadderType);

	MLadderGroupMap* pGroupMap = GetWaitGroupContainer(nLadderType);
	if (pGroupMap == NULL) return;

	pGroupMap->Add(pGroup);
	m_GroupList.push_back(pGroup);

	// Ladder 상대 찾는중 알림(for UI)
	for (list<MUID>::iterator i = pGroup->GetPlayerListBegin(); i != pGroup->GetPlayerListEnd(); i++)
	{
		MUID uidPlayer = (*i);
		MCommand* pCmd = MMatchServer::GetInstance()->CreateCommand(MC_MATCH_LADDER_SEARCH_RIVAL, uidPlayer);

		MMatchObject* pObj = MMatchServer::GetInstance()->GetObject(uidPlayer);
		if (!IsEnabledObject(pObj))
		{
			delete pCmd;
			continue;
		}

		MMatchServer::GetInstance()->RouteToListener(pObj, pCmd);
	}
}

bool MLadderMgr::Challenge(MLadderGroup* pGroup)
{
	int nPlayerCount = (int)pGroup->GetPlayerCount();

	if (nPlayerCount > 0)
	{
		for (int i = 0; i < MLADDERTYPE_MAX; i++)
		{
			if (nPlayerCount == GetNeedMemberCount(MLADDERTYPE(i)))
			{
				AddGroup(MLADDERTYPE(i), pGroup);
			}
		}

		//AddGroup(MLADDERTYPE(nPlayerCount-1), pGroup);
		return true;
	}

	mlog("UNKNOWN LADDERTYPE\n");

	return false;
}

void MLadderMgr::RemoveFromGroupList(MLadderGroup* pGroup)
{
	if (pGroup)
	{
		m_GroupList.remove(pGroup);
	}
}

void MLadderMgr::CancelChallenge(int nGroupID, const char* pszCancelName)
{
	MLadderGroup* pGroup = FindLadderGroup(nGroupID);
	if (pGroup == NULL) return;
	MLadderGroupMap* pGroupMap = GetWaitGroupContainer((MLADDERTYPE)pGroup->GetLadderType());
	if (pGroupMap == NULL) return;

	for (list<MUID>::iterator i = pGroup->GetPlayerListBegin(); i != pGroup->GetPlayerListEnd(); i++)
	{
		MUID uidMember = (*i);

		MMatchObject* pMemberObject = MMatchServer::GetInstance()->GetObject(uidMember);
		if (!IsEnabledObject(pMemberObject)) continue;
		pMemberObject->SetLadderChallenging(false);
		pMemberObject->SetLadderGroupID(0);

		MCommand* pCmd = MMatchServer::GetInstance()->CreateCommand(MC_MATCH_LADDER_CANCEL_CHALLENGE, uidMember);
		pCmd->AddParameter(new MCmdParamStr(pszCancelName));
		pCmd->AddParameter(new MCmdParamBool(false));
		MMatchObject* pObj = MMatchServer::GetInstance()->GetObject(uidMember);
		if (!IsEnabledObject(pObj))
		{
			delete pCmd;
			continue;
		}

		MMatchServer::GetInstance()->RouteToListener(pObj, pCmd);
	}
	pGroupMap->Remove(pGroup->GetID());
	RemoveFromGroupList(pGroup);
	delete pGroup;
}

int MLadderMgr::MakeMatch(MLADDERTYPE nLadderType)
{
	MLadderGroupMap* pWaitGroupMap = GetWaitGroupContainer(nLadderType);
	if (pWaitGroupMap == NULL) return 0;

	MTime time;
	MLadderPicker	ladderPicker;

	for (MLadderGroupMap::iterator i=pWaitGroupMap->begin();
		 i!=pWaitGroupMap->end(); i++) 
	{
		MLadderGroup* pGroup = (*i).second;
		pGroup->UpdateTick();

/*
#ifdef _DEBUG
		const unsigned int MIN_ADDTICKET_TICK = 3000;
		if (MGetTimeDistance(MMatchServer::GetInstance()->GetTickTime(), pGroup->GetRegTime()) < MIN_ADDTICKET_TICK) continue;
#endif
*/

		//ladderPicker.AddTicket( pGroup, time.MakeNumber(0,pGroup->GetScore()) );
		
		int nClanPoint = DEFAULT_CLAN_POINT;
		MMatchClan* pClan = MMatchServer::GetInstance()->GetClanMap()->GetClan(pGroup->GetCLID());
		if (pClan)
		{
			nClanPoint = pClan->GetClanInfoEx()->nPoint;
		}

		const int MAX_RANDOM_ARG = 100;

		ladderPicker.AddTicket(pGroup, nClanPoint, pGroup->GetTickCount(), time.MakeNumber(0, MAX_RANDOM_ARG));
	}

	ladderPicker.Shuffle();

	int nLaunchCount = 0;
	while(true) {
		int nGroupA = 0;
		int nGroupB = 0;
		if (ladderPicker.PickMatch(&nGroupA, &nGroupB, 0) == false)
			break;
		LaunchLadder(nLadderType, nGroupA, nGroupB);
		nLaunchCount++;
	}
	return nLaunchCount;
}

void MLadderMgr::CleaningGarbages()
{
	for (int i = 0; i < MLADDERTYPE_MAX; i++)
	{
		MLADDERTYPE nLadderType = MLADDERTYPE(i);

		MLadderGroupMap* pWaitGroupMap = GetWaitGroupContainer(nLadderType);
		if (pWaitGroupMap == NULL) continue;

		list<int>		CancelGroupIDList;

		for (MLadderGroupMap::iterator itorGroup = pWaitGroupMap->begin(); itorGroup!=pWaitGroupMap->end(); itorGroup++) 
		{
			MLadderGroup* pGroup = (*itorGroup).second;

			// 한명이라도 접속을 끊었으면 Cancel한다.
			bool bExistCannotPlayer = false;
			for (list<MUID>::iterator itorPlayerUID=pGroup->GetPlayerListBegin(); itorPlayerUID!= pGroup->GetPlayerListEnd(); 
				itorPlayerUID++)
			{
				MUID uidMember = (*itorPlayerUID);

				MMatchObject* pMemberObject = MMatchServer::GetInstance()->GetObject(uidMember);
				// Custom: made it so the queue is removed when any of the users are not in a clan channel to avoid glitched queues.
	//			MMatchChannel* pChannel = MMatchServer::GetInstance()->FindChannel(pMemberObject->GetChannelUID());

				if (!IsEnabledObject(pMemberObject) /*|| pChannel->GetChannelType() != MCHANNEL_TYPE_CLAN*/)
				{
					bExistCannotPlayer = true;
					break;
				}
			}

			if (bExistCannotPlayer)
			{
				CancelGroupIDList.push_back(pGroup->GetID());
			}
		}

		// 상대팀 대기중인 플레이어의 접속이 끊어져서 Cancel해야 되는 그룹은 Cancel해준다.
		for (list<int>::iterator itorGroupID = CancelGroupIDList.begin(); itorGroupID != CancelGroupIDList.end(); 
			++itorGroupID)
		{
			CancelChallenge((*itorGroupID), "");
		}
	}
}

void MLadderMgr::LaunchLadder(MLADDERTYPE nLadderType, int nGroupA, int nGroupB)
{

	MLadderGroupMap* pGroupMap = GetWaitGroupContainer(nLadderType);
	if (pGroupMap == NULL) return;

	MLadderGroup* pGroupA = pGroupMap->Find(nGroupA);
	MLadderGroup* pGroupB = pGroupMap->Find(nGroupB);

	if ((pGroupA != NULL) && (pGroupB != NULL) && (pGroupA->IsSameGroup(pGroupB))) return;

	pGroupMap->Remove(nGroupA);
	pGroupMap->Remove(nGroupB);

	RemoveFromGroupList(pGroupA);
	RemoveFromGroupList(pGroupB);

	if ((pGroupA == NULL) || (pGroupB == NULL)) {
		return;
	}

	LadderGameMapVoteInfo* m = new LadderGameMapVoteInfo();
	for (int i = 0; i < 3; i++)
	{
		m->Votes[i] = 0;
		m->Maps[i] = -1;
	}
	MBaseTeamGameStrategy* pTeamGameStrategy = MBaseTeamGameStrategy::GetInstance(MGetServerConfig()->GetServerMode());
	if (pTeamGameStrategy)
	{
		for (int i = 0; i < 3; i++)
		{
			int index = pTeamGameStrategy->GetLadderWarsRandomMap((int)pGroupA->GetPlayerCount());
			while (index == m->Maps[0] || index == m->Maps[1] || index == m->Maps[2])
				index = pTeamGameStrategy->GetLadderWarsRandomMap((int)pGroupA->GetPlayerCount());
			m->Maps[i] = index;
		}
	};
	int ID = counter++;
	m->RegisterTime = timeGetTime();
	MMatchServer* pServer = MMatchServer::GetInstance();
	pServer = MMatchServer::GetInstance();
	MCommand* pCommand = pServer->CreateCommand(MC_MATCH_RANDOM_MAPS, MUID(0, 0));
	pCommand->AddParameter(new MCmdParamInt(m->Maps[0]));
	pCommand->AddParameter(new MCmdParamInt(m->Maps[1]));
	pCommand->AddParameter(new MCmdParamInt(m->Maps[2]));
	for (list<MUID>::iterator i = pGroupA->GetPlayerListBegin(); i != pGroupA->GetPlayerListEnd(); i++)
	{
		MUID uidPlayer = (*i);
		MMatchObject* pObj = (MMatchObject*)pServer->GetObject(uidPlayer);
		if (pObj)
		{
			pObj->LadderWarsIdentifier = ID;
			MCommand* pSendCmd = pCommand->Clone();
			pServer->RouteToListener(pObj, pSendCmd);
		}
	}
	for (list<MUID>::iterator i = pGroupB->GetPlayerListBegin(); i != pGroupB->GetPlayerListEnd(); i++)
	{
		MUID uidPlayer = (*i);
		MMatchObject* pObj = (MMatchObject*)pServer->GetObject(uidPlayer);
		if (pObj)
		{
			pObj->LadderWarsIdentifier = ID;
			MCommand* pSendCmd = pCommand->Clone();
			pServer->RouteToListener(pObj, pSendCmd);
		}
	}
	delete pCommand;
	m->pGroupA = pGroupA;
	m->pGroupB = pGroupB;
	WaitingMapSelectionGames.insert(map<unsigned long int, LadderGameMapVoteInfo*>::value_type(ID, m));
}

#ifdef _CW_VOTE
void MLadderMgr::UpdatePlayerVote(int VoteID, MMatchObject* pObj)
{
	if (pObj->LadderWarsIdentifier != -1)
	{
		map<unsigned long int, LadderGameMapVoteInfo*>::iterator i = WaitingMapSelectionGames.find(pObj->LadderWarsIdentifier);
		if (i != WaitingMapSelectionGames.end())
		{
			LadderGameMapVoteInfo* m = i->second;
			if (m)
			{
				if (pObj->LastVoteID != -1)
					m->Votes[pObj->LastVoteID]--;
				m->Votes[VoteID]++;
				pObj->LastVoteID = VoteID;
				pObj->bMatching = true;
			}
		}
	}
}

void MLadderMgr::UpdateMapCountDown(unsigned long int NowTime)
{
	MMatchServer* pServer = MMatchServer::GetInstance();
	for (map<unsigned long int, LadderGameMapVoteInfo*>::iterator i = WaitingMapSelectionGames.begin(); i != WaitingMapSelectionGames.end();)
	{
		LadderGameMapVoteInfo* m = i->second;
		if (!m)
		{
			i = WaitingMapSelectionGames.erase(i);
			continue;
		}
		if ((NowTime - m->RegisterTime) >= 15000) // 15 sec
		{
			int winningmapindex = -1, winningindex = -1, votecount = 0;
			for (int a = 0; a < 3; a++)
			{
				if (m->Votes[a] == votecount)
				{
					votecount = m->Votes[a];
					winningmapindex = m->Maps[a];
					winningindex = a;
				}
				else if (m->Votes[a] >= votecount)
				{
					votecount = m->Votes[a];
					winningmapindex = m->Maps[a];
					winningindex = a;
				}
			}
			if (winningmapindex == -1)
				winningmapindex = m->Maps[RandomNumber(0, 2)];
			else
			{
				switch (winningindex)
				{
				case 0:
					if (m->Votes[winningindex] == m->Votes[1])
						winningmapindex = m->Maps[RandomNumber(0, 1)];
					else if (m->Votes[winningindex] == m->Votes[2])
						if (RandomNumber(0, 1) == 0)
							winningmapindex = m->Maps[winningindex];
						else
							winningmapindex = m->Maps[2];
					break;
				case 1:
					if (m->Votes[winningindex] == m->Votes[0])
						winningmapindex = m->Maps[RandomNumber(0, 1)];
					else if (m->Votes[winningindex] == m->Votes[2])
						if (RandomNumber(0, 1) == 0)
							winningmapindex = m->Maps[winningindex];
						else
							winningmapindex = m->Maps[2];
					break;
				case 2:
					if (m->Votes[winningindex] == m->Votes[1])
						winningmapindex = m->Maps[RandomNumber(1, 2)];
					else if (m->Votes[winningindex] == m->Votes[0])
						if (RandomNumber(0, 1) == 1)
							winningmapindex = m->Maps[winningindex];
						else
							winningmapindex = m->Maps[0];
					break;
				}
			}
			i = WaitingMapSelectionGames.erase(i);
			pServer->LadderGameLaunch(m->pGroupA, m->pGroupB, winningmapindex);
			continue;
		}
		else
		{
			MCommand* pCommand = pServer->CreateCommand(MC_MATCH_VOTE_UPDATE, MUID(0, 0));
			pCommand->AddParameter(new MCmdParamInt(m->Votes[0]));
			pCommand->AddParameter(new MCmdParamInt(m->Votes[1]));
			pCommand->AddParameter(new MCmdParamInt(m->Votes[2]));
			for (list<MUID>::iterator i = m->pGroupA->GetPlayerListBegin(); i != m->pGroupA->GetPlayerListEnd(); i++)
			{
				MUID uidPlayer = (*i);
				MMatchObject* pObj = (MMatchObject*)pServer->GetObject(uidPlayer);
				if (pObj)
				{
					MCommand* pSendCmd = pCommand->Clone();
					pServer->RouteToListener(pObj, pSendCmd);
				}
			}
			for (list<MUID>::iterator i = m->pGroupB->GetPlayerListBegin(); i != m->pGroupB->GetPlayerListEnd(); i++)
			{
				MUID uidPlayer = (*i);
				MMatchObject* pObj = (MMatchObject*)pServer->GetObject(uidPlayer);
				if (pObj)
				{
					MCommand* pSendCmd = pCommand->Clone();
					pServer->RouteToListener(pObj, pSendCmd);
				}
			}
			delete pCommand;
		}
		i++;
	}
}
#endif // _CW_VOTE

#define MTIME_LADDER_DEFAULT_TICKINTERVAL		10000			// 기본틱 10초

unsigned long int MLadderMgr::GetTickInterval()
{
	unsigned long int nDefaultTickInterval = MTIME_LADDER_DEFAULT_TICKINTERVAL;

	// 동접에 따라 틱시간을 가변으로 한다.
	int nObjSize = (int)MMatchServer::GetInstance()->GetObjects()->size();
	
	if (nObjSize < 50)
	{
		nDefaultTickInterval = 5000;		// 50명미만은 5초
	}
	else if ((nObjSize>=50) && (nObjSize < 150))
	{
		nDefaultTickInterval = 7000;		// 150명 미만은 7초
	}
	else if ((nObjSize>=150) && (nObjSize < 300))
	{
		nDefaultTickInterval = 9000;		// 300명 미만은 9초
	}
	return nDefaultTickInterval;
}


void MLadderMgr::Tick(unsigned long nTick)
{
	if (nTick - GetLastTick() < GetTickInterval())
		return;
	else
		SetLastTick(nTick);

	CleaningGarbages();

	for (int i = 0; i < MLADDERTYPE_MAX; i++)
	{
		MakeMatch(MLADDERTYPE(i));
	}

#ifdef _CW_VOTE
	UpdateMapCountDown(timeGetTime());
#endif // _CW_VOTE
	m_Stat.Tick(nTick);
}

int MLadderMgr::GetNeedMemberCount(MLADDERTYPE nLadderType)
{
	if ((nLadderType >= 0) && (nLadderType < MLADDERTYPE_MAX))
	{
		return g_nNeedLadderMemberCount[(int)nLadderType];
	}

	return -1;
}

int MLadderMgr::GetTotalGroupCount()
{
	int ret = 0;
	for (int i = 0; i < MLADDERTYPE_MAX; i++)
	{
		ret += (int)m_WaitingMaps[i].size();
	}
	return ret;
}

unsigned long MLadderMgr::GetChecksum(int nFirstIndex, int nGroupCount)
{
	unsigned long int nGroupListChecksum = 0;

	list<MLadderGroup*>::iterator itorGroup = m_GroupList.begin();
	for (int i = 0; i < nFirstIndex; i++, itorGroup++)
	{
		if (itorGroup == m_GroupList.end()) return 0;
	}

	for (int i = 0; i < nGroupCount; i++)
	{
		if (itorGroup == m_GroupList.end()) return nGroupListChecksum;

		MLadderGroup* pGroup = (*itorGroup);
		nGroupListChecksum += pGroup->GetChecksum();
		itorGroup++;
	}

	return nGroupListChecksum;

}

void MLadderMgr::DebugTest()
{
#ifdef _DEBUG
	OutputDebugString("Ahehheh \n");

	MMatchServer* pServer = MMatchServer::GetInstance();

	MMatchObject* pPlayer1 = pServer->GetPlayerByName("라온마이");
	if (pPlayer1 == NULL) return;
	MMatchObject* pPlayer2 = pServer->GetPlayerByName("라온하제2");
	if (pPlayer2 == NULL) return;


	// Ensure All Player Not in LadderGroup
	MLadderGroup* pGroupA = CreateLadderGroup();
	pGroupA->AddPlayer(pPlayer1);
	MLadderGroup* pGroupB = CreateLadderGroup();
	pGroupB->AddPlayer(pPlayer2);

	//	if (ValidateLadderGroup(pGroup))
	Challenge(pGroupA);
	Challenge(pGroupB);
	//z
#endif
}