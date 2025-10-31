#include "stdafx.h"
#include "MAsyncDBJob_LadderWars.h"

void MAsyncDBJob_GetLadderWarsCharInfo::Run(void* pContext)
{
	MMatchDBMgr* pDBMgr = (MMatchDBMgr*)pContext;
	if (!pDBMgr->GetLadderWarsCharInfo(m_dwPlayerCID, &m_pBlitzCharInfo) ) {
			SetResult(MASYNC_RESULT_FAILED);
			return;
		}
	SetResult(MASYNC_RESULT_SUCCEED);
}

void MAsyncDBJob_UpdateLadderWarsCharInfo::Run(void* pContext)
{
	MMatchDBMgr* pDBMgr = (MMatchDBMgr*)pContext;
	if (!pDBMgr->UpdateLadderWarsCharInfo(m_dwPlayerCID, Win, Loss, Draw, Score, Mode) ) {
			SetResult(MASYNC_RESULT_FAILED);
			return;
	}
	//MLog("Win = %d / Loss = %d / Score = %d / Mode = %d \n", Win, Loss, Score, Mode);
	SetResult(MASYNC_RESULT_SUCCEED);
}

void MAsyncDBJob_GetLadderWarsSideRankingInfo::Run(void* pContext)
{
	MMatchDBMgr* pDBMgr = (MMatchDBMgr*)pContext;
	if (!pDBMgr->GetLadderWarsSideRankingInfo(m_dwPlayerCID, &m_SideRankingList) ) {
			SetResult(MASYNC_RESULT_FAILED);
			return;
		}
	SetResult(MASYNC_RESULT_SUCCEED);
}