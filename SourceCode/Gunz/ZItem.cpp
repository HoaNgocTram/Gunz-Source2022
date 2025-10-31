#include "stdafx.h"

#include "ZItem.h"
#include "crtdbg.h"

//////////////////////////////////////////////////////////////////////////////
// ZItem /////////////////////////////////////////////////////////////////////
ZItem::ZItem() : MMatchItem()
{
	m_nBulletSpare.Set_MakeCrc(0);
	m_nBulletCurrMagazine.Set_MakeCrc(0);
}

ZItem::~ZItem()
{

}

void ZItem::InitBullet(int nBulletSpare, int nBulletCurrMagazine)
{
	if(m_pDesc == NULL) return;
	if(!m_pDesc->IsSpendableItem() &&
		nBulletSpare < nBulletCurrMagazine)
	{
		nBulletCurrMagazine = nBulletSpare;
	}

	if(GetItemType() == MMIT_RANGE) 
	{		
		m_nBulletCurrMagazine.Set_CheckCrc( nBulletCurrMagazine );
		m_nBulletSpare.Set_CheckCrc( nBulletSpare - nBulletCurrMagazine );
	}
	else if(GetItemType() == MMIT_CUSTOM) 
	{
		m_nBulletCurrMagazine.Set_CheckCrc( nBulletCurrMagazine );
		m_nBulletSpare.Set_CheckCrc( nBulletSpare );
	}
}

bool ZItem::Shot()
{
	if (m_pDesc == NULL) {
		//		_ASSERT(0);
		return false;
	}

	_ASSERT((GetItemType() == MMIT_RANGE) ||
		(GetItemType() == MMIT_MELEE) ||
		(GetItemType() == MMIT_CUSTOM));//ｼｺｷ・.

	if (GetItemType() == MMIT_MELEE)		///< meleeﾀﾌｸ・ｱﾗｳﾉ true
	{
		return true;
	}
	else if (GetItemType() == MMIT_CUSTOM)	///< ｼｺﾀﾌｳｪ ｱ簀ｸ~ｱｸｺﾐ~
	{
		if (m_nBulletCurrMagazine.Ref() > 0)
		{
			m_nBulletCurrMagazine.CheckCrc();
			m_nBulletCurrMagazine.Ref() -= 1;
			m_nBulletCurrMagazine.MakeCrc();
		}
		else return false;
	}
	else
	{
		//if(ZGetGame()->GetMatch()->IsTurboMode() && ZGetGameClient()->GetMatchStageSetting()->IsModifierUsed( MMOD_INFINITE_AMMO ) && !ZGetGameClient()->GetMatchStageSetting()->IsQuestDrived() )
		//	return true;

		if (m_nBulletCurrMagazine.Ref() > 0)
		{
			m_nBulletCurrMagazine.CheckCrc();
			m_nBulletCurrMagazine.Ref() -= 1;
			m_nBulletCurrMagazine.MakeCrc();
		}
		else return false;
	}

	return true;
}

bool ZItem::Reload()
{
	if (m_pDesc == NULL)
	{
		_ASSERT(0);
		return false;
	}
	_ASSERT(GetItemType() == MMIT_RANGE);

	if(ZGetGame()->GetMatch()->IsTurboMode() && ZGetGameClient()->GetMatchStageSetting()->IsModifierUsed( MMOD_INFINITE_AMMO ) && !ZGetGameClient()->GetMatchStageSetting()->IsQuestDrived()
		&& ZGetGameClient()->GetMatchStageSetting()->GetGameType() != MMATCH_GAMETYPE_QUEST_CHALLENGE)
	{
		m_nBulletCurrMagazine.Set_CheckCrc(m_pDesc->m_nMagazine.Ref());
		return true;
	}

 	int nAddedBullet = m_pDesc->m_nMagazine.Ref() - m_nBulletCurrMagazine.Ref();
	if (nAddedBullet > m_nBulletSpare.Ref()) nAddedBullet = m_nBulletSpare.Ref();
	if (nAddedBullet <= 0) return false;

	m_nBulletCurrMagazine.Set_CheckCrc(m_nBulletCurrMagazine.Ref() + nAddedBullet);
	m_nBulletSpare.Set_CheckCrc(m_nBulletSpare.Ref() - nAddedBullet);

	return true;
}

bool ZItem::isReloadable()
{
	if (m_pDesc == NULL) {
		_ASSERT(0);
		return false;
	}

	if( GetItemType() == MMIT_CUSTOM ) { // ｱ簀ｸ ｹｫｱ箏鯊ｺ ﾀ鄲蠡・ﾌ ｾﾙ~
		return false;
	}

	if(m_nBulletSpare.Ref() > 0) {
		if(m_pDesc->m_nMagazine.Ref() != m_nBulletCurrMagazine.Ref())
			return true;
	}

	return false;
}

//////////////////////////////////////////////////
// HP ｿﾍ AP ｵ･ｹﾌﾁ・ｺｲ ｰ霆・ｰﾄ~
float ZItem::GetPiercingRatio(MMatchWeaponType wtype, RMeshPartsType partstype)
{
	float fRatio = 0.5f;

	bool bHead = false;

	if(partstype == eq_parts_head) { // ﾇ・蠑ｦ ｱｸｺﾐ~ 
		bHead = true;
	}

	switch(wtype) {
		case MWT_DAGGER:		// ｴﾜｰﾋ
		case MWT_DUAL_DAGGER:	// ｾ郛ﾕｴﾜｰﾋ
			{
				if(bHead)	fRatio = 0.75f;
				else		fRatio = 0.7f;
			}
			break;

		case MWT_KATANA:		// ﾄｫﾅｸｳｪ
		case MWT_SPYCASE:
			{
				if(bHead)	fRatio = 0.65f;
				else		fRatio = 0.6f;
			}
			break;

		case MWT_DOUBLE_KATANA:
			{
				if(bHead)	fRatio = 0.65f;
				else		fRatio = 0.6f;
			}
			break;

		case MWT_GREAT_SWORD:	
			{
				if(bHead)	fRatio = 0.65f;
				else		fRatio = 0.6f;
			}
			break;

		case MWT_PISTOL:
		case MWT_PISTOLx2:
			{
				if(bHead)	fRatio = 0.7f;
				else		fRatio = 0.5f;
			}
			break;

		case MWT_REVOLVER:
		case MWT_REVOLVERx2:
			{
				// Custom: Reduced Piercing Ratios of Revolvers
				if(bHead)	fRatio = 0.7f;
				else		fRatio = 0.5f;

				//if(bHead)	fRatio = 0.9f;
				//else		fRatio = 0.7f;
			}
			break;

		case MWT_SMG:
		case MWT_SMGx2:			// ｼｭｺ・ﾓｽﾅｰﾇ
			{
				if(bHead)	fRatio = 0.6f;
				else		fRatio = 0.3f;
			}
			break;

		case MWT_SHOTGUN:	
		case MWT_SAWED_SHOTGUN:
			{
				if(bHead)	fRatio = 0.2f;
				else		fRatio = 0.2f;
			}
			break;

		case MWT_MACHINEGUN:	// ｸﾓｽﾅｰﾇ
			{
				if(bHead)	fRatio = 0.6f;
				else		fRatio = 0.3f;
			}
			break;

		case MWT_RIFLE:			// ｵｹｰﾝｼﾒﾃﾑ
			{
				if(bHead)	fRatio = 0.6f;
				else		fRatio = 0.3f;
			}
			break;

		case MWT_SNIFER:		//ｿ・ｱﾀｺ ｶﾌﾇﾃﾃｳｷｳ...
			{
				if(bHead)	fRatio = 0.8f;
				else		fRatio = 0.4f;
			}
			break;

		case MWT_FRAGMENTATION:
		case MWT_FLASH_BANG:
		case MWT_SMOKE_GRENADE:
		case MWT_FLASH_BANG_SPY:
		case MWT_SMOKE_GRENADE_SPY:
		case MWT_ROCKET:		// ｷﾎﾄﾏｷｱﾃﾄ
			{
				if(bHead)	fRatio = 0.4f;
				else		fRatio = 0.4f;
			}
			break;

		case MWT_TRAP:
		case MWT_TRAP_SPY:
			{
				// ﾆｮｷｦﾀｺ fRatioｰ｡ ﾀﾇｹﾌｰ｡ ｾ・ ﾆｮｷｦﾀﾌ ｰ｡ﾁ・ﾀﾎﾃｦﾆｮ ｼﾓｼｺﾀﾇ ｰ・・・ｻ ｵ鄕･ｴﾙ
				fRatio = 0;
			}
			break;

		case MWT_DYNAMITYE:
			{
				fRatio = 0.4f;
			}
			break;
		case MWT_LANDMINE:
			{
				fRatio = 0.f;
			}
			break;

		default:
			break;
	}

	return fRatio;
}

float ZItem::GetKnockbackForce()
{
	float fKnockbackForce = 0.0f;

	MMatchItemDesc* pDesc = GetDesc();
	if(pDesc) {
		if (pDesc->m_nType.Ref() == MMIT_MELEE) {
			fKnockbackForce = 200.0f;
		}
		else 
			if (pDesc->m_nType.Ref() == MMIT_RANGE) {
				if (pDesc->m_pEffect != NULL)
					fKnockbackForce = (float)(pDesc->m_pEffect->m_nKnockBack);
				else
					fKnockbackForce = 200.0f;
			}
	}
	else {
		_ASSERT(0);
		return 0;
	}

	return fKnockbackForce;
}

void ZItem::ShiftFugitiveValues()
{
	m_nBulletSpare.ShiftHeapPos_CheckCrc();
	m_nBulletCurrMagazine.ShiftHeapPos_CheckCrc();
}