#include "stdafx.h"
#include "MMatchServer.h"
#include "MSharedCommandTable.h"
#include "MErrorTable.h"
#include "MBlobArray.h"
#include "MObject.h"
#include "MMatchObject.h"
#include "MMatchItem.h"
#include "MAgentObject.h"
#include "MMatchNotify.h"
#include "Msg.h"
#include "MMatchObjCache.h"
#include "MMatchStage.h"
#include "MMatchTransDataType.h"
#include "MMatchFormula.h"
#include "MMatchConfig.h"
#include "MCommandCommunicator.h"
#include "MMatchShop.h"
#include "MMatchTransDataType.h"
#include "MDebug.h"
#include "MMatchAuth.h"
#include "MMatchStatus.h"
#include "MAsyncDBJob.h"
#include "MMatchTransDataType.h"
#include "MMatchAntiHack.h"

time_t fechaHora = time(0);
tm* fhLocal = localtime(&fechaHora);

void MMatchServer::OnAdminTerminal(const MUID& uidAdmin, const char* szText)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if (pObj == NULL) return;

	// ｰ・ｮﾀﾚ ｱﾇﾇﾑﾀｻ ｰ｡ﾁ・ｻ邯ﾌ ｾﾆｴﾏｸ・ｿｬｰ眤ｻ ｲﾂｴﾙ.
	if (!IsAdminGrade(pObj))
	{
//		DisconnectObject(uidAdmin);		
		return;
	}

	char szOut[32768]; szOut[0] = 0;

	if (m_Admin.Execute(uidAdmin, szText))
	{
		MCommand* pNew = CreateCommand(MC_ADMIN_TERMINAL, MUID(0,0));
		pNew->AddParameter(new MCmdParamUID(MUID(0,0)));
		pNew->AddParameter(new MCmdParamStr(szOut));
		RouteToListener(pObj, pNew);
	}
}

void MMatchServer::OnPlayerNotify(const MUID& uidChar,const char* szReason)
{
	MMatchObject* pObj = GetObject(uidChar);
	if (!pObj) return;

	MCommand* pCmd = CreateCommand(MC_SERVER_ANNOUNCE, MUID(0, 0));
	pCmd->AddParameter(new MCmdParamUID(pObj->GetUID()));
	pCmd->AddParameter(new MCmdParamStr(szReason));

	RouteToListener(pObj, pCmd);
}

void MMatchServer::OnAdminAnnounce(const MUID& uidAdmin, const char* szChat, unsigned long int nType)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if (pObj == NULL) return;

	// ｰ・ｮﾀﾚ ｱﾇﾇﾑﾀｻ ｰ｡ﾁ・ｻ邯ﾌ ｾﾆｴﾏｸ・ｿｬｰ眤ｻ ｲﾂｴﾙ.
	if (!IsAdminGrade(pObj) && pObj->GetAccountInfo()->m_nUGrade != MMUG_EVENTTEAM)
	{
//		DisconnectObject(uidAdmin);		
		return;
	}

	DWORD dwTime = GetGlobalClockCount();
	if (dwTime - pObj->GetLastAnnounceTime() < 5000)
	{
		Announce(pObj, "Please wait 5 seconds before announcing again.");
		return;
	}

	pObj->SetLastAnnounceTime(dwTime);

	// Custom: Updated buffer length to 512 and append admin name to notice.
	char szMsg[512]; // 256

	sprintf( szMsg, "%s : %s", pObj->GetName(), szChat );
	//strcpy(szMsg, szChat);

	MCommand* pCmd = CreateCommand(MC_ADMIN_ANNOUNCE, MUID(0,0));
	pCmd->AddParameter(new MCmdParamUID(uidAdmin));
	pCmd->AddParameter(new MCmdParamStr(szMsg));
	pCmd->AddParameter(new MCmdParamUInt(nType));

	RouteToAllClient(pCmd);
}



void MMatchServer::OnAdminRequestServerInfo(const MUID& uidAdmin)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if (pObj == NULL) return;

	// ｰ・ｮﾀﾚ ｱﾇﾇﾑﾀｻ ｰ｡ﾁ・ｻ邯ﾌ ｾﾆｴﾏｸ・ｿｬｰ眤ｻ ｲﾂｴﾙ.
	if (!IsAdminGrade(pObj))
	{
//		DisconnectObject(uidAdmin);		
		return;
	}

	// ｼｭｹ・ﾁ､ｺｸ ｺｸｿｩﾁﾖｴﾂｰﾍ ｾﾆﾁ・ｾﾈｳﾖｾ惕ｽ
/*
	MCommand* pNew = CreateCommand(MC_MATCH_ANNOUNCE, MUID(0,0));
	pNew->AddParameter(new MCmdParamUInt(0));

	RouteToListener(pObj, pNew);
*/
}



void MMatchServer::OnAdminServerHalt(const MUID& uidAdmin)
{
	LOG(LOG_PROG, "OnAdminServerHalt(...) Called");

	MMatchObject* pObj = GetObject(uidAdmin);
	if (pObj == NULL) return;

	MMatchUserGradeID nGrade = pObj->GetAccountInfo()->m_nUGrade;

	// ｰ・ｮﾀﾚ ｱﾇﾇﾑﾀｻ ｰ｡ﾁ・ｻ邯ﾌ ｾﾆｴﾏｸ・ｹｫｽﾃ.
	if ((nGrade != MMUG_ADMIN) && (nGrade != MMUG_DEVELOPER)) return;

	// Shutdown ｽﾃﾀﾛ	
	m_MatchShutdown.Start(GetGlobalClockCount());	
}

// ｼｭｹ｡ｼｭ ｸﾞｴｺｷﾎｸｸ ｾｲｴﾂ ｸ昞ﾉｾ・.
void MMatchServer::OnAdminServerHalt()
{
	LOG(LOG_PROG, "OnAdminServerHalt() Called");

	// Shutdown ｽﾃﾀﾛ	
	m_MatchShutdown.Start(GetGlobalClockCount());	
}

void MMatchServer::OnAdminRequestUpdateAccountUGrade(const MUID& uidAdmin, const char* szPlayer)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if (pObj == NULL) return;

	// ｰ・ｮﾀﾚ ｱﾇﾇﾑﾀｻ ｰ｡ﾁ・ｻ邯ﾌ ｾﾆｴﾏｸ・ｿｬｰ眤ｻ ｲﾂｴﾙ.
	if (!IsAdminGrade(pObj))
	{
//		DisconnectObject(uidAdmin);		
		return;
	}

	int nRet = MOK;

	if ((strlen(szPlayer)) < 2) return;
	MMatchObject* pTargetObj = GetPlayerByName(szPlayer);
	if (pTargetObj == NULL) return;



/*
	MCommand* pNew = CreateCommand(MC_ADMIN_REQUEST_UPDATE_ACCOUNT_UGRADE, MUID(0,0));
	pNew->AddParameter(new MCmdParamUInt(nRet));
	RouteToListener(pObj, pNew);
*/
}

void MMatchServer::OnAdminPingToAll(const MUID& uidAdmin)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if (pObj == NULL) return;

	// ｰ・ｮﾀﾚ ｱﾇﾇﾑﾀｻ ｰ｡ﾁ・ｻ邯ﾌ ｾﾆｴﾏｸ・ｿｬｰ眤ｻ ｲﾂｴﾙ.
	if (!IsAdminGrade(pObj))
	{
//		DisconnectObject(uidAdmin);		
		return;
	}

	MCommand* pNew = CreateCommand(MC_NET_PING, MUID(0,0));
	pNew->AddParameter(new MCmdParamUInt(GetGlobalClockCount()));
	RouteToAllConnection(pNew);
}


void MMatchServer::OnAdminRequestSwitchLadderGame(const MUID& uidAdmin, const bool bEnabled)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if (!IsEnabledObject(pObj)) return;

	// ｰ・ｮﾀﾚ ｱﾇﾇﾑﾀｻ ｰ｡ﾁ・ｻ邯ﾌ ｾﾆｴﾏｸ・ｿｬｰ眤ｻ ｲﾂｴﾙ.
	if (!IsAdminGrade(pObj))
	{
//		DisconnectObject(uidAdmin);		
		return;
	}

	
	MGetServerConfig()->SetEnabledCreateLadderGame(bEnabled);


	char szMsg[256] = "Action completed.";
	Announce(pObj, szMsg);
}

void MMatchServer::OnAdminHide(const MUID& uidAdmin)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if (!IsEnabledObject(pObj)) return;

	// ｰ・ｮﾀﾚ ｱﾇﾇﾑﾀｻ ｰ｡ﾁ・ｻ邯ﾌ ｾﾆｴﾏｸ・ｿｬｰ眤ｻ ｲﾂｴﾙ.
	if (!IsAdminGrade(pObj) && pObj->GetAccountInfo()->m_nUGrade != MMUG_EVENTTEAM)
	{
//		DisconnectObject(uidAdmin);		
		return;
	}

#if defined(LOCALE_NHNUSA) || defined(_DEBUG)
	m_HackingChatList.Init();
	mlog( "reload hacking chat list.\n" );
#endif

	if (pObj->CheckPlayerFlags(MTD_PlayerFlags_AdminHide)) {
		pObj->SetPlayerFlag(MTD_PlayerFlags_AdminHide, false);
		Announce(pObj, "Now Revealing...");
	} else {
		pObj->SetPlayerFlag(MTD_PlayerFlags_AdminHide, true);
		Announce(pObj, "Now Hiding...");
	}
}

void MMatchServer::OnAdminResetAllHackingBlock( const MUID& uidAdmin )
{
	MMatchObject* pObj = GetObject( uidAdmin );
	if( (0 != pObj) && IsAdminGrade(pObj) )
	{
		GetDBMgr()->AdminResetAllHackingBlock();
	}
}

/*void MMatchServer::OnAdminKickAll(const MUID& uidAdmin)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if (!pObj) return;
	if (!IsAdminGrade(pObj) && pObj->GetAccountInfo()->m_nUGrade != MMUG_EVENTTEAM) return;
	MMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (!pStage) return;

	char szAnnounce[31];
	for (auto itor = pStage->GetObjBegin(); itor != pStage->GetObjEnd(); ++itor)
	{
		MMatchObject* pTargetObj = (MMatchObject*)(*itor).second;

		if (pTargetObj)
		{
			if (pObj->GetStageUID() != pTargetObj->GetStageUID()) continue;
			if (IsAdminGrade(pTargetObj) || pTargetObj->GetAccountInfo()->m_nUGrade == MMUG_EVENTTEAM) continue;

			if (pTargetObj->GetPlace() == MMP_BATTLE)
			{
				StageLeaveBattle(pTargetObj->GetUID(),false,true);
			}
			else if (pTargetObj->GetPlace() == MMP_STAGE)
			{
				StageLeave(pTargetObj->GetUID());
			}
		}
		return;
	}
	sprintf_s(szAnnounce, sizeof(szAnnounce), "All players have been kicked.");
	Announce(pObj, szAnnounce);
}*/

void MMatchServer::OnAdminRequestKickPlayer(const MUID& uidAdmin, const char* szPlayer)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if (pObj == NULL)			return;
	if (!IsAdminGrade(pObj) && pObj->GetAccountInfo()->m_nUGrade != MMUG_EVENTTEAM)	return;
	if ((strlen(szPlayer)) < 2) return;

	int nRet = MOK;

	MMatchObject* pTargetObj = GetPlayerByName(szPlayer);
	char szCmd[128];
	if (pTargetObj != NULL)
	{
		// Notify Message ﾇﾊｿ・-> ｰ・ｮﾀﾚ ﾀ・・- ﾇﾘｰ・ﾆｯｺｰﾇﾑ ｸﾞｼｼﾁ・ﾇﾊｿ・ｾｽ)
		Disconnect(pTargetObj->GetUID());
	}
	else {
		nRet = MERR_ADMIN_NO_TARGET;
	}


	MCommand* pNew = CreateCommand(MC_ADMIN_RESPONSE_KICK_PLAYER, MUID(0, 0));
	pNew->AddParameter(new MCmdParamInt(nRet));
	RouteToListener(pObj, pNew);
}

void MMatchServer::OnAdminRequestMutePlayer(const MUID& uidAdmin, const char* szPlayer, const int nPenaltyHour)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if (pObj == NULL)			return;	
	if (!IsAdminGrade(pObj))	return;
	if ((strlen(szPlayer)) < 2) return;

	int nRet = MOK;
	MMatchObject* pTargetObj = GetPlayerByName(szPlayer);	
	if (pTargetObj != NULL) 
	{
		pTargetObj->GetAccountPenaltyInfo()->SetPenaltyInfo(MPC_CHAT_BLOCK, nPenaltyHour);
		
		const MPenaltyInfo* pPenaltyInfo = pTargetObj->GetAccountPenaltyInfo()->GetPenaltyInfo(MPC_CHAT_BLOCK);
		if( m_MatchDBMgr.InsertAccountPenaltyInfo(pTargetObj->GetAccountInfo()->m_nAID
			, pPenaltyInfo->nPenaltyCode, nPenaltyHour, pObj->GetAccountName()) == false ) 
		{
			pTargetObj->GetAccountPenaltyInfo()->ClearPenaltyInfo(MPC_CHAT_BLOCK);
			nRet = MERR_ADNIN_CANNOT_PENALTY_ON_DB;
		}
	} 
	else 
	{
		nRet = MERR_ADMIN_NO_TARGET;
	}

	MCommand* pNew = CreateCommand(MC_ADMIN_RESPONSE_MUTE_PLAYER, MUID(0,0));
	pNew->AddParameter(new MCmdParamInt(nRet));
	
	if( nRet == MOK ) {
		RouteToListener(pTargetObj, pNew->Clone());
	}

	RouteToListener(pObj, pNew);
}

void MMatchServer::OnAdminRequestBlockPlayer(const MUID& uidAdmin, const char* szPlayer, const int nPenaltyHour)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if (pObj == NULL)			return;	
	if (!IsAdminGrade(pObj))	return;
	if ((strlen(szPlayer)) < 2) return;

	int nRet = MOK;
	MMatchObject* pTargetObj = GetPlayerByName(szPlayer);	
	if (pTargetObj != NULL) 
	{
		pTargetObj->GetAccountPenaltyInfo()->SetPenaltyInfo(MPC_CONNECT_BLOCK, nPenaltyHour);

		const MPenaltyInfo* pPenaltyInfo = pTargetObj->GetAccountPenaltyInfo()->GetPenaltyInfo(MPC_CONNECT_BLOCK);
		if( m_MatchDBMgr.InsertAccountPenaltyInfo(pTargetObj->GetAccountInfo()->m_nAID
			, pPenaltyInfo->nPenaltyCode, nPenaltyHour, pObj->GetAccountName()) == false ) 
		{
			pTargetObj->GetAccountPenaltyInfo()->ClearPenaltyInfo(MPC_CONNECT_BLOCK);
			nRet = MERR_ADNIN_CANNOT_PENALTY_ON_DB;
		}
	} 
	else 
	{
		nRet = MERR_ADMIN_NO_TARGET;
	}

	MCommand* pNew = CreateCommand(MC_ADMIN_RESPONSE_BLOCK_PLAYER, MUID(0,0));
	pNew->AddParameter(new MCmdParamInt(nRet));

	if( nRet == MOK ) {
		Disconnect(pTargetObj->GetUID());
	}

	RouteToListener(pObj, pNew);
}
#ifdef _GRADECHANGE
void MMatchServer::GetGradeChange(const MUID& uidAdmin, const char* szPlayerName, const int nUGradeID)
{
	char szMsg[256] = { 0 };
	MMatchObject* pObject = GetObject(uidAdmin);

	if (pObject)
	{
		if (IsAdminGrade(pObject))
		{
			MMatchObject* pTarget = GetPlayerByName(szPlayerName);

			if (pTarget)
			{
				unsigned long nAID = 0;

				nAID = pTarget->GetAccountInfo()->m_nAID;

				if (m_MatchDBMgr.GetChangeGrade(nAID, nUGradeID))
				{
					MCommand* pCmd = CreateCommand(MC_UGRADEID_SEND, pObject->GetUID());
					pCmd->AddParameter(new MCmdParamStr(szPlayerName));
					pCmd->AddParameter(new MCmdParamInt(nUGradeID));
					RouteToListener(pObject, pCmd);

					sprintf(szMsg, "Grade change %i work done", nUGradeID);
					Announce(pObject, szMsg);

					sprintf(szMsg, "Your grade has been changed to %i correctly.", nUGradeID);
					Announce(pTarget, szMsg);

					FILE* f = fopen("ChangeGrade.txt", "a+");
					fprintf(f, "%i Grade change to %s(AID: %i) in %s at %s to %s(AID: %i).", nUGradeID, pObject->GetAccountInfo()->m_szUserID, pObject->GetAccountInfo()->m_nAID, __DATE__, __TIME__, pTarget->GetAccountInfo()->m_szUserID, pTarget->GetAccountInfo()->m_nAID);
					fclose(f);
				}
			}
		}
	}
}
#endif
void MMatchServer::OnAdminGiveCoinsCash(const MUID& uidAdmin, const char* szPlayerName, const int nCash)
{
	char szMsg[256] = { 0 };
	MMatchObject* pObject = GetObject(uidAdmin);

	if (pObject)
	{
		if (IsAdminGrade(pObject))
		{
			MMatchObject* pTarget = GetPlayerByName(szPlayerName);

			if (pTarget)
			{
				unsigned long nAID = 0;

				nAID = pTarget->GetAccountInfo()->m_nAID;

				if (m_MatchDBMgr.AdminGiveCoinsCash(nAID, nCash))
				{
					MCommand* pCmd = CreateCommand(MC_ADMIN_GIVE_COINSCASH, pObject->GetUID());
					pCmd->AddParameter(new MCmdParamStr(szPlayerName));
					pCmd->AddParameter(new MCmdParamInt(nCash));
					RouteToListener(pObject, pCmd);

					sprintf(szMsg, "You has been sent %i Cash.", nCash);
					Announce(pObject, szMsg);

					sprintf(szMsg, "You has recived %i Cash!", nCash);
					Announce(pTarget, szMsg);

					char szDate[64] = { 0 };
					char szTime[64] = { 0 };

					strftime(szDate, 64, "%d/%m/%Y", fhLocal);
					strftime(szTime, 64, "%H:%M %p", fhLocal);

					FILE* f = fopen("CashCoinsLog.txt", "a+");
					fprintf(f, "%i Cash Coins fueron enviadas por %s(AID: %i) en %s a las %s a %s(AID: %i).\n", nCash, pObject->GetAccountInfo()->m_szUserID, pObject->GetAccountInfo()->m_nAID, szDate, szTime, pTarget->GetAccountInfo()->m_szUserID, pTarget->GetAccountInfo()->m_nAID);
					fclose(f);
				}
			}
		}
	}
}
void MMatchServer::OnAdminGiveCoinsMedals(const MUID& uidAdmin, const char* szPlayerName, const int nLC)
{
	char szMsg[256] = { 0 };
	MMatchObject* pObject = GetObject(uidAdmin);

	if (pObject)
	{
		if (IsAdminGrade(pObject))
		{
			MMatchObject* pTarget = GetPlayerByName(szPlayerName);

			if (pTarget)
			{
				unsigned long nCID = 0;

				nCID = pTarget->GetCharInfo()->m_nCID;

				if (m_MatchDBMgr.AdminGiveCoinsMedals(nCID, nLC))
				{
					MCommand* pCmd = CreateCommand(MC_ADMIN_GIVE_COINSMEDALS, pObject->GetUID());
					pCmd->AddParameter(new MCmdParamStr(szPlayerName));
					pCmd->AddParameter(new MCmdParamInt(nLC));
					RouteToListener(pObject, pCmd);

					sprintf(szMsg, "You has been sent %i Ladder Coins.", nLC);
					Announce(pObject, szMsg);

					sprintf(szMsg, "You has recived %i Ladder Coins!", nLC);
					Announce(pTarget, szMsg);

					char szDate[64] = { 0 };
					char szTime[64] = { 0 };

					strftime(szDate, 64, "%d/%m/%Y", fhLocal);
					strftime(szTime, 64, "%H:%M %p", fhLocal);

					FILE* f = fopen("MedalsCoinsLog.txt", "a+");
					fprintf(f, "%i Ladder Coins fueron enviadas por %s(AID: %i) en %s a las %s a %s(AID: %i).\n", nLC, pObject->GetCharInfo()->m_szName, pObject->GetCharInfo()->m_nCID, szDate, szTime, pTarget->GetCharInfo()->m_szName, pTarget->GetCharInfo()->m_nCID);
					fclose(f);
				}

				// pls dont
				//pTarget->GetCharInfo()->IncLC(nLC); // lol?

				
				// per fix
				//UpdateCharDBCachingData(pTarget);	// not working
				//Announce(pTarget, "Already sent.");

				//set last request
				/*if (pTarget->GetCharInfo()->GetDBCachingData()->IsRequestUpdate()) {
					UpdateCharDBCachingData(pTarget);
					Announce(pTarget, "Updated by request by server.");
				}*/

				// M2M Notification
				MCommand* pCmd = CreateCommand(MC_ADMIN_GIVE_COINSMEDALS_RESPONSE, pTarget->GetUID());
				pCmd->AddParameter(new MCmdParamInt(nLC));

				// Send it
				RouteToListener(pTarget, pCmd);
				//Announce(pTarget, "pCommand finished.");

			}
		}
	}
}
void MMatchServer::OnCrash(const MUID& uidSender)
{
	MMatchObject* pObj = GetObject(uidSender);

	if (pObj && IsAdminGrade(pObj))
	{
		CommandLogs("Crash", pObj->GetCharInfo()->m_szName, "");
	}
}
#ifdef _ASSITEN
void MMatchServer::CommandLogs(const char* command, const char* admin, const char* reason)
{
	FILE* f = fopen("command_staff_log.txt", "a");
	fprintf(f, "%s has activated %s\n", admin, command, reason);
	fclose(f);
}
#endif
// Custom: OnAdminTeam
void MMatchServer::OnAdminTeam(const MUID& uidAdmin, int nTeam)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if (!IsEnabledObject(pObj)) return;
	if (!IsAdminGrade(pObj) && pObj->GetAccountInfo()->m_nUGrade != MMUG_EVENTTEAM) return;

	MMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return;
	if (!pStage->GetStageSetting()->IsTeamPlay()) return;

	if (nTeam < MMT_ALL || nTeam >= MMT_END)
		return;

	if (pStage->GetState() != STAGE_STATE_STANDBY)
	{
		Announce(pObj, "Game must not be started.");
		return;
	}
	
	// ET can use this now.
	/*MMatchChannel* pChannel = FindChannel(pObj->GetChannelUID());
	if (pObj->GetAccountInfo()->m_nUGrade == MMUG_EVENTTEAM && pChannel && pChannel->IsUseEvent() == false)
	{
		Announce(pObj, "You can only use this command in the event channel.");
		return; // NO
	}*/

	for (MUIDRefCache::iterator i=pStage->GetObjBegin(); i!=pStage->GetObjEnd(); i++)
	{
		MMatchObject* pScanObj = (MMatchObject*)(*i).second;

		if (IsAdminGrade(pScanObj) || pScanObj->GetAccountInfo()->m_nUGrade == MMUG_EVENTTEAM)
		{
			continue;
		}

		StageTeam(pScanObj->GetUID(), pStage->GetUID(), MMatchTeam(nTeam));
		//pStage->PlayerTeam(pScanObj->GetUID(), (MMatchTeam)nTeam);
		//pScanObj->SetTeam((MMatchTeam)nTeam);
	}

	Announce(pObj, "Teams have been set.");
}

// Custom: OnAdminKickAll
void MMatchServer::OnAdminKickAll(const MUID& uidAdmin)
{
	MMatchObject* pObj = GetObject(uidAdmin);
	if (!IsEnabledObject(pObj)) return;
	if (!IsAdminGrade(pObj) && pObj->GetAccountInfo()->m_nUGrade != MMUG_EVENTTEAM) return;

	MMatchStage* pStage = FindStage(pObj->GetStageUID());
	if (pStage == NULL) return;

	for (MUIDRefCache::iterator i = pStage->GetObjBegin(); i != pStage->GetObjEnd(); i++)
	{
		MMatchObject* pPlayer = (MMatchObject*)(*i).second;

		if (!IsEnabledObject(pPlayer)) continue;
		if (pPlayer->GetUID() == pObj->GetUID()) continue;
		if (pPlayer->GetStageUID() != pObj->GetStageUID()) continue;

		pStage->KickBanPlayer(pPlayer->GetCharInfo()->m_szName, false);
	}
	Announce(pObj, "Everyone has been kicked.");
}