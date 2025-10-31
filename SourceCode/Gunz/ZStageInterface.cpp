/***********************************************************************
  ZStageInterface.cpp
  
  øÅE µµ : Ω∫≈◊¿Ã¡ÅE¿Œ≈Õ∆‰¿ÃΩ∫∏¶ ∞ÅEÆ«œ¥¬ ≈¨∑°Ω∫. ƒ⁄µÅE∞ÅEÆªÛ¿« ∆˙‘Æ∏¶ ¿ß«ÿ
           ∫–∏Æ«ﬂ¿Ω(ªÁΩ« æ∆¡ÅEøœ¿ÅEÅE¥Ÿ ∫–∏Æ ∏¯«ﬂ¿Ω. -_-;).
  ¿€º∫¿œ : 11, MAR, 2004
  ¿€º∫¿⁄ : ¿”µø»Ø
************************************************************************/


#include "stdafx.h"							// Include stdafx.h
#include "ZStageInterface.h"				// Include ZStageInterface.h
#include "ZStageSetting.h"					// Include ZStageSetting.h
#include "ZGameInterface.h"
#include "ZPlayerListBox.h"
#include "ZCombatMenu.h"
#include "ZShopEquipListbox.h"
#include "ZMyItemList.h"
#include "ZItemSlotView.h"
#include "ZMessages.h"
#include "ZLanguageConf.h"
#include "MNewQuestScenario.h"
#include "MMatchSpyMap.h"

/* «ÿæﬂ«“ ∞ÕµÅE..

 1. ZStageSetting ∞ÅE√ ∑Á∆æ¿ª ø©±‚∑Œ ø≈∞‹øÕæﬂ «œ¥¬µÅE..  -_-;
 2. πˆ∆∞ UI¬ µµ ø™Ω√ ø©±‚∑Œ ø≈∞‹øÕæﬂ «œ¥¬µ• ¡π∂ÅE≤øø©¿÷¥Ÿ...  -_-;
*/
/***********************************************************************
  ZStageInterface : public
  
  desc : ª˝º∫¿⁄
************************************************************************/
ZStageInterface::ZStageInterface( void)
{
	m_bPrevQuest = false;
	m_bDrawStartMovieOfQuest = false;
	m_pTopBgImg = NULL;
	m_pStageFrameImg = NULL;
	m_pItemListFrameImg = NULL;
	m_nListFramePos = 0;
	m_nStateSacrificeItemBox = 0;
	m_pRelayMapListFrameImg = NULL;
	m_nRelayMapListFramePos = 0;
	m_bRelayMapRegisterComplete = true;
	m_bEnableWidgetByRelayMap = true;
	m_pChallengeQuestScenario = new MNewQuestScenarioManager;

	m_pSpyBanMapListFrameImg = NULL;
	m_nSpyBanMapListFramePos = 0;

}


/***********************************************************************
  ~ZStageInterface : public
  
  desc : º“∏ÅE⁄
************************************************************************/
ZStageInterface::~ZStageInterface( void)
{
	if ( m_pTopBgImg != NULL)
	{
		delete m_pTopBgImg;
		m_pTopBgImg = NULL;
	}

	if ( m_pStageFrameImg != NULL)
	{
		delete m_pStageFrameImg;
		m_pStageFrameImg = NULL;
	}

	delete m_pChallengeQuestScenario;
}


/***********************************************************************
  Create : public
  
  desc : √ ±‚»≠
************************************************************************/
void ZStageInterface::OnCreate( void)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	// √ ±‚»≠ «ÿ¡÷∞ÅE
	m_bPrevQuest = false;
	m_bDrawStartMovieOfQuest = false;
	m_nStateSacrificeItemBox = 0;		// Hide
	m_nGameType = MMATCH_GAMETYPE_DEATHMATCH_SOLO;
	m_SacrificeItem[ SACRIFICEITEM_SLOT0].RemoveItem();
	m_SacrificeItem[ SACRIFICEITEM_SLOT1].RemoveItem();

	ReadSenarioNameXML();

	_ASSERT(m_pChallengeQuestScenario);
	m_pChallengeQuestScenario->ReadXml(ZApplication::GetFileSystem(), "system/scenario2.xml", ZGetQuest()->GetDropTable());

	MPicture* pPicture = (MPicture*)pResource->FindWidget( "Stage_SacrificeItemImage0");
	if ( pPicture)
		pPicture->SetOpacity( 255);
	pPicture = (MPicture*)pResource->FindWidget( "Stage_SacrificeItemImage1");
	if ( pPicture)
		pPicture->SetOpacity( 255);

	pPicture = (MPicture*)pResource->FindWidget( "Stage_MainBGTop");
	if ( pPicture)
		pPicture->SetBitmap( MBitmapManager::Get( "main_bg_t.png"));
	pPicture = (MPicture*)pResource->FindWidget( "Stage_FrameBG");
	if ( pPicture)
	{
		m_pStageFrameImg = new MBitmapR2;
		((MBitmapR2*)m_pStageFrameImg)->Create( "stage_frame.png", RGetDevice(), "interface/loadable/stage_frame.png");

		if ( m_pStageFrameImg != NULL)
			pPicture->SetBitmap( m_pStageFrameImg->GetSourceBitmap());
	}
	pPicture = (MPicture*)pResource->FindWidget( "Stage_ItemListBG");
	if ( pPicture)
	{
		m_pItemListFrameImg = new MBitmapR2;
#ifdef _SYSINTERNEW
		((MBitmapR2*)m_pItemListFrameImg)->Create( "itemlistframe.png", RGetDevice(), "interface/loadable/itemlistframe.png");
#else
		((MBitmapR2*)m_pItemListFrameImg)->Create("itemlistframe.tga", RGetDevice(), "interface/loadable/itemlistframe.tga");
#endif

		if ( m_pItemListFrameImg != NULL)
			pPicture->SetBitmap( m_pItemListFrameImg->GetSourceBitmap());
	}
	MWidget* pWidget = (MWidget*)pResource->FindWidget( "Stage_ItemListView");
	if ( pWidget)
	{
		MRECT rect;
		rect = pWidget->GetRect();
		rect.x = -rect.w;
		m_nListFramePos = rect.x;
		pWidget->SetBounds( rect);
	}
	MLabel* pLabel = (MLabel*)pResource->FindWidget( "Stage_SenarioName");
	if ( pLabel)
		pLabel->SetText( "");
	pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Stage_SenarioNameImg");
	if ( pWidget)
		pWidget->Show( false);
	MListBox* pListBox = (MListBox*)pResource->FindWidget( "Stage_SacrificeItemListbox");
	if ( pListBox)
		pListBox->RemoveAll();
	MTextArea* pDesc = (MTextArea*)pResource->FindWidget( "Stage_ItemDesc");
	if ( pDesc)
	{
		pDesc->SetTextColor( MCOLOR(0xFF808080));
		//pDesc->SetText( "æ∆¿Ã≈€¿ª »≠∏ÅE¡ﬂæ”ø° ¿÷¥¬ µŒ∞≥¿« ¡¶¥‹ø° ≤¯æ˚œı¿Ω¿∏∑ŒΩÅE∞‘¿” ∑π∫ß¿ª ¡∂¡§«“ ºÅE¿÷Ω¿¥œ¥Ÿ.");
		char szText[256];
		sprintf(szText, ZMsg( MSG_QUESTITEM_USE_DESCRIPTION ));
		pDesc->SetText(szText);
	}

	{ // ∏±∑π¿Ã∏  "RelayMap" √ ±‚»≠
		pPicture = (MPicture*)pResource->FindWidget( "Stage_RelayMapListBG" );
		if(pPicture)
		{ // f∏±∑π¿Ã∏  ∏ÆΩ∫∆Æ πË∞ÅE¿ÃπÃ¡ÅE
			m_pRelayMapListFrameImg = new MBitmapR2;
#ifdef _SYSINTERNEW
			((MBitmapR2*)m_pRelayMapListFrameImg)->Create( "relaymaplistframe.png", RGetDevice(), "interface/loadable/relaymaplistframe.png");
#else
			((MBitmapR2*)m_pRelayMapListFrameImg)->Create("relaymaplistframe.tga", RGetDevice(), "interface/loadable/relaymaplistframe.tga");
#endif

			if(m_pRelayMapListFrameImg != NULL)
				pPicture->SetBitmap(m_pRelayMapListFrameImg->GetSourceBitmap());
		}
		MWidget* pWidget = (MWidget*)pResource->FindWidget( "Stage_RelayMapListView");
		if ( pWidget)
		{
			MRECT rect;
			rect = pWidget->GetRect();
			rect.x = -rect.w;
			m_nRelayMapListFramePos = rect.x;
			pWidget->SetBounds( rect);
		}

		MComboBox* pCombo = (MComboBox*)pResource->FindWidget("Stage_RelayMapType");			
		if ( pCombo)
			pCombo->CloseComboBoxList();
		pCombo = (MComboBox*)pResource->FindWidget("Stage_RelayMapRepeatCount");					
		if ( pCombo)
			pCombo->CloseComboBoxList();

		MListBox* pListBox = (MListBox*)pResource->FindWidget( "Stage_RelayMapListbox");
		if ( pListBox)
			pListBox->RemoveAll();
		pListBox = (MListBox*)pResource->FindWidget( "Stage_MapListbox");
		if ( pListBox)
			pListBox->RemoveAll();

	}

	{ // SPY mode : ban map list.
		pPicture = (MPicture*)pResource->FindWidget("Stage_SpyBanMapListBG");
		if (pPicture)
		{ // f∏±∑π¿Ã∏  ∏ÆΩ∫∆Æ πË∞ÅE¿ÃπÃ¡ÅE
			m_pSpyBanMapListFrameImg = new MBitmapR2;
#ifdef _SYSINTERNEW
			((MBitmapR2*)m_pSpyBanMapListFrameImg)->Create("relaymaplistframe.png", RGetDevice(), "interface/loadable/relaymaplistframe.png");
#else
			((MBitmapR2*)m_pSpyBanMapListFrameImg)->Create("relaymaplistframe.tga", RGetDevice(), "interface/loadable/relaymaplistframe.tga");
#endif

			if (m_pSpyBanMapListFrameImg != NULL)
				pPicture->SetBitmap(m_pSpyBanMapListFrameImg->GetSourceBitmap());
		}
		MWidget* pWidget = (MWidget*)pResource->FindWidget("Stage_SpyBanMapListView");
		if (pWidget)
		{
			MRECT rect;
			rect = pWidget->GetRect();
			rect.x = -rect.w;
			m_nSpyBanMapListFramePos = rect.x;
			pWidget->SetBounds(rect);
		}

		MListBox* pListBox = (MListBox*)pResource->FindWidget("Stage_SpyBanMapList");
		if (pListBox)
			pListBox->RemoveAll();

		pLabel = (MLabel*)pResource->FindWidget("Stage_SpyRandomMapLabel");
		if (pLabel)
			pLabel->Show(false);

		MComboBox* pComboBox = (MComboBox*)pResource->FindWidget("MapSelection");
		if (pComboBox)
			pComboBox->Show(true);
	}

	ZApplication::GetGameInterface()->ShowWidget("Stage_Flame0", false);
	ZApplication::GetGameInterface()->ShowWidget("Stage_Flame1", false);

	// ∞‘¿” πÊΩƒ¿Œ √§∑Œ πÊ ≥™∞¨¥Ÿ∞° ¥Ÿ∏• πÊ µÈæÓ∞°∏È πˆ±◊ ª˝±‚¥¬∞≈ ºˆ¡§ 
	MComboBox* pCombo = (MComboBox*)pResource->FindWidget("StageType");
	if (pCombo)
		pCombo->CloseComboBoxList();

	// ∏ ∏ÆΩ∫∆Æ ø¨ √§∑Œ πÊ ≥™∞¨¥Ÿ∞° ¥Ÿ∏• πÊ µÈæÓ∞°∏È πÊ¿Â¿Ã æ∆¥—µ•µµ ∏  πŸ≤„¡ˆ¥¬ πˆ±◊ ºˆ¡§
	pCombo = (MComboBox*)pResource->FindWidget("MapSelection");
	if (pCombo)
		pCombo->CloseComboBoxList();


	// √§≥Œ ∏ÆΩ∫∆Æ π⁄Ω∫¥¬ ¥›æ∆πˆ∏≤
	pWidget = (MWidget*)pResource->FindWidget("ChannelListFrame");
	if (pWidget)
		pWidget->Show(false);


	// »≠∏È æ˜µ• «—π¯ «ÿ¡÷ªÔ~
	UpdateSacrificeItem();
	SerializeSacrificeItemListBox();

	// QL √ ±‚»≠
	OnResponseQL(0);


	ZApplication::GetGameInterface()->ShowWidget("Stage_Lights0", false);
	ZApplication::GetGameInterface()->ShowWidget("Stage_Lights1", false);
}


/***********************************************************************
  OnDestroy : public
  
  desc : º≠πˆ≥™ »§¿∫ Ω√Ω∫≈€¿« ø‰√ª¿∏∑Œ∫Œ≈Õ Ω∫≈◊¿Ã¡ÅE»≠∏È¿ª ªı∑Œ ∞ªΩ≈«œ¥¬ «‘ºÅE
  arg  : none
  ret  : none
************************************************************************/
void ZStageInterface::OnDestroy( void)
{
	ZApplication::GetGameInterface()->ShowWidget( "Stage", false);

	MPicture* pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Stage_MainBGTop");
	if ( pPicture)
		pPicture->SetBitmap( MBitmapManager::Get( "main_bg_t.png"));
	pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Stage_FrameBG");
	if ( pPicture)
		pPicture->SetBitmap( NULL);
	pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Stage_ItemListBG");
	if ( pPicture)
		pPicture->SetBitmap( NULL);

	pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Stage_RelayMapListBG");
	if ( pPicture)
		pPicture->SetBitmap( NULL);

	pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Stage_SpyBanMapListBG");
	if (pPicture)
		pPicture->SetBitmap(NULL);

	if ( m_pTopBgImg != NULL)
	{
		delete m_pTopBgImg;
		m_pTopBgImg = NULL;
	}
	if ( m_pStageFrameImg != NULL)
	{
		delete m_pStageFrameImg;
		m_pStageFrameImg = NULL;
	}
	if ( m_pItemListFrameImg != NULL)
	{
		delete m_pItemListFrameImg;
		m_pItemListFrameImg = NULL;
	}
	if (m_pRelayMapListFrameImg != NULL)
	{
		delete m_pRelayMapListFrameImg;
		m_pRelayMapListFrameImg = NULL;
	}
	if (m_pSpyBanMapListFrameImg != NULL)
	{
		delete m_pSpyBanMapListFrameImg;
		m_pSpyBanMapListFrameImg = NULL;
	}
	MWidget* pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Stage_CharacterInfo");
	if ( pWidget)
		pWidget->Enable( true);

//	m_SenarioDesc.clear();
}

void ZStageInterface::OnStageCharListSettup()
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	for ( MStageCharSettingList::iterator itor = ZGetGameClient()->GetMatchStageSetting()->m_CharSettingList.begin();
		itor != ZGetGameClient()->GetMatchStageSetting()->m_CharSettingList.end();  ++itor) 
	{
		MSTAGE_CHAR_SETTING_NODE* pCharNode = (*itor);

		ZPlayerListBox* pItem = (ZPlayerListBox*)pResource->FindWidget( "StagePlayerList_");
		if ( pItem)
		{
			bool bMaster = false;

			if ( ZGetGameClient()->GetMatchStageSetting()->GetMasterUID() == pCharNode->uidChar)
				bMaster = true;

			pItem->UpdatePlayer( pCharNode->uidChar,(MMatchObjectStageState)pCharNode->nState,bMaster,MMatchTeam(pCharNode->nTeam));
		}
	}
}


/***********************************************************************
  OnStageInterfaceSettup : public
  
  desc : º≠πˆ≥™ »§¿∫ Ω√Ω∫≈€¿« ø‰√ª¿∏∑Œ∫Œ≈Õ Ω∫≈◊¿Ã¡ÅE»≠∏È¿ª ªı∑Œ ∞ªΩ≈«œ¥¬ «‘ºÅE
  arg  : none
  ret  : none
************************************************************************/
void ZStageInterface::OnStageInterfaceSettup( void)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	ZStageSetting::InitStageSettingGameType();
/*
	// ∏  ¡æ∑˘∏¶ º≥¡§«—¥Ÿ.
	MComboBox* pCB = (MComboBox*)pResource->FindWidget( "MapSelection");
	if ( pCB)
	{
		int nSelected = pCB->GetSelIndex();

		InitMaps( pCB);

		if ( nSelected >= pCB->GetCount())
			nSelected = pCB->GetCount() - 1;

		pCB->SetSelIndex( nSelected);
	}
*/

	// CharListView¿« Add, Remove, Update¥¬ ZGameClient::OnObjectCache ø°º≠ ∞ÅEÆ«—¥Ÿ.
	MSTAGE_CHAR_SETTING_NODE* pMyCharNode = NULL;
	bool bMyReady = false;		// Ready ªÛ≈¬¿Œ¡ÅEæ∆¥—¡ÅE..
	for ( MStageCharSettingList::iterator itor = ZGetGameClient()->GetMatchStageSetting()->m_CharSettingList.begin();
		itor != ZGetGameClient()->GetMatchStageSetting()->m_CharSettingList.end();  ++itor) 
	{
		MSTAGE_CHAR_SETTING_NODE* pCharNode = (*itor);

		// ≥™ ¿⁄Ω≈¿œ ∞ÊøÅE
		if ( pCharNode->uidChar == ZGetGameClient()->GetPlayerUID()) 
		{
			pMyCharNode = pCharNode;
			if (pMyCharNode->nState == MOSS_READY)
				bMyReady = true;
			else
				bMyReady = false;
		}

		ZPlayerListBox* pItem = (ZPlayerListBox*)pResource->FindWidget( "StagePlayerList_");
		if ( pItem)
		{
			bool bMaster = false;

			if ( ZGetGameClient()->GetMatchStageSetting()->GetMasterUID() == pCharNode->uidChar)
				bMaster = true;
			
			pItem->UpdatePlayer( pCharNode->uidChar,(MMatchObjectStageState)pCharNode->nState,bMaster,MMatchTeam(pCharNode->nTeam));
		}
	}

	// Ω∫≈◊¿Ã¡ˆ¿« πˆ∆∞ ªÛ≈¬(∞‘¿”Ω√¿€, ≥≠¿‘, ¡ÿ∫Òøœ∑ÅE∏¶ º≥¡§«—¥Ÿ.
	ChangeStageButtons( ZGetGameClient()->IsForcedEntry(), ZGetGameClient()->AmIStageMaster(), bMyReady);

	// Ω∫≈◊¿Ã¡ˆ¿«...
	ChangeStageGameSetting( ZGetGameClient()->GetMatchStageSetting()->GetStageSetting());
	
	// ≥≠¿‘ ∏‚πˆ¿œ ∞ÊøÅE°...
	if ( !ZGetGameClient()->AmIStageMaster() && ( ZGetGameClient()->IsForcedEntry()))
	{
		if ( pMyCharNode != NULL)
			ChangeStageEnableReady( bMyReady);
	}

	// ∏∏æÅE≥≠¿‘¿∏∑Œ µÈæ˚€‘¥¬µ• ¥Ÿ∏• ªÁ∂ÅE¥Ÿ ≥™∞°º≠ ≥ª∞° πÊ¿Â¿Ã µ«æ˙¥Ÿ∏ÅE≥≠¿‘∏µÅE«ÿ¡¶
	if ( (ZGetGameClient()->AmIStageMaster() == true) && ( ZGetGameClient()->IsForcedEntry()))
	{
		if ( ZGetGameClient()->GetMatchStageSetting()->GetStageState() == STAGE_STATE_STANDBY)
		{
			ZGetGameClient()->ReleaseForcedEntry();

			// ¿Œ≈Õ∆‰¿ÃΩ∫∞ÅE√
			if(ZGetGameClient()->GetMatchStageSetting()->GetGameType() != MMATCH_GAMETYPE_SPY)
			ZApplication::GetGameInterface()->EnableWidget("StageSettingCaller", true); 
			ZApplication::GetGameInterface()->EnableWidget( "MapSelection", true);			
			ZApplication::GetGameInterface()->EnableWidget( "StageType", true);				
			ZApplication::GetGameInterface()->EnableWidget( "StageMaxPlayer", true);		
			ZApplication::GetGameInterface()->EnableWidget( "StageRoundCount", true);
#ifdef _BOXLEAD
			ZApplication::GetGameInterface()->EnableWidget("Lead", true);
#endif

		}
		else	
		{
			
			ZApplication::GetGameInterface()->EnableWidget( "StageSettingCaller", false);	
			ZApplication::GetGameInterface()->EnableWidget( "MapSelection", false);			
			ZApplication::GetGameInterface()->EnableWidget( "StageType", false);			
			ZApplication::GetGameInterface()->EnableWidget( "StageMaxPlayer", false);		
			ZApplication::GetGameInterface()->EnableWidget( "StageRoundCount", false);	
#ifdef _BOXLEAD
			ZApplication::GetGameInterface()->EnableWidget("Lead", false);
#endif
		}
	}
	/// ∏±∑π¿Ã∏ ¿Ã∏Èº≠ ¥Ÿ¿Ω∏ ¿∏∑Œ ∞Ëº” ¡¯«‡¡ﬂ¿œ∂ß Ω√¿€ πˆ∆∞¿ª ∏µŒ OFF Ω√ƒ—¡ÿ¥Ÿ.
	if(	ZGetGameClient()->AmIStageMaster() && 
		!m_bEnableWidgetByRelayMap && 
		ZGetGameClient()->GetMatchStageSetting()->GetStageSetting()->bIsRelayMap)
	{
		ZApplication::GetGameInterface()->EnableWidget( "GameStart", false);
		// ZApplication::GetGameInterface()->EnableWidget( "StageReady", false);	// ¡ÿ∫Òøœ∑·πˆ∆∞(±‚»πø° µ˚∂ÅE∫Ø∞Ê∞°¥…)

		// ¿Œ≈Õ∆‰¿ÃΩ∫∞ÅE√
		ZApplication::GetGameInterface()->EnableWidget( "StageSettingCaller",	false);		// πÊº≥¡§ πˆ∆∞
		ZApplication::GetGameInterface()->EnableWidget( "MapSelection",			false);		// ∏ º±≈√ ƒﬁ∫∏π⁄Ω∫
		ZApplication::GetGameInterface()->EnableWidget( "StageType",			false);		// ∞‘¿”πÊΩƒ ƒﬁ∫∏π⁄Ω∫
		ZApplication::GetGameInterface()->EnableWidget( "StageMaxPlayer",		false);		// √÷¥ÅEŒøÅEƒﬁ∫∏π⁄Ω∫
		ZApplication::GetGameInterface()->EnableWidget( "StageRoundCount",		false);		// ∞Ê±‚»ΩºÅEƒﬁ∫∏π⁄Ω∫
#ifdef _BOXLEAD
		ZApplication::GetGameInterface()->EnableWidget("Lead", false);
#endif

		ZApplication::GetGameInterface()->EnableWidget( "StageSettingCaller",		false);
		ZApplication::GetGameInterface()->EnableWidget( "Stage_RelayMap_OK_Button", false);
		ZApplication::GetGameInterface()->EnableWidget( "Stage_RelayMapType",		false);
		ZApplication::GetGameInterface()->EnableWidget( "Stage_RelayMapRepeatCount", false);
	}

	MPicture* pPicture = 0;
	MBitmap* pBitmap = 0;
	// »≠∏ÅEªÛ¥‹¿« ∏  ¿ÃπÃ¡ÅEº≥¡§«œ±ÅE
	if (ZGetGameClient()->GetMatchStageSetting()->GetGameType() != MMATCH_GAMETYPE_QUEST_CHALLENGE)
	{
		char szMapName[256];
		pPicture = (MPicture*)pResource->FindWidget("Stage_MainBGTop");
		if (pPicture)
		{
			if (0 == strcmp(MMATCH_MAPNAME_RELAYMAP, ZGetGameClient()->GetMatchStageSetting()->GetMapName()))
			{
				sprintf(szMapName, "interface/MapGB/%s", MGetMapDescMgr()->GetMapImageName(MMATCH_DEFAULT_STAGESETTING_MAPNAME));
			}
			else
			{
				sprintf(szMapName, "interface/MapGB/%s", MGetMapDescMgr()->GetMapImageName(ZGetGameClient()->GetMatchStageSetting()->GetMapName()));
			}
			if (m_pTopBgImg != NULL)
			{
				delete m_pTopBgImg;
				m_pTopBgImg = NULL;
			}

			m_pTopBgImg = new MBitmapR2;
			((MBitmapR2*)m_pTopBgImg)->Create("TopBgImg.png", RGetDevice(), szMapName);

			if (m_pTopBgImg != NULL)
				pPicture->SetBitmap(m_pTopBgImg->GetSourceBitmap());
		}
	}
	
	// ¡§∫∏√¢ø° ∞‘¿”πÊ¡¶ º≥¡§«œ±ÅE
	MLabel* pLabel = (MLabel*)pResource->FindWidget( "StageNameLabel");
	if ( pLabel != 0)
	{
		char szStr[ 256];
		// Custom: remove servername from titlebar
		sprintf( szStr, "%s > %03d:%s", ZMsg( MSG_WORD_STAGE), ZGetGameClient()->GetStageNumber(), ZGetGameClient()->GetStageName());
		pLabel->SetText( szStr);
	}

	// ªÛ«œ¥‹ Ω∫∆Æ∂Û¿Ã«¡¿« ªˆªÅEπŸ≤Ÿ±ÅE
#define SDM_COLOR			MCOLOR(255,0,0)
#define TDM_COLOR			MCOLOR(0,255,0)
#define SGD_COLOR			MCOLOR(0,0,255)
#define TGD_COLOR			MCOLOR(255,255,0)
#define ASSASIN_COLOR		MCOLOR(255,0,255)
#define TRAINING_COLOR		MCOLOR(0,255,255)
#define QUEST_COLOR			MCOLOR(255,255,255)
#define SURVIVAL_COLOR		MCOLOR(255,255,255)

	MCOLOR color;
	switch ( ZGetGameClient()->GetMatchStageSetting()->GetGameType() )
	{	
		case MMATCH_GAMETYPE_ASSASSINATE:
			color = ASSASIN_COLOR;
			break;

		case MMATCH_GAMETYPE_DEATHMATCH_SOLO:
			color = SDM_COLOR;
			break;

		case MMATCH_GAMETYPE_DEATHMATCH_TEAM:
		case MMATCH_GAMETYPE_DEATHMATCH_TEAM2:
		case MMATCH_GAMETYPE_CTF:
		case MMATCH_GAMETYPE_BLITZKRIEG:
		case MMATCH_GAMETYPE_INFECTED:
			color = TDM_COLOR;
			break;

		case MMATCH_GAMETYPE_GLADIATOR_SOLO:
		case MMATCH_GAMETYPE_GUNGAME:
			color = SGD_COLOR;
			break;

		case MMATCH_GAMETYPE_GLADIATOR_TEAM:
			color = TGD_COLOR;
			break;

		case MMATCH_GAMETYPE_TRAINING:
		case MMATCH_GAMETYPE_TEAM_TRAINING:
			color = TRAINING_COLOR;
			break;

#ifdef _QUEST
		case MMATCH_GAMETYPE_SURVIVAL:
			color = QUEST_COLOR;
			break;

		case MMATCH_GAMETYPE_QUEST:
			color = SURVIVAL_COLOR;
			break;
#endif
		case MMATCH_GAMETYPE_QUEST_CHALLENGE:
			color = QUEST_COLOR; //todok ƒ˘Ω∫∆Æ√ß∏∞¡ÅEªˆ±ÅE
			break;

		case MMATCH_GAMETYPE_BERSERKER:
			color = SDM_COLOR;
			break;

		case MMATCH_GAMETYPE_DUEL:
			color = SDM_COLOR;
			break;
		case MMATCH_GAMETYPE_DUELTOURNAMENT:
			color = SDM_COLOR;
			break;
		case MMATCH_GAMETYPE_TURBO:
			color = SDM_COLOR;
			break;
		case MMATCH_GAMETYPE_SKILLMAP:
			color = SDM_COLOR;
			break;
		case MMATCH_GAMETYPE_SPY:
			color = TDM_COLOR;
			break;

		default:
			_ASSERT(0);
			color = MCOLOR(255,255,255,255);
	}
	pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Stage_StripBottom");
	if(pPicture != NULL && !pPicture->IsAnim())
	{		
        pPicture->SetBitmapColor( color );
		if(!(pPicture->GetBitmapColor().GetARGB() == pPicture->GetReservedBitmapColor().GetARGB()))
			pPicture->SetAnimation( 2, 700.0f);
	}
	pPicture = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Stage_StripTop");
	if(pPicture != NULL && !pPicture->IsAnim())	
	{
		pPicture->SetBitmapColor( color );
		if(!(pPicture->GetBitmapColor().GetARGB() == pPicture->GetReservedBitmapColor().GetARGB()))
			pPicture->SetAnimation( 3, 700.0f);		
	}
}

void ZStageInterface::SetStageRelayMapImage()
{
	MPicture* pPicture = 0;
	MBitmap* pBitmap = 0;
	char szMapName[256];
	pPicture = (MPicture*)ZGetGameInterface()->GetIDLResource()->FindWidget( "Stage_MainBGTop");
	if(!pPicture) return;
	MListBox* pRelayMapListBox = (MListBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("Stage_RelayMapListbox");
	if(pRelayMapListBox == NULL) return;
	if( 0 < pRelayMapListBox->GetCount())
	{
		sprintf( szMapName, "interface/MapGB/%s", MGetMapDescMgr()->GetMapImageName( pRelayMapListBox->GetString(pRelayMapListBox->GetStartItem())));
		if ( m_pTopBgImg != NULL)
		{
			delete m_pTopBgImg;
			m_pTopBgImg = NULL;
		}

		m_pTopBgImg = new MBitmapR2;
		((MBitmapR2*)m_pTopBgImg)->Create( "TopBgImg.png", RGetDevice(), szMapName);

		if ( m_pTopBgImg != NULL)
			pPicture->SetBitmap( m_pTopBgImg->GetSourceBitmap());
	}
}

void ZStageInterface::SetEnableWidgetByRelayMap(bool b)
{
	if(!ZGetGameClient()->GetMatchStageSetting()->GetStageSetting()->bIsRelayMap)
	{	// ∏±∑π¿Ã∏ ¿Ã æ∆¥œ∏ÅE∏±∑π¿Ã∏ øÅE¿ß¡¨ ∫Ò»∞º∫»≠ √≥∏Æ∏¶ «“ « øÅEæ¯¥Ÿ.
		m_bEnableWidgetByRelayMap = true;
		return;
	}

	m_bEnableWidgetByRelayMap = b;
}

/***********************************************************************
  ChangeStageGameSetting : public
  
  desc : ¿Ã∞Õµµ ∞‘¿” ∞ÅE√ ¿Œ≈Õ∆‰¿ÃΩ∫∏¶ ºˆ¡§«œ¥¬∞≈ ∞∞¿∫µ•... ø÷¿Ã∑∏∞‘ ∏π¿Ã ≥™¥≤≥ı¿∫∞≈¡ÅE -_-;
         ¡÷∑Œ »≠∏È¿« ¿ÅEº¿˚¿Œ UI∏¶ º≥¡§«—¥Ÿ.
  arg  : pSetting = Ω∫≈◊¿Ã¡ÅEº≥¡§ ¡§∫∏
  ret  : none
************************************************************************/
void ZStageInterface::ChangeStageGameSetting( const MSTAGE_SETTING_NODE* pSetting)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	m_nGameType = pSetting->nGameType;
	SetMapName(pSetting->szMapName);
	ZApplication::GetGameInterface()->m_bTeamPlay = ZGetGameTypeManager()->IsTeamGame( pSetting->nGameType);


	// ∞ÅEÅE«„øÅEø©∫Œ »Æ¿Œ
	MComboBox* pCombo = (MComboBox*)pResource->FindWidget( "StageObserver");
	MButton* pObserverBtn = (MButton*)pResource->FindWidget( "StageObserverBtn");
	MLabel* pObserverLabel = (MLabel*)pResource->FindWidget( "StageObserverLabel");
	if ( pCombo && pObserverBtn && pObserverLabel)
	{
		if ( pCombo->GetSelIndex() == 1)
		{
			pObserverBtn->SetCheck( false);
			pObserverBtn->Enable( false);
			pObserverLabel->Enable( false);
		}
		else
		{
			pObserverBtn->Enable( true);
			pObserverLabel->Enable( true);
		}
	}

	// √ª∆¿, »´∆¿ ªÛ≈¬ º≥¡§
	ZApplication::GetGameInterface()->UpdateBlueRedTeam();

	// ∞‘¿” πÊΩƒø° µ˚∂Ûº≠ UI∏¶ ∫Ø∞Ê«—¥Ÿ
	MAnimation* pAniMapImg = (MAnimation*)pResource->FindWidget( "Stage_MapNameBG");
	bool bQuestUI = false;
	bool bChallengeQuestUI = false;
	bool bSpyUI = false;
	if ( (pSetting->nGameType == MMATCH_GAMETYPE_DEATHMATCH_SOLO) ||			// µ•æ≤∏≈ƒ° ∞≥¿Œ¿ÅEÃ∞≈≥™...
		 (pSetting->nGameType == MMATCH_GAMETYPE_GLADIATOR_SOLO) ||				// ƒÆ¿ÅE∞≥¿Œ¿ÅEÃ∞≈≥™...
		 (pSetting->nGameType == MMATCH_GAMETYPE_BERSERKER) ||					// πˆº≠ƒø∏µÂ¿Ã∞≈≥™...
		 (pSetting->nGameType == MMATCH_GAMETYPE_TURBO) ||
		 (pSetting->nGameType == MMATCH_GAMETYPE_SKILLMAP) ||
		 (pSetting->nGameType == MMATCH_GAMETYPE_TRAINING) ||					// ∆Æ∑π¿Ã¥◊¿Ã∞≈≥™...
		 (pSetting->nGameType == MMATCH_GAMETYPE_DUEL))							// µ‡æÛ∏µÅE¿Ã∏ÅE..
	{
		// ∏  ¿Ã∏ß πË∞ÅE¿ÃπÃ¡ÅE∫Ø»Ø
		if ( pAniMapImg)
			pAniMapImg->SetCurrentFrame(0);

		// ƒ˘Ω∫∆Æ UI ∞®√ÅE
		bQuestUI = false;
		bChallengeQuestUI = false;
	}
	else if ( (pSetting->nGameType == MMATCH_GAMETYPE_DEATHMATCH_TEAM) ||		// µ•æ≤∏≈ƒ° ∆¿¿ÅEÃ∞≈≥™...
		(pSetting->nGameType == MMATCH_GAMETYPE_DEATHMATCH_TEAM2) ||			// π´«—µ•Ω∫∏≈ƒ° ∆¿¿ÅEÃ∞≈≥™...
		 (pSetting->nGameType == MMATCH_GAMETYPE_GLADIATOR_TEAM) ||				// ƒÆ¿ÅE∆¿¿ÅEÃ∞≈≥™...
		 (pSetting->nGameType == MMATCH_GAMETYPE_ASSASSINATE) ||				// æœªÅEÅE¿Ã∏ÅE..
		(pSetting->nGameType == MMATCH_GAMETYPE_BLITZKRIEG) ||
		 (pSetting->nGameType == MMATCH_GAMETYPE_TEAM_TRAINING) ||
		 (pSetting->nGameType == MMATCH_GAMETYPE_CTF))
	{
		// ∏  ¿Ã∏ß πË∞ÅE¿ÃπÃ¡ÅE∫Ø»Ø
		if ( pAniMapImg)
			pAniMapImg->SetCurrentFrame(1);

		// ƒ˘Ω∫∆Æ UI ∞®√ÅE
		bQuestUI = false;
		bChallengeQuestUI = false;
	}
	else if ( pSetting->nGameType == MMATCH_GAMETYPE_SURVIVAL)					// √ß∏∞¡ˆƒ˘Ω∫∆Æ ∏µÅE/todok µ˚∑Œ «ÿæﬂ«“±ÅE
	{
		// ∏  ¿Ã∏ß πË∞ÅE¿ÃπÃ¡ÅE∫Ø»Ø
		if ( pAniMapImg)
			pAniMapImg->SetCurrentFrame(0);

		// ƒ˘Ω∫∆Æ UI ∞®√ÅE
		bQuestUI = false;
		bChallengeQuestUI = false;
	}
	else if ( pSetting->nGameType == MMATCH_GAMETYPE_QUEST)						// ƒ˘Ω∫∆Æ ∏µÂ¿Ã∏ÅE..
	{
		// ∏  ¿Ã∏ß πË∞ÅE¿ÃπÃ¡ÅE∫Ø»Ø
		if ( pAniMapImg)
			pAniMapImg->SetCurrentFrame(2);

		// ƒ˘Ω∫∆Æ UI ∫∏¿”
		bQuestUI = true;
		bChallengeQuestUI = false;
	}

	else if (pSetting->nGameType == MMATCH_GAMETYPE_QUEST_CHALLENGE)					// º≠πŸ¿ÃπÅE∏µÂ¿Ã∏ÅE..
	{
		// ∏  ¿Ã∏ß πË∞ÅE¿ÃπÃ¡ÅE∫Ø»Ø
		if (pAniMapImg)
			pAniMapImg->SetCurrentFrame(3);

		// ƒ˘Ω∫∆Æ UI ∞®√ÅE
		bChallengeQuestUI = true;
	}

	else if ( pSetting->nGameType == MMATCH_GAMETYPE_INFECTED)					// Custom: Infected game mode stage
	{
		if ( pAniMapImg)
			pAniMapImg->SetCurrentFrame(4);

		bQuestUI = false;
		bChallengeQuestUI = false;
	}

	else if (pSetting->nGameType == MMATCH_GAMETYPE_SPY)
	{
		if (pAniMapImg)
			pAniMapImg->SetCurrentFrame(6);	// Check your interface templates.xml for more details about this number.

		bQuestUI = false;
		bChallengeQuestUI = false;
		bSpyUI = true;
	}

	// ∏ º±≈√Ω√ ∏±∑π¿Ã∏ ¿Ã∏ÅE∏±∑π¿Ã∏  ∏ÆΩ∫∆Æ π⁄Ω∫∏¶ ø≠æ˚›ÿ¥Ÿ.
	 if(pSetting->bIsRelayMap)
		OpenRelayMapBox();
	else
		HideRelayMapBox();

	 ///// SPY MODE /////
	 if (bSpyUI)
		 OpenSpyBanMapBox();
	 else
		 HideSpyBanMapBox();

	 MLabel* pSpyRandomMapLabel = (MLabel*)pResource->FindWidget("Stage_SpyRandomMapLabel");
	 if (pSpyRandomMapLabel)
		 pSpyRandomMapLabel->Show(bSpyUI);

pCombo = (MComboBox*)pResource->FindWidget("MapSelection");
if (pCombo)
pCombo->Show(!bSpyUI);
////////////////////
ZApplication::GetGameInterface()->ShowWidget("Stage_SacrificeItemImage0", bQuestUI);
ZApplication::GetGameInterface()->ShowWidget("Stage_SacrificeItemImage1", bQuestUI);
ZApplication::GetGameInterface()->ShowWidget("Stage_QuestLevel", bQuestUI);
ZApplication::GetGameInterface()->ShowWidget("Stage_QuestLevelBG", bQuestUI);

#ifdef _BOXLEAD
MButton* pButton = (MButton*)pResource->FindWidget("Lead");
if (pButton != 0) pButton->SetCheck(!pSetting->bIsLeading);
#endif

// Custom: CQ Add Time Limit
ZApplication::GetGameInterface()->ShowWidget("Stage_ChallengeTime", bChallengeQuestUI);
ZApplication::GetGameInterface()->ShowWidget("Stage_ChallengeTimeBG", bChallengeQuestUI);

ZApplication::GetGameInterface()->ShowWidget("Stage_SacrificeItemButton0", bQuestUI);
ZApplication::GetGameInterface()->ShowWidget("Stage_SacrificeItemButton1", bQuestUI);

if (m_bPrevQuest != bQuestUI)
{
	ZApplication::GetGameInterface()->ShowWidget("Stage_Lights0", bQuestUI);
	ZApplication::GetGameInterface()->ShowWidget("Stage_Lights1", bQuestUI);

	m_SacrificeItem[SACRIFICEITEM_SLOT0].RemoveItem();
	m_SacrificeItem[SACRIFICEITEM_SLOT1].RemoveItem();

	UpdateSacrificeItem();

	if (bQuestUI)
	{
		//			ZApplication::GetGameInterface()->EnableWidget( "StageMaxPlayer", false);
		ZPostRequestSacrificeSlotInfo(ZGetGameClient()->GetPlayerUID());
		ZPostRequestQL(ZGetGameClient()->GetPlayerUID());
		OpenSacrificeItemBox();
	}
	else
	{
		MLabel* pLabel = (MLabel*)pResource->FindWidget("Stage_SenarioName");
		if (pLabel)
			pLabel->SetText("");
		ZApplication::GetGameInterface()->ShowWidget("Stage_SenarioNameImg", false);
		//			if (ZGetGameClient()->AmIStageMaster())
		//				ZApplication::GetGameInterface()->EnableWidget( "StageMaxPlayer", true);

		HideSacrificeItemBox();
	}

	m_bPrevQuest = !m_bPrevQuest;
}
if ((pSetting->nGameType == MMATCH_GAMETYPE_BLITZKRIEG))
{
	ZApplication::GetGameInterface()->EnableWidget("StageSettingCaller", false);
	ZApplication::GetGameInterface()->EnableWidget("MapSelection", true);
	ZApplication::GetGameInterface()->EnableWidget("StageType", true);
	ZApplication::GetGameInterface()->EnableWidget("StageMaxPlayer", true);
	ZApplication::GetGameInterface()->EnableWidget("StageRoundCount", false);
#ifdef _BOXLEAD
	ZApplication::GetGameInterface()->EnableWidget("Lead", true);
#endif
}
if ((pSetting->nGameType == MMATCH_GAMETYPE_SURVIVAL) || 
	(pSetting->nGameType == MMATCH_GAMETYPE_QUEST) || 
	(pSetting->nGameType == MMATCH_GAMETYPE_QUEST_CHALLENGE) ||
	(pSetting->nGameType == MMATCH_GAMETYPE_SPY))
ZApplication::GetGameInterface()->EnableWidget("StageSettingCaller", false);
ZApplication::GetGameInterface()->ShowWidget("Stage_SenarioDescription", bChallengeQuestUI);
ZApplication::GetGameInterface()->ShowWidget("Stage_SenarioTitle", bChallengeQuestUI);
ZApplication::GetGameInterface()->ShowWidget("Stage_ChallengeTime", bChallengeQuestUI);
if (bChallengeQuestUI)
{
	char szText[256];
	MNewQuestScenarioManager* pNewScenarioMgr = ZApplication::GetStageInterface()->GetChallengeQuestScenario();
	if (pNewScenarioMgr)
	{
		MNewQuestScenario* pNewScenario = pNewScenarioMgr->GetScenario(pSetting->szMapName);
		if (pNewScenario)
		{
			MTextArea* pNewScenarioDesc = (MTextArea*)pResource->FindWidget("Stage_SenarioDescription");
			if (pNewScenarioDesc)
			{
				pNewScenarioDesc->SetText(pNewScenario->GetDescription());
			}

			MLabel* pNewScenarioTitle = (MLabel*)pResource->FindWidget("Stage_SenarioTitle");
			if (pNewScenarioTitle)
			{
				pNewScenarioTitle->SetText(pNewScenario->GetName());
			}
			// Custom: CQ Add Time Limit
			MLabel* pGoodTime = (MLabel*)pResource->FindWidget("Stage_ChallengeTime");
			if (pGoodTime)
			{
				sprintf(szText, "Gen. Record : 8:00", pNewScenario->GetGoodTime() / 60, pNewScenario->GetGoodTime() % 60);
				pGoodTime->SetText(szText);
			}
			MPicture* pPicture = 0;
			MBitmap* pBitmap = 0;
			char szMapName[256];
			pPicture = (MPicture*)pResource->FindWidget("Stage_MainBGTop");
			if (pPicture)
			{
				if (((pNewScenario->GetID() == 101) ||
					(pNewScenario->GetID() == 201) ||
					(pNewScenario->GetID() == 301) ||
					(pNewScenario->GetID() == 401)))
					sprintf(szMapName, "interface/MapGB/map_ChallengeQuest_Guerilla.bmp");
				else
					sprintf(szMapName, "interface/MapGB/map_ChallengeQuest_Research.bmp");

					if (m_pTopBgImg != NULL)
					{
						delete m_pTopBgImg;
						m_pTopBgImg = NULL;
					}

					m_pTopBgImg = new MBitmapR2;
					((MBitmapR2*)m_pTopBgImg)->Create("TopBgImg.png", RGetDevice(), szMapName);

					if (m_pTopBgImg != NULL)
						pPicture->SetBitmap(m_pTopBgImg->GetSourceBitmap());
				}
			}
		}
	}
	// ∂Ûø˚—ÅEº±≈√ ƒﬁ∫∏π⁄Ω∫ ∫∏¿Ã±ÅE
//	bool bShowRound = true;
//	if ( ( pSetting->nGameType == MMATCH_GAMETYPE_SURVIVAL) || ( pSetting->nGameType == MMATCH_GAMETYPE_QUEST))
//		bShowRound = false;
	
//	ZApplication::GetGameInterface()->ShowWidget( "StageRoundCountLabelBG", bShowRound);
//	ZApplication::GetGameInterface()->ShowWidget( "StageRoundCountLabel", bShowRound);
//	ZApplication::GetGameInterface()->ShowWidget( "StageRoundCount", bShowRound);


	// ∂Ûø˚—ÅEor Kill
	MWidget* pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "StageRoundCountLabel");
	if ( pWidget)
	{
		if ((pSetting->nGameType == MMATCH_GAMETYPE_DEATHMATCH_SOLO) ||
			(pSetting->nGameType == MMATCH_GAMETYPE_DEATHMATCH_TEAM2) ||		// ∆¿µ•Ω∫∏≈ƒ° ¿ÕΩ∫∆Æ∏≤¿œ ∂ßø°µµ Kill∑Œ «•Ω√.... by kammir 20081117
			(pSetting->nGameType == MMATCH_GAMETYPE_GLADIATOR_SOLO) ||
			(pSetting->nGameType == MMATCH_GAMETYPE_TRAINING) ||
			(pSetting->nGameType == MMATCH_GAMETYPE_BERSERKER) ||
			(pSetting->nGameType == MMATCH_GAMETYPE_DUEL) ||
			(pSetting->nGameType == MMATCH_GAMETYPE_GUNGAME))
			pWidget->SetText( ZMsg(MSG_WORD_KILL));

		if (pSetting->nGameType == MMATCH_GAMETYPE_SKILLMAP)
			pWidget->SetText(ZMsg(MSG_WORD_ROUND));

		else if(pSetting->nGameType == MMATCH_GAMETYPE_CTF)
			pWidget->SetText(ZMsg(MSG_WORD_CAPUTRES));
		else
			pWidget->SetText( ZMsg(MSG_WORD_ROUND));
	}


	// ƒﬁ∆˜≥Õ∆Æ æ˜µ•¿Ã∆Æ
	ZStageSetting::ShowStageSettingDialog( pSetting, false);


	// ∞‘¿”¡ﬂ ∏ﬁ¥∫ ºˆ¡§ - ∏”«œ¥¬ ∫Œ∫–¿Œ¡ÅEæÀºÅEæ¯¿Ω...
	// ∞‘¿”¡ﬂ ∏ﬁ¥∫ø°º≠ ƒ˘Ω∫∆Æπ◊ º≠πŸ¿Ãπ˙¿œ∂ß ¥ÅE‚πÊ¿∏∑Œ ≥™∞°±ÅEπˆ∆∞ ∫Ò»∞º∫»≠ Ω√≈¥
#ifdef _QUEST
	if (ZGetGameTypeManager()->IsQuestDerived(pSetting->nGameType) || pSetting->nGameType == MMATCH_GAMETYPE_QUEST_CHALLENGE || ZGetGameClient()->IsCWChannel())
	{
		ZApplication::GetGameInterface()->GetCombatMenu()->EnableItem(ZCombatMenu::ZCMI_BATTLE_EXIT, false);
	}
	else if (ZGetGameClient()->IsLadderChannel())
	{
		ZApplication::GetGameInterface()->GetCombatMenu()->EnableItem(ZCombatMenu::ZCMI_BATTLE_EXIT, false);
	}
	else
	{
		ZApplication::GetGameInterface()->GetCombatMenu()->EnableItem(ZCombatMenu::ZCMI_BATTLE_EXIT, true);
	}
#endif
}


/***********************************************************************
  ChangeStageButtons : public
  
  desc : Ω∫≈◊¿Ã¡ÅE≥ª¿« πˆ∆∞µÅE∞‘¿”Ω√¿€, ≥≠¿‘, ¡ÿ∫Òøœ∑ÅE¿« ªÛ≈¬∏¶ º≥¡§«—¥Ÿ.
         ∞‘¿” º≥¡§∞ÅE∞ÅE√µ» ¿ß¡¨¿« UI∏¶ º≥¡§«—¥Ÿ.
  arg  : bForcedEntry = ≥≠¿‘ ø©∫Œ(true or false)
         bMaster = πÊ¿ÅEø©∫Œ(true or false)
		 bReady = ¡ÿ∫ÅEøœ∑ÅEø©∫Œ(true or false)
  ret  : none
************************************************************************/
void ZStageInterface::ChangeStageButtons( bool bForcedEntry, bool bMaster, bool bReady)
{
	if ( bForcedEntry)	// ≥≠¿‘ ∏µÅE
	{
		ZApplication::GetGameInterface()->ShowWidget( "GameStart", false);
		ZApplication::GetGameInterface()->ShowWidget( "StageReady", false);

		ZApplication::GetGameInterface()->ShowWidget( "ForcedEntryToGame", true);
		ZApplication::GetGameInterface()->ShowWidget( "ForcedEntryToGame2", true);

		ChangeStageEnableReady( false);
	}
	else
	{
		ZApplication::GetGameInterface()->ShowWidget( "ForcedEntryToGame", false);
		ZApplication::GetGameInterface()->ShowWidget( "ForcedEntryToGame2", false);

		ZApplication::GetGameInterface()->ShowWidget( "GameStart", bMaster);
		ZApplication::GetGameInterface()->ShowWidget( "StageReady", !bMaster);

		if ( bMaster)
		{
			// ∫∏≈ÅEπÊ¿Â¿Ã∂Û∏ÅEπÊºº∆√¿ª πŸ≤‹ ºÅE¿÷¥¬ ±««—¿ª ∞°¡¯¥Ÿ (UI enable).
			// ±◊∑Ø≥™ ±◊∑°º≠¥¬ æ»µ«¥¬ ∞ÊøÅE° ¿÷¥¬µ• ±◊∞… √º≈©«—¥Ÿ.
			bool bMustDisableUI_despiteMaster = false;
			
			// ƒ˘Ω∫∆Æ∏µÂø°º≠ πÊ¿Â¿Ã ∞‘¿”Ω√¿€¿ª ¥©∏£∞ÅE»Òª˝æ∆¿Ã≈€ ∫“≈∏¥¬ æ÷¥œ∏ﬁ¿Ãº« µµ¡ﬂ πÊ¿Â¿Ã Ω∫Ω∫∑Œ∏¶ kick«ÿº≠
			// ¥Ÿ∏• ªÁ∂˜¿Ã πÊ¿ÅE±««—¿ª æÚ¿∫ ∞ÊøÅE ≥ª∞° πÊ¿Â¿œ¡ˆ∂Ûµµ ¥ı¿ÃªÅEπÊºº∆√¿ª πŸ≤‹ ºˆ¥¬ æ¯¥Ÿ
			if ( m_bDrawStartMovieOfQuest)
				bMustDisableUI_despiteMaster = true;


			if (bMustDisableUI_despiteMaster)
				ChangeStageEnableReady( true);	// UI disable
			else
				ChangeStageEnableReady( false);	// UI enable
		}
		else
			ChangeStageEnableReady( bReady);
	}
}


/***********************************************************************
  ChangeStageEnableReady : public
  
  desc : Ω∫≈◊¿Ã¡ˆ¿« ¿Œ≈Õ∆‰¿ÃΩ∫(πˆ∆∞ enableµÅE∏¶ ƒ—∞≈≥™ ≤Ù¥¬ «‘ºÅE
  arg  : true(=interface enable) or false(=interface disable)
  ret  : none
************************************************************************/
void ZStageInterface::ChangeStageEnableReady( bool bReady)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	ZApplication::GetGameInterface()->EnableWidget( "GameStart", !bReady);

	ZApplication::GetGameInterface()->EnableWidget( "StageTeamBlue",  !bReady);
	ZApplication::GetGameInterface()->EnableWidget( "StageTeamBlue2", !bReady);
	ZApplication::GetGameInterface()->EnableWidget( "StageTeamRed",  !bReady);
	ZApplication::GetGameInterface()->EnableWidget( "StageTeamRed2", !bReady);
	ZApplication::GetGameInterface()->EnableWidget( "Lobby_StageExit", !bReady);

	if ( (m_nGameType == MMATCH_GAMETYPE_SURVIVAL) || (m_nGameType == MMATCH_GAMETYPE_QUEST))
	{
		ZApplication::GetGameInterface()->EnableWidget( "Stage_SacrificeItemListbox", !bReady);
		ZApplication::GetGameInterface()->EnableWidget( "Stage_PutSacrificeItem",     !bReady);
		ZApplication::GetGameInterface()->EnableWidget( "Stage_SacrificeItemButton0", !bReady);
		ZApplication::GetGameInterface()->EnableWidget( "Stage_SacrificeItemButton1", !bReady);
		if ( ZGetGameClient()->AmIStageMaster())
		{
			ZApplication::GetGameInterface()->EnableWidget( "MapSelection", !bReady);
			ZApplication::GetGameInterface()->EnableWidget( "StageType", !bReady);
		}
//		ZApplication::GetGameInterface()->EnableWidget( "StageMaxPlayer", false);
		ZApplication::GetGameInterface()->EnableWidget( "StageSettingCaller", false);
		ZApplication::GetGameInterface()->EnableWidget("Lead", false);
	}
	else
	{
		if ( ZGetGameClient()->AmIStageMaster())
		{
			ZApplication::GetGameInterface()->EnableWidget( "MapSelection", !bReady);
			ZApplication::GetGameInterface()->EnableWidget( "StageType", !bReady);
			ZApplication::GetGameInterface()->EnableWidget( "StageMaxPlayer", !bReady);
			ZApplication::GetGameInterface()->EnableWidget( "StageRoundCount", !bReady);
			if (m_nGameType != MMATCH_GAMETYPE_SPY)
				ZApplication::GetGameInterface()->EnableWidget("StageSettingCaller", !bReady);
			ZApplication::GetGameInterface()->EnableWidget( "Stage_RelayMap_OK_Button", !bReady);
			ZApplication::GetGameInterface()->EnableWidget( "Stage_RelayMapType", !bReady);
			ZApplication::GetGameInterface()->EnableWidget( "Stage_RelayMapRepeatCount", !bReady);
#ifdef _BOXLEAD
			ZApplication::GetGameInterface()->EnableWidget( "Lead", !bReady);
#endif
		}
		else
		{
			ZApplication::GetGameInterface()->EnableWidget( "MapSelection", false);
			ZApplication::GetGameInterface()->EnableWidget( "StageType", false);
			ZApplication::GetGameInterface()->EnableWidget( "StageMaxPlayer", false);
			ZApplication::GetGameInterface()->EnableWidget( "StageRoundCount", false);
			ZApplication::GetGameInterface()->EnableWidget( "StageSettingCaller", false);
			ZApplication::GetGameInterface()->EnableWidget( "Stage_RelayMap_OK_Button", false);
			ZApplication::GetGameInterface()->EnableWidget( "Stage_RelayMapType", false);
			ZApplication::GetGameInterface()->EnableWidget( "Stage_RelayMapRepeatCount", false);
#ifdef _BOXLEAD
			ZApplication::GetGameInterface()->EnableWidget("Lead", false);
#endif
		}
	}
    
	BEGIN_WIDGETLIST( "Stage_OptionFrame", pResource, MButton*, pButton);
	pButton->Enable( !bReady);
	END_WIDGETLIST();

	BEGIN_WIDGETLIST( "EquipmentCaller", pResource, MButton*, pButton);
	pButton->Enable( !bReady);
	END_WIDGETLIST();

	// Custom: Show shop in stage
	BEGIN_WIDGETLIST( "ShopCaller", pResource, MButton*, pButton);
	pButton->Enable( !bReady);
	END_WIDGETLIST();

	BEGIN_WIDGETLIST("MedalShopCaller", pResource, MButton*, pButton);
	pButton->Enable(!bReady);
	END_WIDGETLIST();

	ZApplication::GetGameInterface()->EnableWidget( "StagePlayerList_", !bReady);
}


/***********************************************************************
  SetMapName : public
  
  desc : ∏  º±≈√ ƒﬁ∫∏π⁄Ω∫ø° «ˆ¿ÅEº±≈√µ» ∏  ¿Ã∏ß¿ª ºº∆√«—¥Ÿ.
  arg  : szMapName = ∏  ¿Ã∏ß
  ret  : none
************************************************************************/
void ZStageInterface::SetMapName( const char* szMapName)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	if ( szMapName == NULL)
		return;

	MComboBox* pMapCombo = (MComboBox*)pResource->FindWidget( "MapSelection");
	if ( pMapCombo)
	{
		// ¿œ¥‹ ¿”Ω√ «œµÂƒ⁄µÅEøÅE°ø®~ §–.§–)
//		if ( m_nGameType == MMATCH_GAMETYPE_QUEST)
//			pMapCombo->SetText( "Mansion");
//		else
		pMapCombo->SetText(szMapName);
	}
}




/*
	ø©±‚º≠∫Œ≈Õ ªı∑Œ √ﬂ∞°µ» ≥ªøÅE..

	øÿ∏∏«—∞« ∏ÆΩ∫≥ ≥™ ¥Ÿ∏• ∞˜ø°º≠ ¿⁄µø¿∏∑Œ »£√‚µ«µµ∑œ «ÿ≥˘¿∏≥™ æ∆¡ÅE≈◊Ω∫∆Æ¿Œ ∞ÅEË∑Œ
	øœ∫Æ«—∞« æ∆¥‘...  -_-;
*/



/***********************************************************************
  OpenSacrificeItemBox : public
  
  desc : »Òª˝ æ∆¿Ã≈€ º±≈√ √¢ ø≠±ÅE
  arg  : none
  ret  : none
************************************************************************/
void ZStageInterface::OpenSacrificeItemBox( void)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	MButton* pButton = (MButton*)pResource->FindWidget( "Stage_SacrificeItemBoxOpen");
	if ( pButton)
		pButton->Show( false);
	pButton = (MButton*)pResource->FindWidget( "Stage_SacrificeItemBoxClose");
	if ( pButton)
		pButton->Show( true);

	m_nStateSacrificeItemBox = 2;
	GetSacrificeItemBoxPos();
}


/***********************************************************************
  CloseSacrificeItemBox : public
  
  desc : »Òª˝ æ∆¿Ã≈€ º±≈√ √¢ ¥›±ÅE
  arg  : none
  ret  : none
************************************************************************/
void ZStageInterface::CloseSacrificeItemBox( void)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	MButton* pButton = (MButton*)pResource->FindWidget( "Stage_SacrificeItemBoxClose");
	if ( pButton)
		pButton->Show( false);
	pButton = (MButton*)pResource->FindWidget( "Stage_SacrificeItemBoxOpen");
	if ( pButton)
		pButton->Show( true);

	MWidget* pWidget = pResource->FindWidget( "Stage_CharacterInfo");
	if ( pWidget)
		pWidget->Enable( true);

	m_nStateSacrificeItemBox = 1;
	GetSacrificeItemBoxPos();
}


/***********************************************************************
  HideSacrificeItemBox : public
  
  desc : »Òª˝ æ∆¿Ã≈€ º±≈√ √¢ ∞®√ﬂ±ÅE
  arg  : none
  ret  : none
************************************************************************/
void ZStageInterface::HideSacrificeItemBox( void)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	MButton* pButton = (MButton*)pResource->FindWidget( "Stage_SacrificeItemBoxClose");
	if ( pButton)
		pButton->Show( false);
	pButton = (MButton*)pResource->FindWidget( "Stage_SacrificeItemBoxOpen");
	if ( pButton)
		pButton->Show( true);

	MWidget* pWidget = pResource->FindWidget( "Stage_CharacterInfo");
	if ( pWidget)
		pWidget->Enable( true);

	m_nStateSacrificeItemBox = 0;
	GetSacrificeItemBoxPos();
}


/***********************************************************************
  HideSacrificeItemBox : public
  
  desc : »Òª˝ æ∆¿Ã≈€ º±≈√ √¢ ¿ßƒ° ±∏«œ±ÅE
  arg  : none
  ret  : none
************************************************************************/
void ZStageInterface::GetSacrificeItemBoxPos( void)
{
//#ifdef _DEBUG
//	m_nListFramePos = 0;
//	return;
//#endif

	MWidget* pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Stage_ItemListView");
	if ( pWidget)
	{
		MRECT rect;

		switch ( m_nStateSacrificeItemBox)
		{
			case 0 :		// Hide
				rect = pWidget->GetRect();
				m_nListFramePos = -rect.w;
				break;

			case 1 :		// Close
				rect = pWidget->GetRect();
				m_nListFramePos = -rect.w + ( rect.w * 0.14);
				break;

			case 2 :		// Open
				m_nListFramePos = 0;
				break;
		}
	}
}


/***********************************************************************
  OnSacrificeItem0 : public
  
  desc : »Òª˝ æ∆¿Ã≈€0¿ª ¥≠∑∂¿ª ∂ß
  arg  : none
  ret  : none
************************************************************************/
void ZStageInterface::OnSacrificeItem0( void)
{
}


/***********************************************************************
  OnSacrificeItem1 : public
  
  desc : »Òª˝ æ∆¿Ã≈€1∏¶ ¥≠∑∂¿ª ∂ß
  arg  : none
  ret  : none
************************************************************************/
void ZStageInterface::OnSacrificeItem1( void)
{
}


/***********************************************************************
  UpdateSacrificeItem : protected
  
  desc : ∫Ø∞Êµ» »Òª˝ æ∆¿Ã≈€ ¿ÃπÃ¡ÅE ¡§∫∏µ˚‹ª æ˜µ•¿Ã∆Æ «‘.
  arg  : none
  ret  : none
************************************************************************/
void ZStageInterface::UpdateSacrificeItem( void)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	// Ω∫≈◊¿Ã¡ÅEøµø™ø° ¿÷¥¬ »Òª˝ æ∆¿Ã≈€ ¿ÃπÃ¡ÅEºˆ¡§
	for ( int i = SACRIFICEITEM_SLOT0;  i <= SACRIFICEITEM_SLOT1;  i++)
	{
		char szWidgetNameItem[ 128];
		sprintf( szWidgetNameItem, "Stage_SacrificeItemImage%d", i);
		MPicture* pPicture = (MPicture*)pResource->FindWidget( szWidgetNameItem);
		if ( pPicture)
		{
			if ( m_SacrificeItem[ i].IsExist())
			{
				pPicture->SetBitmap( m_SacrificeItem[ i].GetIconBitmap());
				char szMsg[ 128];
				MMatchObjCache* pObjCache = ZGetGameClient()->FindObjCache( m_SacrificeItem[ i].GetUID());
				if ( pObjCache)
					sprintf( szMsg, "%s (%s)", m_SacrificeItem[ i].GetName(), pObjCache->GetName());
				else
					strcpy( szMsg, m_SacrificeItem[ i].GetName());
				pPicture->AttachToolTip( szMsg);
			}
			else
			{
				pPicture->SetBitmap( NULL);
				pPicture->DetachToolTip();
			}
		}
	}
}


/***********************************************************************
  SerializeSacrificeItemListBox : public
  
  desc : »Òª˝ æ∆¿Ã≈€ ∏ÆΩ∫∆Æ π⁄Ω∫ø° ¿⁄∑·∏¶ πﬁ¥¬¥Ÿ.
  arg  : none
  ret  : none
************************************************************************/
void ZStageInterface::SerializeSacrificeItemListBox( void)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	MListBox* pListBox = (MListBox*)pResource->FindWidget( "Stage_SacrificeItemListbox");
	if ( !pListBox)
		return;

	int nStartIndex  = pListBox->GetStartItem();
	int nSelectIndex = pListBox->GetSelIndex();
	pListBox->RemoveAll();

	// ∏ÆΩ∫∆Æø° √ﬂ∞°
	for ( MQUESTITEMNODEMAP::iterator questitem_itor = ZGetMyInfo()->GetItemList()->GetQuestItemMap().begin();
		  questitem_itor != ZGetMyInfo()->GetItemList()->GetQuestItemMap().end();
		  questitem_itor++)
	{
		ZMyQuestItemNode* pItemNode = (*questitem_itor).second;
		MQuestItemDesc* pItemDesc = GetQuestItemDescMgr().FindQItemDesc( pItemNode->GetItemID());
		if ( pItemDesc)
		{
			int nCount = pItemNode->m_nCount;
			if ( m_SacrificeItem[ SACRIFICEITEM_SLOT0].IsExist() &&
				 (m_SacrificeItem[ SACRIFICEITEM_SLOT0].GetUID() == ZGetGameClient()->GetPlayerUID()) &&
				 (pItemDesc->m_nItemID == m_SacrificeItem[ SACRIFICEITEM_SLOT0].GetItemID()))
				nCount--;
			if ( m_SacrificeItem[ SACRIFICEITEM_SLOT1].IsExist() &&
				 (m_SacrificeItem[ SACRIFICEITEM_SLOT1].GetUID() == ZGetGameClient()->GetPlayerUID()) &&
				 (pItemDesc->m_nItemID == m_SacrificeItem[ SACRIFICEITEM_SLOT1].GetItemID()))
				nCount--;

			if ( pItemDesc->m_bSecrifice && (nCount > 0))		// »Òª˝ æ∆¿Ã≈€∏∏ √ﬂ∞°
			{
				pListBox->Add( new SacrificeItemListBoxItem( pItemDesc->m_nItemID,
															 ZApplication::GetGameInterface()->GetQuestItemIcon( pItemDesc->m_nItemID, true),
															 pItemDesc->m_szQuestItemName,
															 nCount,
															 pItemDesc->m_szDesc));
			}
		}
	}

	MWidget* pWidget = pResource->FindWidget( "Stage_NoItemLabel");
	if ( pWidget)
	{
		if ( pListBox->GetCount() > 0)
			pWidget->Show( false);
		else
			pWidget->Show( true);
	}

	pListBox->SetStartItem( nStartIndex);
	pListBox->SetSelIndex( min( (pListBox->GetCount() - 1), nSelectIndex));
}


/***********************************************************************
  OnDropSacrificeItem : public
  
  desc : »Òª˝ æ∆¿Ã≈€ ≥ı±ÅE
  arg  : none
  ret  : none
************************************************************************/
void ZStageInterface::OnDropSacrificeItem( int nSlotNum)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	MButton* pReadyBtn = (MButton*)pResource->FindWidget("StageReady");
	if(pReadyBtn) 
		if(pReadyBtn->GetCheck()) 
		{	// ¡ÿ∫ÅEøœ∑·πˆ∆∞¿Ã ¥≠∑¡¿÷¥Ÿ∏ÅE
			ZApplication::GetStageInterface()->ChangeStageEnableReady(pReadyBtn->GetCheck());	// ¥Ÿ∏• UI∏¶ ≤®¡ÿ¥Ÿ.
			return;	// »Òª˝æ∆¿Ã≈€¿ª ΩΩ∑‘ø° ø√∏Æ¡ÅEæ ¥¬¥Ÿ.
		}

	MListBox* pListBox = (MListBox*)pResource->FindWidget( "Stage_SacrificeItemListbox");
	if ( !pListBox || (pListBox->GetSelIndex() < 0))
		return;

	SacrificeItemListBoxItem* pItemDesc = (SacrificeItemListBoxItem*)pListBox->Get( pListBox->GetSelIndex());
	if ( pItemDesc)
	{
		MTextArea* pDesc = (MTextArea*)pResource->FindWidget( "Stage_ItemDesc");

		// ΩΩ∑‘¿Ã ∫Òæ˚‹÷¿∏∏ÅEπ´¡∂∞« ø√∏≤
		if ( ! m_SacrificeItem[ nSlotNum].IsExist())
		{
			ZPostRequestDropSacrificeItem( ZGetGameClient()->GetPlayerUID(), nSlotNum, pItemDesc->GetItemID());
			char szText[256];
			sprintf(szText, ZMsg( MSG_QUESTITEM_USE_DESCRIPTION ));
			pDesc->SetText(szText);
			//if ( pDesc)
			//	pDesc->Clear();
		}

		// ΩΩ∑‘¿Ã ∫Òæ˚‹÷¡ÅEæ ¿∏∏ÅE..
		else
		{
			if ( (m_SacrificeItem[ nSlotNum].GetUID()    != ZGetGameClient()->GetPlayerUID()) ||
				 (m_SacrificeItem[ nSlotNum].GetItemID() != pItemDesc->GetItemID()))
				ZPostRequestDropSacrificeItem( ZGetGameClient()->GetPlayerUID(), nSlotNum, pItemDesc->GetItemID());

			char szText[256];
			sprintf(szText, ZMsg( MSG_QUESTITEM_USE_DESCRIPTION ));
			pDesc->SetText(szText);
			//if ( pDesc)
			//	pDesc->Clear();
		}
	}
}


/***********************************************************************
  OnRemoveSacrificeItem : public
  
  desc : »Òª˝ æ∆¿Ã≈€ ª©±ÅE
  arg  : none
  ret  : none
************************************************************************/
void ZStageInterface::OnRemoveSacrificeItem( int nSlotNum)
{
	if ( !m_SacrificeItem[ nSlotNum].IsExist())
		return;

	ZPostRequestCallbackSacrificeItem( ZGetGameClient()->GetPlayerUID(),
									   nSlotNum,
									   m_SacrificeItem[ nSlotNum].GetItemID());

	MTextArea* pDesc = (MTextArea*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Stage_ItemDesc");
	char szText[256];
	sprintf(szText, ZMsg( MSG_QUESTITEM_USE_DESCRIPTION ));
	pDesc->SetText(szText);
	//if ( pDesc)
	//	pDesc->Clear();
}


/***********************************************************************
  MSacrificeItemListBoxListener
  
  desc : »Òª˝ æ∆¿Ã≈€ ∏ÆΩ∫∆Æ π⁄Ω∫ ∏ÆΩ∫≥ 
************************************************************************/
class MSacrificeItemListBoxListener : public MListener
{
public:
	virtual bool OnCommand(MWidget* pWidget, const char* szMessage)
	{
		// On select
		if ( MWidget::IsMsg( szMessage, MLB_ITEM_SEL) == true)
		{
			MListBox* pListBox = (MListBox*)pWidget;

			// æ∆¿Ã≈€ º≥∏ÅEæ˜µ•¿Ã∆Æ
			SacrificeItemListBoxItem* pItemDesc = (SacrificeItemListBoxItem*)pListBox->GetSelItem();
			MTextArea* pDesc = (MTextArea*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Stage_ItemDesc");
			if ( pItemDesc && pDesc)
			{
				char szCount[ 128];
				sprintf( szCount, "%s : %d", ZMsg( MSG_WORD_QUANTITY), pItemDesc->GetItemCount());
				pDesc->SetTextColor( MCOLOR( 0xFFD0D0D0));
				pDesc->SetText( szCount);
				pDesc->AddText( "\n");
				pDesc->AddText( pItemDesc->GetItemDesc(), 0xFF808080);
			}

			return true;
		}


		// On double click
		else if ( MWidget::IsMsg( szMessage, MLB_ITEM_DBLCLK) == true)
		{
			// Put item
			if ( !ZApplication::GetStageInterface()->m_SacrificeItem[ 0].IsExist())
				ZApplication::GetStageInterface()->OnDropSacrificeItem( 0);
			else if ( !ZApplication::GetStageInterface()->m_SacrificeItem[ 1].IsExist())
				ZApplication::GetStageInterface()->OnDropSacrificeItem( 1);
		
			return true;
		}

		return false;
	}
};

MSacrificeItemListBoxListener g_SacrificeItemListBoxListener;

MListener* ZGetSacrificeItemListBoxListener( void)
{
	return &g_SacrificeItemListBoxListener;
}


/***********************************************************************
  OnDropCallbackRemoveSacrificeItem
  
  desc : »Òª˝ æ∆¿Ã≈€ ¡¶∞≈
************************************************************************/
void OnDropCallbackRemoveSacrificeItem( void* pSelf, MWidget* pSender, MBitmap* pBitmap, const char* szString, const char* szItemString)
{
	if ( (pSender == NULL) || (strcmp(pSender->GetClassName(), MINT_ITEMSLOTVIEW)))
		return;

	ZItemSlotView* pItemSlotView = (ZItemSlotView*)pSender;
	ZApplication::GetStageInterface()->OnRemoveSacrificeItem( (strcmp( pItemSlotView->m_szItemSlotPlace, "SACRIFICE0") == 0) ? 0 : 1);
}


// ∏±∑π¿Ã∏  ¿Œ≈Õ∆‰¿ÃΩ∫
/***********************************************************************
OpenRelayMapBox : public

desc : ∏±∑π¿Ã∏  º±≈√ √¢ ø≠±ÅE
arg  : none
ret  : none
************************************************************************/

void ZStageInterface::OpenRelayMapBox( void)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	MButton* pButton = (MButton*)pResource->FindWidget( "Stage_RelayMapBoxOpen");
	if ( pButton)
		pButton->Show( false);
	pButton = (MButton*)pResource->FindWidget( "Stage_RelayMapBoxClose");
	if ( pButton)
		pButton->Show( true);

	SetRelayMapBoxPos(2);

	// ∏Æ«√∑π¿Ã π⁄Ω∫ø° ∏ ∏ÆΩ∫∆Æ∏¶ ª˝º∫
	ZApplication::GetStageInterface()->RelayMapCreateMapList();
}

/***********************************************************************
CloseRelayMapBox : public

desc : ∏±∑π¿Ã∏  º±≈√ √¢ ¥›±ÅE
arg  : none
ret  : none
************************************************************************/

void ZStageInterface::CloseRelayMapBox( void)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	MButton* pButton = (MButton*)pResource->FindWidget( "Stage_RelayMapBoxClose");
	if ( pButton)
		pButton->Show( false);
	pButton = (MButton*)pResource->FindWidget( "Stage_RelayMapBoxOpen");
	if ( pButton)
		pButton->Show( true);

	MWidget* pWidget = pResource->FindWidget( "Stage_CharacterInfo");
	if ( pWidget)
		pWidget->Enable( true);

	SetRelayMapBoxPos(1);
}

/***********************************************************************
HideRelayMapBox : public

desc : ∏±∑π¿Ã∏  º±≈√ √¢ ∞®√ﬂ±ÅE
arg  : none
ret  : none
************************************************************************/

void ZStageInterface::HideRelayMapBox( void)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	MButton* pButton = (MButton*)pResource->FindWidget( "Stage_RelayMapBoxClose");
	if ( pButton)
		pButton->Show( false);
	pButton = (MButton*)pResource->FindWidget( "Stage_RelayMapBoxOpen");
	if ( pButton)
		pButton->Show( true);

	MWidget* pWidget = pResource->FindWidget( "Stage_CharacterInfo");
	if ( pWidget)
		pWidget->Enable( true);

	SetRelayMapBoxPos(0);
}

/***********************************************************************
GetRelayMapBoxPos : public

desc : ∏±∑π¿Ã∏  º±≈√ √¢ ¿ßƒ° ±∏«œ±ÅE
arg  : none
ret  : none
************************************************************************/

void ZStageInterface::SetRelayMapBoxPos( int nBoxPos)
{
	MWidget* pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Stage_RelayMapListView");
	if ( pWidget)
	{
		MRECT rect;

		switch ( nBoxPos)
		{
		case 0 :		// Hide
			rect = pWidget->GetRect();
			m_nRelayMapListFramePos = -rect.w;
			break;

		case 1 :		// Close
			rect = pWidget->GetRect();
			m_nRelayMapListFramePos = -rect.w + ( rect.w * 0.14);
			break;

		case 2 :		// Open
			m_nRelayMapListFramePos = 0;
			break;
		}
	}
}

/***********************************************************************
PostRelayMapType : public

desc : ∏±∑π¿Ã∏  ≈∏¿‘ æ˜µ•¿Ã∆Æ π◊ º≠πˆø° ¿ÅE€
arg  : none
ret  : none
************************************************************************/
void ZStageInterface::PostRelayMapElementUpdate( void)
{
	MComboBox* pCBRelayMapType = (MComboBox*)ZGetGameInterface()->GetIDLResource()->FindWidget( "Stage_RelayMapType" );
	if ( !pCBRelayMapType) return;
	MComboBox* pCBRelayMapTurnCount = (MComboBox*)ZGetGameInterface()->GetIDLResource()->FindWidget( "Stage_RelayMapRepeatCount" );
	if ( !pCBRelayMapTurnCount) return;
	ZPostStageRelayMapElementUpdate(ZGetGameClient()->GetStageUID(), pCBRelayMapType->GetSelIndex(), pCBRelayMapTurnCount->GetSelIndex());
}

/***********************************************************************
PostRelayMapListUpdate : public

desc : ∏±∑π¿Ã∏  ∏ÆΩ∫∆Æ æ˜µ•¿Ã∆Æ(º≠πˆø°µµ ∏ÆΩ∫∆Æ∏¶ ∫∏≥Ω¥Ÿ.)
arg  : none
ret  : none
************************************************************************/
void ZStageInterface::PostRelayMapInfoUpdate( void)
{
	MComboBox* pCBRelayMapType = (MComboBox*)ZGetGameInterface()->GetIDLResource()->FindWidget( "Stage_RelayMapType" );
	if ( !pCBRelayMapType) return;
	MComboBox* pCBRelayMapRepeatCount = (MComboBox*)ZGetGameInterface()->GetIDLResource()->FindWidget( "Stage_RelayMapRepeatCount" );
	if ( !pCBRelayMapRepeatCount) return;

	MListBox* pRelayMapListBox = (MListBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("Stage_RelayMapListbox");
	if(pRelayMapListBox == NULL) return;

	if(pRelayMapListBox->GetCount() <= 0)	
	{
		ZApplication::GetGameInterface()->ShowMessage(MSG_GAME_RELAYMAP_ONE_OR_MORE_MAP_SELECT);
		return;
	}
	
	//∏±∑π¿Ã∏  ∏ÆΩ∫∆Æ ¿ÅE€
	void* pMapArray = MMakeBlobArray(sizeof(MTD_RelayMap), pRelayMapListBox->GetCount());
	int nMakeBlobCnt = 0;
	for(int i=0; i<pRelayMapListBox->GetCount(); i++)
	{
		MTD_RelayMap* pRelayMapNode = (MTD_RelayMap*)MGetBlobArrayElement(pMapArray, nMakeBlobCnt);
		for (int j = 0; j < MMATCH_MAP_COUNT; j++)
		{
			if(0 == strcmp(pRelayMapListBox->GetString(i), (char*)MGetMapDescMgr()->GetMapName(j)))
			{
				pRelayMapNode->nMapID = j;
				break;
			}
		}
		++nMakeBlobCnt;
	}

	ZPostStageRelayMapInfoUpdate(ZGetGameClient()->GetStageUID(), pCBRelayMapType->GetSelIndex(), pCBRelayMapRepeatCount->GetSelIndex(), pMapArray);
	MEraseBlobArray(pMapArray);

	ZApplication::GetStageInterface()->SetIsRelayMapRegisterComplete(true);
	ZApplication::GetGameInterface()->EnableWidget( "Stage_RelayMap_OK_Button", false);
}

/***********************************************************************
PostRelayMapList : public

desc : ∏±∑π¿Ã∏  º±≈√¿∏∑Œ ∏  ƒﬁ∫∏π⁄Ω∫ ∏ÆΩ∫∆Æ∏¶ ∏∏µÈæÅE¡ÿ¥Ÿ.
arg  : none
ret  : none
************************************************************************/
void ZStageInterface::RelayMapCreateMapList()
{
	MComboBox* pCombo = (MComboBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("MapSelection");
	if(pCombo == NULL) return;

	// ∏  ∏ÆΩ∫∆Æ ∏∏µÈæÅE¡÷±ÅE
	MListBox* pMapListBox = (MListBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("Stage_MapListbox");
	if(pMapListBox == NULL) return;

	pMapListBox->RemoveAll();	// ±‚¡∏ ∏±∑π¿Ã∏  ∏ÆΩ∫∆Æ∏¶ ∏µŒ ¡ˆøˆ¡ÿ¥Ÿ.
	for( int i = 0 ; i < pCombo->GetCount(); ++i )
	{
		if(strcmp(MMATCH_MAPNAME_RELAYMAP, pCombo->GetString(i)) == 0)
			continue;
		RelayMapList* pRelayMapList = new RelayMapList( pCombo->GetString(i), MBitmapManager::Get( "Mark_Arrow.bmp"));
		pMapListBox->Add( pRelayMapList);
	}


	// πÊ¿Â¿Ã ∏±∑π¿Ã∏  ºº∆√¡ﬂ¿Ã∏ÅE∏±∑π¿Ã∏  ∏ÆΩ∫∆Æ∏¶ æ˜µ•¿Ã∆Æ «œ¡ÅEæ ¥¬¥Ÿ.
	if(!ZApplication::GetStageInterface()->GetIsRelayMapRegisterComplete())
		return;

	// ∏±∑π¿Ã∏  ∏ÆΩ∫∆Æ ∏∏µÈæÅE¡÷±ÅE
	MListBox* pRelaMapListBox = (MListBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("Stage_RelayMapListbox");
	if(pRelaMapListBox == NULL) return;
	RelayMap arrayRelayMapList[MAX_RELAYMAP_LIST_COUNT];
	memcpy(arrayRelayMapList, ZGetGameClient()->GetMatchStageSetting()->GetRelayMapList(), sizeof(RelayMap)*MAX_RELAYMAP_LIST_COUNT);
	
	pRelaMapListBox->RemoveAll();	// ±‚¡∏ ∏±∑π¿Ã∏  ∏ÆΩ∫∆Æ∏¶ ∏µŒ ¡ˆøˆ¡ÿ¥Ÿ.
	for( int i = 0 ; i < ZGetGameClient()->GetMatchStageSetting()->GetRelayMapListCount(); ++i )
	{// ∏±∑π¿Ã∏  ∏ÆΩ∫∆Æø° µ•¿Ã≈Õ∏¶ √ﬂ∞°«ÿ¡ÿ¥Ÿ.
		int nMapID = arrayRelayMapList[i].nMapID;
		RelayMapList* pRelayMapList = new RelayMapList( MGetMapDescMgr()->GetMapName(MGetMapDescMgr()->GetMapID(nMapID)), MBitmapManager::Get( "Mark_X.bmp"));
		pRelaMapListBox->Add( pRelayMapList);
	}
}

/***********************************************************************
MMapListBoxListener

desc : ∏  ∏ÆΩ∫∆Æ π⁄Ω∫ ∏ÆΩ∫≥ 
************************************************************************/
class MMapListBoxListener : public MListener
{
public:
	virtual bool OnCommand(MWidget* pWidget, const char* szMessage)
	{
		// Ω∫≈◊¿Ã¡ÅE∏∂Ω∫≈Õ∞° æ∆¥œ∏ÅE∏  ∏ÆΩ∫∆Æ∏¶ ƒ¡∆Æ∑— «“ºˆæ¯¥Ÿ.
		if(!ZGetGameClient()->AmIStageMaster())
			return false;
		// On select
		if ( MWidget::IsMsg( szMessage, MLB_ITEM_SEL) == true)
		{
			// ∏ ∏ÆΩ∫∆Æø°º≠ º±≈√µ» ∏ ¡§∫∏∏¶ ∏±∑π¿Ã∏  ∏ÆΩ∫∆Æø° √ﬂ∞°«—¥Ÿ.
			MListBox* pMapListBox = (MListBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("Stage_MapListbox");
			if(pMapListBox == NULL) return false;
			MListBox* pRelayMapListBox = (MListBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("Stage_RelayMapListbox");
			if(pRelayMapListBox == NULL) return false;
			if(MAX_RELAYMAP_LIST_COUNT <= pRelayMapListBox->GetCount()) 
			{
				ZApplication::GetGameInterface()->ShowMessage( MSG_GAME_RELAYMAP_TOO_MANY_LIST_COUNT );
				return false;
			}

			RelayMapList* pMapList = (RelayMapList*)pMapListBox->GetSelItem();
			char szMapName[ MAPNAME_LENGTH ];
			strcpy(szMapName, pMapList->GetString());
			RelayMapList* pRelayMapList = new RelayMapList( szMapName, MBitmapManager::Get( "Mark_X.bmp"));

			pRelayMapListBox->Add( pRelayMapList);
			pRelayMapListBox->ShowItem(pRelayMapListBox->GetCount());	// Ω∫≈©∑— ¿ßƒ° ºº∆√

			// ∏±∑π¿Ã ∏  ºˆ¡§ Ω√¿€
			ZApplication::GetStageInterface()->SetIsRelayMapRegisterComplete(false);
			ZApplication::GetGameInterface()->EnableWidget( "Stage_RelayMap_OK_Button", true);

			return true;
		}

		// On double click
		else if ( MWidget::IsMsg( szMessage, MLB_ITEM_DBLCLK) == true)
		{
			return true;
		}

		return false;
	}
};
MMapListBoxListener g_MapListBoxListener;
MListener* ZGetMapListBoxListener( void)
{
	return &g_MapListBoxListener;
}

/***********************************************************************
MRelayMapListBoxListener

desc : ∏±∑π¿Ã∏  ∏ÆΩ∫∆Æ π⁄Ω∫ ∏ÆΩ∫≥ 
************************************************************************/
class MRelayMapListBoxListener : public MListener
{
public:
	virtual bool OnCommand(MWidget* pWidget, const char* szMessage)
	{
		// Ω∫≈◊¿Ã¡ÅE∏∂Ω∫≈Õ∞° æ∆¥œ∏ÅE∏±∑π¿Ã∏  ∏ÆΩ∫∆Æ∏¶ ƒ¡∆Æ∑— «“ºˆæ¯¥Ÿ.
		if(!ZGetGameClient()->AmIStageMaster())
			return false;
		// On select
		if ( MWidget::IsMsg( szMessage, MLB_ITEM_SEL) == true)
		{
			MListBox* pRelayMapListBox = (MListBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("Stage_RelayMapListbox");
			if(pRelayMapListBox == NULL) return false;
			if(pRelayMapListBox->GetCount() > 1)
			{	// ∏ ¿Ã √÷º“«— «—∞≥∂Ûµµ ¿÷æ˚⁄ﬂ «—¥Ÿ.
				pRelayMapListBox->Remove(pRelayMapListBox->GetSelIndex());

				// ∏±∑π¿Ã∏  ∏ÆΩ∫∆Æ Ω∫≈©∑— ¿ßƒ° ºº∆√
				if(pRelayMapListBox->GetCount() <= pRelayMapListBox->GetShowItemCount())
					pRelayMapListBox->ShowItem(0);
				else
					pRelayMapListBox->SetStartItem(pRelayMapListBox->GetCount());
				
				// ∏±∑π¿Ã ∏  ºˆ¡§ Ω√¿€
				ZApplication::GetStageInterface()->SetIsRelayMapRegisterComplete(false);
				ZApplication::GetGameInterface()->EnableWidget( "Stage_RelayMap_OK_Button", true);

				return true;
			}
			else
			{
				ZApplication::GetGameInterface()->ShowMessage(MSG_GAME_RELAYMAP_ONE_OR_MORE_MAP_SELECT);
				return false;
			}
		}

		// On double click
		else if ( MWidget::IsMsg( szMessage, MLB_ITEM_DBLCLK) == true)
		{
			return true;
		}
		return false;
	}
};
MRelayMapListBoxListener g_RelayMapListBoxListener;
MListener* ZGetRelayMapListBoxListener( void)
{
	return &g_RelayMapListBoxListener;
}

/***********************************************************************
  StartMovieOfQuest : public
  
  desc : ƒ˘Ω∫∆Æ ∏µÂ∑Œ Ω√¿€«“∂ß æ∆¿Ã≈€ «’√ƒ¡ˆ¥¬ π´∫Ò∏¶ Ω√¿€«‘
  arg  : none
  ret  : none
************************************************************************/
void ZStageInterface::StartMovieOfQuest( void)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	m_dwClockOfStartMovie = timeGetTime();

	// »≠ø∞ æ÷¥œ∏ﬁ¿Ãº« Ω√¿€
	MAnimation* pAnimation = (MAnimation*)pResource->FindWidget( "Stage_Flame0");
	if ( pAnimation && m_SacrificeItem[ SACRIFICEITEM_SLOT0].IsExist())
	{
		pAnimation->SetCurrentFrame( 0);
		pAnimation->Show( true);
		pAnimation->SetRunAnimation( true);
	}
	pAnimation = (MAnimation*)pResource->FindWidget( "Stage_Flame1");
	if ( pAnimation && m_SacrificeItem[ SACRIFICEITEM_SLOT1].IsExist())
	{
		pAnimation->SetCurrentFrame( 0);
		pAnimation->Show( true);
		pAnimation->SetRunAnimation( true);
	}

	m_bDrawStartMovieOfQuest = true;
}


/***********************************************************************
  OnDrawStartMovieOfQuest : public
  
  desc : ƒ˘Ω∫∆Æ ∏µÂ∑Œ Ω√¿€«“∂ß æ∆¿Ã≈€ «’√ƒ¡ˆ¥¬ π´∫Ò∏¶ ±◊∏≤
  arg  : none
  ret  : none
************************************************************************/
void ZStageInterface::OnDrawStartMovieOfQuest( void)
{
	if ( !m_bDrawStartMovieOfQuest)
		return ;

	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	// ∞Ê∞ÅEΩ√∞£¿ª ±∏«—¥Ÿ.
	DWORD dwClock = timeGetTime() - m_dwClockOfStartMovie;

	// »Òª˝ æ∆¿Ã≈€ ∆‰¿ÃµÅEæ∆øÅE
	int nOpacity = 255 - dwClock * 0.12f;
	if ( nOpacity < 0)
		nOpacity = 0;

	MPicture* pPicture = (MPicture*)pResource->FindWidget( "Stage_SacrificeItemImage0");
	if ( pPicture && m_SacrificeItem[ SACRIFICEITEM_SLOT0].IsExist())
		pPicture->SetOpacity( nOpacity);

	pPicture = (MPicture*)pResource->FindWidget( "Stage_SacrificeItemImage1");
	if ( pPicture && m_SacrificeItem[ SACRIFICEITEM_SLOT1].IsExist())
		pPicture->SetOpacity( nOpacity);

	// ¡æ∑ÅEΩ√∞£¿œ ∞ÊøÅE°...
	if ( dwClock > 3200)
	{
		m_bDrawStartMovieOfQuest = false;

		ZMyQuestItemMap::iterator itMyQItem;

		// πˆ±◊«ÿ∞ÅE.. ƒ˘Ω∫∆Æ ∞‘¿”Ω√¿€¿ª ¥©∏£¥¬ º¯∞£ «ÿ¥ÅEƒ˘Ω∫∆Æ æ∆¿Ã≈€¿Ã ∞®º“µ» ªÛ≈¬(ø≈πŸ∏•)¿« ∞πºˆ∏¶ 
		// DBø°º≠ πﬁæ∆ø¿±‚∂ßπÆø° ƒ´ø˚‚Æ ∞®º“∏¶ «œ∏ÅE¿ﬂ∏¯µ»∞™¿Ã µ»¥Ÿ....20090318 by kammir
		// ø©±‚º≠ ΩΩ∑‘ø° ¿⁄Ω≈¿« æ∆¿Ã≈€¿Ã ø√∑¡¡Æ ¿÷¿∏∏ÅE«ÿ¥ÅEæ∆¿Ã≈€ ƒ´ø˚‚Æ ∞®º“.
		//if( ZGetGameClient()->GetUID() == m_SacrificeItem[ SACRIFICEITEM_SLOT0].GetUID() )
		//{
		//	itMyQItem = ZGetMyInfo()->GetItemList()->GetQuestItemMap().find( m_SacrificeItem[ SACRIFICEITEM_SLOT0].GetItemID() );
		//	itMyQItem->second->Decrease();
		//}
		//if( ZGetGameClient()->GetUID() == m_SacrificeItem[ SACRIFICEITEM_SLOT1].GetUID() )
		//{
		//	itMyQItem = ZGetMyInfo()->GetItemList()->GetQuestItemMap().find( m_SacrificeItem[ SACRIFICEITEM_SLOT1].GetItemID() );
		//	itMyQItem->second->Decrease();
		//}
		
		m_SacrificeItem[ SACRIFICEITEM_SLOT0].RemoveItem();
		m_SacrificeItem[ SACRIFICEITEM_SLOT1].RemoveItem();

		ZApplication::GetGameInterface()->SetState( GUNZ_GAME);
	}
}


/***********************************************************************
  IsShowStartMovieOfQuest : public
  
  desc : ƒ˘Ω∫∆Æ ∏µÂ∑Œ Ω√¿€«“∂ß æ∆¿Ã≈€ «’√ƒ¡ˆ¥¬ π´∫Ò∏¶ ∫∏ø©¡Ÿ¡ÅEø©∫Œ∏¶ ∞·¡§.
  arg  : none
  ret  : true(=Quest mode start movie) or false(=none)
************************************************************************/
bool ZStageInterface::IsShowStartMovieOfQuest( void)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	if ( m_nGameType == MMATCH_GAMETYPE_QUEST)
	{
		if ( m_SacrificeItem[ SACRIFICEITEM_SLOT0].IsExist() || m_SacrificeItem[ SACRIFICEITEM_SLOT1].IsExist())
			return true;
	}

	return false;
}


/***********************************************************************
  OnResponseDropSacrificeItemOnSlot : public
  
  desc : »Òª˝ æ∆¿Ã≈€¿Ã ø√∂Û∞¨¿ª∂ß
  arg  : none
  ret  : none
************************************************************************/
bool ZStageInterface::OnResponseDropSacrificeItemOnSlot( const int nResult, const MUID& uidRequester, const int nSlotIndex, const int nItemID )
{
#ifdef _QUEST_ITEM
	if( MOK == nResult)
	{
		MQuestItemDesc* pItemDesc = GetQuestItemDescMgr().FindQItemDesc( nItemID);
		MBitmap* pIconBitmap = ZApplication::GetGameInterface()->GetQuestItemIcon( nItemID, false);

		m_SacrificeItem[ nSlotIndex].SetSacrificeItemSlot( uidRequester, nItemID, pIconBitmap, pItemDesc->m_szQuestItemName, pItemDesc->m_nLevel);
		SerializeSacrificeItemListBox();

		UpdateSacrificeItem();
	}
	else if( ITEM_TYPE_NOT_SACRIFICE == nResult)
	{
		// »Òª˝ æ∆¿Ã≈€¿Ã æ∆¥‘.
		return false;
	}
	else if( NEED_MORE_QUEST_ITEM == nResult )
	{
		// «ˆ¡¶ ∞°¡ˆ∞ÅE¿÷¥¬ ºˆ∑Æ¿ª √ ∞˙«ÿº≠ ø√∑¡ ≥ı¿∏∑¡∞ÅE«ﬂ¿ª∞ÊøÅE
	}
	else if( MOK != nResult )
	{
		// Ω«∆–...
		return false;
	}
	else
	{
		// ¡§¿«µ«¡ÅEæ ¿∫ error...
		ASSERT( 0 );
	}

#endif

	return true;
}


/***********************************************************************
  OnResponseCallbackSacrificeItem : public
  
  desc : »Òª˝ æ∆¿Ã≈€¿Ã ≥ª∑¡∞¨¿ª∂ß
  arg  : none
  ret  : none
************************************************************************/
bool ZStageInterface::OnResponseCallbackSacrificeItem( const int nResult, const MUID& uidRequester, const int nSlotIndex, const int nItemID )
{
#ifdef _QUEST_ITEM
	if( MOK == nResult )
	{
		m_SacrificeItem[ nSlotIndex].RemoveItem();
		SerializeSacrificeItemListBox();

		UpdateSacrificeItem();

		MTextArea* pDesc = (MTextArea*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Stage_ItemDesc");
		char szText[256];
		sprintf(szText, ZMsg( MSG_QUESTITEM_USE_DESCRIPTION ));
		pDesc->SetText(szText);
		//if ( pDesc)
		//	pDesc->Clear();
	}
	else if( ERR_SACRIFICE_ITEM_INFO == nResult )
	{
		// ≈¨∂Û¿Ãæ∆Æø°º≠ ∫∏≥Ω ¡§∫∏∞° ¿ﬂ∏¯µ» ¡§∫∏. µ˚∑Ø ø°∑Ø√≥∏Æ∞° « ø‰«œ∏ÅEø©±‚º≠ «ÿ¡÷∏ÅEµ .
	}

#endif

	return true;
}

#ifdef _QUEST_ITEM
///
// Fist : √ﬂ±≥º∫.
// Last : √ﬂ±≥º∫.
// 
// º≠πˆ∑Œ∫Œ≈Õ QL¿« ¡§∫∏∏¶ πﬁ¿Ω.
///
bool ZStageInterface::OnResponseQL( const int nQL )
{
	ZGetQuest()->GetGameInfo()->SetQuestLevel( nQL);

	// Ω∫≈◊¿Ã¡ÅEøµø™ø° ¿÷¥¬ ƒ˘Ω∫∆Æ ∑π∫ß «•Ω√ ºˆ¡§
	MLabel* pLabel = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Stage_QuestLevel");
	if ( pLabel)
	{
		char szText[125];
		sprintf( szText, "%s %s : %d", ZMsg( MSG_WORD_QUEST), ZMsg( MSG_CHARINFO_LEVEL), nQL);
		pLabel->SetText( szText);
	}

	return true;
}

bool ZStageInterface::OnStageGameInfo( const int nQL, const int nMapsetID, const unsigned int nScenarioID )
{
	if (nScenarioID != 0)
	{
		ZGetQuest()->GetGameInfo()->SetQuestLevel( nQL );
	}
	else
	{
		// Ω√≥™∏Æø¿∞° æ¯¿∏∏ÅE±◊≥… 0¿∏∑Œ ∫∏¿Ã∞‘ «—¥Ÿ.
		ZGetQuest()->GetGameInfo()->SetQuestLevel( 0 );
	}

	ZGetQuest()->GetGameInfo()->SetMapsetID( nMapsetID );
	ZGetQuest()->GetGameInfo()->SetSenarioID( nScenarioID );

	UpdateStageGameInfo(nQL, nMapsetID, nScenarioID);

	return true;
}

bool ZStageInterface::OnResponseSacrificeSlotInfo( const MUID& uidOwner1, const unsigned long int nItemID1, 
												   const MUID& uidOwner2, const unsigned long int nItemID2 )
{
	if ( (uidOwner1 != MUID(0,0)) && nItemID1)
	{
		MQuestItemDesc* pItemDesc = GetQuestItemDescMgr().FindQItemDesc( nItemID1);
		MBitmap* pIconBitmap = ZApplication::GetGameInterface()->GetQuestItemIcon( nItemID1, false);
		m_SacrificeItem[ SACRIFICEITEM_SLOT0].SetSacrificeItemSlot( uidOwner1, nItemID1, pIconBitmap, pItemDesc->m_szQuestItemName, pItemDesc->m_nLevel);
	}
	else
		m_SacrificeItem[ SACRIFICEITEM_SLOT0].RemoveItem();

	if ( (uidOwner2 != MUID(0,0)) && nItemID2)
	{
		MQuestItemDesc* pItemDesc = GetQuestItemDescMgr().FindQItemDesc( nItemID2);
		MBitmap* pIconBitmap = ZApplication::GetGameInterface()->GetQuestItemIcon( nItemID2, false);
		m_SacrificeItem[ SACRIFICEITEM_SLOT1].SetSacrificeItemSlot( uidOwner2, nItemID2, pIconBitmap, pItemDesc->m_szQuestItemName, pItemDesc->m_nLevel);
	}
	else
		m_SacrificeItem[ SACRIFICEITEM_SLOT1].RemoveItem();

	UpdateSacrificeItem();

	MTextArea* pDesc = (MTextArea*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Stage_ItemDesc");
	//if ( pDesc)
	//	pDesc->Clear();

	return true;
}


bool ZStageInterface::OnQuestStartFailed( const int nState )
{
	MTextArea* pTextArea = (MTextArea*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "StageChattingOutput");
	if ( pTextArea)
	{
		char text[256];
		sprintf(text, "%s", ZMsg(MSG_GANE_NO_QUEST_SCENARIO));
		pTextArea->AddText( text);
	}

/*
	if( MSQITRES_INV == nState )
	{
		// «ÿ¥ÅEQLø°¥ÅE— »Òª˝æ∆¿Ã≈€ ¡§∫∏ ≈◊¿Ã∫˙‹Ã æ¯¿Ω. ¿Ã∞ÊøÅE¬ ∏¬¡ÅEæ ¥¬ »Òª˝ æ∆¿Ã≈€¿Ã ø√∑¡¡Æ ¿÷¿ª∞ÊøÅE
		MTextArea* pTextArea = (MTextArea*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "StageChattingOutput");
		if ( pTextArea)
			pTextArea->AddText( "^1«ˆ¿ÅE≥ıø©¿÷¥¬ æ∆¿Ã≈€¿∫ ¡∂∞«ø° ∏¬¡ÅEæ æ∆ ∞‘¿”¿ª Ω√¿€«“ ºÅEæ¯Ω¿¥œ¥Ÿ.");
	}
	else if( MSQITRES_DUP == nState )
	{
		// æÁ¬  ΩΩ∑‘ø° ∞∞¿∫ æ∆¿Ã≈€¿Ã ø√∑¡¡Æ ¿÷¿Ω.
		MTextArea* pTextArea = (MTextArea*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "StageChattingOutput");
		if ( pTextArea)
			pTextArea->AddText( "^1∞∞¿∫ æ∆¿Ã≈€ 2∞≥∞° ≥ıø©¿÷¿∏π«∑Œ ∞‘¿”¿ª Ω√¿€«“ ºÅEæ¯Ω¿¥œ¥Ÿ.");
	}
*/
	return true;
}


bool ZStageInterface::OnNotAllReady()
{
	return true;
}
#endif

void ZStageInterface::UpdateStageGameInfo(const int nQL, const int nMapsetID, const int nScenarioID)
{
	if (!ZGetGameTypeManager()->IsQuestOnly(ZGetGameClient()->GetMatchStageSetting()->GetGameType())) return;

	// Ω∫≈◊¿Ã¡ÅEøµø™ø° ¿÷¥¬ ƒ˘Ω∫∆Æ ∑π∫ß «•Ω√ ºˆ¡§
	MLabel* pLabel = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Stage_QuestLevel");
	if ( pLabel)
	{
		char szText[125];
		sprintf( szText, "%s %s : %d", ZMsg( MSG_WORD_QUEST), ZMsg( MSG_CHARINFO_LEVEL), nQL);
		pLabel->SetText( szText);
	}


#define		MAPSET_NORMAL		MCOLOR(0xFFFFFFFF)
#define		MAPSET_SPECIAL		MCOLOR(0xFFFFFF40)			// Green
//#define		MAPSET_SPECIAL		MCOLOR(0xFFFF2020)		// Red

	// ø©±‚º≠ Ω√≥™∏Æø¿ ¿Ã∏ß¿ª ∫∏ø©¡ÿ¥Ÿ.
	pLabel = (MLabel*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Stage_SenarioName");
	MWidget* pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Stage_SenarioNameImg");
	MPicture* pPictureL = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Stage_Lights0");
	MPicture* pPictureR = (MPicture*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget( "Stage_Lights1");
	if ( pLabel)
	{
		if (nScenarioID == 0)
		{
			// Ω√≥™∏Æø¿∞° æ¯¥¬ ∞ÊøÅE
			pLabel->SetText( "");
			if ( pWidget)
				pWidget->Show( false);
			if ( pPictureL) {
				pPictureL->Show( false);
			}
			if ( pPictureR) {
				pPictureR->Show( false);
			}
		}
		else
		{
			pLabel->SetAlignment( MAM_HCENTER | MAM_VCENTER);

			LIST_SCENARIONAME::iterator itr =  m_SenarioDesc.find( nScenarioID);
			if ( itr != m_SenarioDesc.end())
			{
				pLabel->SetText( (*itr).second.m_szName);
				pLabel->SetTextColor(MCOLOR(0xFFFFFF00));
				if ( pWidget)
					pWidget->Show( true);

				if ( pPictureL) {
					pPictureL->Show( true);
					pPictureL->SetBitmapColor(MAPSET_SPECIAL);
				}
				if ( pPictureR) {
					pPictureR->Show( true);
					pPictureR->SetBitmapColor(MAPSET_SPECIAL);
				}
			}
			else
			{
				// ∆Ø∫∞Ω√≥™∏Æø¿∞° æ¯¿ª∞ÊøÅE¬ ¡§±‘Ω√≥™∏Æø¿¿Ã¥Ÿ.
				pLabel->SetText("");
				pLabel->SetTextColor(MCOLOR(0xFFFFFFFF));
				if ( pWidget)
					pWidget->Show( false);

				if ( pPictureL) {
					pPictureL->Show( true);
					pPictureL->SetBitmapColor(MAPSET_NORMAL);
				}
				if ( pPictureR) {
					pPictureR->Show( true);
					pPictureR->SetBitmapColor(MAPSET_NORMAL);
				}

			}
		}
	}
}

/***********************************************************************
  SetSacrificeItemSlot : public
  
  desc : »Òª˝ æ∆¿Ã≈€ ΩΩ∑‘ø° æ∆¿Ã≈€ ¡§∫∏∏¶ ¿‘∑¬
  arg  : none
  ret  : none
************************************************************************/
void SacrificeItemSlotDesc::SetSacrificeItemSlot( const MUID& uidUserID, const unsigned long int nItemID, MBitmap* pBitmap, const char* szItemName, const int nQL)
{
	m_uidUserID = uidUserID;
	m_nItemID = nItemID;
	m_pIconBitmap = pBitmap;
	strcpy( m_szItemName, szItemName);
	m_nQL = nQL;
	m_bExist = true;
}


/***********************************************************************
  ReadSenarioNameXML : protected
  
  desc : ƒ˘Ω∫∆Æ »Òª˝ æ∆¿Ã≈€ XML¿ª ¿–¥¬¥Ÿ
  arg  : none
  ret  : true(=success) or false(=fail)
************************************************************************/
bool ZStageInterface::ReadSenarioNameXML( void)
{
	if ( (int)m_SenarioDesc.size())
		return true;


	// XML ∆ƒ¿œ¿ª ø¨¥Ÿ
	MXmlDocument xmlQuestItemDesc;
	xmlQuestItemDesc.Create();

	char			*buffer;
	MZFile			mzFile;

	string strFileScenario("System/scenario.xml");
#ifndef _DEBUG
	strFileScenario += ""; // MEF NULL
#endif
	if( !mzFile.Open(strFileScenario.c_str(), ZApplication::GetFileSystem())) 
	{
		mlog("Error while read : %s", strFileScenario.c_str());
		xmlQuestItemDesc.Destroy();
		return false;
	} 

	buffer = new char[ mzFile.GetLength() + 1];
	buffer[ mzFile.GetLength()] = 0;

	mzFile.Read( buffer, mzFile.GetLength());

	if( !xmlQuestItemDesc.LoadFromMemory( buffer))
	{
		delete[] buffer;
		xmlQuestItemDesc.Destroy();
		return false;
	}
	delete[] buffer;
	mzFile.Close();

	int nStdScenarioCount = 1000;

	// µ•¿Ã≈Õ∏¶ ¿–æ˚€¬¥Ÿ
	MXmlElement rootElement = xmlQuestItemDesc.GetDocumentElement();
	for ( int i = 0;  i < rootElement.GetChildNodeCount();  i++)
	{
		MXmlElement chrElement = rootElement.GetChildNode( i);

		char szTagName[ 256];
		chrElement.GetTagName( szTagName);

		if ( szTagName[ 0] == '#')
			continue;


		char szAttrName[64];
		char szAttrValue[256];
		int nItemID = 0;
		MSenarioList SenarioMapList;

		bool bFind = false;


		// ¡§±‘ Ω√≥™∏Æø¿
		if ( !stricmp( szTagName, "STANDARD_SCENARIO"))			// ≈¬±◊ Ω√¿€
		{
			// Set Tag
			for ( int k = 0;  k < chrElement.GetAttributeCount();  k++)
			{
				chrElement.GetAttribute( k, szAttrName, szAttrValue);

				SenarioMapList.m_ScenarioType = ST_STANDARD;	// Type

				if ( !stricmp( szAttrName, "title"))			// Title
					strcpy( SenarioMapList.m_szName, szAttrValue);

				else if ( !stricmp( szAttrName, "mapset"))		// Map set
					strcpy( SenarioMapList.m_szMapSet, szAttrValue);
			}

			nItemID = nStdScenarioCount++;					// ID

			bFind = true;
		}

		// ∆ØºÅEΩ√≥™∏Æø¿
		else if ( !stricmp( szTagName, "SPECIAL_SCENARIO"))			// ≈¬±◊ Ω√¿€
		{
			// Set Tag
			for ( int k = 0;  k < chrElement.GetAttributeCount();  k++)
			{
				chrElement.GetAttribute( k, szAttrName, szAttrValue);

				SenarioMapList.m_ScenarioType = ST_SPECIAL;		// Type

				if ( !stricmp( szAttrName, "id"))				// ID
					nItemID = atoi( szAttrValue);

				else if ( !stricmp( szAttrName, "title"))		// Title
					strcpy( SenarioMapList.m_szName, szAttrValue);

				else if ( !stricmp( szAttrName, "mapset"))		// Map set
					strcpy( SenarioMapList.m_szMapSet, szAttrValue);
			}

			bFind = true;
		}


		if ( bFind)
			m_SenarioDesc.insert( LIST_SCENARIONAME::value_type( nItemID, SenarioMapList));
	}

	xmlQuestItemDesc.Destroy();


	return true;
}

bool ZStageInterface::OnStopVote()
{
	ZGetGameClient()->SetVoteInProgress( false );
	ZGetGameClient()->SetCanVote( false );

#ifdef _DEBUG
	string str = ZMsg(MSG_VOTE_VOTE_STOP);
#endif

	ZChatOutput(ZMsg(MSG_VOTE_VOTE_STOP), ZChat::CMT_SYSTEM, ZChat::CL_CURRENT);
	return true;
}


void ZStageInterface::OnStartFail( const int nType, const MUID& uidParam )
{
	if( ALL_PLAYER_NOT_READY == nType )
	{
		// ∏µÅE¿Ø¿˙∞° ∑πµ∏¶ «œ¡ÅEæ æ“¿Ω.
		ZGetGameInterface()->PlayVoiceSound( VOICE_PLAYER_NOT_READY, 1500);
	}
	else if( QUEST_START_FAILED_BY_SACRIFICE_SLOT == nType )
	{
		OnQuestStartFailed( uidParam.Low );

		ZGetGameInterface()->PlayVoiceSound( VOICE_QUEST_START_FAIL, 2800);
	}
	else if( INVALID_TACKET_USER == nType )
	{
		// ¿⁄±ÅE¿⁄Ω≈¿Ã∏ÅE-_-;;
		//if( uidParam == ZGetMyUID() )
		//{
			char szMsg[ 128 ];

			sprintf( szMsg, MGetStringResManager()->GetErrorStr(MERR_CANNOT_START_NEED_TICKET), ZGetGameClient()->GetObjName(uidParam).c_str() );

			ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
			ZChatOutput(szMsg, ZChat::CMT_BROADCAST);
		//}

		// ¿‘¿Â±« ¥©∞° æ¯¥¬∞≈æﬂ? -_-+ - by SungE
		//const MMatchPeerInfo* pPeer = ZGetGameClient()->GetPeers()->Find( uidParam );
		//if( 0 != pPeer )
		//{
		//	char szMsg[ 128 ];

		//	sprintf( szMsg, MGetStringResManager()->GetErrorStr(MERR_CANNOT_START_NEED_TICKET), pPeer->CharInfo.szName );

		//	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
		//	ZChatOutput(szMsg, ZChat::CMT_BROADCAST);
		//}
	}
	else if( INVALID_MAP == nType )
	{
		char szMsg[ 128 ];

			sprintf( szMsg, "INVALID MAP!" );
				//MGetStringResManager()->GetErrorStr(MERR_CANNOT_START_NEED_TICKET), 
				//ZApplication::GetInstance()->GetGameClient()->GetObjName(uidParam).c_str() );

			ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
			ZChatOutput(szMsg, ZChat::CMT_BROADCAST);
	}

	// Stage UI Enable
	ChangeStageEnableReady( false);
}

////////////////////////////////// Spy Mode. //////////////////////////////////
class MSpyBanMapListListener : public MListener
{
public:
	virtual bool OnCommand(MWidget* pWidget, const char* szMessage)
	{
		// Ω∫≈◊¿Ã¡ÅE∏∂Ω∫≈Õ∞° æ∆¥œ∏ÅE∏  ∏ÆΩ∫∆Æ∏¶ ƒ¡∆Æ∑— «“ºˆæ¯¥Ÿ.
		if (!ZGetGameClient()->AmIStageMaster())
			return false;
		// On select
		if (MWidget::IsMsg(szMessage, MLB_ITEM_SEL) == true)
		{
			MListBox* pListBox = (MListBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("Stage_SpyBanMapList");
			if (!pListBox) return false;

			SpyBanMapList* pSpyBanMapList = (SpyBanMapList*)pListBox->GetSelItem();
			if (!MMatchSpyMap::IsExistingMap(pSpyBanMapList->GetMapID())) return false;

			ZPostActivateSpyMap(pSpyBanMapList->GetMapID(), ZGetGameClient()->GetMatchStageSetting()->IsIncludedSpyMap(pSpyBanMapList->GetMapID()));
			return true;
		}

		return false;
	}
};
MSpyBanMapListListener g_SpyBanMapListListener;
MListener* ZGetSpyBanMapListListener(void)
{
	return &g_SpyBanMapListListener;
}

///////////////////////////////////////////////////////

void ZStageInterface::OpenSpyBanMapBox()
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	MButton* pButton = (MButton*)pResource->FindWidget("Stage_SpyBanMapBoxOpen");
	if (pButton)
		pButton->Show(false);
	pButton = (MButton*)pResource->FindWidget("Stage_SpyBanMapBoxClose");
	if (pButton)
		pButton->Show(true);

	SetSpyBanMapBoxPos(2);

	CreateSpyBanMapList();
}

void ZStageInterface::CloseSpyBanMapBox()
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	MButton* pButton = (MButton*)pResource->FindWidget("Stage_SpyBanMapBoxClose");
	if (pButton)
		pButton->Show(false);
	pButton = (MButton*)pResource->FindWidget("Stage_SpyBanMapBoxOpen");
	if (pButton)
		pButton->Show(true);

	MWidget* pWidget = pResource->FindWidget("Stage_CharacterInfo");
	if (pWidget)
		pWidget->Enable(true);

	SetSpyBanMapBoxPos(1);
}

void ZStageInterface::HideSpyBanMapBox()
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	MButton* pButton = (MButton*)pResource->FindWidget("Stage_SpyBanMapBoxClose");
	if (pButton)
		pButton->Show(false);
	pButton = (MButton*)pResource->FindWidget("Stage_SpyBanMapBoxOpen");
	if (pButton)
		pButton->Show(true);

	MWidget* pWidget = pResource->FindWidget("Stage_CharacterInfo");
	if (pWidget)
		pWidget->Enable(true);

	SetSpyBanMapBoxPos(0);
}

void ZStageInterface::SetSpyBanMapBoxPos(int nBoxPos)
{
	MWidget* pWidget = ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("Stage_SpyBanMapListView");
	if (pWidget)
	{
		MRECT rect;

		switch (nBoxPos)
		{
		case 0:		// Hide
			rect = pWidget->GetRect();
			m_nSpyBanMapListFramePos = -rect.w;
			break;

		case 1:		// Close
			rect = pWidget->GetRect();
			m_nSpyBanMapListFramePos = -rect.w + (rect.w * 0.14);
			break;

		case 2:		// Open
			m_nSpyBanMapListFramePos = 0;
			break;
		}
	}
}

void ZStageInterface::CreateSpyBanMapList()
{
	// ∏  ∏ÆΩ∫∆Æ ∏∏µÈæÅE¡÷±ÅE
	MListBox* pMapListBox = (MListBox*)ZGetGameInterface()->GetIDLResource()->FindWidget("Stage_SpyBanMapList");
	if (pMapListBox == NULL) return;

	pMapListBox->RemoveAll();	// ±‚¡∏ ∏±∑π¿Ã∏  ∏ÆΩ∫∆Æ∏¶ ∏µŒ ¡ˆøˆ¡ÿ¥Ÿ.
	int nCurrPlayers = (int)ZGetGameClient()->GetMatchStageSetting()->m_CharSettingList.size();

	for (int i = MMATCH_SPY_MAP_MANSION; i < MMATCH_SPY_MAP_MAX; i++)
	{
		char szListName[256];
		sprintf(szListName, "(%d~%d%s) %s", g_SpyMapNode[i].nMinPlayers, g_SpyMapNode[i].nMaxPlayers, ZMsg(MSG_SPY_WORD_PLAYERS), g_SpyMapNode[i].szName);

		SpyBanMapList* pSpyBanMapList = new SpyBanMapList(szListName,
			MBitmapManager::Get(
				ZGetGameClient()->GetMatchStageSetting()->IsIncludedSpyMap(g_SpyMapNode[i].nID) ?
				"Mark_Arrow.bmp" : "Mark_X.bmp"),
			g_SpyMapNode[i].nID);

		if (nCurrPlayers < g_SpyMapNode[i].nMinPlayers || nCurrPlayers > g_SpyMapNode[i].nMaxPlayers)
		{
			pSpyBanMapList->SetColor(MCOLOR(0xFFFF0000));	// RED color.
		}

		pMapListBox->Add(pSpyBanMapList);
	}
}

bool ZStageInterface::GetPlayableSpyMapList(int players, vector<int>& out)
{
	int nCurrPlayers = (int)ZGetGameClient()->GetMatchStageSetting()->m_CharSettingList.size();

	for (int i = MMATCH_SPY_MAP_MANSION; i < MMATCH_SPY_MAP_MAX; i++)
	{
		if (ZGetGameClient()->GetMatchStageSetting()->IsIncludedSpyMap(g_SpyMapNode[i].nID))
		{
#if !defined(_ARTIC_DEBUG)
			if (nCurrPlayers >= g_SpyMapNode[i].nMinPlayers && nCurrPlayers <= g_SpyMapNode[i].nMaxPlayers)
#endif
			{
				out.push_back(g_SpyMapNode[i].nID);
			}
		}
	}

	return !out.empty();
}

void ZStageInterface::OnSpyStageActivateMap(int nMapID, bool bExclude)
{
	MMatchStageSetting* pSetting = ZGetGameClient()->GetMatchStageSetting();

	if (bExclude)
		pSetting->ExcludeSpyMap(nMapID);
	else
		pSetting->IncludeSpyMap(nMapID);

	CreateSpyBanMapList();	// Refresh.
}

void ZStageInterface::OnSpyStageBanMapList(void* pMapListBlob)
{
	MMatchStageSetting* pSetting = ZGetGameClient()->GetMatchStageSetting();
	pSetting->UnbanAllSpyMap();

	int nCount = MGetBlobArrayCount(pMapListBlob);

	for (int i = 0; i < nCount; i++)
		pSetting->ExcludeSpyMap(*(int*)MGetBlobArrayElement(pMapListBlob, i));

	CreateSpyBanMapList();
}
///////////////////////////////////////////////////////////////////////////////

