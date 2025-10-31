#ifndef _ZRULE_QUESTCHALLENGE_H
#define _ZRULE_QUESTCHALLENGE_H

#include "IRuleNewQuest.h"
#include "MMatchUtil.h"
#include "MNEWQUESTSCENARIO.H"
//class MNewQuestScenario;


class ZRuleQuestChallenge : public IRuleNewQuest
{
	MUID m_uidBoss;

	MNewQuestScenario* m_pScenario;	// 이 퀘스트의 시나리오
	int m_nCurrSector;				// 현재 진행중인 섹터 번호
	UINT m_nRewardXP;
	UINT m_nRewardBP;
	int m_nItemID;
	//DWORD m_FadeBGTime;
	float m_fElapsedTime;
	std::string m_szElapsedTime;
	bool m_bLateJoin;
	DWORD m_FadeBGTime;
public:
	ZRuleQuestChallenge(ZMatch* pMatch);
	virtual ~ZRuleQuestChallenge();

	std::vector<int> m_OurCQRewards;

public:

	static bool LoadScenarioMap(const char* szScenarioName);

	virtual bool Init();
	virtual bool OnCommand(MCommand* pCommand);
	virtual void OnUpdate(float fDelta);
	virtual void OnSetRoundState(MMATCH_ROUNDSTATE roundState);
	virtual ZActorWithFSM* SpawnActor(const char* szActorDefName, MUID& uid, int nCustomSpawnTypeIndex, int nSpawnIndex, bool bMyControl);
	virtual ZActorWithFSM* SpawnActorSummon(const char* szActorName, MUID& uid, int nNum, bool bMyControl, MShortVector vec, MShortVector vec2,
		int route);
	virtual ZActorWithFSM* SpawnLateJoinNpc(const char* szActorDefName, MUID& uid, int nCustomSpawnTypeIndex, int nSpawnIndex, bool bMyControl = false);
	
	virtual void CheckSpecialEvent(ZActorWithFSM* pDeadActor, MMatchTeam nTeamID) {
		return;
	}

	virtual int GetRoundMax();
	virtual int	GetCurrSector();
	virtual int GetNPC();
	//int GetNPC() { return IRuleNewQuest::GetNPC(); } 

	void MoveToNextSector();
	void MoveToRealSector(int currSector);//retrieved from server :)

	//Custom:
	int GetClearedSectors() { return m_nCurrSector; }
	uInt GetRewardXP() { return m_nRewardXP; }
	uInt GetRewardBP() { return m_nRewardBP; }
	bool OnSectorBonus(MCommand* pCommand);
	DWORD GetElapsedTime() { return IRuleNewQuest::GetElapsedTime(); }
	DWORD GetCurrTime();
	MUID GetBoss() { return m_uidBoss; }
	const int GetSectorCount() const { return m_pScenario->GetNumSector(); }
	const char* GetCurrTimeString() { return m_szElapsedTime.c_str(); }
};



#endif