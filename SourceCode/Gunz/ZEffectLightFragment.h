#ifndef ZEffectLightFragment_H
#define ZEffectLightFragment_H

#include "ZEffectBillboard.h"
#include "RTypes.h"

#include "mempool.h"
// 사용안하는것들은 지운다..
class ZEffectLightFragment : public ZEffectBillboard, public CMemPoolSm<ZEffectLightFragment>
{
protected:
	u64 m_nStartTime;
	u64 m_nPrevTime;

	rvector	m_OrigPos;
	rvector	m_Velocity;

public:
	ZEffectLightFragment(ZEffectBillboardSource* pEffectBillboardSource, rvector& Pos, rvector& Velocity);
	virtual ~ZEffectLightFragment(void);

	virtual bool Draw(u64 nTime) override;
};

class ZEffectLightFragment2 : public ZEffectBillboard2, public CMemPoolSm<ZEffectLightFragment2>
{
protected:
	u64 m_nStartTime;
	u64 m_nPrevTime;


	rvector	m_OrigPos;
	rvector	m_Velocity;

public:
	ZEffectLightFragment2(LPDIRECT3DTEXTURE9 pEffectBillboardTexture, rvector& Pos, rvector& Velocity);
	virtual ~ZEffectLightFragment2(void);

	virtual bool Draw(u64 nTime) override;
};

#endif