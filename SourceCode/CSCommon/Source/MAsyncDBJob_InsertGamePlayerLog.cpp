#include "stdafx.h"
#include "MAsyncDBJob_InsertGamePlayerLog.h"

bool MAsyncDBJob_InsertGamePlayerLog::Input(unsigned int nGameLogID, unsigned int nCID, unsigned int nPlayTime, unsigned int nKills, unsigned int nDeaths, int nXP, unsigned int nBP)
{
	m_nGameLogID = nGameLogID;

	m_nCID = nCID;
	m_nPlayTime = nPlayTime;
	m_nKills = nKills;
	m_nDeaths = nDeaths;
	m_nXP = nXP;
	m_nBP = nBP;

	return true;
}

void MAsyncDBJob_InsertGamePlayerLog::Run(void* pContext)
{
	MMatchDBMgr* pDBMgr = (MMatchDBMgr*)pContext;

	if (!pDBMgr->InsertGamePlayerLog(m_nGameLogID, m_nCID, m_nPlayTime, m_nKills, m_nDeaths, m_nXP, m_nBP)) {
		SetResult(MASYNC_RESULT_FAILED);
		return;
	}

	SetResult(MASYNC_RESULT_SUCCEED);
}
