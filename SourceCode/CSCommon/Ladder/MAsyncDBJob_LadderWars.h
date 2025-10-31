#pragma once

#ifndef _MASYNCDBJOB_LadderWars
#define _MASYNCDBJOB_LadderWars

#include "MAsyncDBJob.h"

class MAsyncDBJob_GetLadderWarsCharInfo : public MAsyncJob
{
protected:
	DWORD m_dwPlayerCID;

	MUID m_uidPlayer;
	LadderWarsCharInfo m_pBlitzCharInfo;
public:
	MAsyncDBJob_GetLadderWarsCharInfo(void) : MAsyncJob(MASYNCJOB_GET_PW_INFO, MUID(0, 0)) {
		m_dwPlayerCID = 0;
		m_uidPlayer = MUID(0, 0);
	}
	~MAsyncDBJob_GetLadderWarsCharInfo(void){}

	void Input(MUID uidPlayer, DWORD dwPlayerCID) {
		m_uidPlayer = uidPlayer;
		m_dwPlayerCID = dwPlayerCID;		
	}

	virtual void Run(void* pContext);

	MUID GetPlayerUID()									{ return m_uidPlayer; }
	LadderWarsCharInfo* GetPWCharInfo()	{ return &m_pBlitzCharInfo; }	
};

class MAsyncDBJob_UpdateLadderWarsCharInfo : public MAsyncJob
{
protected:
	DWORD m_dwPlayerCID;
	int Win, Loss, Draw, Score, Mode;
public:
	MAsyncDBJob_UpdateLadderWarsCharInfo(void) : MAsyncJob(MASYNCJOB_UPDATE_PW_INFO, MUID(0, 0)) {
		m_dwPlayerCID = 0;
	}
	~MAsyncDBJob_UpdateLadderWarsCharInfo(void){}

	void Input(DWORD dwPlayerCID, int win, int loss, int draw, int score, int mode) {
		m_dwPlayerCID = dwPlayerCID;
		Win = win;
		Loss = loss;
		Draw = draw;
		Score = score;
		Mode = mode;
	}

	virtual void Run(void* pContext);
};

class MAsyncDBJob_GetLadderWarsSideRankingInfo : public MAsyncJob
{
protected:
	DWORD m_dwPlayerCID;

	MUID m_uidPlayer;
	list<PWRankingInfo*> m_SideRankingList;
public:
	MAsyncDBJob_GetLadderWarsSideRankingInfo(void) : MAsyncJob(MASYNCJOB_GET_PW_SIDE_RANKING, MUID(0, 0)) {
		m_dwPlayerCID = 0;
		m_uidPlayer = MUID(0, 0);
	}

	~MAsyncDBJob_GetLadderWarsSideRankingInfo(void)
	{
		RemoveListAll();
	}


	void Input(MUID uidPlayer, DWORD dwPlayerCID) {
		m_uidPlayer = uidPlayer;
		m_dwPlayerCID = dwPlayerCID;		
	}

	virtual void Run(void* pContext);

	MUID GetPlayerUID()							{ return m_uidPlayer; }
	list<PWRankingInfo*>* GetSideRankingList()	{ return &m_SideRankingList; }	

	void RemoveListAll()
	{
		list<PWRankingInfo*>::iterator iter = m_SideRankingList.begin();
		for(; iter != m_SideRankingList.end(); ){
			PWRankingInfo *pInfo = (*iter);
			delete pInfo;

			iter = m_SideRankingList.erase(iter);			
		}
	}
};

#endif