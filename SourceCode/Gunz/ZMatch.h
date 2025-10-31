#ifndef _ZMATCH_H
#define _ZMATCH_H

#include <list>
using namespace std;

#include "ZPrerequisites.h"
#include "MUID.h"
#include "MMatchStage.h"

class ZCharacter;
class MMatchStageSetting;
class ZRule;

/// ﾅｬｶﾌｾｮ ｰﾔﾀﾓ ｷ・ｰ・ﾃ ﾅｬｷ｡ｽｺ
class ZMatch
{
private:
protected:
	ZRule*					m_pRule;
	MMatchStageSetting*		m_pStageSetting;
	MMATCH_ROUNDSTATE		m_nRoundState;
	int						m_nCurrRound;
	int						m_nTeamScore[MMT_END];
	int						m_nRoundKills;			// ﾇ邯鋙蠡ﾇ ﾅｳｼ・- FirstKillﾃｼﾅｩﾇﾏｷﾁｰ・ｸｸｵ・
	int						m_nTeamKillCount[MMT_END]; // ｰ｢ ﾆﾀﾀﾇ ﾅｳ ｼ・ﾃﾑﾇﾕ

	unsigned long			m_nNowTime;
	unsigned long			m_nStartTime;
	unsigned long int		m_nLastDeadTime;
	int						m_nSoloSpawnTime;

	DWORD					m_dwStartTime;			// ｶ鋙蟶ｦ ｽﾃﾀﾛﾇﾟﾀｻ ｶｧﾀﾇ ｽﾃｰ｣ｰｪ

	void SoloSpawn();				///< ｳｻ ﾄｳｸｯﾅﾍ ｽｺﾆ・
	void InitCharactersProperties();	///< ﾄｳｸｯﾅﾍ HP, ﾃﾑｾﾋ ｵ・ﾃﾊｱ篳ｭ
	void InitRound();				///< ｶ鋙・ﾃｳﾀｽ ｽﾃﾀﾛｽﾃ ﾃﾊｱ篳ｭ
	void ProcessRespawn();
public:
	ZMatch();
	virtual ~ZMatch();
	bool Create();
	void Destroy();

	void Update(float fDelta);
	bool OnCommand(MCommand* pCommand);
	void OnResponseRuleInfo(MTD_RuleInfo* pInfo);
	void SetRound(int nCurrRound);						///< ﾇ・ｶ鋙・ｼｳﾁ､
	void OnForcedEntry(ZCharacter* pCharacter);			///< ｳｭﾀﾔﾇﾟﾀｻ ｰ豼・
	void InitCharactersPosition();						///< ｶ鋙・ｽﾃﾀﾛﾇﾒｶｧ ﾄｳｸｯﾅﾍ ﾀｧﾄ｡ ｼｳﾁ､
	void OnDrawGameMessage();
	void RespawnSolo();

	int GetRemainedSpawnTime();
	int GetRoundCount(); 
	int GetRoundReadyCount(void);
	void GetTeamAliveCount(int* pnRedTeam, int* pnBlueTeam);
	void GetTeamPlayers(int* pnRedTeam, int* pnBlueTeam, int* nTotalPlayers);
	const char* GetTeamName(int nTeamID);
	void SetRoundState(MMATCH_ROUNDSTATE nRoundState, int nArg=0);

	void SetRoundStartTime( DWORD dwElapsed ); // Custom: dwElapsed
	DWORD GetRoundStartTime() { return m_dwStartTime; }
	DWORD GetMatchTime(void);
	DWORD GetRemaindTime( void);

	inline int GetCurrRound();
	inline bool IsTeamPlay();
	inline bool IsSoloPlay();
	inline bool IsWaitForRoundEnd();
	inline bool IsQuestDrived();
	inline MMATCH_ROUNDSTATE GetRoundState();
	inline MMATCH_GAMETYPE GetMatchType();
	inline bool GetTeamKillEnabled();
	inline bool GetTeamRotation();
	inline const char* GetMapName();
	inline int GetTeamScore(MMatchTeam nTeam);
	inline void SetTeamScore(MMatchTeam nTeam, int nScore);
	inline int GetTeamKills(MMatchTeam nTeam);		// ﾆﾀﾀﾇ ﾃﾑﾇﾕﾅｳｼ・
	inline void AddTeamKills(MMatchTeam nTeam, int amount = 1);
	inline void SetTeamKills(MMatchTeam nTeam, int amount);
	inline int GetRoundKills();
	inline void AddRoundKills();
	inline bool IsRuleGladiator();
	inline bool IsRuleSpy();
	inline bool IsQuestChallengue();
	inline bool IsBlitz();
	inline bool IsMapSkill();
	inline bool IsClassiMode();
	inline bool IsTurboMode();
	inline ZRule* GetRule();
};



#define DEFAULT_ONETURN_GAMETIME		300000		// ﾇﾑﾅﾏ ｱ篌ｻ ｴ・篆ﾃｰ｣ = 5ｺﾐ
#define DEFAULT_WAITTIME				120000		// ｱ篌ｻ ｴ・・ｽﾃｰ｣ = 2ｺﾐ
#define DEFAULT_READY_TIME				5000		// ﾁﾘｺ・ｽﾃｰ｣


// inline func ////////////////////////////////////////////////////////////////////////
inline int ZMatch::GetCurrRound() 
{ 
	return m_nCurrRound; 
}
inline bool ZMatch::IsTeamPlay() 
{ 
	return m_pStageSetting->IsTeamPlay(); 
}
inline bool ZMatch::IsSoloPlay()
{
	return m_pStageSetting->IsSoloPlay();
}
inline bool ZMatch::IsWaitForRoundEnd() 
{ 
	return m_pStageSetting->IsWaitforRoundEnd(); 
}

inline MMATCH_ROUNDSTATE ZMatch::GetRoundState()
{ 
	return m_nRoundState; 
}
inline MMATCH_GAMETYPE ZMatch::GetMatchType()
{ 
	return m_pStageSetting->GetStageSetting()->nGameType; 
}
inline bool ZMatch::GetTeamKillEnabled()
{ 
	return m_pStageSetting->GetStageSetting()->bTeamKillEnabled; 
}
inline bool ZMatch::GetTeamRotation()
{
	return m_pStageSetting->GetStageSetting()->bTeamAlternateSpawns;
}
inline const char* ZMatch::GetMapName()
{
	return m_pStageSetting->GetStageSetting()->szMapName;
}
inline int ZMatch::GetTeamScore(MMatchTeam nTeam)
{ 
	return m_nTeamScore[nTeam]; 
}

inline int ZMatch::GetTeamKills(MMatchTeam nTeam)
{ 
	return m_nTeamKillCount[nTeam]; 
}

inline void ZMatch::AddTeamKills(MMatchTeam nTeam, int amount)
{ 
	m_nTeamKillCount[nTeam]+=amount; 
}

inline void ZMatch::SetTeamKills(MMatchTeam nTeam, int amount)
{ 
	m_nTeamKillCount[nTeam]=amount; 
}

inline void ZMatch::SetTeamScore(MMatchTeam nTeam, int nScore)
{ 
	m_nTeamScore[nTeam] = nScore; 
}
inline int ZMatch::GetRoundKills()
{ 
	return m_nRoundKills; 
}
inline void ZMatch::AddRoundKills()
{ 
	m_nRoundKills++; 
}
inline bool ZMatch::IsRuleGladiator()
{
	return ((GetMatchType() == MMATCH_GAMETYPE_GLADIATOR_SOLO) || 
			(GetMatchType() == MMATCH_GAMETYPE_GLADIATOR_TEAM));
}
inline ZRule* ZMatch::GetRule() 
{ 
	return m_pRule; 
}

inline bool ZMatch::IsQuestDrived()
{
	return m_pStageSetting->IsQuestDrived();
}

inline bool ZMatch::IsRuleSpy()
{
	return (GetMatchType() == MMATCH_GAMETYPE_SPY);
}
inline bool ZMatch::IsQuestChallengue()
{
	return (GetMatchType() == MMATCH_GAMETYPE_QUEST_CHALLENGE);
}
inline bool ZMatch::IsBlitz()
{
	return (GetMatchType() == MMATCH_GAMETYPE_BLITZKRIEG);
}
inline bool ZMatch::IsMapSkill()
{
	return (GetMatchType() == MMATCH_GAMETYPE_SKILLMAP);
}
inline bool ZMatch::IsClassiMode()
{
	return ((GetMatchType() == MMATCH_GAMETYPE_TRAINING) ||
		(GetMatchType() == MMATCH_GAMETYPE_TEAM_TRAINING));
}
inline bool ZMatch::IsTurboMode()
{
	return (GetMatchType() == MMATCH_GAMETYPE_TURBO);
}
#endif