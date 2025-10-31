#include "stdafx.h"
#include "CStringTable.h"
#include <map>
#include <string>
using namespace std;

map<string, string>		g_MapList;

void STDestroy()
{
	if (!g_MapList.empty())
		g_MapList.clear();
}

bool STLoadFromFile(const char* szFileName)
{
	if (!g_MapList.empty())
		g_MapList.clear();

	CFile file;
	CFileException ex;
	if (file.Open(szFileName, CFile::modeRead, &ex) == FALSE)
		return false;

	char* szBuff = new char[(unsigned int)file.GetLength()];
	file.Read(szBuff, (UINT)file.GetLength());

	bool bRetVal = STLoadFromMemory(szBuff);

	delete[] szBuff;
	file.Close();

	return bRetVal;
}

bool STLoadFromMemory(const char* szMemString)
{
#ifdef _OUTPUT_STRING
	DeleteFile("str_out.txt");
#endif

	char ch;
	while (1)
	{
		while (1)
		{
			ch = *szMemString++;
			if (ch == 0)
				return true;

			if ((ch == '\n') || (ch == '\r') || (ch == ' '))
				continue;

			break;
		}

		char szName[64];
		int nNameCount = 0;
		while (1)
		{
			szName[nNameCount++] = ch;

			ch = *szMemString++;
			if ((ch == 0) || (nNameCount >= 64))
				return true;

			if (ch == ',')
				break;
		}
		szName[nNameCount] = 0;

		char szString[512];
		int nStringCount = 0;
		while (1)
		{
			ch = *szMemString++;
			if ((ch == 0) || (nStringCount >= 512))
				return true;

			if ((ch == '\n') || (ch == '\r'))
				break;

			szString[nStringCount++] = ch;
		}
		szString[nStringCount] = 0;

		if (g_MapList.find(szName) != g_MapList.end())
		{
			ASSERT(0);

			continue;
		}

		g_MapList.insert(map<string, string>::value_type(szName, szString));

#ifdef _OUTPUT_STRING
		FILE* pFile = fopen("str_out.txt", "a");
		if (fopen)
		{
			fprintf(pFile, "%s , %s\n", szName, szString);
			fclose(pFile);
		}
#endif
	}
}

const char* STGetString(const char* szName)
{
	map<string, string>::iterator itr = g_MapList.find(szName);
	if (itr == g_MapList.end())
		return NULL;

	return (char*)(*itr).second.data();
}

void STGetString(const char* szName, char* szString)
{
	strcpy(szString, "NO_MSG");

	map<string, string>::iterator itr = g_MapList.find(szName);
	if (itr == g_MapList.end())
		return;

	strcpy(szString, (char*)(*itr).second.data());
}