#pragma once

#include "MMatchGlobal.h"
#include "MLadderGroup.h"
#include "MLadderStatistics.h"
#include <vector>

class MMatchObject;

enum LadderGameTypes
{
	Normal = 0,
	Assassinate,
	Gladiator,
	EndGames
};

#ifdef _CW_VOTE
struct LadderGameMapVoteInfo
{
	MLadderGroup* pGroupA;
	MLadderGroup* pGroupB;
	int Maps[3];
	int Votes[3];
	int SelectedMap;
	unsigned long int RegisterTime;
};
#endif

class MLadderMgr {
protected:
	int					m_idGenerate;
	unsigned long		m_nLastTick;

	MLadderGroupMap		m_WaitingMaps[MLADDERTYPE_MAX];
	list<MLadderGroup*>	m_GroupList;

	MLadderStatistics	m_Stat;
protected:
	unsigned long int counter;
#ifdef _CW_VOTE
	map<unsigned long int, LadderGameMapVoteInfo*> WaitingMapSelectionGames;
#endif
	unsigned long GetLastTick()	{ return m_nLastTick; }
	void SetLastTick(unsigned long nTick)	{ m_nLastTick = nTick; }

#ifdef _CW_VOTE
	void UpdateMapCountDown(unsigned long int NowTime);
#endif // _CW_VOTE

	inline MLadderGroupMap* GetWaitGroupContainer(MLADDERTYPE nLadderType);

	void AddGroup(MLADDERTYPE nLadderType, MLadderGroup* pGroup);
	int MakeMatch(MLADDERTYPE nLadderType);
	void LaunchLadder(MLADDERTYPE nLadderType, int nGroupA, int nGroupB);
	void RemoveFromGroupList(MLadderGroup* pGroup);
	void CleaningGarbages();
	unsigned long int GetTickInterval();
public:
	MLadderMgr() {
		m_idGenerate = 0;
		counter = 0;
	}
	bool Init();
	MLadderGroup* CreateLadderGroup();
	MLadderGroup* FindLadderGroup(int nGroupID);
	bool Challenge(MLadderGroup* pGroup);
	void CancelChallenge(int nGroupID, const char* pszCancelName);

#ifdef _CW_VOTE
	void UpdatePlayerVote(int VoteID, MMatchObject* pObj);
#endif // _CW_VOTE


	void Tick(unsigned long nTick);
	int GenerateID()	{ return ++m_idGenerate; }	
	int GetNeedMemberCount(MLADDERTYPE nLadderType);
	int GetTotalGroupCount();
	unsigned long GetChecksum(int nFirstIndex, int nGroupCount);

	void DebugTest();
	
	list<MLadderGroup*>::iterator GetGroupListBegin()		{ return m_GroupList.begin(); }
	list<MLadderGroup*>::iterator GetGroupListEnd()			{ return m_GroupList.end(); }
	size_t GetGroupCount()									{ return m_GroupList.size(); }

	MLadderStatistics*	GetStatistics()						{ return &m_Stat; }
};




