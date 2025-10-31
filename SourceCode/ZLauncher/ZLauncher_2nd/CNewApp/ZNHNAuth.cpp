#include "stdafx.h"
#include "ZGlobal.h"
#include "ZNHNAuth.h"
#include "MDebug.h"

ZNHNAuth::ZNHNAuth()
{
	m_bIsCompleted = false;

	memset(m_szAuthString, 0, NHN_AUTH_LENGTH);
}

ZNHNAuth::~ZNHNAuth()
{
}

void ZNHNAuth::InitTok()
{
	m_Tok.clear();
}

bool ZNHNAuth::GetGameStr(const string& strCmdLine, char* szOutGameStr, const int nOutGameStrLen)
{
#ifdef _DEBUG
#define _GAMESTR_DEBUG
#endif

#ifdef _DEBUG
#endif

	if (strCmdLine.empty() || (0 == szOutGameStr))
		return false;

#define MAX_FULLPATH_LEN 256
#define MAX_FILENAME_LEN 32

	vector< string >::iterator	it, end;
	size_t						nPos = 0;
	char						szFullPath[MAX_FULLPATH_LEN] = { 0, };
	char						szFileName[MAX_FILENAME_LEN] = { 0, };
	string						strFileName;
	int							nFullPathLen = 0;

	if (0 == _fullpath(szFullPath, "", MAX_FULLPATH_LEN))
	{
		return false;
	}

	if (string::npos != strCmdLine.find(szFullPath))
	{
#ifdef _GAMESTR_DEBUG
#endif

		nFullPathLen = strlen(szFullPath);
	}

	const size_t nGameStrPos = strCmdLine.find(" ", nFullPathLen);
	if (string::npos == nGameStrPos)
	{
		ASSERT(0);
		return false;
	}

	if (nOutGameStrLen < (strCmdLine.length() - nGameStrPos))
	{
		return false;
	}

	strncpy(szFileName, strCmdLine.c_str() + nFullPathLen, MAX_FILENAME_LEN);
	szFileName[MAX_FILENAME_LEN - 1] = '\0';

#ifdef _GAMESTR_DEBUG
#endif

	strFileName = szFileName;
	end = m_Tok.end();
	for (it = m_Tok.begin(); it != end; ++it)
	{
#ifdef _GAMESTR_DEBUG
#endif
		nPos = strFileName.find(*it);
		if (string::npos != nPos)
		{
			strncpy(szOutGameStr, strCmdLine.c_str() + nGameStrPos, strCmdLine.length() - nGameStrPos);
#ifdef _GAMESTR_DEBUG
#endif
			return true;
		}
	}

	return false;
}

bool ZNHNAuth::InitAuth()
{
	if (true == m_bIsCompleted)
	{
		return true;
	}

	char szGameStr[NHN_AUTH_LENGTH] = { 0, };

	const char* pszCmdLine = GetCommandLine();
	if (0 == pszCmdLine)
		return false;

	const int nCmdLen = strlen(pszCmdLine);

	if (NHN_AUTH_LENGTH < nCmdLen)
	{
		return false;
	}

	InitTok();

	if (!GetGameStr(pszCmdLine, szGameStr, NHN_AUTH_LENGTH))
	{
		return false;
	}

	char szGameStringNew[4096] = { 0x00 };
	char szMemberId[25] = { 0x00 };

	strcpy(m_szAuthString, szGameStr);

	m_bIsCompleted = true;

	return true;
}