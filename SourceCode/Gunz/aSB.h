#include <Windows.h>
#include "detours.h"

class cDetour 
{
public:
	void* Tramp;
	void* Det;
	void* Target;
	
	cDetour(void* From, void* To)
	{
		DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
		DetourAttachEx(&From, To, (DETOUR_TRAMPOLINE**)&Tramp, &Target, &Det);
		DetourTransactionCommit();
	}
};

cDetour* GetRawInputDataDetour;
unsigned int __stdcall GetRawInputDataHook(HRAWINPUT hRawInput, unsigned int uiCommand, void* pData, unsigned int* pcbSize, unsigned int cbSizeHeader)
{
	unsigned int RetVal = ((unsigned int (__stdcall *)(HRAWINPUT, unsigned int, void*, unsigned int*, unsigned int))GetRawInputDataDetour->Tramp)(hRawInput, uiCommand, pData, pcbSize, cbSizeHeader);

	if (RetVal && pData)
	{
		if (((RAWINPUT*)pData)->header.hDevice == NULL)
			return 0;
	}

	return RetVal;
}