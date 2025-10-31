#include "stdafx.h"
#include "MBMatchAsyncDBJob_GameOnLogin.h"
#include "MBMatchGameOnAuth.h"
#include "MMatchObject.h"
#include "MMatchDBMgr.h"


bool MBMatchAsyncDBJob_GameOnLogin::Input( const MUID& uidCommUID, const string& strUserID, const int nCheckSum,
									   const bool bIsFreeLoginIP, const string strCountryCode3, const int nServerID )
{
	m_CommUID			= uidCommUID;
	m_strUserID			= strUserID;
	m_nCheckSum			= nCheckSum;
	m_bIsFreeLoginIP	= bIsFreeLoginIP;
	m_strCountryCode3	= strCountryCode3;
	m_nServerID			= nServerID;

	m_pAccountInfo = new MMatchAccountInfo;
	if( 0 == m_pAccountInfo ) return false;

	m_pAccountPenaltyInfo = new MMatchAccountPenaltyInfo;
	if( 0 == m_pAccountPenaltyInfo ) return false;

	return true;
}


MBMatchAsyncDBJob_GameOnLogin::~MBMatchAsyncDBJob_GameOnLogin()
{
	DeleteMemory();
}


void MBMatchAsyncDBJob_GameOnLogin::Run( void* pContext )
{
	
}


void MBMatchAsyncDBJob_GameOnLogin::DeleteMemory()
{
	if( NULL != m_pAccountInfo )
	{
		delete m_pAccountInfo;
		m_pAccountInfo = NULL;
	}

	if( NULL != m_pAccountPenaltyInfo )
	{
		delete m_pAccountPenaltyInfo;
		m_pAccountPenaltyInfo = NULL;
	}
}