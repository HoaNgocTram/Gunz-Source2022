#ifndef _MCRASHDUMP_H
#define _MCRASHDUMP_H

DWORD CrashExceptionDump(PEXCEPTION_POINTERS ExceptionInfo, const char* szDumpFileName, bool bMLog=false);

#endif