#ifndef _MMATCHMAP_H
#define _MMATCHMAP_H

#include "MMatchGlobal.h"

enum MMATCH_MAP
{
	MMATCH_MAP_MANSION = 0,
	MMATCH_MAP_PRISON = 1,
	MMATCH_MAP_STATION = 2,
	MMATCH_MAP_PRISON_II = 3,
	MMATCH_MAP_BATTLE_ARENA = 4,
	MMATCH_MAP_TOWN = 5,
	MMATCH_MAP_DUNGEON = 6,
	MMATCH_MAP_DUNGEON_II = 7,
	MMATCH_MAP_RUIN = 8,
	MMATCH_MAP_ISLAND = 9,
	MMATCH_MAP_GARDEN = 10,
	MMATCH_MAP_CASTLE = 11,
	MMATCH_MAP_FACTORY = 12,
	MMATCH_MAP_PORT = 13,
	MMATCH_MAP_LOST_SHRINE = 14,
	MMATCH_MAP_STAIRWAY = 15,
	MMATCH_MAP_SNOWTOWN = 16,
	MMATCH_MAP_HALL = 17,
	MMATCH_MAP_CATACOMB = 18,
	MMATCH_MAP_JAIL = 19,
	MMATCH_MAP_SHOWERROOM = 20,
	MMATCH_MAP_HIGH_HAVEN = 21,
	MMATCH_MAP_CITADEL = 22,
	MMATCH_MAP_RELAYMAP = 23,
	MMATCH_MAP_HALLOWEEN_TOWN = 24,
	MMATCH_MAP_WEAPON_SHOP = 25,
	MMATCH_MAP_BLITZKRIEG = 26,

	MMATCH_MAP_SKILLMAP = 27,
	MMATCH_MAP_SKILLMAPV2 = 28,
	MMATCH_MAP_SKILLMAPV3 = 29,
	MMATCH_MAP_SKILLMAPV4 = 30,
	MMATCH_MAP_SUPERFLIP  = 31,

	//ChallengeQuest Maps (look banner map)
	MMATCH_MAP_CQ_1_GUERRILLA = 32,
	MMATCH_MAP_CQ_2_GUERRILLA = 33,
	MMATCH_MAP_CQ_3_GUERRILLA = 34,
	MMATCH_MAP_CQ_4_GUERRILLA = 35,

	MMATCH_MAP_CQ_5_RESEARCH = 36,
	MMATCH_MAP_CQ_6_RESEARCH = 37,
	MMATCH_MAP_CQ_7_RESEARCH = 38,
	MMATCH_MAP_CQ_8_RESEARCH = 39,

#ifdef _LIMITMAP
	MMATCH_MAP_MAX
#else
	MMATCH_MAP_MAX = 150
#endif
};

#define MMATCH_MAP_COUNT	MMATCH_MAP_MAX	

#define MMATCH_MAPNAME_RELAYMAP				"RelayMap"


class MMapDesc
{
private:
	const struct MapInfo
	{
		int			nMapID;							// map id
		char		szMapName[MAPNAME_LENGTH];		// ¸Ê ÀÌ¸§
		char		szMapImageName[MAPNAME_LENGTH];	// ¸Ê ÀÌ¹ÌÁö ÀÌ¸§
		char		szBannerName[MAPNAME_LENGTH];	// º£³Ê ÀÌ¸§
		float		fExpRatio;						// °æÇèÄ¡ ½Àµæ·ü
		int			nMaxPlayers;					// ÃÖ´ë ÀÎ¿ø
		bool		bOnlyDuelMap;					// µà¾ó¸Ê Àü¿ë
		bool		bIsCTFMap;

#ifdef _CUSTOMRULEMAPS
		bool		bBlitzMap;
#endif

	};


	// data
	MapInfo	m_MapVectors[MMATCH_MAP_COUNT];
	MMapDesc();
public:
	~MMapDesc() { }
	
	static MMapDesc* GetInstance();

	bool Initialize(const char* szFileName);
	bool Initialize(MZFileSystem* pfs, const char* szFileName);
	bool MIsCorrectMap(const int nMapID);
	

 
	bool IsMapOnlyDuel( const int nMapID);
	bool IsCTFMap( const int nMapID);

#ifdef _CUSTOMRULEMAPS
	bool IsSWRMap(const int nMapID);
#endif

	int GetMapID( const int nMapID);
	int GetMapID(const char* mapName);
	const char* GetMapName(const int nMapID);
	const char* GetMapImageName(const char* szMapName);
	const char* GetBannerName(const char* szMapName);
	float GetExpRatio( const int nMapID); 
	int GetMaxPlayers( const int nMapID);

	int GetMapCount(){ return MMATCH_MAP_COUNT; }
};


inline MMapDesc* MGetMapDescMgr() 
{ 
	return MMapDesc::GetInstance();
}



#endif