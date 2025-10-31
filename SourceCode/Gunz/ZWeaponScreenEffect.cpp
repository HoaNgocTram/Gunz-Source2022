#include "stdafx.h"
#include "ZWeaponScreenEffect.h"
#include "ZGame.h"
#include "ZMyInfo.h"
#include "ZMyItemList.h"
#include "ZConfiguration.h"
#include "ZInput.h"

//int g_nOriginalSensitivity = 0;

ZWeaponScreenEffect::ZWeaponScreenEffect()
{
	m_nWeaponType = MWT_NONE;
	m_pSniferPanel = NULL;
	m_pSniferWidescreenPanel = NULL;
}

ZWeaponScreenEffect::~ZWeaponScreenEffect()
{

}

void ZWeaponScreenEffect::Draw(MDrawContext* pDC)
{
	//return;

	switch (m_nWeaponType)
	{
	case MWT_SNIFER:
		{
			// Resolution change fix
			MBitmap* pBitmapSelection = (RGetIsWidthScreen() || RGetIsLongScreen()) ? m_pSniferWidescreenPanel : m_pSniferPanel;

			if (pBitmapSelection)
			{
				RGetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
				RGetDevice()->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
				RGetDevice()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
				pDC->SetOpacity(191); // 75 percent of 0xFF (255)
				pDC->SetBitmap(pBitmapSelection);
				pDC->Draw(0, 0, RGetScreenWidth(), RGetScreenHeight());
				pDC->SetOpacity(255); // Reset opacity
				pDC->SetBitmap(NULL); // Reset bitmap
				RGetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
			}
		}
		break;
	}
}

void ZWeaponScreenEffect::InitWeapon(MMatchWeaponType nWeaponType)
{
	//return;

	switch (nWeaponType)
	{
	case MWT_SNIFER:
		{
			// Custom: Original zoomed scope was 12.0f FOV
			g_fFOV = (20.0f / 180.0f * pi);
		}
		break;
	}

}

void ZWeaponScreenEffect::FinalWeapon(MMatchWeaponType nWeaponType)
{
	//return;

	switch (nWeaponType)
	{
	case MWT_SNIFER:
		{
			g_fFOV = (70.0f / 180.0f * pi);
		}
		break;
	}
}

void ZWeaponScreenEffect::OnGadget(MMatchWeaponType nWeaponType)
{
	//return;

	if (m_nWeaponType == nWeaponType) return;
	if (m_nWeaponType != MWT_NONE) OnGadgetOff();

	if( !ZGetGameInterface()->GetCombatInterface()->GetObserverMode() )
	{
		ZCharaterStatusBitPacking& uStatus = ZGetGame()->m_pMyCharacter->m_dwStatusBitPackingValue.Ref();
		uStatus.m_bSniping = true;

		if (!ZGetGame()->IsReplay())
		{
			ZPostSnifer( true );
			ZGetInput()->m_bSnifer = true;
		}

		// Code moved to ZInput::Update to prevent messing up the config
		//g_nOriginalSensitivity = ZGetConfiguration()->GetMouseSensitivityInInt();

		//int nSensitivity = (int)((float)0.5f * (float)g_nOriginalSensitivity);
        //int nNewSensitivity = ZGetConfiguration()->SetMouseSensitivityInInt(nSensitivity);
	}

	InitWeapon(nWeaponType);
	m_nWeaponType = nWeaponType;
}

void ZWeaponScreenEffect::OnGadgetOff()
{
	//return;

	FinalWeapon(m_nWeaponType);
	m_nWeaponType = MWT_NONE;

	if( !ZGetGameInterface()->GetCombatInterface()->GetObserverMode() )
	{
		ZCharaterStatusBitPacking& uStatus = ZGetGame()->m_pMyCharacter->m_dwStatusBitPackingValue.Ref();

		if (uStatus.m_bSniping)
		{
			uStatus.m_bSniping = false;

			if (!ZGetGame()->IsReplay())
			{
				ZPostSnifer( false );
				ZGetInput()->m_bSnifer = false;
			}
		}

		// Code moved to ZInput::Update to prevent messing up the config
		//if( g_nOriginalSensitivity > 0 )
		//	ZGetConfiguration()->SetMouseSensitivityInInt(g_nOriginalSensitivity);
	}
}

bool ZWeaponScreenEffect::IsScopeEnabled()
{
	return m_nWeaponType != MWT_NONE;
}

void ZWeaponScreenEffect::Create()
{
	//return;

	MMatchWeaponType nPrimaryWeaponType = MWT_NONE, 
					 nSecondaryWeaponType = MWT_NONE, 
					 nMeleeWeaponType = MWT_NONE;

	unsigned long int nItemID;
	nItemID = ZGetMyInfo()->GetItemList()->GetEquipedItemID(MMCIP_PRIMARY);
	MMatchItemDesc* pItemDesc = NULL;
	pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);
	if (pItemDesc)
	{
		nPrimaryWeaponType = pItemDesc->m_nWeaponType.Ref();
	}

	nItemID = ZGetMyInfo()->GetItemList()->GetEquipedItemID(MMCIP_SECONDARY);
	
	pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);
	if (pItemDesc)
	{
		nSecondaryWeaponType = pItemDesc->m_nWeaponType.Ref();
	}
	nItemID = ZGetMyInfo()->GetItemList()->GetEquipedItemID(MMCIP_MELEE);
	
	pItemDesc = MGetMatchItemDescMgr()->GetItemDesc(nItemID);
	if (pItemDesc)
	{
		nMeleeWeaponType = pItemDesc->m_nWeaponType.Ref();
	}

	// Initialization fix (for spectators)
	//if ((nPrimaryWeaponType == MWT_SNIFER) || (nSecondaryWeaponType == MWT_SNIFER))
	//{
		if (m_pSniferPanel) delete m_pSniferPanel;
		if (m_pSniferWidescreenPanel) delete m_pSniferWidescreenPanel;
		m_pSniferPanel = new MBitmapR2;
		m_pSniferWidescreenPanel = new MBitmapR2;

		// Custom: Wide screen scope
		((MBitmapR2*)m_pSniferWidescreenPanel)->Create("snifer_ws.png", RGetDevice(), "interface/Default/Combat/snifer_ws.png");
		((MBitmapR2*)m_pSniferPanel)->Create("snifer.png", RGetDevice(), "interface/Default/Combat/snifer.png");
	//}
}

void ZWeaponScreenEffect::Destroy()
{
	return; //Custom: no se porque esto estaba desactivado pero lo volvi a reactivar porque crasheaba despues de una partida o retirarse.

	if (m_pSniferPanel)
	{
		delete m_pSniferPanel;
		delete m_pSniferWidescreenPanel;
		m_pSniferPanel = NULL;
		m_pSniferWidescreenPanel = NULL;
	}
}