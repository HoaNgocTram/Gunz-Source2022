#include "stdafx.h"
#include "MEventInfo.h"

MEvents::MEvents()
{

}

MEvents::~MEvents()
{

}

MEvents* MEvents::GetInstance()
{
	static MEvents m_EventMgr;
	return &m_EventMgr;
}

bool MEvents::ReadXml(const char* szFileName, MZFileSystem* pFileSystem)
{
	MXmlDocument	xmlIniData;
	xmlIniData.Create();

	char *buffer;
	MZFile mzf;

	if (pFileSystem)
	{
		if (!mzf.Open(szFileName, pFileSystem))
		{
			if (!mzf.Open(szFileName))
			{
				xmlIniData.Destroy();
				return false;
			}
		}
	}
	else
	{
		if (!mzf.Open(szFileName))
		{
			xmlIniData.Destroy();
			return false;
		}
	}

	buffer = new char[mzf.GetLength() + 1];
	buffer[mzf.GetLength()] = 0;
	mzf.Read(buffer, mzf.GetLength());

	if (!xmlIniData.LoadFromMemory(buffer))
	{
		xmlIniData.Destroy();
		return false;
	}
	delete[] buffer;
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

		if (!stricmp(szTagName, "EVENTINFO"))
		{
			LoadEventInfo(chrElement);
		}
	}

	xmlIniData.Destroy();
	return true;
}

void MEvents::LoadEventInfo(MXmlElement& element)
{
	char szAttrValue[256];
	char szAttrName[64];
	char szTagName[128];


	int nChildCount = element.GetChildNodeCount();
	MXmlElement chrElement;
	for (int i = 0; i < nChildCount; i++)
	{
		chrElement = element.GetChildNode(i);
		chrElement.GetTagName(szTagName);
		if (szTagName[0] == '#') continue;

		if (!stricmp(szTagName, "EVENT"))
		{ 
			int nAttrCount = chrElement.GetAttributeCount();

			MEventInfo* node = new MEventInfo();

			for (int j = 0; j < nAttrCount; ++j) 
			{

				chrElement.GetAttribute(j, szAttrName, szAttrValue);
				if (!stricmp(szAttrName, "name"))
				{
					node->strEventName = szAttrValue;
				}
				else if (!stricmp(szAttrName, "secondaryname"))
				{
					node->strSecondaryName = szAttrValue;
				}
				else if (!stricmp(szAttrName, "tertiaryname"))
				{
					node->strTertiaryName = szAttrValue;
				}
				else if (!stricmp(szAttrName, "quarternaryname"))
				{
					node->strQuarteraryName = szAttrValue;
				}
				else if (!stricmp(szAttrName, "objective"))
				{
					node->strEventObjective = MGetStringResManager()->GetStringFromXml(szAttrValue);
				}
				else if (!stricmp(szAttrName, "description"))
				{
					node->strEventDesc = MGetStringResManager()->GetStringFromXml(szAttrValue);
				}
				else if (!stricmp(szAttrName, "objColor"))
				{
					sscanf(szAttrValue, "%d:%d:%d", &node->nObjColor[0], &node->nObjColor[1], &node->nObjColor[2]);
				}
				else if (!stricmp(szAttrName, "descColor"))
				{
					sscanf(szAttrValue, "%d:%d:%d", &node->nDescColor[0], &node->nDescColor[1], &node->nDescColor[2]);
				}
			}
			m_pEventInfo.push_back(move(node));
		}
	}
}