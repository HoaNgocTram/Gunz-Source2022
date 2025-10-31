#include "stdafx.h"
#include "MMatchGunGame.h"
#include "MXml.h"
#include <vector>

MGunGameData g_GunGameData;
int g_nGunGameMaxLevel = 0;
bool g_bRandomize = false;
bool g_bSpecialFirst = false;

bool InitializeGunGame()
{
	MXmlDocument	xmlDocument;

	xmlDocument.Create();

	if (!xmlDocument.LoadFromFile("battle/gungame.xml"))
	{
		xmlDocument.Destroy();
		return false;
	}

	MXmlElement rootElement, chrElement, attrElement;
	char szTagName[256];

	rootElement = xmlDocument.GetDocumentElement();

	int iCount = rootElement.GetChildNodeCount();

	for (int i = 0; i < iCount; i++)
	{
		chrElement = rootElement.GetChildNode(i);
		chrElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!strcmp(szTagName, "RULE"))
		{
			int nMaxLevel = 0;
			char szSortSpecial[128];
			
			chrElement.GetAttribute(&nMaxLevel, "maxlevel");
			chrElement.GetAttribute(&g_bRandomize, "randomize");
			chrElement.GetAttribute(szSortSpecial, "sortspecial", "last");

			if (!stricmp(szSortSpecial, "first"))
			{
				g_bSpecialFirst = true;
			}
			else if (!stricmp(szSortSpecial, "last"))
			{
				g_bSpecialFirst = false;
			}

			g_nGunGameMaxLevel = nMaxLevel;
		}

		if (!strcmp(szTagName, "WEAPONSET"))
		{
			vector<GunGameItemData> vItemData;
			int nLevel = 0;
			char szSpecial[128];
			chrElement.GetAttribute(&nLevel, "level");
			chrElement.GetAttribute(szSpecial, "special", "false");

			if (nLevel <= 0)
				continue;

			int nWeaponsetChildCount = chrElement.GetChildNodeCount();
			MXmlElement elementTarget;
			char szTargetTagName[128];

			for (int j = 0; j < nWeaponsetChildCount; j++)
			{
				elementTarget = chrElement.GetChildNode(j);
				elementTarget.GetTagName(szTargetTagName);
				if (szTargetTagName[0] == '#') continue;

				if (!stricmp(szTargetTagName, "WEAPON"))
				{
					int nDescID = 0;
					int parts = 3;
					char szSlot[128];

					elementTarget.GetAttribute(&nDescID, "id");
					elementTarget.GetAttribute(szSlot, "parts");

					if (!stricmp(szSlot, "melee"))
					{
						parts = 0;
					}
					else if (!stricmp(szSlot, "primary"))
					{
						parts = 1;
					}
					else if (!stricmp(szSlot, "secondary"))
					{
						parts = 2;
					}

					if (parts >= 3)
					{
						continue;
					}

					GunGameItemData itemData;
					itemData.nItemID = nDescID;
					itemData.nItemParts = parts;
					vItemData.push_back(itemData);
				}
			}

			GunGameData ggData;
			ggData.nLevel = nLevel;
			ggData.bSpecial = (stricmp(szSpecial, "true") == 0);
			ggData.itemData.swap(vItemData); // swap contents

			g_GunGameData.push_back(ggData);
		}
	}

	xmlDocument.Destroy();

	return true;
}

int* GetGunGameItemSet(MGunGameData data, int nLevel)
{
	int* nItemSet = new int[3];
	memset(nItemSet, 0, sizeof(nItemSet));

	for (int i = 0; i < data.size(); ++i)
	{
		GunGameData gg = data[i];

		if (gg.nLevel == nLevel)
		{
			for (int j = 0; j < gg.itemData.size(); j++)
			{
				nItemSet[gg.itemData[j].nItemParts] = gg.itemData[j].nItemID;
			}
		}
	}

	return nItemSet;
}

int GetGunGameMaxLevel()
{
	return g_nGunGameMaxLevel;
}

bool GetGunGameRuleRandomize()
{
	return g_bRandomize;
}

vector<GunGameData> GetGunGameData()
{
	return g_GunGameData;
}

bool IsSpecialSortedFirst()
{
	return g_bSpecialFirst;
}

int GetSpecialCount()
{
	int nSpecialCount = 0;
	for (int i = 0; i < g_GunGameData.size(); i++)
	{
		GunGameData ggData = g_GunGameData[i];

		if (ggData.bSpecial)
			nSpecialCount++;
	}

	return nSpecialCount;
}