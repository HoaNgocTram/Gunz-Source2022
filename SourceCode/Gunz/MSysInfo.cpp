#include "StdAfx.h"
#include "MDebug.h"
#include "d3d9.h"
//#include <dsound.h>
void MSysInfoLog_CPU()
{
	LARGE_INTEGER ulFreq, ulTicks, ulValue, ulStartCounter,
		ulEAX_EDX, ulResult;

	// it is number of ticks per seconds
	QueryPerformanceFrequency(&ulFreq);
	// current value of the performance counter

	QueryPerformanceCounter(&ulTicks);
	// calculate one-second interval
	ulValue.QuadPart = ulTicks.QuadPart + ulFreq.QuadPart;
	// read time stamp counter
	// this asm instruction load the highorder 32 bit of the
	// register into EDX and the lower order 32 bits into EAX
	_asm
	{
		rdtsc
		mov ulEAX_EDX.LowPart, EAX
		mov ulEAX_EDX.HighPart, EDX
	}

	// start number of ticks
	ulStartCounter.QuadPart = ulEAX_EDX.QuadPart;
	// loop for 1 second
	do
	{
		QueryPerformanceCounter(&ulTicks);
	} while (ulTicks.QuadPart <= ulValue.QuadPart);

	// get the actual number of ticks
	_asm
	{
		rdtsc
		mov ulEAX_EDX.LowPart, EAX
		mov ulEAX_EDX.HighPart, EDX
	}

	// calculate result
	ulResult.QuadPart = ulEAX_EDX.QuadPart
		- ulStartCounter.QuadPart;
	int nCPUClock = int(ulResult.QuadPart / 1000000);


	DWORD nCPUFamily,nCPUModel,nCPUStepping;
	static char pszCPUType[13];
	memset(pszCPUType, 0, 13);
	_asm
	{
		mov eax, 0
		cpuid

		// getting information from EBX
		mov pszCPUType[0], bl
		mov pszCPUType[1], bh
		ror ebx, 16
		mov pszCPUType[2], bl
		mov pszCPUType[3], bh

		// getting information from EDX
		mov pszCPUType[4], dl
		mov pszCPUType[5], dh
		ror edx, 16
		mov pszCPUType[6], dl
		mov pszCPUType[7], dh

		// getting information from ECX
		mov pszCPUType[8], cl
		mov pszCPUType[9], ch
		ror ecx, 16
		mov pszCPUType[10], cl
		mov pszCPUType[11], ch

		mov eax, 1
		cpuid

		mov nCPUFamily, eax
		mov nCPUModel, eax
		mov nCPUStepping, eax

	}
	pszCPUType[12] = '\0';

	nCPUFamily = (nCPUFamily >> 8);
	nCPUModel =  ((nCPUModel >> 4 ) & 0x0000000f);
	nCPUStepping = (nCPUStepping & 0x0000000f);

	char szDesc[512]="";
	sprintf(szDesc, "CPU ID = %s ( family = %d , model = %d , stepping = %d ) @ %d MHz\n",
		pszCPUType,nCPUFamily,nCPUModel,nCPUStepping,nCPUClock);
	mlog(szDesc);
}

void MSysInfoLog_Display()
{
	HMODULE					hD3DLibrary = NULL;
	LPDIRECT3D9				pD3D = NULL;
	LPDIRECT3DDEVICE9		pLPDIRECT3DDEVICE9 = NULL;
	D3DADAPTER_IDENTIFIER9	deviceID;
	/*
		hD3DLibrary = LoadLibrary( "d3d9.dll" );

		if (!hD3DLibrary)
		{
			mlog("Error, could not load d3d9.dll");
			return;
		}*/

	pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (pD3D == nullptr)
	{
		mlog("Error creating device.\n");
		return;
	}



	pD3D->GetAdapterIdentifier(0, 0, &deviceID);
	pD3D->Release();

#ifndef _PUBLISH
	mlog("D3D_SDK_VERSION = %d \n", D3D_SDK_VERSION);
#endif

	mlog("Display Device = %s ( vendor=%x device=%x subsys=%x revision=%x )\n",
		deviceID.Description, deviceID.VendorId, deviceID.
		DeviceId, deviceID.SubSysId, deviceID.Revision);

	mlog("Display Driver Version = %d.%d.%04d.%04d\n",
		deviceID.DriverVersion.HighPart >> 16, deviceID.DriverVersion.HighPart & 0xffff,
		deviceID.DriverVersion.LowPart >> 16, deviceID.DriverVersion.LowPart & 0xffff);
}

//BOOL CALLBACK MSysInfoLog_Audio_EnumProc(LPGUID lpGUID, LPCTSTR lpszDesc, LPCTSTR lpszDrvName, LPVOID lpContext)
//{
//	mlog("Audio : %s %s \n",lpszDesc,lpszDrvName);
//    return TRUE;
//}
//
//void MSysInfoLog_Audio()
//{
//	HMODULE					hDSoundLibrary=NULL;
//	hDSoundLibrary = LoadLibrary( "dsound.dll" );
//	if (!hDSoundLibrary)
//	{
//		mlog("Error, could not load dsound.dll");
//		return;
//	}
//
//	typedef HRESULT * (__stdcall *DIRECTSOUNDENUMERATETYPE)(LPDSENUMCALLBACK  lpDSEnumCallback,LPVOID  lpContext);
//	DIRECTSOUNDENUMERATETYPE pDirectSoundEnumerate = (DIRECTSOUNDENUMERATETYPE) GetProcAddress(hDSoundLibrary, "DirectSoundEnumerate");
//
//	if (!pDirectSoundEnumerate)
//	{
//		mlog("Error, could not get proc adress of Direct3DCreate9.");
//		FreeLibrary(hDSoundLibrary);
//		return;
//	}
//
//	*pDirectSoundEnumerate((LPDSENUMCALLBACK)MSysInfoLog_Audio_EnumProc, NULL);
//
//}

typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
LPFN_ISWOW64PROCESS fnIsWow64Process;
BOOL MSysInfo_IsWow64()
{
	BOOL bIsWow64 = FALSE;

	//IsWow64Process is not available on all supported versions of Windows.
	//Use GetModuleHandle to get a handle to the DLL that contains the function
	//and GetProcAddress to get a pointer to the function if available.

	int nIsWow64Proc[] = { 0xC9, 0xF3, 0xD7, 0xEF, 0xF7, 0xB6, 0xB4, 0xD0, 0xF2, 0xEF, 0xE3, 0xE5, 0xF3, 0xF3, 0x80 };
	int nKernel32Dll[] = { 0xEB, 0xE5, 0xF2, 0xEE, 0xE5, 0xEC, 0xB3, 0xB2, 0xAE, 0xE4, 0xEC, 0xEC, 0x80 };

	char szIsWow64Proc[128];
	memset( szIsWow64Proc, 0, sizeof(szIsWow64Proc) );
	char szKernel32Dll[128];
	memset( szKernel32Dll, 0, sizeof(szKernel32Dll) );

	for( int i = 0; i < _countof( nIsWow64Proc ); ++i )
		szIsWow64Proc[i] = nIsWow64Proc[i] ^ 0x80;

	for( int i = 0; i < _countof( nKernel32Dll ); ++i )
		szKernel32Dll[i] = nKernel32Dll[i] ^ 0x80;

	fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(GetModuleHandle(TEXT(szKernel32Dll)), szIsWow64Proc);

	if(NULL != fnIsWow64Process)
	{
		if (!fnIsWow64Process(GetCurrentProcess(),&bIsWow64))
		{
			//handle error
			return FALSE;
		}
	}

    return bIsWow64;
}

void MSysInfoLog_OS()
{
	// Custom: Modified and added Win Vista + 7
	OSVERSIONINFOEX os;
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	GetVersionEx( (OSVERSIONINFO*)&os );

	MEMORYSTATUS ms;
	ms.dwLength = sizeof(MEMORYSTATUS);
	::GlobalMemoryStatus(&ms);
	DWORD dwPhysicalMemory;
	dwPhysicalMemory=ms.dwTotalPhys;

	// Custom: Get service pack data, print n/a if not applicable
	char szCSDVersion[128];
	memset( szCSDVersion, 0, sizeof( szCSDVersion ) );
	strcpy( szCSDVersion, os.szCSDVersion );

	if( strlen( os.szCSDVersion ) == 0 )
		strcpy( szCSDVersion, "n/a" );

	char szDesc[512]="";
	sprintf(szDesc, "Windows = %d.%d Build %d , %s (%s) (%dKB) : ", os.dwMajorVersion, os.dwMinorVersion,
		os.dwBuildNumber, szCSDVersion, MSysInfo_IsWow64() ? "64-bit" : "32-bit", (int)(dwPhysicalMemory/1024));
	mlog(szDesc);

	if (os.dwMajorVersion == 10)
	{
		if (os.dwMinorVersion == 0)
		{
			if (os.wProductType == VER_NT_WORKSTATION)
				sprintf(szDesc, " Windows 10..\n");
			else
				sprintf(szDesc, " Windows Server Technical Preview..\n");
		}
	}

	else if( os.dwMajorVersion == 6 )
	{
		if( os.dwMinorVersion == 0 )
		{
			if( os.wProductType == VER_NT_WORKSTATION )
				sprintf( szDesc, " Windows Vista..\n" );
			else
				sprintf( szDesc, " Windows Server 2008..\n" );
		}
		else if( os.dwMinorVersion == 1 )
		{
			if( os.wProductType == VER_NT_WORKSTATION )
				sprintf( szDesc, " Windows 7..\n" );
			else
				sprintf( szDesc, " Windows Server 2008 R2..\n" );
		}
		else if( os.dwMinorVersion == 2 )
		{
			if (os.wProductType == VER_NT_WORKSTATION)
				sprintf(szDesc, " Windows 8..\n");
			else
				sprintf(szDesc, " Windows Server 2012..\n");
		}
		else if( os.dwMinorVersion == 3 )
		{
			if (os.wProductType == VER_NT_WORKSTATION)
				sprintf(szDesc, " Windows 8.1..\n");
			else
				sprintf(szDesc, " Windows Server 2012 R2..\n");
		}
		else
			sprintf(szDesc," ..\n");
	}
	else if(os.dwMajorVersion==5)
	{
		if(os.dwMinorVersion==0)		sprintf(szDesc," Windows 2000..\n");
		else if(os.dwMinorVersion==1)	sprintf(szDesc," Windows xp..\n");
		else if(os.dwMinorVersion==2)	sprintf(szDesc," Windows 2003..\n");
		else							sprintf(szDesc," ..\n");
	}
	else if(os.dwMajorVersion==4)
	{
		if(os.dwMinorVersion==0)		sprintf(szDesc," Windows 95..\n");
		else if(os.dwMinorVersion==10)	sprintf(szDesc," Windows 98..\n");
		else if(os.dwMinorVersion==90)	sprintf(szDesc," Windows Me..\n");
		else							sprintf(szDesc," ..\n");
	}
	else if(os.dwMajorVersion==3)
	{
		if(os.dwMinorVersion==51)		sprintf(szDesc," Windows NT 3.51..\n");
		else							sprintf(szDesc," ..\n");
	}
	else
	{
		sprintf(szDesc," ..\n");
	}
	mlog(szDesc);
}

void MSysInfoLog()
{
	MSysInfoLog_CPU();
	MSysInfoLog_Display();
//	MSysInfoLog_Audio();
	MSysInfoLog_OS();
}