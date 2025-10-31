#pragma once

#include "MAsyncDBJob.h"
#include "MMatchShop.h"


class MBMatchAsyncDBJob_BuyBountyItem : public MAsyncJob
{
public :
	MBMatchAsyncDBJob_BuyBountyItem(const MUID& uidOwner) : MAsyncJob( MASYNCJOB_BUYBOUNTRYITEM, uidOwner )
	{
		m_nCID				= 0;
		m_nAID				= 0;
		m_nItemID			= 0;
		m_nItemCount		= 0;
		m_dwPrice			= 0;
		m_wRentHourPeriod	= 0;
		m_dwCIID			= 0;
	}

	~MBMatchAsyncDBJob_BuyBountyItem()
	{
	}

	void Input( const MUID& uidPlaeyr 
		, const unsigned long nCID
		, const unsigned long nAID
		, const unsigned long nItemID
		, const int nItemCount
		, const DWORD dwPrice
		, const WORD wRehtHourPeriod
		, const bool bIsGambleItem
		, const bool bIsSpendableItem
		, MSHOPMODE nShopMode)
	{
		m_uidPlayer			= uidPlaeyr;
		m_nCID				= nCID;
		m_nAID				= nAID;
		m_nItemID			= nItemID;
		m_nItemCount		= nItemCount;
		m_dwPrice			= dwPrice;
		m_wRentHourPeriod	= wRehtHourPeriod;
		m_bIsGambleItem		= bIsGambleItem;
		m_bIsSpendableItem  = bIsSpendableItem;
		m_nShopMode			= nShopMode;
	}

	void Run( void* pContext );

	const MUID&			GetPlayerUID()		{ return m_uidPlayer; }
	const unsigned int	GetItemID()			{ return m_nItemID; }
	const int			GetItemCount()		{ return m_nItemCount; }
	const DWORD			GetPrice()			{ return m_dwPrice; }
	const WORD			GetRentHourPeriod()	{ return m_wRentHourPeriod; }
	const bool			IsGambleItem()		{ return m_bIsGambleItem; }
	const DWORD			GetNewCIID()		{ return m_dwCIID; }
	const MSHOPMODE		GetShopMode()		{ return m_nShopMode; }

private :
	MUID			m_uidPlayer;
	unsigned long	m_nCID;
	unsigned long	m_nAID;
	unsigned int	m_nItemID;
	int				m_nItemCount;
	DWORD			m_dwPrice;
	WORD			m_wRentHourPeriod;
	bool			m_bIsGambleItem;
	bool			m_bIsSpendableItem;

	MSHOPMODE		m_nShopMode;
	DWORD			m_dwCIID;
};