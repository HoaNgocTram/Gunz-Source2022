#include "stdafx.h"
#include "MLadderWarsMgr.h"
#include "MTeamGameStrategy.h"
#include "MCrashDump.h"
#include <Include\MMatchConfig.h>

DWORD LadderWarsMgrExceptionHandler( PEXCEPTION_POINTERS ExceptionInfo, char* Place )
{
	char szStageDumpFileName[ _MAX_DIR ]= {0,};
	SYSTEMTIME SystemTime;
	GetLocalTime( &SystemTime );
	sprintf( szStageDumpFileName, "Log/MBlitzkriegsMgr[%s]_%d-%d-%d_%d-%d-%d.dmp"
		, Place
		, SystemTime.wYear
		, SystemTime.wMonth
		, SystemTime.wDay
		, SystemTime.wHour
		, SystemTime.wMinute
		, SystemTime.wSecond );

	return CrashExceptionDump( ExceptionInfo, szStageDumpFileName, true );
}

void MLadderWarsMgr::Tick()
{
	__try
	{
		unsigned long int NowTime = timeGetTime();
		if(NowTime - nLastTime >= 5000)
		{
			MatchMaking(NowTime);
			UpdateMapCountDown(NowTime);
			UpdateQueueingCounters(NowTime);
			nLastTime = NowTime;
		}	
	}
	__except( LadderWarsMgrExceptionHandler(GetExceptionInformation(), "Tick") ){}
}

void MLadderWarsMgr::UpdateQueueingCounters(unsigned long int NowTime)
{
	__try
	{
		if(NowTime - nLastTime1 >= 15000)
		{
			MTD_LadderWarsCounterInfo CountInfo;
			for(int i = 0; i < MPLADDERWARSTYPE_MAX; i++)
			{
				LadderWarsData* TempWaitingGames = GetWaitingGames((LadderWars)i);
				CountInfo.Count[i] = TempWaitingGames->GetCount();
			}
			MMatchServer::GetInstance()->UpdateLadderWarsQueueingCounts(CountInfo);
			nLastTime1 = NowTime;
		}
	}
	__except( LadderWarsMgrExceptionHandler(GetExceptionInformation(), "UpdateQueueingCounters") ){}
}

void MLadderWarsMgr::RemovePlayer(MUID Player)
{		
	for(int i = MPLADDERWARSTYPE_NORMAL_1VS1; i < MPLADDERWARSTYPE_MAX; i++)
	{
		LadderWars Type = (LadderWars)i;
		LadderWarsData* TempWaitingGames = GetWaitingGames(Type);
		if(!TempWaitingGames) continue;
		if(TempWaitingGames->RemovePlayer(Player) == true) break;
	}
	MCommand* pCmd = MMatchServer::GetInstance()->CreateCommand(MC_MATCH_LADDER_CANCEL_CHALLENGE, Player);
	pCmd->AddParameter(new MCmdParamStr(""));
	pCmd->AddParameter(new MCmdParamBool(true));
		pCmd->m_Receiver = Player;
		MMatchServer::GetInstance()->Post(pCmd);
}

void MLadderWarsMgr::MatchMaking(unsigned long int NowTime)
{
	//__try
//	{
		for(int Type = MPLADDERWARSTYPE_NORMAL_1VS1; Type < MPLADDERWARSTYPE_MAX; Type++)
		{
			LadderWarsData* TempWaitingGames = GetWaitingGames((LadderWars)Type);
			if(!TempWaitingGames) continue;
			int PlayerCount= 0;
			switch((LadderWars)Type)
			{
				case MPLADDERWARSTYPE_NORMAL_1VS1: PlayerCount = 2; break;
				case MPLADDERWARSTYPE_NORMAL_2VS2: PlayerCount = 4; break;
				case MPLADDERWARSTYPE_NORMAL_3VS3: PlayerCount = 6; break;
				case MPLADDERWARSTYPE_NORMAL_4VS4: PlayerCount = 8; break;
				case MPLADDERWARSTYPE_NORMAL_5VS5: PlayerCount = 10; break;
				case MPLADDERWARSTYPE_NORMAL_6VS6: PlayerCount = 12; break;
				case MPLADDERWARSTYPE_NORMAL_7VS7: PlayerCount = 14; break;
				case MPLADDERWARSTYPE_NORMAL_8VS8: PlayerCount = 16; break;
			}
			int Fails = 0;
			while(TempWaitingGames->GetCount() >= PlayerCount)
			{
				if(Fails >= 12) break;
				PlayerJoiningDataStruct ReadyGame;
				for(int l = 0; l < PlayerCount;)
				{
					if(Fails >= 12) break;
					MUID Player = TempWaitingGames->GetFirst();
					MMatchObject* pObj = MMatchServer::GetInstance()->GetObject(Player);
					if (!IsEnabledObject(pObj) || !pObj)
					{
						TempWaitingGames->RemovePlayer(Player);
						Fails++;
						continue;
					}
					else
					{
						switch (pObj->PwState)
						{
							case MMatchObject::Invited:
							{
								MMatchObject* pFriendObj = MMatchServer::GetInstance()->GetObject(pObj->LadderWarsFriend);
								if(IsEnabledObject(pFriendObj) && pFriendObj) 
								{ 
									if(pFriendObj->LadderWarsFriend == pObj->GetUID() && TempWaitingGames->IsPlayerAlreadyQueued(pObj->LadderWarsFriend) == true)
									{
										TempWaitingGames->RemovePlayer(pObj->GetUID());
										continue;
									}
								}
								pObj->PwState = MMatchObject::NotInvited;
								pObj->LadderWarsFriend = MUID(0, 0);
								ReadyGame.insert(PlayerJoiningDataStruct::value_type(pObj->GetUID(), pObj->LadderWarsFriend));
								TempWaitingGames->RemovePlayer(pObj->GetUID());
								l++;
								continue;
							}
							break;

							case MMatchObject::Inviter:
							{
								if((PlayerCount - ReadyGame.size()) <= 1) 
								{
								
									TempWaitingGames->RemovePlayer(Player);
									TempWaitingGames->AddPlayer(Player);
									Fails++;
									continue;
								}
								MMatchObject* pFriendObj = MMatchServer::GetInstance()->GetObject(pObj->LadderWarsFriend);
								if(IsEnabledObject(pFriendObj) && pFriendObj) 
								{ 
									MMatchChannel* chan = MGetMatchServer()->FindChannel(pFriendObj->GetChannelUID());
									if(chan->GetChannelType() == MCHANNEL_TYPE_LADDER && pFriendObj->GetPlace() == MMP_LOBBY)
									{
										if(TempWaitingGames->IsPlayerAlreadyQueued(pObj->LadderWarsFriend) == true)
											TempWaitingGames->RemovePlayer(pObj->LadderWarsFriend);
											ReadyGame.insert(PlayerJoiningDataStruct::value_type(pObj->LadderWarsFriend, MUID(0, 0)));
											l++;
									}
									else
									{
										pFriendObj->LadderWarsFriend = MUID(0, 0);
										pFriendObj->PwState = MMatchObject::NotInvited;
										pObj->LadderWarsFriend = MUID(0, 0);
										pObj->PwState = MMatchObject::NotInvited;
									}
								} else { pObj->LadderWarsFriend = MUID(0, 0); pObj->PwState = MMatchObject::NotInvited; }
								ReadyGame.insert(PlayerJoiningDataStruct::value_type(pObj->GetUID(), pObj->LadderWarsFriend));
								TempWaitingGames->RemovePlayer(pObj->GetUID());
								l++;
								continue;
							}
							break;

							case MMatchObject::NotInvited:
							{
								ReadyGame.insert(PlayerJoiningDataStruct::value_type(pObj->GetUID(), MUID(0, 0)));
								TempWaitingGames->RemovePlayer(pObj->GetUID());
								l++;
								continue;
							}
							break;
						}
					}
				}
				int ReadySize = ReadyGame.size();
				if(ReadySize == PlayerCount)
				{
					MapSelection(ReadyGame, (LadderWars)Type, NowTime);
				}
			}
		}
	//}
	//__except( LadderWarsMgrExceptionHandler(GetExceptionInformation(), "MatchMaking") ){}
}

void MLadderWarsMgr::UpdateMapCountDown(unsigned long int NowTime)
{
	//__try
	//{
		for(map<unsigned long int, MapSelectionStruct*>::iterator i = WaitingMapSelectionGames.begin(); i != WaitingMapSelectionGames.end();)
		{
			MapSelectionStruct* m = i->second;
			if(!m) 
			{
				i = WaitingMapSelectionGames.erase(i);
				continue;
			}
			if((NowTime - m->RegisterTime) >= 20000)
			{
				int winningmapindex = -1, winningindex = -1, votecount = 0;
				for(int a = 0; a < 3; a++)
				{
					if(m->Votes[a] == votecount)
					{
						votecount = m->Votes[a];
						winningmapindex = m->Maps[a];
						winningindex = a;
					}
					else if(m->Votes[a] >= votecount)
					{
						votecount = m->Votes[a];
						winningmapindex = m->Maps[a];
						winningindex = a;
					}
				}
				if(winningmapindex == -1)
				winningmapindex = m->Maps[RandomNumber(0, 2)];
				else
				{
					switch(winningindex)
					{
					case 0:
						if(m->Votes[winningindex] == m->Votes[1])
							winningmapindex = m->Maps[RandomNumber(0, 1)];
						else if(m->Votes[winningindex] == m->Votes[2])
							if(RandomNumber(0, 1) == 0)
							winningmapindex = m->Maps[winningindex];
							else
							winningmapindex = m->Maps[2];
						break;
					case 1:
						if(m->Votes[winningindex] == m->Votes[0])
							winningmapindex = m->Maps[RandomNumber(0, 1)];
						else if(m->Votes[winningindex] == m->Votes[2])
							if(RandomNumber(0, 1) == 0)
							winningmapindex = m->Maps[winningindex];
							else
							winningmapindex = m->Maps[2];
						break;
					case 2:					
						if(m->Votes[winningindex] == m->Votes[1])
							winningmapindex = m->Maps[RandomNumber(1, 2)];
						else if(m->Votes[winningindex] == m->Votes[0])
							if(RandomNumber(0, 1) == 1)
							winningmapindex = m->Maps[winningindex];
							else
							winningmapindex = m->Maps[0];
						break;
					}
				}
				i = WaitingMapSelectionGames.erase(i);
				MMatchServer::GetInstance()->LaunchLadderWarsGame(m->Players, m->Type, winningmapindex);
				continue;
			} 
			else 
			{	
				MCommand* pCommand = MMatchServer::GetInstance()->CreateCommand(MC_MATCH_VOTE_UPDATE,  MUID(0,0));
				pCommand->AddParameter(new MCmdParamInt(m->Votes[0]));
				pCommand->AddParameter(new MCmdParamInt(m->Votes[1]));
				pCommand->AddParameter(new MCmdParamInt(m->Votes[2]));
				for (PlayerJoiningDataStruct::iterator i=m->Players.begin(); i!=m->Players.end(); i++) 
				{
					MMatchObject* pObj = MMatchServer::GetInstance()->GetObject(i->first);
					if (pObj) 
					{
						MCommand* pSendCmd = pCommand->Clone();
						MMatchServer::GetInstance()->RouteToListener(pObj, pSendCmd);
					}
				}
				delete pCommand;
			}
			i++;
		}
	//}
	//__except( LadderWarsMgrExceptionHandler(GetExceptionInformation(), "UpdateMapCountDown") ){}
}

void MLadderWarsMgr::MapSelection(PlayerJoiningDataStruct ReadyGame, LadderWars Type, unsigned long int NowTime)
{
	//__try
	//{
		MapSelectionStruct* m = new MapSelectionStruct();
		m->Players = ReadyGame;
		m->Type = Type;
		for(int i = 0; i < 3; i++)
		{
			m->Votes[i] = 0;
			m->Maps[i] = -1;
		}
		
		int MaxTeamCount = ceil((float)(m->Players.size() / 2));
		MBaseTeamGameStrategy* pTeamGameStrategy = MBaseTeamGameStrategy::GetInstance(MGetServerConfig()->GetServerMode());
		if (pTeamGameStrategy)
		{
			for(int i = 0; i < 3; i++)
			{
				int index = pTeamGameStrategy->GetLadderWarsRandomMap(MaxTeamCount);
				while(index == m->Maps[0] || index == m->Maps[1] || index == m->Maps[2])
					index = pTeamGameStrategy->GetLadderWarsRandomMap(MaxTeamCount);
				m->Maps[i] = index;
			}
		};
		int ID = counter++;
		m->RegisterTime = NowTime;
		MCommand* pCommand = MMatchServer::GetInstance()->CreateCommand(MC_MATCH_RANDOM_MAPS,  MUID(0,0));
		pCommand->AddParameter(new MCmdParamInt(m->Maps[0]));
		pCommand->AddParameter(new MCmdParamInt(m->Maps[1]));
		pCommand->AddParameter(new MCmdParamInt(m->Maps[2]));
		for (PlayerJoiningDataStruct::iterator i=m->Players.begin(); i!=m->Players.end(); i++) 
		{
			MMatchObject* pObj = MMatchServer::GetInstance()->GetObject(i->first);
			if (pObj) 
			{
				pObj->LadderWarsIdentifier = ID;
				MCommand* pSendCmd = pCommand->Clone();
				MMatchServer::GetInstance()->RouteToListener(pObj, pSendCmd);
			}
		}
		delete pCommand;
		WaitingMapSelectionGames.insert(map<unsigned long int, MapSelectionStruct*>::value_type(ID, m));
	//}
	//__except( LadderWarsMgrExceptionHandler(GetExceptionInformation(), "MapSelection") ){}
}

