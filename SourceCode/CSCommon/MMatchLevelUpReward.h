#pragma once

#include "stdafx.h"
#include <map>
#include "MMatchDBMgr.h" 

///////////////////////////////////////////////////////////////////////////////////
//
//		MMatchReward Map
//
///////////////////////////////////////////////////////////////////////////////////

class MMatchRewardMap
{
protected:
	int m_nLevel;
	int m_nBounty;
	int m_nCash;
	int m_nItemID;
public:
	void SetLevel(int i) { m_nLevel = i; }
	void SetBounty(int i) { m_nBounty = i; }
	void SetCash(int i) { m_nCash = i; }
	void SetItemID(int i) { m_nItemID = i; }

	int GetLevel() { return m_nLevel; }
	int GetBounty() { return m_nBounty; }
	int GetCash() { return m_nCash; }
	int GetItemID() { return m_nItemID; }
};

///////////////////////////////////////////////////////////////////////////////////
//
//		MMatchReward Manager
//
///////////////////////////////////////////////////////////////////////////////////

class MMatchRewardMgr : public map<int, MMatchRewardMap*>
{

public:
	MMatchRewardMgr() { };
	~MMatchRewardMgr() { Clear(); };

	void AddReward(int nLevel, int nBounty, int nCash, int nItemID)
	{
		MMatchRewardMap* pReward = new MMatchRewardMap;

		pReward->SetLevel(nLevel);
		pReward->SetBounty(nBounty);
		pReward->SetCash(nCash);
		pReward->SetItemID(nItemID);

		if (!AddRewardMapByLevel(pReward))
		{
			mlog("ERROR: Reward Level Duplication.\n");
			delete pReward;
		}
	}

private:
	bool AddRewardMapByLevel(MMatchRewardMap* pRewardMap)
	{
		if (GetRewardMapByLevel(pRewardMap->GetLevel()))
			return false;

		insert(value_type(pRewardMap->GetLevel(), pRewardMap));
		return true;
	}

public:
	MMatchRewardMap* GetRewardMapByLevel(int nLevel)
	{
		iterator itor = find(nLevel);
		if (itor != end())
		{
			return (*itor).second;
		}
		return NULL;
	}

private:
	void Clear()
	{
		for (iterator itor = begin(); itor != end(); ++itor)
		{
			delete (*itor).second;
		}
		clear();
	}
};

///////////////////////////////////////////////////////////////////////////////////
//
//		MMatchLevelUp Manager
//
///////////////////////////////////////////////////////////////////////////////////

class MMatchLevelUpReward : public map<int, MMatchRewardMap*>
{
private:
	MMatchDBMgr	m_MatchDBMgr;
public:
	////////////////////////////////////////////////
	MMatchLevelUpReward()
	{

	}

	////////////////////////////////////////////////
	~MMatchLevelUpReward()
	{
		
	}

private:
	////////////////////////////////////////////////
	#define MTOK_LEVELUPREWARD_REWARD			"REWARD"

	#define MTOK_LEVELUPREWARD_ATTR_LEVEL		"level"
	#define MTOK_LEVELUPREWARD_ATTR_BOUNTY		"bounty"
	#define MTOK_LEVELUPREWARD_ATTR_CASH		"cash"
	#define MTOK_LEVELUPREWARD_ATTR_ITEMID		"itemid"

	////////////////////////////////////////////////
	static bool ReadXml(MZFileSystem* pFileSystem, const char* szFileName)
	{
		MXmlDocument xmlIniData;
		xmlIniData.Create();

		char *buffer;
		MZFile mzf;

		if (pFileSystem)
		{
			if (!mzf.Open(szFileName, pFileSystem))
			{
				if (!mzf.Open(szFileName))
				{
					mlog("LevelUpReward Can't Open File in MZFileSystem (LevelUpReward.xml)\n");
					xmlIniData.Destroy();
					return false;
				}
			}
		}
		else
		{
			if (!mzf.Open(szFileName))
			{
				mlog("LevelUpReward Can't Open File (LevelUpReward.xml)\n");
				xmlIniData.Destroy();
				return false;
			}
		}

		buffer = new char[mzf.GetLength() + 1];
		buffer[mzf.GetLength()] = 0;

		mzf.Read(buffer, mzf.GetLength());

		if (!xmlIniData.LoadFromMemory(buffer))
		{
			mlog("LevelUpReward Can't Load (LevelUpReward.xml) From Memory\n");
			xmlIniData.Destroy();
			return false;
		}

		delete [] buffer;
		mzf.Close();


		MXmlElement rootElement, chrElement, attrElement;
		char szTagName[256];

		rootElement = xmlIniData.GetDocumentElement();
		int iCount = rootElement.GetChildNodeCount();

		for (int i = 0; i < iCount; i++)
		{

			chrElement = rootElement.GetChildNode(i);
			chrElement.GetTagName(szTagName);

			if (szTagName[0] == '#') continue;

			if (!stricmp(szTagName, MTOK_LEVELUPREWARD_REWARD))
			{
				ParseLevelUpReward(chrElement);
			}
			else
			{
				return false;
			}
		}

		xmlIniData.Destroy();

		return true;
	}

	////////////////////////////////////////////////
	static void ParseLevelUpReward(MXmlElement& element)
	{
		char szAttrValue[256];
		char szAttrName[64];
		char szTagName[128];

		MMatchRewardMgr* pRewardMgr = new MMatchRewardMgr;

		// Reward
		int nAttrCount = element.GetAttributeCount();
		int nLevel;
		int nBounty;
		int nCash;
		int nItemID;

		for (int i = 0; i < nAttrCount; i++)
		{
			element.GetAttribute(i, szAttrName, szAttrValue);
			if (!stricmp(szAttrName, MTOK_LEVELUPREWARD_ATTR_LEVEL))
			{
				nLevel = atoi(szAttrValue);
			}
			else if (!stricmp(szAttrName, MTOK_LEVELUPREWARD_ATTR_BOUNTY))
			{
				nBounty = atoi(szAttrValue);
			}
			else if (!stricmp(szAttrName, MTOK_LEVELUPREWARD_ATTR_CASH))
			{
				nCash = atoi(szAttrValue);
			}
			else if (!stricmp(szAttrName, MTOK_LEVELUPREWARD_ATTR_ITEMID))
			{
				nItemID = atoi(szAttrValue);
			}
		}
		if (nLevel >= 0)
		{
			mlog("Level: %i , BP: %i , Cash: %i , ItemID: %i - ", nLevel, nBounty, nCash, nItemID);
			pRewardMgr->AddReward(nLevel, nBounty, nCash, nItemID);
		}
	}
public:
	////////////////////////////////////////////////
	MMatchRewardMap* GetLevelUpRewardByLevel(int nId)
	{
		iterator itor = find(nId);
		if (itor != end())
		{
			return (*itor).second;
		}
	}

	static bool InitLevelUpReward()
	{
#ifdef _UPREWARDLEVEL
		if (!ReadXml(NULL, "LevelUpReward.xml"))
		{
			mlog("Load XML from memory : %s - FAILED\n", "LevelUpReward.xml");
			return false;
		}
		else
		{
			mlog("Load XML from memory : %s - SUCCESS\n", "LevelUpReward.xml");
			return true;
		}
#else
		return true;
#endif
	}

	void GiveReward(MMatchObject* pPlayer, int nLevel)
	{
#ifdef _UPREWARDLEVEL
		if (!IsEnabledObject(pPlayer))
			return;

		MMatchRewardMap* pLvlUp = GetLevelUpRewardByLevel(nLevel);
		if (pLvlUp == NULL)
			return;

		if (!m_MatchDBMgr.InsertLevelUpReward(pPlayer->GetAccountInfo()->m_nAID, pPlayer->GetCharInfo()->m_nCID, pLvlUp->GetBounty(), pLvlUp->GetCash(), pLvlUp->GetItemID()))
		{
			mlog("DB Query(InserLevelUpReward) Failed\n");
		}
#endif
		return;
	}
};