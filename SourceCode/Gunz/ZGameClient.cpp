#include "stdafx.h"

#include <winsock2.h>
#include "MErrorTable.h"
#include "ZConfiguration.h"
#include "ZGameClient.h"
#include "MSharedCommandTable.h"
#include "ZConsole.h"
#include "MCommandLogFrame.h"
#include "ZIDLResource.h"
#include "MBlobArray.h"
#include "ZInterface.h"
#include "ZApplication.h"
#include "ZGameInterface.h"
#include "MMatchGlobal.h"
#include "MMatchChannel.h"
#include "MMatchStage.h"
#include "ZCommandTable.h"
#include "ZPost.h"
#include "ZPostLocal.h"
#include "MMatchNotify.h"
#include "ZMatch.h"
#include "MComboBox.h"
#include "MTextArea.h"
#include "ZCharacterViewList.h"
#include "ZCharacterView.h"
#include "MDebug.h"
#include "ZScreenEffectManager.h"
#include "ZNetmarble.h"
#include "ZRoomListBox.h"
#include "ZPlayerListBox.h"
#include "ZChat.h"
#include "ZWorldItem.h"
#include "ZChannelRule.h"
#include "ZNetRepository.h"
#include "ZMyInfo.h"
#include "MToolTip.h"
#include "ZColorTable.h"
#include "ZClan.h"
#include "ZSecurity.h"
#include "ZItemDesc.h"
#include "ZCharacterSelectView.h"
#include "ZChannelListItem.h"
#include "ZCombatInterface.h"
#include "ZLocale.h"
#include "ZMap.h"
#include "UPnP.h"
#include "MMD5.h"
#include "ZPlayerManager.h"
#include "DiskSerialNumber.h"
#include <Psapi.h>
#include "MEventInfo.h"

#ifdef _MAGICBOX
#include "ZRuleDropGunGame.h"
#endif

#ifdef _XTRAP											// Update sgk 0760 start
#include "./XTrap/Xtrap_C_Interface.h"
#pragma comment (lib, "XTrap/XTrap4Client_mt.lib")
#endif													// Update sgk 0760 end

// Custom: Disable NHN auth
//#ifdef LOCALE_NHNUSA
//#include "ZNHN_USA.h"
//#include "ZNHN_USA_Report.h"
//#endif

#ifdef _GAMEGUARD
#include "ZGameGuard.h"
#endif


// Custom: Disk serial number
//CDiskSerialNumber g_HDD;

bool GetUserGradeIDColor(MMatchUserGradeID gid, MCOLOR& UserNameColor, char* sp_name);

MCommand* ZNewCmd(int nID)
{
	MCommandDesc* pCmdDesc = ZGetGameClient()->GetCommandManager()->GetCommandDescByID(nID);

	MUID uidTarget;
	if (pCmdDesc->IsFlag(MCDT_PEER2PEER) == true)
		uidTarget = MUID(0, 0);
	else
		uidTarget = ZGetGameClient()->GetServerUID();

	MCommand* pCmd = new MCommand(nID,
		ZGetGameClient()->GetUID(),
		uidTarget,
		ZGetGameClient()->GetCommandManager());
	return pCmd;
}

bool GetUserInfoUID(MUID uid, MCOLOR& _color, char* sp_name, MMatchUserGradeID& gid)
{
	if (ZGetGameClient() == NULL)
		return false;

	//	MMatchUserGradeID gid = MMUG_FREE;

	MMatchObjCache* pObjCache = ZGetGameClient()->FindObjCache(uid);

	if (pObjCache) {
		gid = pObjCache->GetUGrade();
	}

	return GetUserGradeIDColor(gid, _color, sp_name);
}


extern MCommandLogFrame* m_pLogFrame;
extern ZIDLResource	g_IDLResource;



bool ZPostCommand(MCommand* pCmd)
{
	// Replay �߿��� �Ʒ� ������ Ŀ�ǵ常 ��E� �㿁E
	if (ZGetGame() && ZGetGame()->IsReplay())
	{
		switch (pCmd->GetID())
		{
		case MC_CLOCK_SYNCHRONIZE:
		case MC_MATCH_USER_WHISPER:
		case MC_MATCH_CHATROOM_JOIN:
		case MC_MATCH_CHATROOM_LEAVE:
		case MC_MATCH_CHATROOM_SELECT_WRITE:
		case MC_MATCH_CHATROOM_INVITE:
		case MC_MATCH_CHATROOM_CHAT:
		case MC_MATCH_CLAN_MSG:
		case MC_HSHIELD_PONG:
		case MC_RESPONSE_XTRAP_HASHVALUE:
		case MC_RESPONSE_GAMEGUARD_AUTH:
		case MC_RESPONSE_XTRAP_SEEDKEY:
			break;
		default:
			delete pCmd;
#ifdef _PUBLISH
			CHECK_RETURN_CALLSTACK(ZPostCommand);
#endif
			return false;
		};
#ifdef _PUBLISH
		CHECK_RETURN_CALLSTACK(ZPostCommand);
#endif
		return ZGetGameClient()->Post(pCmd);
	}
	else
	{
		// ���� ��EĿ�ǵ�E��E� �㿁E
#ifdef _PUBLISH
		CHECK_RETURN_CALLSTACK(ZPostCommand);
#endif
		bool bRet = ZGetGameClient()->Post(pCmd);

		// �̱��� ��E� ���ν����忡�� �������� �ϴ� Ŀ�ǵ���� ������E�˻�E
		// Post()����E� �˻��ϸ�EPost()�� �ٷ� ������ ��E�����Ƿ� �ڿ��� �ξ��� �Ѵ�
		// (�׷��� ZGameClient::Post()�� �ٷ� ȣ���ع�����E�Ʒ� ����ڵ尡 �ҿ�E��� �ȴ�..�׶� ���� ��E��ؾ� �Ѵ�.)
		int cmdId = pCmd->GetID();
		if (cmdId == MC_ADMIN_ANNOUNCE ||
			cmdId == MC_ADMIN_REQUEST_SERVER_INFO ||
			cmdId == MC_ADMIN_SERVER_HALT ||
			cmdId == MC_ADMIN_REQUEST_UPDATE_ACCOUNT_UGRADE ||
			cmdId == MC_ADMIN_REQUEST_KICK_PLAYER ||
			cmdId == MC_ADMIN_REQUEST_MUTE_PLAYER ||
			cmdId == MC_ADMIN_REQUEST_BLOCK_PLAYER ||
			cmdId == MC_ADMIN_PING_TO_ALL ||
			cmdId == MC_MATCH_LADDERWARS_FRIENDINVITE ||
			cmdId == MC_MATCH_LADDERWARS_FRIENDACCEPT ||
			cmdId == MC_MATCH_LADDERWARS_FRIENDLEAVE ||
			cmdId == MC_ADMIN_REQUEST_SWITCH_LADDER_GAME ||
			cmdId == MC_ADMIN_HIDE ||
#ifdef _SPEC 1
			cmdId == MC_MATCH_REQUEST_SPEC ||
			cmdId == MC_MATCH_RESPONSE_SPEC ||
			cmdId == MC_PEER_SPEC ||
			cmdId == MC_PEER_MOVE_DELTA ||
#endif
			cmdId == MC_ADMIN_RESET_ALL_HACKING_BLOCK ||
			cmdId == MC_ADMIN_RELOAD_GAMBLEITEM ||
			cmdId == MC_ADMIN_DUMP_GAMBLEITEM_LOG ||
			cmdId == MC_ADMIN_ASSASIN ||

			cmdId == MC_GUNZ_LASTDMG ||
			cmdId == MC_GUNZ_DMGGIVEN ||
#ifdef _VOICE_CHAT
			cmdId == MC_MATCH_SEND_VOICE_CHAT ||
			cmdId == MC_MATCH_RECEIVE_VOICE_CHAT ||
#endif
			cmdId == MC_MATCH_GAME_KILL ||
			cmdId == MC_MATCH_GAME_REQUEST_SPAWN ||

			cmdId == MC_MATCH_REQUEST_SUICIDE ||
			cmdId == MC_MATCH_REQUEST_OBTAIN_WORLDITEM ||
			cmdId == MC_MATCH_REQUEST_SPAWN_WORLDITEM ||
			cmdId == MC_MATCH_SET_OBSERVER ||

			/*cmdId == MC_PEER_SET_SWORD_COLOR ||
			cmdId == MC_MATCH_REQUEST_SPEC ||
			cmdId == MC_MATCH_RESPONSE_SPEC ||
			cmdId == MC_PEER_SPEC ||
			cmdId == MC_PEER_MOVE_DELTA ||*/

			cmdId == MC_MATCH_REQUEST_CREATE_CHAR ||
			cmdId == MC_MATCH_REQUEST_DELETE_CHAR ||

			cmdId == MC_MATCH_REQUEST_BUY_ITEM ||
			cmdId == MC_MATCH_REQUEST_SELL_ITEM ||
			cmdId == MC_MATCH_REQUEST_EQUIP_ITEM ||
			cmdId == MC_MATCH_REQUEST_TAKEOFF_ITEM ||

			cmdId == MC_MATCH_REQUEST_GAMBLE ||

			cmdId == MC_QUEST_REQUEST_NPC_DEAD ||
			cmdId == MC_MATCH_QUEST_REQUEST_DEAD ||

			cmdId == MC_QUEST_PEER_NPC_BASICINFO ||
			cmdId == MC_QUEST_PEER_NPC_HPINFO ||
			cmdId == MC_QUEST_PEER_NPC_DEAD ||
			cmdId == MC_QUEST_PEER_NPC_BOSS_HPAP ||
			cmdId == MC_QUEST_REQUEST_MOVETO_PORTAL ||
			cmdId == MC_QUEST_TEST_REQUEST_NPC_SPAWN ||
			cmdId == MC_QUEST_TEST_REQUEST_CLEAR_NPC ||
			cmdId == MC_QUEST_TEST_REQUEST_SECTOR_CLEAR ||

#ifndef _PLAYERINO
			cmdId == MC_MATCH_INFORMATION ||
			cmdId == MC_MATCH_INFORMATION_RECEIVE ||
			cmdId == MC_MATCH_INFORMATION_HPAP ||
			cmdId == MC_MATCH_INFORMATION_HPAP_GO ||
			cmdId == MC_MATCH_INFORMATION_CLOTHES ||
			cmdId == MC_MATCH_INFORMATION_CLOTHES_GO ||
#endif

#ifdef _POSTABINFO
			cmdId == MC_PEER_HPAP_TAB ||
			cmdId == MC_PEER_DAMAGE_TAB ||
#endif
			cmdId == MC_QUEST_TEST_REQUEST_FINISH ||
			cmdId == MC_PEER_MOVE ||
			cmdId == MC_PEER_ATTACK ||
			cmdId == MC_PEER_DAMAGE ||
			cmdId == MC_PEER_SHOT ||
			cmdId == MC_PEER_SHOT_SP ||
			cmdId == MC_PEER_SKILL ||
			cmdId == MC_PEER_SHOT_MELEE ||
			cmdId == MC_PEER_DIE ||
			cmdId == MC_PEER_SPAWN ||
			cmdId == MC_PEER_DASH ||
#ifdef _CMD_ALL
			cmdId == MC_ALL_CMD ||
#endif
			cmdId == MC_PEER_CHAT ||

			// Custom Commands:
			cmdId == MC_PEER_VAMPIRE ||
#ifdef _LADDERWARSPACKETS
			cmdId == MC_PEER_LADDERWARS ||
#endif
			cmdId == MC_PEER_SNIFER ||
#ifdef _NOLEAD
			cmdId == MC_PEER_NOLEAD ||
#endif
			cmdId == MC_ADMIN_SUMMON ||
			cmdId == MC_ADMIN_GOTO ||
			cmdId == MC_ADMIN_SLAP ||
			cmdId == MC_ADMIN_GIVE_COINSMEDALS ||
			cmdId == MC_ADMIN_SPAWN)
#ifdef _ANTIHACK
		{
			extern DWORD g_dwMainThreadID;
			if (g_dwMainThreadID != GetCurrentThreadId())
			{
				_ASSERT(0);
				ZPostDisconnect();
			}
		}
		static unsigned long int DwCheckTime = timeGetTime();
		unsigned long int DwNowTime = timeGetTime();
		if (DwNowTime - DwCheckTime >= 15000)
			/*{
				if (WaitForSingleObject(ZGetApplication()->CheckThread[0], 0) == WAIT_OBJECT_0 ||
					SuspendThread(ZGetApplication()->CheckThread[0]) >= 1 ||
					WaitForSingleObject(ZGetApplication()->CheckThread[1], 0) == WAIT_OBJECT_0 ||
					SuspendThread(ZGetApplication()->CheckThread[1]) >= 1)*/
			if (WaitForSingleObject(AntiNoob, 0) == WAIT_OBJECT_0 || SuspendThread(AntiNoob) >= 1)
			{
				ZPostDisconnect();
				Sleep(5000);
				DWORD addressB = (DWORD)GetProcAddress(GetModuleHandle("ntdll"), "NtRaiseException");
				__asm
				{
					mov ESP, 0
					jmp dword ptr addressB
				};
				PostMessage(g_hWnd, WM_CLOSE, 0, 0);
				ZApplication::GetGameInterface()->ShowMessage("Illegal Action detected.");
			}
		//ResumeThread(ZGetApplication()->CheckThread[0]);
		//ResumeThread(ZGetApplication()->CheckThread[1]);
		ResumeThread(AntiNoob);
		DwCheckTime = DwNowTime;
	}
	//return true;
}

#else
{
#ifdef _PUBLISH
	extern DWORD g_dwMainThreadID;
	if (g_dwMainThreadID != GetCurrentThreadId())
	{
#ifdef _DEBUG
		mlog("CMD THREAD MISMATCH : cmdId(%d), mainId(%d), currId(%d)\n", cmdId, g_dwMainThreadID, GetCurrentThreadId());
#endif
		_ASSERT(0);
		MCommand* pC = ZNewCmd(MC_REQUEST_GIVE_ONESELF_UP);
		ZPostCommand(pC);
	}
#endif

	return bRet;
}
	}
	return true;
}
#endif
ZGameClient::ZGameClient() : MMatchClient(), m_pUPnP(NULL)
{
	m_pUPnP = new UPnP;
	//ShowDamage = false;
	m_uidPlayer = MUID(0, 0);
	m_nClockDistance = 0;
	m_fnOnCommandCallback = NULL;
	m_nPrevClockRequestAttribute = 0;
	m_nBridgePeerCount = 0;
	m_tmLastBridgePeer = 0;
	m_bForcedEntry = false;
	m_szChannel[0] = NULL;
	m_szStageName[0] = NULL;
	m_szChatRoomInvited[0] = NULL;
	SetChannelRuleName("");

	m_LobbyChat = false;
	m_bAdminNAT = false;
	m_nRoomNo = 0;
	m_nStageCursor = 0;
	m_bLadderWars = false;
	IsRejoin = false;

	m_nCountdown = 0;
	m_tmLastCountdown = 0;
	m_nRequestID = 0;
	m_uidRequestPlayer = MUID(0, 0);
	m_nProposalMode = MPROPOSAL_NONE;
	m_bLadderGame = false;

	m_CurrentChannelType = MCHANNEL_TYPE_PRESET;

	SetRejectWhisper(true);
	SetRejectInvite(true);

	SetVoteInProgress(false);
	SetCanVote(false);


	m_EmblemMgr.Create();
	m_EmblemMgr.PrepareCache();

	memset(&m_dtCharInfo, 0, sizeof(m_dtCharInfo));
	memset(&m_dtCharInfoPrev, 0, sizeof(m_dtCharInfoPrev));
	memset(&m_LadderWarsCharInfo, 0, sizeof(m_LadderWarsCharInfo));

	// HShield Init
// #ifdef _HSHIELD
//	MPacketHShieldCrypter::Init();
//#endif

#ifdef _LOCATOR // -by �߱���. Locator�� �����ؼ� Ŀ�ǵ带 ��������Em_This�� UID�� (0,0)�� �ƴϾ��� ��.
	m_This = MUID(0, 1);
#endif

	m_UPDCommadHackShield.Init();


#ifdef _CW_VOTE
	LastVoteID = -1;
	bMatching = false;;
#endif // _CW_VOTE
}


ZGameClient::~ZGameClient()
{
	DestroyUPnP();
	m_EmblemMgr.Destroy();

	ZGetMyInfo()->Clear();

#ifdef LOCALE_NHNUSA
	// Custom: Disable NHN related
	//GetNHNUSAReport().ReportCloseGame();
#endif
}

void ZGameClient::PriorityBoost(bool bBoost)
{
#ifdef _GAMEGUARD
	return;
#endif

	if (bBoost) {
		SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);
		m_bPriorityBoost = true;
		OutputDebugString("<<<<  BOOST ON  >>>> \n");
	}
	else {
		SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
		m_bPriorityBoost = false;
		OutputDebugString("<<<<  BOOST OFF  >>>> \n");
	}
}


void ZGameClient::OnRegisterCommand(MCommandManager* pCommandManager)
{
	MMatchClient::OnRegisterCommand(pCommandManager);
	ZAddCommandTable(pCommandManager);
}

void ZGameClient::OnPrepareCommand(MCommand* pCommand)
{
#ifndef _PUBLISH
	m_pLogFrame->AddCommand(GetGlobalClockCount(), pCommand);
#endif

}

int ZGameClient::OnResponseMatchLogin(const MUID& uidServer, int nResult, const char* szServerName, const MMatchServerMode nServerMode,
	const char* szAccountID, const MMatchUserGradeID nUGradeID, const MMatchPremiumGradeID nPGradeID, int nCountryFlag, int nCash, int nEvent,
	const MUID& uidPlayer, bool bEnabledSurvivalMode, bool bEnabledDuelTournament, unsigned char* pbyGuidReqMsg)
{
	int nRet = MMatchClient::OnResponseMatchLogin(uidServer, nResult, szServerName, nServerMode,
		szAccountID, nUGradeID, nPGradeID, nCountryFlag, nCash, nEvent, uidPlayer, bEnabledSurvivalMode, bEnabledDuelTournament, pbyGuidReqMsg);

	ZGetMyInfo()->InitAccountInfo(szAccountID, nUGradeID, nPGradeID, nCountryFlag, nCash, nEvent);

#ifdef _DEBUG
	mlog("InitAccountInfo :: nCountryFlag = %d - nCash = %d - nEvent = %d\n", nCountryFlag, nCash, nEvent);
#endif
	m_LobbyChat = (nUGradeID >= MMUG_JORK && nUGradeID != MMUG_BLOCKED); // Custom: Auto-enable Lobbychat for staff.
	m_bAdminNAT = (nUGradeID >= MMUG_JORK && nUGradeID != MMUG_BLOCKED); // Custom: Admin NAT 

	if ((nResult == 0) && (nRet == MOK)) {	// Login successful
		mlog("Login Successful. \n");

#ifdef _HSHIELD
		int dwRet = _AhnHS_MakeGuidAckMsg(pbyGuidReqMsg,        // [in]
			ZGetMyInfo()->GetSystemInfo()->pbyGuidAckMsg // [out]
		);
		if (dwRet != ERROR_SUCCESS)
			mlog("Making Guid Ack Msg Failed. (Error code = %x)\n", dwRet);
#endif

		// ���⼭ AccountCharList�� ��û�Ѵ�.
		ZApplication::GetGameInterface()->ChangeToCharSelection();
	}
	else {								// Login failed
		mlog("Login Failed.(ErrCode=%d) \n", nResult);


#ifdef LOCALE_NHNUSA
		if (nResult == 10003)
		{	// ����E�ο����� ��ã�µ� ��� �α��� �õ� �Ѵٸ�E10������ ���̸� �ش�.
			ZApplication::GetGameInterface()->SetErrMaxPlayer(true);
			ZApplication::GetGameInterface()->SetErrMaxPlayerDelayTime(timeGetTime() + 7000);
		}
		else if (nResult == MERR_COMMAND_INVALID_VERSION)
		{
			// Close the client for users who spam relogin
#ifndef _DEBUG
			ZApplication::GetGameInterface()->OnDisconnectMsg(MERR_COMMAND_INVALID_VERSION);

			// Just a simple trick to stop the client from responding
			ZApplication::GetGameInterface()->SetErrMaxPlayer(true);
			ZApplication::GetGameInterface()->SetErrMaxPlayerDelayTime(timeGetTime() + 9000);
#else
			ZPostDisconnect();
			if (nResult != MOK)
				ZApplication::GetGameInterface()->ShowErrorMessage(nResult);
#endif
		}
		else
		{
			ZPostDisconnect();
			if (nResult != MOK)
				ZApplication::GetGameInterface()->ShowErrorMessage(nResult);
		}
#else
		ZPostDisconnect();

		if (nResult != MOK)
		{
			ZApplication::GetGameInterface()->ShowErrorMessage(nResult);
		}
#endif
		return MOK;
	}

	ZApplication::GetGameInterface()->ShowWidget("NetmarbleLogin", false);

	StartBridgePeer();

	return MOK;
}
#ifdef _SRVRPNG
#define DELAY 120000
unsigned long int st_nLastTime = 0;
void ZGameClient::OnServerPing(DWORD ping, bool ServerType)
{
	m_ServerPing = ping;

	unsigned long int nNowTime = timeGetTime();
	if ((nNowTime - st_nLastTime) >= DELAY)
	{
		st_nLastTime = nNowTime;
	}
	if (ZGetGame() && ZGetGame()->GetMatch()->GetMatchType() != MMATCH_GAMETYPE_DUELTOURNAMENT && ZGetGame()->GetMatch()->GetMatchType() != MMATCH_GAMETYPE_QUEST && ZGetGame()->GetMatch()->GetMatchType() != MMATCH_GAMETYPE_SURVIVAL) {
		MSTAGE_SETTING_NODE* pStage = (MSTAGE_SETTING_NODE*)GetMatchStageSetting()->GetStageSetting();
		if (pStage && pStage->nPingLimitHigh != 0)
		{
			if (pStage->nPingType == ServerType)
			{
				int mystrikes = GetPingLimitStrikes();
				if ((int)ping >= pStage->nPingLimitHigh || (int)ping <= pStage->nPingLimitLow)
				{
					mystrikes++;
					SetPingLimitStrikes(mystrikes);
					MClient::OutputMessage(MZMOM_LOCALREPLY, "Ping limit strike (%d/%d).", mystrikes, pStage->nPingStrikes);
				}
				else SetPingLimitStrikes(0);

				if (mystrikes >= pStage->nPingStrikes)
				{
					SetPingLimitStrikes(0);
					ZGetGameInterface()->ReserveLeaveStage();
					ZGetGameInterface()->ShowMessage("Your ping is too high!");
				}
			}
		}
	}
	else {
		SetPingLimitStrikes(0);
	}
}
#endif
void ZGameClient::OnStageSettingUpdate(DWORD ping)
{
	if (ping != 0)
	{
		ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
		MComboBox* pCombo = (MComboBox*)pResource->FindWidget("PingType");
		MEdit* pEdit = (MEdit*)pResource->FindWidget("ServerPing");
		if (!pCombo || !pEdit) return;
		if (pCombo->GetSelIndex() == 1)
			ping = 0;
		char pingchar[10];
		sprintf(pingchar, "%d", ping);
		pEdit->SetText(pingchar);
		char pingtext[10];
		pEdit = (MEdit*)pResource->FindWidget("PingLimitLow");
		int nping = ping;
		if (pEdit) {
			nping -= 55;
			if (nping <= 0) nping = 0;
			sprintf(pingtext, "%d", nping);
			pEdit->SetText(pingtext);
		}
		pEdit = (MEdit*)pResource->FindWidget("PingLimitHigh");
		if (pEdit) {
			nping += 110;
			sprintf(pingtext, "%d", nping);
			pEdit->SetText(pingtext);
		}
	}
}
void ZGameClient::OnAnnounce(unsigned int nType, char* szMsg)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	if (strncmp(szMsg, MTOK_ANNOUNCE_PARAMSTR, strlen(MTOK_ANNOUNCE_PARAMSTR)) == 0)
	{
		const char* szId = szMsg + strlen(MTOK_ANNOUNCE_PARAMSTR);
		int idErrMsg = 0;
		if (1 == sscanf(szId, "%d", &idErrMsg)) {
			char szTranslated[256];
			const char* szErrStr = ZErrStr(idErrMsg);
			const char* szArg = "";

			// ���� �Ľ� : ���ϴ� �ϴ� ���� 0�� �Ǵ� 1 �����̶�E�����ϰ�E��ǁE
			const char* pSeperator = strchr(szMsg, '\a');
			if (pSeperator) {
				szArg = pSeperator + 1;
			}

			sprintf(szTranslated, szErrStr, szArg);
			ZChatOutput(szTranslated, ZChat::CMT_SYSTEM);

#ifdef _FLAHS
			if (idErrMsg == MERR_HE_IS_NOT_READY)
			{
				RFlashWindow();
			}
#endif
			if (ZGetGame() && ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_INFECTED)
			{
				if (idErrMsg == MERR_TIME_10REMAINING || idErrMsg == MERR_TIME_30REMAINING || idErrMsg == MERR_TIME_60REMAINING)
				{
					ZGetSoundEngine()->PlaySound("time_tick_alert");
				}
			}
			return;
		}
	}

	ZChatOutput(szMsg, ZChat::CMT_SYSTEM);
}

void ZGameClient::OnBridgePeerACK(const MUID& uidChar, int nCode)
{
	SetBridgePeerFlag(true);
}

void ZGameClient::OnObjectCache(unsigned int nType, void* pBlob, int nCount)
{
	MMatchClient::OnObjectCache(nType, pBlob, nCount);

	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	ZPlayerListBox* pList = (ZPlayerListBox*)pResource->FindWidget("StagePlayerList_");

	vector< int > vecClanID;


	if (pList)
	{
		if (nType == MATCHCACHEMODE_UPDATE) {
			pList->RemoveAll();
			ZGetPlayerManager()->Clear();
			for (int i = 0; i < nCount; i++) {
				MMatchObjCache* pCache = (MMatchObjCache*)MGetBlobArrayElement(pBlob, i);
				if (pCache->CheckFlag(MTD_PlayerFlags_AdminHide) == false) {
					pList->AddPlayer(pCache->GetUID(), MOSS_NONREADY, pCache->GetLevel(),
						pCache->GetName(), pCache->GetClanName(), pCache->GetCLID(), false, MMT_ALL, pCache->GetDTGrade());


					if (m_EmblemMgr.CheckEmblem(pCache->GetCLID(), pCache->GetEmblemChecksum())) {
						// Begin Draw
					}
					else if (pCache->GetEmblemChecksum() != 0) {
						vecClanID.push_back(pCache->GetCLID());
					}
#ifdef _DYNAMIC
					for (int i = 0; i < MMCIP_END; ++i)
					{
						MMatchItemDesc* playerItem = MGetMatchItemDescMgr()->GetItemDesc(pCache->GetCostume()->nEquipedItemID[(MMatchCharItemParts)i]);
						if (playerItem != nullptr)
						{
							if (playerItem->GetDyn() != nullptr)
							{
								RMesh* playerMesh = nullptr;
								if (pCache->GetCostume()->nSex == MMS_MALE)
								{
									playerMesh = ZGetMeshMgr()->Get("heroman1");
								}
								else
								{
									playerMesh = ZGetMeshMgr()->Get("herowoman1");
								}
								if (playerMesh->m_parts_mgr->Find(playerItem->m_szDyn) == false)
								{
									string filePath = playerItem->m_szDyn;
									if (filePath.find("woman") != std::string::npos)
									{
										filePath = string("model/woman/") + playerItem->m_szDyn;
									}
									else
									{
										filePath = string("model/man/") + playerItem->m_szDyn;
									}
									playerMesh->m_parts_mgr->Add((char*)filePath.c_str());
								}
							}
						}
					}
#endif
					ZGetPlayerManager()->AddPlayer(pCache->GetUID(), pCache->GetName(), pCache->GetRank(), pCache->GetKillCount(), pCache->GetDeathCount());
				}
			}
		}
		else if (nType == MATCHCACHEMODE_ADD) {
			for (int i = 0; i < nCount; i++) {
				MMatchObjCache* pCache = (MMatchObjCache*)MGetBlobArrayElement(pBlob, i);
				if (pCache->CheckFlag(MTD_PlayerFlags_AdminHide) == false) {	//  Skip on AdminHide
					pList->AddPlayer(pCache->GetUID(), MOSS_NONREADY, pCache->GetLevel(),
						pCache->GetName(), pCache->GetClanName(), pCache->GetCLID(), false, MMT_ALL, pCache->GetDTGrade());

#ifdef _DYNAMIC
					for (int i = 0; i < MMCIP_END; ++i)
					{
						MMatchItemDesc* playerItem = MGetMatchItemDescMgr()->GetItemDesc(pCache->GetCostume()->nEquipedItemID[(MMatchCharItemParts)i]);
						if (playerItem != nullptr)
						{
							if (playerItem->GetDyn() != nullptr)
							{
								RMesh* playerMesh = nullptr;
								if (pCache->GetCostume()->nSex == MMS_MALE)
								{
									playerMesh = ZGetMeshMgr()->Get("heroman1");
								}
								else
								{
									playerMesh = ZGetMeshMgr()->Get("herowoman1");
								}
								if (playerMesh->m_parts_mgr->Find(playerItem->m_szDyn) == false)
								{
									string filePath = playerItem->m_szDyn;
									if (filePath.find("woman") != std::string::npos)
									{
										filePath = string("model/woman/") + playerItem->m_szDyn;
									}
									else
									{
										filePath = string("model/man/") + playerItem->m_szDyn;
									}
									playerMesh->m_parts_mgr->Add((char*)filePath.c_str());
								}
							}
						}
					}
#endif



					if (m_EmblemMgr.CheckEmblem(pCache->GetCLID(), pCache->GetEmblemChecksum())) {
						// Begin Draw
					}
					else if (pCache->GetEmblemChecksum() != 0) {
						vecClanID.push_back(pCache->GetCLID());
					}

					ZGetPlayerManager()->AddPlayer(pCache->GetUID(), pCache->GetName(), pCache->GetRank(), pCache->GetKillCount(), pCache->GetDeathCount());
				}
			}
		}
		else if (nType == MATCHCACHEMODE_REMOVE) {
			for (int i = 0; i < nCount; i++) {
				MMatchObjCache* pCache = (MMatchObjCache*)MGetBlobArrayElement(pBlob, i);
				pList->DelPlayer(pCache->GetUID());

				ZGetPlayerManager()->RemovePlayer(pCache->GetUID());
			}

			ZApplication::GetGameInterface()->UpdateBlueRedTeam();

			// Update Spy mode ban map list.
			ZApplication::GetStageInterface()->CreateSpyBanMapList();

		}


		if (vecClanID.size() > 0)
		{
			void* pBlob = MMakeBlobArray(sizeof(int), (int)vecClanID.size());
			int nCount = 0;
			for (vector<int>::iterator it = vecClanID.begin(); it != vecClanID.end(); it++, nCount++)
			{
				int* nClanID = (int*)MGetBlobArrayElement(pBlob, nCount);
				*nClanID = *it;
			}

			ZPostRequestEmblemURL(pBlob);
			MEraseBlobArray(pBlob);
			vecClanID.clear();
		}
	}
}

void ZGameClient::OnChannelResponseJoin(const MUID& uidChannel, MCHANNEL_TYPE nChannelType, const char* szChannelName, bool bEnableInterface)
{
	ZApplication::GetGameInterface()->SetState(GUNZ_LOBBY);

	m_uidChannel = uidChannel;
	strcpy(m_szChannel, szChannelName);
	m_CurrentChannelType = nChannelType;
	m_bEnableInterface = bEnableInterface;

	char szText[256];

	// Custom: Msg LadderWars, DT & CW
	//if (ZGetGameClient()->IsLadderWarsChannel())
	//{
	//   ZGetGameInterface()->ShowMessage(MSG_JOIN_CHANNEL_STREAK_LADDER);
	//}
	//else if (ZGetGameClient()->IsCWChannel())
	//{
	//	ZGetGameInterface()->ShowMessage(MSG_JOIN_CHANNEL_RULE_LADDER);
	//}
	ZGetGameInterface()->GetChat()->Clear(ZChat::CL_LOBBY);
	//	wsprintf(szText, "����� ä�� '%s'�� �����ϼ̽��ϴ�.", szChannelName);
		//wsprintf(szText, "����� ä�� '%s'�� �����ϼ̽��ϴ�.", szChannelName);
	ZTransMsg(szText, MSG_LOBBY_JOIN_CHANNEL, 1, szChannelName);

	ZChatOutput(szText, ZChat::CMT_SYSTEM, ZChat::CL_LOBBY);

	switch (GetServerMode())
	{
	case MSM_NORMAL1:
	{
		//			wsprintf(szText, "���������� ��ġ �����ø�E����ä���� �̿�E� �ֽñ�E�ٶ��ϴ�.");
		wsprintf(szText,
			ZMsg(MSG_LOBBY_LIMIT_LEVEL));
		ZChatOutput(szText, ZChat::CMT_SYSTEM, ZChat::CL_LOBBY);
	}
	break;
	case MSM_LADDER:
	{
		//			wsprintf(szText, "���װ����� ä�ο� ��E��� ���翁Eڵ鰁E�ܷ�� �˴ϴ�.");
		wsprintf(szText,
			ZMsg(MSG_LOBBY_LEAGUE));
		ZChatOutput(szText, ZChat::CMT_SYSTEM, ZChat::CL_LOBBY);
	}
	break;
	case MSM_CLAN:
	{
		if (nChannelType == MCHANNEL_TYPE_CLAN)
		{
			/*
							{
								static bool bUsed = false;
								if (!bUsed)
								{
			/*						wsprintf(szText, "[����] ��а� �����⸦ �翁EϽô� �����е��� Ŭ����E� ��ȰȁE���� ��E�����ϴ�.");
									ZChatOutput(szText, ZChat::CMT_SYSTEM, ZChat::CL_LOBBY);
									wsprintf(szText, "[����] ��E� �ð����� �����ϵ��� �ϰڽ��ϴ�. ����ܻ ���ĵ�� �˼��մϴ�.");
									ZChatOutput(szText, ZChat::CMT_SYSTEM, ZChat::CL_LOBBY);

									bUsed = true;
								}
							}
			*/


			ZPostRequestClanInfo(GetPlayerUID(), szChannelName);
		}
	}
	break;
	}


	ZRoomListBox* pRoomList =
		(ZRoomListBox*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Lobby_StageList");
	if (pRoomList) pRoomList->Clear();

	// 
	ZApplication::GetGameInterface()->SetRoomNoLight(1);
#ifdef _SYSINTERNEW
	ZGetGameInterface()->NormalChannelType();
	ZGetGameInterface()->ClanChannelType();
	ZGetGameInterface()->DuelChannelType();
	ZGetGameInterface()->PlayerChannelType();
#else
	ZGetGameInterface()->InitLobbyUIByChannelType();
#endif


#ifdef LOCALE_NHNUSA
	// Custom: Disable NHN related
	//GetNHNUSAReport().ReportJoinChannel();
#endif

#ifdef LIMIT_ACTIONLEAGUE
	// �ӽ÷� �׼Ǹ��� 
	bool bActionLeague = (strstr(szChannelName, "�׼�") != NULL) || (nChannelType == MCHANNEL_TYPE_USER);

	ZGetGameInterface()->InitLadderUI(bActionLeague);
#endif
}

void ZGameClient::OnChannelChat(const MUID& uidChannel, char* szName, char* szChat, int nGrade)
{
	if (GetChannelUID() != uidChannel)		return;
	if ((szChat[0] == 0) || (szName[0] == 0))	return;

	//	MUID uid = GetObject(szName);
	//	MMatchObjectCache* pObjCache = FindObjCache(uid);
	MCOLOR _color = MCOLOR(0, 0, 0);

	MMatchUserGradeID gid = (MMatchUserGradeID)nGrade;
	//	gid = MMUG_DEVELOPER;

	char sp_name[256];

	bool bSpUser = GetUserGradeIDColor(gid, _color, sp_name);

	char szText[512];


	// Custom: Ignore List check (Not self)
	if (ZGetGameClient()->IsUserIgnored(szName) && stricmp(szName, ZGetMyInfo()->GetCharName()))
		return;

	if (bSpUser)	// Ư������E
	{
		// Custom: Unmask
		wsprintf(szText, "%s : %s", szName, szChat);
		//wsprintf(szText, "%s : %s", sp_name , szChat);
		ZChatOutput(szText, ZChat::CMT_NORMAL, ZChat::CL_LOBBY, _color);
	}
	else if (!ZGetGameClient()->GetRejectNormalChat() ||				// �Ϲ� ����E
		(strcmp(szName, ZGetMyInfo()->GetCharName()) == 0))
	{
		wsprintf(szText, "%s : %s", szName, szChat);
		//wsprintf(szText, "^4%s^9 : %s", szName, szChat);
		ZChatOutput(szText, ZChat::CMT_NORMAL, ZChat::CL_LOBBY);
	}

	if (ZGetGameClient()->m_LobbyChat && (ZGetGameInterface()->GetState() == GUNZ_STAGE || ZGetGameInterface()->GetState() == GUNZ_GAME))
	{
		wsprintf(szText, "^2[Lobby]%s: %s", szName, szChat);
		ZChatOutput(szText);
	}
}

void ZGameClient::OnChannelList(void* pBlob, int nCount)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MListBox* pWidget = (MListBox*)pResource->FindWidget("ChannelList");
	if (pWidget == NULL) {
		ZGetGameClient()->StopChannelList();
		return;
	}

	int nStartIndex = pWidget->GetStartItem();
	int nSelIndex = pWidget->GetSelIndex();
	const char* szChannelName = NULL;
	pWidget->RemoveAll();
	for (int i = 0; i < nCount; i++)
	{
		MCHANNELLISTNODE* pNode = (MCHANNELLISTNODE*)MGetBlobArrayElement(pBlob, i);


		if (pNode->szChannelNameStrResId[0] != 0)
		{
			szChannelName = ZGetStringResManager()->GetStringFromXml(pNode->szChannelNameStrResId);
		}
		else
		{
			szChannelName = pNode->szChannelName;
		}

		/*if (ZGetMyInfo()->IsAdminGrade())
		{
		  MLabel* pLabel = (MLabel*)pResource->FindWidget("ChannelOnline");
		  char szText[128];
		  sprintf(szText, "Players Online : %d", pNode->nPlayerOn);
		  pLabel->SetText(szText);
		}*/

		pWidget->Add(new ZChannelListItem(pNode->uidChannel, (int)pNode->nNo, szChannelName, pNode->nChannelType, (int)pNode->nPlayers, (int)pNode->nMaxPlayers)
		);
	}

	pWidget->SetStartItem(nStartIndex);
	pWidget->SetSelIndex(nSelIndex);
}

void ZGameClient::OnChannelResponseRule(const MUID& uidchannel, const char* pszRuleName)
{
	MChannelRule* pRule = ZGetChannelRuleMgr()->GetRule(pszRuleName);
	if (pRule == NULL)
		return;

	SetChannelRuleName(pszRuleName);

	// �ӽ� ó��?
	MComboBox* pCombo = (MComboBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("MapSelection");
	if (pCombo != NULL)
	{
		InitMaps(pCombo);
		MListBox* pList = (MListBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("MapList");
		pList->RemoveAll();
		if (pList != NULL)
		{
			for (int i = 0; i < pCombo->GetCount(); ++i)
			{
				pList->Add(pCombo->GetString(i));
			}
		}
	}


	bool bEnable = GetEnableInterface();

	MWidget* pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("StageJoin");
	if (pWidget)		pWidget->Enable(bEnable);

	pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("StageCreateFrameCaller");
	if (pWidget)		pWidget->Enable(bEnable);

	pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("QuickJoin");
	if (pWidget)		pWidget->Enable(bEnable);

	pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("QuickJoin2");
	if (pWidget)		pWidget->Enable(bEnable);

	pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("ArrangedTeamGame");
	if (pWidget)		pWidget->Enable(bEnable);
#ifdef _LADDERUPDATE 
	pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("CharUpdateCaller");
	if (pWidget)		pWidget->Enable(bEnable);
#endif

	pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("RejoinCW");
	if (pWidget)		pWidget->Enable(bEnable);

	pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("ChannelChattingInput");
	if (pWidget)		pWidget->Enable(bEnable);

	pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Lobby_StageList");
	if (pWidget)		pWidget->Enable(bEnable);
}

void ZGameClient::OnStageEnterBattle(const MUID& uidChar, MCmdEnterBattleParam nParam)
{
	// �̰��� ZGame ���� �ҷ��ش�
	if (uidChar == GetPlayerUID())		// enter�ѻ���� ���ڽ��� �濁E
	{
		ZPostRequestGameInfo(ZGetGameClient()->GetPlayerUID(), ZGetGameClient()->GetStageUID());
#ifdef _FORHPAP
		ZPostClientSettings(ClientSettings);
#endif
		MStageCharSettingList::iterator itor = m_MatchStageSetting.m_CharSettingList.begin();
		for (; itor != m_MatchStageSetting.m_CharSettingList.end(); ++itor)
		{
			MSTAGE_CHAR_SETTING_NODE* pCharNode = (*itor);
			pCharNode->nState = MOSS_NONREADY;
		}
	}

	StartUDPTest(uidChar);
}

void ZGameClient::OnStageJoin(const MUID& uidChar, const MUID& uidStage, unsigned int nRoomNo, char* szStageName)
{
	//	SetBridgePeerFlag(false);

		// Custom: Open close tag count
	int nOpenCount = 0;
	int nCloseCount = 0;

	if (uidChar == GetPlayerUID()) {
		m_nStageCursor = 0;
		m_uidStage = uidStage;
		m_nRoomNo = nRoomNo;
#ifdef _FORHPAP
		JustJoinedStage = true;
#endif
		memset(m_szStageName, 0, sizeof(m_szStageName));
		strcpy(m_szStageName, szStageName);	// Save StageName

		// Custom: Check the room modifier information
		// MAKE SURE THE DESCRIPTION FOR ENUM IS ADDED >>>> MMatchStageSetting::GetModifierString!!!
		char szStageNameCopy[256];
		strcpy(szStageNameCopy, szStageName);

		for (int i = 0; i < strlen(szStageNameCopy); ++i)
			szStageNameCopy[i] = tolower(szStageNameCopy[i]);

		if (strlen(szStageNameCopy) > 0)
		{
			// get every pos of [, ] and then repeat
			vector<int> vPos;

			for (int i = 0; i < strlen(szStageNameCopy); ++i)
			{
				// recursive parsing?
				if (szStageNameCopy[i] == '[')
				{
					vPos.push_back(i);
					++nOpenCount;
				}
				else if (szStageNameCopy[i] == ']')
				{
					vPos.push_back(i);
					++nCloseCount;
				}
			}

			if (nOpenCount == nCloseCount && (nOpenCount > 0 && nCloseCount > 0))
			{
				vector<string> vTagArray;

				int nTagCount = (int)(vPos.size() / 2);
				int nVec = 0;

				// Parsing works.
				for (int i = 0; i < nTagCount; ++i)
				{
					const char* szSource = &szStageNameCopy[vPos[nVec]];
					int nLength = (vPos[nVec + 1] + 1) - vPos[nVec]; // assume it's okay

					if (nLength < 0)
						break;

					char* szBuf = new char[nLength + 1];

					strncpy(szBuf, szSource, nLength);
					szBuf[nLength] = '\0';

					string strSplit = szBuf;
					vTagArray.push_back(strSplit);

					delete[] szBuf;
					szBuf = NULL;

					nVec += 2;
				}

#define RMOD_BEGIN for (int i = 0; i < vTagArray.size(); ++i) {
#define RMOD_END }
#define RMOD(x, y) if( strstr( vTagArray[i].c_str(), x ) ) m_MatchStageSetting.InsertRoomModifier(y, -1);
#define RMOD2(x, y, type) {if( strstr( vTagArray[i].c_str(), x ) ){		int nVal = -1;	string strSearch = string(x) + string("%d") + string(y);	sscanf( vTagArray[i].c_str(), strSearch.c_str(), &nVal );	if( nVal < INT_MIN || nVal > INT_MAX )		nVal = -1;	if( nVal >= 0 )		m_MatchStageSetting.InsertRoomModifier( type, nVal );}}

				//#define RMOD(x, y) for( int i = 0; i < vTagArray.size(); ++i ) { if( strstr( vTagArray[i].c_str(), x ) ) m_MatchStageSetting.InsertRoomModifier(y, -1); }
				//#define RMOD2(x, y, type) for( int i = 0; i < vTagArray.size(); ++i ){ if( !strstr( vTagArray[i].c_str(), x ) ) continue; int nVal = -1;string strSearch = string(x) + string("%d") + string(y);sscanf( vTagArray[i].c_str(), strSearch.c_str(), &nVal );if( nVal < INT_MIN || nVal > INT_MAX )nVal = -1; if( nVal >= 0 )m_MatchStageSetting.InsertRoomModifier( type, nVal );}

				RMOD_BEGIN
					RMOD("[lead]", MMOD_LEAD)
					RMOD("[r]", MMOD_FASTRELOAD)
					RMOD("[ex]", MMOD_EXPLOSION)
					RMOD("[f]", MMOD_FLIP)
					RMOD("[glad]", MMOD_GLAD)
					RMOD("[fps]", MMOD_FPS)
					RMOD("[m]", MMOD_MASSIVES)
					RMOD("[n]", MMOD_NINJA)
					RMOD("[v]", MMOD_VANILLA)
					RMOD("[nc]", MMOD_NC)
					RMOD("[nr]", MMOD_NR)
					RMOD("[ns]", MMOD_NS)
					RMOD("[ng]", MMOD_NG)
					RMOD("[ni]", MMOD_NI)
					RMOD("[gla]", MMOD_MELEEONLY)
					RMOD("[sgo]", MMOD_SHOTGUNONLY)
					RMOD("[rvo]", MMOD_REVONLY)
					RMOD("[ia]", MMOD_INFINITE_AMMO)
					RMOD("[rtd]", MMOD_ROLLTHEDICE)
					RMOD("[wiw]", MMOD_WIWEAPON)
					RMOD2("[g", "]", MMOD_GRAVITY)
					RMOD2("[s", "]", MMOD_SPEED)
					RMOD2("[j", "]", MMOD_JUMP)
					RMOD2("[dmg", "]", MMOD_DMG)
					RMOD("[paint]", MMOD_PAINT)
					RMOD("[wire]", MMOD_WIREFRAME)
					RMOD("[evm]", MMOD_EVENTMODE)
					RMOD_END

#undef RMOD2
#undef RMOD
#undef RMOD_END
#undef RMOD_BEGIN
			}
		}

		unsigned int nStageNameChecksum = m_szStageName[0] + m_szStageName[1] + m_szStageName[2] + m_szStageName[3];
		InitPeerCrypt(uidStage, nStageNameChecksum);
		CastStageBridgePeer(uidChar, uidStage);
	}

	MCommand* pCmd = new MCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_REQUEST_STAGESETTING), GetServerUID(), m_This);
	pCmd->AddParameter(new MCommandParameterUID(GetStageUID()));
	Post(pCmd);

	if (uidChar == GetPlayerUID())
	{
		ZChangeGameState(GUNZ_STAGE);
	}

	string name = GetObjName(uidChar);
	char szText[256];
	if (uidChar == GetPlayerUID())
	{
		ZGetGameInterface()->GetChat()->Clear(ZChat::CL_STAGE);

		char szTmp[256];
		sprintf(szTmp, "(%03d)%s", nRoomNo, szStageName);

		ZTransMsg(szText, MSG_JOINED_STAGE, 1, szTmp);
		ZChatOutput(szText, ZChat::CMT_SYSTEM, ZChat::CL_STAGE);

		// Custom: Display room modifier information.
		// NOTE: Room modifier descriptions are in MMatchStageSetting::GetModifierString!!!
		char szModString[1024];
		sprintf(szModString, "[Room Modifiers] : ");

		for (int i = 0, g = 0; i != MMOD_END; ++i)
		{
			if (m_MatchStageSetting.IsModifierUsed((ROOM_MOD)i))
			{
				strcat(szModString, m_MatchStageSetting.GetModifierString((ROOM_MOD)i));

				int nValue = m_MatchStageSetting.GetModifierValue((ROOM_MOD)i);
				char szExtra[32];
				memset(szExtra, 0, sizeof(szExtra));
				sprintf(szExtra, " (%d%s)", nValue, m_MatchStageSetting.IsModifierPercentage((ROOM_MOD)i) ? "%" : "");

				if (nValue >= 0)
					strcat(szModString, szExtra);

				++g;

				if (g != m_MatchStageSetting.GetModifierCount())
					strcat(szModString, ", ");
			}
		}

		if (m_MatchStageSetting.GetModifierCount() > 0)
		{
			ZChatOutput(MCOLOR(ZCOLOR_CHAT_BROADCAST), szModString, ZChat::CL_STAGE);
			ZChatOutput(MCOLOR(ZCOLOR_CHAT_LOBBY_DEFALT), "Type /mods for more information.", ZChat::CL_STAGE);

			if (m_MatchStageSetting.IsModifierUsed(MMOD_LEAD))
			{
				ZGetGameInterface()->ShowMessage("This room has Leading enabled. All shots in-game will not be compensated for lag.");
			}

			if (m_MatchStageSetting.GetStageSetting()->bTeamKillEnabled == true)
			{
				ZGetGameInterface()->ShowMessage("This room has Friendly Fire enabled. Your teammates can kill you.");
			}
			//else if( m_MatchStageSetting.IsModifierUsed( MMOD_INFINITE_AMMO ) ||
			//		 m_MatchStageSetting.IsModifierUsed( MMOD_FASTRELOAD ) )
			//{
			//	// exceptions
			//	if( ZGetGameClient()->GetMatchStageSetting()->GetGameType() == MMATCH_GAMETYPE_DEATHMATCH_SOLO )
			//		ZGetGameInterface()->ShowMessage("Infinite Ammo and Fast Reload will not work under Deathmatch, TDM and Duel.");
			//}
		}

		if (nOpenCount != nCloseCount)
			ZChatOutput(MCOLOR(ZCOLOR_CHAT_BROADCAST), "Warning: Invalid Room Modifiers!", ZChat::CL_STAGE);

		if (ZGetGameClient()->GetMatchStageSetting()->IsModifierUsed(MMOD_SHOTGUNONLY))
		{
			ZMyItemNode* pPrimary = ZGetMyInfo()->GetItemList()->GetEquipedItem(MMCIP_PRIMARY);

			if (pPrimary)
			{
				MMatchItemDesc* pPrimaryDesc = MGetMatchItemDescMgr()->GetItemDesc(pPrimary->GetItemID());

				if (pPrimaryDesc && pPrimaryDesc->m_nWeaponType.Ref() != MWT_SHOTGUN && pPrimaryDesc->m_nWeaponType.Ref() != MWT_SAWED_SHOTGUN)
					ZPostRequestTakeoffItem(ZGetGameClient()->GetPlayerUID(), MMCIP_PRIMARY);
			}

			ZMyItemNode* pSecondary = ZGetMyInfo()->GetItemList()->GetEquipedItem(MMCIP_SECONDARY);

			if (pSecondary)
			{
				MMatchItemDesc* pSecondaryDesc = MGetMatchItemDescMgr()->GetItemDesc(pSecondary->GetItemID());

				if (pSecondaryDesc && pSecondaryDesc->m_nWeaponType.Ref() != MWT_SHOTGUN && pSecondaryDesc->m_nWeaponType.Ref() != MWT_SAWED_SHOTGUN)
					ZPostRequestTakeoffItem(ZGetGameClient()->GetPlayerUID(), MMCIP_SECONDARY);
			}
		}
		if (ZGetGameClient()->GetMatchStageSetting()->IsModifierUsed(MMOD_REVONLY))
		{
			ZMyItemNode* pPrimary = ZGetMyInfo()->GetItemList()->GetEquipedItem(MMCIP_PRIMARY);

			if (pPrimary)
			{
				MMatchItemDesc* pPrimaryDesc = MGetMatchItemDescMgr()->GetItemDesc(pPrimary->GetItemID());

				if (pPrimaryDesc && pPrimaryDesc->m_nWeaponType.Ref() != MWT_REVOLVER && pPrimaryDesc->m_nWeaponType.Ref() != MWT_REVOLVERx2)
					ZPostRequestTakeoffItem(ZGetGameClient()->GetPlayerUID(), MMCIP_PRIMARY);
			}

			ZMyItemNode* pSecondary = ZGetMyInfo()->GetItemList()->GetEquipedItem(MMCIP_SECONDARY);

			if (pSecondary)
			{
				MMatchItemDesc* pSecondaryDesc = MGetMatchItemDescMgr()->GetItemDesc(pSecondary->GetItemID());

				if (pSecondaryDesc && pSecondaryDesc->m_nWeaponType.Ref() != MWT_REVOLVER && pSecondaryDesc->m_nWeaponType.Ref() != MWT_REVOLVERx2)
					ZPostRequestTakeoffItem(ZGetGameClient()->GetPlayerUID(), MMCIP_SECONDARY);
			}
		}
		// Custom: Infected game mode avatar check
		if (ZGetGameClient()->GetMatchStageSetting()->GetGameType() == MMATCH_GAMETYPE_INFECTED)
		{
			ZPostRequestTakeoffItem(ZGetGameClient()->GetPlayerUID(), MMCIP_AVATAR);
		}

		if (ZGetGameClient()->GetMatchStageSetting()->GetGameType() == MMATCH_GAMETYPE_GUNGAME)
		{
			ZPostRequestTakeoffItem(ZGetGameClient()->GetPlayerUID(), MMCIP_CUSTOM1);
			ZPostRequestTakeoffItem(ZGetGameClient()->GetPlayerUID(), MMCIP_CUSTOM2);
			ZPostRequestTakeoffItem(ZGetGameClient()->GetPlayerUID(), MMCIP_CUSTOM3);
		}

		// Custom: Room mod: N[X]
		if (!ZGetGameClient()->GetMatchStageSetting()->IsUserExempt(ZGetMyInfo()->GetUGradeID()))// Custom: Staff Exception Check
		{
			if (ZGetGameClient()->GetMatchStageSetting()->IsModifierUsed(MMOD_NC))
			{
				ZPostRequestTakeoffItem(ZGetGameClient()->GetPlayerUID(), MMCIP_HEAD);
				ZPostRequestTakeoffItem(ZGetGameClient()->GetPlayerUID(), MMCIP_CHEST);
				ZPostRequestTakeoffItem(ZGetGameClient()->GetPlayerUID(), MMCIP_HANDS);
				ZPostRequestTakeoffItem(ZGetGameClient()->GetPlayerUID(), MMCIP_LEGS);
				ZPostRequestTakeoffItem(ZGetGameClient()->GetPlayerUID(), MMCIP_FEET);
				ZPostRequestTakeoffItem(ZGetGameClient()->GetPlayerUID(), MMCIP_AVATAR);
			}
			if (ZGetGameClient()->GetMatchStageSetting()->IsModifierUsed(MMOD_NR))
			{
				ZPostRequestTakeoffItem(ZGetGameClient()->GetPlayerUID(), MMCIP_FINGERL);
				ZPostRequestTakeoffItem(ZGetGameClient()->GetPlayerUID(), MMCIP_FINGERR);
			}

			if (ZGetGameClient()->GetMatchStageSetting()->IsModifierUsed(MMOD_NS))
				ZPostRequestTakeoffItem(ZGetGameClient()->GetPlayerUID(), MMCIP_MELEE);
			// Custom: Melee Only
			if (ZGetGameClient()->GetMatchStageSetting()->IsModifierUsed(MMOD_NG) || ZGetGameClient()->GetMatchStageSetting()->IsModifierUsed(MMOD_GLAD) || ZGetGameClient()->GetMatchStageSetting()->IsModifierUsed(MMOD_MELEEONLY))
			{
				ZPostRequestTakeoffItem(ZGetGameClient()->GetPlayerUID(), MMCIP_PRIMARY);
				ZPostRequestTakeoffItem(ZGetGameClient()->GetPlayerUID(), MMCIP_SECONDARY);
			}
			// Custom: Melee Only
			if (ZGetGameClient()->GetMatchStageSetting()->IsModifierUsed(MMOD_NI) || ZGetGameClient()->GetMatchStageSetting()->IsModifierUsed(MMOD_GLAD) || ZGetGameClient()->GetMatchStageSetting()->IsModifierUsed(MMOD_MELEEONLY))
			{
				ZPostRequestTakeoffItem(ZGetGameClient()->GetPlayerUID(), MMCIP_CUSTOM1);
				ZPostRequestTakeoffItem(ZGetGameClient()->GetPlayerUID(), MMCIP_CUSTOM2);
				ZPostRequestTakeoffItem(ZGetGameClient()->GetPlayerUID(), MMCIP_CUSTOM3);
			}
		}
	}
	else if (GetStageUID() == uidStage)
	{
		char sp_name[256];
		MCOLOR _color;
		MMatchUserGradeID gid = MMUG_FREE;

		char name[32];
		char kill[32];
		char death[32];
		char winning[32];

		ZPlayerInfo* pInfo = ZGetPlayerManager()->Find((MUID)uidChar);
		if (pInfo != NULL)
		{
			sprintf(kill, "%d %s", pInfo->GetKill(), ZMsg(MSG_CHARINFO_KILL));
			sprintf(death, "%d %s", pInfo->GetDeath(), ZMsg(MSG_CHARINFO_DEATH));
			sprintf(winning, "%.1f%%", pInfo->GetWinningRatio());
		}
		else
		{
			sprintf(kill, "? %s", ZMsg(MSG_CHARINFO_KILL));
			// Custom: Removed pInfo->GetDeath(), from underlying code as it caused crashes (STUPID MAIET.)
			sprintf(death, "? %s", ZMsg(MSG_CHARINFO_DEATH));
			sprintf(winning, "0.0%%");
		}

		if (GetUserInfoUID(uidChar, _color, sp_name, gid))
		{
			MMatchObjCache* pObjCache = ZGetGameClient()->FindObjCache(uidChar);
			if (pObjCache && pObjCache->CheckFlag(MTD_PlayerFlags_AdminHide))
				return;
			// Skip on AdminHide

			// Custom: Unmask names
			strcpy(name, pInfo->GetName());
			//strcpy( name, sp_name);
			ZTransMsg(szText, MSG_JOINED_STAGE2, 4, name, kill, death, winning);
			ZChatOutput(szText, ZChat::CMT_SYSTEM, ZChat::CL_STAGE);
		}
		else
		{
			strcpy(name, pInfo->GetName());
			ZTransMsg(szText, MSG_JOINED_STAGE2, 4, name, kill, death, winning);
			ZChatOutput(szText, ZChat::CMT_SYSTEM, ZChat::CL_STAGE);
		}
	}
	// Custom:  If joining stage, update distance automatically.
	/*	if (ZGetGameInterface() && !ZGetGameInterface()->GetGameClient()->GetMatchStageSetting()->IsModifierUsed(MMOD_FPS))
		{
			ZCamera* pCamera = ZGetGameInterface()->GetCamera();
			if (pCamera)
				pCamera->AdjustDist();
		}*/
}

void ZGameClient::OnStageLeave(const MUID& uidChar, const MUID& uidStage)
{
	if (uidChar == GetPlayerUID()) {
		m_uidStage = MUID(0, 0);
		m_nRoomNo = 0;
	}


	if (uidChar == GetPlayerUID())
	{
		ZChangeGameState(GUNZ_LOBBY);
	}

	for (MStageCharSettingList::iterator i = m_MatchStageSetting.m_CharSettingList.begin(); i != m_MatchStageSetting.m_CharSettingList.end(); i++) {
		if (uidChar == (*i)->uidChar)
		{
			delete (*i);
			m_MatchStageSetting.m_CharSettingList.erase(i);
			break;
		}
	}

	// Custom: Empty delayed command list to prevent EnterBattle bugs
	if (uidChar == GetPlayerUID())
	{
		if (ZGetGame() == NULL)
		{
			std::list<MCommand*>::iterator it = ZGetGameInterface()->m_listDelayedGameCmd.begin();
			for (; it != ZGetGameInterface()->m_listDelayedGameCmd.end();)
			{
				MCommand* pWaitCmd = *it;
				delete pWaitCmd;
				it = ZGetGameInterface()->m_listDelayedGameCmd.erase(it);
			}
		}
	}

	ZGetGameClient()->SetVoteInProgress(false);
	ZGetGameClient()->SetCanVote(false);

	AgentDisconnect();
}

void ZGameClient::OnStageStart(const MUID& uidChar, const MUID& uidStage, int nCountdown)
{
	// ���� ���۽� ���� ���ð�E� ���� ��Ȱ��ȭ ����
	ZApplication::GetStageInterface()->SetEnableWidgetByRelayMap(false);
	SetCountdown(nCountdown);
}

void ZGameClient::OnStageRelayStart()
{
	SetCountdown(3);
}

void ZGameClient::OnStageLaunch(const MUID& uidStage, const char* pszMapName)
{
	m_bLadderGame = false;
	m_bLadderWars = false;

	// Custom: Allow tunneling.
	//SetAllowTunneling(true);
	SetAllowTunneling(false);

	m_MatchStageSetting.SetMapName(const_cast<char*>(pszMapName));

	if (ZApplication::GetGameInterface()->GetState() != GUNZ_GAME) {
		ZChangeGameState(GUNZ_GAME);		// thread safely
	}
}

void ZGameClient::OnStageFinishGame(const MUID& uidStage, const bool bIsRelayMapUnFinish)
{
	if (ZApplication::GetGameInterface()->GetState() == GUNZ_GAME)
	{
		ZApplication::GetGameInterface()->FinishGame();
		//		ZChangeGameState(GUNZ_STAGE);		// thread safely
	}
	else if (ZApplication::GetGameInterface()->GetState() == GUNZ_STAGE) {

	}
	// �����̸� ����, �������� �� ���� ���ð�E� ���� ����(���ӽ��۹�ư ����)
	bool bEndRelayMap = !bIsRelayMapUnFinish;
	ZApplication::GetStageInterface()->SetEnableWidgetByRelayMap(bEndRelayMap);

	ZPostRequestStageSetting(ZGetGameClient()->GetStageUID());
	// Custom: Request spymaplist
	ZPostRequestSpyBanMapList();

}

void ZGameClient::OnStageMap(const MUID& uidStage, char* szMapName, bool bIsRelayMap)
{
	if (uidStage != GetStageUID()) return;

	m_MatchStageSetting.SetMapName(szMapName);
	m_MatchStageSetting.SetIsRelayMap(strcmp(MMATCH_MAPNAME_RELAYMAP, szMapName) == 0);

	ZApplication::GetGameInterface()->SerializeStageInterface();
}
void ZGameClient::OnStageTeam(const MUID& uidChar, const MUID& uidStage, unsigned int nTeam)
{
	MMatchObjectStageState nStageState = MOSS_NONREADY;
	MSTAGE_CHAR_SETTING_NODE* pCharNode = m_MatchStageSetting.FindCharSetting(uidChar);
	if (pCharNode)
	{
		nStageState = pCharNode->nState;
	}

	m_MatchStageSetting.UpdateCharSetting(uidChar, nTeam, nStageState);
	ZApplication::GetGameInterface()->SerializeStageInterface();
}

void ZGameClient::OnStagePlayerState(const MUID& uidChar, const MUID& uidStage, MMatchObjectStageState nStageState)
{
#ifdef _SPEC 1
	int nTeam = MMT_SPECTATOR;
	MSTAGE_CHAR_SETTING_NODE* pCharNode = m_MatchStageSetting.FindCharSetting(uidChar);
	if (pCharNode) nTeam = pCharNode->nTeam;

	m_MatchStageSetting.UpdateCharSetting(uidChar, nTeam, nStageState);
	ZApplication::GetGameInterface()->SerializeStageInterface();

	m_MatchStageSetting.UpdateCharSetting(uidChar, nTeam, nStageState);

	GunzState GunzState = ZApplication::GetGameInterface()->GetState();
	if (GunzState == GUNZ_STAGE)
	{
		ZApplication::GetStageInterface()->OnStageCharListSettup();
	}

#else
	int nTeam = MMT_SPECTATOR;
	MSTAGE_CHAR_SETTING_NODE* pCharNode = m_MatchStageSetting.FindCharSetting(uidChar);
	if (pCharNode != NULL)
	{
		nTeam = pCharNode->nTeam;
	}

	m_MatchStageSetting.UpdateCharSetting(uidChar, nTeam, nStageState);

	GunzState GunzState = ZApplication::GetGameInterface()->GetState();
	if (GunzState == GUNZ_STAGE)
	{
		ZApplication::GetStageInterface()->OnStageCharListSettup();
	}
#endif
}

void ZGameClient::OnStageMaster(const MUID& uidStage, const MUID& uidChar)
{
	int nTeam = MMT_SPECTATOR;
	MMatchObjectStageState nStageState = MOSS_NONREADY;
	MSTAGE_CHAR_SETTING_NODE* pCharNode = m_MatchStageSetting.FindCharSetting(uidChar);
	if (pCharNode)
	{
		nTeam = pCharNode->nTeam;
		nStageState = pCharNode->nState;
	}

	m_MatchStageSetting.SetMasterUID(uidChar);
	m_MatchStageSetting.UpdateCharSetting(uidChar, nTeam, nStageState);

	ZApplication::GetGameInterface()->SerializeStageInterface();
}

void ZGameClient::OnStageChat(const MUID& uidChar, const MUID& uidStage, char* szChat)
{
	if (GetStageUID() != uidStage) return;
	if (szChat[0] == 0) return;

	string name = GetObjName(uidChar);

	MCOLOR _color = MCOLOR(0, 0, 0);

	MMatchUserGradeID gid = MMUG_FREE;

	MMatchObjCache* pObjCache = FindObjCache(uidChar);

	if (pObjCache) {
		gid = pObjCache->GetUGrade();
	}

	//	gid = MMUG_DEVELOPER;

	char sp_name[256];

	bool bSpUser = GetUserGradeIDColor(gid, _color, sp_name);

	char szText[512];

	// Custom: Ignore List check (Not self)
	if (ZGetGameClient()->IsUserIgnored(name.c_str()) && stricmp(name.c_str(), ZGetMyInfo()->GetCharName()))
		return;

	if (bSpUser)	// Ư������E
	{
		// Custom: Unmask
		wsprintf(szText, "%s : %s", name.c_str(), szChat);
		//wsprintf(szText, "%s : %s", sp_name , szChat);
		ZChatOutput(szText, ZChat::CMT_NORMAL, ZChat::CL_STAGE, _color);
	}
	else if (!ZGetGameClient()->GetRejectNormalChat() ||				// �Ϲ� ����E
		(strcmp(pObjCache->GetName(), ZGetMyInfo()->GetCharName()) == 0))
	{
		wsprintf(szText, "^4%s^9 : %s", name.c_str(), szChat);
		ZChatOutput(szText, ZChat::CMT_NORMAL, ZChat::CL_STAGE);
	}
}

void ZGameClient::OnStageList(int nPrevStageCount, int nNextStageCount, void* pBlob, int nCount)
{
#ifdef _DEBUG
	char szTemp[256];
	sprintf(szTemp, "OnStageList (nPrevStageCount = %d , nNextStageCount = %d , nCount = %d\n",
		nPrevStageCount, nNextStageCount, nCount);
	OutputDebugString(szTemp);
#endif
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	ZRoomListBox* pRoomListBox = (ZRoomListBox*)pResource->FindWidget("Lobby_StageList");
	if (pRoomListBox == NULL)
	{
		ZGetGameClient()->StopStageList();
		return;
	}

	pRoomListBox->Clear();
	for (int i = 0; i < nCount; i++) {

		MTD_StageListNode* pNode = (MTD_StageListNode*)MGetBlobArrayElement(pBlob, i);

		// log debug
		if (pNode)
		{
			bool bForcedEntry = false, bPrivate = false, bLimitLevel = false;
			int nLimitLevel = 0;
			if (pNode->nSettingFlag & MSTAGENODE_FLAG_FORCEDENTRY_ENABLED) bForcedEntry = true;
			if (pNode->nSettingFlag & MSTAGENODE_FLAG_PRIVATE) bPrivate = true;
			if (pNode->nSettingFlag & MSTAGENODE_FLAG_LIMITLEVEL) bLimitLevel = true;

			char szMapName[256] = "";
			for (int tt = 0; tt < MMATCH_MAP_COUNT; tt++)
			{
				if (MGetMapDescMgr()->GetMapID(tt) == pNode->nMapIndex)
				{
					strcpy(szMapName, MGetMapDescMgr()->GetMapName(tt));
					break;
				}
			}

			ZRoomListBox::_RoomInfoArg roominfo;
			roominfo.nIndex = i;
			roominfo.nRoomNumber = (int)pNode->nNo;
			roominfo.uidStage = pNode->uidStage;
			roominfo.szRoomName = pNode->szStageName;
			roominfo.szMapName = szMapName;
			roominfo.nMaxPlayers = pNode->nMaxPlayers;
			roominfo.nCurrPlayers = pNode->nPlayers;
			roominfo.bPrivate = bPrivate;
			roominfo.bForcedEntry = bForcedEntry;
			roominfo.bLimitLevel = bLimitLevel;
			roominfo.nMasterLevel = pNode->nMasterLevel;
			roominfo.nLimitLevel = pNode->nLimitLevel;
			roominfo.nGameType = pNode->nGameType;
			roominfo.nStageState = pNode->nState;
#ifdef _SPEC 1
			roominfo.bObserverEnabled = pNode->bObserverEnabled;
#endif
#ifdef _BOXLEAD
			roominfo.bIsLead = pNode->bIsLead;
#endif
			pRoomListBox->SetRoom(&roominfo);
		}
	}
	pRoomListBox->SetScroll(nPrevStageCount, nNextStageCount);

	MWidget* pBtn = pResource->FindWidget("StageBeforeBtn");
	if (nPrevStageCount != -1)
	{
		if (nPrevStageCount == 0)
		{
			if (pBtn) pBtn->Enable(false);
		}
		else
		{
			if (pBtn) pBtn->Enable(true);
		}
	}

	pBtn = pResource->FindWidget("StageAfterBtn");
	if (nNextStageCount != -1)
	{
		if (nNextStageCount == 0)
		{
			if (pBtn) pBtn->Enable(false);
		}
		else
		{
			if (pBtn) pBtn->Enable(true);
		}
	}

}

ZPlayerListBox* GetProperFriendListOutput()
{
	ZIDLResource* pIDLResource = ZApplication::GetGameInterface()->GetIDLResource();

	GunzState nState = ZApplication::GetGameInterface()->GetState();
	switch (nState) {
	case GUNZ_LOBBY:
	{
		ZPlayerListBox* pList = (ZPlayerListBox*)pIDLResource->FindWidget("LobbyChannelPlayerList");
		if (pList && pList->GetMode() == ZPlayerListBox::PLAYERLISTMODE_CHANNEL_FRIEND)
			return pList;
		else
			return NULL;
	}
	break;
	case GUNZ_STAGE:
	{
		ZPlayerListBox* pList = (ZPlayerListBox*)pIDLResource->FindWidget("StagePlayerList_");
		if (pList && pList->GetMode() == ZPlayerListBox::PLAYERLISTMODE_STAGE_FRIEND)
			return pList;
		else
			return NULL;
	}
	break;
	};
	return NULL;
}

void ZGameClient::OnResponseFriendList(void* pBlob, int nCount)
{
	ZPlayerListBox* pList = GetProperFriendListOutput();
	if (pList)
		pList->RemoveAll();
	for (int i = 0; i < nCount; i++) {
		MFRIENDLISTNODE* pNode = (MFRIENDLISTNODE*)MGetBlobArrayElement(pBlob, i);

		ePlayerState state;
		switch (pNode->nState)
		{
		case MMP_LOBBY: state = PS_LOBBY; break;
		case MMP_STAGE: state = PS_WAIT; break;
		case MMP_BATTLE: state = PS_FIGHT; break;
		default: state = PS_LOGOUT;
		};

		if (pList) {
			pList->AddPlayer(state, pNode->szName, pNode->szDescription);
			//			pList->AttachToolTip(new MToolTip("ToolTipTest", pList));	// ������ ���̸�EBMButton�� ����
		}
		else {
			// Custom: Made friends list not show in other places
			//if (ZApplication::GetGameInterface()->GetState() != GUNZ_LOBBY )
			//{
			//	sprintf(szBuf, "    %s (%s)", pNode->szName, pNode->szDescription);
			//	ZChatOutput(szBuf,  ZChat::CMT_NORMAL, ZChat::CL_CURRENT);
			//}
		}
	}
}

void ZGameClient::OnChannelPlayerList(int nTotalPlayerCount, int nPage, void* pBlob, int nCount)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	ZPlayerListBox* pPlayerListBox = (ZPlayerListBox*)pResource->FindWidget("LobbyChannelPlayerList");

	if (!pPlayerListBox) return;
	if (pPlayerListBox->GetMode() != ZPlayerListBox::PLAYERLISTMODE_CHANNEL) return;

	MUID selUID = pPlayerListBox->GetSelectedPlayerUID();

	int nStartIndex = pPlayerListBox->GetStartItem();

	if (nCount) {
		pPlayerListBox->RemoveAll();
	}
	else {//�ƹ�����E� ���ٸ�~
		return;
	}

	pPlayerListBox->m_nTotalPlayerCount = nTotalPlayerCount;
	pPlayerListBox->m_nPage = nPage;

	ZLobbyPlayerListItem* pItem = NULL;

	vector< int > vecClanID;

	for (int i = 0; i < nCount; i++)
	{
		MTD_ChannelPlayerListNode* pNode = (MTD_ChannelPlayerListNode*)MGetBlobArrayElement(pBlob, i);
		if (pNode)
		{
			ePlayerState state;
			switch (pNode->nPlace)
			{
			case MMP_LOBBY: state = PS_LOBBY; break;
			case MMP_STAGE: state = PS_WAIT; break;
			case MMP_BATTLE: state = PS_FIGHT; break;
			default: state = PS_LOBBY;
			};

			if ((pNode->nPlayerFlags & MTD_PlayerFlags_AdminHide) == true)
			{
				//  Skip on AdminHide
			}
			else
			{

				// ä��â���� �÷��̾�E����Ʈ�� Ŭ����ũ�� �Ⱥ��δ�..... 20090216...by kammir
				// �̰����� Ŭ����ũ�� �� ��ǻ�Ϳ� ����Ǿ�E�ִ���EȮ���Ŀ� ������E������ ��û�Ѵ�.
				if (m_EmblemMgr.CheckEmblem(pNode->nCLID, pNode->nEmblemChecksum)) {		// Ŭ�� URL�� ������EVector�� �״´�
					// Begin Draw
				}
				else if (pNode->nEmblemChecksum != 0) {
					vecClanID.push_back(pNode->nCLID);
				}

				pPlayerListBox->AddPlayer(pNode->uidPlayer, state, pNode->nLevel, pNode->szName, pNode->szClanName, pNode->nCLID,
					(MMatchUserGradeID)pNode->nGrade, pNode->nDTLastWeekGrade);
			}
		}
	}

	//// Emblem // Ŭ�� URL�� ���� vector�� ������ ������.
	if (vecClanID.size() > 0)
	{
		void* pBlob = MMakeBlobArray(sizeof(int), (int)vecClanID.size()); /// nOneBlobSize��ŭ nBlobCount������ŭ �迭�� ��Ӱ ���鱁E
		int nCount = 0;
		for (vector<int>::iterator it = vecClanID.begin(); it != vecClanID.end(); it++, nCount++)
		{
			int* nClanID = (int*)MGetBlobArrayElement(pBlob, nCount);
			*nClanID = *it;
		}

		ZPostRequestEmblemURL(pBlob);
		MEraseBlobArray(pBlob);
		vecClanID.clear();
	}

	pPlayerListBox->SetStartItem(nStartIndex);
	pPlayerListBox->SelectPlayer(selUID);

	pPlayerListBox->AddTestItems();
}

void ZGameClient::OnChannelAllPlayerList(const MUID& uidChannel, void* pBlob, int nBlobCount)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	MListBox* pListBox = NULL;

	// Ŭ������â����E������ ���� â����Eã�ƺ���.

	MWidget* pDialog = pResource->FindWidget("ClanCreateDialog");
	if (pDialog && pDialog->IsVisible())
		pListBox = (MListBox*)pResource->FindWidget("ClanSponsorSelect");

	pDialog = pResource->FindWidget("ArrangedTeamGameDialog");
	if (pDialog && pDialog->IsVisible())
		pListBox = (MListBox*)pResource->FindWidget("ArrangedTeamSelect");

	if (pListBox && pListBox->IsVisible()) {
		pListBox->RemoveAll();
		for (int i = 0; i < nBlobCount; i++)
		{
			MTD_ChannelPlayerListNode* pNode = (MTD_ChannelPlayerListNode*)MGetBlobArrayElement(pBlob, i);
			if (pNode)
			{
				if (pNode->uidPlayer != GetPlayerUID())
					pListBox->Add(pNode->szName);
			}
		}
	}
}

void ZGameClient::UpdateStageSetting(MSTAGE_SETTING_NODE* pSetting, STAGE_STATE nStageState, const MUID& uidMaster)
{
	//m_MatchStageSetting.ShiftHeapPos();
	m_MatchStageSetting.UpdateStageSetting(pSetting);
	m_MatchStageSetting.SetMasterUID(uidMaster);
	m_MatchStageSetting.SetStageState(nStageState);

	// Custom: Infected game mode avatar check
	if (ZGetGameClient()->GetMatchStageSetting()->GetGameType() == MMATCH_GAMETYPE_INFECTED)
	{
		ZPostRequestTakeoffItem(ZGetGameClient()->GetPlayerUID(), MMCIP_AVATAR);
	}

	if (ZGetGameClient()->GetMatchStageSetting()->GetGameType() == MMATCH_GAMETYPE_GUNGAME)
	{
		ZPostRequestTakeoffItem(ZGetGameClient()->GetPlayerUID(), MMCIP_CUSTOM1);
		ZPostRequestTakeoffItem(ZGetGameClient()->GetPlayerUID(), MMCIP_CUSTOM2);
		ZPostRequestTakeoffItem(ZGetGameClient()->GetPlayerUID(), MMCIP_CUSTOM3);
	}

	bool bForceEntry = false;
	if (nStageState != STAGE_STATE_STANDBY)
	{
		bForceEntry = true;
	}
	m_bForcedEntry = bForceEntry;
#ifdef _FORHPAP
	char buf[256];
	bool Changed = false;
	auto CheckSetting = [&](auto Old, auto New, auto Default)
	{
		if (JustJoinedStage)
		{
			Changed = false;

			if (New == Default)
				return false;

			return true;
		}

		if (Old != New)
		{
			Changed = true;
			return true;
		}

		Changed = false;
		return false;
	};

#define CHECK_SETTING(member, def) CheckSetting(LastStageSetting.member, pSetting->member, def)

	if (CHECK_SETTING(ForceHPAP, false))
	{
		sprintf_safe(buf, "Force HP/AP%s%s", Changed ? " changed to " : ": ",
			pSetting->ForceHPAP ? "true" : "false");
		ZChatOutput(buf, ZChat::CMT_SYSTEM);
	}
	if (CHECK_SETTING(HP, 120))
	{
		sprintf_safe(buf, "Forced HP%s%d", Changed ? " it has been " : ": ",
			pSetting->HP);
		ZChatOutput(buf, ZChat::CMT_SYSTEM);
	}
	if (CHECK_SETTING(AP, 110))
	{
		sprintf_safe(buf, "Forced AP%s%d", Changed ? " it has been " : ": ",
			pSetting->AP);
		ZChatOutput(buf, ZChat::CMT_SYSTEM);
	}
	if (CHECK_SETTING(nPingLimitLow, 0))
	{
		sprintf_safe(buf, "PingLimit Low%s%d", Changed ? " changed to " : ": ",
			pSetting->nPingLimitLow);
		ZChatOutput(buf, ZChat::CMT_SYSTEM);
	}
	if (CHECK_SETTING(nPingLimitHigh, 0))
	{
		sprintf_safe(buf, "PingLimit High%s%d", Changed ? " changed to " : ": ",
			pSetting->nPingLimitHigh);
		ZChatOutput(buf, ZChat::CMT_SYSTEM);
	}
#ifdef _BOXLEAD
	if (CHECK_SETTING(bIsLeading, false))
	{
		sprintf_safe(buf, "System Lead%s%s", Changed ? " it has been " : ": ",
			pSetting->bIsLeading ? "On" : "Off");
		ZChatOutput(buf, ZChat::CMT_SYSTEM);
	}
#endif
	if (CHECK_SETTING(bForcedEntryEnabled, false))
	{
		sprintf_safe(buf, "Late Join%s%s", Changed ? " it has been " : ": ",
			pSetting->bForcedEntryEnabled ? "On" : "Off");
		ZChatOutput(buf, ZChat::CMT_SYSTEM);
	}
	if (CHECK_SETTING(bTeamKillEnabled, false))
	{
		sprintf_safe(buf, "Team Kill%s%s", Changed ? " it has been " : ": ",
			pSetting->bTeamKillEnabled ? "On" : "Off");
		ZChatOutput(buf, ZChat::CMT_SYSTEM);
	}
	if (CHECK_SETTING(bAutoTeamBalancing, false))
	{
		sprintf_safe(buf, "Team Balance%s%s", Changed ? " it has been " : ": ",
			pSetting->bAutoTeamBalancing ? "On" : "Off");
		ZChatOutput(buf, ZChat::CMT_SYSTEM);
	}
	if (CHECK_SETTING(nLimitLevel, 0))
	{
		sprintf_safe(buf, "Limit Level%s%d", Changed ? " changed to " : ": ",
			pSetting->nLimitLevel);
		ZChatOutput(buf, ZChat::CMT_SYSTEM);
	}
	if (CHECK_SETTING(bTeamAlternateSpawns, false))
	{
		sprintf_safe(buf, "Base Rotation%s%s", Changed ? " it has been " : ": ",
			pSetting->bTeamAlternateSpawns ? "On" : "Off");
		ZChatOutput(buf, ZChat::CMT_SYSTEM);
	}
#ifdef _SPEC 1
	if (CHECK_SETTING(bObserverEnabled, false))
	{
		sprintf_safe(buf, "Spectator Mode%s%s", Changed ? " it has been " : ": ",
			pSetting->bObserverEnabled ? "On" : "Off");
		ZChatOutput(buf, ZChat::CMT_SYSTEM);
	}
#endif
#ifndef _LIGHTURNOFF
	if (CHECK_SETTING(bLightMapTurn, false))
	{
		sprintf_safe(buf, "Forze LightMap%s%s", Changed ? " it has been " : ": ",
			pSetting->bLightMapTurn ? "Off" : "On");
		ZChatOutput(buf, ZChat::CMT_SYSTEM);
	}
#endif

#undef CHECK_SETTING

	JustJoinedStage = false;
	CreatedStage = false;
	LastStageSetting = *pSetting;
#endif
	ZApplication::GetGameInterface()->SerializeStageInterface();
}

void ZGameClient::OnStageRelayMapListUpdate(int nRelayMapType, int nRelayMapRepeatCount, void* pStageRelayMapListBlob)
{
	MComboBox* pCBRelayMapType = (MComboBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("Stage_RelayMapType");
	if (pCBRelayMapType)
		pCBRelayMapType->SetSelIndex(nRelayMapType);
	MComboBox* pCBRelayMapRepeatCount = (MComboBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("Stage_RelayMapRepeatCount");
	if (pCBRelayMapRepeatCount)
		pCBRelayMapRepeatCount->SetSelIndex(nRelayMapRepeatCount);

	MListBox* pRelaMapListBox = (MListBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("Stage_RelayMapListbox");
	if (pRelaMapListBox == NULL) return;

	RelayMap relayMapList[MAX_RELAYMAP_LIST_COUNT];
	for (int i = 0; i < MAX_RELAYMAP_LIST_COUNT; i++)
		relayMapList[i].nMapID = -1;
	// ���� �����̸� ����Ʈ�� ��� �����ش�.
	pRelaMapListBox->RemoveAll();
	int nRelayMapListCount = MGetBlobArrayCount(pStageRelayMapListBlob);
	for (int i = 0; i < nRelayMapListCount; ++i)
	{// �����̸� ����Ʈ�� �����͸� �߰����ش�.
		MTD_RelayMap* pNode = (MTD_RelayMap*)MGetBlobArrayElement(pStageRelayMapListBlob, i);
		RelayMapList* pRelayMapList = new RelayMapList(MGetMapDescMgr()->GetMapName(MGetMapDescMgr()->GetMapID(pNode->nMapID)), MBitmapManager::Get("Mark_X.bmp"));
		pRelaMapListBox->Add(pRelayMapList);
		relayMapList[i].nMapID = MGetMapDescMgr()->GetMapID(pNode->nMapID);
	}

	ZGetGameClient()->GetMatchStageSetting()->SetRelayMapListCount(nRelayMapListCount);
	ZGetGameClient()->GetMatchStageSetting()->SetRelayMapList(relayMapList);

	// ȭ��E����� �� �̹���E�����ϱ�E
	ZApplication::GetStageInterface()->SetStageRelayMapImage();
}

void ZGameClient::OnStageRelayMapElementUpdate(int nRelayMapType, int nRelayMapRepeatCount)
{
	MComboBox* pCombo = (MComboBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("Stage_RelayMapType");
	if (pCombo)
		pCombo->SetSelIndex(nRelayMapType);
	pCombo = (MComboBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("Stage_RelayMapRepeatCount");
	if (pCombo)
		pCombo->SetSelIndex(nRelayMapRepeatCount);
}

void ZGameClient::OnResponseStageSetting(const MUID& uidStage, void* pStageBlob, int nStageCount, void* pCharBlob,
	int nCharCount, STAGE_STATE nStageState, const MUID& uidMaster)
{
	if (GetStageUID() != uidStage) return;
	if (nStageCount <= 0 || nCharCount <= 0) return;

	MSTAGE_SETTING_NODE* pNode = (MSTAGE_SETTING_NODE*)MGetBlobArrayElement(pStageBlob, 0);
	UpdateStageSetting(pNode, nStageState, uidMaster);

	// Char Setting
	m_MatchStageSetting.ResetCharSetting();
	for (int i = 0; i < nCharCount; i++) {
		MSTAGE_CHAR_SETTING_NODE* pCharSetting = (MSTAGE_CHAR_SETTING_NODE*)MGetBlobArrayElement(pCharBlob, i);
		m_MatchStageSetting.UpdateCharSetting(pCharSetting->uidChar, pCharSetting->nTeam, pCharSetting->nState);
	}

	ZApplication::GetGameInterface()->SerializeStageInterface();
}

void ZGameClient::OnAgentError(int nError)
{
	if (ZGetGame()) {
		const MCOLOR ChatColor = MCOLOR(0xffffffff);
		ZChatOutput(ChatColor, "Agent Error : Agent not available");
	}
}

void ZGameClient::OnMatchNotify(unsigned int nMsgID)
{
	string strMsg;
	NotifyMessage(nMsgID, &strMsg);

	if ((nMsgID == MATCHNOTIFY_GAME_SPEEDHACK) ||
		(nMsgID == MATCHNOTIFY_GAME_MEMORYHACK))
	{
		ZGetGameInterface()->ShowMessage(strMsg.c_str());
	}

	ZChatOutput(MCOLOR(255, 70, 70), strMsg.data());


	/*
	GunzState nState = ZApplication::GetGameInterface()->GetState();
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	switch(nState) {
	case GUNZ_LOBBY:
		{
			MListBox* pWidget = (MListBox*)pResource->FindWidget("ChannelChattingOutput");
			if (pWidget) pWidget->Add(strMsg.data(), MCOLOR(255,70,70));
		}
		break;
	case GUNZ_STAGE:
		{
			MListBox* pWidget = (MListBox*)pResource->FindWidget("StageChattingOutput");
			if (pWidget) pWidget->Add(strMsg.data(), MCOLOR(255,70,70));
		}
		break;
	case GUNZ_GAME:
		{
			ZApplication::GetGameInterface()->OutputChatMsg(strMsg.data());
		}
		break;
	};
	*/
}
#ifdef _UPCHARCMD
void ZGameClient::OutputMessage_Sub(const char* szMessage, ...)
{
	va_list args;
	static char temp[1024];

	va_start(args, szMessage);
	vsprintf(temp, szMessage, args);

	ZChatOutput(MCOLOR(0xFFFFC600), temp);
	OutputToConsole(temp);

	va_end(args);
}
#endif
void ZGameClient::OutputMessage(const char* szMessage, MZMOMType nType)
{
	OutputToConsole(szMessage);
	ZChatOutput(MCOLOR(0xFFFFC600), szMessage);
}
//void GetVolumeID(std::string& VolumeID)
//{
//	DWORD Volume;
//	::GetVolumeInformation(L"c:\\", NULL, 12, &Volume, NULL, NULL, NULL, 10);
//
//	char szText[64];
//	_itoa_s(Volume, szText, 16);
//	VolumeID = szText;
//}
//bool GetMacAdress(std::string& s)
//{
//	IP_ADAPTER_INFO		Info[32];
//	PIP_ADAPTER_INFO	pInfo;
//
//	DWORD dwSize = sizeof(Info);
//
//	if (GetAdaptersInfo(Info, &dwSize) != NO_ERROR)
//		return FALSE;
//
//	for (pInfo = Info; pInfo; pInfo = pInfo->Next)
//	{
//		for (size_t i = 0; i < 6; i++)
//		{
//			char Out[10] = { 0 };
//			int32_t k = pInfo->Address[i];
//			sprintf_s(Out, "%s%02X", i ? "-" : "", k);
//			s += Out;
//		}
//	}
//	return TRUE;
//}
#ifdef _HWID
void ZGameClient::GetHWID(char* hdID)
{
	DWORD Hwid;
	::GetVolumeInformation("c:\\", NULL, 12, &Hwid, NULL, NULL, NULL, 10);

	char szText[64];
	//_itoa_s(Hwid, szText, 16);
	//strcat(strcpy(hdID, HIWORD(Hwid)), LOWORD(Hwid));
	sprintf(hdID, "%04X-%04X", HIWORD(Hwid), LOWORD(Hwid));

	//mlog("\n\n\n %04X-%04X \n\n\n", HIWORD(Hwid), LOWORD(Hwid));
	//return true;
}
#endif
int ZGameClient::OnConnected(SOCKET sock, MUID* pTargetUID, MUID* pAllocUID, unsigned int nTimeStamp)
{

	mlog("Server Connected\n");

	int ret = MMatchClient::OnConnected(sock, pTargetUID, pAllocUID, nTimeStamp);

	if (sock == m_ClientSocket.GetSocket())
	{
		char szID[256];
		char szPassword[256];
		ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
		MWidget* pWidget = pResource->FindWidget("LoginID");
		if (pWidget == NULL) return true;
		strcpy(szID, pWidget->GetText());
		pWidget = pResource->FindWidget("LoginPassword");
		if (pWidget == NULL) return true;
		strcpy(szPassword, pWidget->GetText());


		unsigned long nChecksum = 0;
		nChecksum = nChecksum ^ (*pAllocUID).High ^ (*pAllocUID).Low;

		char szEncryptMD5Value[MAX_MD5LENGH] = { 0, };
		GetEncryptMD5HashValue(szEncryptMD5Value);

#ifdef _HWID
		char Hwid[MAX_HWID_ENGH] = { 0, };
		GetHWID(Hwid);
		ZPostLogin(szID, szPassword, nChecksum, szEncryptMD5Value, Hwid);
#else
		ZPostLogin(szID, szPassword, nChecksum, szEncryptMD5Value);
#endif
		mlog("Login Posted\n");
	}
	else if (sock == m_AgentSocket.GetSocket())
	{

	}

	return ret;
}

void ZGameClient::GetEncryptMD5HashValue(char* szEncryptMD5Value)
{
	char filePath[MAX_PATH] = { 0, };
	GetModuleFileName(NULL, filePath, MAX_PATH);

	unsigned char szMD5Value[MAX_MD5LENGH] = { 0, };
	MMD5* pMD5 = new MMD5;
	if (pMD5->md5_file(filePath, szMD5Value) != 0)
	{
		return;
	}
	delete pMD5;
	char szEncryptValue[MAX_MD5LENGH] = { 0, };
	memcpy(szEncryptValue, szMD5Value, MAX_MD5LENGH);

	m_ServerPacketCrypter.Encrypt(szEncryptValue, MAX_MD5LENGH, (MPacketCrypterKey*)m_ServerPacketCrypter.GetKey());

	memcpy(szEncryptMD5Value, szEncryptValue, MAX_MD5LENGH);
}

bool ZGameClient::OnSockConnect(SOCKET sock)
{
	ZPOSTCMD0(MC_NET_ONCONNECT);
	return MMatchClient::OnSockConnect(sock);
}

bool ZGameClient::OnSockDisconnect(SOCKET sock)
{
	if (sock == m_ClientSocket.GetSocket()) {
		AgentDisconnect();
		// ·Î±×¾Æ¿ô ¹öÆ°Å¬¸¯½Ã ¼­¹ö¸®½ºÆ®·Î ÀÌµ¿(³Ý¸¶ºíµµ º¯°æ)
		ZChangeGameState(GUNZ_LOGIN);
		ZPOSTCMD0(MC_NET_ONDISCONNECT);

		ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
		MButton* pWidget = (MButton*)pResource->FindWidget("LoginOK");
		if (pWidget) pWidget->Enable(true);
		MWidget* pLogin = pResource->FindWidget("LoginFrame");
		if (pLogin) pLogin->Show(true);
		pLogin = pResource->FindWidget("Login_ConnectingMsg");
		if (pLogin) pLogin->Show(false);

		ZGetGameInterface()->m_bLoginTimeout = false;

	}
	else if (sock == m_AgentSocket.GetSocket()) {
	}

	return true;
}

void ZGameClient::OnSockError(SOCKET sock, SOCKET_ERROR_EVENT ErrorEvent, int& ErrorCode)
{
	MMatchClient::OnSockError(sock, ErrorEvent, ErrorCode);

	ZPOSTCMD1(MC_NET_ONERROR, MCmdParamInt(ErrorCode));

	if (ZApplication::GetInstance()->GetLaunchMode() == ZApplication::ZLAUNCH_MODE_NETMARBLE) {
		// �ݸ���ۡ�� �α���
		ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
		MLabel* pLabel = (MLabel*)pResource->FindWidget("NetmarbleLoginMessage");
		if (pLabel) {
			//			pLabel->SetText(MGetErrorString(MERR_CLIENT_CONNECT_FAILED));
			char szText[128] = { 0, };
			sprintf(szText, "%s\n", ZErrStr(MERR_CLIENT_CONNECT_FAILED));
			ZGetGameInterface()->ShowMessage(szText);
		}
	}
	else {
		ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
		MButton* pWidget = (MButton*)pResource->FindWidget("LoginOK");
		if (pWidget) pWidget->Enable(true);
		MWidget* pLogin = pResource->FindWidget("LoginFrame");
		if (pLogin) pLogin->Show(true);
		pLogin = pResource->FindWidget("Login_ConnectingMsg");
		if (pLogin) pLogin->Show(false);

		MLabel* pLabel = (MLabel*)pResource->FindWidget("LoginError");
		if (pLabel) {
			//			pLabel->SetText(MGetErrorString(MERR_CLIENT_CONNECT_FAILED)); 
			char szText[128] = { 0, };
			sprintf(szText, "%s\n", ZErrStr(MERR_CLIENT_CONNECT_FAILED));
			ZGetGameInterface()->ShowMessage(szText);

		}

		ZGetGameInterface()->m_bLoginTimeout = false;
	}
}

#include "MListBox.h"
class MCharListItem : public MListItem {
	MUID	m_uid;
	char	m_szName[32];
public:
	MCharListItem(MUID uid, char* szName) {
		m_uid = uid; strcpy(m_szName, szName);
	}
	virtual ~MCharListItem() {}
	virtual const char* GetString(void) { return m_szName; }
	MUID GetUID() { return m_uid; }
	char* GetName() { return m_szName; }

public:

};

int ZGameClient::FindListItem(MListBox* pListBox, const MUID& uid)
{
	for (int i = 0; i < pListBox->GetCount(); i++) {
		MCharListItem* pItem = (MCharListItem*)pListBox->Get(i);
		if (pItem->GetUID() == uid) return i;
	}
	return -1;
}

unsigned long int ZGameClient::GetGlobalClockCount(void)
{
	unsigned long int nLocalClock = GetClockCount();
	if (m_bIsBigGlobalClock) return (nLocalClock + m_nClockDistance);
	else return (nLocalClock - m_nClockDistance);
}

unsigned long int ZGetClockDistance(unsigned long int nGlobalClock, unsigned long int nLocalClock)
{
	if (nGlobalClock > nLocalClock) {
		return nGlobalClock - nLocalClock;
	}
	else {
		return nLocalClock + (UINT_MAX - nGlobalClock + 1);
	}
}

void ZGameClient::StartBridgePeer()
{
	SetBridgePeerFlag(false);
	SetBridgePeerCount(10);

	UpdateBridgePeerTime(0);
}

void ZGameClient::Tick(void)
{
	m_MatchStageSetting.AntiHack_CheckCrc();

	unsigned long int nClock = GetGlobalClockCount();

	m_EmblemMgr.Tick(nClock);

	if ((GetBridgePeerCount() > 0) && (GetBridgePeerFlag() == false)) {
#define CLOCK_BRIDGE_PEER	200
		if (nClock - m_tmLastBridgePeer > CLOCK_BRIDGE_PEER) {
			SetBridgePeerCount(GetBridgePeerCount() - 1);
			UpdateBridgePeerTime(nClock);
			CastStageBridgePeer(GetPlayerUID(), GetStageUID());
		}
	}

	if (GetUDPTestProcess()) {
#define CLOCK_UDPTEST	500
		static unsigned long nUDPTestTimer = 0;
		if (nClock - nUDPTestTimer > CLOCK_UDPTEST) {
			nUDPTestTimer = nClock;

			MMatchPeerInfoList* PeerList = GetPeers();
			for (MMatchPeerInfoList::iterator i = PeerList->begin(); i != PeerList->end(); i++) {
				MMatchPeerInfo* pPeer = (*i).second;
				if (pPeer->GetProcess()) {
					// Custom: Start testing only if IP and Port are not empty
					if (strlen(pPeer->szIP) > 0 && pPeer->nPort > 0)
					{
						MCommand* pCmd = CreateCommand(MC_PEER_UDPTEST, pPeer->uidChar);
						SendCommandByUDP(pCmd, pPeer->szIP, pPeer->nPort);
						delete pCmd;
					}
				}
			}

			UpdateUDPTestProcess();
		}
	}

	if ((GetAgentPeerCount() > 0) && (GetAgentPeerFlag() == false)) {
		static unsigned long tmLastAgentPeer = 0;
#define CLOCK_AGENT_PEER	200
		if (nClock - tmLastAgentPeer > CLOCK_AGENT_PEER) {
			SetAgentPeerCount(GetAgentPeerCount() - 1);
			CastAgentPeerConnect();
			tmLastAgentPeer = nClock;
		}
	}
}

void ZGameClient::OnResponseRecommandedChannel(const MUID& uidChannel)
{
	RequestChannelJoin(uidChannel);
}

void ZGameClient::OnBirdTest()
{
#ifdef _PUBLISH
	return;
#endif

	char szText[256];
	char szList[256]; szList[0] = '\0';


	int nCount = (int)m_ObjCacheMap.size();
	for (MMatchObjCacheMap::iterator itor = m_ObjCacheMap.begin(); itor != m_ObjCacheMap.end(); ++itor)
	{
		MMatchObjCache* pObj = (*itor).second;
		strcat(szList, pObj->GetName());
		strcat(szList, ", ");
	}

	sprintf(szText, "BirdTest: %d, %s", nCount, szList);
	MClient::OutputMessage(MZMOM_LOCALREPLY, szText);

	ZCharacterViewList* pWidget = ZGetCharacterViewList(GUNZ_STAGE);
	pWidget->RemoveAll();

	pWidget = ZGetCharacterViewList(GUNZ_LOBBY);
	pWidget->RemoveAll();

}

void ZGameClient::OnForcedEntryToGame()
{
	m_bLadderGame = false;
	m_bForcedEntry = true;
	m_bLadderWars = false;
	// Custom: Allow tunneling.
	//SetAllowTunneling(true);
	SetAllowTunneling(false);
	ZChangeGameState(GUNZ_GAME);
}

void ZGameClient::ClearStageSetting()
{
	m_bForcedEntry = false;

	m_MatchStageSetting.Clear();
}



void ZGameClient::OnLoadingComplete(const MUID& uidChar, int nPercent)
{
	if (ZGetGame())
	{
		ZCharacter* pCharacter = (ZCharacter*)ZGetGame()->m_CharacterManager.Find(uidChar);
		if (pCharacter != NULL)
		{
			MEMBER_SET_CHECKCRC(pCharacter->GetStatus(), nLoadingPercent, nPercent);
		}
	}
}


void ZGameClient::OnResponsePeerRelay(const MUID& uidPeer)
{
	// Custom: Bug fix if NAT fails
	if (ZGetGame() == NULL) return;

	string strNotify = "Unknown Notify";
	NotifyMessage(MATCHNOTIFY_NETWORK_NAT_ESTABLISH, &strNotify);

	char* pszName = "UnknownPlayer";
	MMatchPeerInfo* pPeer = FindPeer(uidPeer);
	if (pPeer) pszName = pPeer->CharInfo.szName;

	char szMsg[128];
	sprintf(szMsg, "%s : from %s", strNotify.c_str(), pszName);


	ZCharacter* pChar = (ZCharacter*)ZGetCharacterManager()->Find(uidPeer);
	if (pChar && pChar->IsAdminHide() && pChar->IsAdminHide())
		return;

	ZChatOutput(szMsg, ZChat::CMT_SYSTEM);
}

void ZGameClient::StartStageList()
{
	MCommand* pCmd = new MCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_STAGE_LIST_START), GetServerUID(), m_This);
	Post(pCmd);
}

void ZGameClient::StopStageList()
{
	MCommand* pCmd = new MCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_STAGE_LIST_STOP), GetServerUID(), m_This);
	Post(pCmd);
}

/*
void ZGameClient::SetChannelType(MCHANNEL_TYPE type)
{
	m_CurrentChannelType = type;
}
*/

void ZGameClient::StartChannelList(MCHANNEL_TYPE nChannelType)
{
	ZPostStartChannelList(GetPlayerUID(), (int)nChannelType);
}

void ZGameClient::StopChannelList()
{
	ZPostStopChannelList(GetPlayerUID());
}

void ZGameClient::ReleaseForcedEntry()
{
	m_bForcedEntry = false;
}

void ZGameClient::OnPlayerNotify(const char* szReason)
{
	ZApplication::GetGameInterface()->ShowMessage(szReason);
}

void ZGameClient::OnAdminAnnounce(const char* szMsg, const ZAdminAnnounceType nType)
{
	switch (nType)
	{
#ifdef _GLOBALANNOUNCE
	case ZAAT_CHAT:
		ZChatOutput(szMsg, ZChat::CMT_SYSTEM);
		m_GlobalAnnounce->SetGlobalMessage(szMsg);
		break;
#else
	case ZAAT_CHAT:
	{
		char szText[512];
		//			sprintf(szText, "%s : %s", "��E���", szMsg);
		ZTransMsg(szText, MSG_ADMIN_ANNOUNCE, 1, szMsg);
		ZChatOutput(szText, ZChat::CMT_SYSTEM);
	}
	break;
#endif
	case ZAAT_MSGBOX:
	{
		if (ZApplication::GetGameInterface()->GetState() != GUNZ_GAME)
		{
			ZApplication::GetGameInterface()->ShowMessage(szMsg);
		}
		else
		{
			// Custom: Fixed Gunz bug - popup announces showing in-game as normal text, not Green.
			char szText[512];
			ZTransMsg(szText, MSG_ADMIN_ANNOUNCE, 1, szMsg);
			ZChatOutput(szText, ZChat::CMT_SYSTEM);

			//ZChatOutput(szMsg);
		}
	}
	break;
	}
}

#ifdef _UPREWARDLEVEL
void ZGameClient::OnGameLevelUp(const MUID& uidChar, const int nBounty, const int nCash, const int nItemID)
#else
void ZGameClient::OnGameLevelUp(const MUID& uidChar)
#endif
{
	if (ZGetGame())
	{
		ZCharacter* pCharacter = (ZCharacter*)ZGetGame()->m_CharacterManager.Find(uidChar);
		if (pCharacter) {
			pCharacter->LevelUp();

			char temp[256] = "";
#ifdef _UPREWARDLEVEL
			char szMsg[255] = "";
			if (nItemID != 0)
			{
				MMatchItemDesc* pDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);
				if (pDesc != NULL)
				{
					const char* text = pDesc->m_pMItemName->Ref().m_szItemName;
					sprintf(szMsg, "%s has leveled up and received (%i bounty - %i cash - %s Item)", pCharacter->GetUserNameA(), (int)nBounty, (int)nCash, text);
				}

			}
			else
			{
				sprintf(szMsg, "%s has leveled up and received (%i bounty - %i cash)", pCharacter->GetUserNameA(), (int)nCash, (int)nBounty);
			}
#else
			ZTransMsg(temp, MSG_GAME_LEVEL_UP, 1, pCharacter->GetUserAndClanName());
			ZChatOutput(MCOLOR(ZCOLOR_GAME_INFO), temp);
			ZGetSoundEngine()->PlaySound("nar/NARUP", pCharacter->GetPosition()); //Levelup sound 
#endif
		}
	}
}

void ZGameClient::OnGameLevelDown(const MUID& uidChar)
{
	if (ZGetGame())
	{
		ZCharacter* pCharacter = (ZCharacter*)ZGetGame()->m_CharacterManager.Find(uidChar);
		if (pCharacter) {
			pCharacter->LevelDown();

			char temp[256] = "";
			ZTransMsg(temp, MSG_GAME_LEVEL_DOWN, 1, pCharacter->GetUserAndClanName());
			ZChatOutput(MCOLOR(ZCOLOR_GAME_INFO), temp);
			ZGetSoundEngine()->PlaySound("nar/NARDOWNEW", pCharacter->GetPosition()); //Levelup sound 
		}
	}
}
#ifdef _ANTISPAM
void ZGameClient::OnAdminWall(const char* szMsg, const char* szCharName)
{
	char szText[512];
	char szTitle[512];
	bool bShowMe = false;
	if (ZApplication::GetGameInterface()->GetState() != GUNZ_GAME)
	{
		bShowMe = false;
		sprintf(szText, "[Anti-Spam] %s : %s", szCharName, szMsg);
	}
	else if (!strstr(szCharName, "Clan War") && !strstr(szCharName, "Chat Limiter"))
	{
		bShowMe = true;
		sprintf(szText, "[Anti-Spam] %s : %s", szCharName, szMsg);

	}
	ZChatOutput(szText, ZChat::CMT_SYSTEM);
}
#endif
void ZGameClient::OnStaffHelp(const char* szMsg, const char* szUsr, const int UgradeID)
{
	if (ZApplication::GetGameInterface()->GetState() == GUNZ_GAME)
	{
		if (strstr(szMsg, "[!]"))
			ZGetGameInterface()->GetCombatInterface()->m_AdminMsg.OutputChatMsg(MCOLOR(255, 0, 0), szMsg);
		//else if(strstr(szMsg, "[REPORT:"))
		//ZGetGameInterface()->GetCombatInterface()->m_AdminMsg.OutputChatMsg(MCOLOR(255, 255, 255), szMsg);
		else
		{
			MCOLOR _color;
			GetUserGradeIDColor((MMatchUserGradeID)UgradeID, _color, "");
			char szChat[420];
			sprintf(szChat, "%s : ^9%s", szUsr, szMsg);
			ZGetGameInterface()->GetCombatInterface()->m_AdminMsg.OutputChatMsg(_color, szChat);
		}
	}
	else
	{
		if (strstr(szMsg, "[!]"))
			ZChatOutput(szMsg, ZChat::CMT_CURSOUR);
		//else if(strstr(szMsg, "[REPORT:"))
		//ZChatOutput(szMsg, ZChat::CMT_STAFFHELP);
		else
		{
			MCOLOR _color;
			GetUserGradeIDColor((MMatchUserGradeID)UgradeID, _color, "");
			char szChat[420];
			sprintf(szChat, "%s : ^9%s", szUsr, szMsg);
			ZChatOutput(_color, szChat);
		}
	}
}
void ZGameClient::OnResponseGameInfo(const MUID& uidStage, void* pGameInfoBlob, void* pRuleInfoBlob, void* pPlayerInfoBlob, bool bNew)
{
	if (ZGetGame() == NULL) return;

	// Game Info
	int nGameInfoCount = MGetBlobArrayCount(pGameInfoBlob);
	if (nGameInfoCount > 0) {
		if (bNew)
		{
			MTD_GameInfo_V2* pGameInfo = (MTD_GameInfo_V2*)MGetBlobArrayElement(pGameInfoBlob, 0);
			ZGetGame()->GetMatch()->SetTeamScore(MMT_RED, pGameInfo->nRedTeamScore);
			ZGetGame()->GetMatch()->SetTeamScore(MMT_BLUE, pGameInfo->nBlueTeamScore);
			ZGetGame()->GetMatch()->SetTeamKills(MMT_RED, pGameInfo->nRedTeamKills);
			ZGetGame()->GetMatch()->SetTeamKills(MMT_BLUE, pGameInfo->nBlueTeamKills);
		}
		else
		{
			MTD_GameInfo* pGameInfo = (MTD_GameInfo*)MGetBlobArrayElement(pGameInfoBlob, 0);
			ZGetGame()->GetMatch()->SetTeamScore(MMT_RED, pGameInfo->nRedTeamScore);
			ZGetGame()->GetMatch()->SetTeamScore(MMT_BLUE, pGameInfo->nBlueTeamScore);
			ZGetGame()->GetMatch()->SetTeamKills(MMT_RED, pGameInfo->nRedTeamKills);
			ZGetGame()->GetMatch()->SetTeamKills(MMT_BLUE, pGameInfo->nBlueTeamKills);
		}
	}

	// Player Info
	int nPlayerCount = MGetBlobArrayCount(pPlayerInfoBlob);

	for (int i = 0; i < nPlayerCount; i++)
	{
		MTD_GameInfoPlayerItem* pPlayerInfo = (MTD_GameInfoPlayerItem*)MGetBlobArrayElement(pPlayerInfoBlob, i);
		ZCharacter* pCharacter = (ZCharacter*)ZGetGame()->m_CharacterManager.Find(pPlayerInfo->uidPlayer);
		if (pCharacter == NULL) continue;

		if (pPlayerInfo->bAlive == true)
		{
			//			pCharacter->SetVisible(true);	// RAONHAJE: PeerOpened TEST
			pCharacter->Revival();
		}
		else
		{
			if ((ZGetGame()->GetMatch()->IsTeamPlay()) && (ZGetGame()->GetMatch()->GetRoundState() != MMATCH_ROUNDSTATE_FREE))
			{
				// ������ �濁E�׾�������E�׳� ��������E�ʴ´�. 
				// - �Բ� ������ �綁E0,0,0�� ���ִ°� �Ⱥ��̰� �Ϸ���E.
				pCharacter->ForceDie();
				pCharacter->SetVisible(false);
			}
		}

		pCharacter->GetStatus().CheckCrc();
		pCharacter->GetStatus().Ref().nKills = pPlayerInfo->nKillCount;
		pCharacter->GetStatus().Ref().nDeaths = pPlayerInfo->nDeathCount;

#ifdef _ICONCHAT
		pCharacter->GetStatus().Ref().isTalking;
#endif
		//		pCharacter->GetStatus()->nScore = (pPlayerInfo->nKillCount-pPlayerInfo->nDeathCount)*1000;
		pCharacter->GetStatus().MakeCrc();
	}


	// Rule Info
	// RuleInfo�� PlayerInfo �� ������ ������ �����Ѵ�. - ��E�����E�÷��̾�E������ �ٲ��ֱ�E����..
	int nRuleCount = MGetBlobArrayCount(pRuleInfoBlob);
	if (nRuleCount > 0) {
		MTD_RuleInfo* pRuleInfoHeader = (MTD_RuleInfo*)MGetBlobArrayElement(pRuleInfoBlob, 0);

		ZGetGame()->GetMatch()->OnResponseRuleInfo(pRuleInfoHeader);
	}
	CHECK_RETURN_CALLSTACK(OnResponseGameInfo);
}

void ZGameClient::OnObtainWorldItem(const MUID& uidChar, const int nItemUID)
{
	if (ZGetGame() == NULL) return;

	ZCharacter* pCharacter = (ZCharacter*)ZGetGame()->m_CharacterManager.Find(uidChar);
	if (pCharacter)
	{
		ZGetWorldItemManager()->ApplyWorldItem(nItemUID, pCharacter);

		ZWeapon* pWeapon = ZGetGame()->m_WeaponManager.GetWorldItem(nItemUID);

		ZWeaponItemKit* pItemKit = MDynamicCast(ZWeaponItemKit, pWeapon);
		if (pItemKit)
		{
			if (ZGetGameClient()->GetMatchStageSetting()->GetGameType() == MMATCH_GAMETYPE_SPY)
			{
				if (pItemKit->m_nWorldItemID == 200)
				{
					pItemKit->Explosion();
				}
			}
			pItemKit->m_bDeath = true;
		}
	}
}

void ZGameClient::OnSpawnWorldItem(void* pBlob)
{
	if (ZGetGame() == NULL) return;

	int nWorldItemCount = MGetBlobArrayCount(pBlob);

	ZWeaponItemKit* pItemKit = NULL;

	ZMovingWeapon* pMWeapon = NULL;
	ZWorldItem* pWorldItem = NULL;

	for (int i = 0; i < nWorldItemCount; i++)
	{
		MTD_WorldItem* pWorldItemNode = (MTD_WorldItem*)MGetBlobArrayElement(pBlob, i);

		pWorldItem = ZGetWorldItemManager()->AddWorldItem(pWorldItemNode->nUID, pWorldItemNode->nItemID,
			(MTD_WorldItemSubType)pWorldItemNode->nItemSubType,
			rvector((float)pWorldItemNode->x, (float)pWorldItemNode->y, (float)pWorldItemNode->z));

		pMWeapon = ZGetGame()->m_WeaponManager.UpdateWorldItem(pWorldItemNode->nItemID, rvector(pWorldItemNode->x, pWorldItemNode->y, pWorldItemNode->z));

		if (pWorldItem && (pItemKit = MDynamicCast(ZWeaponItemKit, pMWeapon)))
		{
			pItemKit->SetItemUID(pWorldItemNode->nUID);
			pWorldItem->m_bisDraw = false;
		}
	}
}

void ZGameClient::OnRemoveWorldItem(const int nItemUID)
{
	if (ZGetGame() == NULL) return;

	ZGetWorldItemManager()->DeleteWorldItem(nItemUID, true);

	ZWeapon* pWeapon = ZGetGame()->m_WeaponManager.GetWorldItem(nItemUID);
	ZWeaponItemKit* pItemKit = MDynamicCast(ZWeaponItemKit, pWeapon);
	if (pItemKit) { pItemKit->m_bDeath = true; }
}

void ZGameClient::OnNotifyActivatedTrapItemList(void* pBlob)
{
	if (ZGetGame() == NULL) return;

	// ������ ������ �������� ����E�ʻ� �ߵ��Ǿ�E�ִ� Ʈ�� ����� �˷��ִ� Ŀ�ǵ�E�ڵ鷯

	int numTrap = MGetBlobArrayCount(pBlob);

	rvector pos;
	ZObject* pOwner;
	for (int i = 0; i < numTrap; i++)
	{
		MTD_ActivatedTrap* pTrap = (MTD_ActivatedTrap*)MGetBlobArrayElement(pBlob, i);

		pos.x = (float)pTrap->x;
		pos.y = (float)pTrap->y;
		pos.z = (float)pTrap->z;

		pOwner = ZGetCharacterManager()->Find(pTrap->uidOwner);
		if (pOwner == NULL)
			continue;
		// ������E�ƽ���E�κ�������, Ʈ���� ��������E�������� �÷��̾��� Ʈ���� ��������. ������ ������ ���� �� Ʈ���� ��������E�ʴ� ���� �ȴ�.

		float fTimeActivated = ZGetGame()->GetTime() - pTrap->nTimeElapsed * 0.001f;

		ZGetGame()->m_WeaponManager.AddTrapAlreadyActivated(pos, fTimeActivated, pTrap->nItemID, pOwner);
	}
}

void ZGameClient::OnUserWhisper(char* pszSenderName, char* pszTargetName, char* pszMessage)
{
	// Custom: Ignore List check (Not self)
	if (ZGetGameClient()->IsUserIgnored(pszSenderName) && stricmp(pszSenderName, ZGetMyInfo()->GetCharName()))
		return;

	char szText[256];
	ZTransMsg(szText, MSG_GAME_WHISPER, 2, pszSenderName, pszMessage);

	// Custom: Disable duel chat block
	//if ( ZGetGame())
	//{
		//if ( (ZGetGame()->GetMatch()->GetMatchType() == MMATCH_GAMETYPE_DUEL)	&& !ZGetGame()->m_pMyCharacter->IsDie())
			//ZTransMsg( szText, MSG_GAME_WHISPER, 2, pszSenderName, ". . . . .");
	//}


	ZChatOutput(MCOLOR(ZCOLOR_CHAT_WHISPER), szText, ZChat::CL_CURRENT);

	// �������� �ӼӸ��� ��������� �⾁EѴ�.
	ZGetGameInterface()->GetChat()->SetWhisperLastSender(pszSenderName);

	// ����E�����߿� ä��â����E�ɼ��� �������� �濁E�ٽ� ���ش�.
	if ((ZApplication::GetGameInterface()->GetState() == GUNZ_GAME) && (ZGetGame()))
	{
		if (ZGetCombatInterface())
		{
			if (!ZGetConfiguration()->GetViewGameChat())
			{
				ZGetCombatInterface()->ShowChatOutput(true);
			}
		}
	}

}

void ZGameClient::OnChatRoomJoin(char* pszPlayerName, char* pszChatRoomName)
{
	char szText[256];
	//	sprintf(szText, "ä�ù�E'%s'�� '%s'���� �����ϼ̽��ϴ�.", pszChatRoomName, pszPlayerName);
	ZTransMsg(szText, MSG_LOBBY_WHO_CHAT_ROMM_JOIN, 2, pszChatRoomName, pszPlayerName);
	ZChatOutput(szText, ZChat::CMT_NORMAL, ZChat::CL_CURRENT);
}

void ZGameClient::OnChatRoomLeave(char* pszPlayerName, char* pszChatRoomName)
{
	char szText[256];
	//	sprintf(szText, "ä�ù�E'%s'���� '%s'���� �����ϼ̽��ϴ�.", pszChatRoomName, pszPlayerName);
	ZTransMsg(szText, MSG_LOBBY_WHO_CHAT_ROOM_EXIT, 2, pszChatRoomName, pszPlayerName);
	ZChatOutput(szText, ZChat::CMT_NORMAL, ZChat::CL_CURRENT);
}

void ZGameClient::OnChatRoomSelectWrite(char* pszChatRoomName)
{
	char szText[256];
	// 	sprintf(szText, "ä�ù�E'%s'�� ��E��մϴ�.", pszChatRoomName);
	ZTransMsg(szText, MSG_LOBBY_CHAT_ROOM_CHANGE, 1, pszChatRoomName);
	ZChatOutput(szText, ZChat::CMT_NORMAL, ZChat::CL_CURRENT);
}

void ZGameClient::OnChatRoomInvite(char* pszSenderName, char* pszRoomName)
{
	// Custom: Ignore List check (Not self)
	if (ZGetGameClient()->IsUserIgnored(pszSenderName) && stricmp(pszSenderName, ZGetMyInfo()->GetCharName()))
		return;

	char szLog[256];
	//	sprintf(szLog, "'%s' ���� ä�ù�E'%s'�� �ʴ�Eϼ̽��ϴ�.", pszSenderName, pszRoomName);
	ZTransMsg(szLog, MSG_LOBBY_WHO_INVITATION, 2, pszSenderName, pszRoomName);
	ZChatOutput(szLog, ZChat::CMT_NORMAL, ZChat::CL_CURRENT);

	SetChatRoomInvited(pszRoomName);
}

void ZGameClient::OnChatRoomChat(char* pszChatRoomName, char* pszPlayerName, char* pszChat)
{
	// Custom: Ignore List check (Not self)
	if (ZGetGameClient()->IsUserIgnored(pszPlayerName) && stricmp(pszPlayerName, ZGetMyInfo()->GetCharName()))
		return;

	char szText[256];
	//	sprintf(szText, "ä�ù�E%s) %s : %s", pszChatRoomName, pszPlayerName, pszChat);
	ZTransMsg(szText, MRESULT_CHAT_ROOM, 3, pszChatRoomName, pszPlayerName, pszChat);
	ZChatOutput(MCOLOR(ZCOLOR_CHAT_ROOMCHAT), szText, ZChat::CL_CURRENT);
}

void ZGameClient::RequestPrevStageList()
{
	int nStageCursor;
	ZRoomListBox* pRoomList =
		(ZRoomListBox*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Lobby_StageList");
	if (!pRoomList) return;

	nStageCursor = pRoomList->GetFirstStageCursor() - NUM_DISPLAY_ROOM;
	if (nStageCursor < 0) nStageCursor = 0;

	ZPostRequestStageList(m_uidPlayer, m_uidChannel, nStageCursor);

	int nPage = (nStageCursor / TRANS_STAGELIST_NODE_COUNT) + 1;
	ZApplication::GetGameInterface()->SetRoomNoLight(nPage);
}

void ZGameClient::RequestNextStageList()
{
	int nStageCursor;
	ZRoomListBox* pRoomList =
		(ZRoomListBox*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Lobby_StageList");
	if (!pRoomList) return;

	nStageCursor = pRoomList->GetLastStageCursor() + 1;
	if (nStageCursor > 100) nStageCursor = 100;

	ZPostRequestStageList(m_uidPlayer, m_uidChannel, nStageCursor);

	int nPage = (nStageCursor / TRANS_STAGELIST_NODE_COUNT) + 1;
	ZApplication::GetGameInterface()->SetRoomNoLight(nPage);
}

void ZGameClient::RequestStageList(int nPage)
{
	int nStageCursor;

	nStageCursor = (nPage - 1) * TRANS_STAGELIST_NODE_COUNT;
	if (nStageCursor < 0) nStageCursor = 0;
	else if (nStageCursor > 100) nStageCursor = 100;

	ZPostRequestStageList(m_uidPlayer, m_uidChannel, nStageCursor);
}

void ZGameClient::OnLocalReport119()
{
	/*
		ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
		MEdit* pReasonEdit = (MEdit*)pResource->FindWidget("112_ConfirmEdit");
		if (pReasonEdit)
		{
			pReasonEdit->SetText("");
			pReasonEdit->SetFocus();
		}

		MWidget* pWidget = pResource->FindWidget("112Confirm");
		if(pWidget!=NULL)
		{
			pWidget->Show(true);
	//		ZApplication::GetGameInterface()->SetCursorEnable(true);
		}
	*/
	ZApplication::GetGameInterface()->Show112Dialog(true);
}

int ZGameClient::ValidateRequestDeleteChar()
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	int nCharIndex = ZCharacterSelectView::GetSelectedCharacter();
	if ((nCharIndex < 0) || (nCharIndex >= MAX_CHAR_COUNT)) return ZERR_UNKNOWN;

	ZSelectCharacterInfo* pSelectCharInfo = &ZCharacterSelectView::m_CharInfo[nCharIndex];
	MTD_AccountCharInfo* pAccountCharInfo = &pSelectCharInfo->m_AccountCharInfo;
	MTD_CharInfo* pCharInfo = &pSelectCharInfo->m_CharInfo;

	// ����Eĳ���� ������ ���޾ƿ�����E������ ��E����.
	if (!pSelectCharInfo->m_bLoaded) return ZERR_UNKNOWN;

	// Ŭ���� ���ԵǾ�E������Eĳ���͸� ������ ��E����.
	if (pCharInfo->szClanName[0] != 0)
		return MSG_CLAN_PLEASE_LEAVE_FROM_CHAR_DELETE;

	// ĳ���������� ������E������ ��E����
	for (int i = 0; i < MMCIP_END; i++)
	{
		MMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(pCharInfo->nEquipedItemDesc[i]);
		if (pItemDesc)
		{
			if (pItemDesc->IsCashItem()) return MSG_CANNOT_DELETE_CHAR_FOR_CASHITEM;
		}
	}

	return ZOK;
}

void ZGameClient::RequestChannelJoin(const MUID& uidChannel)
{
	ZPostChannelRequestJoin(GetPlayerUID(), uidChannel);
}

void ZGameClient::RequestChannelJoin(const MCHANNEL_TYPE nChannelType, char* szChannelName)
{
	ZPostChannelRequestJoinFromChannelName(GetPlayerUID(), (int)nChannelType, szChannelName);
}

void ZGameClient::RequestGameSuicide()
{
	ZGame* pGame = ZGetGameInterface()->GetGame();
	if (!pGame) return;

	ZMyCharacter* pMyCharacter = pGame->m_pMyCharacter;
	if (!pMyCharacter) return;

	if ((!pMyCharacter->IsDie()) && (pGame->GetMatch()->GetRoundState() == MMATCH_ROUNDSTATE_PLAY))
	{
		pMyCharacter->SetLastDamageType(ZD_NONE);//��ź�� �й�E�ߴٰ�Ը �ȳ������~

		ZPostRequestSuicide(ZGetGameClient()->GetPlayerUID());
	}
}

void ZGameClient::OnResponseResult(const int nResult)
{
	if (nResult != MOK)
	{
		if (ZApplication::GetGameInterface()->GetState() == GUNZ_GAME)
		{
			ZChatOutput(MCOLOR(ZCOLOR_CHAT_SYSTEM), ZErrStr(nResult));
		}
		else
		{
			ZApplication::GetGameInterface()->ShowErrorMessage(nResult);
		}

	}
}

// ���߿� ���� ���
void blog(const char* pFormat, ...)
{
	char szBuf[256];

	va_list args;
	va_start(args, pFormat);
	vsprintf(szBuf, pFormat, args);
	va_end(args);

	strcat(szBuf, "\n");

	//	OutputDebugString(szBuf);
	if (ZApplication::GetGameInterface()->GetState() == GUNZ_LOBBY)
		ZChatOutput(szBuf, ZChat::CMT_SYSTEM, ZChat::CL_LOBBY);
	else if (ZApplication::GetGameInterface()->GetState() == GUNZ_STAGE)
		ZChatOutput(szBuf, ZChat::CMT_SYSTEM, ZChat::CL_STAGE);
}


// �ٸ� �÷��̾�E���� ����E
void ZGameClient::OnResponseCharInfoDetail(void* pBlob)
{
#ifndef _DEBUG	// �ϼ��ɶ���݁E
	return;
#endif

	MWidget* pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Characterinfo");
	if (pWidget)
		pWidget->Show();

	int nCount = MGetBlobArrayCount(pBlob);
	if (nCount != 1) return;

	MTD_CharInfo_Detail* pCharInfoDetail = (MTD_CharInfo_Detail*)MGetBlobArrayElement(pBlob, 0);

	/*
	  == �÷��̾�E���� ���̾˷α׿� ���̡�� �ϴ� �͵�E==
	   * ��ư: �Ӹ�, ���󰡱�E ģ���߰�, �ݱ�E
	   * ĳ���� �ڽ�Ƭ ȭ��E
	   * �Ϲ�������, ���������
	*/

	// �Ϲ����� ��
	blog("^9%s", ZMsg(MSG_CHARINFO_TITLE));
	blog("^9%s: ^1%s^9(%s)", ZMsg(MSG_CHARINFO_NAME),
		pCharInfoDetail->szName,
		ZGetSexStr(MMatchSex(pCharInfoDetail->nSex), true));
	char sztemp[256];
	if (strcmp(pCharInfoDetail->szClanName, "") == 0)
		strcpy(sztemp, "---");
	else
		sprintf(sztemp, "%s(%s)", pCharInfoDetail->szClanName, ZGetClanGradeStr(pCharInfoDetail->nClanGrade));
	blog("^9%s: %s", ZMsg(MSG_CHARINFO_CLAN), sztemp);
	blog("^9%s: %d", ZMsg(MSG_CHARINFO_LEVEL), pCharInfoDetail->nLevel);
	int nWinPercent = (int)((float)pCharInfoDetail->nKillCount / (float)(pCharInfoDetail->nKillCount + pCharInfoDetail->nDeathCount) * 100.0f);
	blog("^9%s: %d%s/%d%s(%d%%)", ZMsg(MSG_CHARINFO_WINPERCENT),
		pCharInfoDetail->nKillCount,
		ZMsg(MSG_CHARINFO_WIN),
		pCharInfoDetail->nDeathCount,
		ZMsg(MSG_CHARINFO_LOSE),
		nWinPercent);
	ZGetTimeStrFromSec(sztemp, pCharInfoDetail->nConnPlayTimeSec);
	blog("^9%s: %s", ZMsg(MSG_CHARINFO_CONNTIME), sztemp);
	blog("");

	//	blog("�̸�: %s", pCharInfoDetail->szName);
	//	blog("Ŭ��: %s", pCharInfoDetail->szClanName);
	//	blog("Ŭ����å: %s", ZGetClanGradeStr(pCharInfoDetail->nClanGrade));
	//	blog("����: %s", ZGetSexStr(MMatchSex(pCharInfoDetail->nSex), true));
	//	blog("����: %d", pCharInfoDetail->nLevel);
	//	blog("����ġ: %d", pCharInfoDetail->nXP);
	//	blog("�ٿ��: %d", pCharInfoDetail->nBP);
	//	blog("Kill/Death: %d/%d", pCharInfoDetail->nKillCount, pCharInfoDetail->nDeathCount);

	//	char sztemp[256];
	//	ZGetTimeStrFromSec(sztemp, pCharInfoDetail->nConnPlayTimeSec);
	//	blog("�������ӽð�: %s", sztemp);

	//	ZGetTimeStrFromSec(sztemp, pCharInfoDetail->nTotalPlayTimeSec);
	//	blog("�����ӽð�: %s", sztemp);

		// ������� ��
	//	blog("=���������=================");

	//	blog(" �� ������ ������ �̸� ����...");
}

void ZGameClient::OnNotifyCallVote(const char* pszDiscuss, const char* pszArg)
{
	//	ZGetGameInterface()->GetCombatInterface()->GetVoteInterface()->ShowVote(true);

	SetVoteInProgress(true);
	SetCanVote(true);

	char szText[256] = "";
	if (stricmp(pszDiscuss, "joke") == 0) {
		ZTransMsg(szText, MSG_VOTE_START, 1, pszArg);
		ZChatOutput(szText, ZChat::CMT_SYSTEM, ZChat::CL_CURRENT);
	}
	else if (stricmp(pszDiscuss, "kick") == 0) {
		sprintf(m_szVoteText, ZMsg(MSG_VOTE_KICK), pszArg);
		ZChatOutput(szText, ZChat::CMT_SYSTEM, ZChat::CL_CURRENT);
	}
}

void ZGameClient::OnNotifyVoteResult(const char* pszDiscuss, int nResult)
{
	if (ZGetGameInterface()->GetCombatInterface() == NULL)
		return;

	ZGetGameInterface()->GetCombatInterface()->GetVoteInterface()->ShowTargetList(false);

	SetVoteInProgress(false);
	SetCanVote(false);

	//	char szText[256] = "";
	if (nResult == 0) {
		ZChatOutput(ZMsg(MSG_VOTE_REJECTED), ZChat::CMT_SYSTEM, ZChat::CL_CURRENT);
	}
	else if (nResult == 1) {
		ZChatOutput(ZMsg(MSG_VOTE_PASSED), ZChat::CMT_SYSTEM, ZChat::CL_CURRENT);
	}
}

void ZGameClient::OnVoteAbort(const int nMsgCode)
{
	ZChatOutput(ZMsg(nMsgCode), ZChat::CMT_SYSTEM, ZChat::CL_CURRENT);
}

void ZGameClient::RequestOnLobbyCreated()
{
	ZPostRequestStageList(GetPlayerUID(), GetChannelUID(), 0);
	ZPostRequestChannelPlayerList(GetPlayerUID(), GetChannelUID(), 0);

	// Custom: Auto-load friend list
	ZPostFriendList();
}

void ZGameClient::RequestOnGameDestroyed()
{
	// ���� �ٲ�E�������� ��û�Ѵ�.
	ZPostRequestMySimpleCharInfo(ZGetGameClient()->GetPlayerUID());

	// ���� �ٲ�EŬ�� ������ ��û�Ѵ�.
	if ((GetServerMode() == MSM_CLAN) && (GetChannelType() == MCHANNEL_TYPE_CLAN))
	{
		ZPostRequestClanInfo(GetPlayerUID(), m_szChannel);
	}
}

// ���󰡱�E��û�� ��E� ������ ������ ������ ä��â�� �������.
// [ IN ] : ����� �޽���E
void ZGameClient::OnFollowResponse(const int nMsgID)
{
	ZGetGameInterface()->GetChat()->Clear(ZChat::CL_LOBBY);
	const char* pszMsg = ZErrStr(nMsgID);
	if (0 == pszMsg)
		return;

	ZChatOutput(pszMsg, ZChat::CMT_SYSTEM, ZChat::CL_LOBBY);
}// OnFollowResponse

void ZGameClient::OnClanResponseEmblemURL(unsigned int nCLID, unsigned int nEmblemChecksum, const char* szEmblemURL)
{
	char szFullURL[2048] = "";
	//sprintf(szFullURL, "http://cwfile.netmarble.com%s", szEmblemURL);

	// Custom: Disable NHN Auth
	/*
#ifdef LOCALE_NHNUSA

	if ( ((ZNHN_USAAuthInfo*)(ZGetLocale()->GetAuthInfo()))->IsAlpha())
		sprintf( szFullURL, "%s%s", Z_LOCALE_TEMBLEM_URL, szEmblemURL);
	else
		sprintf( szFullURL, "%s%s", Z_LOCALE_EMBLEM_URL, szEmblemURL);
#else
	*/
	sprintf(szFullURL, "%s%s", Z_LOCALE_EMBLEM_URL, szEmblemURL);
	//#endif

#ifdef _DEBUG
	mlog("ZGameClient::OnClanResponseEmblemURL. full url : %s\n", szFullURL);
#endif

	// ����ӽ url�� ��E���·� ���� �濁E� �ִ� �� ����.
	// �׷���E�ּ� �޺κ��� �ϼ��� �ȵǱ�E������ ��������Ʈ ��E��� ��û�ϰ� �Ǵµ� �̷���E����ӽ���ϼ���E���ϰ� Ŀ���ٰ�E�Ѵ�..
	if (0 == strlen(szEmblemURL)) {
		mlog("Emblem url is null! clanID(%d)\n", nCLID);
		return;
	}

	m_EmblemMgr.ProcessEmblem(nCLID, szFullURL, nEmblemChecksum);
}

void ZGameClient::OnClanEmblemReady(unsigned int nCLID, const char* szURL)
{
	ZGetEmblemInterface()->ReloadClanInfo(nCLID);

	if (ZGetNetRepository()->GetClanInfo()->nCLID == nCLID) {
		ZIDLResource* pRes = ZApplication::GetGameInterface()->GetIDLResource();
		MPicture* pPicture = (MPicture*)pRes->FindWidget("Lobby_ClanInfoEmblem");
		if (pPicture)
			pPicture->SetBitmap(ZGetEmblemInterface()->GetClanEmblem(nCLID));
	}
}


void ZGameClient::OnExpiredRentItem(void* pBlob)
{
	int nBlobSize = MGetBlobArrayCount(pBlob);

	char szText[1024];
	sprintf(szText, "%s\n", ZMsg(MSG_EXPIRED));

	for (int i = 0; i < nBlobSize; i++)
	{
		unsigned long int* pExpiredItemID = (unsigned long int*)MGetBlobArrayElement(pBlob, i);

		char szItemText[256];

		MMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(*pExpiredItemID);
		if (pItemDesc)
		{
			sprintf(szItemText, "[%d] %s\n", i + 1, pItemDesc->m_pMItemName->Ref().m_szItemName);
			if ((strlen(szText) + strlen(szItemText)) <= 1022) strcat(szText, szItemText);
		}
	}

	ZApplication::GetGameInterface()->ShowMessage(szText);
}


bool ZGameClient::CreateUPnP(unsigned short nUDPPort)
{
	if (!m_pUPnP)
		m_pUPnP = new UPnP;

	//////////////////////////////////////////////////////////////////////////
	/// UPnP Enable
	// Activate Port
	if (m_pUPnP->Create(nUDPPort))
	{
		TRACE("UPnP: Port: %d\n", nUDPPort);
		mlog("%d upnp port forward initialized.\n", nUDPPort);
		return true;
	}
	else
	{
		// Failed: Use Default Port
		TRACE("UPnP: Failed to forward port\n");
		//		Sleep(1000);
	}
	return false;
}

bool ZGameClient::DestroyUPnP()
{
	if (m_pUPnP)
	{
		m_pUPnP->Destroy();
		delete m_pUPnP;
	}

	return true;
}



//////////////////////////////////////////////////////////////////////////


void ZGameClient::OnBroadcastDuelRenewVictories(const char* pszChampionName, const char* pszChannelName, int nRoomno, int nVictories)
{
	char szText[256];
	char szVic[32], szRoomno[32];

	sprintf(szVic, "%d", nVictories);
	sprintf(szRoomno, "%d", nRoomno);

	ZTransMsg(szText, MSG_DUEL_BROADCAST_RENEW_VICTORIES, 4, pszChampionName, pszChannelName, szRoomno, szVic);

	ZChatOutput(szText, ZChat::CMT_BROADCAST);
}

void ZGameClient::OnBroadcastDuelInterruptVictories(const char* pszChampionName, const char* pszInterrupterName, int nVictories)
{
	char szText[256];
	char szVic[32];
	sprintf(szVic, "%d", nVictories);
	ZTransMsg(szText, MSG_DUEL_BROADCAST_INTERRUPT_VICTORIES, 3, pszChampionName, pszInterrupterName, szVic);

	ZChatOutput(szText, ZChat::CMT_BROADCAST);
}

void ZGameClient::OnRecieveGambleItem(unsigned int nRecvItem, unsigned int nCnt, unsigned int nTime)
{
	char szText[256];
	char szName[256];

	MMatchItemDesc* pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(nRecvItem);
	if (pItemDesc)
	{
		sprintf(szName, "%s (x%d)", pItemDesc->m_pMItemName->Ref().m_szItemName, nCnt);
	}
	MQuestItemDesc* pQuestItemDesc = GetQuestItemDescMgr().FindQItemDesc(nRecvItem);
	if (pQuestItemDesc)
	{
		sprintf(szName, "%s (x%d)", pQuestItemDesc->m_szQuestItemName, nCnt);
	}

	if (!pItemDesc && !pQuestItemDesc)
		return;

	ZTransMsg(szText, MSG_RECEIVE_GAMBLE_ITEM, 1, szName);
	ZApplication::GetGameInterface()->ShowMessage(szText);
}


void ZGameClient::OnResponseUpdateStageEquipLook(const MUID& uidPlayer, const int nParts, const int nItemID)
{
	MMatchObjCacheMap::iterator itFind = m_ObjCacheMap.find(uidPlayer);
	if (m_ObjCacheMap.end() == itFind)
	{
		return;
	}

	MMatchObjCache* pObjCache = itFind->second;

	pObjCache->GetCostume()->nEquipedItemID[nParts] = nItemID;

#ifdef UPDATE_STAGE_CHARVIEWER
	auto StageCharViewer = ZFindWidgetAs<ZCharacterView>("Stage_Charviewer");
	if (StageCharViewer && StageCharViewer->m_Info.UID == uidPlayer)
	{
		StageCharViewer->SetParts(MMatchCharItemParts(nParts), nItemID);
	}
#endif
}

void ZGameClient::OnPrepareRun(void)
{
}

bool ZGameClient::IsUDPCommandValidationCheck(int nCommandID)
{
	return !m_UPDCommadHackShield.IsDeniedCommand(nCommandID);
}

void ZGameClient::OnAdminResponseKickPlayer(int nResult)
{
	if (IsAdminGrade(ZGetMyInfo()->GetUGradeID()))
	{
		if (nResult != MOK) {
			const char* strError = ZErrStr(nResult);
			if (strError) {
				ZChatOutput(strError, ZChat::CMT_NORMAL, ZChat::CL_LOBBY, MCOLOR(255, 128, 64));
			}
		}
		else {
#ifdef _DEBUG
			ZChatOutput("Success", ZChat::CMT_NORMAL, ZChat::CL_LOBBY, MCOLOR(255, 128, 64));
#endif
		}
	}
}

void ZGameClient::OnAdminResponseBlockPlayer(int nResult)
{
	if (IsAdminGrade(ZGetMyInfo()->GetUGradeID()))
	{
		if (nResult != MOK) {
			const char* strError = ZErrStr(nResult);
			if (strError) {
				ZChatOutput(strError, ZChat::CMT_NORMAL, ZChat::CL_LOBBY, MCOLOR(255, 128, 64));
			}
		}
		else {
#ifdef _DEBUG
			ZChatOutput("Success", ZChat::CMT_NORMAL, ZChat::CL_LOBBY, MCOLOR(255, 128, 64));
#endif
		}
	}
}

void ZGameClient::OnAdminResponseMutePlayer(int nResult)
{
	if (IsAdminGrade(ZGetMyInfo()->GetUGradeID())) {
		if (nResult != MOK) {
			const char* strError = ZErrStr(nResult);
			if (strError) {
				ZChatOutput(strError, ZChat::CMT_NORMAL, ZChat::CL_LOBBY, MCOLOR(255, 128, 64));
			}
		}
		else {
#ifdef _DEBUG
			ZChatOutput("Success", ZChat::CMT_NORMAL, ZChat::CL_LOBBY, MCOLOR(255, 128, 64));
#endif
		}
	}
	else {
		if (nResult == MOK) {
			const char* strMsg = ZMsg(MSG_CHAT_MUTE_BY_ADMIN);
			if (strMsg) {
				ZChatOutput(strMsg, ZChat::CMT_NORMAL, ZChat::CL_LOBBY);
			}
		}
	}
}

void ZGameClient::OnMedalsReceivedResponse(const MUID& uidObj)
{
	// Invoke ZCharacter
	// Find who is
	// Then set
}

// Custom: Ignore List

bool ZGameClient::IsUserIgnored(const char* szCharName)
{
	for (list<string>::iterator it = m_IgnoreList.begin(); it != m_IgnoreList.end(); ++it)
	{
		if (!stricmp(szCharName, (*it).c_str()))
			return true;
	}

	return false;
}

void ZGameClient::SetIgnoreUser(const char* szCharName, bool bIgnore)
{
	if (bIgnore)
	{
		if (!IsUserIgnored(szCharName))
			m_IgnoreList.push_back(szCharName);
	}
	else
	{
		if (!IsUserIgnored(szCharName))
			return;

		for (list<string>::iterator it = m_IgnoreList.begin(); it != m_IgnoreList.end(); )
		{
			if (!stricmp(szCharName, (*it).c_str()))
			{
				it = m_IgnoreList.erase(it);
				break;
			}
			else
				++it;
		}
	}
}

