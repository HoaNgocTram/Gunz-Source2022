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
#include "MAsyncDBJob_WinTheClanGame.h"
#include "MUtil.h"

// Å¬·£ °EÃ °ø¿EÇÔ¼E////////////////////////////////////////////////////////////////////////////
void CopyClanMemberListNodeForTrans(MTD_ClanMemberListNode* pDest, MMatchObject* pSrcObject)
{
	pDest->uidPlayer = pSrcObject->GetUID();
	strcpy(pDest->szName, pSrcObject->GetCharInfo()->m_szName);
	pDest->nLevel = (char)pSrcObject->GetCharInfo()->m_nLevel;
	pDest->nPlace = pSrcObject->GetPlace();
	pDest->nClanGrade = pSrcObject->GetCharInfo()->m_ClanInfo.m_nGrade;
}


#ifndef _2PCLAN
int MMatchServer::ValidateCreateClan(const char* szClanName, MMatchObject* pMasterObject, MMatchObject** ppSponsorObject)
#else
int MMatchServer::ValidateCreateClan(const char* szClanName, MMatchObject* pMasterObject)
#endif // !_2PCLAN

{
	int nResult = MOK;

	nResult = ValidateMakingName(szClanName, MIN_CLANNAME, MAX_CLANNAME);
	if (nResult != MOK)
	{
		return nResult;
	}

	// Å¬·£»ý¼º¿¡ ÇÊ¿äÇÑ ¸¶½ºÅÍÀÇ ·¹º§ °Ë»ç
	if (pMasterObject->GetCharInfo()->m_nLevel < CLAN_CREATING_NEED_LEVEL)
	{
		return MERR_CLAN_CREATING_LESS_LEVEL;
	}

	// Å¬·£»ý¼º¿¡ ÇÊ¿äÇÑ ¸¶½ºÅÍÀÇ ¹Ù¿îÆ¼°¡ ÃæºÐÇÑÁö °Ë»ç
	if (pMasterObject->GetCharInfo()->m_nBP < CLAN_CREATING_NEED_BOUNTY)
	{
		return MERR_CLAN_CREATING_LESS_BOUNTY;
	}


	// Å¬·£ Áßº¹ °Ë»ç - µðºñ¿¡¼­ Á÷Á¢ °Ë»çÇÑ´Ù.
	int nTempCLID = 0;
	if (m_MatchDBMgr.GetClanIDFromName(szClanName, &nTempCLID))
	{
		return MERR_EXIST_CLAN;
	}


#ifndef _2PCLAN
	for (int i = 0; i < CLAN_SPONSORS_COUNT; i++)
	{
		// Å¬·£»ý¼º¸â¹ö°¡ Å¬·£¿¡ ¼ÓÇÏÁö ¾Ê¾Ò´ÂÁö °Ë»ç
		if (ppSponsorObject[i]->GetCharInfo() == NULL) return MERR_CLAN_NO_SPONSOR;
		if (ppSponsorObject[i]->GetCharInfo()->m_ClanInfo.m_nClanID != 0) return MERR_CLAN_SPONSOR_JOINED_OTHERCLAN;

		// ·Îºñ¿¡ ÀÖ´ÂÁö È®ÀÎ
		if (ppSponsorObject[i]->GetPlace() != MMP_LOBBY) return MERR_CLAN_SPONSOR_NOT_LOBBY;
	}

	// Å¬·£»ý¼º¸â¹öµéÀÌ Áßº¹µÇ¾ú´ÂÁö °Ë»ç
	MUID* tempUID = new MUID[CLAN_SPONSORS_COUNT + 1];

	tempUID[0] = pMasterObject->GetUID();

	for (int i = 0; i < CLAN_SPONSORS_COUNT; i++)
	{
		tempUID[i + 1] = ppSponsorObject[i]->GetUID();
	}

	bool bExist = false;

	for (int i = 0; i < CLAN_SPONSORS_COUNT; i++)
	{
		for (int j = i + 1; j < CLAN_SPONSORS_COUNT + 1; j++)
		{
			if (tempUID[i] == tempUID[j])
			{
				delete[] tempUID;
				return MERR_CLAN_NO_SPONSOR;
			}
		}
	}
	delete[] tempUID;
#endif // !_2PCLAN



	return MOK;
}


int ValidateJoinClan(MMatchObject* pAdminObject, MMatchObject* pJoinerObject, const char* szClanName)
{
	// Å¬·£ ¾ûÑå¹ÎÀÌ»EµûÍÞÀÎÁEÈ®ÀÎ
	if (! IsUpperClanGrade(pAdminObject->GetCharInfo()->m_ClanInfo.m_nGrade, MCG_ADMIN))
	{
		return MERR_CLAN_NOT_MASTER_OR_ADMIN;
	}

	// °¡ÀÔÀÚ°¡ Å¬·£¿¡ ÀÌ¹Ì °¡ÀÔµÇ¾ûÜÖ´ÂÁEÈ®ÀÎ
	if (pJoinerObject->GetCharInfo()->m_ClanInfo.IsJoined() == true)
	{
		return MERR_CLAN_JOINER_JOINED_ALREADY;
	}

	// °¡ÀÔÀÚ°¡ ·Îºñ¿¡ ÀÖ´ÂÁEÈ®ÀÎ
	if (pJoinerObject->GetPlace() != MMP_LOBBY)
	{
		return MERR_CLAN_JOINER_NOT_LOBBY;
	}


	// Å¬·£ ÀÌ¸§ÀÌ ¸Â´ÂÁEÈ®ÀÎ
	if (stricmp(pAdminObject->GetCharInfo()->m_ClanInfo.m_szClanName, szClanName))
	{
		return MERR_CLAN_WRONG_CLANNAME;
	}
	
	return MOK;
}


int ValidateLeaveClan(MMatchObject* pLeaverObject)
{
	// Å¬·£¿¡ ¼ÓÇØÀÖ´ÂÁEÈ®ÀÎ
	if (!pLeaverObject->GetCharInfo()->m_ClanInfo.IsJoined())
	{
		return MERR_CLAN_NOT_JOINED;
	}

	// ¸¶½ºÅÍ´Â Å»ÅðÇÒ ¼E¾ø´Ù
	if (pLeaverObject->GetCharInfo()->m_ClanInfo.m_nGrade == MCG_MASTER)
	{
		return MERR_CLAN_CANNOT_LEAVE;
	}

	return MOK;
}

bool IsSameClan(MMatchObject* pSrcObject, MMatchObject* pTarObject)
{
	if ((pSrcObject->GetCharInfo()->m_ClanInfo.m_nClanID == 0) || (pTarObject->GetCharInfo()->m_ClanInfo.m_nClanID == 0) ||
	   (pSrcObject->GetCharInfo()->m_ClanInfo.m_nClanID != pTarObject->GetCharInfo()->m_ClanInfo.m_nClanID))
	{
		return false;
	}

	return true;
}

void MMatchServer::UpdateCharClanInfo(MMatchObject* pObject, const int nCLID, const char* szClanName, const MMatchClanGrade nGrade)
{
	if (! IsEnabledObject(pObject)) return;

	bool bHasJoined = pObject->GetCharInfo()->m_ClanInfo.IsJoined();


	// m_ClanMapÀÇ Join, Leaveµµ ¿©±â¼­ ÇØÁØ´Ù.
	if ((bHasJoined) && (nCLID == 0))
	{
		m_ClanMap.RemoveObject(pObject->GetUID(), pObject);
	}

	// objectÀÇ Á¤º¸ º¯°E
	strcpy(pObject->GetCharInfo()->m_ClanInfo.m_szClanName, szClanName);
	pObject->GetCharInfo()->m_ClanInfo.m_nGrade = nGrade;
	pObject->GetCharInfo()->m_ClanInfo.m_nClanID = nCLID;


	if ((pObject->GetCharInfo()->m_ClanInfo.IsJoined()) && (!bHasJoined))
	{
		m_ClanMap.AddObject(pObject->GetUID(), pObject);

		// ÀÓ½ÃÄÚµE.. Àß¸øµÈ Å¬·£ID ¿Â´Ù¸EÃ¼Å©ÇÏ¿© Àâ±âÀ§ÇÔ...20090224 by kammir
		if(pObject->GetCharInfo()->m_ClanInfo.GetClanID() >= 9000000)
			LOG(LOG_FILE, "[UpdateCharClanInfo()] %s's ClanID:%d.", pObject->GetAccountName(), pObject->GetCharInfo()->m_ClanInfo.GetClanID());
	}

	// Custom: New IJJI clan changes.
	if (nGrade == MCG_MASTER)
	{
		MMatchClan* pClan = m_ClanMap.GetClan( nCLID );

		if (pClan)
			strcpy( pClan->GetClanInfoEx()->szMaster, pObject->GetCharInfo()->m_szName );
	}


	// route±ûÝE¿©±â¼­ ÇØÁØ´Ù.
	MCommand* pNewCmd = CreateCommand(MC_MATCH_CLAN_UPDATE_CHAR_CLANINFO, MUID(0,0));
	void* pClanInfoArray = MMakeBlobArray(sizeof(MTD_CharClanInfo), 1);

	MTD_CharClanInfo* pClanInfo = (MTD_CharClanInfo*)MGetBlobArrayElement(pClanInfoArray, 0);
	
	strcpy(pClanInfo->szClanName, szClanName);
	pClanInfo->nGrade = nGrade;

	
	pNewCmd->AddParameter(new MCommandParameterBlob(pClanInfoArray, MGetBlobArraySize(pClanInfoArray)));
	MEraseBlobArray(pClanInfoArray);

	RouteToListener(pObject, pNewCmd);

}





///////////////////////////////////////////////////////////////////////////////////////////////////
MMatchClan* MMatchServer::FindClan(const int nCLID)
{
	MMatchClanMap::iterator i = m_ClanMap.find(nCLID);
	if(i==m_ClanMap.end()) return NULL;

	MMatchClan* pClan = (*i).second;
	return pClan;
}

void MMatchServer::OnClanRequestCreateClan(const MUID& uidPlayer, const int nRequestID, const char* szClanName,
	char** szSponsorNames)
{
	MMatchObject* pMasterObject = GetObject(uidPlayer);
	if (!IsEnabledObject(pMasterObject)) return;

#ifndef _2PCLAN
	MMatchObject* pSponsorObjects[CLAN_SPONSORS_COUNT];

	for (int i = 0; i < CLAN_SPONSORS_COUNT; i++)
	{
		pSponsorObjects[i] = GetPlayerByName(szSponsorNames[i]);

		// Å¬·£»ý¼º¸â¹öÁß ÇÑ¸íÀÌ¶óµµ Á¸ÀçÇÏÁö ¾ÊÀ¸¸é ¾ÈµÈ´Ù
		if (pSponsorObjects[i] == NULL)
		{
			// ¸Þ¼¼Áö º¸³»ÁÖ°í ³¡.
			MCommand* pNewCmd = CreateCommand(MC_MATCH_CLAN_RESPONSE_CREATE_CLAN, MUID(0, 0));
			pNewCmd->AddParameter(new MCommandParameterInt(MERR_CLAN_NO_SPONSOR));
			pNewCmd->AddParameter(new MCommandParameterInt(nRequestID));
			RouteToListener(pMasterObject, pNewCmd);

			return;
		}
	}
#endif // !_2PCLAN



	// ¼­¹ö´Ü¿¡¼­ Å¬·£À» »ý¼ºÇÒ ¼ö ÀÖ´ÂÁö °Ë»çÇÑ´Ù.
#ifndef _2PCLAN
	int nRet = ValidateCreateClan(szClanName, pMasterObject, pSponsorObjects);
#else
	int nRet = ValidateCreateClan(szClanName, pMasterObject);
#endif // !_2PCLAN


	if (nRet != MOK)
	{
		MCommand* pNewCmd = CreateCommand(MC_MATCH_CLAN_RESPONSE_CREATE_CLAN, MUID(0, 0));
		pNewCmd->AddParameter(new MCommandParameterInt(nRet));
		pNewCmd->AddParameter(new MCommandParameterInt(nRequestID));
		RouteToListener(pMasterObject, pNewCmd);
		return;
	}

#ifdef _2PCLAN
	MAsyncDBJob_CreateClan* pNewJob = new MAsyncDBJob_CreateClan(uidPlayer);
	pNewJob->Input(szClanName,
		pMasterObject->GetCharInfo()->m_nCID,
		pMasterObject->GetUID());
	pMasterObject->m_DBJobQ.DBJobQ.push_back(pNewJob);
#endif // _2PCLAN


	// Å¬·£»ý¼º¸â¹öÁß ÃÊÃ»°ÅÀý ¸â¹ö°¡ ÀÖ´ÂÁö »ìÆìº»´Ù
#ifndef _2PCLAN
	bool bCheckRejectInvite = false;
	for (int i = 0; i < CLAN_SPONSORS_COUNT; i++)
	{
		if (pSponsorObjects[i]->CheckUserOption(MBITFLAG_USEROPTION_REJECT_INVITE) == true) {
			bCheckRejectInvite = true;
			NotifyMessage(pSponsorObjects[i]->GetUID(), MATCHNOTIFY_USER_INVITE_IGNORED);
		}
	}
	if (bCheckRejectInvite == true) {
		// ¸Þ¼¼Áö º¸³»ÁÖ°í ³¡.
		RouteResponseToListener(pMasterObject, MC_MATCH_CLAN_RESPONSE_AGREED_CREATE_CLAN, MERR_CLAN_NO_SPONSOR);
		NotifyMessage(pMasterObject->GetUID(), MATCHNOTIFY_USER_INVITE_REJECTED);
		return;
	}

	// Å¬·£»ý¼º¸â¹ö¿¡°Ô µ¿ÀÇ¸¦ ¹°¾îº»´Ù.
	for (int i = 0; i < CLAN_SPONSORS_COUNT; i++)
	{
		// ¸Þ¼¼Áö º¸³»Áà¾ß ÇÔ
		MCommand* pNewCmd = CreateCommand(MC_MATCH_CLAN_ASK_SPONSOR_AGREEMENT, MUID(0, 0));
		pNewCmd->AddParameter(new MCommandParameterInt(nRequestID));
		pNewCmd->AddParameter(new MCommandParameterString((char*)szClanName));
		pNewCmd->AddParameter(new MCommandParameterUID(uidPlayer));
		pNewCmd->AddParameter(new MCommandParameterString(pMasterObject->GetCharInfo()->m_szName));

		RouteToListener(pSponsorObjects[i], pNewCmd);
	}


	MCommand* pNewCmd = CreateCommand(MC_MATCH_CLAN_RESPONSE_CREATE_CLAN, MUID(0, 0));
	pNewCmd->AddParameter(new MCommandParameterInt(nRet));
	pNewCmd->AddParameter(new MCommandParameterInt(nRequestID));
	RouteToListener(pMasterObject, pNewCmd);
#endif // !_2PCLAN

}




void MMatchServer::OnClanAnswerSponsorAgreement(const int nRequestID, const MUID& uidClanMaster, char* szSponsorCharName, const bool bAnswer)
{
	MMatchObject* pClanMasterObject = GetObject(uidClanMaster);
	if (! IsEnabledObject(pClanMasterObject)) return;

	
	MCommand* pNewCmd = CreateCommand(MC_MATCH_CLAN_ANSWER_SPONSOR_AGREEMENT, MUID(0,0));
	pNewCmd->AddParameter(new MCommandParameterInt(nRequestID));
	pNewCmd->AddParameter(new MCommandParameterUID(uidClanMaster));
	pNewCmd->AddParameter(new MCommandParameterString(szSponsorCharName));
	pNewCmd->AddParameter(new MCommandParameterBool(bAnswer));

	RouteToListener(pClanMasterObject, pNewCmd);	
}

void MMatchServer::OnClanRequestAgreedCreateClan(const MUID& uidPlayer, const char* szClanName, char** szSponsorNames)
{
	MMatchObject* pMasterObject = GetObject(uidPlayer);
	if (! IsEnabledObject(pMasterObject)) return;

	// Custom: New IJJI clan changes.
	//MMatchObject* pSponsorObjects[CLAN_SPONSORS_COUNT];

	//for (int i = 0; i < CLAN_SPONSORS_COUNT; i++)
	//{
	//	pSponsorObjects[i] = GetPlayerByName(szSponsorNames[i]);

	//	// Å¬·£»ý¼º¸â¹öÁß ÇÑ¸úÜÌ¶óµµ Á¸ÀçÇÏÁE¾ÊÀ¸¸E¾ÈµÈ´Ù
	//	if (! IsEnabledObject(pSponsorObjects[i]))
	//	{
	//		// ¸Þ¼¼ÁEº¸³»ÁÖ°E³¡.
	//		RouteResponseToListener(pMasterObject, MC_MATCH_CLAN_RESPONSE_AGREED_CREATE_CLAN, MERR_CLAN_NO_SPONSOR);

	//		return;
	//	}
	//}
	
	// ¼­¹ö´Ü¿¡¼­ Å¬·£À» »ý¼ºÇÒ ¼EÀÖ´ÂÁE°Ë»çÇÑ´Ù.
#ifndef _2PCLAN
	int nRet = ValidateCreateClan(szClanName, pMasterObject, pSponsorObjects);
#else
	int nRet = ValidateCreateClan(szClanName, pMasterObject);
#endif // !_2PCLAN

	if (nRet != MOK)
	{
		RouteResponseToListener(pMasterObject, MC_MATCH_CLAN_RESPONSE_AGREED_CREATE_CLAN, nRet);
		return;
	}

	// Custom: New IJJI clan changes.
	int nMasterCID = 0;
	//int nMemberCID[CLAN_SPONSORS_COUNT] = {0, };

	nMasterCID = pMasterObject->GetCharInfo()->m_nCID;
	//for (int i = 0; i < CLAN_SPONSORS_COUNT; i++)
	//{
	//	nMemberCID[i] = pSponsorObjects[i]->GetCharInfo()->m_nCID;
	//}

	int nNewCLID = 0;

	// ½ÇÁ¦·Î µðºñ¿¡ ³Ö´Â´Ù.
#ifndef _2PCLAN
	if (CLAN_SPONSORS_COUNT == 4)
#else
	if (CLAN_SPONSORS_COUNT == 1)
#endif // !_2PCLAN

	{
		MAsyncDBJob_CreateClan* pNewJob = new MAsyncDBJob_CreateClan(uidPlayer);
		pNewJob->Input(szClanName,
			nMasterCID,
#ifndef _2PCLAN
			nMemberCID[0],
			nMemberCID[1],
			nMemberCID[2],
			nMemberCID[3],
#endif // !_2PCLAN

			pMasterObject->GetUID()
#ifndef _2PCLAN
			, pSponsorObjects[0]->GetUID(),
			pSponsorObjects[1]->GetUID(),
			pSponsorObjects[2]->GetUID(),
			pSponsorObjects[3]->GetUID()
#endif // !_2PCLAN
		);
		// PostAsyncJob(pNewJob);
		pMasterObject->m_DBJobQ.DBJobQ.push_back(pNewJob);
	}
	else
	{
		//_ASSERT(0);
	}
}

void MMatchServer::OnClanRequestCloseClan(const MUID& uidClanMaster, const char* szClanName)
{
	ResponseCloseClan(uidClanMaster, szClanName);
}

void MMatchServer::ResponseCloseClan(const MUID& uidClanMaster, const char* szClanName)
{
	MMatchObject* pMasterObject = GetObject(uidClanMaster);
	if (! IsEnabledObject(pMasterObject)) return;

	// Å¬·£¸¶½ºÅÍÀÎÁEÈ®ÀÎ
	if (pMasterObject->GetCharInfo()->m_ClanInfo.m_nGrade != MCG_MASTER)
	{
		RouteResponseToListener(pMasterObject, MC_MATCH_CLAN_RESPONSE_CLOSE_CLAN, MERR_CLAN_NOT_MASTER);
		return;
	}

	// Å¬·£ ÀÌ¸§ÀÌ Á¦´EÎ µÇ¾ú´ÂÁEÈ®ÀÎ
	if (stricmp(pMasterObject->GetCharInfo()->m_ClanInfo.m_szClanName, szClanName))
	{
		RouteResponseToListener(pMasterObject, MC_MATCH_CLAN_RESPONSE_CLOSE_CLAN, MERR_CLAN_WRONG_CLANNAME);
		return;
	}

	// ½ÇÁ¦·Î µðºñ¿¡¼­ Æó¼E¿¹¾E
	if (!m_MatchDBMgr.ReserveCloseClan(pMasterObject->GetCharInfo()->m_ClanInfo.m_nClanID,
										pMasterObject->GetCharInfo()->m_ClanInfo.m_szClanName,
										pMasterObject->GetCharInfo()->m_nCID,
										MGetStrLocalTime(DAY_OF_DELETE_CLAN, 0, 0, MDT_YMD)))
	{
		RouteResponseToListener(pMasterObject, MC_MATCH_CLAN_RESPONSE_CLOSE_CLAN, MERR_CLAN_CANNOT_CLOSE);
		return;
	}

	// Custom: Delete clan instantly with stats refresh
	char szNullName[16];
	memset( szNullName, 0, sizeof( szNullName ) );

	MMatchClan* pClan = m_ClanMap.GetClan( szClanName );
	if( pClan )
	{
		list<MMatchObject*> memberList;

		// Clone the list so it doesn't mess up UpdateCharClanInfo which removes the member from m_Members
		for( MUIDRefCache::iterator it = pClan->GetMemberBegin(); it != pClan->GetMemberEnd(); ++it )
			memberList.push_back( (MMatchObject*)(*it).second );

		for( list<MMatchObject*>::iterator it = memberList.begin(); it != memberList.end(); ++it )
		{
			MMatchObject* pObj = (*it);

			if( pObj && pObj->GetCharInfo() )
				UpdateCharClanInfo( pObj, 0, szNullName, MCG_NONE );
		}
	}

	// Æó¼â¿¹¾àµÇ¾ú´Ù´Â ¸Þ¼¼Áö¸¦ º¸³½´Ù.
	RouteResponseToListener(pMasterObject, MC_MATCH_CLAN_RESPONSE_CLOSE_CLAN, MOK);
}

void MMatchServer::OnClanRequestJoinClan(const MUID& uidClanAdmin, const char* szClanName, const char* szJoiner)
{
	ResponseJoinClan(uidClanAdmin, szClanName, szJoiner);
}


void MMatchServer::ResponseJoinClan(const MUID& uidClanAdmin, const char* szClanName, const char* szJoiner)
{
	MMatchObject* pAdminObject = GetObject(uidClanAdmin);
	if (! IsEnabledObject(pAdminObject)) return;

	MMatchObject* pJoinerObject = GetPlayerByName(szJoiner);
	if (! IsEnabledObject(pJoinerObject))
	{
		RouteResponseToListener(pAdminObject, MC_MATCH_CLAN_RESPONSE_JOIN_CLAN, MERR_CLAN_OFFLINE_TARGET);
		return;
	}

	// ´EóÀÌ ÃÊÃ»°ÅºÎ »óÅÂÀÌ¸EÃÊÃ»¸øÇÑ´Ù
	if (pJoinerObject->CheckUserOption(MBITFLAG_USEROPTION_REJECT_INVITE) == true) {
		NotifyMessage(pAdminObject->GetUID(), MATCHNOTIFY_USER_INVITE_REJECTED);
		return;
	}

	int nRet = ValidateJoinClan(pAdminObject, pJoinerObject, szClanName);
	if (nRet != MOK)
	{
		RouteResponseToListener(pAdminObject, MC_MATCH_CLAN_RESPONSE_JOIN_CLAN, nRet);
		return;
	}

	// °¡ÀÔÀÚ¿¡°Ô µ¿ÀÇ¸¦ ¹¯´Â´Ù.
	MCommand* pNewCmd = CreateCommand(MC_MATCH_CLAN_ASK_JOIN_AGREEMENT, MUID(0,0));
	pNewCmd->AddParameter(new MCommandParameterString((char*)szClanName));
	pNewCmd->AddParameter(new MCommandParameterUID(uidClanAdmin));
	pNewCmd->AddParameter(new MCommandParameterString(pAdminObject->GetCharInfo()->m_szName));
	RouteToListener(pJoinerObject, pNewCmd);


	RouteResponseToListener(pAdminObject, MC_MATCH_CLAN_RESPONSE_JOIN_CLAN, MOK);
}

void MMatchServer::OnClanAnswerJoinAgreement(const MUID& uidSender, const MUID& uidClanAdmin, const char* szJoiner, const bool bAnswer)
{
	MMatchObject* pClanAdminObject = GetObject(uidClanAdmin);
	if (! IsEnabledObject(pClanAdminObject)) return;

	MMatchObject* pJoinerObject = GetObject(uidSender);
	if (! IsEnabledObject(pJoinerObject))

	// Custom: Exploit fix
	// check if acceptor name == admin name (if it is, reject)
	if (!strcmp(pJoinerObject->GetCharInfo()->m_szName, pClanAdminObject->GetCharInfo()->m_szName))
		return;

	
	MCommand* pNewCmd = CreateCommand(MC_MATCH_CLAN_ANSWER_JOIN_AGREEMENT, MUID(0,0));
	pNewCmd->AddParameter(new MCommandParameterUID(uidClanAdmin));
	pNewCmd->AddParameter(new MCommandParameterString(pJoinerObject->GetCharInfo()->m_szName));//pNewCmd->AddParameter(new MCommandParameterString((char*)szJoiner));
	pNewCmd->AddParameter(new MCommandParameterBool(bAnswer));

	RouteToListener(pClanAdminObject, pNewCmd);	
}

void MMatchServer::OnClanRequestAgreedJoinClan(const MUID& uidClanAdmin, const char* szClanName, const char* szJoiner)
{
	ResponseAgreedJoinClan(uidClanAdmin, szClanName, szJoiner);
}

void MMatchServer::ResponseAgreedJoinClan(const MUID& uidClanAdmin, const char* szClanName, const char* szJoiner)
{
	MMatchObject* pAdminObject = GetObject(uidClanAdmin);
	if (! IsEnabledObject(pAdminObject)) return;

	MMatchObject* pJoinerObject = GetPlayerByName(szJoiner);
	if (! IsEnabledObject(pJoinerObject)) return;


	int nRet = ValidateJoinClan(pAdminObject, pJoinerObject, szClanName);
	if (nRet != MOK)
	{
		RouteResponseToListener(pAdminObject, MC_MATCH_CLAN_RESPONSE_AGREED_JOIN_CLAN, nRet);
		RouteResponseToListener(pJoinerObject, MC_MATCH_CLAN_RESPONSE_AGREED_JOIN_CLAN, nRet);
		return;
	}


	int nCLID = pAdminObject->GetCharInfo()->m_ClanInfo.m_nClanID;
	int nJoinerCID = pJoinerObject->GetCharInfo()->m_nCID;
	int nClanGrade = (int)MCG_MEMBER;

	bool bDBRet = false;

	// ½ÇÁ¦ µðºñ»ó¿¡¼­ °¡ÀÔÃ³¸®
	if (!m_MatchDBMgr.AddClanMember(nCLID, nJoinerCID, nClanGrade, &bDBRet))
	{
		RouteResponseToListener(pAdminObject, MC_MATCH_CLAN_RESPONSE_AGREED_JOIN_CLAN, MERR_CLAN_DONT_JOINED);
		RouteResponseToListener(pJoinerObject, MC_MATCH_CLAN_RESPONSE_AGREED_JOIN_CLAN, MERR_CLAN_DONT_JOINED);
		return;
	}

	// ÀÎ¿øÀÌ ÃÊ°úµÇ¸Edb return °ªÀÌ falseÀÌ´Ù.
	if (!bDBRet)
	{
		RouteResponseToListener(pAdminObject, MC_MATCH_CLAN_RESPONSE_AGREED_JOIN_CLAN, MERR_CLAN_MEMBER_FULL);
		RouteResponseToListener(pJoinerObject, MC_MATCH_CLAN_RESPONSE_AGREED_JOIN_CLAN, MERR_CLAN_MEMBER_FULL);
		return;
	}

	// Å¬·£Á¤º¸ ¾÷µ¥ÀÌÆ®ÇÏ°ERouteÇØÁÜ
	UpdateCharClanInfo(pJoinerObject, pAdminObject->GetCharInfo()->m_ClanInfo.m_nClanID, szClanName, MCG_MEMBER);
	// ÀÓ½ÃÄÚµE.. Àß¸øµÈ MMatchObject*°¡ ¿Â´Ù¸EÃ¼Å©ÇÏ¿© Àâ±âÀ§ÇÔ...20090224 by kammir
	if(pAdminObject->GetCharInfo()->m_ClanInfo.GetClanID() >= 9000000)
		LOG(LOG_FILE, "[ResponseAgreedJoinClan()] %s's ClanID:%d.", pAdminObject->GetAccountName(), pAdminObject->GetCharInfo()->m_ClanInfo.GetClanID());




	RouteResponseToListener(pAdminObject, MC_MATCH_CLAN_RESPONSE_AGREED_JOIN_CLAN, MOK);
	RouteResponseToListener(pJoinerObject, MC_MATCH_RESPONSE_RESULT, MRESULT_CLAN_JOINED);
}



void MMatchServer::OnClanRequestLeaveClan(const MUID& uidPlayer)
{
	ResponseLeaveClan(uidPlayer);
}

void MMatchServer::ResponseLeaveClan(const MUID& uidPlayer)
{
	MMatchObject* pLeaverObject = GetObject(uidPlayer);
	if (! IsEnabledObject(pLeaverObject)) return;

	int nRet = ValidateLeaveClan(pLeaverObject);

	if (nRet != MOK)
	{
		RouteResponseToListener(pLeaverObject, MC_MATCH_CLAN_RESPONSE_LEAVE_CLAN, nRet);

		// Custom: Notify user that clan masters need to use the clan close command
		if( nRet == MERR_CLAN_CANNOT_LEAVE )
			Announce( pLeaverObject, "Use the /clan close <clanname> command to leave the clan." );

		return;
	}


	int nCLID = pLeaverObject->GetCharInfo()->m_ClanInfo.m_nClanID;
	int nLeaverCID = pLeaverObject->GetCharInfo()->m_nCID;

	// ½ÇÁ¦·Î µðºñ»ó¿¡¼­ Å»ÅðÃ³¸®
	if (!m_MatchDBMgr.RemoveClanMember(nCLID, nLeaverCID))
	{
		RouteResponseToListener(pLeaverObject, MC_MATCH_CLAN_RESPONSE_LEAVE_CLAN, MERR_CLAN_CANNOT_LEAVE);
		return;
	}

	// Å¬·£Á¤º¸ ¾÷µ¥ÀÌÆ®ÇÏ°ERouteÇØÁÜ
	UpdateCharClanInfo(pLeaverObject, 0, "", MCG_NONE);


	RouteResponseToListener(pLeaverObject, MC_MATCH_CLAN_RESPONSE_LEAVE_CLAN, MOK);
}

void MMatchServer::OnClanRequestChangeClanGrade(const MUID& uidClanMaster, const char* szMember, int nClanGrade)
{
	ResponseChangeClanGrade(uidClanMaster, szMember, nClanGrade);
}

int ValidateChangeClanGrade(MMatchObject* pMasterObject, MMatchObject* pTargetObject, int nClanGrade)
{
	// ¸¶½ºÅÍÀÎÁEÈ®ÀÎ
	if (pMasterObject->GetCharInfo()->m_ClanInfo.m_nGrade != MCG_MASTER)
	{
		return MERR_CLAN_NOT_MASTER;
	}

	// °°Àº Å¬·£ÀÎÁEÈ®ÀÎ
	if (!IsSameClan(pMasterObject, pTargetObject))
	{
		return MERR_CLAN_OTHER_CLAN;
	}


	// ¸¶½ºÅÍ¸¦ ±ÇÇÑº¯°æÇÒ ¼E¾ø´Ù.
	if (pTargetObject->GetCharInfo()->m_ClanInfo.m_nGrade == MCG_MASTER)
	{
		return MERR_CLAN_NOT_MASTER;
	}

	MMatchClanGrade grade = (MMatchClanGrade)nClanGrade;

	// Custom: New IJJI clan changes.
	if ((grade != MCG_MASTER) && (grade != MCG_ADMIN) && (grade != MCG_MEMBER))
	{
		return MERR_CLAN_CANNOT_CHANGE_GRADE;
	}

	return MOK;
}

void MMatchServer::ResponseChangeClanGrade(const MUID& uidClanMaster, const char* szMember, int nClanGrade)
{
	MMatchObject* pMasterObject = GetObject(uidClanMaster);
	if (! IsEnabledObject(pMasterObject)) return;

	MMatchObject* pTargetObject = GetPlayerByName(szMember);
	if (! IsEnabledObject(pTargetObject))
	{
		RouteResponseToListener(pMasterObject, MC_MATCH_CLAN_MASTER_RESPONSE_CHANGE_GRADE, MERR_CLAN_OFFLINE_TARGET);
		return;
	}

	// ±ÇÇÑ º¯°E°¡´ÉÇÑÁEÃ¼Å©
	int nRet = ValidateChangeClanGrade(pMasterObject, pTargetObject, nClanGrade);
	if (nRet != MOK)
	{
		RouteResponseToListener(pMasterObject, MC_MATCH_CLAN_MASTER_RESPONSE_CHANGE_GRADE, nRet);
		return;
	}

	int nCLID = pMasterObject->GetCharInfo()->m_ClanInfo.m_nClanID;
	int nMemberCID = pTargetObject->GetCharInfo()->m_nCID;
	
	// ½ÇÁ¦·Î µðºñ»ó¿¡¼­ ±ÇÇÑ º¯°E
	if (!m_MatchDBMgr.UpdateClanGrade(nCLID, nMemberCID, nClanGrade))
	{
		RouteResponseToListener(pMasterObject, MC_MATCH_CLAN_MASTER_RESPONSE_CHANGE_GRADE, MERR_CLAN_CANNOT_CHANGE_GRADE);
		return;
	}

	// Custom: New IJJI clan changes.
	if ((MMatchClanGrade)nClanGrade == MCG_MASTER)
	{
		if (!m_MatchDBMgr.UpdateClanGrade(nCLID, pMasterObject->GetCharInfo()->m_nCID, MCG_MEMBER))
		{
			RouteResponseToListener(pMasterObject, MC_MATCH_CLAN_MASTER_RESPONSE_CHANGE_GRADE, MERR_CLAN_CANNOT_CHANGE_GRADE);
			return;
		}

		UpdateCharClanInfo(pMasterObject, pMasterObject->GetCharInfo()->m_ClanInfo.m_nClanID, 
							pMasterObject->GetCharInfo()->m_ClanInfo.m_szClanName, (MMatchClanGrade)MCG_MEMBER);
	}

	// Å¬·£Á¤º¸ ¾÷µ¥ÀÌÆ®ÇÏ°ERouteÇØÁÜ
	UpdateCharClanInfo(pTargetObject, pTargetObject->GetCharInfo()->m_ClanInfo.m_nClanID, 
						pTargetObject->GetCharInfo()->m_ClanInfo.m_szClanName, (MMatchClanGrade)nClanGrade);
	// ÀÓ½ÃÄÚµE.. Àß¸øµÈ MMatchObject*°¡ ¿Â´Ù¸EÃ¼Å©ÇÏ¿© Àâ±âÀ§ÇÔ...20090224 by kammir
	if(pTargetObject->GetCharInfo()->m_ClanInfo.GetClanID() >= 9000000)
		LOG(LOG_FILE, "[ResponseChangeClanGrade()] %s's ClanID:%d.", pTargetObject->GetAccountName(), pTargetObject->GetCharInfo()->m_ClanInfo.GetClanID());


	RouteResponseToListener(pMasterObject, MC_MATCH_CLAN_MASTER_RESPONSE_CHANGE_GRADE, MOK);
}


void MMatchServer::OnClanRequestExpelMember(const MUID& uidClanAdmin, const char* szMember)
{
	ResponseExpelMember(uidClanAdmin, szMember);
}

void MMatchServer::ResponseExpelMember(const MUID& uidClanAdmin, const char* szMember)
{
	MMatchObject* pAdminObject = GetObject(uidClanAdmin);
	if (! IsEnabledObject(pAdminObject)) return;

	// Å»ÅðÃ³¸®ÇÒ ¼EÀÖ´Â ±ÇÇÑÀÎÁE°Ë»E
	if (!IsUpperClanGrade(pAdminObject->GetCharInfo()->m_ClanInfo.m_nGrade, MCG_ADMIN))
	{
		RouteResponseToListener(pAdminObject, MC_MATCH_CLAN_ADMIN_RESPONSE_EXPEL_MEMBER, MERR_CLAN_NOT_MASTER_OR_ADMIN);
		return;
	}

	int nCLID = pAdminObject->GetCharInfo()->m_ClanInfo.m_nClanID;
	int nClanGrade = pAdminObject->GetCharInfo()->m_ClanInfo.m_nGrade;

	MAsyncDBJob_ExpelClanMember* pNewJob = new MAsyncDBJob_ExpelClanMember(uidClanAdmin);
	pNewJob->Input(uidClanAdmin,
					nCLID,
					nClanGrade,
					szMember);
	// PostAsyncJob(pNewJob);
	pAdminObject->m_DBJobQ.DBJobQ.push_back( pNewJob );

/*
	// µðºñ»ó¿¡¼­ Å»ÅðÃ³¸®
	int nDBRet = 0;
	int nCLID = pAdminObject->GetCharInfo()->m_ClanInfo.m_nClanID;
	int nClanGrade = pAdminObject->GetCharInfo()->m_ClanInfo.m_nGrade;
	char szTarMember[256]; 
	sprintf(szTarMember, szMember);

	// ½ÇÁ¦·Î µðºñ»ó¿¡¼­ ±ÇÇÑ º¯°E
	if (!m_MatchDBMgr.ExpelClanMember(nCLID, nClanGrade, szTarMember, &nDBRet))
	{
		RouteResponseToListener(pAdminObject, MC_MATCH_CLAN_ADMIN_RESPONSE_EXPEL_MEMBER, MERR_CLAN_CANNOT_EXPEL_FOR_NO_MEMBER);
		return;
	}


	switch (nDBRet)
	{
	case MMatchDBMgr::ER_NO_MEMBER:
		{
			RouteResponseToListener(pAdminObject, MC_MATCH_CLAN_ADMIN_RESPONSE_EXPEL_MEMBER, MERR_CLAN_CANNOT_EXPEL_FOR_NO_MEMBER);
			return;
		}
		break;
	case MMatchDBMgr::ER_WRONG_GRADE:
		{
			RouteResponseToListener(pAdminObject, MC_MATCH_CLAN_ADMIN_RESPONSE_EXPEL_MEMBER, MERR_CLAN_CANNOT_CHANGE_GRADE);
			return;
		}
		break;
	}


	// ¸¸¾E´ç»çÀÚ°¡ Á¢¼ÓÇØÀÖÀ¸¸EÅ¬·£Å»ÅðµÇ¾ú´Ù°E¾Ë·ÁÁà¾ßÇÑ´Ù.
	MMatchObject* pMemberObject = GetPlayerByName(szMember);
	if (IsEnabledObject(pMemberObject))
	{
		UpdateCharClanInfo(pMemberObject, 0, "", MCG_NONE);
	}


	RouteResponseToListener(pAdminObject, MC_MATCH_CLAN_ADMIN_RESPONSE_EXPEL_MEMBER, MOK);
*/
}


void MMatchServer::OnClanRequestMsg(const MUID& uidSender, const char* szMsg)
{
	MMatchObject* pSenderObject = GetObject(uidSender);
	if (! IsEnabledObject(pSenderObject)) return;
	
	if (!pSenderObject->GetCharInfo()->m_ClanInfo.IsJoined()) return;

	MCommand* pNewCmd = CreateCommand(MC_MATCH_CLAN_MSG, MUID(0,0));

	char szSenderName[256];
	char szTransMsg[512];
	strcpy(szSenderName, pSenderObject->GetCharInfo()->m_szName);
	strcpy(szTransMsg, szMsg);

	pNewCmd->AddParameter(new MCommandParameterString(szSenderName));
	pNewCmd->AddParameter(new MCommandParameterString(szTransMsg));


	int nCLID = pSenderObject->GetCharInfo()->m_ClanInfo.m_nClanID;

	RouteToClan(nCLID, pNewCmd);	
}

//void MMatchServer::OnClanRequestMemberList(const MUID& uidChar)
//{
//	ResponseClanMemberList(uidChar);
//}

void MMatchServer::OnClanRequestMemberList(const MUID& uidChar)
{
	MMatchObject* pObj = (MMatchObject*)GetObject(uidChar);
	if (! IsEnabledObject(pObj)) return;

	MMatchClan* pClan = FindClan(pObj->GetCharInfo()->m_ClanInfo.m_nClanID);
	if (pClan == NULL) return;

	MRefreshClientClanMemberImpl* pImpl = pObj->GetRefreshClientClanMemberImplement();
	pImpl->SetCategory(0);
	pImpl->SetChecksum(0);
	pImpl->Enable(true);
	pClan->SyncPlayerList(pObj, 0);
}

void MMatchServer::ResponseClanMemberList(const MUID& uidChar)
{
	MMatchObject* pObject = GetObject(uidChar);
	if (! IsEnabledObject(pObject)) return;

	if (!pObject->GetCharInfo()->m_ClanInfo.IsJoined()) return;

	MMatchClan* pClan = FindClan(pObject->GetCharInfo()->m_ClanInfo.m_nClanID);
	if (pClan == NULL) return;

	int nNodeCount = pClan->GetMemberCount();
	if (nNodeCount <= 0) return;

	MCommand* pNew = new MCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_CLAN_RESPONSE_MEMBER_LIST), MUID(0,0), m_This);

	void* pMemberArray = MMakeBlobArray(sizeof(MTD_ClanMemberListNode), nNodeCount);

	int nArrayIndex=0;
	for (MUIDRefCache::iterator itor= pClan->GetMemberBegin(); itor != pClan->GetMemberEnd(); ++itor) 
	{
		MMatchObject* pScanObj = (MMatchObject*)(*itor).second;

		MTD_ClanMemberListNode* pNode = (MTD_ClanMemberListNode*)MGetBlobArrayElement(pMemberArray, nArrayIndex++);

		if (IsEnabledObject(pScanObj))
		{
			CopyClanMemberListNodeForTrans(pNode, pScanObj);		
		}

		if (nArrayIndex >= nNodeCount) break;
	}

	pNew->AddParameter(new MCommandParameterBlob(pMemberArray, MGetBlobArraySize(pMemberArray)));
	MEraseBlobArray(pMemberArray);
	RouteToListener(pObject, pNew);
}

void CopyClanInfoForTrans(MTD_ClanInfo* pDest, MMatchClan* pClan)
{
	strcpy(pDest->szClanName, pClan->GetName());
	strcpy(pDest->szMaster, pClan->GetClanInfoEx()->szMaster);
	pDest->nLevel = pClan->GetClanInfoEx()->nLevel;
	pDest->nTotalPoint = pClan->GetClanInfoEx()->nTotalPoint;
	pDest->nRanking = pClan->GetClanInfoEx()->nRanking;
	pDest->nWins = pClan->GetClanInfoEx()->nWins;
	pDest->nLosses = pClan->GetClanInfoEx()->nLosses;
	pDest->nPoint = pClan->GetClanInfoEx()->nPoint;
	pDest->nTotalMemberCount = pClan->GetClanInfoEx()->nTotalMemberCount;
	pDest->nConnedMember = pClan->GetMemberCount();
	pDest->nCLID = pClan->GetCLID();
	pDest->nEmblemChecksum = pClan->GetEmblemChecksum();
}

void MMatchServer::OnClanRequestClanInfo(const MUID& uidChar, const char* szClanName)
{
	MMatchObject* pObject = GetObject(uidChar);
	if (! IsEnabledObject(pObject)) return;

	MMatchClan* pClan = m_ClanMap.GetClan(szClanName);
	if ((pClan == NULL) || (!pClan->IsInitedClanInfoEx())) return;

	MCommand* pNew = new MCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_CLAN_RESPONSE_CLAN_INFO), MUID(0,0), m_This);

	void* pClanInfoArray = MMakeBlobArray(sizeof(MTD_ClanInfo), 1);
	MTD_ClanInfo* pClanInfo = (MTD_ClanInfo*)MGetBlobArrayElement(pClanInfoArray, 0);
	CopyClanInfoForTrans(pClanInfo, pClan);

	pNew->AddParameter(new MCommandParameterBlob(pClanInfoArray, MGetBlobArraySize(pClanInfoArray)));
	MEraseBlobArray(pClanInfoArray);
	RouteToListener(pObject, pNew);
}

void MMatchServer::OnClanRequestEmblemURL(const MUID& uidChar, void* pEmblemURLListBlob)
{
	MMatchObject* pObject = GetObject(uidChar);
	if (! IsEnabledObject(pObject)) return;

	int nClanURLCount = MGetBlobArrayCount(pEmblemURLListBlob);
	if (nClanURLCount < 1) return;

	for (int i = 0; i < nClanURLCount; i++)
	{
		int* pClanID = (int*)MGetBlobArrayElement(pEmblemURLListBlob, i);
		MMatchClan* pClan = m_ClanMap.GetClan(*pClanID);
		if (pClan == NULL) continue;

		MCommand* pNew = CreateCommand(MC_MATCH_CLAN_RESPONSE_EMBLEMURL, MUID(0,0));
		pNew->AddParameter(new MCmdParamInt(pClan->GetCLID()));
		pNew->AddParameter(new MCmdParamInt(pClan->GetEmblemChecksum()));
		pNew->AddParameter(new MCmdParamStr(pClan->GetEmblemURL()));
		RouteToListener(pObject, pNew);
	}
}

void MMatchServer::StandbyClanList(const MUID& uidPlayer, int nClanListStartIndex, bool bCacheUpdate)
{
	MMatchObject* pObject = GetObject(uidPlayer);
	if (! IsEnabledObject(pObject)) return;

	int nPrevClanListCount = -1, nNextClanListCount = -1;
	int nGroupCount = (int)GetLadderMgr()->GetGroupCount();
	if (nClanListStartIndex < 0) nClanListStartIndex = 0;
	if (nClanListStartIndex > nGroupCount) nClanListStartIndex = nGroupCount;


	MCommand* pNew = new MCommand(m_CommandManager.GetCommandDescByID(MC_MATCH_CLAN_STANDBY_CLAN_LIST), MUID(0,0), m_This);


	nPrevClanListCount = nClanListStartIndex;
	if ((nPrevClanListCount < 0) || (nPrevClanListCount > nGroupCount)) nPrevClanListCount = 0;
	nNextClanListCount = nGroupCount - (nClanListStartIndex + MAX_LADDER_TEAM_MEMBER);
	if ((nNextClanListCount < 0) || (nNextClanListCount > nGroupCount)) nNextClanListCount = 0;

	pNew->AddParameter(new MCommandParameterInt(nPrevClanListCount));
	pNew->AddParameter(new MCommandParameterInt(nNextClanListCount));


	int nRealCount = max(0, min(MAX_LADDER_TEAM_MEMBER, (nGroupCount - nClanListStartIndex)));

	list<MLadderGroup*>::iterator itorGroup = GetLadderMgr()->GetGroupListBegin();
	for (int i = 0; i < nClanListStartIndex; i++)
	{
		if (itorGroup == GetLadderMgr()->GetGroupListEnd()) break;
		itorGroup++;
	}

	int nArrayIndex=0;
	void* pClanListArray = MMakeBlobArray(sizeof(MTD_StandbyClanList), nRealCount);

	for (int i = 0; i < nRealCount; i++)
	{
		if (itorGroup == GetLadderMgr()->GetGroupListEnd()) break;

		MTD_StandbyClanList* pNode = (MTD_StandbyClanList*)MGetBlobArrayElement(pClanListArray, i);
		memset(pNode, 0, sizeof(MTD_StandbyClanList));

		MLadderGroup* pLadderGroup = *itorGroup;
		
		if (pLadderGroup->GetPlayerCount() > 0)
		{
			MUID uidMember = *pLadderGroup->GetPlayerListBegin();
			MMatchObject* pMember = GetObject(uidMember);
			if ((IsEnabledObject(pMember)) && (pMember->GetCharInfo()->m_ClanInfo.IsJoined()))
			{
				strcpy(pNode->szClanName, pMember->GetCharInfo()->m_ClanInfo.m_szClanName);
			}

			pNode->nPlayers = (int)pLadderGroup->GetPlayerCount();
			pNode->nCLID = pLadderGroup->GetCLID();

			MMatchClan* pClan = FindClan(pLadderGroup->GetCLID());
			if (pClan)
				pNode->nEmblemChecksum = pClan->GetEmblemChecksum();
			else
				pNode->nEmblemChecksum = 0;
		}
		itorGroup++;
	}

	pNew->AddParameter(new MCommandParameterBlob(pClanListArray, MGetBlobArraySize(pClanListArray)));
	MEraseBlobArray(pClanListArray);
	
	RouteToListener(pObject, pNew);	
}


void MMatchServer::SaveClanPoint(MMatchClan* pWinnerClan, MMatchClan* pLoserClan, const bool bIsDrawGame,
								 const int nRoundWins, const int nRoundLosses, const int nMapID, const int nGameType,
								 const int nOneTeamMemberCount, list<MUID>& WinnerObjUIDs,
								 const char* szWinnerMemberNames, const char* szLoserMemberNames,
								 float fPointRatio)
{
	if (bIsDrawGame) return;
	if ((!pWinnerClan) || (!pLoserClan)) return;

	int nAddedWinnerPoint = 0, nAddedLoserPoint = 0;

	int nWinnerClanPoint = pWinnerClan->GetClanInfoEx()->nPoint;
	int nLoserClanPoint = pLoserClan->GetClanInfoEx()->nPoint;

	int nWinnerCLID = pWinnerClan->GetCLID();
	int nLoserCLID = pLoserClan->GetCLID();

	int nPoint = MMatchFormula::GetClanBattlePoint(nWinnerClanPoint, nLoserClanPoint, nOneTeamMemberCount);

	nAddedWinnerPoint = nPoint;

	// Custom: 5vs5 clan wars
	if (nOneTeamMemberCount == MLADDERTYPE_NORMAL_5VS5)
		nAddedWinnerPoint = nAddedWinnerPoint * 2.5f;
	else if (nOneTeamMemberCount == MLADDERTYPE_NORMAL_4VS4) // 4v4ÀÎÀEÆ÷ÀÎÆ®°¡ µÎ¹E
		nAddedWinnerPoint = nAddedWinnerPoint * 2;	
	else if (nOneTeamMemberCount == MLADDERTYPE_NORMAL_3VS3) 
		nAddedWinnerPoint = (int)(nAddedWinnerPoint * 1.5f);	
#ifdef _LADDERGAME
	else if (nOneTeamMemberCount == MLADDERTYPE_NORMAL_1VS1)
		nAddedWinnerPoint = (int)(nAddedWinnerPoint * 0.6f); // Custom: 1 vs 1
#endif

	nAddedWinnerPoint = int(nAddedWinnerPoint * fPointRatio);

	nAddedLoserPoint = -(nPoint / 2);


	if (pWinnerClan)
	{
		pWinnerClan->IncWins(1, nGameType);
		pWinnerClan->IncPoint(nAddedWinnerPoint, nGameType);
		pWinnerClan->InsertMatchedClanID(nLoserCLID);
	}

	if (pLoserClan)
	{
		pLoserClan->IncLosses(1, nGameType);
		pLoserClan->IncPoint(nAddedLoserPoint, nGameType);
		pLoserClan->InsertMatchedClanID(nWinnerCLID);
	}

	MAsyncDBJob_WinTheClanGame* pNewJob = new MAsyncDBJob_WinTheClanGame();
	pNewJob->Input(nWinnerCLID, 
				   nLoserCLID, 
				   bIsDrawGame,
                   nAddedWinnerPoint, 
				   nAddedLoserPoint,
                   pWinnerClan->GetName(), 
				   pLoserClan->GetName(),
                   nRoundWins, 
				   nRoundLosses, 
				   nMapID, 
				   nGameType, 
				   szWinnerMemberNames, 
				   szLoserMemberNames );
	PostAsyncJob(pNewJob);


	// Ä³¸¯ÅÍÀÇ Å¬·£ ±â¿©µµ ¾÷µ¥ÀÌÆ®
	for (list<MUID>::iterator itor=WinnerObjUIDs.begin(); itor!=WinnerObjUIDs.end(); itor++) 
	{
		MUID uidObject = (*itor);

		MMatchObject* pObject = GetObject(uidObject);
		if (IsEnabledObject(pObject))
		{
			int nCID = pObject->GetCharInfo()->m_nCID;
			pObject->GetCharInfo()->m_ClanInfo.m_nContPoint += nAddedWinnerPoint;

			MAsyncDBJob_UpdateCharClanContPoint* pJob=new MAsyncDBJob_UpdateCharClanContPoint(nCID, nWinnerCLID, nAddedWinnerPoint);
			PostAsyncJob(pJob);
		}
	}

	
}
