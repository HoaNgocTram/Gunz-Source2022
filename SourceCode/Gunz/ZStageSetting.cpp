#include "stdafx.h"

#include "ZStageSetting.h"
#include "ZGameInterface.h"
#include "MComboBox.h"
#include "MFrame.h"
#include "ZApplication.h"
#include "ZGameClient.h"
#include "ZPost.h"
#include "MMatchGameType.h"
#include "ZLanguageConf.h"
#include "ZChannelRule.h"
#include "ZConfiguration.h"
#include "ZGameTypeList.h"
#include "ZMap.h"

// 최대인원
#define STAGESETTING_MAXPLAYER_MAX		5
#define STAGESETTING_MAXPLAYER_DEFAULT	1
static struct _STAGESETTING_MAXPLAYER
{
	int					Value;
	char				szText[16];
} StageSetting_MaxPlayer[STAGESETTING_MAXPLAYER_MAX] = 
	{ {4, "4"}, {8, "8"}, {10, "10"}, {12, "12"}, {16, "16"} };

// 라운드
#define STAGESETTING_MAXROUND_MAX		6
#define STAGESETTING_MAXROUND_DEFAULT	3
static struct _STAGESETTING_MAXROUND
{
	int					Value;
	char				szText[32];
} StageSetting_MaxRound[STAGESETTING_MAXROUND_MAX] = { 
	{10, "10"}, {20, "20"}, {30, "30"}, {50, "50"}, {70, "70"}, {100, "100"} };

// 제한시간
#define STAGESETTING_LIMITTIME_MAX		6
#define STAGESETTING_LIMITTIME_DEFAULT	4
static struct _STAGESETTING_LIMITTIME_SINGLE
{
	int					Value;
	char				szText[32];
} StageSetting_LimitTime_Single[STAGESETTING_LIMITTIME_MAX] = 
{ {99999, "무한"}, {10, "10분"}, {15, "15분"}, {20, "20분"}, {30, "30분"}, {60, "60분"} };
static struct _STAGESETTING_LIMITTIME_TEAM
{
	int					Value;
	char				szText[32];
} StageSetting_LimitTime_Team[STAGESETTING_LIMITTIME_MAX] = 
{ {99999, "무한"}, {3, "3분"}, {5, "5분"}, {7, "7분"}, {10, "10분"}, {15, "15분"} };

// 제한레벨
#define STAGESETTING_LIMITLEVEL_MAX		4
#define STAGESETTING_LIMITLEVEL_DEFAULT	0
static struct _STAGESETTING_LIMITLEVEL
{
	int					Value;
	char				szText[32];
} StageSetting_LimitLevel[STAGESETTING_LIMITLEVEL_MAX] = 
{ {0, "없음"}, {5, "레벨차 5"}, {10, "레벨차 10"}, {15, "레벨차 15"} };

// 팀킬여부
#define STAGESETTING_TEAMKILL_MAX		2
#define STAGESETTING_TEAMKILL_DEFAULT	0
static struct _STAGESETTING_TEAM
{
	bool	Value;
	char	szText[32];
} StageSetting_TeamKill[STAGESETTING_TEAMKILL_MAX] = 
{ {true, "허용"}, {false, "금지"} };

// 난입여부
#define STAGESETTING_FORCEDENTRY_MAX		2
#define STAGESETTING_FORCEDENTRY_DEFAULT	0
static struct _STAGESETTING_FORCEDENTRY
{
	bool	Value;
	char	szText[32];
} StageSetting_ForcedEntry[STAGESETTING_FORCEDENTRY_MAX] = 
{ {true, "허용"}, {false, "금지"} };

// 관전 허용
#define STAGESETTING_OBSERVER_MAX		2
#define STAGESETTING_OBSERVER_DEFAULT	1
static struct _STAGESETTING_OBSERVER
{
	bool	Value;
	char	szText[32];
} StageSetting_Observer[STAGESETTING_OBSERVER_MAX] = 
{ {true, "허용"}, {false, "금지"} };

// 투표 가능
#define STAGESETTING_VOTE_MAX		2
#define STAGESETTING_VOTE_DEFAULT	0
static struct _STAGESETTING_VOTE
{
	bool	Value;
	char	szText[32];
} StageSetting_Vote[STAGESETTING_VOTE_MAX] = 
{ {true, "허용"}, {false, "금지"} };

// 팀 밸런스
#define STAGESETTING_TEAMBALANCING_MAX		2
#define STAGESETTING_TEAMBALANCING_DEFAULT	0
static struct _STAGESETTING_TEAMBALANCING
{
	bool	Value;
	char	szText[32];
} StageSetting_TeamBalancing[STAGESETTING_TEAMBALANCING_MAX] = 
{ {true, "ON"}, {false, "OFF"} };

#define STAGESETTING_PING_STRIKE_MAX	12
#define STAGESETTING_PING_STRIKE_DEFAULT 0
static struct _STAGESETTING_PING_STRIKE
{
	int					Value;
	char				szText[32];
} StageSetting_Ping_Strike[STAGESETTING_PING_STRIKE_MAX] = {
	{ 1, "1" }, { 2, "2" }, { 3, "3" }, { 4, "4" }, { 5, "5" }, { 6, "6" }, { 7, "7" }, { 8, "8" }, { 9, "9" }, { 10, "10" }, { 11, "11" }, { 12, "12" } };

#define STAGESETTING_PING_TYPE_MAX 2
#define STAGESETTING_PING_TYPE_DEFAULT 0
static struct _STAGESETTING_PING_TYPE
{
	bool					Value;
	char				szText[32];
} StageSetting_Ping_Type[STAGESETTING_PING_TYPE_MAX] = {
	{ true, "Server Ping" }, { false, "Peer Ping" } };

#ifdef _ROTATION
#define STAGESETTING_TEAMROTATION_MAX		2
#define STAGESETTING_TEAMROTATION_DEFAULT	0
static struct _STAGESETTING_TEAMROTATION
{
	bool	Value;
	char	szText[32];
} StageSetting_TeamRotation[STAGESETTING_TEAMROTATION_MAX] =
{ {true, "ON"}, {false, "OFF"} };
#endif
static bool BUILD_STAGESETTING_LISTITEM(ZIDLResource* pResource, const char* _WidgetItemName, int& _NodeVariable, MGAMETYPECFGDATA& _ItemList)	
{																				
	MComboBox* pCB = (MComboBox*)pResource->FindWidget( _WidgetItemName);		
	if ( pCB && (pCB->GetSelIndex() >= 0))										
	{																			
		int nItemCount = 0;														
		MGAMETYPECFGDATA::iterator itr = _ItemList.begin();						
		for ( int i = 0;  i < pCB->GetSelIndex();  i++)							
		{																		
			if ( itr == _ItemList.end())										
				return false;
			itr++;																
		}
		if (itr == _ItemList.end())
		{
			_ASSERT(0);
			return false;
		}
		_NodeVariable = (*itr)->m_nValue;										
	}	
	return true;
}


static bool BuildStageSetting(MSTAGE_SETTING_NODE* pOutNode)
{
	ZeroMemory(pOutNode, sizeof(MSTAGE_SETTING_NODE));
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	// 게임타입
	{																					
		MComboBox* pCB = (MComboBox*)pResource->FindWidget("StageType");
		if (pCB == NULL) return false;
		bool bExistGameType = false;
		for (int i = 0; i < MMATCH_GAMETYPE_MAX; i++)
		{
			if (!strcmp(pCB->GetText(), ZGetGameTypeManager()->GetGameTypeStr(MMATCH_GAMETYPE(i))))
			{
				bExistGameType = true;
				pOutNode->nGameType = ZGetGameTypeManager()->GetInfo(MMATCH_GAMETYPE(i))->nGameTypeID;
				break;
			}
		}
		if (!bExistGameType) return false;
	}

	// 릴레이맵 세팅
	MComboBox* pCB = (MComboBox*)pResource->FindWidget( "MapSelection");
	if ( pCB)
	{
		strcpy( pOutNode->szMapName, pCB->GetText());

		if (0 == strcmp(MMATCH_MAPNAME_RELAYMAP, pOutNode->szMapName))
			pOutNode->bIsRelayMap = true;
		else
			pOutNode->bIsRelayMap = false;


	}
	// 릴레이맵 타입
	pCB = (MComboBox*)ZGetGameInterface()->GetIDLResource()->FindWidget( "Stage_RelayMapType" );
	if ( pCB)
		pOutNode->nRelayMapType = (RELAY_MAP_TYPE)pCB->GetSelIndex();

	// 릴레이맵 횟수
	pCB = (MComboBox*)ZGetGameInterface()->GetIDLResource()->FindWidget( "Stage_RelayMapRepeatCount" );
	if ( pCB)
		pOutNode->nRelayMapRepeatCount = (RELAY_MAP_REPEAT_COUNT)pCB->GetSelIndex();

#ifdef _BOXLEAD
	MButton* pWidget = (MButton*)ZGetGameInterface()->GetIDLResource()->FindWidget("Lead");
	if (pWidget)
		pOutNode->bIsLeading = !pWidget->GetCheck();
#endif

	// 릴레이맵 리스트
	MListBox* pRelayMapListBox = (MListBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("Stage_RelayMapListbox");
	if(pRelayMapListBox)
	{
		pOutNode->nRelayMapListCount = pRelayMapListBox->GetCount();
		memset(pOutNode->MapList, -1, sizeof(RelayMap)*MAX_RELAYMAP_LIST_COUNT );
		for (int i = 0; i < pRelayMapListBox->GetCount(); i++)
			for (int j = 0; j < MMATCH_MAP_COUNT; j++)
				if(0 == strcmp(pRelayMapListBox->GetString(i), (char*)MGetMapDescMgr()->GetMapName(j)))
				{
					RelayMap relayMap;
					relayMap.nMapID = j;
					pOutNode->MapList[i] = relayMap;
					break;
				}
	}

	/////////////////////////////////////////////////////////////////////////////////
#define BUILD_STAGESETTING_LISTITEM( _WidgetItemName, _NodeVariable, _ItemList)	\
{																				\
	MComboBox* pCB = (MComboBox*)pResource->FindWidget( _WidgetItemName);		\
	if ( pCB && (pCB->GetSelIndex() >= 0))										\
	{																			\
		int nItemCount = 0;														\
		MGAMETYPECFGDATA::iterator itr = _ItemList.begin();						\
		for ( int i = 0;  i < pCB->GetSelIndex();  i++)							\
		{																		\
			if ( itr == _ItemList.end())										\
				return false;													\
			itr++;																\
		}																		\
		_NodeVariable = (*itr)->m_nValue;										\
	}																			\
}
/////////////////////////////////////////////////////////////////////////////////

	ZGameTypeConfig* pGameTypeCfg = ZGetConfiguration()->GetGameTypeList()->GetGameTypeCfg(pOutNode->nGameType);
	if (pGameTypeCfg)
	{
		// 최대 인원
		BUILD_STAGESETTING_LISTITEM("StageMaxPlayer", pOutNode->nMaxPlayers, pGameTypeCfg->m_MaxPlayers);

		// 라운드
		BUILD_STAGESETTING_LISTITEM("StageRoundCount", pOutNode->nRoundMax, pGameTypeCfg->m_Round);

		if (pOutNode->nGameType == MMATCH_GAMETYPE_SPY)
		{
			pOutNode->nLimitTime = -1;//99999;
		}
		else
		{
			// 제한시간
			BUILD_STAGESETTING_LISTITEM("StageLimitTime", pOutNode->nLimitTime, pGameTypeCfg->m_LimitTime);
		}
	}


/////////////////////////////////////////////////////////////////////////////////////////////
#define BUILD_STAGESETTING_ITEM(_WidgetItemName, _NodeVariable, _ItemList, _nItemCount)		\
{																					\
	MComboBox* pCB = (MComboBox*)pResource->FindWidget(_WidgetItemName);			\
	if (pCB == NULL) return false;													\
	if (pCB->GetSelIndex() < _nItemCount)											\
	{																				\
		_NodeVariable = _ItemList[pCB->GetSelIndex()].Value;						\
	}																				\
	else																			\
	{																				\
		return false;																\
	}																				\
}																					\
/////////////////////////////////////////////////////////////////////////////////////////////
	
	// 제한레벨
	BUILD_STAGESETTING_ITEM("StageLevelLimit", pOutNode->nLimitLevel, 
							StageSetting_LimitLevel, STAGESETTING_LIMITLEVEL_MAX);

	// 팀킬여부
	BUILD_STAGESETTING_ITEM("StageTeamKill", pOutNode->bTeamKillEnabled, 
							StageSetting_TeamKill, STAGESETTING_TEAMKILL_MAX);

	// 난입여부
	BUILD_STAGESETTING_ITEM("StageIntrude", pOutNode->bForcedEntryEnabled, 
							StageSetting_ForcedEntry, STAGESETTING_FORCEDENTRY_MAX);

	// 팀 밸런스
	BUILD_STAGESETTING_ITEM("StageTeamBalancing", pOutNode->bAutoTeamBalancing,
		StageSetting_TeamBalancing, STAGESETTING_TEAMBALANCING_MAX);


	BUILD_STAGESETTING_ITEM("PingStrikes", pOutNode->nPingStrikes, StageSetting_Ping_Strike, STAGESETTING_PING_STRIKE_MAX);
	BUILD_STAGESETTING_ITEM("PingType", pOutNode->nPingType, StageSetting_Ping_Type, STAGESETTING_PING_TYPE_MAX);
	MEdit* pEdit = (MEdit*)pResource->FindWidget("PingLimitLow");
	if (pEdit)
		pOutNode->nPingLimitLow = atoi(pEdit->GetText());
	if (pOutNode->nPingLimitLow <= 0) pOutNode->nPingLimitLow = 0;
	pEdit = (MEdit*)pResource->FindWidget("PingLimitHigh");
	if (pEdit)
		pOutNode->nPingLimitHigh = atoi(pEdit->GetText());
#ifdef _ROTATION
	BUILD_STAGESETTING_ITEM("StageTeamRotation", pOutNode->bTeamAlternateSpawns,
		StageSetting_TeamRotation, STAGESETTING_TEAMROTATION_MAX);
#endif

	return true;
}


void ZStageSetting::ShowStageSettingDialog( const MSTAGE_SETTING_NODE* pStageSetting, bool bShowAll)
{
	// bShowAll 이 true면 현재 게임 설정 데이터를 서버로 데이터 전송함

	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	if (pResource == NULL) return;


	// 게임 타입
	{																						
		MComboBox* pCB = (MComboBox*)pResource->FindWidget("StageType");				
		if (pCB)
		{																					
			for (int i = 0; i < MMATCH_GAMETYPE_MAX; i++)
			{																				
				if (pStageSetting->nGameType == ZGetGameTypeManager()->GetInfo(MMATCH_GAMETYPE(i))->nGameTypeID)
				{
					int nGameType = i;

					for (int j = 0; j < pCB->GetCount(); j++)
					{
						if (!stricmp(pCB->GetString(j), ZGetGameTypeManager()->GetInfo(MMATCH_GAMETYPE(i))->szGameTypeStr))
						{
                            pCB->SetSelIndex( j);
							break;
						}
					}
				}																			
			}																				
		}																					
	}
	
/////////////////////////////////////////////////////////////////////////////////
#define SHOWSTAGESETTING_LISTITEM( _WidgetItemName, _NodeVariable, _ItemList, _MaxValue)	\
{																				\
	MComboBox* pCB = (MComboBox*)pResource->FindWidget( _WidgetItemName);		\
	if ( pCB)																	\
	{																			\
		int nSelect = pCB->GetSelIndex(), nItemCount = 0;						\
		pCB->RemoveAll();														\
		MGAMETYPECFGDATA::iterator itr = _ItemList.begin();						\
		for ( ; itr != _ItemList.end();  itr++)									\
		{																		\
			if ( (*itr)->m_nValue <= _MaxValue)									\
			{																	\
				pCB->Add( (*itr)->m_szString);									\
				if ( (*itr)->m_nValue == _NodeVariable)							\
					nSelect = nItemCount;										\
				nItemCount++;													\
			}																	\
		}																		\
		if ( nSelect >= nItemCount)												\
			nSelect = nItemCount - 1;											\
		pCB->SetSelIndex( nSelect);												\
	}																			\
}
/////////////////////////////////////////////////////////////////////////////////

	ZGameTypeConfig* pGameTypeCfg = ZGetConfiguration()->GetGameTypeList()->GetGameTypeCfg( pStageSetting->nGameType);
	if ( pGameTypeCfg)
	{
		// 최대 인원
		SHOWSTAGESETTING_LISTITEM( "StageMaxPlayer", pStageSetting->nMaxPlayers, pGameTypeCfg->m_MaxPlayers, MGetMapDescMgr()->GetMaxPlayers(pStageSetting->nMapIndex) );

		// 라운드
		SHOWSTAGESETTING_LISTITEM( "StageRoundCount", pStageSetting->nRoundMax, pGameTypeCfg->m_Round, 99999);

		// 제한시간
		SHOWSTAGESETTING_LISTITEM( "StageLimitTime", pStageSetting->nLimitTime, pGameTypeCfg->m_LimitTime, 2147483647);
	}


/////////////////////////////////////////////////////////////////////////////////////////
#define SHOWSTAGESETTING_ITEM(_WidgetItemName, _NodeVariable, _nItemCount, _ItemList)	\
{																						\
	MComboBox* pCB = (MComboBox*)pResource->FindWidget(_WidgetItemName);				\
	if (pCB)																			\
	{																					\
		for (int i = 0; i < _nItemCount; i++)											\
		{																				\
			if (_ItemList[i].Value == _NodeVariable)									\
			{																			\
				pCB->SetSelIndex(i);													\
				break;																	\
			}																			\
		}																				\
	}																					\
}
/////////////////////////////////////////////////////////////////////////////////////////

	// 레벨제한
	SHOWSTAGESETTING_ITEM("StageLevelLimit", pStageSetting->nLimitLevel,
		STAGESETTING_LIMITLEVEL_MAX, StageSetting_LimitLevel);

	// 팀킬여부
	SHOWSTAGESETTING_ITEM("StageTeamKill", pStageSetting->bTeamKillEnabled,
		STAGESETTING_TEAMKILL_MAX, StageSetting_TeamKill);

	// 게임중참가
	SHOWSTAGESETTING_ITEM("StageIntrude", pStageSetting->bForcedEntryEnabled,
		STAGESETTING_FORCEDENTRY_MAX, StageSetting_ForcedEntry);

	// 팀 밸런스
	SHOWSTAGESETTING_ITEM("StageTeamBalancing", pStageSetting->bAutoTeamBalancing,
		STAGESETTING_TEAMBALANCING_MAX, StageSetting_TeamBalancing);

	MComboBox* pCBTeamBanlance = (MComboBox*)pResource->FindWidget( "StageTeamBalancing");
	if ( pCBTeamBanlance)
		pCBTeamBanlance->Enable( ZGetGameTypeManager()->IsTeamGame( pStageSetting->nGameType));

	SHOWSTAGESETTING_ITEM("PingStrikes", pStageSetting->nPingStrikes,
		STAGESETTING_PING_STRIKE_MAX, StageSetting_Ping_Strike);

	SHOWSTAGESETTING_ITEM("PingType", pStageSetting->nPingType,
		STAGESETTING_PING_TYPE_MAX, StageSetting_Ping_Type);
#ifdef _ROTATION
	SHOWSTAGESETTING_ITEM("StageTeamRotation", pStageSetting->bTeamAlternateSpawns,
		STAGESETTING_TEAMROTATION_MAX, StageSetting_TeamRotation);

	MComboBox* pCBTeamRotation = (MComboBox*)pResource->FindWidget("StageTeamRotation");
	if (pCBTeamRotation)
		pCBTeamRotation->Enable(ZGetGameTypeManager()->IsTeamGame(pStageSetting->nGameType));
#endif


	auto SetEditToInt = [&](const char* Name, int Value)
	{
		char buf[64];
		auto Widget = static_cast<MEdit*>(pResource->FindWidget(Name));
		if (Widget)
		{
			itoa_safe(Value, buf, 10);
			Widget->SetText(buf);
		}
	};

	auto SetButtonWidget = [&](const char* Name, bool Value, int Enable = -1)
	{
		auto Widget = static_cast<MButton*>(pResource->FindWidget(Name));
		if (Widget)
		{
			Widget->SetCheck(Value);
			if (Enable != -1)
				Widget->Enable(Enable != 0);
		}
	};


	if ( bShowAll)
	{
		AdjustLimitTimeStageSettingDialog();

		MWidget* pFindWidget = pResource->FindWidget("StageSettingFrame");
		if(pFindWidget!=NULL) pFindWidget->Show(true, true);
	}
}



void ZStageSetting::InitStageSettingDialog()
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	if (pResource == NULL) return;


#define INITSTAGESETTING_ITEM(_WidgetItemName, _NodeVariable, _nItemCount, _ItemList, _nItemDefaultIndex)	\
{																				\
	MComboBox* pCB = (MComboBox*)pResource->FindWidget(_WidgetItemName);		\
	if(pCB!=NULL)																\
	{																			\
		pCB->RemoveAll();														\
		for (int i = 0; i < _nItemCount; i++)									\
		{																		\
			pCB->Add(_ItemList[i].szText);										\
		}																		\
		pCB->SetSelIndex(_nItemDefaultIndex);									\
	}																			\
}

	char szText[ 256];


	// 게임 타입
	InitStageSettingGameType();



	// 레벨제한
#define GET_LIMITLEVEL(_value)		sprintf(szText,"%s %d",ZMsg(MSG_WORD_LEVELDIFF),_value);
	strcpy( StageSetting_LimitLevel[0].szText, ZMsg( MSG_WORD_NONE));
	GET_LIMITLEVEL( 5);	strcpy( StageSetting_LimitLevel[1].szText, szText);
	GET_LIMITLEVEL( 10);	strcpy( StageSetting_LimitLevel[2].szText, szText);
	GET_LIMITLEVEL( 15);	strcpy( StageSetting_LimitLevel[3].szText, szText);
	INITSTAGESETTING_ITEM("StageLevelLimit", pStageSetting->nLimitLevel,
		STAGESETTING_LIMITLEVEL_MAX, StageSetting_LimitLevel, STAGESETTING_LIMITLEVEL_DEFAULT);

	// 팀킬여부
	strcpy( StageSetting_TeamKill[0].szText, ZMsg(MSG_WORD_PERMIT));
	strcpy( StageSetting_TeamKill[1].szText, ZMsg(MSG_WORD_PROHIBIT));
	INITSTAGESETTING_ITEM("StageTeamKill", pStageSetting->bTeamKillEnabled,
		STAGESETTING_TEAMKILL_MAX, StageSetting_TeamKill, STAGESETTING_TEAMKILL_DEFAULT);

	// 게임중참가
	strcpy( StageSetting_ForcedEntry[0].szText, ZMsg(MSG_WORD_PERMIT));
	strcpy( StageSetting_ForcedEntry[1].szText, ZMsg(MSG_WORD_PROHIBIT));
	INITSTAGESETTING_ITEM("StageIntrude", pStageSetting->bForcedEntryEnabled,
		STAGESETTING_FORCEDENTRY_MAX, StageSetting_ForcedEntry, STAGESETTING_FORCEDENTRY_DEFAULT);

	// 팀 밸런스
	strcpy( StageSetting_TeamBalancing[0].szText, ZMsg(MSG_WORD_ON));
	strcpy( StageSetting_TeamBalancing[1].szText, ZMsg(MSG_WORD_OFF));
	INITSTAGESETTING_ITEM("StageTeamBalancing", pStageSetting->bAutoTeamBalancing,
		STAGESETTING_TEAMBALANCING_MAX, StageSetting_TeamBalancing, STAGESETTING_TEAMBALANCING_DEFAULT);

	// 관전 허용
	strcpy( StageSetting_Observer[0].szText, ZMsg(MSG_WORD_PERMIT));
	strcpy( StageSetting_Observer[1].szText, ZMsg(MSG_WORD_PROHIBIT));
	INITSTAGESETTING_ITEM("StageObserver", pStageSetting->bObserverEnabled,
		STAGESETTING_OBSERVER_MAX, StageSetting_Observer, STAGESETTING_OBSERVER_DEFAULT);

	// 투표 가능
	strcpy( StageSetting_Vote[0].szText, ZMsg(MSG_WORD_PERMIT));
	strcpy( StageSetting_Vote[1].szText, ZMsg(MSG_WORD_PROHIBIT));
	INITSTAGESETTING_ITEM("StageVote", pStageSetting->bVoteEnabled,
		STAGESETTING_VOTE_MAX, StageSetting_Vote, STAGESETTING_VOTE_DEFAULT);

	INITSTAGESETTING_ITEM("PingStrikes", pStageSetting->nPingStrikes, STAGESETTING_PING_STRIKE_MAX, StageSetting_Ping_Strike, STAGESETTING_PING_STRIKE_DEFAULT);
	INITSTAGESETTING_ITEM("PingType", pStageSetting->nPingType, STAGESETTING_PING_TYPE_MAX, StageSetting_Ping_Type, STAGESETTING_PING_TYPE_DEFAULT);
#ifdef _ROTATION
	strcpy(StageSetting_TeamRotation[0].szText, ZMsg(MSG_WORD_ON));
	strcpy(StageSetting_TeamRotation[1].szText, ZMsg(MSG_WORD_OFF));
	INITSTAGESETTING_ITEM("StageTeamRotation", pStageSetting->bTeamAlternateSpawns,
		STAGESETTING_TEAMROTATION_MAX, StageSetting_TeamRotation, STAGESETTING_TEAMROTATION_DEFAULT);
#endif


}

void ZStageSetting::ApplyStageSettingDialog()
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	if (pResource == NULL) return;

	MWidget* pWidget = pResource->FindWidget("StageSettingFrame");
	if(pWidget!=NULL) pWidget->Show(false);

	//	if(pWidget->GetParent()!=NULL) pWidget->GetParent()->Show(false);

	// 서버로 변경된 데이터 전송
	PostDataToServer();		// 동환이가 넣음
}

void ZStageSetting::AdjustLimitTimeStageSettingDialog()
{
/*	MSTAGE_SETTING_NODE*	pStageSetting = ZGetGameClient()->GetMatchStageSetting()->GetStageSetting();

	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	if (pResource == NULL) return;

	MComboBox* pCBLimitTime = (MComboBox*)pResource->FindWidget("StageLimitTime");
	MComboBox* pCBType = (MComboBox*)pResource->FindWidget( "StageType");
	if ( pCBType && (pCBLimitTime->GetSelIndex() >= 0))
	{
		// 이거 완전 하드코드... 고민끝에 나두 어쩔수 읍땅.... -_-;
		bool bIsTeam = false;
		if ( strcmp( pCBType->GetString( pCBType->GetSelIndex()), ZMsg(MSG_MT_DEATHMATCH_TEAM)) == 0)
			bIsTeam = true;
		else if ( strcmp( pCBType->GetString( pCBType->GetSelIndex()), ZMsg(MSG_MT_GLADIATOR_TEAM)) == 0)
			bIsTeam = true;
		else if ( strcmp( pCBType->GetString( pCBType->GetSelIndex()), ZMsg(MSG_MT_ASSASSINATE)) == 0)
			bIsTeam = true;
		else if ( strcmp( pCBType->GetString( pCBType->GetSelIndex()), ZMsg(MSG_MT_DUEL)) == 0)
			bIsTeam = true;
		

		if ( bIsTeam)
		{
			INITSTAGESETTING_ITEM( "StageLimitTime", pStageSetting->nLimitTime, STAGESETTING_LIMITTIME_MAX, StageSetting_LimitTime_Team, STAGESETTING_LIMITTIME_DEFAULT);
		}
		else
		{
			INITSTAGESETTING_ITEM( "StageLimitTime", pStageSetting->nLimitTime, STAGESETTING_LIMITTIME_MAX, StageSetting_LimitTime_Single, STAGESETTING_LIMITTIME_DEFAULT);
		}
	}
*/
}

// 서버로 변경된 데이터를 전송함(동환이가 작성)
void ZStageSetting::PostDataToServer()
{
	// 난입 상태라면 게임 상태를 바꿀 수 없다
	if ( !ZGetGameClient())
		return;

	if ( ZGetGameClient()->IsForcedEntry())
		return;

	MSTAGE_SETTING_NODE SettingNode;
	BuildStageSetting(&SettingNode);

	ZPostStageSetting(ZGetGameClient()->GetPlayerUID(), ZGetGameClient()->GetStageUID(), &SettingNode);
}


void ZStageSetting::InitStageSettingGameType()
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	if (pResource == NULL) return;

	MComboBox* pCB = (MComboBox*)pResource->FindWidget("StageType");
	MChannelRule* pRule = ZGetChannelRuleMgr()->GetCurrentRule();
	if (pRule == NULL) return;
	if(pCB == NULL) return;

	MChannelRuleGameTypeList* pGameTypeList = pRule->GetGameTypeList();

	pCB->RemoveAll();														

	for (MChannelRuleGameTypeList::iterator itor = pGameTypeList->begin(); itor != pGameTypeList->end(); ++itor)
	{
		int nGameType = (*itor);
		if ((nGameType < 0) || (nGameType >= MMATCH_GAMETYPE_MAX)) continue;

		// Custom: Forece Quest and Survival
		/*if (nGameType == MMATCH_GAMETYPE_QUEST || nGameType == MMATCH_GAMETYPE_SURVIVAL)  
			continue;*/
		pCB->Add(ZGetGameTypeManager()->GetGameTypeStr(MMATCH_GAMETYPE(nGameType)));
	}

	#ifdef _QUEST 
	{
		// Custom: Quest/Clan Server
		if ((ZGetGameClient()) && (ZGetGameClient()->GetServerMode() == MSM_TEST))
		{
			pCB->Add(ZGetGameTypeManager()->GetGameTypeStr(MMATCH_GAMETYPE(MMATCH_GAMETYPE_QUEST)));
			
#ifdef LOCALE_BRAZIL
#else
			// 브라질에서는 아직 서바이벌모드를 서비스 안함....나중에 서비스 할때 #ifdef를 풀어줘야함
			// server.ini에서 SURVIVALENABLE=0이면 비활성화
			if (ZGetGameClient()->IsEnabledSurvivalMode())
				pCB->Add(ZGetGameTypeManager()->GetGameTypeStr(MMATCH_GAMETYPE(MMATCH_GAMETYPE_SURVIVAL)));
#endif
		}
	}
	#endif

	pCB->SetSelIndex(MMATCH_GAMETYPE_DEFAULT);									
}


void ZStageSetting::InitStageSettingGameFromGameType()
{
	// 난입 상태라면 게임 상태를 바꿀 수 없다
	if ( !ZGetGameClient())
		return;

	if ( ZGetGameClient()->IsForcedEntry())
		return;


	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	if (pResource == NULL) return;

/////////////////////////////////////////////////////////////////////////////////
#define INITSTAGESETTING_LISTITEM( _WidgetItemName, _ItemList, _nItemDefaultIndex)	\
{																				\
	MComboBox* pCB = (MComboBox*)pResource->FindWidget( _WidgetItemName);		\
	if ( pCB)																	\
	{																			\
		int nSelect = pCB->GetSelIndex(), nItemCount = 0;						\
		pCB->RemoveAll();														\
		MGAMETYPECFGDATA::iterator itr = _ItemList.begin();						\
		for ( ; itr != _ItemList.end();  itr++)									\
			pCB->Add( (*itr)->m_szString);										\
		pCB->SetSelIndex( _nItemDefaultIndex);									\
	}																			\
}
/////////////////////////////////////////////////////////////////////////////////
	

	int nGameType = 0;
	MMATCH_GAMETYPE nPrevGameType = ZGetGameClient()->GetMatchStageSetting()->GetGameType();

	MComboBox* pCBType = (MComboBox*)pResource->FindWidget( "StageType");
	if ( pCBType && (pCBType->GetCount() > 0))
	{
		// 이거 완전 무식한 하드코드... 고민끝에 나두 어쩔수 읍땅.... -_-;
		if ( strcmp( pCBType->GetString( pCBType->GetSelIndex()), ZMsg(MSG_MT_DEATHMATCH_SOLO)) == 0)
			nGameType = 0;
		else if ( strcmp( pCBType->GetString( pCBType->GetSelIndex()), ZMsg(MSG_MT_DEATHMATCH_TEAM)) == 0)
			nGameType = 1;
		else if ( strcmp( pCBType->GetString( pCBType->GetSelIndex()), ZMsg(MSG_MT_GLADIATOR_SOLO)) == 0)
			nGameType = 2;
		else if ( strcmp( pCBType->GetString( pCBType->GetSelIndex()), ZMsg(MSG_MT_GLADIATOR_TEAM)) == 0)
			nGameType = 3;
		else if ( strcmp( pCBType->GetString( pCBType->GetSelIndex()), ZMsg(MSG_MT_ASSASSINATE)) == 0)
			nGameType = 4;
		else if ( strcmp( pCBType->GetString( pCBType->GetSelIndex()), ZMsg(MSG_MT_TRAINING)) == 0)
			nGameType = 5;
		else if ( strcmp( pCBType->GetString( pCBType->GetSelIndex()), ZMsg(MSG_MT_SURVIVAL)) == 0)
			nGameType = 6;
		else if ( strcmp( pCBType->GetString( pCBType->GetSelIndex()), ZMsg(MSG_MT_QUEST)) == 0)
			nGameType = 7;
		else if ( strcmp( pCBType->GetString( pCBType->GetSelIndex()), ZMsg(MSG_MT_BERSERKER)) == 0)
			nGameType = 8;
		else if ( strcmp( pCBType->GetString( pCBType->GetSelIndex()), ZMsg(MSG_MT_DEATHMATCH_TEAM2)) == 0)
			nGameType = 9;
		else if ( strcmp( pCBType->GetString( pCBType->GetSelIndex()), ZMsg(MSG_MT_DUEL)) == 0)
			nGameType = 10;
		else if ( strcmp( pCBType->GetString( pCBType->GetSelIndex()), ZMsg(MSG_MT_QUEST_CHALLENGE)) == 0)
			nGameType = 12;
		else if ( strcmp( pCBType->GetString( pCBType->GetSelIndex()), ZMsg(MSG_MT_TEAM_TRAINING)) == 0)
			nGameType = 13;
		else if ( strcmp( pCBType->GetString( pCBType->GetSelIndex()), ZMsg(MSG_MT_CTF)) == 0)
			nGameType = 14;
		else if ( strcmp( pCBType->GetString( pCBType->GetSelIndex()), ZMsg(MSG_MT_INFECTED)) == 0)
			nGameType = 15;
		else if (strcmp(pCBType->GetString(pCBType->GetSelIndex()), ZMsg(MSG_MT_GUNGAME)) == 0)
			nGameType = 16;
		else if (strcmp(pCBType->GetString(pCBType->GetSelIndex()), ZMsg(MSG_MT_SPYMODE)) == 0)
			nGameType = 17;
		else if (strcmp(pCBType->GetString(pCBType->GetSelIndex()), ZMsg(MSG_MT_TURBO)) == 0)
			nGameType = 18;
		else if (strcmp(pCBType->GetString(pCBType->GetSelIndex()), ZMsg(MSG_MT_BLITZ)) == 0)
			nGameType = 19;
		else if (strcmp(pCBType->GetString(pCBType->GetSelIndex()), ZMsg(MSG_MT_SKILL)) == 0)
			nGameType = 20;
		else
			_ASSERT(0); // Custom: Assert this in case I forget
	}


	// Set game type
	ZGetGameClient()->GetMatchStageSetting()->SetGameType( (MMATCH_GAMETYPE)nGameType);


	// Set game setting
	ZGameTypeConfig* pGameTypeCfg = ZGetConfiguration()->GetGameTypeList()->GetGameTypeCfg( nGameType);
	if ( pGameTypeCfg)
	{
		// 최대 인원
		INITSTAGESETTING_LISTITEM( "StageMaxPlayer", pGameTypeCfg->m_MaxPlayers, pGameTypeCfg->m_nDefaultMaxPlayers);

		// 라운드
		INITSTAGESETTING_LISTITEM( "StageRoundCount", pGameTypeCfg->m_Round, pGameTypeCfg->m_nDefaultRound);

		// 제한시간
		INITSTAGESETTING_LISTITEM( "StageLimitTime", pGameTypeCfg->m_LimitTime, pGameTypeCfg->m_nDefaultLimitTime);
	}
	else
		_ASSERT(0); // Custom: Sneaky gametypecfg shit


	// Set map
	MComboBox* pCB = (MComboBox*)pResource->FindWidget( "MapSelection");
	if ( pCB)
		InitMaps( pCB);
}