#include "stdafx.h"
#include "Log.h"
#include <sys/timeb.h>

void InitLog()
{
	return;

	FILE* pFile;
	pFile = fopen("patchlog.txt", "w");
	if (pFile == NULL)
		return;

	__time64_t ltime;
	_time64(&ltime);
	fprintf(pFile, "Log start : %s\n", _ctime64(&ltime));

	char szDir[512];
	GetCurrentDirectory(sizeof(szDir), szDir);
	fprintf(pFile, "Current Directory : %s\n", szDir);

	fclose(pFile);
}

void PutLog(const char* pszLog)
{
	return;

	FILE* pFile;
	pFile = fopen("patchlog.txt", "a");
	if (pFile == NULL)
		return;

	char szBuff[128];
	_strtime(szBuff);
	struct __timeb64 tstruct;
	_ftime64(&tstruct);
	fprintf(pFile, "%s:%03d   ", szBuff, tstruct.millitm);

	fprintf(pFile, pszLog);
	fprintf(pFile, "\n");

	fclose(pFile);
}