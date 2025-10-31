#include "stdafx.h"
#include "MMatchStageSetting.h"
#include "MMatchServer.h"
#include "MMatchObject.h"
#include "MMatchSpyMap.h"

MMatchStageSetting::MMatchStageSetting()
{
	Clear();
}


MMatchStageSetting::~MMatchStageSetting()
{
	Clear();
}

unsigned long MMatchStageSetting::GetChecksum()
{
	return (m_StageSetting.Ref().nMapIndex + m_StageSetting.Ref().nGameType + m_StageSetting.Ref().nMaxPlayers);
}

void MMatchStageSetting::SetDefault()
{
	m_StageSetting.Ref().nMapIndex = 0;
	strcpy(m_StageSetting.Ref().szMapName, MMATCH_DEFAULT_STAGESETTING_MAPNAME);
	m_StageSetting.Ref().nGameType = MMATCH_DEFAULT_STAGESETTING_GAMETYPE;
	m_StageSetting.Ref().bTeamKillEnabled = MMATCH_DEFAULT_STAGESETTING_TEAMKILL;
	m_StageSetting.Ref().bTeamWinThePoint = MMATCH_DEFAULT_STAGESETTING_TEAM_WINTHEPOINT;
	m_StageSetting.Ref().bForcedEntryEnabled = MMATCH_DEFAULT_STAGESETTING_FORCEDENTRY;
	m_StageSetting.Ref().nLimitTime = MMATCH_DEFAULT_STAGESETTING_LIMITTIME;
	m_StageSetting.Ref().nMaxPlayers = MMATCH_DEFAULT_STAGESETTING_MAXPLAYERS;
	m_StageSetting.Ref().nRoundMax = MMATCH_DEFAULT_STAGESETTING_ROUNDMAX;
	m_StageSetting.Ref().nLimitLevel = MMATCH_DEFAULT_STAGESETTING_LIMITLEVEL;
	m_StageSetting.Ref().bAutoTeamBalancing = MMATCH_DEFAULT_STAGESETTING_AUTOTEAMBALANCING;
	m_StageSetting.Ref().uidStage = MUID(0,0);
	m_StageSetting.Ref().bIsRelayMap = false;
	m_StageSetting.Ref().bIsStartRelayMap = false;
	m_StageSetting.Ref().nRelayMapListCount = 0;
#ifdef _BOXLEAD
	m_StageSetting.Ref().bIsLeading = false;
#endif
	m_StageSetting.Ref().nPingLimitHigh = 0;
	m_StageSetting.Ref().nPingLimitLow = 0;
	m_StageSetting.Ref().nPingStrikes = 0;
	m_StageSetting.Ref().nPingType = true;
	for (int i=0; i<MAX_RELAYMAP_LIST_COUNT; ++i)
		m_StageSetting.Ref().MapList[i].nMapID = -1;
	m_StageSetting.Ref().nRelayMapType = RELAY_MAP_TURN;
	m_StageSetting.Ref().nRelayMapRepeatCount = RELAY_MAP_3REPEAT;
	strcpy(m_StageSetting.Ref().szStageBestTime, "");
#ifdef _VOTESETTING
	m_StageSetting.Ref().bVoteEnabled = true;
	m_StageSetting.Ref().bObserverEnabled = false;
#endif

	m_StageSetting.MakeCrc();
}

void MMatchStageSetting::SetMapName(char* pszName)
{ 
	if(pszName == NULL) return;
	m_StageSetting.CheckCrc();
	if( strlen(pszName) < MAPNAME_LENGTH )
	{
		memset( m_StageSetting.Ref().szMapName, 0, MAPNAME_LENGTH );

		strcpy(m_StageSetting.Ref().szMapName, pszName); // -- by SungE 2007-04-02

		// MapIndex까지 함께 세팅해준다.
		m_StageSetting.Ref().nMapIndex = 0;
		for (int i = 0; i < MMATCH_MAP_MAX; i++)
		{
			if (!stricmp( MGetMapDescMgr()->GetMapName(i), pszName))
			{
				m_StageSetting.Ref().nMapIndex = MGetMapDescMgr()->GetMapID(i);
				break;
			}
		}
	}
	m_StageSetting.MakeCrc();
}

void MMatchStageSetting::SetStageBestTime(const char* szBestTime)
{
	if (szBestTime == NULL) return;
	m_StageSetting.CheckCrc();
	if (strlen(szBestTime) < 3)
	{
		memset(m_StageSetting.Ref().szStageBestTime, 0, strlen(szBestTime));

		strcpy(m_StageSetting.Ref().szStageBestTime, szBestTime); // -- by SungE 2007-04-02
	}
	m_StageSetting.MakeCrc();
}

void MMatchStageSetting::SetMapIndex(int nMapIndex)
{
	m_StageSetting.CheckCrc();
	m_StageSetting.Ref().nMapIndex = nMapIndex; 

	// MapName까지 함께 세팅해준다.
	if ( MGetMapDescMgr()->MIsCorrectMap(nMapIndex))
	{
		strcpy(m_StageSetting.Ref().szMapName, MGetMapDescMgr()->GetMapName(nMapIndex)); 
	}
	m_StageSetting.MakeCrc();
}

void MMatchStageSetting::Clear()
{
	SetDefault();
	m_CharSettingList.DeleteAll();
	m_uidMaster = MUID(0,0);
	m_nStageState = STAGE_STATE_STANDBY;
	m_bIsCheckTicket = false;
	m_RoomMods.clear(); // Custom: Clear room modifiers
	m_vecSpyBanMapList.reserve(MMATCH_SPY_MAP_MAX - 1);
}

MSTAGE_CHAR_SETTING_NODE* MMatchStageSetting::FindCharSetting(const MUID& uid)
{
	for (MStageCharSettingList::iterator i=m_CharSettingList.begin();i!=m_CharSettingList.end();i++) {
		if (uid == (*i)->uidChar) return (*i);
	}
	return NULL;
}

bool MMatchStageSetting::IsTeamPlay()
{
	return MGetGameTypeMgr()->IsTeamGame(m_StageSetting.Ref().nGameType);
}
bool MMatchStageSetting::IsSoloPlay()
{
	return MGetGameTypeMgr()->IsSoloGame(m_StageSetting.Ref().nGameType);
}
bool MMatchStageSetting::IsWaitforRoundEnd()
{
	return MGetGameTypeMgr()->IsWaitForRoundEnd(m_StageSetting.Ref().nGameType);
}

bool MMatchStageSetting::IsQuestDrived()
{
	return MGetGameTypeMgr()->IsQuestDerived(m_StageSetting.Ref().nGameType);
}

void MMatchStageSetting::UpdateStageSetting(MSTAGE_SETTING_NODE* pSetting)
{
	// move to MMatchServer::OnStageSetting(...) - by SungE 2007-05-14
	//if( STAGE_BASIC_MAX_PLAYERCOUNT < pSetting->nMaxPlayers )
	//{
	//	if( QuestTestServer() && MGetGameTypeMgr()->IsQuestDerived(pSetting->nGameType) )
	//	{
	//		pSetting->nMaxPlayers = STAGE_QUEST_MAX_PLAYER;
	//	}
	//	else
	//	{
	//		pSetting->nMaxPlayers = STAGE_BASIC_MAX_PLAYERCOUNT;
	//	}
	//}

	m_StageSetting.Set_CheckCrc(*pSetting);
	/*m_StageSetting.CheckCrc();
	memcpy(&m_StageSetting.Ref(), pSetting, sizeof(MSTAGE_SETTING_NODE));
	m_StageSetting.MakeCrc();*/
}


void MMatchStageSetting::UpdateCharSetting(const MUID& uid, unsigned int nTeam, MMatchObjectStageState nStageState)
{
	MSTAGE_CHAR_SETTING_NODE* pNode = FindCharSetting(uid);
	if (pNode) {
		pNode->nTeam = nTeam;
		pNode->nState = nStageState;
	} else {
		MSTAGE_CHAR_SETTING_NODE* pNew = new MSTAGE_CHAR_SETTING_NODE;
		pNew->uidChar = uid;
		pNew->nTeam = nTeam;
		pNew->nState = nStageState;
		m_CharSettingList.push_back(pNew);
	}			
}



const MMatchGameTypeInfo* MMatchStageSetting::GetCurrGameTypeInfo()
{ 
	return MGetGameTypeMgr()->GetInfo(m_StageSetting.Ref().nGameType); 
}

void MMatchStageSetting::SetRelayMapList(RelayMap* pValue)
{ 
	m_StageSetting.CheckCrc();
	memcpy(m_StageSetting.Ref().MapList, pValue, sizeof(m_StageSetting.Ref().MapList));
	m_StageSetting.MakeCrc();
}

// Custom: Room Modifiers
void MMatchStageSetting::InsertRoomModifier( ROOM_MOD mod, int nValue )
{
	MMatchRoomModifier roomMod;
	roomMod.mod = mod;
	roomMod.nValue = nValue;

	m_RoomMods.push_back( roomMod );
}

bool MMatchStageSetting::IsUserExempt(MMatchUserGradeID uGrade)
{
	//TODO: Request a packet from the server confirming if the channel is event or not, I don't want to use string confirmation.
	//Since server stuff is in your hands I'll let you handle it unless you want me to. 
	switch(uGrade)
		{
			case 251:
			case 252:
			case 254:
			case 255:
			case 32:
				return true;
			default:
				return false;
		}
	//If a better idea occurs such as a #macro feel free to replace, I decided this would work for now. 
}

bool MMatchStageSetting::IsModifierUsed( ROOM_MOD nRoomMod )
{
	for (int i = 0; i < m_RoomMods.size(); ++i)
	{
		if (m_RoomMods[i].mod == nRoomMod)
			return true;
	}
	return false;
}

int MMatchStageSetting::GetModifierValue( ROOM_MOD nRoomMod )
{
	for( int i = 0; i < m_RoomMods.size(); ++i )
	{
		if( m_RoomMods[i].mod == nRoomMod )
			return m_RoomMods[i].nValue;
	}

	return -1;
}

bool MMatchStageSetting::IsModifierPercentage( ROOM_MOD nRoomMod )
{
	switch( nRoomMod )
	{
	case MMOD_DMG:
		return false;
	}
	
	return true;
}

const char*	MMatchStageSetting::GetModifierString( ROOM_MOD nRoomMod )
{
	switch( nRoomMod )
	{
		case MMOD_LEAD:
			return "Leading";
		case MMOD_VAMPIRE:
			return "Vampire";
		case MMOD_FASTRELOAD:
			return "Fast Reload";
		case MMOD_EXPLOSION:
			return "Ignore Explosions / Flashbang / Smoke Grenade";
		case MMOD_GRAVITY:
			return "Gravity";
		case MMOD_TELEPORT:
			return "Teleport";
		case MMOD_SPEED:
			return "Speed";
		case MMOD_JUMP:
			return "Jump";
		case MMOD_FLIP:
			return "No Flips";
		case MMOD_FPS:
			return "FPS Mode";
		case MMOD_MASSIVES:
			return "No Massives";
		case MMOD_NINJA:
			return "Ninja";
		case MMOD_VANILLA:
			return "Vanilla Mode";
		case MMOD_EVENTMODE:
			return "No AP Mode";
		case MMOD_SNIPERONLY:
			return "Snipers Only";
		case MMOD_MELEEONLY:
			return "Melee Only";
		case MMOD_SHOTGUNONLY:
			return "Shotguns Only";
		case MMOD_REVONLY:
			return "Revolvers Only";
		case MMOD_INFINITE_AMMO:
			return "Infinite Ammo";
		case MMOD_DMG:
			return "Damage Modifier";
		case MMOD_ESP:
			return "ESP";
		case MMOD_NC:
			return "No Clothes";
		case MMOD_NR:
			return "No Rings";
		case MMOD_NS:
			return "No Sword";
		case MMOD_NG:
			return "No Guns";
		case MMOD_NI:
			return "No Items";
		case MMOD_ROLLTHEDICE:
			return "Roll The Dice";
		case MMOD_GLAD:
			return "No Flips, No Guns, No Items";
		case MMOD_WIWEAPON:
			return "WorldItem weapon spawns";
		case MMOD_PAINT:
			return "Paint Mode";
		default:
			break;
	}

	return "Unknown";
}
#ifdef _SPYMODE
void MMatchStageSetting::ExcludeSpyMap(int nMapID)
{
	vector<int>::iterator itBegin = m_vecSpyBanMapList.begin(), itEnd = m_vecSpyBanMapList.end();
	vector<int>::iterator itFound = std::find(itBegin, itEnd, nMapID);

	if (itFound != itEnd) return;
	m_vecSpyBanMapList.push_back(nMapID);
}

void MMatchStageSetting::IncludeSpyMap(int nMapID)
{
	vector<int>::iterator itBegin = m_vecSpyBanMapList.begin(), itEnd = m_vecSpyBanMapList.end();
	vector<int>::iterator itFound = std::find(itBegin, itEnd, nMapID);

	if (itFound == itEnd) return;
	m_vecSpyBanMapList.erase(itFound);
}

bool MMatchStageSetting::IsExcludedSpyMap(int nMapID)
{
	vector<int>::iterator itBegin = m_vecSpyBanMapList.begin(), itEnd = m_vecSpyBanMapList.end();
	vector<int>::iterator itFound = std::find(itBegin, itEnd, nMapID);

	return itFound != itEnd;
}

bool MMatchStageSetting::IsIncludedSpyMap(int nMapID)
{
	return !IsExcludedSpyMap(nMapID);
}

void MMatchStageSetting::UnbanAllSpyMap()
{
	m_vecSpyBanMapList.clear();

}
#endif