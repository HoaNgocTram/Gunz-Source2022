#pragma once
#include <map>
using namespace std;
#include "MMatchGlobal.h"
#include "MUID.h"
typedef map<MUID, MUID> PlayerJoiningDataStruct;

struct MapSelectionStruct
{
	PlayerJoiningDataStruct Players;
	LadderWars Type;
	int Maps[3];
	int Votes[3];
	int SelectedMap;
	unsigned long int RegisterTime;
};

class LadderWarsData
{
private:
	vector<MUID> AntiLead; 
	LadderWarsData* m_pInstance;
public:
	LadderWarsData()
	{
		_ASSERT(m_pInstance==NULL);
		m_pInstance = this;
	}
	LadderWarsData* GetInstance() { return m_pInstance; }

	bool IsPlayerAlreadyQueued(MUID ID)
	{
		vector<MUID>::iterator i;
		i = find(AntiLead.begin(), AntiLead.end(), ID);
		if(i != AntiLead.end()) return true;
		return false;
	}

	void AddPlayer(MUID ID)
	{  
			AntiLead.push_back(ID); 
	}

	bool RemovePlayer(MUID ID) 
	{ 
		vector<MUID>::iterator i = find(AntiLead.begin(), AntiLead.end(), ID);
		if(i != AntiLead.end()) 
		{
			AntiLead.erase(i);
			return true;
		}
		return false;
	}
	MUID GetFirst() { return (MUID)(*AntiLead.begin()); }
	int GetCount() { return AntiLead.size(); }
};

class MLadderWarsMgr
{
private:
	unsigned long int nLastTime, nLastTime1;
	LadderWarsData WatingGames[MPLADDERWARSTYPE_MAX];
	MLadderWarsMgr* m_pInstance;
	map<unsigned long int, MapSelectionStruct*> WaitingMapSelectionGames;
	unsigned long int counter;
public: 
	void Tick();
	LadderWarsData* GetWaitingGames(LadderWars Type) { return WatingGames[(int)Type].GetInstance(); }
	void MapSelection(PlayerJoiningDataStruct ReadyGame, LadderWars Type, unsigned long int NowTime);
	void MatchMaking(unsigned long int NowTime);
	void UpdateQueueingCounters(unsigned long int NowTime);
	void UpdateMapCountDown(unsigned long int NowTime);
	void RemovePlayer(MUID PlayerID);
	bool IsWaitingGame(MUID PlayerID)
	{
		for(map<unsigned long int, MapSelectionStruct*>::iterator i = WaitingMapSelectionGames.begin(); i != WaitingMapSelectionGames.end();)
		{
			MapSelectionStruct* m = i->second;
			if(m)
			for (PlayerJoiningDataStruct::iterator i=m->Players.begin(); i!=m->Players.end(); i++) 
				if(PlayerID == i->first) return true;
		}
		return false;
	}

	void UpdatePlayerVote(int VoteID, MMatchObject* pObj)
	{
		if(pObj->LadderWarsIdentifier != -1)
		{
			map<unsigned long int, MapSelectionStruct*>::iterator i = WaitingMapSelectionGames.find(pObj->LadderWarsIdentifier);
			if(i!=WaitingMapSelectionGames.end())
			{
				MapSelectionStruct*  m = i->second;
				if(m)
				{
					if(pObj->LastVoteID != -1)
						m->Votes[pObj->LastVoteID]--;
					m->Votes[VoteID]++;
					pObj->LastVoteID = VoteID;
				}
			}
		}
	}
	MLadderWarsMgr()
	{
		_ASSERT(m_pInstance==NULL);
		m_pInstance = this;
		counter = 0;
	}
	MLadderWarsMgr* GetInstance() { return m_pInstance; }
};