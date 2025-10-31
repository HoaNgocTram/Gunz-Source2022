#ifndef _MBASEGAMETYPE_H
#define _MBASEGAMETYPE_H

#include <crtdbg.h>
#include <map>
#include <set>
using namespace std;


/// ∞‘¿” ≈∏¿‘
enum MMATCH_GAMETYPE {
	MMATCH_GAMETYPE_DEATHMATCH_SOLO		=0,			///< ∞≥¿Œ µ•æ≤∏≈ƒ°
	MMATCH_GAMETYPE_DEATHMATCH_TEAM		=1,			///< ∆¿ µ•æ≤∏≈ƒ°
	MMATCH_GAMETYPE_GLADIATOR_SOLO		=2,			///< ∞≥¿Œ ±€∑°µø°¿Ã≈Õ
	MMATCH_GAMETYPE_GLADIATOR_TEAM		=3,			///< ∆¿ ±€∑°µø°¿Ã≈Õ
	MMATCH_GAMETYPE_ASSASSINATE			=4,			///< ∫∏Ω∫¿ÅEæœªÅEÅE
	MMATCH_GAMETYPE_TRAINING			=5,			///< ø¨Ω¿

	MMATCH_GAMETYPE_SURVIVAL			=6,			///< º≠πŸ¿ÃπÅE
	MMATCH_GAMETYPE_QUEST				=7,			///< ƒ˘Ω∫∆Æ

	MMATCH_GAMETYPE_BERSERKER			=8,			///< µ•æ≤∏≈ƒ° πˆº≠ƒø
	MMATCH_GAMETYPE_DEATHMATCH_TEAM2	=9,			///< ∆¿µ•æ≤∏≈ƒ° ¿ÕΩ∫∆Æ∏≤
	MMATCH_GAMETYPE_DUEL				=10,		///< µ‡æÅE∏≈ƒ°
	MMATCH_GAMETYPE_DUELTOURNAMENT		=11,		///< µ‡æÅE≈‰≥ ∏’∆Æ
	MMATCH_GAMETYPE_QUEST_CHALLENGE		=12,		///< √ß∏∞¡ˆƒ˘Ω∫∆Æ

	// Custom: Game modes
	MMATCH_GAMETYPE_TEAM_TRAINING		=13,
	MMATCH_GAMETYPE_CTF					=14,
	MMATCH_GAMETYPE_INFECTED			=15,
	MMATCH_GAMETYPE_GUNGAME				=16,
	MMATCH_GAMETYPE_SPY				    =17,
	MMATCH_GAMETYPE_TURBO               =18,		///< Turbo
	MMATCH_GAMETYPE_BLITZKRIEG          =19,		///< Blitz
	MMATCH_GAMETYPE_SKILLMAP            =20,        /// Skill map
/*
#ifndef _CLASSIC
	MMATCH_GAMETYPE_CLASSIC_SOLO,
	MMATCH_GAMETYPE_CLASSIC_TEAM,
#endif
*/
	MMATCH_GAMETYPE_MAX,
};
// ∏µÂ∞° √ﬂ∞°µ«∏ÅE∏Æ«√∑π¿Ã ¿˙¿ÂøÅE∏µÅEæ‡æ˚—µ √ﬂ∞°«ÿæﬂ «—¥Ÿ (cpp)
extern const char* MMatchGameTypeAcronym[MMATCH_GAMETYPE_MAX];

#define		MAX_RELAYMAP_LIST_COUNT			20
struct RelayMap
{
	int				nMapID;
};

enum RELAY_MAP_TYPE
{
	RELAY_MAP_TURN			= 0,
	RELAY_MAP_RANDOM,

	RELAY_MAP_MAX_TYPE_COUNT
};

enum RELAY_MAP_REPEAT_COUNT
{
	RELAY_MAP_1REPEAT			= 0,
	RELAY_MAP_2REPEAT,
	RELAY_MAP_3REPEAT,
	RELAY_MAP_4REPEAT,
	RELAY_MAP_5REPEAT,

	RELAYMAP_MAX_REPEAT_COUNT,
};

// µ∆˙∆Æ ∞‘¿” ≈∏¿‘
const MMATCH_GAMETYPE MMATCH_GAMETYPE_DEFAULT = MMATCH_GAMETYPE_DEATHMATCH_SOLO;
// const MMATCH_GAMETYPE MMATCH_GAMETYPE_DEFAULT = MMATCH_GAMETYPE_DEATHMATCH_TEAM2;


struct MMatchGameTypeInfo
{
	MMATCH_GAMETYPE		nGameTypeID;			// ID
	char				szGameTypeStr[24];		// ∞‘¿” ≈∏¿‘ ¿Ã∏ß
	float				fGameExpRatio;			// ∞Ê«Ëƒ° πË∫– ∫Ò¿≤ - «ˆ¿ÅEGladiator∏∏ 50%¿Ã¥Ÿ
	float				fTeamMyExpRatio;		// ∆¿¿ÅE°º≠ ∞≥¿Œø°∞‘ ¿˚øÅE«¥¬ ∞Ê«Ëƒ°
	float				fTeamBonusExpRatio;		// ∆¿¿ÅE°º≠ ∆¿ø°∞‘ ¿˚∏≥µ«¥¬ ∞Ê«Ëƒ°
	set<int>			MapSet;					// ¿Ã ∞‘¿”≈∏¿‘ø°º≠ «√∑π¿Ã ∞°¥…«— ∏ 
	void Set(const MMATCH_GAMETYPE a_nGameTypeID, const char* a_szGameTypeStr, const float a_fGameExpRatio,
		     const float a_fTeamMyExpRatio, const float a_fTeamBonusExpRatio);
	void AddMap(int nMapID);
	void AddAllMap();
};


class MBaseGameTypeCatalogue
{
private:
	MMatchGameTypeInfo			m_GameTypeInfo[MMATCH_GAMETYPE_MAX];
public:
	MBaseGameTypeCatalogue();
	virtual ~MBaseGameTypeCatalogue();

	inline MMatchGameTypeInfo* GetInfo(MMATCH_GAMETYPE nGameType);
	inline const char* GetGameTypeStr(MMATCH_GAMETYPE nGameType);
	inline void SetGameTypeStr(MMATCH_GAMETYPE nGameType, const char* szName);
	inline bool IsCorrectGameType(const int nGameTypeID);	///< ø√πŸ∏• ∞‘¿” ≈∏¿‘ ID¿Œ¡ÅEø©∫Œ
	inline bool IsTeamGame(MMATCH_GAMETYPE nGameType);		///< «ÿ¥ÅE∞‘¿”≈∏¿‘¿Ã ∆¿¿ÅEŒ¡ÅEø©∫Œ
	inline bool IsSoloGame(MMATCH_GAMETYPE nGameType);
	inline bool IsTeamExtremeGame(MMATCH_GAMETYPE nGameType); // Custom: IsTeamExtremeGame
	inline bool IsTeamLimitTime(MMATCH_GAMETYPE nGameType);
	inline bool IsSpyGame(MMATCH_GAMETYPE nGameType);
	inline bool IsMapSkill(MMATCH_GAMETYPE nGameType); //Skill Mas
	inline bool IsWaitForRoundEnd(MMATCH_GAMETYPE nGameType);		///< ∂Ûø˚—ÅE≥°≥Ø∂ß±˚›ÅE¥ÅE‚∏µÅE«ÿæﬂ«œ¥¬¡ÅEø©∫Œ
	inline bool IsQuestOnly(MMATCH_GAMETYPE nGameType);	///< «ÿ¥ÅE∞‘¿”≈∏¿‘¿Ã ƒ˘Ω∫∆Æ¿Œ¡ÅEø©∫Œ
	inline bool IsSurvivalOnly(MMATCH_GAMETYPE nGameType); ///< «ÿ¥ÅE∞‘¿”≈∏¿‘¿Ã º≠πŸ¿Ãπ˙¿Œ¡ÅEø©∫Œ
	inline bool IsQuestChallengeOnly(MMATCH_GAMETYPE nGameType); // Custom: CQ fixes
	inline bool IsBlitzQuest(MMATCH_GAMETYPE nGameType); 
	inline bool IsQuestDerived(MMATCH_GAMETYPE nGameType);	///< «ÿ¥ÅE∞‘¿”≈∏¿‘¿Ã ƒ˘Ω∫∆Æ ∞ÅE√(NPC∞° ≥™ø¿¥¬¡ÅE¿Œ¡ÅEø©∫Œ
	inline bool IsWorldItemSpawnEnable(MMATCH_GAMETYPE nGameType);	///< «ÿ¥ÅE∞‘¿”≈∏¿‘ø°º≠ ø˘µÂæ∆¿Ã≈€¿Ã Ω∫∆˘µ«¥¬¡ÅEø©∫Œ
};


//////////////////////////////////////////////////////////////////////////////////
inline bool MBaseGameTypeCatalogue::IsTeamGame(MMATCH_GAMETYPE nGameType)
{
	// ¡ª¥ÅE∫π¿‚«ÿ¡ˆ∏ÅEDescriptionø° teamgame¿Œ¡ÅEø©∫Œ∞™¿ª ≥÷µµ∑œ «œ¿⁄.

	if ((nGameType == MMATCH_GAMETYPE_DEATHMATCH_TEAM) ||
		(nGameType == MMATCH_GAMETYPE_DEATHMATCH_TEAM2) ||
		(nGameType == MMATCH_GAMETYPE_GLADIATOR_TEAM) ||
		(nGameType == MMATCH_GAMETYPE_ASSASSINATE) ||
		(nGameType == MMATCH_GAMETYPE_TEAM_TRAINING) ||
		(nGameType == MMATCH_GAMETYPE_CTF) ||
		(nGameType == MMATCH_GAMETYPE_INFECTED)||
		(nGameType == MMATCH_GAMETYPE_BLITZKRIEG) ||
		(nGameType == MMATCH_GAMETYPE_SPY))
	{
		return true;
	}
	return false;
}
inline bool MBaseGameTypeCatalogue::IsSoloGame(MMATCH_GAMETYPE nGameType)
{
	// ¡ª¥ÅE∫π¿‚«ÿ¡ˆ∏ÅEDescriptionø° teamgame¿Œ¡ÅEø©∫Œ∞™¿ª ≥÷µµ∑œ «œ¿⁄.

	if ((nGameType == MMATCH_GAMETYPE_DEATHMATCH_SOLO) ||
		(nGameType == MMATCH_GAMETYPE_GLADIATOR_SOLO) ||
		(nGameType == MMATCH_GAMETYPE_TRAINING) ||
		(nGameType == MMATCH_GAMETYPE_DUEL) ||
		(nGameType == MMATCH_GAMETYPE_SKILLMAP) ||
		(nGameType == MMATCH_GAMETYPE_GUNGAME) ||
		(nGameType == MMATCH_GAMETYPE_TURBO))
	{
		return true;
	}
	return false;
}
inline bool MBaseGameTypeCatalogue::IsTeamLimitTime(MMATCH_GAMETYPE nGameType)
{
	// ¡ª¥ÅE∫π¿‚«ÿ¡ˆ∏ÅEDescriptionø° teamgame¿Œ¡ÅEø©∫Œ∞™¿ª ≥÷µµ∑œ «œ¿⁄.

	if ((nGameType == MMATCH_GAMETYPE_DEATHMATCH_TEAM) ||
		(nGameType == MMATCH_GAMETYPE_GLADIATOR_TEAM) ||
		(nGameType == MMATCH_GAMETYPE_DUEL) ||
		(nGameType == MMATCH_GAMETYPE_ASSASSINATE) ||
		(nGameType == MMATCH_GAMETYPE_TEAM_TRAINING) ||
		(nGameType == MMATCH_GAMETYPE_INFECTED)||
		(nGameType == MMATCH_GAMETYPE_SPY))
	{
		return true;
	}
	return false;
}

inline bool MBaseGameTypeCatalogue::IsWaitForRoundEnd(MMATCH_GAMETYPE nGameType)
{
	// Run until standby mode??

	if ((nGameType == MMATCH_GAMETYPE_DEATHMATCH_TEAM) ||
		(nGameType == MMATCH_GAMETYPE_DUEL) ||
		(nGameType == MMATCH_GAMETYPE_GLADIATOR_TEAM) ||
		(nGameType == MMATCH_GAMETYPE_ASSASSINATE) ||
		(nGameType == MMATCH_GAMETYPE_DUELTOURNAMENT) ||
		(nGameType == MMATCH_GAMETYPE_TEAM_TRAINING) ||
		(nGameType == MMATCH_GAMETYPE_INFECTED)||
		(nGameType == MMATCH_GAMETYPE_SPY))
	{
		return true;
	}
	return false;
}


inline bool MBaseGameTypeCatalogue::IsQuestDerived(MMATCH_GAMETYPE nGameType)
{
	if ( (nGameType == MMATCH_GAMETYPE_SURVIVAL) ||(nGameType == MMATCH_GAMETYPE_QUEST) )
	{
		return true;
	}

	return false;
}

inline bool MBaseGameTypeCatalogue::IsQuestOnly(MMATCH_GAMETYPE nGameType)
{
	return nGameType == MMATCH_GAMETYPE_QUEST;
}

inline bool MBaseGameTypeCatalogue::IsSurvivalOnly(MMATCH_GAMETYPE nGameType)
{
	return nGameType == MMATCH_GAMETYPE_SURVIVAL;
}

inline bool MBaseGameTypeCatalogue::IsQuestChallengeOnly(MMATCH_GAMETYPE nGameType)
{
	if ((nGameType == MMATCH_GAMETYPE_QUEST_CHALLENGE))
		return true;

	return false;
}
inline bool MBaseGameTypeCatalogue::IsBlitzQuest(MMATCH_GAMETYPE nGameType)
{
	if ((nGameType == MMATCH_GAMETYPE_BLITZKRIEG))
		return true;

	return false;
}

inline const char* MBaseGameTypeCatalogue::GetGameTypeStr(MMATCH_GAMETYPE nGameType)
{
	return m_GameTypeInfo[nGameType].szGameTypeStr;
}

inline void MBaseGameTypeCatalogue::SetGameTypeStr(MMATCH_GAMETYPE nGameType, const char* szName)
{
	strcpy( m_GameTypeInfo[nGameType].szGameTypeStr, szName) ;
}

bool MBaseGameTypeCatalogue::IsCorrectGameType(const int nGameTypeID)
{
	if ((nGameTypeID < 0) || (nGameTypeID >= MMATCH_GAMETYPE_MAX)) return false;
	return true;
}

inline MMatchGameTypeInfo* MBaseGameTypeCatalogue::GetInfo(MMATCH_GAMETYPE nGameType)
{
	//_ASSERT((nGameType >= 0) && (nGameType < MMATCH_GAMETYPE_MAX));
	return &m_GameTypeInfo[nGameType];
}

inline bool MBaseGameTypeCatalogue::IsWorldItemSpawnEnable(MMATCH_GAMETYPE nGameType)
{
	if ( (nGameType == MMATCH_GAMETYPE_SURVIVAL) || (nGameType == MMATCH_GAMETYPE_QUEST) || (nGameType == MMATCH_GAMETYPE_QUEST_CHALLENGE))
	{
		return false;
	}

	return true;

}
inline bool MBaseGameTypeCatalogue::IsTeamExtremeGame(MMATCH_GAMETYPE nGameType)
{
	// ¡ª¥ı ∫π¿‚«ÿ¡ˆ∏È Descriptionø° teamgame¿Œ¡ˆ ø©∫Œ∞™¿ª ≥÷µµ∑œ «œ¿⁄.

	if ((nGameType == MMATCH_GAMETYPE_DEATHMATCH_TEAM2) ||
		(nGameType == MMATCH_GAMETYPE_BLITZKRIEG) ||
		(nGameType == MMATCH_GAMETYPE_CTF))
	{
		return true;
	}
	return false;
}
inline bool MBaseGameTypeCatalogue::IsSpyGame(MMATCH_GAMETYPE nGameType)
{
	if (nGameType == MMATCH_GAMETYPE_SPY)
		return true;

	return false;
}
inline bool MBaseGameTypeCatalogue::IsMapSkill(MMATCH_GAMETYPE nGameType)
{
	if (nGameType == MMATCH_GAMETYPE_SKILLMAP)
		return true;

	return false;
}
#endif