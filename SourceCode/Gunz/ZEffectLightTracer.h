#pragma once

#include "GlobalTypes.h"
#include "ZEffectBillboard.h"
#include "mempool.h"

class ZEffectLightTracer : public ZEffectBillboard, public CMemPoolSm<ZEffectLightTracer>
{
protected:
	u64 m_nStartTime;

	D3DXVECTOR3	m_LightTracerDir;
	D3DXVECTOR3	m_Start, m_End;
	float		m_fLength;

public:
	ZEffectLightTracer(ZEffectBillboardSource* pEffectBillboardSource, D3DXVECTOR3& Start, D3DXVECTOR3& End);
	virtual ~ZEffectLightTracer(void);

	virtual bool Draw(u64 nTime) override;
};