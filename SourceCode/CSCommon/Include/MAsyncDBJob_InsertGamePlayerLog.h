#pragma once

#include "MAsyncDBJob.h"

class MAsyncDBJob_InsertGamePlayerLog : public MAsyncJob 
{
protected: // Input Argument
	unsigned int m_nGameLogID;
	unsigned int m_nCID;
	unsigned int m_nPlayTime;
	unsigned int m_nKills;
	unsigned int m_nDeaths;
	unsigned int m_nXP;
	unsigned int m_nBP;

protected:	// Output Result

public:
	MAsyncDBJob_InsertGamePlayerLog() : MAsyncJob(MASYNCJOB_INSERTGAMEPLAYERLOG, MUID(0, 0)) {}
	virtual ~MAsyncDBJob_InsertGamePlayerLog() {}

	bool Input(unsigned int nGameLogID, unsigned int nCID, unsigned int nPlayTime, unsigned int nKills, unsigned int nDeaths, int nXP, unsigned int nBP);

	virtual void Run(void* pContext);
};