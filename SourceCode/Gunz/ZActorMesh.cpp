#include "stdafx.h"
#include "ZActorMesh.h"

//void ZActorMesh::InitNpcMesh(ZActorBase* pActor, const char* szMeshName)
//{
//	RMesh* pMesh;
//	pMesh = ZGetNpcMeshMgr()->Get((char*)szMeshName);
//	if (!pMesh)
//	{
//		mlog("Npc Mesh Not Found.\n");
//		return;
//	}
//	else
//	{
//		int n_NPCMesh = ZGetGame()->m_VisualMeshMgr.Add(pMesh);
//		pActor->SetVisualMesh(ZGetGame()->m_VisualMeshMgr.GetFast(n_NPCMesh));
//	}
//}