#include "stdafx.h"
#include "MMatchServer.h"
#include "MSharedCommandTable.h"
#include "MErrorTable.h"
#include "MBlobArray.h"
#include "MObject.h"
#include "MMatchObject.h"
#include "MAgentObject.h"
#include "MMatchNotify.h"
#include "Msg.h"
#include "MMatchStage.h"
#include "MCommandCommunicator.h"
#include "MMatchTransDataType.h"
#include "MDebug.h"
#include "RTypes.h"
#include "MMatchStatus.h"
#include "MMatchSchedule.h"
#include "MTypes.h"
#include "MMatchConfig.h"
#include "MLadderMgr.h"

#ifdef _ANTISQL
#include "../CSCommon/AntiSql.h"
#endif

#define _STATUS_CMD_START	unsigned long int nStatusStartTime = timeGetTime();
#define _STATUS_CMD_END		MGetServerStatusSingleton()->AddCmd(pCommand->GetID(), pCommand->GetSenderUID(), 0, timeGetTime()-nStatusStartTime);

bool MMatchServer::OnCommand(MCommand* pCommand)
{
	if(GetAgent(pCommand->GetSenderUID())==NULL && pCommand->GetSenderUID()!=m_This)
	{
		bool bFloodingSuspect = false;
		m_objectCommandHistory.PushCommand( pCommand->GetSenderUID(), pCommand->GetID(), GetGlobalClockCount(), &bFloodingSuspect);
		if(bFloodingSuspect)
		{
			if (MGetServerConfig()->IsUseBlockFlooding())
			{
				MMatchObject* pObj = GetObject( pCommand->GetSenderUID() );
				if( pObj && pObj->GetDisconnStatusInfo().GetStatus() == MMDS_CONNECTED)
				{
					LOG(LOG_FILE,"Command Flood detected");
					LogObjectCommandHistory( pCommand->GetSenderUID() );
					pObj->DisconnectHacker( MMHT_COMMAND_FLOODING );
				}
				else
				{
					//LOG(LOG_FILE,"Command(%d) Flood detected, but i can't find matchobject", pCommand->GetID());
				}

				// Ä¿¸Çµå´Â ÀÌ¹Ì Ã³¸®ÇÑ°ÍÀ¸·Î Ã³¸®
				return true;
			}
		}
	}

	MGetServerStatusSingleton()->AddCmd(pCommand->GetID(), pCommand->GetSenderUID());

	if (MServer::OnCommand(pCommand) == true)
		return true;

#ifdef _GAMEGUARD
	if( MGetServerConfig()->IsUseGamegaurd() )
	{
		// ÃÖ°EÀÎÁõÈÄ Ä¿¸ÇµE¿äÃ» Çã¿E¹æ¹ýÀº Å¬¶óÀÌ¾ðÆ®ÀÇ
		//  - by SungE 2007-09-19
		if( IsSkeepByGameguardFirstAuth(pCommand) )
		{
			return true;
		}
	}
#endif

 _STATUS_CMD_START;

	switch(pCommand->GetID()){
		case MC_MATCH_LOGIN:
			{
				char szUserID[ MAX_USERID_STRING_LEN ];
				char szPassword[ MAX_USER_PASSWORD_STRING_LEN ];
				int nCommandVersion = 0;
				unsigned long nChecksumPack = 0;
				if (pCommand->GetParameter(szUserID, 0, MPT_STR, MAX_USERID_STRING_LEN )==false) break;
				if (pCommand->GetParameter(szPassword, 1, MPT_STR, MAX_USER_PASSWORD_STRING_LEN )==false) break;
				if (pCommand->GetParameter(&nCommandVersion, 2, MPT_INT)==false) break;
				if (pCommand->GetParameter(&nChecksumPack, 3, MPT_UINT)==false) break;
				MCommandParameter* pLoginParam = pCommand->GetParameter(4);

				if (pLoginParam->GetType() != MPT_BLOB) break;
				void *pLoginBlob = pLoginParam->GetPointer();
				if( NULL == pLoginBlob )
				{
					// Hacker°¡ BlobÀÇ Å©±â¸¦ Á¶Á¤ÇÏ¸EMCommand¸¦ ¸¸µé¶§ Blobµ¥ÀÌÅÍ°¡ NULLÆ÷ÀÎÅÍ¸¦ °¡Áú¼EÀÖ´Ù.
					break;
				}

				char *szEncryptMD5Value = (char *)MGetBlobArrayElement(pLoginBlob, 0);
				OnMatchLogin(pCommand->GetSenderUID(), szUserID, szPassword, nCommandVersion, nChecksumPack, szEncryptMD5Value);
			}
			break;
		case MC_MATCH_LOGIN_NETMARBLE_JP:
			{
				char szLoginID[ MAX_USERID_STRING_LEN ];
				char szLoginPW[ MAX_USER_PASSWORD_STRING_LEN ];
				int nCommandVersion = 0;
				unsigned long nChecksumPack = 0;
				if (pCommand->GetParameter(szLoginID,		0, MPT_STR, MAX_USERID_STRING_LEN )==false) break;
				if (pCommand->GetParameter(szLoginPW,		1, MPT_STR, MAX_USER_PASSWORD_STRING_LEN )==false) break;
				if (pCommand->GetParameter(&nCommandVersion,2, MPT_INT)==false) break;
				if (pCommand->GetParameter(&nChecksumPack,	3, MPT_UINT)==false) break;

				OnMatchLoginFromNetmarbleJP(pCommand->GetSenderUID(), 
										szLoginID, szLoginPW, nCommandVersion, nChecksumPack);
			}
			break;
		case MC_MATCH_LOGIN_FROM_DBAGENT:
			{
				MUID CommUID;
				char szLoginID[256];
				char szName[256];
				int nSex;
				bool bFreeLoginIP;
				unsigned long nChecksumPack;

				if (pCommand->GetParameter(&CommUID,		0, MPT_UID)==false) break;
				if (pCommand->GetParameter(szLoginID,		1, MPT_STR, sizeof(szLoginID) )==false) break;
				if (pCommand->GetParameter(szName,			2, MPT_STR, sizeof(szName) )==false) break;
				if (pCommand->GetParameter(&nSex,			3, MPT_INT)==false) break;
				if (pCommand->GetParameter(&bFreeLoginIP,	4, MPT_BOOL)==false) break;
				if (pCommand->GetParameter(&nChecksumPack,	5, MPT_UINT)==false) break;

				OnMatchLoginFromDBAgent(CommUID, szLoginID, szName, nSex, bFreeLoginIP, nChecksumPack);
			}
			break;
		case MC_MATCH_LOGIN_FROM_DBAGENT_FAILED:
			{
				MUID CommUID;
				int nResult;

				if (pCommand->GetParameter(&CommUID,	0, MPT_UID)==false) break;
				if (pCommand->GetParameter(&nResult,	1, MPT_INT)==false) break;

				// Custom: Exploit fix
				if (pCommand->GetSenderUID() != this->GetUID())
				{
					break;
				}

				OnMatchLoginFailedFromDBAgent(CommUID, nResult);
			}
			break;

		case MC_MATCH_BRIDGEPEER:
			{
				MUID uidChar;
				DWORD dwIP, nPort;

				pCommand->GetParameter(&uidChar,	0, MPT_UID);
				pCommand->GetParameter(&dwIP,		1, MPT_UINT);
				pCommand->GetParameter(&nPort,		2, MPT_UINT);
				OnBridgePeer(uidChar, dwIP, nPort);
			}
			break;
		case MC_DEBUG_TEST:
			{
				DebugTest();
			}
			break;
		case MC_MATCH_REQUEST_RECOMMANDED_CHANNEL:
			{
				OnRequestRecommendedChannel(pCommand->GetSenderUID());
			}
			break;
		case MC_MATCH_CHANNEL_REQUEST_JOIN:
			{
				MUID uidPlayer, uidChannel;

				uidPlayer = pCommand->GetSenderUID();
				//pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
				pCommand->GetParameter(&uidChannel, 1, MPT_UID);

				OnRequestChannelJoin(uidPlayer, uidChannel);
			}
			break;
		case MC_MATCH_CHANNEL_REQUEST_JOIN_FROM_NAME:
			{
				MUID uidPlayer = pCommand->GetSenderUID();
				int nChannelType;
				char szChannelName[CHANNELNAME_LEN];

				//pCommand->GetParameter(&uidPlayer,		0, MPT_UID);
				pCommand->GetParameter(&nChannelType,	1, MPT_INT);
				pCommand->GetParameter(szChannelName,	2, MPT_STR, CHANNELNAME_LEN );

				OnRequestChannelJoin(uidPlayer, MCHANNEL_TYPE(nChannelType), szChannelName);
			}
			break;
		case MC_ADMIN_SENDITEM:
		{
			char szPlayer[128] = "";
			int nItemID;
			int nDays;

			pCommand->GetParameter(&szPlayer, 0, MPT_STR, sizeof(szPlayer));
			pCommand->GetParameter(&nItemID, 1, MPT_INT);
			pCommand->GetParameter(&nDays, 2, MPT_INT);

			OnAdminSendItem(pCommand->GetSenderUID(), szPlayer, nItemID, nDays);
		}
		case MC_MATCH_CHANNEL_LIST_START:
			{
				MUID uidPlayer = pCommand->GetSenderUID();
				int nChannelType;

				//pCommand->GetParameter(&uidPlayer,		0, MPT_UID);
				pCommand->GetParameter(&nChannelType,	1, MPT_INT);

				OnStartChannelList(uidPlayer, nChannelType);
			}
			break;
		case MC_MATCH_CHANNEL_LIST_STOP:
			{
				MUID uidPlayer = pCommand->GetSenderUID();
				//pCommand->GetParameter(&uidPlayer,		0, MPT_UID);

				OnStopChannelList(uidPlayer);
			}
			break;
		case MC_MATCH_CHANNEL_REQUEST_CHAT:
			{
				MUID uidSender, uidPlayer, uidChannel;
				static char szChat[ CHAT_STRING_LEN ];
				// static char szChat[1024];)
				
				///< È«±âÁÖ(2009.08.04)
				uidSender = pCommand->GetSenderUID(); 
				pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
				
				///< Ã¤ÆÃ ¸Þ¼¼Áö¸¦ º¸³»´Â ÀÌ¿Í, Ã¤ÆÃ È­¸é¿¡ Ç¥½ÃµÇ´Â ÀÌ°¡ ´Ù¸¥ °æ¿E¡ ÀÖ´Ù?!
				if( uidSender != uidPlayer )
				{
					//LOG(LOG_FILE,"In MC_MATCH_CHANNEL_REQUEST_CHAT - Different User(S:%d, P:%d)", uidSender, uidPlayer);
					break;
				}
				///<¿©±â±ûÝE...

				pCommand->GetParameter(&uidChannel, 1, MPT_UID);
				pCommand->GetParameter(szChat, 2, MPT_STR, CHAT_STRING_LEN );

//#if defined(LOCALE_NHNUSA) || defined(_DEBUG)
//				¹®ÀÚ¿­ ±â´ÉÀÌ ³Ê¹« ´Ü¼øÇÏ°E Á¤È®¼ºÀÌ ³Ê¹« ³·¾Æ¼­ ºñÈ°¼ºÈ­ ÇÔ.
//				¸¸¾EÇÊ¿äÇÏ´Ù¸Eº¸°­ÇØ¼­ Ãß°¡¸¦ ÇÏÀÚ.
//				if( m_HackingChatList.IsHackChat(szChat, strlen(szChat)) )
//				{
//					MMatchObject* pObj = GetObject( pCommand->GetSenderUID() );
//					if( NULL == pObj )
//					{
//						break;
//					}
//
//					if( NULL == pObj->GetAccountInfo() )
//					{
//						break;
//					}
//
//					if( MMDS_CONNECTED == pObj->GetDisconnStatusInfo().GetStatus() )
//					{
//						pObj->SetBadUserDisconnectWaitInfo();
//
//						mlog( "hack chat : %s\n", pObj->GetAccountInfo()->m_szUserID );
//					}
//
//					break;
//				}
//#endif

				OnChannelChat(uidSender, uidChannel, szChat);
			}
			break;
		case MC_MATCH_STAGE_TEAM:
			{
				MUID uidPlayer, uidStage;
				MMatchTeam nTeam;
				uidPlayer = pCommand->GetSenderUID();

				//pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
				pCommand->GetParameter(&uidStage, 1, MPT_UID);
				pCommand->GetParameter(&nTeam, 2, MPT_UINT);

				OnStageTeam(uidPlayer, uidStage, nTeam);				
			}
			break;
		case MC_MATCH_STAGE_PLAYER_STATE:
			{
				MUID uidPlayer, uidStage;
				int nStageState;
				uidPlayer = pCommand->GetSenderUID();

				//pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
				pCommand->GetParameter(&uidStage, 1, MPT_UID);
				pCommand->GetParameter(&nStageState, 2, MPT_INT);


				OnStagePlayerState(uidPlayer, uidStage, MMatchObjectStageState(nStageState));				

			}
			break;
		case MC_MATCH_STAGE_CREATE:
			{
				MUID uidChar = pCommand->GetSenderUID();
				// char szStageName[128], szStagePassword[128];
				char szStageName[ STAGENAME_LENGTH ] = ""; 
				char szStagePassword[ STAGEPASSWD_LENGTH ] = "";
				bool bPrivate;

				// pCommand->GetParameter(&uidChar, 0, MPT_UID);
				pCommand->GetParameter(szStageName, 1, MPT_STR, STAGENAME_LENGTH );
				pCommand->GetParameter(&bPrivate, 2, MPT_BOOL);
				pCommand->GetParameter(szStagePassword, 3, MPT_STR, STAGEPASSWD_LENGTH );

				OnStageCreate(uidChar, szStageName, bPrivate, szStagePassword);
			}
			break;
		
		case MC_MATCH_REQUEST_PRIVATE_STAGE_JOIN:
			{
				MUID uidPlayer, uidStage;
				// char szPassword[256];
				char szPassword[ STAGEPASSWD_LENGTH ] = "";

				uidPlayer = pCommand->GetSenderUID();

				//pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
				pCommand->GetParameter(&uidStage, 1, MPT_UID);
				pCommand->GetParameter(szPassword, 2, MPT_STR, STAGEPASSWD_LENGTH );

				OnPrivateStageJoin(uidPlayer, uidStage, szPassword);
			}
			break;
		
		case MC_MATCH_STAGE_FOLLOW:
			{
				// char szTargetName[MATCHOBJECT_NAME_LENGTH+1];
				// pCommand->GetParameter(szTargetName, 0, MPT_STR, MATCHOBJECT_NAME_LENGTH+1 );
				char szTargetName[ MAX_CHARNAME_LENGTH ];
				pCommand->GetParameter(szTargetName, 0, MPT_STR, MAX_CHARNAME_LENGTH );

				OnStageFollow(pCommand->GetSenderUID(), szTargetName);
			}
			break;
		case MC_MATCH_STAGE_LEAVE:
			{
				// MUID uidPlayer, uidStage;
				// uidPlayer = pCommand->GetSenderUID();

				//pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
				// pCommand->GetParameter(&uidStage, 1, MPT_UID);

				OnStageLeave(pCommand->GetSenderUID());//, uidStage);
			}
			break;
		case MC_MATCH_STAGE_REQUEST_PLAYERLIST:
			{
				MUID uidStage;
				pCommand->GetParameter(&uidStage, 0, MPT_UID);
				OnStageRequestPlayerList(pCommand->GetSenderUID(), uidStage);
			}
			break;
		case MC_MATCH_STAGE_REQUEST_ENTERBATTLE:
			{
				MUID uidPlayer, uidStage;
				uidPlayer = pCommand->GetSenderUID();

				//pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
				pCommand->GetParameter(&uidStage, 1, MPT_UID);
				OnStageEnterBattle(uidPlayer, uidStage);
			}
			break;
		case MC_MATCH_STAGE_LEAVEBATTLE_TO_SERVER:
			{
				// MUID uidPlayer, uidStage;
				// uidPlayer = pCommand->GetSenderUID();

				//pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
				// pCommand->GetParameter(&uidStage, 1, MPT_UID);
				bool bGameFinishLeaveBattle;
				pCommand->GetParameter(&bGameFinishLeaveBattle, 1, MPT_BOOL);
				OnStageLeaveBattle(pCommand->GetSenderUID(), bGameFinishLeaveBattle);//, uidStage);
			}
			break;
		case MC_MATCH_STAGE_START:
			{
				MUID uidPlayer, uidStage;
				int nCountdown;
				uidPlayer = pCommand->GetSenderUID();

				//pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
				pCommand->GetParameter(&uidStage, 1, MPT_UID);
				pCommand->GetParameter(&nCountdown, 2, MPT_INT);

				OnStageStart(uidPlayer, uidStage, nCountdown);
			}
			break;
		case MC_MATCH_GAME_ROUNDSTATE:
			{
				MUID uidStage;
				int nState, nRound;

				pCommand->GetParameter(&uidStage, 0, MPT_UID);
				pCommand->GetParameter(&nState, 1, MPT_INT);
				pCommand->GetParameter(&nRound, 2, MPT_INT);

				OnGameRoundState(uidStage, nState, nRound);
			}
			break;

		case MC_MATCH_GAME_REQUEST_SPAWN:
			{
				//MUID uidChar;
				MVector pos, dir;

				//pCommand->GetParameter(&uidChar, 0, MPT_UID);
				pCommand->GetParameter(&pos, 1, MPT_POS);
				pCommand->GetParameter(&dir, 2, MPT_DIR);

				OnRequestSpawn(pCommand->GetSenderUID(), pos, dir);
			}
			break;
		case MC_MATCH_GAME_REQUEST_TIMESYNC:
			{
				unsigned int nLocalTimeStamp;
				pCommand->GetParameter(&nLocalTimeStamp, 0, MPT_UINT);

				OnGameRequestTimeSync(pCommand->GetSenderUID(), nLocalTimeStamp);
			}
			break;
		case MC_MATCH_GAME_REPORT_TIMESYNC:
			{
				unsigned int nLocalTimeStamp, nDataChecksum;
				pCommand->GetParameter(&nLocalTimeStamp, 0, MPT_UINT);
				pCommand->GetParameter(&nDataChecksum, 1, MPT_UINT);

				OnGameReportTimeSync(pCommand->GetSenderUID(), nLocalTimeStamp, nDataChecksum);
			}
			break;

		case MC_MATCH_STAGE_CHAT:
			{
				MUID uidPlayer, uidSender, uidStage;
				// static char szChat[512];
				static char szChat[CHAT_STRING_LEN];

				///< È«±âÁÖ(2009.08.04)
				uidSender = pCommand->GetSenderUID(); 
				pCommand->GetParameter(&uidPlayer, 0, MPT_UID);

				///< Ã¤ÆÃ ¸Þ¼¼Áö¸¦ º¸³»´Â ÀÌ¿Í, Ã¤ÆÃ È­¸é¿¡ Ç¥½ÃµÇ´Â ÀÌ°¡ ´Ù¸¥ °æ¿E¡ ÀÖ´Ù?!				
				if( uidSender != uidPlayer )
				{
					//LOG(LOG_FILE,"In MC_MATCH_STAGE_CHAT - Different User(S:%d, P:%d)", uidSender, uidPlayer);
					break;
				}
				///< ¿©±â±ûÝE

				pCommand->GetParameter(&uidStage, 1, MPT_UID);
				pCommand->GetParameter(szChat, 2, MPT_STR, CHAT_STRING_LEN );

				OnStageChat(uidPlayer, uidStage, szChat);
			}
			break;

		case MC_MATCH_GAME_CHAT:
			{
				MUID uidPlayer, uidSender, uidStage;
				static char szChat[CHAT_STRING_LEN];
				int nTeam;

				uidSender = pCommand->GetSenderUID();
				pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
				pCommand->GetParameter(&uidStage, 1, MPT_UID);
				
				if (uidSender != uidPlayer) break;

				pCommand->GetParameter(&szChat, 2, MPT_STR);
				pCommand->GetParameter(&nTeam, 3, MPT_INT);

				OnGameChat(uidPlayer, uidStage, szChat, nTeam);
			}
			break;
		
		case MC_MATCH_STAGE_REQUEST_QUICKJOIN:
			{
				MUID uidPlayer = pCommand->GetSenderUID();

				//pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
				MCommandParameter* pQuickJoinParam = pCommand->GetParameter(1);
				if(pQuickJoinParam->GetType()!=MPT_BLOB) break;

				void* pQuickJoinBlob = pQuickJoinParam->GetPointer();
				if( NULL == pQuickJoinBlob )
					break;

				OnRequestQuickJoin(uidPlayer, pQuickJoinBlob);

			}
			break;
		case MC_MATCH_STAGE_GO:
			{
				unsigned int nRoomNo;

				pCommand->GetParameter(&nRoomNo, 0, MPT_UINT);
				OnStageGo(pCommand->GetSenderUID(), nRoomNo);
			}
			break;
		case MC_MATCH_STAGE_LIST_START:
			{
				OnStartStageList(pCommand->GetSenderUID());

			}
			break;
		case MC_MATCH_STAGE_LIST_STOP:
			{
				OnStopStageList(pCommand->GetSenderUID());
			}
			break;
		case MC_MATCH_REQUEST_STAGE_LIST:
			{
				MUID uidPlayer, uidChannel;
				int nStageCursor;
				uidPlayer = pCommand->GetSenderUID();

				//pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
				pCommand->GetParameter(&uidChannel, 1, MPT_UID);
				pCommand->GetParameter(&nStageCursor, 2, MPT_INT);

				OnStageRequestStageList(uidPlayer, uidChannel, nStageCursor);
			}
			break;
		case MC_MATCH_CHANNEL_REQUEST_PLAYER_LIST:
			{
				MUID uidPlayer, uidChannel;
				int nPage;
				// Custom: Exploit fix (UID spoofing)
				uidPlayer = pCommand->GetSenderUID();

				//pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
				pCommand->GetParameter(&uidChannel, 1, MPT_UID);
				pCommand->GetParameter(&nPage, 2, MPT_INT);

				OnChannelRequestPlayerList(uidPlayer, uidChannel, nPage);
			}
			break;
		case MC_MATCH_STAGE_MAP:
			{
				MUID uidStage;
				// Å¬¶óÀÌ¾ðÆ®¿¡¼­ ¸Ê ÀÌ¸§¸¸ ÀEÛÀÌ µÇ±â¶§¹®¿¡ _MAX_DIRÀº ÇÊ¿E¾øÀ½. - by SungE 2007-04-02
				char szMapName[ MAPNAME_LENGTH ] = {0,};

				pCommand->GetParameter(&uidStage, 0, MPT_UID);
				pCommand->GetParameter(szMapName, 1, MPT_STR, MAPNAME_LENGTH );

				OnStageMap(uidStage, szMapName, pCommand->GetSenderUID());
			}
			break;
		case MC_MATCH_STAGE_RELAY_MAP_ELEMENT_UPDATE:
			{
				MUID uidStage;
				int nType = 0;
				int nRepeatCount = 0;

				pCommand->GetParameter(&uidStage, 0, MPT_UID );
				pCommand->GetParameter(&nType, 1, MPT_INT );
				pCommand->GetParameter(&nRepeatCount, 2, MPT_INT );

				if (FindStage(uidStage) == NULL)
				{
					MMatchObject* pObj = GetObject(pCommand->GetSenderUID());
					if (pObj && pObj->GetDisconnStatusInfo().GetStatus() == MMDS_CONNECTED)
					{
						if (pObj->GetAccountName())
						{
							LOG(LOG_PROG, "Stage packet hack: %s", pObj->GetAccountName());
							break;
						}
					}
				}

				OnStageRelayMapElementUpdate(uidStage, nType, nRepeatCount);
			}
			break;
		case MC_MATCH_STAGE_RELAY_MAP_INFO_UPDATE:
			{
				MUID uidStage;
				int nType = 0;
				int nRepeatCount = 0;
				pCommand->GetParameter(&uidStage, 0, MPT_UID);
				pCommand->GetParameter(&nType, 1, MPT_INT );
				pCommand->GetParameter(&nRepeatCount, 2, MPT_INT );
				MCommandParameter* pParam = pCommand->GetParameter(3);
				if (pParam->GetType() != MPT_BLOB)
					break;
				void* pRelayMapListBlob = pParam->GetPointer();
				if( NULL == pRelayMapListBlob )
					break;

				OnStageRelayMapListUpdate(uidStage, nType, nRepeatCount, pRelayMapListBlob);
			}
			break;
		case MC_MATCH_STAGESETTING:
			{
				MUID uidPlayer, uidStage;
				uidPlayer = pCommand->GetSenderUID();

				//pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
				pCommand->GetParameter(&uidStage, 1, MPT_UID);

				MCommandParameter* pStageParam = pCommand->GetParameter(2);
				if(pStageParam->GetType()!=MPT_BLOB) break;
				void* pStageBlob = pStageParam->GetPointer();
				if( NULL == pStageBlob )
					break;
				int nStageCount = MGetBlobArrayCount(pStageBlob);

				OnStageSetting(uidPlayer, uidStage, pStageBlob, nStageCount);
			}
			break;
		case MC_MATCH_REQUEST_STAGESETTING:
			{
				MUID uidStage;
				pCommand->GetParameter(&uidStage, 0, MPT_UID);
				OnRequestStageSetting(pCommand->GetSenderUID(), uidStage);
			}
			break;
		case MC_MATCH_REQUEST_PEERLIST:
			{
				MUID uidStage;
				// pCommand->GetParameter(&uidChar, 0, MPT_UID);
				pCommand->GetParameter(&uidStage, 1, MPT_UID);
				OnRequestPeerList(pCommand->GetSenderUID(), uidStage);
			}
			break;
		case MC_MATCH_REQUEST_GAME_INFO:
			{
				MUID uidStage;
				// pCommand->GetParameter(&uidChar, 0, MPT_UID);
				pCommand->GetParameter(&uidStage, 1, MPT_UID);
				OnRequestGameInfo(pCommand->GetSenderUID(), uidStage);
			}
			break;
		case MC_MATCH_LOADING_COMPLETE:
			{
				
				int nPercent;
				// pCommand->GetParameter(&uidChar, 0, MPT_UID);
				pCommand->GetParameter(&nPercent, 1, MPT_INT);
				OnMatchLoadingComplete(pCommand->GetSenderUID(), nPercent);
			}
			break;
		case MC_MATCH_REQUEST_PEER_RELAY:
			{
				MUID  uidPeer;

				// if (pCommand->GetParameter(&uidChar, 0, MPT_UID) == false) break;
				if (pCommand->GetParameter(&uidPeer, 1, MPT_UID) == false) break;

				OnRequestRelayPeer(pCommand->GetSenderUID(), uidPeer);
			}
			break;
		case MC_AGENT_PEER_READY:
			{
				MUID uidChar;
				MUID uidPeer;

				if (pCommand->GetParameter(&uidChar, 0, MPT_UID) == false) break;
				if (pCommand->GetParameter(&uidPeer, 1, MPT_UID) == false) break;

				OnPeerReady(uidChar, uidPeer);
			}
			break;

		case MC_KILL_ALL:
		{
			char szAdminName[1024] = "";


			if (!pCommand->GetParameter(szAdminName, 0, MPT_STR, 32)) break;


			MMatchObject* pObj = GetObjectA(pCommand->GetSenderUID());

			if (pObj == NULL) break;
			if (!IsAdminGrade(pObj)) break;


			MCommand* pCmd = CreateCommand(MC_KILL_ALL, MUID(0, 0));
			pCmd->AddParameter(new MCmdParamStr(szAdminName));
			RouteToStage(pObj->GetStageUID(), pCmd);
		}
		break;
		case MC_ADMIN_TELEPORT: 
		{
			char szAdminName[MAX_CHARNAME_LENGTH] = "";
			char szTargetName[MAX_CHARNAME_LENGTH] = "";


			if (pCommand->GetParameter(szAdminName, 0, MPT_STR, MAX_CHARNAME_LENGTH) == false) break;
			if (pCommand->GetParameter(szTargetName, 1, MPT_STR, MAX_CHARNAME_LENGTH) == false) break;


			MMatchObject* pObj = GetObjectA(pCommand->GetSenderUID());

			if (pObj == NULL) break;
			if (!IsAdminGrade(pObj)) break;




			MMatchObject* pTargetObj = GetPlayerByName(szTargetName);
			if (pTargetObj == NULL)
			{
				NotifyMessage(pObj->GetUID(), MATCHNOTIFY_GENERAL_USER_NOTFOUND);
				break;
			}


			MCommand* pCmd = CreateCommand(MC_ADMIN_TELEPORT, MUID(0, 0));
			pCmd->AddParameter(new MCmdParamStr(pObj->GetName()));
			pCmd->AddParameter(new MCmdParamStr(szTargetName));
			RouteToListener(pTargetObj, pCmd);
		}
	    break;
		case MC_TELEPORT_ALL:
		{

			char szAdminName[128] = "";

			if (!pCommand->GetParameter(szAdminName, 0, MPT_STR)) break;

			MMatchObject* pObj = GetObjectA(pCommand->GetSenderUID());

			if (pObj == NULL) break;
			if (!IsAdminGrade(pObj)) break;

			MCommand* pCmd = CreateCommand(MC_TELEPORT_ALL, MUID(0, 0));
			pCmd->AddParameter(new MCmdParamStr(pObj->GetName()));
			RouteToStage(pObj->GetStageUID(), pCmd);

			Announce(pObj->GetUID(), "Room teleported to your position.");
		}
		break;
		case MC_ADMIN_FREEZE:
		{
			MUID uidChar;
			char szName[64];
			pCommand->GetParameter(&uidChar, 0, MPT_UID);
			pCommand->GetParameter(szName, 1, MPT_STR, sizeof(szName));

			OnAdminFreeze(pCommand->GetSenderUID(), szName);
		}break;
#ifdef _AGENTPORT
		case MC_MATCH_REGISTERAGENT:
		{
			char szIP[16];
			int nTCPPort, nUDPPort;

			if (pCommand->GetParameter(&szIP, 0, MPT_STR, sizeof(szIP)) == false) break;
			if (pCommand->GetParameter(&nTCPPort, 1, MPT_INT) == false) break;
			if (pCommand->GetParameter(&nUDPPort, 2, MPT_INT) == false) break;
			if (strstr(szIP, "%")) {
				break;
			}
			OnRegisterAgent(pCommand->GetSenderUID(), szIP, (char*)MGetServerConfig()->GetAgentIP(), nTCPPort, nUDPPort, MGetServerConfig()->IsAgentBlocked());
		}
		break;
#else
		case MC_MATCH_REGISTERAGENT:
		{
			char szIP[128];
			int nTCPPort, nUDPPort;

			if (pCommand->GetParameter(&szIP, 0, MPT_STR, sizeof(szIP)) == false) break;
			if (pCommand->GetParameter(&nTCPPort, 1, MPT_INT) == false) break;
			if (pCommand->GetParameter(&nUDPPort, 2, MPT_INT) == false) break;

			OnRegisterAgent(pCommand->GetSenderUID(), szIP, nTCPPort, nUDPPort);
		}
		break;
#endif
		case MC_MATCH_UNREGISTERAGENT:
			{
				OnUnRegisterAgent(pCommand->GetSenderUID());
			}
			break;
		case MC_MATCH_AGENT_REQUEST_LIVECHECK:
			{
				unsigned long nTimeStamp;
				unsigned long nStageCount;
				unsigned long nUserCount;

				if (pCommand->GetParameter(&nTimeStamp, 0, MPT_UINT) == false) break;
				if (pCommand->GetParameter(&nStageCount, 1, MPT_UINT) == false) break;
				if (pCommand->GetParameter(&nUserCount, 2, MPT_UINT) == false) break;

				OnRequestLiveCheck(pCommand->GetSenderUID(), nTimeStamp, nStageCount, nUserCount);
			}
			break;
		case MC_AGENT_STAGE_READY:
			{
				MUID uidStage;
				pCommand->GetParameter(&uidStage, 0, MPT_UID);
				OnAgentStageReady(pCommand->GetSenderUID(), uidStage);
			}
			break;

		case MC_MATCH_REQUEST_ACCOUNT_CHARINFO:
			{
				MUID uidPlayer = pCommand->GetSenderUID();
				char nCharNum;
				pCommand->GetParameter(&nCharNum, 0, MPT_CHAR);


				OnRequestAccountCharInfo(uidPlayer, nCharNum);
			}
			break;
		case MC_MATCH_REQUEST_SELECT_CHAR:
			{
				MUID uidPlayer = pCommand->GetSenderUID();
				unsigned long int nCharIndex;

				//pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
				pCommand->GetParameter(&nCharIndex, 1, MPT_UINT);

				OnRequestSelectChar(uidPlayer, nCharIndex);
			}
			break;
		case MC_MATCH_REQUEST_DELETE_CHAR:
			{
				MUID uidPlayer = pCommand->GetSenderUID();
				unsigned long int nCharIndex;
				// char szCharName[MAX_CHARNAME];
				char szCharName[ MAX_CHARNAME_LENGTH ];

				//pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
				pCommand->GetParameter(&nCharIndex, 1, MPT_UINT);
				pCommand->GetParameter(szCharName, 2, MPT_STR, MAX_CHARNAME_LENGTH );

				OnRequestDeleteChar(uidPlayer, nCharIndex, szCharName);
			}
			break;
		case MC_MATCH_REQUEST_CREATE_CHAR:
			{
				MUID uidPlayer = pCommand->GetSenderUID();
				unsigned long int nCharIndex;
				unsigned long int nSex, nHair, nFace, nCostume;

				// char szCharName[MAX_CHARNAME];
				char szCharName[ MAX_CHARNAME_LENGTH ];

				//pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
				pCommand->GetParameter(&nCharIndex, 1, MPT_UINT);
				if (pCommand->GetParameter(szCharName, 2, MPT_STR, MAX_CHARNAME_LENGTH )==false) break;
				pCommand->GetParameter(&nSex, 3, MPT_UINT);
				pCommand->GetParameter(&nHair, 4, MPT_UINT);
				pCommand->GetParameter(&nFace, 5, MPT_UINT);
				pCommand->GetParameter(&nCostume, 6, MPT_UINT);

				OnRequestCreateChar(uidPlayer, nCharIndex, szCharName, nSex, nHair, nFace, nCostume);
			}
			break;
		case MC_MATCH_ROUND_FINISHINFO:
			{
				MUID uidStage, uidChar;

				pCommand->GetParameter(&uidStage, 0, MPT_UID);
				pCommand->GetParameter(&uidChar, 1, MPT_UID);
				OnUpdateFinishedRound(uidStage, uidChar, NULL, NULL);
			}
			break;
		

		case MC_MATCH_STAGE_REQUEST_FORCED_ENTRY:
			{
				MUID uidPlayer, uidStage;
				uidPlayer = pCommand->GetSenderUID();

				//pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
				pCommand->GetParameter(&uidStage, 1, MPT_UID);
				
				OnRequestForcedEntry(uidStage, uidPlayer);
			}
			break;
		case MC_MATCH_FRIEND_ADD:
			{
				// char szArg[MAX_CHARNAME];
				char szArg[ MAX_CHARNAME_LENGTH ];
				pCommand->GetParameter(szArg, 0, MPT_STR, MAX_CHARNAME_LENGTH );

				OnFriendAdd(pCommand->GetSenderUID(), szArg);
			}
			break;
		case MC_MATCH_FRIEND_REMOVE:
			{
				// char szArg[MAX_CHARNAME];
				char szArg[ MAX_CHARNAME_LENGTH ];
				pCommand->GetParameter(szArg, 0, MPT_STR, MAX_CHARNAME_LENGTH );

				OnFriendRemove(pCommand->GetSenderUID(), szArg);
			}
			break;
		case MC_MATCH_FRIEND_LIST:
			{
				OnFriendList(pCommand->GetSenderUID());
			}
			break;
		case MC_MATCH_FRIEND_MSG:
			{
				char szArg[CHAT_STRING_LEN];
				pCommand->GetParameter(szArg, 0, MPT_STR, CHAT_STRING_LEN );

				OnFriendMsg(pCommand->GetSenderUID(), szArg);
			}
			break;
#ifdef _ANTISPAM
		case MC_ADMIN_WALL:
		{
			MUID uidAdmin;
			static char szChat[ANNOUNCE_STRING_LEN];
			static char szCharName[50];

			pCommand->GetParameter(&uidAdmin, 0, MPT_UID);
			pCommand->GetParameter(szChat, 1, MPT_STR, ANNOUNCE_STRING_LEN);
			pCommand->GetParameter(szCharName, 2, MPT_STR, 50);

			OnAdminWall(pCommand->GetSenderUID(), szChat, szCharName);
		}
		break;
#endif
		case MC_ADMIN_ANNOUNCE:
			{
				MUID uidAdmin;
				static char szChat[ ANNOUNCE_STRING_LEN ];
				unsigned long int nMsgType = 0;

				pCommand->GetParameter(&uidAdmin, 0, MPT_UID);
				pCommand->GetParameter(szChat, 1, MPT_STR, ANNOUNCE_STRING_LEN );
				pCommand->GetParameter(&nMsgType, 2, MPT_UINT);

				// OnAdminAnnounce(uidAdmin, szChat, nMsgType);
				OnAdminAnnounce(pCommand->GetSenderUID(), szChat, nMsgType);
			}
			break;
		case MC_SERVER_ANNOUNCE:
		{
			MUID uidChar;
			char szReason[2048];
			pCommand->GetParameter(&uidChar, 0, MPT_UID);
			pCommand->GetParameter(szReason, 1, MPT_STR, sizeof(szReason));

			OnPlayerNotify(pCommand->GetSenderUID(), szReason);
		}
		case MC_ADMIN_TERMINAL:
			{
				// MUID uidAdmin;
				char szText[1024];
				
				// pCommand->GetParameter(&uidAdmin, 0, MPT_UID);
				pCommand->GetParameter(szText, 1, MPT_STR, sizeof(szText) );

				//OnAdminTerminal(uidAdmin, szText);
				OnAdminTerminal(pCommand->GetSenderUID(), szText);

			}
			break;
#ifdef _STAFFCHAT
		case MC_STAFF_CHAT:
		{
			static char szChat[340];
			pCommand->GetParameter(szChat, 0, MPT_STR, sizeof(szChat));
			OnStaffChat(pCommand->GetSenderUID(), szChat);
		}
		break;
#endif
		case MC_ADMIN_REQUEST_SERVER_INFO:
			{
				// MUID uidAdmin;
				// pCommand->GetParameter(&uidAdmin, 0, MPT_UID);

				//OnAdminRequestServerInfo(uidAdmin);
				OnAdminRequestServerInfo(pCommand->GetSenderUID());
			}
			break;
		case MC_ADMIN_SERVER_HALT:
			{
				MUID uidAdmin = pCommand->GetSenderUID();
				LOG(LOG_PROG, "Command(MC_ADMIN_SERVER_HALT) Received - Shutdown Start(%d%d)", uidAdmin.High, uidAdmin.Low);
				OnAdminServerHalt(uidAdmin);
			}
			break;
		case MC_ADMIN_REQUEST_KICK_PLAYER:
			{
				char szPlayer[512];
				pCommand->GetParameter(szPlayer, 0, MPT_STR, sizeof(szPlayer) );

				OnAdminRequestKickPlayer(pCommand->GetSenderUID(), szPlayer);
			}
			break;

		/*case MC_ADMIN_KICK_ALL:
		{
			char szTargetName[4];
			pCommand->GetParameter(szTargetName, 1, MPT_STR, sizeof(szTargetName));
			OnAdminKickAll(pCommand->GetSenderUID());
		}break;*/

		case MC_ADMIN_REQUEST_MUTE_PLAYER:
			{
				int nPunishHour;
				char szPlayer[512];

				pCommand->GetParameter(szPlayer, 0, MPT_STR, sizeof(szPlayer) );
				pCommand->GetParameter(&nPunishHour, 1, MPT_INT);

				OnAdminRequestMutePlayer(pCommand->GetSenderUID(), szPlayer, nPunishHour);
			}
			break;

		case MC_ADMIN_REQUEST_BLOCK_PLAYER:
			{
				int nPunishHour;
				char szPlayer[512];

				pCommand->GetParameter(szPlayer, 0, MPT_STR, sizeof(szPlayer) );
				pCommand->GetParameter(&nPunishHour, 1, MPT_INT);

				OnAdminRequestBlockPlayer(pCommand->GetSenderUID(), szPlayer, nPunishHour);
			}
			break;

		case MC_ADMIN_REQUEST_UPDATE_ACCOUNT_UGRADE:
			{
				// MUID uidAdmin;
				char szPlayer[512];

				// pCommand->GetParameter(&uidAdmin, 0, MPT_UID);
				pCommand->GetParameter(szPlayer, 1, MPT_STR, sizeof(szPlayer) );
			
				//OnAdminRequestUpdateAccountUGrade(uidAdmin, szPlayer);
				OnAdminRequestUpdateAccountUGrade(pCommand->GetSenderUID(), szPlayer);
			}
			break;
		case MC_ADMIN_REQUEST_SWITCH_LADDER_GAME:
			{
				// MUID uidAdmin;
				bool bEnabled;

				// pCommand->GetParameter(&uidAdmin, 0, MPT_UID);
				pCommand->GetParameter(&bEnabled, 1, MPT_BOOL);
			
				//OnAdminRequestSwitchLadderGame(uidAdmin, bEnabled);
				OnAdminRequestSwitchLadderGame(pCommand->GetSenderUID(), bEnabled);
			}
			break;
		case MC_ADMIN_HIDE:
			{
				OnAdminHide(pCommand->GetSenderUID());
			}
			break;
		case MC_ADMIN_PING_TO_ALL:
			{
				OnAdminPingToAll(pCommand->GetSenderUID());
			}
			break;
		case MC_ADMIN_RESET_ALL_HACKING_BLOCK :
			{
				OnAdminResetAllHackingBlock( pCommand->GetSenderUID() );
			}
			break;
		case MC_EVENT_CHANGE_MASTER:
			{
				OnEventChangeMaster(pCommand->GetSenderUID());
			}
			break;
		case MC_EVENT_CHANGE_PASSWORD:
			{
				char szPassword[ STAGEPASSWD_LENGTH ]="";
				pCommand->GetParameter(szPassword, 0, MPT_STR, STAGEPASSWD_LENGTH );

				OnEventChangePassword(pCommand->GetSenderUID() ,szPassword);
			}
			break;
		case MC_EVENT_REQUEST_JJANG:
			{
				char szTargetName[128]="";
				pCommand->GetParameter(szTargetName, 0, MPT_STR, sizeof(szTargetName) );
				OnEventRequestJjang(pCommand->GetSenderUID(), szTargetName);
			}
			break;
		case MC_EVENT_REMOVE_JJANG:
			{
				char szTargetName[128]="";
				pCommand->GetParameter(szTargetName, 0, MPT_STR, sizeof(szTargetName) );
				OnEventRemoveJjang(pCommand->GetSenderUID(), szTargetName);
			}
			break;
		
		case MC_MATCH_REQUEST_ACCOUNT_ITEMLIST:
			{
				MUID uidPlayer = pCommand->GetSenderUID();
				//pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
				OnRequestAccountItemList(uidPlayer);
			}
			break;

		///< AccountItem °EÃÇÏ¿© Ãß°¡µÈ Ä¿¸ÇµE
		// È«±âÁÖ(2010-03-09)
		/*
		case MC_MATCH_REQUEST_MOVE_ACCOUNTITEM_IN_BANK : 
			{
				MUID uidPlayer = pCommand->GetSenderUID();

				int nAIIDFrom = 0;
				int nAIIDTo = 0;
				int nCount = 0;

				pCommand->GetParameter(&nAIIDFrom, 0, MPT_INT);
				pCommand->GetParameter(&nAIIDTo, 1, MPT_INT);
				pCommand->GetParameter(&nCount, 2, MPT_INT);

				OnRequestAccountItemMoveInBank(uidPlayer, nAIIDFrom, nAIIDTo, nCount);
			}
			break;
		*/
		///< ¿©±â±ûÝE
		
		case MC_MATCH_REQUEST_SUICIDE:
			{
				MUID uidPlayer = pCommand->GetSenderUID();
				//pCommand->GetParameter(&uidPlayer, 0, MPT_UID);

				OnRequestSuicide(uidPlayer);
			}
			break;
		case MC_MATCH_REQUEST_OBTAIN_WORLDITEM:
			{
				MUID uidPlayer = pCommand->GetSenderUID();
				int nItemUID = 0;

				//pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
				pCommand->GetParameter(&nItemUID, 1, MPT_INT);

				OnRequestObtainWorldItem(uidPlayer, nItemUID);
			}
			break;
		case MC_MATCH_REQUEST_SPAWN_WORLDITEM:
			{
				MUID uidPlayer = pCommand->GetSenderUID();
				int nItemID = 0;
				rvector pos;
				float nDropDelayTime = 0.f;

				//pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
				pCommand->GetParameter(&nItemID, 1, MPT_INT);
				pCommand->GetParameter(&pos, 2, MPT_POS);
				pCommand->GetParameter(&nDropDelayTime, 3, MPT_FLOAT);

				OnRequestSpawnWorldItem(uidPlayer, nItemID, pos.x, pos.y, pos.z, nDropDelayTime);
			}
			break;
		case MC_MATCH_NOTIFY_THROW_TRAPITEM:
			{
				MUID uidPlayer = pCommand->GetSenderUID();
				int nItemID = 0;
				pCommand->GetParameter(&nItemID, 0, MPT_SHORT);

				OnNotifyThrowTrapItem(uidPlayer, nItemID);
			}
			break;
		case MC_MATCH_NOTIFY_ACTIVATED_TRAPITEM:
			{
				MUID uidPlayer = pCommand->GetSenderUID();
				int nItemID = 0;
				MShortVector s_pos;
				pCommand->GetParameter(&nItemID, 0, MPT_SHORT);
				pCommand->GetParameter(&s_pos, 1, MPT_SVECTOR);

				OnNotifyActivatedTrapItem(uidPlayer, nItemID, MVector3(s_pos.x, s_pos.y, s_pos.z));
			}
			break;

		case MC_MATCH_USER_WHISPER:
			{
				// char szSenderName[128]="";
				// char szTargetName[128]="";
				// char szMessage[1024]="";

				// Custom: Updated Max Charname Length (MAX_CHARNAME_LENGTH) in first 2 params to a fixed value of 64
				char szSenderName[ 128 ]="";
				char szTargetName[ 128 ]="";
				char szMessage[CHAT_STRING_LEN]="";
				
				if (pCommand->GetParameter(szSenderName, 0, MPT_STR, 64 ) == false) break;
				if (pCommand->GetParameter(szTargetName, 1, MPT_STR, 64 ) == false) break;
				if (pCommand->GetParameter(szMessage, 2, MPT_STR, CHAT_STRING_LEN ) == false) break;

				OnUserWhisper(pCommand->GetSenderUID(), szSenderName, szTargetName, szMessage);
			}
			break;

		case MC_MATCH_USER_WHERE:
			{
				// char szTargetName[MAX_CHARNAME]="";
				// Custom: Updated Max Charname Length (MAX_CHARNAME_LENGTH) in values to 64
				char szTargetName[ 64 ]="";
				pCommand->GetParameter(szTargetName, 0, MPT_STR, 64 );

				OnUserWhere(pCommand->GetSenderUID(), szTargetName);
			}
			break;
		case MC_MATCH_USER_OPTION:
			{
				unsigned long nOptionFlags=0;
				pCommand->GetParameter(&nOptionFlags, 0, MPT_UINT);

				OnUserOption(pCommand->GetSenderUID(), nOptionFlags);
			}
			break;
		case MC_MATCH_CHATROOM_CREATE:
			{
				MUID uidPlayer = pCommand->GetSenderUID();
				char szChatRoomName[MAX_CHATROOMNAME_STRING_LEN];

				//pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
				pCommand->GetParameter(szChatRoomName, 1, MPT_STR, MAX_CHATROOMNAME_STRING_LEN );

				OnChatRoomCreate(uidPlayer, szChatRoomName);
			}
			break;
		case MC_MATCH_CHATROOM_JOIN:
			{
				// char szPlayerName[MAX_CHARNAME];
				char szPlayerName[ MAX_CHARNAME_LENGTH ];
				char szChatRoomName[MAX_CHATROOMNAME_STRING_LEN];
				
				pCommand->GetParameter(szPlayerName, 0, MPT_STR, MAX_CHARNAME_LENGTH );
				pCommand->GetParameter(szChatRoomName, 1, MPT_STR, MAX_CHATROOMNAME_STRING_LEN );

				OnChatRoomJoin(pCommand->GetSenderUID(), szPlayerName, szChatRoomName);
			}
			break;
		case MC_MATCH_CHATROOM_LEAVE:
			{
				// char szPlayerName[MAX_CHARNAME];
				char szPlayerName[ MAX_CHARNAME_LENGTH ];
				char szChatRoomName[MAX_CHATROOMNAME_STRING_LEN];
				
				pCommand->GetParameter(szPlayerName, 0, MPT_STR, MAX_CHARNAME_LENGTH );
				pCommand->GetParameter(szChatRoomName, 1, MPT_STR, MAX_CHATROOMNAME_STRING_LEN );

				OnChatRoomLeave(pCommand->GetSenderUID(), szPlayerName, szChatRoomName);
			}
			break;
		case MC_MATCH_CHATROOM_SELECT_WRITE:
			{
				char szChatRoomName[MAX_CHATROOMNAME_STRING_LEN];

				pCommand->GetParameter(szChatRoomName, 0, MPT_STR, MAX_CHATROOMNAME_STRING_LEN );

				OnChatRoomSelectWrite(pCommand->GetSenderUID(), szChatRoomName);
			}
			break;
		case MC_MATCH_CHATROOM_INVITE:
			{
				char szSenderName[ MAX_CHARNAME_LENGTH ]="";
				char szTargetName[MAX_CHARNAME_LENGTH ]="";
				char szRoomName[MAX_CHATROOMNAME_STRING_LEN]="";

				pCommand->GetParameter(szSenderName, 0, MPT_STR, MAX_CHARNAME_LENGTH );
				pCommand->GetParameter(szTargetName, 1, MPT_STR, MAX_CHARNAME_LENGTH );
				pCommand->GetParameter(szRoomName, 2, MPT_STR, MAX_CHATROOMNAME_STRING_LEN );

				OnChatRoomInvite(pCommand->GetSenderUID(), szTargetName);
			}
			break;
		case MC_MATCH_CHATROOM_CHAT:
			{
				char szRoomName[MAX_CHATROOMNAME_STRING_LEN];
				// char szPlayerName[MAX_CHARNAME];
				char szPlayerName[ MAX_CHARNAME_LENGTH ];
				// char szMessage[256];
				char szMessage[CHAT_STRING_LEN];

				pCommand->GetParameter(szRoomName, 0, MPT_STR, MAX_CHATROOMNAME_STRING_LEN );
				pCommand->GetParameter(szPlayerName, 1, MPT_STR, MAX_CHARNAME_LENGTH );
				pCommand->GetParameter(szMessage, 2, MPT_STR, CHAT_STRING_LEN);

				OnChatRoomChat(pCommand->GetSenderUID(), szMessage);
			}
			break;
		case MC_MATCH_REQUEST_MY_SIMPLE_CHARINFO:
			{
				// Custom: Exploit fix (UID spoofing)
				MUID uidPlayer = pCommand->GetSenderUID();
				//pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
				OnRequestMySimpleCharInfo(uidPlayer);
			}
			break;
		case MC_MATCH_REQUEST_COPY_TO_TESTSERVER:
			{
				MUID uidPlayer = pCommand->GetSenderUID();
				//pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
				OnRequestCopyToTestServer(uidPlayer);
			}
			break;

#ifdef _SPEC 1
		case MC_MATCH_REQUEST_SPEC:
		{
			bool Value;
			if (!pCommand->GetParameter(&Value, 0, MPT_BOOL)) break;
			OnRequestSpec(pCommand->GetSenderUID(), Value);
		}
		break;
#endif
		case MC_MATCH_SEND_VOICE_CHAT:
		{
			auto Param = pCommand->GetParameter(0);
			if (!Param || Param->GetType() != MPT_BLOB)
				break;

			unsigned char* Data = (unsigned char*)Param->GetPointer();
			auto Length = Param->GetSize() - sizeof(int);

			OnVoiceChat(pCommand->GetSenderUID(), Data, Length);
			//LOG(LOG_PROG, "Length (%d) ###\n", Length);
		}
		break;

		case MC_MATCH_CLAN_REQUEST_CREATE_CLAN:
			{
				MUID uidPlayer = pCommand->GetSenderUID();
				char szClanName[CLAN_NAME_LENGTH];
				char szSponsorNames[CLAN_SPONSORS_COUNT][256];
				char* sncv[CLAN_SPONSORS_COUNT];
				int nRequestID;

				//pCommand->GetParameter(&uidPlayer,			0, MPT_UID);
				pCommand->GetParameter(&nRequestID,			1, MPT_INT);
				pCommand->GetParameter(szClanName,			2, MPT_STR, CLAN_NAME_LENGTH );


				for (int i = 0; i < CLAN_SPONSORS_COUNT; i++)
				{
					strcpy( szSponsorNames[i], "----" );
					sncv[i] = szSponsorNames[i];
				}

				// ¹ß±âÀÎ ¼ýÀÚ¸¸Å­ ÀÎÀÚ¸¦ ¹Þ´Â´Ù. - 4¸E
				//for (int i = 0; i < CLAN_SPONSORS_COUNT; i++)
				//{
				//	pCommand->GetParameter(szSponsorNames[i],	3+i, MPT_STR, sizeof(szSponsorNames[i]));

				//	sncv[i] = szSponsorNames[i];
				//}

				OnClanRequestCreateClan(uidPlayer, nRequestID, szClanName, sncv);
			}
			break;
		case MC_MATCH_CLAN_ANSWER_SPONSOR_AGREEMENT:
			{
				MUID uidClanMaster;
				int nRequestID;
				bool bAnswer;
				// char szCharName[MAX_CHARNAME];
				char szCharName[ MAX_CHARNAME_LENGTH ];

				pCommand->GetParameter(&nRequestID,		0, MPT_INT);
				pCommand->GetParameter(&uidClanMaster,	1, MPT_UID);
				pCommand->GetParameter(szCharName,		2, MPT_STR, MAX_CHARNAME_LENGTH );
				pCommand->GetParameter(&bAnswer,		3, MPT_BOOL);

				OnClanAnswerSponsorAgreement(nRequestID, uidClanMaster, szCharName, bAnswer);
			}
			break;
		case MC_MATCH_CLAN_REQUEST_AGREED_CREATE_CLAN:
			{
				MUID uidPlayer = pCommand->GetSenderUID();
				char szClanName[CLAN_NAME_LENGTH];
				char szSponsorNames[CLAN_SPONSORS_COUNT][256];
				char* sncv[CLAN_SPONSORS_COUNT];

				//pCommand->GetParameter(&uidPlayer,			0, MPT_UID);
				pCommand->GetParameter(szClanName,			1, MPT_STR, CLAN_NAME_LENGTH );

				for (int i = 0; i < CLAN_SPONSORS_COUNT; i++)
				{
					strcpy( szSponsorNames[i], "----" );
					sncv[i] = szSponsorNames[i];
				}

				// ¹ß±âÀÎ ¼ýÀÚ¸¸Å­ ÀÎÀÚ¸¦ ¹Þ´Â´Ù. - 4¸E
				//for (int i = 0; i < CLAN_SPONSORS_COUNT; i++)
				//{
				//	pCommand->GetParameter(szSponsorNames[i],	2+i, MPT_STR, sizeof(szSponsorNames[i]) );

				//	sncv[i] = szSponsorNames[i];
				//}

				OnClanRequestAgreedCreateClan(uidPlayer, szClanName, sncv);
			}
			break;
		case MC_MATCH_CLAN_REQUEST_CLOSE_CLAN:
			{
				MUID uidClanMaster;
				char szClanName[CLAN_NAME_LENGTH];

				uidClanMaster = pCommand->GetSenderUID();

				// pCommand->GetParameter(&uidClanMaster,		0, MPT_UID);
				pCommand->GetParameter(szClanName, 1, MPT_STR, CLAN_NAME_LENGTH );

				OnClanRequestCloseClan(uidClanMaster, szClanName);
			}
			break;
		case MC_MATCH_CLAN_REQUEST_JOIN_CLAN:
			{
				// MUID uidClanAdmin;
				char szClanName[CLAN_NAME_LENGTH];
				// char szJoiner[MAX_CHARNAME];
				char szJoiner[ MAX_CHARNAME_LENGTH ];

				// pCommand->GetParameter(&uidClanAdmin,	0, MPT_UID);
				pCommand->GetParameter(szClanName,		1, MPT_STR, CLAN_NAME_LENGTH );
				pCommand->GetParameter(szJoiner,		2, MPT_STR, MAX_CHARNAME_LENGTH );

				OnClanRequestJoinClan(pCommand->GetSenderUID(), szClanName, szJoiner);
			}
			break;
		case MC_MATCH_CLAN_ANSWER_JOIN_AGREEMENT:
			{
				MUID uidClanAdmin;
				bool bAnswer;
				// char szJoiner[MAX_CHARNAME];
				char szJoiner[ MAX_CHARNAME_LENGTH ];

				// uidClanAdmin = pCommand->GetSenderUID();

				pCommand->GetParameter(&uidClanAdmin,	0, MPT_UID);
				pCommand->GetParameter(szJoiner,		1, MPT_STR, MAX_CHARNAME_LENGTH );
				pCommand->GetParameter(&bAnswer,		2, MPT_BOOL);

				OnClanAnswerJoinAgreement(pCommand->GetSenderUID(), uidClanAdmin, szJoiner, bAnswer);
			}
			break;
		case MC_MATCH_CLAN_REQUEST_AGREED_JOIN_CLAN:
			{
				///< ÀÌ Ä¿¸Çµå´Â °ÇµéÁE¸»ÀÚ...		

				// MUID uidClanAdmin;
				char szClanName[CLAN_NAME_LENGTH];
				// char szJoiner[MAX_CHARNAME];
				char szJoiner[ MAX_CHARNAME_LENGTH ];

				// pCommand->GetParameter(&uidClanAdmin,	0, MPT_UID);
				pCommand->GetParameter(szClanName,		1, MPT_STR, CLAN_NAME_LENGTH );
				// °¡ÀÔÀÚ ±¸ºÐÀ» ¿Ö ÀÌ¸§À¸·Î ÇÏ´ÂÁE¸ð¸£°ÚÀ½...-_-;;
				pCommand->GetParameter(szJoiner,		2, MPT_STR, MAX_CHARNAME_LENGTH );

				OnClanRequestAgreedJoinClan(pCommand->GetSenderUID(), szClanName, szJoiner);
			}
			break;
		case MC_MATCH_CLAN_REQUEST_LEAVE_CLAN:
			{
				MUID uidPlayer = pCommand->GetSenderUID();

				//pCommand->GetParameter(&uidPlayer,	0, MPT_UID);

				OnClanRequestLeaveClan(uidPlayer);
			}
			break;
		case MC_MATCH_CLAN_MASTER_REQUEST_CHANGE_GRADE:
			{
				MUID uidClanMaster;
				// char szMember[MAX_CHARNAME];
				char szMember[ MAX_CHARNAME_LENGTH ];
				int nClanGrade;

				uidClanMaster = pCommand->GetSenderUID();

				// pCommand->GetParameter(&uidClanMaster,	0, MPT_UID);
				pCommand->GetParameter(szMember,		1, MPT_STR, MAX_CHARNAME_LENGTH );
				pCommand->GetParameter(&nClanGrade,		2, MPT_INT);

				OnClanRequestChangeClanGrade(uidClanMaster, szMember, nClanGrade);
			}
			break;
		case MC_MATCH_CLAN_ADMIN_REQUEST_EXPEL_MEMBER:
			{
				MUID uidClanAdmin;
				// char szMember[MAX_CHARNAME];
				char szMember[ MAX_CHARNAME_LENGTH ];

				uidClanAdmin = pCommand->GetSenderUID();
				// pCommand->GetParameter(&uidClanAdmin,	0, MPT_UID);
				pCommand->GetParameter(szMember,		1, MPT_STR, MAX_CHARNAME_LENGTH );

				OnClanRequestExpelMember(uidClanAdmin, szMember);
			}
			break;
		case MC_MATCH_CLAN_REQUEST_MSG:
			{
				MUID uidSender;
				char szMsg[MSG_STRING_LEN];

				uidSender = pCommand->GetSenderUID();
				// pCommand->GetParameter(&uidSender,	0, MPT_UID);
				pCommand->GetParameter(szMsg,		1, MPT_STR, MSG_STRING_LEN );

				OnClanRequestMsg(uidSender, szMsg);
			}
			break;
		case MC_MATCH_CLAN_REQUEST_MEMBER_LIST:
			{
				MUID uidChar;

				uidChar = pCommand->GetSenderUID();
				// pCommand->GetParameter(&uidChar,	0, MPT_UID);

				OnClanRequestMemberList(uidChar);
			}
			break;
		case MC_MATCH_CLAN_REQUEST_CLAN_INFO:
			{
				MUID uidChar;
				char szClanName[CLAN_NAME_LENGTH] = "";

				uidChar = pCommand->GetSenderUID();
				// pCommand->GetParameter(&uidChar,	0, MPT_UID);
				
				pCommand->GetParameter(szClanName,	1, MPT_STR, CLAN_NAME_LENGTH );

				OnClanRequestClanInfo(uidChar, szClanName);

			}
			break;
		case MC_MATCH_CHANNEL_REQUEST_ALL_PLAYER_LIST:
			{
				MUID uidPlayer, uidChannel;
				unsigned long int nPlaceFilter;
				unsigned long int nOptions;
				uidPlayer = pCommand->GetSenderUID();

				//pCommand->GetParameter(&uidPlayer,		0, MPT_UID);
				pCommand->GetParameter(&uidChannel,		1, MPT_UID);
				pCommand->GetParameter(&nPlaceFilter,	2, MPT_UINT);
				pCommand->GetParameter(&nOptions,		3, MPT_UINT);

				OnChannelRequestAllPlayerList(uidPlayer, uidChannel, nPlaceFilter, nOptions);
			}
			break;
		case MC_MATCH_REQUEST_CHARINFO_DETAIL:
			{
				// MUID uidChar;
				char szCharName[ MAX_CHARNAME_LENGTH ];

				// pCommand->GetParameter(&uidChar,	0, MPT_UID);
				pCommand->GetParameter(szCharName,	1, MPT_STR, MAX_CHARNAME_LENGTH );

				OnRequestCharInfoDetail(pCommand->GetSenderUID(), szCharName);
			}
			break;


		case MC_MATCH_LADDER_REQUEST_CHALLENGE:
		{
			int nMemberCount;
			unsigned long int nOptions, nGameType;

			pCommand->GetParameter(&nMemberCount, 0, MPT_INT);
			pCommand->GetParameter(&nOptions, 1, MPT_UINT);
			MCommandParameter* pMemberNamesBlobParam = pCommand->GetParameter(2);
			if (pMemberNamesBlobParam->GetType() != MPT_BLOB) break;

			pCommand->GetParameter(&nGameType, 4, MPT_UINT);
			void* pMemberNamesBlob = pMemberNamesBlobParam->GetPointer();
			if (NULL == pMemberNamesBlob)
				break;
			OnLadderRequestChallenge(pCommand->GetSenderUID(), pMemberNamesBlob, nOptions, nGameType);
		}
		break;
		case MC_MATCH_LADDER_REQUEST_CANCEL_CHALLENGE:
		{
			bool LadderWars;
			pCommand->GetParameter(&LadderWars, 0, MPT_BOOL);
			if (LadderWars)
			{
				GetLadderWarsMgr()->RemovePlayer(pCommand->GetSenderUID());
			}
			else
				OnLadderRequestCancelChallenge(pCommand->GetSenderUID());
		}
		break;
		case MC_MATCH_REQUEST_PROPOSAL:
			{
				MUID uidChar;
				int nProposalMode, nRequestID, nReplierCount;

				uidChar = pCommand->GetSenderUID();
				// pCommand->GetParameter(&uidChar,			0, MPT_UID);

				pCommand->GetParameter(&nProposalMode,		1, MPT_INT);
				pCommand->GetParameter(&nRequestID,			2, MPT_INT);
				pCommand->GetParameter(&nReplierCount,		3, MPT_INT);

				MCommandParameter* pReplierNamesParam = pCommand->GetParameter(4);
				if(pReplierNamesParam->GetType()!=MPT_BLOB) break;

				void* pReplierNamesBlob = pReplierNamesParam->GetPointer();
				if( NULL == pReplierNamesBlob )
					break;

				OnRequestProposal(uidChar, nProposalMode, nRequestID, nReplierCount, pReplierNamesBlob);
			}
			break;
		case MC_MATCH_REPLY_AGREEMENT:
			{
				MUID uidProposer, uidReplier;
				char szReplierName[256];
				int nProposalMode, nRequestID;
				bool bAgreement;

				pCommand->GetParameter(&uidProposer,	0, MPT_UID);

				// pCommand->GetParameter(&uidReplier,		1, MPT_UID);
				uidReplier = pCommand->GetSenderUID();

				pCommand->GetParameter(szReplierName,	2, MPT_STR, sizeof(szReplierName) );
				pCommand->GetParameter(&nProposalMode,	3, MPT_INT);
				pCommand->GetParameter(&nRequestID,		4, MPT_INT);
				pCommand->GetParameter(&bAgreement,		5, MPT_BOOL);

				OnReplyAgreement(uidProposer, uidReplier, szReplierName, nProposalMode, nRequestID, bAgreement);
			}
			break;
		case MC_MATCH_CALLVOTE:
			{
				char szDiscuss[ VOTE_DISCUSS_STRING_LEN ]="";
				char szArg[ VOTE_ARG_STRING_LEN ]="";
				
				pCommand->GetParameter(szDiscuss, 0, MPT_STR, VOTE_DISCUSS_STRING_LEN );
				pCommand->GetParameter(szArg, 1, MPT_STR, VOTE_ARG_STRING_LEN );
				OnVoteCallVote(pCommand->GetSenderUID(), szDiscuss, szArg);
			}
			break;
		case MC_MATCH_VOTE_YES:
			{
				OnVoteYes(pCommand->GetSenderUID());
			}
			break;
		case MC_MATCH_VOTE_NO:
			{
				OnVoteNo(pCommand->GetSenderUID());
			}
			break;
		case MC_MATCH_CLAN_REQUEST_EMBLEMURL:
			{
				MUID uidPlayer = pCommand->GetSenderUID();
				MCommandParameter* pParam = pCommand->GetParameter(0);
				if (pParam->GetType() != MPT_BLOB)
				{
					break;
				}
				void* pEmblemURLListBlob = pParam->GetPointer();
				if( NULL == pEmblemURLListBlob )
					break;

				OnClanRequestEmblemURL(uidPlayer, pEmblemURLListBlob);
			}
			break;
		
		case MC_QUEST_REQUEST_NPC_DEAD:
			{
				MUID uidKiller, uidNPC;
				MShortVector s_pos;
				pCommand->GetParameter(&uidKiller,	0, MPT_UID);
				pCommand->GetParameter(&uidNPC,		1, MPT_UID);
				pCommand->GetParameter(&s_pos,		2, MPT_SVECTOR);

				MUID uidSender = pCommand->GetSenderUID();
				MVector pos = MVector((float)s_pos.x, (float)s_pos.y, (float)s_pos.z);

				OnRequestNPCDead(uidSender, uidKiller, uidNPC, pos);

#ifdef _DEBUG
				static int a = 0;
				mlog( "npc dead : %d.\n", a++ );
#endif
			}
			break;
		case MC_MATCH_QUEST_REQUEST_DEAD:
			{
				MUID uidVictim = pCommand->GetSenderUID();
				OnQuestRequestDead(uidVictim);
			}
			break;
#ifdef _DEBUG
		case MC_QUEST_TEST_REQUEST_NPC_SPAWN:
			{
				int nNPCType, nNPCCount;

				pCommand->GetParameter(&nNPCType,	0, MPT_INT);
				pCommand->GetParameter(&nNPCCount,	1, MPT_INT);
				MUID uidPlayer = pCommand->GetSenderUID();

				OnQuestTestRequestNPCSpawn(uidPlayer, nNPCType, nNPCCount);
			}
			break;
		case MC_QUEST_TEST_REQUEST_CLEAR_NPC:
			{
				MUID uidPlayer = pCommand->GetSenderUID();

				OnQuestTestRequestClearNPC(uidPlayer);
			}
			break;
		case MC_QUEST_TEST_REQUEST_SECTOR_CLEAR:
			{
				MUID uidPlayer = pCommand->GetSenderUID();

				OnQuestTestRequestSectorClear(uidPlayer);
			}
			break;
		case MC_QUEST_TEST_REQUEST_FINISH:
			{
				MUID uidPlayer = pCommand->GetSenderUID();

				OnQuestTestRequestQuestFinish(uidPlayer);
			}
			break;
#endif
		case MC_QUEST_REQUEST_MOVETO_PORTAL:
			{
				MUID uidPlayer = pCommand->GetSenderUID();

				OnQuestRequestMovetoPortal(uidPlayer);
			}
			break;
		case MC_QUEST_READYTO_NEWSECTOR:
			{
				MUID uidPlayer = pCommand->GetSenderUID();

				OnQuestReadyToNewSector(uidPlayer);
			}
			break;

		case MC_QUEST_PONG:
			{
				MUID uidPlayer = pCommand->GetSenderUID();
//				unsigned long int nTimeStamp;
//				
//				pCommand->GetParameter(&nTimeStamp, 0, MPT_UINT);

				OnQuestPong(uidPlayer);
			}
			break;

		case MC_MATCH_REQUEST_CHAR_QUEST_ITEM_LIST :
			{
				MUID uidChar;

				pCommand->GetParameter( &uidChar, 0, MPT_UID );

				OnRequestCharQuestItemList( uidChar );
			}
			break;

		case MC_MATCH_REQUEST_DROP_SACRIFICE_ITEM :
			{
				// MUID	uidQuestItemOwner;
				int		nSlotIndex;
				int		nItemID;

				// pCommand->GetParameter( &uidQuestItemOwner, 0, MPT_UID );
				pCommand->GetParameter( &nSlotIndex, 1, MPT_INT );
				pCommand->GetParameter( &nItemID, 2, MPT_INT );

				OnRequestDropSacrificeItemOnSlot( pCommand->GetSenderUID(), nSlotIndex, nItemID );
			}
			break;

		case MC_MATCH_REQUEST_CALLBACK_SACRIFICE_ITEM :
			{
				MUID uidPlayer = pCommand->GetSenderUID();
				int		nSlotIndex;
				int		nItemID;

				//pCommand->GetParameter( &uidPlayer, 0, MPT_UID );
				pCommand->GetParameter( &nSlotIndex, 1, MPT_INT );
				pCommand->GetParameter( &nItemID, 2, MPT_INT );

				OnRequestCallbackSacrificeItem( uidPlayer, nSlotIndex, nItemID );
			}
			break;

		case MC_MATCH_REQUEST_BUY_QUEST_ITEM :
			{
				int	nItemID;
				int	nItemCount;

				pCommand->GetParameter( &nItemID, 1, MPT_INT );
				pCommand->GetParameter( &nItemCount, 2, MPT_INT );

				OnRequestBuyQuestItem( pCommand->GetSenderUID(), nItemID, nItemCount );
			}
			break;

		case MC_MATCH_REQUEST_SELL_QUEST_ITEM :
			{
				int		nItemID;
				int		nCount;

				pCommand->GetParameter( &nItemID, 1, MPT_INT );
				pCommand->GetParameter( &nCount, 2, MPT_INT );

				OnRequestSellQuestItem( pCommand->GetSenderUID(), nItemID, nCount );
			}
			break;

		case MC_QUEST_REQUEST_QL :
			{
				// MUID uidSender;

				// pCommand->GetParameter( &uidSender, 0, MPT_UID );

				OnRequestQL( pCommand->GetSenderUID() );
			}
			break;

		case MC_MATCH_REQUEST_SLOT_INFO :
			{
				//MUID uidSender;

				//pCommand->GetParameter( &uidSender, 0, MPT_UID );

				OnRequestSacrificeSlotInfo( pCommand->GetSenderUID() );
			}
			break;

		case MC_QUEST_STAGE_MAPSET :
			{
				MUID uidStage;
				char nMapsetID;

				pCommand->GetParameter( &uidStage,	0, MPT_UID );
				pCommand->GetParameter( &nMapsetID, 1, MPT_CHAR );

				OnQuestStageMapset(uidStage, (int)nMapsetID );
			}
			break;
		case MC_MATCH_REQUEST_MONSTER_BIBLE_INFO :
			{
				//MUID uidSender;

				//pCommand->GetParameter( &uidSender, 0, MPT_UID );

				// Custom: Exploit fix (UID spoofing)
				OnRequestMonsterBibleInfo( pCommand->GetSenderUID() );
			}
			break;

		case MC_RESPONSE_XTRAP_HASHVALUE :
			{
			}
			break;

		case MC_RESPONSE_RESOURCE_CRC32 :
			{
				DWORD dwResourceXORCache = 0;
				DWORD dwResourceCRC32Cache = 0;

				pCommand->GetParameter( &dwResourceCRC32Cache, 0, MPT_UINT );
				pCommand->GetParameter( &dwResourceXORCache, 1, MPT_UINT );

				_ASSERT( 0 != dwResourceXORCache );
				_ASSERT( 0 != dwResourceCRC32Cache );

				MMatchObject* pObj = GetObject( pCommand->GetSenderUID() );
				if( NULL == pObj )
				{
					return false;
				}

				MMatchStage* pStage = FindStage( pObj->GetStageUID() );
				if( NULL == pStage )
				{
					// À¯Àú¸¦ ·Îºñ³ª ¹æÀ¸·Î º¸³»ÁÖ¾ûÚß ÇÑ´Ù.
					_ASSERT( 0 );
					return false;
				}

				if( !pStage->IsValidResourceCRC32Cache(pCommand->GetSenderUID(), dwResourceCRC32Cache, dwResourceXORCache) )
				{
					// À¯Àú¸¦ ·Îºñ³ª ¹æÀ¸·Î º¸³»ÁÖ¾ûÚß ÇÑ´Ù.
					StageLeave( pObj->GetUID());//, pObj->GetStageUID() );
					return false;
				}
			}
			break;


		///< È«±âÁÖ(2009.08.04)
		///< CommandBuilder ´Ü¿¡¼­ ¸¸µEFlooding °EÃ À¯Àú¿¡ ´EÑ Command Ã³¸®ÀÌ´Ù.
		///< ÀÌ ºÎºÐ¿¡ Handler¸¦ µÎ±â°¡ Á» ½È±EÇÏÁö¸¸.. ¾ûÞ¿ ¼E¾øÀÌ.. ÀÏ´ÜÀº Ã³¸®ÇÏÀÚ!
		///< ¹«Á¶°Ç MC_NET_CLEARº¸´Ù ¸ÕÀE½ÇÇàµÇ¾ûÚß Abuse¸¦ ½ÃÅ³ ¼EÀÖ´Ù.		
		case MC_NET_BANPLAYER_FLOODING :
			{
				MUID uidPlayer;
				
				pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
				if (MGetServerConfig()->IsUseBlockFlooding())
				{
					// Custom: Exploit fix
					if (pCommand->GetSenderUID() != this->GetUID())
					{
						break;
					}

					MMatchObject* pObj = GetObject( uidPlayer );
					if( pObj && pObj->GetDisconnStatusInfo().GetStatus() == MMDS_CONNECTED)
					{
						if( pObj->GetAccountName() ) {
							LOG(LOG_PROG,"Ban Player On Flooding - (MUID:%d%d, ID:%s)"
								, uidPlayer.High, uidPlayer.Low, pObj->GetAccountName());
						} else {
							LOG(LOG_PROG,"Ban Player On Flooding - (MUID:%d%d, ID:%s)"
								, uidPlayer.High, uidPlayer.Low);
						}
						
						pObj->DisconnectHacker( MMHT_COMMAND_FLOODING );
					} 
					else 
					{
						LOG(LOG_PROG,"Ban Player On Flooding - Can't Find Object");
					}
				}
			}
			break;
		///< ¿©±â±ûÝE

		case MC_MATCH_DUELTOURNAMENT_REQUEST_JOINGAME :
			{
				// Custom: Exploit fix (UID spoofing)
				MUID uidPlayer = pCommand->GetSenderUID();
				MDUELTOURNAMENTTYPE nType;

				//pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
				pCommand->GetParameter(&nType, 1, MPT_INT);

				if( MGetServerConfig()->IsEnabledDuelTournament() )	{
                    ResponseDuelTournamentJoinChallenge(uidPlayer, nType);
				}

			}
			break;

		case MC_MATCH_DUELTOURNAMENT_REQUEST_CANCELGAME :
			{
				// Custom: Exploit fix (UID spoofing)
				MUID uidPlayer = pCommand->GetSenderUID();
				MDUELTOURNAMENTTYPE nType;

				//pCommand->GetParameter(&uidPlayer, 0, MPT_UID);
				pCommand->GetParameter(&nType, 1, MPT_INT);

				if( MGetServerConfig()->IsEnabledDuelTournament() )	{
					ResponseDuelTournamentCancelChallenge(uidPlayer, nType);
				}
			}
			break;

		case MC_MATCH_DUELTOURNAMENT_REQUEST_SIDERANKING_INFO :
			{
				// Custom: Exploit fix (UID spoofing)
				MUID uidPlayer = pCommand->GetSenderUID();

				//pCommand->GetParameter(&uidPlayer, 0, MPT_UID);

				if( MGetServerConfig()->IsEnabledDuelTournament() ){
					ResponseDuelTournamentCharSideRanking(uidPlayer);
				}
			}
			break;
 
		case MC_MATCH_DUELTOURNAMENT_GAME_PLAYER_STATUS :
			{
				// Custom: Exploit fix (UID spoofing)
				MUID uidPlayer = pCommand->GetSenderUID();

				//pCommand->GetParameter(&uidPlayer, 0, MPT_UID);

				if( MGetServerConfig()->IsEnabledDuelTournament() ){
					ResponseDuelTournamentCharStatusInfo(uidPlayer, pCommand);
				}
			}
			break;

#ifdef _CW_VOTE
		case MC_MATCH_VOTE:
		{
			MMatchObject* pObj = (MMatchObject*)GetObject(pCommand->GetSenderUID());
			if (pObj)
			{
				int Map;
				if (pCommand->GetParameter(&Map, 0, MPT_INT) == false) break;
				if (Map < 0 || Map > 2) return true;
				MMatchChannel* channel = MGetMatchServer()->FindChannel(pObj->GetChannelUID());
				if (channel && channel->GetChannelType() == MCHANNEL_TYPE_LADDER)
					GetLadderWarsMgr()->UpdatePlayerVote(Map, pObj);
				else
					GetLadderMgr()->UpdatePlayerVote(Map, pObj);
			}
		}
		break;
			case MC_MATCH_LADDERWARS_FRIENDACCEPT:
				{
					MMatchObject* pObj = (MMatchObject*)GetObject(pCommand->GetSenderUID());
					if (pObj)
					{
						MMatchObject* pFObj = (MMatchObject*)GetObject(pObj->LadderWarsFriend);
						if(pFObj && pObj->GetUID() == pFObj->LadderWarsFriend)
						{
							pFObj->PwState = MMatchObject::Inviter;
							pObj->PwState = MMatchObject::Invited;
							char mbuf[256], tbuf[256];
							sprintf(tbuf, "%s Has accepted your partnership.", pObj->GetCharInfo()->m_szName);
							sprintf(mbuf, "You are now partnered with %s.", pFObj->GetCharInfo()->m_szName);
							Announce(pFObj, tbuf);
							Announce(pFObj, "To leave type /duleave");
							Announce(pObj, mbuf);
							Announce(pObj, "To leave type /duleave");
						}
					}
				}
				break;
			case MC_MATCH_LADDERWARS_CHARINFO:
				{
					MMatchObject* pObj = (MMatchObject*)GetObject(pCommand->GetSenderUID());
					if (pObj)
					{
						MMatchObject* pFObj = (MMatchObject*)GetObject(pObj->LadderWarsFriend);
						if(pFObj && pObj->GetUID() == pFObj->LadderWarsFriend)
						{
							pFObj->PwState = MMatchObject::NotInvited;
							pFObj->LadderWarsFriend = MUID(0, 0);
							pObj->PwState = MMatchObject::NotInvited;
							pObj->LadderWarsFriend = MUID(0, 0);
							char mbuf[256], tbuf[256];
							sprintf(tbuf, "%s Has left your partnership.", pObj->GetCharInfo()->m_szName);
							sprintf(mbuf, "You have left your partnership with %s.", pFObj->GetCharInfo()->m_szName);
							Announce(pFObj, tbuf);
							Announce(pObj, mbuf);
						}
					}
				}
				break;
			case MC_MATCH_LADDERWARS_FRIENDINVITE:
				{
					MMatchObject* pObj = (MMatchObject*)GetObject(pCommand->GetSenderUID());
					if (pObj)
					{
						char szCharName[ MAX_CHARNAME_LENGTH ];
						pCommand->GetParameter(szCharName, 0, MPT_STR, MAX_CHARNAME_LENGTH );
						if ((strlen(szCharName)) < 2) return false;
						MMatchObject* pTargetObj = GetPlayerByName(szCharName);
						if(pTargetObj == NULL) return false;
						if (pTargetObj->PwState == MMatchObject::NotInvited)
						{
							pObj->LadderWarsFriend = pTargetObj->GetUID();
							pTargetObj->LadderWarsFriend = pObj->GetUID();
							char mbuf[256], tbuf[256];
							sprintf(tbuf, "%s Has invited you as a LadderWars partner.", pObj->GetCharInfo()->m_szName);
							sprintf(mbuf, "%s Has been invited to be your LadderWars partner.", pTargetObj->GetCharInfo()->m_szName);
							Announce(pTargetObj, tbuf);
							Announce(pTargetObj, "To accept type /duaccept");
							Announce(pObj, mbuf);

						}
						else
						{
							char tbuf[256];
							sprintf(tbuf, "%s Is already partnered up or is not found.", pTargetObj->GetCharInfo()->m_szName);
							Announce(pObj, tbuf);
						}
					}
			}
			break;
			case MC_MATCH_JOIN_LADDERWARS:
			{
				MMatchObject* pObj = (MMatchObject*)GetObject(pCommand->GetSenderUID());
				if (pObj)
				{

					int Arg1, Arg2;
					if (pCommand->GetParameter(&Arg1, 0, MPT_INT) == false) break;
					if (pCommand->GetParameter(&Arg2, 1, MPT_INT) == false) break;
					if (Arg2 <= -1 || Arg2 >= MPLADDERWARSTYPE_MAX) break;
					bool Arg3 = false;
					if (Arg1 == 1) Arg3 = true;
						bool DoesExist = false;
						for (int i = 0; i < MPLADDERWARSTYPE_MAX; i++)
							if (GetLadderWarsMgr()->GetWaitingGames((LadderWars)i)->IsPlayerAlreadyQueued(pObj->GetUID()) == true)
								DoesExist = true;
						if (DoesExist == true)
							return true;
						else
						{
							GetLadderWarsMgr()->GetWaitingGames((LadderWars)Arg2)->AddPlayer(pObj->GetUID());
							if (pObj->PwState == MMatchObject::Inviter)
							{
								MMatchObject* pFObj = (MMatchObject*)GetObject(pObj->LadderWarsFriend);
								if (!pFObj)
								{
									pObj->PwState = MMatchObject::NotInvited;
									pObj->LadderWarsFriend = MUID(0, 0);
									return true;
								}
								MMatchChannel* chan = FindChannel(pFObj->GetChannelUID());
								if (chan && chan->GetChannelType() == MCHANNEL_TYPE_LADDER)
								{
								bool DoesExist = false;
								for (int i = 0; i < MPLADDERWARSTYPE_MAX; i++)
									if (GetLadderWarsMgr()->GetWaitingGames((LadderWars)i)->IsPlayerAlreadyQueued(pFObj->GetUID()) == true)
										DoesExist = true;
								if (DoesExist == false)
								{
									MCommand* pCommand = MMatchServer::GetInstance()->CreateCommand(MC_MATCH_LADDERWARS_INVITED, MUID(0, 0));
									MMatchServer::GetInstance()->RouteToListener(pFObj, pCommand);
									char buf[256];
									sprintf(buf, "%s Has invited you.", pObj->GetCharInfo()->m_szName);
									Announce(pFObj, buf);
									GetLadderWarsMgr()->GetWaitingGames((LadderWars)Arg2)->AddPlayer(pFObj->GetUID());
								}
							}
						}
					}
				}
			}	
			break;

#endif

		case MC_MATCH_REQUEST_USE_SPENDABLE_NORMAL_ITEM :
			{
				MUID uidItem, uidPlayer;

				uidPlayer = pCommand->GetSenderUID();
				pCommand->GetParameter(&uidItem, 0, MPT_UID);

				OnRequestUseSpendableNormalItem(uidPlayer, uidItem);			
			}
			break;
			//Banderas
#ifdef _COUNTRYFLAG
		case MC_GUNZ_SEND_COUNTRYCODE:
		{
			char szPlayerName[20] = { 0 };
			int nCountryCode = 0;

			pCommand->GetParameter(szPlayerName, 0, MPT_STR, 20);
			pCommand->GetParameter(&nCountryCode, 1, MPT_INT);

			OnSendCountryCode(szPlayerName, nCountryCode);
		}
		break;
#endif
		case MC_MATCH_REQUEST_USE_SPENDABLE_BUFF_ITEM :
			{
				MUID uidItem;
				pCommand->GetParameter(&uidItem, 0, MPT_UID);

				//¹öÇÁÁ¤º¸ÀÓ½ÃÁÖ¼® OnRequestUseSpendableBuffItem(pCommand->GetSenderUID(), uidItem);
				_ASSERT(0);//ÁÖ¼®Ã³¸®ÇßÀ¸´Ï ¿©±Eµé¾ûÛÀÁE¾Ê´Â °ÍÀ¸·Î °¡Á¤ÇÔ
			}
			break;
#ifdef _REPORT
		case MC_REPORT_USER:
		{
			char szName[64] = { 0 };
			char szReason[64] = { 0 };

			pCommand->GetParameter(szName, 0, MPT_STR, sizeof(szName));
			pCommand->GetParameter(szReason, 1, MPT_STR, sizeof(szReason));

			OnReport(pCommand->GetSenderUID(), szName, szReason);
		}
		break;
#endif
		case MC_ADMIN_GIVE_COINSCASH:
		{
			char szPlayerName[20] = { 0 };
			int nCash = 0;

			pCommand->GetParameter(szPlayerName, 0, MPT_STR, 20);
			pCommand->GetParameter(&nCash, 1, MPT_INT);

			OnAdminGiveCoinsCash(pCommand->GetSenderUID(), szPlayerName, nCash);
		}
		break;
#ifdef _GRADECHANGE
		case MC_UGRADEID_SEND:
		{
			char szPlayerName[20] = { 0 };
			int nUGradeID = 0;

			pCommand->GetParameter(szPlayerName, 0, MPT_STR, 20);
			pCommand->GetParameter(&nUGradeID, 1, MPT_INT);

			GetGradeChange(pCommand->GetSenderUID(), szPlayerName, nUGradeID);
		}
		break;
#endif
		case MC_ADMIN_GIVE_COINSMEDALS:
		{
			char szPlayerName[20] = { 0 };
			int nLC = 0;

			pCommand->GetParameter(szPlayerName, 0, MPT_STR, 20);
			pCommand->GetParameter(&nLC, 1, MPT_INT);

			OnAdminGiveCoinsMedals(pCommand->GetSenderUID(), szPlayerName, nLC);
		}
		break;

		//Custom: blitz
		case MC_BLITZ_REQUEST_CLASS:
		case MC_BLITZ_REQUEST_UPGRADE:
		{
			MUID sender = pCommand->GetSenderUID();
			MMatchObject* player = GetObject(sender);
			if (!IsEnabledObject(player))
				break;

			MMatchStage* pStage = FindStage(player->GetStageUID());
			if (!pStage)
			{
				_ASSERT(0); break;
			}

			pStage->OnCommand(pCommand);

		}break;

		case MC_NEWQUEST_REQUEST_NPC_DEAD:
			{
				MUID uidSender = pCommand->GetSenderUID();
				MMatchObject* pSender = GetObject(uidSender);
				if (!IsEnabledObject(pSender)) break;

				MMatchStage* pStage = FindStage(pSender->GetStageUID());
				if (!pStage)
				{
					_ASSERT(0); break;
				}

				pStage->OnCommand(pCommand);
			}
			break;

		case MC_NEWQUEST_REQUEST_NPC_SPAWN:
		{
			MUID uidSender = pCommand->GetSenderUID();
			MMatchObject* pSender = GetObject(uidSender);
			if (!IsEnabledObject(pSender)) break;

			MMatchStage* pStage = FindStage(pSender->GetStageUID());
			if (!pStage)
			{
				_ASSERT(0); break;
			}

			pStage->OnCommand(pCommand);
		}
		break;

		case MC_ADMIN_SUMMON:
			{
				MUID uidChar;
				char szName[128];

				pCommand->GetParameter( &uidChar, 0, MPT_UID );
				pCommand->GetParameter( szName, 1, MPT_STR, 128 );

				OnAdminSummon( pCommand->GetSenderUID(), szName );
			}
			break;
		case MC_ADMIN_GOTO:
			{
				MUID uidChar;
				char szName[128];

				pCommand->GetParameter( &uidChar, 0, MPT_UID );
				pCommand->GetParameter( szName, 1, MPT_STR, 128 );

				OnAdminGoTo( pCommand->GetSenderUID(), szName );
			}
			break;

		case MC_ADMIN_SLAP:
			{
				MUID uidChar;
				char szName[128];

				pCommand->GetParameter( &uidChar, 0, MPT_UID );
				pCommand->GetParameter( szName, 1, MPT_STR, 128 );

				OnAdminSlap( pCommand->GetSenderUID(), szName );
			}
			break;

		case MC_ADMIN_SPAWN:
			{
				MUID uidChar;
				char szName[128];

				pCommand->GetParameter( &uidChar, 0, MPT_UID );
				pCommand->GetParameter( szName, 1, MPT_STR, 128 );

				OnAdminSpawn( pCommand->GetSenderUID(), szName );
			}
			break;

		case MC_ADMIN_PKICK:
			{
				MUID uidChar;
				char szName[128];
				bool bBan = true;

				pCommand->GetParameter( &uidChar, 0, MPT_UID );
				pCommand->GetParameter( szName, 1, MPT_STR, 128 );
				pCommand->GetParameter( &bBan, 2, MPT_BOOL );

				OnAdminPKick( pCommand->GetSenderUID(), szName, bBan );
			}
			break;

		case MC_ADMIN_SILENCE:
			{
				MUID uidChar;
				int nDurationMin;

				pCommand->GetParameter( &uidChar, 0, MPT_UID );
				pCommand->GetParameter( &nDurationMin, 1, MPT_INT );

				OnAdminSilence( pCommand->GetSenderUID(), nDurationMin );
			}
			break;

		case MC_ADMIN_NAT:
			{
				OnAdminNAT( pCommand->GetSenderUID() );
			}
			break;

		case MC_MATCH_REQUEST_ROLL:
			{
				int checksumInt;
				MUID checksumUID;
				pCommand->GetParameter(&checksumInt , 1, MPT_INT);
				pCommand->GetParameter(&checksumUID, 0, MPT_UID);
				if(pCommand->GetSenderUID() != checksumUID) /*Insert Autoban here*/ break;
				
				if(checksumInt == 2)
					OnRequestRoll(pCommand->GetSenderUID());
				else
					/*Insert Autoban here*/;

			}
			break;

#ifdef _REJOIN_NEW
		case MC_MATCH_STAGE_REQUEST_REJOIN:
		{
			MMatchObject* pObj = (MMatchObject*)GetObject(pCommand->GetSenderUID());
			if (pObj)
			{
				ClanReDef::iterator it = ClanRejoiner.find(pObj->GetCharInfo()->m_nCID);
				if (it != ClanRejoiner.end())
				{
					if (!StageJoin(pObj->GetUID(), it->second->StageUID, true, it->second->Team))
					{
						NotifyMessage(pObj->GetUID(), MATCHNOTIFY_STAGE_NOT_EXIST);
					}
				}
			}
		}
		break;
#endif

#ifdef _NOLEAD
		case MC_PEER_NOLEAD:
		{
           //Custom: none
		}
		break;

		case MC_PEER_NOLEADBLOB:
		{
			//Custom: none
		}
		break;
#endif
		case MC_MATCH_REQUEST_FLAG_CAP:
			{
				int nItemID;

				pCommand->GetParameter( &nItemID, 0, MPT_INT );

				OnRequestFlagCap( pCommand->GetSenderUID(), nItemID );
			}
			break;

		case MC_ADMIN_CRASH:
		{
			OnCrash(pCommand->GetSenderUID());
			break;
		}

		case MC_ADMIN_TEAM:
			{
				int nTeam;

				pCommand->GetParameter( &nTeam, 1, MPT_INT );
				
				OnAdminTeam( pCommand->GetSenderUID(), nTeam );
			}
			break;

		case MC_ADMIN_SBTEST:
		{
			MUID uidChar;
			char szName[128];

			pCommand->GetParameter(&uidChar, 0, MPT_UID);
			pCommand->GetParameter(szName, 1, MPT_STR, 128);

			OnAdminSBTest(pCommand->GetSenderUID(), szName);
		}
		break;

		case MC_ADMIN_KICKALL:
		{
			OnAdminKickAll(pCommand->GetSenderUID());
		}
		break;

#ifdef _SPYMODE
		case MC_SPY_STAGE_REQUEST_BAN_MAP_LIST:
		{
			OnSpyStageRequestBanMapList(pCommand->GetSenderUID());
		}
		break;

		case MC_SPY_STAGE_ACTIVATE_MAP:
		{
			int nMapID;
			bool bExclude;

			pCommand->GetParameter(&nMapID, 0, MPT_INT);
			pCommand->GetParameter(&bExclude, 1, MPT_BOOL);

			OnSpyStageActivateMap(pCommand->GetSenderUID(), nMapID, bExclude);
		}
		break;
		case MC_MATCH_FREEZESPY:
		case MC_MATCH_UNFREEZESPY:
		{
			MMatchObject* pObj = (MMatchObject*)GetObject(pCommand->GetSenderUID());
			if (pObj && IsAdminGrade(pObj))
			{
				int Type = 0;
				if (pCommand->GetParameter(&Type, 0, MPT_INT) == false) break;
				switch (Type)
				{
				case 0:
				{
					char szCharName[MAX_CHARNAME_LENGTH];
					pCommand->GetParameter(szCharName, 1, MPT_STR, MAX_CHARNAME_LENGTH);
					if ((strlen(szCharName)) < 2) return false;
					MMatchObject* pTargetObj = GetPlayerByName(szCharName);
					if (pTargetObj == NULL) return false;
					MCommand* pCmd = CreateCommand(pCommand->GetID(), MUID(0, 0));
					pCmd->AddParameter(new MCmdParamInt(0));
					pCmd->AddParameter(new MCmdParamStr(""));
					RouteToListener(pTargetObj, pCmd);
					char msg[150];
					sprintf(msg, "You have %s %s", pCommand->GetID() == MC_MATCH_FREEZESPY ? "Frozen" : "Unfrozen", szCharName);
					Announce(pObj, msg);
				}
				break;
				case 1:
				{
					MCommand* pCmd = CreateCommand(pCommand->GetID(), MUID(0, 0));
					pCmd->AddParameter(new MCmdParamInt(0));
					pCmd->AddParameter(new MCmdParamStr(""));
					MMatchTeam t = pObj->GetTeam() == MMT_RED ? MMT_BLUE : MMT_RED;
					RouteTeamChatToStage(pObj->GetStageUID(), pCmd, t, true);
					char msg[150];
					sprintf(msg, "You have %s the %s team", pCommand->GetID() == MC_MATCH_FREEZESPY ? "Frozen" : "Unfrozen", t == MMT_BLUE ? "Blue" : "Red");
					Announce(pObj, msg);
				}
				break;
				}
			}
		}
		break;
		case MC_SPY_STAGE_REQUEST_START:
		{
			MCommandParameter* pParam = pCommand->GetParameter(0);
			if ((!pParam) || (pParam->GetType() != MPT_BLOB) || (pParam->GetSize() < (sizeof(int) * 2)))
			{
				_ASSERT(0);
				break;
			}

			void* pBlob = pParam->GetPointer();
			if (MGetBlobArraySize(pBlob) != (pParam->GetSize() - sizeof(int)))
			{
				_ASSERT(0);
				break;
			}

			OnSpyStageStart(pCommand->GetSenderUID(), pBlob);
		}
		break;
#ifdef _PLAYERINO
		case MC_MATCH_INFORMATION:
		{
			char szTargetName[MAX_CHARNAME_LENGTH];
			pCommand->GetParameter(szTargetName, 0, MPT_STR, MAX_CHARNAME_LENGTH);

			MMatchObject* pPlayerObj = GetObject(pCommand->GetSenderUID());
			if (pPlayerObj == NULL) break;

			MMatchObject* pTargetObj = GetPlayerByName(szTargetName);
			if (pTargetObj == NULL) break;



			MCommand* pNew = CreateCommand(MC_MATCH_INFORMATION_RECEIVE, MUID(0, 0));
			void* pInformationFrame = MMakeBlobArray(sizeof(MTD_Information), 1);
			MTD_Information* pInformation = (MTD_Information*)MGetBlobArrayElement(pInformationFrame, 0);

			strcpy(pInformation->szName, pTargetObj->GetCharInfo()->m_szName);
			pInformation->nLevel = pTargetObj->GetCharInfo()->m_nLevel;
			pInformation->nBounty = pTargetObj->GetCharInfo()->m_nBP;
			pInformation->nLC = pTargetObj->GetCharInfo()->m_nLC;
			pInformation->nBP = pTargetObj->GetCharInfo()->m_nBP;
			pInformation->nEXP = pTargetObj->GetCharInfo()->m_nXP;
			pInformation->nFace = pTargetObj->GetCharInfo()->m_nFace;
			pInformation->nHair = pTargetObj->GetCharInfo()->m_nHair;
			pInformation->nSex = pTargetObj->GetCharInfo()->m_nSex;
			pInformation->uidTarget = pTargetObj->GetUID();
			strcpy(pInformation->szClan, pTargetObj->GetCharInfo()->m_ClanInfo.m_szClanName);
			pInformation->nClanEmblem = pTargetObj->GetCharInfo()->m_ClanInfo.m_nClanID;
			pInformation->nKill = pTargetObj->GetCharInfo()->m_nTotalKillCount;
			pInformation->nDeath = pTargetObj->GetCharInfo()->m_nTotalDeathCount;

			pNew->AddParameter(new MCommandParameterBlob(pInformationFrame, MGetBlobArraySize(pInformationFrame)));
			MEraseBlobArray(pInformationFrame);
			RouteToListener(pPlayerObj, pNew);


			MCommand* pTest = CreateCommand(MC_MATCH_INFORMATION_HPAP, MUID(0, 0));
			pTest->AddParameter(new MCommandParameterString(pPlayerObj->GetCharInfo()->m_szName));
			RouteToListener(pTargetObj, pTest);

			pTest = CreateCommand(MC_MATCH_INFORMATION_CLOTHES, MUID(0, 0));
			pTest->AddParameter(new MCommandParameterString(pPlayerObj->GetCharInfo()->m_szName));
			RouteToListener(pTargetObj, pTest);

			}
		break;

		case MC_MATCH_INFORMATION_HPAP_GO:
		{
			static char szTarget[CHAT_STRING_LEN];
			int nHP, nAP;
			pCommand->GetParameter(szTarget, 0, MPT_STR, CHAT_STRING_LEN);
			pCommand->GetParameter(&nHP, 1, MPT_INT);
			pCommand->GetParameter(&nAP, 2, MPT_INT);

			MMatchObject* pTargetObj = GetPlayerByName(szTarget);
			if (pTargetObj == NULL) break;

			MCommand* pNew = CreateCommand(MC_MATCH_INFORMATION_HPAP_GO, MUID(0, 0));
			pNew->AddParameter(new MCommandParameterString(szTarget));
			pNew->AddParameter(new MCommandParameterInt(nHP));
			pNew->AddParameter(new MCommandParameterInt(nAP));
			RouteToListener(pTargetObj, pNew);

		}
		break;

		case MC_MATCH_INFORMATION_CLOTHES_GO:
		{
			static char szTarget[CHAT_STRING_LEN];
			pCommand->GetParameter(szTarget, 0, MPT_STR, CHAT_STRING_LEN);


			MMatchObject* pTargetObj = GetPlayerByName(szTarget);
			if (pTargetObj == NULL) break;


			MCommandParameter* pParam = pCommand->GetParameter(1);
			if (pParam->GetType() != MPT_BLOB) break;
			void* pBlob = pParam->GetPointer();
			int nCount = MGetBlobArrayCount(pBlob);
			MTD_InformationClothes* pNode = (MTD_InformationClothes*)MGetBlobArrayElement(pBlob, 0);


			MCommand* pNew = CreateCommand(MC_MATCH_INFORMATION_CLOTHES_GO, MUID(0, 0));
			void* pInformationFrame = MMakeBlobArray(sizeof(MTD_InformationClothes), 1);
			MTD_InformationClothes* pInformation = (MTD_InformationClothes*)MGetBlobArrayElement(pInformationFrame, 0);

			char szTest[123];
			for (int i = 0; i < MMCIP_END; i++)
			{
				pInformation->nEquipedItemID[i] = pNode->nEquipedItemID[i];
			}
			pInformation->nFace = pNode->nFace;
			pInformation->nHair = pNode->nHair;
			pInformation->nSex = pNode->nSex;

			pNew->AddParameter(new MCommandParameterString(szTarget));
			pNew->AddParameter(new MCommandParameterBlob(pInformationFrame, MGetBlobArraySize(pInformationFrame)));
			MEraseBlobArray(pInformationFrame);
			RouteToListener(pTargetObj, pNew);

		}
		break;
#endif
		case MC_MATCH_UPDATE_SKILLMAP_BESTTIME:
		{
			MUID player;
			int mapName;
			unsigned int bestTime;
			pCommand->GetParameter(&player, 0, MPT_UID);
			pCommand->GetParameter(&mapName, 1, MPT_INT);
			pCommand->GetParameter(&bestTime, 2, MPT_UINT);

			RequestUpdateSkillMapBestTime(player, mapName, bestTime);
		}break;
		case MC_MATCH_REQUEST_SKILLMAP_BESTTIME:
		{
			MUID player;
			char mapName[24];
			pCommand->GetParameter(&player, 0, MPT_UID);
			pCommand->GetParameter(&mapName, 1, MPT_STR, 24);

			ResponseSkillMapBestTime(player, mapName);
		}break;

		default:
			//			//_ASSERT(0);	// ¾ÆÁ÷ ÇÚµé·¯°¡ ¾ø´Ù.
			return false;
	}
#endif
_STATUS_CMD_END;


	return true;
}