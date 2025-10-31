#include "stdafx.h"

#include "ZPost.h"
#include "MBlobArray.h"
#include "MMatchTransDataType.h"
#include "MMatchGlobal.h"
#include "ZGame.h"
#include "ZMyCharacter.h"
#include "ZGameClient.h"
#include "ZApplication.h"
#include "ZConfiguration.h"
#include "MMD5.h"
#include <Psapi.h>
#pragma comment (lib, "psapi.lib")

void ZPostUserOption()
{
	unsigned long nOptionFlags = 0;

	if (Z_ETC_REJECT_WHISPER)
		nOptionFlags |= MBITFLAG_USEROPTION_REJECT_WHISPER;
	if (Z_ETC_REJECT_INVITE)
		nOptionFlags |= MBITFLAG_USEROPTION_REJECT_INVITE;

	ZPOSTCMD1(MC_MATCH_USER_OPTION, MCmdParamUInt(nOptionFlags));
}


//For module list.
std::string GetSystemPathToDiskPath(char* szPath)
{
	char* pszPath = new char[512];

	strcpy(pszPath,szPath);

	std::string szNewPath = "";
	CHAR szTemp[MAX_PATH];
	szTemp[0] = '\0';

	if (GetLogicalDriveStrings(512-1, szTemp)) 
	{
		CHAR szName[MAX_PATH];
		CHAR szDrive[3] = TEXT(" :");
		BOOL bFound = FALSE;
		CHAR* p = szTemp;

		do 
		{
			*szDrive = *p;

			if (QueryDosDevice(szDrive, szName, 512))
			{
				UINT uNameLen = strlen(szName);

				if (uNameLen < MAX_PATH) 
				{
					bFound = _tcsnicmp(pszPath, szName, 
						uNameLen) == 0;

					if (bFound) 
					{
						CHAR szTempFile[MAX_PATH];
						_snprintf_s(szTempFile,
							MAX_PATH,
							"%s%s",
							szDrive,
							pszPath+uNameLen);
						strncpy_s(pszPath, MAX_PATH+1, szTempFile, strlen(szTempFile));
					}
				}
			}
			// Go to the next NULL character.
			while (*p++);
		} while (!bFound && *p); // end of string
	}
	
	szNewPath = pszPath;
	delete [] pszPath;
	return szNewPath;
}


// 해커 리버싱 방해용도의 커맨드ID 반환 함수 : 자세한 주석은 헤더 참조
//#pragma optimize("", off)

int UncloakCmdId(int cloakedCmdId, int cloakFactor)
{
	return cloakedCmdId - cloakFactor;
}

//#pragma optimize("", on)