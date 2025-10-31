#include "stdafx.h"
#include "MMatchServer.h"
#include "MSharedCommandTable.h"
#include "MErrorTable.h"
#include "MBlobArray.h"
#include "MAgentObject.h"
#include "MDebug.h"
#include "MCommandCommunicator.h"
#include "MCommandBuilder.h"
#ifdef _VOICECHAT
int MMatchServer::TeamSpeakAdd(const MUID& uidComm)
{
	m_TeamSpeak = new MAgentObject(uidComm);
	LOG(LOG_PROG, "TeamSpeak Added (UID:%d%d)", m_TeamSpeak->GetUID().High, m_TeamSpeak->GetUID().Low);
	return MOK;
}

void MMatchServer::TeamSpeakClear()
{
	if(m_TeamSpeak)
	{
		LOG(LOG_PROG, "TeamSpeak Removed (UID:%d%d)", m_TeamSpeak->GetUID().High, m_TeamSpeak->GetUID().Low);
		delete m_TeamSpeak;
	}
}

void MMatchServer::OnRegisterTeamSpeak(const MUID& uidComm, char* szIP, char* AgentIpConfig, int nUDPPort, bool AgentBlock)
{
	LOG(LOG_PROG, "Start Checker Register (CommUID %u:%u) IP:%s, UDPPort:%d\n ", 
		uidComm.High, uidComm.Low, szIP, nUDPPort);
	TeamSpeakClear();

	int nErrCode = TeamSpeakAdd(uidComm);
	if(nErrCode!=MOK)
	{
		LOG(LOG_DEBUG, MErrStr(nErrCode) );
	}

	LockCommList();
	MCommObject* pCommObj = (MCommObject*)m_CommRefCache.GetRef(uidComm);
	if (pCommObj)
	{
		pCommObj->GetCommandBuilder()->SetCheckCommandSN(false);
	}
	UnlockCommList();

	LOG(LOG_PROG, "TeamSpeak Registered (CommUID %u:%u) IP:%s, UDPPort:%d\n ", 
		uidComm.High, uidComm.Low, szIP, nUDPPort);
}

void MMatchServer::OnUnRegisterTeamSpeak(const MUID& uidComm)
{
	TeamSpeakClear();
	LOG(LOG_DEBUG, "TeamSpeak Unregistered (CommUID %u:%u) Cleared", uidComm.High, uidComm.Low);
}

void MMatchServer::OnRequestLiveTeamSpeak(const MUID& uidComm, unsigned long nTimeStamp, unsigned long nUserCount)
{
	MCommand* pCmd = CreateCommand(MC_MATCH_CHECKER_RESPONSE_LIVECHECK, uidComm);
	pCmd->AddParameter(new MCmdParamUInt(nTimeStamp));
	PostSafeQueue(pCmd);
}
#endif