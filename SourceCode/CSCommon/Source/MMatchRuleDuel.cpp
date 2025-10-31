#include "stdafx.h"
#include "MMatchRuleDuel.h"
#include "MMatchTransDataType.h"
#include "MBlobArray.h"
#include "MMatchServer.h"

#include <algorithm>
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////////
// MMatchRuleDuel	   ///////////////////////////////////////////////////////////
MMatchRuleDuel::MMatchRuleDuel(MMatchStage* pStage) : MMatchRule(pStage)
{

}

void MMatchRuleDuel::OnBegin()
{
	uidChampion = MUID(0, 0);
	uidChallenger = MUID(0, 0);

	MMatchStage* pStage = GetStage();

	WaitQueue.clear();	// ｴ・・ﾅ･ｸｦ ｺ・・

	if (pStage != NULL)
	{
		for(MUIDRefCache::iterator itor=pStage->GetObjBegin(); itor!=pStage->GetObjEnd(); itor++) 
		{
			MMatchObject* pItorObj = (MMatchObject*)(*itor).second;

			// Custom: Hide GM from Duel Queue. Also added eventteam to this
			if (pItorObj && IsAdminGrade(pItorObj) && pItorObj->CheckPlayerFlags(MTD_PlayerFlags_AdminHide))
				continue;

			WaitQueue.push_back((*itor).first);			// ﾇﾃｷｹﾀﾌｾ鋙・ｱﾗｳﾉ ｸ･ ｴ・・ﾅ･ｿ｡ ｳﾖｴﾂｴﾙ.
		}

//		SpawnPlayers();
	}

	nVictory = 0;

	return;
}

void MMatchRuleDuel::OnEnd()
{
}


void MMatchRuleDuel::OnRoundBegin()
{
	isRoundEnd = false;
	isTimeover = true;

	SpawnPlayers();
	SendQueueInfo(true);
	// ｿﾖｱﾗｷｱﾁ・ｸｵ ｿﾉﾀ彧・ﾇﾘｾﾟ ﾇﾒ ｳﾌ ｽｺﾆﾈｰﾉ ｺﾁｼｭ -_- ﾁﾗｿｩｹｰｴﾙ. ｼｭｹ｡ｼｱ ｽｺﾆﾈｵﾆﾀｸｴﾏ ｴﾌｻﾇ ﾃｳｸｮｴﾂ ﾇﾊｿ萓ｻｵ・
	// ﾀﾌｰﾅ ﾇﾑ ﾈﾄｿ｡ ｽｺﾆﾇｴﾂｰﾅｸ・ｳｶﾆﾒｵ･;
	for (list<MUID>::iterator i = WaitQueue.begin(); i!=WaitQueue.end();  ++i)
		MMatchServer::GetInstance()->OnDuelSetObserver(*i);							

	// Custom: Set observer for hidden GMs. fixed for eventteam
	for (MUIDRefCache::iterator i=m_pStage->GetObjBegin(); i!=m_pStage->GetObjEnd(); i++) 
	{
		MMatchObject* pObj = (MMatchObject*)(*i).second;
		if (pObj->GetEnterBattle() == false) continue;

#ifdef _SPEC 1
		if (IsAdminGrade(pObj) && pObj->CheckPlayerFlags(MTD_PlayerFlags_AdminHide) ||
			pObj->GetAccountInfo()->m_nUGrade == MMUG_EVENTTEAM && pObj->CheckPlayerFlags(MTD_PlayerFlags_AdminHide) && pObj->GetTeam() == MMT_SPECTATOR)
#else
		if (IsAdminGrade(pObj) && pObj->CheckPlayerFlags(MTD_PlayerFlags_AdminHide))
#endif	
		{
			MUID uid = (*i).first;
			MMatchServer::GetInstance()->OnDuelSetObserver( uid );
		}
	}
}


void MMatchRuleDuel::OnRoundEnd()
{
	if (isTimeover)
	{	
		WaitQueue.push_back(uidChampion);
		WaitQueue.push_back(uidChallenger);
		uidChampion = uidChallenger = MUID(0, 0);
		nVictory = 0;
	}
	else
	{
		if (isChangeChampion || uidChampion == MUID(0, 0))				// ﾃｨﾇﾇｿﾂﾀﾌ ｹﾙｲ﨨﨨ﾟ ﾇﾏｸ・ﾀﾏｴﾜ ﾃｨﾇﾇｿﾂｰ・ｵｵﾀ・ﾚｸｦ ｽｺｿﾒ
		{
			MUID uidTemp;
			uidTemp = uidChampion;
			uidChampion = uidChallenger;
			uidChallenger = uidTemp;
		}

		if (uidChallenger != MUID(0, 0))
		{
			WaitQueue.push_back(uidChallenger);	// ｵｵﾀ・ﾚｴﾂ ﾅ･ﾀﾇ ｸﾇ ｵﾚｷﾎ ｹﾐｾ鋧ﾖｰ・
			uidChallenger = MUID(0, 0);			// ｵｵﾀ・ﾚﾀﾇ idｸｦ ｹｫﾈｿﾈｭ
		}
	}

//	SpawnPlayers();
	LogInfo();
}

bool MMatchRuleDuel::RoundCount() 
{
	if (m_pStage == NULL) return false;

	int nTotalRound = m_pStage->GetStageSetting()->GetRoundMax();
	m_nRoundCount++;

	MMatchStage* pStage = GetStage();
	for (MUIDRefCache::iterator i=pStage->GetObjBegin(); i!=pStage->GetObjEnd(); i++) 
	{
		MMatchObject* pObj = (MMatchObject*)(*i).second;
		if (pObj->GetEnterBattle() == false) continue;

		if (pObj->GetAllRoundKillCount() >= (unsigned int)pStage->GetStageSetting()->GetRoundMax())
		{
			return false;
		}
	}

	return true;
}

bool MMatchRuleDuel::OnCheckRoundFinish()
{
	if (!isRoundEnd)
		return false;
	else
	{
		isRoundEnd = false;
		isTimeover = false;
		return true;	
	}
}

void MMatchRuleDuel::OnRoundTimeOut()
{
	SetRoundArg(MMATCH_ROUNDRESULT_DRAW);
}


void MMatchRuleDuel::OnGameKill(const MUID& uidAttacker, const MUID& uidVictim)
{
	isRoundEnd = true;

	/// ﾅｩｷ｡ｽｬｷﾎ ﾀﾎﾇﾑ ｹ貔・ﾄﾚｵ・ﾃﾟｰ｡(ﾃ箙ﾂｿ｡ｼｭ 0x00000000 ﾀﾌｰｪﾀｸｷﾎ ｿ｡ｷｯｰ｡ ｳｵｱ筝ｧｹｮｿ｡ ﾀﾇｽﾉｰ｡ｴﾂ ｺﾎｺﾐﾀｻ ｰﾉｷｯﾁﾘｴﾙ.)
	if( MMatchServer::GetInstance()->GetChannelMap()->Find(m_pStage->GetOwnerChannel()) == NULL ) 
	{
		mlog( "error: can't find OwnerChannel [ MMatchRuleDuel::OnGameKill() ]\n" );
		//LOG(LOG_FILE, "error: can't find OwnerChannel [ MMatchRuleDuel::OnGameKill() ]\n" );
		return;
	}
	if( m_pStage == NULL)
	{
		mlog( "error: can't find m_pStage [ MMatchRuleDuel::OnGameKill() ]\n" );
		//LOG(LOG_FILE, "error: can't find m_pStage [ MMatchRuleDuel::OnGameKill() ]\n" );
		return;
	}

	MUID chanID = MMatchServer::GetInstance()->GetChannelMap()->Find(m_pStage->GetOwnerChannel())->GetUID();

	if (uidVictim == uidChallenger)		// ﾃｨﾇﾇｿﾂﾀﾌ ﾀ篝ﾒﾀｸｸ・ﾃｨﾇﾇｿﾂ ﾀｯﾁ・
	{
		isChangeChampion = false;
		nVictory++;

		if (m_pStage == NULL) return;
		if (m_pStage->IsPrivate()) return;		// ｺﾐｹ貘ﾌｸ・ｹ貍ﾛ ﾆﾐｽｺ

		if (nVictory % 10 != 0) return;			// ｿｬｽﾂｼ｡ 10ﾀﾇ ｹ霈ﾏｶｧｸｸ

		MMatchObject* pChamp;
		pChamp = m_pStage->GetObj(uidChampion);
		if (pChamp == NULL) return;
		
		MMatchServer::GetInstance()->BroadCastDuelRenewVictories(
			chanID,
			pChamp->GetName(), 
			MMatchServer::GetInstance()->GetChannelMap()->Find(m_pStage->GetOwnerChannel())->GetName(), 
			m_pStage->GetIndex()+1,
			nVictory
			);
	}
	else
	{
		isChangeChampion = true;

		int nowVictory = nVictory;

		nVictory = 1;

		if (nowVictory < 10) return;				// 10ｿｬｽﾂ ﾀﾌｻｻ ﾀ愠ﾟﾀｻｶｧｸｸ
		if (m_pStage == NULL) return;
		if (m_pStage->IsPrivate()) return;		// ｺﾐｹ貘ﾌｸ・ｹ貍ﾛ ﾆﾐｽｺ
	
		MMatchObject* pChamp, *pChallenger;
		pChamp = m_pStage->GetObj(uidChampion);
		if (pChamp == NULL) return;
		pChallenger = m_pStage->GetObj(uidChallenger);
		if (pChallenger == NULL) return;

		if (strcmp(m_pStage->GetPassword(), "") != 0) return;

		MMatchServer::GetInstance()->BroadCastDuelInterruptVictories(
			chanID,
			pChamp->GetName(),
			pChallenger->GetName(),
			nowVictory
			);
	}


	LogInfo();

}

void MMatchRuleDuel::OnEnterBattle(MUID& uidChar)
{
	MMatchObject* pJoiner = m_pStage->GetObj( uidChar );

	// Custom: Don't add hidden GM to duel queue.
#ifdef _SPEC 1
	if (pJoiner && IsAdminGrade(pJoiner) && pJoiner->CheckPlayerFlags(MTD_PlayerFlags_AdminHide) ||
		pJoiner && pJoiner->GetAccountInfo()->m_nUGrade == MMUG_EVENTTEAM && pJoiner->CheckPlayerFlags(MTD_PlayerFlags_AdminHide) && pJoiner->GetTeam() == MMT_SPECTATOR)
#else
	if (pJoiner && IsAdminGrade(pJoiner) && pJoiner->CheckPlayerFlags(MTD_PlayerFlags_AdminHide))
#endif
	{
		SendQueueInfo();
		LogInfo();
		return;
	}
#ifdef _SPEC 1
	MMatchObject* pChar;
	pChar = m_pStage->GetObj(uidChar);
	if (pChar->GetTeam() == MMT_SPECTATOR)
		return;
#endif
	if ((uidChar != uidChampion) && (uidChar != uidChallenger) && (find(WaitQueue.begin(), WaitQueue.end(), uidChar) == WaitQueue.end()))
	{
		WaitQueue.push_back(uidChar);
		SpawnPlayers();
	}
	SendQueueInfo();
	LogInfo();
}

void MMatchRuleDuel::OnLeaveBattle(MUID& uidChar)
{
	if (uidChar == uidChampion)
	{
		isChangeChampion = true;
		isRoundEnd = true;
		uidChampion = MUID(0, 0);
		nVictory = 0;
	}
	else if (uidChar == uidChallenger)
	{
		isChangeChampion = false;
		isRoundEnd = true;
		uidChallenger = MUID(0, 0);
	}
	else
	{
		WaitQueue.remove(uidChar);
		SendQueueInfo();
		LogInfo();
	}
}
#ifdef _SPEC 1
void MMatchRuleDuel::OnTeam(const MUID& uidPlayer, MMatchTeam nTeam)
{
	if (nTeam != MMT_SPECTATOR)
		return;

	auto it = std::find(WaitQueue.begin(), WaitQueue.end(), uidPlayer);

	if (it == WaitQueue.end())
		return;

	WaitQueue.remove(uidPlayer);
}
#endif
void MMatchRuleDuel::SpawnPlayers()
{
	if (uidChampion == MUID(0, 0))
	{
		if (!WaitQueue.empty())
		{
			uidChampion = WaitQueue.front();
			WaitQueue.pop_front();
		}
	}
	if (uidChallenger == MUID(0, 0))
	{
		if (!WaitQueue.empty())
		{
			uidChallenger = WaitQueue.front();
			WaitQueue.pop_front();
		}
	}
}

bool MMatchRuleDuel::OnCheckEnableBattleCondition()
{
	if (uidChampion == MUID(0, 0) || uidChallenger == MUID(0, 0))
	{
		if (WaitQueue.empty())
			return false;
		else
			isRoundEnd = true;
	}

	return true;
}

void MMatchRuleDuel::LogInfo()
{
#ifdef _DEBUG
	if (m_pStage == NULL) return;
	MMatchObject* pObj;
	char buf[250];
	sprintf(buf, "Logging Que--------------------\n");
	OutputDebugString(buf);

	pObj = m_pStage->GetObj(uidChampion);
	if (pObj != NULL)
	{
		sprintf(buf, "Champion name : %s \n", pObj->GetName());
		OutputDebugString(buf);
	}

	pObj = m_pStage->GetObj(uidChallenger);
	if (pObj != NULL)
	{
		sprintf(buf, "Challenger name : %s \n", pObj->GetName());
		OutputDebugString(buf);
	}

	int x = 0;
	for (list<MUID>::iterator i = WaitQueue.begin(); i!=WaitQueue.end();  ++i)
	{
		pObj = m_pStage->GetObj(*i);
		if (pObj != NULL)
		{
			sprintf(buf, "Wait Queue #%d : %s \n", x, pObj->GetName());
			OutputDebugString(buf);		
			x++;
		}
	}
#endif
}

void MMatchRuleDuel::SendQueueInfo(bool isRoundEnd)
{
	if (m_pStage == NULL) return;
	MTD_DuelQueueInfo QInfo;
	QInfo.m_uidChampion = uidChampion;
	QInfo.m_uidChallenger = uidChallenger;
	QInfo.m_nQueueLength = static_cast<char>(WaitQueue.size());
	QInfo.m_nVictory = nVictory;
	QInfo.m_bIsRoundEnd = isRoundEnd;

	int i=0;
	list<MUID>::const_iterator itEnd = WaitQueue.end();
	for (list<MUID>::iterator iter = WaitQueue.begin(); iter != itEnd; ++iter, ++i)
	{
		if( 14 > i )
		{
			QInfo.m_WaitQueue[i] = *iter;
		}
		else
		{
			mlog( "duel queue info index error. size : %u\n", WaitQueue.size() );
			break;
		}
	}

	MMatchServer::GetInstance()->OnDuelQueueInfo(m_pStage->GetUID(), QInfo);
}