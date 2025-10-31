#ifndef __HanReportForClient__
#define __HanReportForClient__

#ifdef HANREPORTFORCLIENT_EXPORTS
#define HANREPORT_API __declspec(dllexport)
#else
#define HANREPORT_API __declspec(dllimport)
#ifdef _DEBUG
#pragma comment(lib, "HanReportForClientD.lib") 
#else
#pragma comment(lib, "HanReportForClient.lib") 
#endif
#endif

#define SERVICE_NATION  0x000000FF
#define SERVICE_KOR		0x00000001
#define SERVICE_USA		0x00000002
#define SERVICE_JPN		0x00000003
#define SERVICE_CHN		0x00000004

#define SERVICE_TYPE    0x00000F00
#define SERVICE_ALPHA	0x00000100
#define SERVICE_REAL	0x00000200
#define SERVICE_BETA	0x00000300

#define SERVICE_SITE	0x0000F000
#define SERVICE_HANGAME 0x00001000
#define SERVICE_ASOBLOG 0x00002000

#define HAN_REPORTCLI_OK								0
#define HAN_REPORTCLI_ARGUMENT_INVALID					-1
#define HAN_REPORTCLI_INITED_ALREADY					-2
#define HAN_REPORTCLI_INITED_NOT						-3
#define HAN_REPORTCLI_INITED_FAIL						-4
#define HAN_REPORTCLI_GAMESTRING_IDINVALID				-14
#define HAN_REPORTCLI_GAMESTRING_USNINVALID				-15
#define HAN_REPORTCLI_GAMESTRING_GAMEIDINVALID			-16
#define HAN_REPORTCLI_GAMESTRING_SERVICECODEINVALID		-17
#define HAN_REPORTCLI_SERVICECODEINVALID				-20
#define HAN_REPORTCLI_PL2FILE_INVALID					-21

#define HAN_REPORTCLI_DLL_UNEXPECTED					-100

#define HAN_REPORTCLI_PARAM_INVALID						1
#define HAN_REPORTCLI_IPADDR_INVALID					2
#define HAN_REPORTCLI_SYSTEM_ERROR						7

HANREPORT_API int __stdcall HanReportInit( char* szGameId,
										   int   nServiceCode,
										   bool  bSendDump=true
										  );

HANREPORT_API int __stdcall HanReportInitGameString( char* szGameString, bool bSendDump=true);

HANREPORT_API int __stdcall HanReportSendPosLog( char* szReportString);

HANREPORT_API int __stdcall HanReportSendPosLogSync( char* szReportString);

HANREPORT_API int __stdcall HanReportSend( char* szSubject,
										   char* szReportString);


HANREPORT_API int __stdcall HanReportConvertMD5( char* szUserId, char* szMD5USerId);

HANREPORT_API int __stdcall HanReportGetNationCode( char *szGameString,
												   int *pNationCode);

HANREPORT_API int __stdcall HanReportGetServiceTypeCode( char *szGameString,
														int *pServiceTypeCode);

HANREPORT_API int __stdcall HanReportGetSiteCode( char *szGameString,
												 int *pSiteCode);

void ConvertHexToString(char*  szMD5String, unsigned char* szHex, int nHex );





HANREPORT_API int __stdcall HanReportInitWithDump( char* szGameId,  int nServiceCode );

HANREPORT_API int __stdcall HanReportInitGameStringWithDump( char* szGameString);

#endif