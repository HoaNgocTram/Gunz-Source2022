#include "stdafx.h"
#include "MMatchServer.h"
#include "MAsyncDBJob_LadderWars.h"
#include "MBlobArray.h"
#include "MCrashDump.h"
#include "MMatchConfig.h"

DWORD LadderWarsExceptionHandler( PEXCEPTION_POINTERS ExceptionInfo, char* Place )
{
	char szStageDumpFileName[ _MAX_DIR ]= {0,};
	SYSTEMTIME SystemTime;
	GetLocalTime( &SystemTime );
	sprintf( szStageDumpFileName, "Log/LadderWars[%s]_%d-%d-%d_%d-%d-%d.dmp"
		, Place
		, SystemTime.wYear
		, SystemTime.wMonth
		, SystemTime.wDay
		, SystemTime.wHour
		, SystemTime.wMinute
		, SystemTime.wSecond );

	return CrashExceptionDump( ExceptionInfo, szStageDumpFileName, true );
}

void MMatchServer::UpdateLadderWarsGame(MMatchStage* pStage, MMatchObject* pObj, MMatchTeam nTeam)
{
//	__try
//	{
	if(!pStage || !pObj) return;
#ifdef _REJOIN_NEW
	ClanRejoiner.insert(ClanReDef::value_type(pObj->GetCharInfo()->m_nCID, new ClanRejoin(nTeam, pStage->GetUID())));
#endif
	pObj->GetCharInfo()->m_nCID, (nTeam, pStage->GetUID());
	pObj->OnStageJoin();
	pObj->LadderWarsIdentifier = -1;
	pObj->LastVoteID = -1;
	pStage->LadderWarsAdd(pObj->GetCharInfo()->m_nCID, nTeam, pObj->GetUID());
	pStage->AddObject(pObj->GetUID(), pObj);
	pObj->SetStageUID(pStage->GetUID());
	pObj->SetStageState(MOSS_READY);
	pObj->SetLadderChallenging(false);
	pStage->PlayerTeam(pObj->GetUID(), nTeam);
	pStage->PlayerState(pObj->GetUID(), MOSS_READY);
	MCommand* pCmd = CreateCommand(MC_MATCH_LADDER_PREPARE, pObj->GetUID());
	pCmd->AddParameter(new MCmdParamUID(pStage->GetUID()));
	pCmd->AddParameter(new MCmdParamInt(nTeam));
	Post(pCmd);
	pStage->SetCLID(pObj->GetCharInfo()->m_ClanInfo.m_nClanID, nTeam);
//	}
	//__except( LadderWarsExceptionHandler(GetExceptionInformation(), "UpdateLadderWarsGame") ){ return;}
}

void MMatchServer::LaunchLadderWarsGame(PlayerJoiningDataStruct Data, LadderWars Type, int MapID)
{
	//	__try
	//{
	bool error = false;
	MUID uidStage = MUID(0, 0);
	if (StageAdd(NULL, "Blitz_Ladder", true, "", &uidStage) == false) {
		error = true;
	}
	MMatchStage* pStage = FindStage(uidStage);
	if (pStage == NULL) {
		error = true;
	}
	int MaxTeamCount = ceil((float)Data.size() / 2);
	int RedTeamCount = 0, BlueTeamCount = 0;
	if (error == false)
	{
		PlayerJoiningDataStruct Teams(Data);
		for (PlayerJoiningDataStruct::iterator i = Teams.begin(); i != Teams.end();)
		{
			MMatchObject* pObj = GetObject(i->first);
			if (pObj == NULL) { error = true; break; }
			if (pObj->GetStageUID() != MUID(0, 0))
				StageLeave(pObj->GetUID());
			MMatchStage* pStage = FindStage(uidStage);
			if (pStage == NULL) { error = true; break; }
			if (i->second != MUID(0, 0))
			{
				MMatchTeam nTeam = (MMatchTeam)RandomNumber(MMT_RED, MMT_BLUE);
				if (nTeam == MMT_RED && (RedTeamCount + 2) <= MaxTeamCount || nTeam == MMT_BLUE && (BlueTeamCount + 2) <= MaxTeamCount)
				{
					UpdateLadderWarsGame(pStage, pObj, nTeam);
					MMatchObject* pf = GetObject(i->second);
					if (pf)
					{
						UpdateLadderWarsGame(pStage, pf, nTeam);
						Teams.erase(Teams.find(i->second));
						if (nTeam == MMT_RED)
							RedTeamCount++;
						else
							BlueTeamCount++;
					}
					if (nTeam == MMT_RED)
						RedTeamCount++;
					else
						BlueTeamCount++;
					i = Teams.erase(i++);
				}
				else i++;
			}
			else i++;
		}

		for (PlayerJoiningDataStruct::iterator i = Teams.begin(); i != Teams.end();)
		{
			MMatchObject* pObj = GetObject(i->first);
			if (pObj == NULL) { error = true; break; }
			if (pObj->GetStageUID() != MUID(0, 0))
				StageLeave(pObj->GetUID());
			MMatchStage* pStage = FindStage(uidStage);
			if (pStage == NULL) { error = true; break; }
			MMatchTeam nTeam = (MMatchTeam)RandomNumber(MMT_RED, MMT_BLUE);
			if (RedTeamCount == MaxTeamCount) nTeam = MMT_BLUE; else if (BlueTeamCount == MaxTeamCount) nTeam = MMT_RED;
			switch (nTeam)
			{
			case MMT_RED:
				RedTeamCount++;
				break;
			case MMT_BLUE:
				BlueTeamCount++;
				break;
			}
			UpdateLadderWarsGame(pStage, pObj, nTeam);
			i = Teams.erase(i++);
		}
	}
	if (error == true)
	{
		for (PlayerJoiningDataStruct::iterator i = Data.begin(); i != Data.end(); i++)
		{
			MMatchObject* pObj = GetObject(i->first);
			if (pObj == NULL) continue;
			(pObj->GetCharInfo()->m_nCID);
#ifdef _REJOIN_NEW
			ClanRejoiner.erase(ClanRejoiner.find(pObj->GetCharInfo()->m_nCID));
#endif
			GetLadderWarsMgr()->GetWaitingGames(Type)->AddPlayer(pObj->GetUID());
		}
		pStage->ChangeState(STAGE_STATE_CLOSE);
		return;
	}

	//Custom: Fixed NAT By Desperate
	//ReserveAgent(pStage);
	pStage->SetStageType(MST_LADDERWARS);
	pStage->ChangeRule(MMATCH_GAMETYPE_DEATHMATCH_TEAM);
	MMatchStageSetting* pSetting = pStage->GetStageSetting();
	int TimeLimit = 3 * 60 * 1000; //Update Time
	if (!pSetting)
	{
		for (PlayerJoiningDataStruct::iterator i = Data.begin(); i != Data.end(); i++)
		{
			MMatchObject* pObj = GetObject(i->first);
			if (pObj == NULL) continue;
			(pObj->GetCharInfo()->m_nCID);
#ifdef _REJOIN_NEW
			ClanRejoiner.erase(ClanRejoiner.find(pObj->GetCharInfo()->m_nCID));
#endif
			GetLadderWarsMgr()->GetWaitingGames(Type)->AddPlayer(pObj->GetUID());
		}
		pStage->ChangeState(STAGE_STATE_CLOSE);
		return;
	}
	pSetting->SetMasterUID(MUID(0, 0));
	pSetting->SetMapIndex(MapID);
	pSetting->SetGameType(MMATCH_GAMETYPE_DEATHMATCH_TEAM);
	pSetting->SetLimitTime(TimeLimit);
	pSetting->SetRoundMax(99);
#ifdef _BOXLEAD
	pSetting->SetLead(false);
#endif
	MCommand* pCmd = CreateCmdResponseStageSetting(uidStage);
	RouteToStage(uidStage, pCmd);
	if ((MGetMapDescMgr()->MIsCorrectMap(MapID)) && (MGetGameTypeMgr()->IsCorrectGameType(MMATCH_GAMETYPE_DEATHMATCH_TEAM)))
	{
		if (pStage->StartGame(MGetServerConfig()->IsUseResourceCRC32CacheCheck()) == true) {		// 게임시작
			MMatchObjectCacheBuilder CacheBuilder;
			CacheBuilder.Reset();
			for (MUIDRefCache::iterator i = pStage->GetObjBegin(); i != pStage->GetObjEnd(); i++) {
				MUID uidObj = (MUID)(*i).first;
				MMatchObject* pScanObj = (MMatchObject*)GetObject(uidObj);
				if (pScanObj) {
					CacheBuilder.AddObject(pScanObj);
				}
			}
			MCommand* pCmdCacheAdd = CacheBuilder.GetResultCmd(MATCHCACHEMODE_UPDATE, this);
			RouteToStage(pStage->GetUID(), pCmdCacheAdd);
			MCommand* pCmd = CreateCommand(MC_MATCH_LADDER_LAUNCH, MUID(0, 0));
			pCmd->AddParameter(new MCmdParamUID(uidStage));
			pCmd->AddParameter(new MCmdParamStr(const_cast<char*>(pStage->GetMapName())));
			pCmd->AddParameter(new MCmdParamBool(true));
			RouteToStage(uidStage, pCmd);
		}
	}
	//	}
	//	__except( PlayerWarsExceptionHandler(GetExceptionInformation(), "LaunchPlayerWarsGame") ){}
}

void MMatchServer::SendLadderWarsCharInfoToPlayer(MUID uidPlayer)
{
	MMatchObject *pPWObj = GetPlayerByCommUID(uidPlayer);
	if(IsEnabledObject(pPWObj) == false || !pPWObj) return;
	if(pPWObj->GetLadderWarsInfo() == NULL)
	{
		if( pPWObj->GetCharInfo() == NULL ) return;
		MAsyncDBJob_GetLadderWarsCharInfo *pAsyncDbJob = new MAsyncDBJob_GetLadderWarsCharInfo;
		if( 0 == pAsyncDbJob ) return;
		pAsyncDbJob->Input(uidPlayer, pPWObj->GetCharInfo()->m_nCID);
		MMatchServer::GetInstance()->PostAsyncJob( pAsyncDbJob );
	} else PostLadderWarsCharInfo(uidPlayer, pPWObj->GetLadderWarsInfo());
}


void MMatchServer::OnAsyncGetLadderWarsCharInfo(MAsyncJob *pJobResult)
{
		__try
	{
	MAsyncDBJob_GetLadderWarsCharInfo* pJob = (MAsyncDBJob_GetLadderWarsCharInfo*)pJobResult;

	if( MASYNC_RESULT_SUCCEED != pJob->GetResult() ) {
		mlog("MMatchServer::OnAsyncResponse_GetLadderWarsCharacterInfo - Error\n");
		return;
	}

	MMatchObject* pObj = GetObject(pJob->GetPlayerUID());
	if( pObj == NULL ) return;
	pObj->SetLadderWarsCharInfo(pJob->GetPWCharInfo());
						
	PostLadderWarsCharInfo(pObj->GetUID(), pObj->GetLadderWarsInfo());
	}
	__except( LadderWarsExceptionHandler(GetExceptionInformation(), "OnAsyncGetLadderWarsCharInfo") ){}
}

void MMatchServer::PostLadderWarsCharInfo(MUID uidPlayer, LadderWarsCharInfo *pLadderCharInfo)
{
	if(pLadderCharInfo == NULL ) return;
	MCommand *pCmd = CreateCommand(MC_MATCH_LADDERWARS_CHARINFO, uidPlayer);
	pCmd->AddParameter(new MCommandParameterInt(pLadderCharInfo->Ranking));
	pCmd->AddParameter(new MCommandParameterInt(pLadderCharInfo->Wins));
	pCmd->AddParameter(new MCommandParameterInt(pLadderCharInfo->Losses));
	pCmd->AddParameter(new MCommandParameterInt(pLadderCharInfo->Draws));
	pCmd->AddParameter(new MCommandParameterInt(pLadderCharInfo->Score));
	Post(pCmd);
}

void MMatchServer::UpdateLadderWarsQueueingCounts(MTD_LadderWarsCounterInfo CounterInfo)
{
		MCommand* pNew = CreateCommand(MC_MATCH_LADDERWARS_COUNTER, MUID(0,0));
		void* pCounterArray	= MMakeBlobArray( sizeof(MTD_LadderWarsCounterInfo), 1 );
		MTD_LadderWarsCounterInfo* pSendCounter = (MTD_LadderWarsCounterInfo*)MGetBlobArrayElement( pCounterArray, 0 );
		memcpy(pSendCounter, &CounterInfo, sizeof(MTD_LadderWarsCounterInfo));
		pNew->AddParameter(new MCommandParameterBlob(pCounterArray, MGetBlobArraySize(pCounterArray)));
		for(MMatchObjectList::iterator i=m_Objects.begin(); i!=m_Objects.end(); i++) 
		{
				MMatchObject *pObj = (MMatchObject*)((*i).second);
				if( pObj != NULL && pObj->GetPlace() == MMP_LOBBY) 
				{
					MMatchChannel* pChannel = FindChannel(pObj->GetChannelUID());
					if(pChannel && pChannel->GetChannelType() == MCHANNEL_TYPE_LADDER)
					{	
						MCommand* pSendCmd = pNew->Clone();
						pSendCmd->m_Receiver = pObj->GetUID();
						Post(pSendCmd);
					}
				}
		}
		MEraseBlobArray(pCounterArray);
}

void MMatchServer::UpdateLadderWarsCharInfo(int cid, int win, int loss, int draw, int score, int mode)
{
	MAsyncDBJob_UpdateLadderWarsCharInfo *pAsyncDbJob = new MAsyncDBJob_UpdateLadderWarsCharInfo;
	if( 0 == pAsyncDbJob ) return;
	pAsyncDbJob->Input(cid, win, loss, draw, score, mode);
	MMatchServer::GetInstance()->PostAsyncJob( pAsyncDbJob );

}

void MMatchServer::OnAsyncResponse_GetLadderWarsSideRanking(MAsyncJob *pJobResult)
{
		__try
	{
	MAsyncDBJob_GetLadderWarsSideRankingInfo* pJob = (MAsyncDBJob_GetLadderWarsSideRankingInfo*)pJobResult;

	if( MASYNC_RESULT_SUCCEED != pJob->GetResult() ) {
		mlog("MMatchServer::OnAsyncResponse_GetLadderWarsSideRanking - 8\n");
		return;
	}

	MMatchObject* pObj = GetObject(pJob->GetPlayerUID());
	if( pObj == NULL ) return;
	PostCmdLadderWarsCharSideRankingInfo(pObj->GetUID(), pJob->GetSideRankingList());
	}
	__except( LadderWarsExceptionHandler(GetExceptionInformation(), "OnAsyncResponse_GetLadderWarsSideRanking") ){}
}

void MMatchServer::PostCmdLadderWarsCharSideRankingInfo(MUID uidPlayer, list<PWRankingInfo*>* pSideRankingList)
{
	MMatchObject *pDTObj = GetObject(uidPlayer);
	if(IsEnabledObject(pDTObj) == false) return;

	void* pBlobRanking = MMakeBlobArray(sizeof(DTRankingInfo), (int)pSideRankingList->size() );

	int index = 0;
	list<PWRankingInfo*>::iterator iter;
	for(iter = pSideRankingList->begin(); iter != pSideRankingList->end(); ++iter){
		PWRankingInfo *pInfoDest = reinterpret_cast<PWRankingInfo *>(MGetBlobArrayElement(pBlobRanking, index++));	
		PWRankingInfo *pInfoSrc = (*iter);

		if( pInfoDest == NULL || pInfoSrc == NULL ) { MEraseBlobArray( pBlobRanking ); return; }
		memcpy(pInfoDest, pInfoSrc, sizeof(DTRankingInfo));
	}
	
	MCommand* pCmd = CreateCommand(MC_MATCH_LADDERWARS_SIDERANK, uidPlayer);
	if( NULL == pCmd ) {
		MEraseBlobArray( pBlobRanking );
		return;
	}
	pCmd->AddParameter( new MCommandParameterBlob(pBlobRanking, MGetBlobArraySize(pBlobRanking)) );
	MEraseBlobArray( pBlobRanking );

	Post(pCmd);
}

void MMatchServer::SendLadderWarsSideRankingToPlayer(MUID uidPlayer)
{

	MMatchObject *pPWObj = GetPlayerByCommUID(uidPlayer);
	if(IsEnabledObject(pPWObj) == false || !pPWObj) return;
	if( pPWObj->GetCharInfo() == NULL ) return;
	MAsyncDBJob_GetLadderWarsSideRankingInfo *pAsyncDbJob = new MAsyncDBJob_GetLadderWarsSideRankingInfo;
	if( 0 == pAsyncDbJob ) return;
	pAsyncDbJob->Input(uidPlayer, pPWObj->GetCharInfo()->m_nCID);
	MMatchServer::GetInstance()->PostAsyncJob( pAsyncDbJob );
}

void MMatchServer::SendLadderWarsRewards(LadderWarsListDef List, bool bIsDrawGame, MMatchTeam nWinnerTeam, int WinningScore, int LosingScore, int MapId)
{
	char szWinnerMembers[512] = "";
	char szLoserMembers[512] = "";
	int Score = ceil((float)List.size()/2);
	for (LadderWarsListDef::iterator itor=List.begin(); itor!=List.end(); itor++) 
	{
		LadderWarsInfos* Info = (LadderWarsInfos*)(*itor).second;
		if(Info)
		{
			MMatchObject *pObj = GetPlayerByCommUID(Info->Player);
			if (pObj && IsEnabledObject(pObj) && pObj->GetLadderWarsInfo() && pObj->GetCharInfo())
			{

				if (pObj->GetTeam() == nWinnerTeam)
				{
					if(bIsDrawGame)
					{
						pObj->GetLadderWarsInfo()->AddDraw();
						pObj->GetLadderWarsInfo()->AddScore(ceil((float)Score/2));
					}
					else
					{
						pObj->GetLadderWarsInfo()->AddWin();
						pObj->GetLadderWarsInfo()->AddScore(Score);
					}
				}
				else
				{
					if(bIsDrawGame)
					{
						pObj->GetLadderWarsInfo()->AddDraw();
						pObj->GetLadderWarsInfo()->AddScore(ceil((float)Score/2));
					}
					else
					{
						pObj->GetLadderWarsInfo()->AddLoss();
						pObj->GetLadderWarsInfo()->MinusScore(Score);
					}
				}

				SendLadderWarsCharInfoToPlayer(pObj->GetUID());
			} 
			
			if (Info->Team == nWinnerTeam)
			{
				
				char cid[100] = "";
				sprintf(cid, "%d", (int)(*itor).first);
				strcat(szWinnerMembers, cid);
				strcat(szWinnerMembers, " ");
				if(bIsDrawGame)
					UpdateLadderWarsCharInfo((int)(*itor).first, 0, 0, 1, ceil((float)Score/2), 0);
				else
					UpdateLadderWarsCharInfo((int)(*itor).first, 1, 0, 0, Score, 0);
			}
			else
			{
				char cid[100] = "";
				sprintf(cid, "%d", (int)(*itor).first);
				strcat(szLoserMembers, cid);
				strcat(szLoserMembers, " ");
				if(bIsDrawGame)
					UpdateLadderWarsCharInfo((int)(*itor).first, 0, 0, 1, ceil((float)Score/2), 0);
				else
					UpdateLadderWarsCharInfo((int)(*itor).first, 0, 1, 0, 0, 0);
			}
		}
	}
	m_MatchDBMgr.InsertBZGameLog(szWinnerMembers, szLoserMembers, WinningScore, LosingScore, MapId);
}