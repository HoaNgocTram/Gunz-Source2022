#include <Windows.h>
bool IsAddressHooked(unsigned long address)
{
	BYTE* offsetValue = (BYTE*)address;
	return (*offsetValue == 0xE8 || *offsetValue == 0xE9 || *offsetValue == 0x7E || *offsetValue == 0x74 || *offsetValue == 0xFF);
}

void StreakGuard()
{
	DWORD getTickCount = (DWORD)GetProcAddress(GetModuleHandleA(XorStr<0xC2, 13, 0xC8D01D45>("\xA9\xA6\xB6\xAB\xA3\xAB\xFB\xFB\xE4\xAF\xA0\xA1" + 0xC8D01D45).s), XorStr<0x7E, 13, 0x39677FD3>("\x39\x1A\xF4\xD5\xEB\xE0\xEF\xC6\xE9\xF2\xE6\xFD" + 0x39677FD3).s);
	DWORD queryPerformanceCounter = (DWORD)GetProcAddress(GetModuleHandleA(XorStr<0xCB, 13, 0x78928F39>("\xA0\xA9\xBF\xA0\xAA\xBC\xE2\xE0\xFD\xB0\xB9\xBA" + 0x78928F39).s), XorStr<0x15, 24, 0x2E187D3F>("\x44\x63\x72\x6A\x60\x4A\x7E\x6E\x7B\x71\x6D\x4D\x40\x4C\x40\x41\x66\x49\x52\x46\x5D\x4F\x59" + 0x2E187D3F).s);

	while (true)
	{
		if (IsAddressHooked(getTickCount) || IsAddressHooked(queryPerformanceCounter) || 
			GetModuleHandleA(XorStr<0xB6, 10, 0x38B0BE6A>("\xC6\xC5\xD7\xC1\xC3\x95\xD8\xD1\xD2" + 0x38B0BE6A).s) ||
			GetModuleHandleA(XorStr<0xF5, 13, 0xE518A4D8>("\x9C\x98\x9D\x9D\x9A\x8E\x94\x8E\xD3\x9A\x93\x6C" + 0xE518A4D8).s) ||
			GetModuleHandleA(XorStr<0xDA, 12, 0x16342B7F>("\xB1\xB5\xEF\xB9\xAC\xB6\x90\xCF\x86\x8F\x88" + 0x16342B7F).s) ||
			GetModuleHandleA(XorStr<0x18, 12, 0xD90D718C>("\x6B\x71\x75\x6F\x7E\x72\x6A\x31\x44\x4D\x4E" + 0xD90D718C).s) ||
			GetModuleHandleA(XorStr<0x89, 24, 0x457ABD19>("\xCA\xF8\xEA\xE5\xEA\xA9\xFC\xB0\xB9\xD0\xF6\xF5\xE6\xE2\xBE\xB8\xDC\xC9\xCB\xB2\xF9\xF2\xF3" + 0x457ABD19).s) ||
			GetModuleHandleA(XorStr<0x63, 26, 0x7ED7050B>("\x2D\x2F\x37\x46\x32\x06\x00\x1C\x0E\x1E\x1E\x0F\x03\x5E\x22\x1A\x1C\x00\x01\x1F\x12\x56\x1D\x16\x17" + 0x7ED7050B).s) ||
			GetModuleHandleA(XorStr<0x63, 26, 0x7ED7050B>("\x2D\x2F\x37\x46\x32\x06\x00\x1C\x0E\x1E\x1E\x0F\x03\x5E\x22\x1A\x1C\x00\x01\x1F\x12\x56\x1D\x16\x17" + 0x7ED7050B).s) ||
			GetModuleHandleA(XorStr<0xB3, 13, 0x55ABBCDD>("\xC0\xDC\xD0\xDA\xDB\xD9\xC9\xD3\x95\xD8\xD1\xD2" + 0x55ABBCDD).s) ||
			GetModuleHandleA(XorStr<0x0A, 14, 0xD156CB07>("\x59\x7B\x69\x68\x6A\x47\x71\x72\x79\x3D\x70\x79\x7A" + 0xD156CB07).s) ||
			GetModuleHandleA(XorStr<0x97, 15, 0x396C08BB>("\xC0\xF9\xF5\xF6\xD3\xFD\xFE\xF5\xC0\x91\x8F\xC6\xCF\xC8" + 0x396C08BB).s) ||
			GetModuleHandleA(XorStr<0xBB, 15, 0x77CCCBB1>("\xEC\xDD\xD1\xD2\xF7\xA1\xA2\xA9\x9C\xF6\xEB\xA2\xAB\xA4" + 0x77CCCBB1).s) ||
			GetModuleHandleA(XorStr<0xAF, 15, 0x727D0191>("\xF8\xD1\xDD\xDE\xFB\xD5\xD6\xDD\xE8\x8B\x97\xDE\xD7\xD0" + 0x727D0191).s) ||
			GetModuleHandleA(XorStr<0x0D, 14, 0x4CD3BD22>("\x20\x23\x22\x3D\x3C\x3F\x3E\x39\x48\x38\x73\x74\x75" + 0x4CD3BD22).s) ||
			GetModuleHandleA(XorStr<0xFC, 14, 0x8D67A660>("\xD1\xD0\xD3\xD2\x2D\x2C\x2F\x2E\x29\x2B\x62\x6B\x64" + 0x8D67A660).s) ||
			GetModuleHandleA(XorStr<0x14, 13, 0x3898A07F>("\x7D\x7B\x7C\x72\x7B\x6D\x75\x69\x32\x79\x72\x73" + 0x3898A07F).s) ||
			GetModuleHandleA(XorStr<0x5E, 11, 0x5E3A4723>("\x3A\x3E\x0E\x08\x07\x0F\x4A\x01\x0A\x0B" + 0x5E3A4723).s) ||
			GetModuleHandleA(XorStr<0x6E, 11, 0x2FF76C66>("\x06\x0E\x02\x15\x01\x03\x5A\x11\x1A\x1B" + 0x2FF76C66).s) ||
			GetModuleHandleA(XorStr<0xBC, 11, 0xC597F72E>("\xCF\xD2\xD8\xCB\xB3\xB1\xEC\xA7\xA8\xA9" + 0xC597F72E).s) ||
			GetModuleHandleA(XorStr<0xA6, 7, 0xDC1BB055>("\xF2\xCA\x86\xCD\xC6\xC7" + 0xDC1BB055).s) ||

			GetModuleHandleA(XorStr<0x12, 9, 0x02C658DB>("\x7A\x7C\x7B\x7E\x38\x73\x74\x75" + 0x02C658DB).s) != NULL)
		{
			char guard[128];
			sprintf(guard, "SecureJork: No use dll, most likely, you are automatically banned, avoid using third-party programs.");
			mlog("%s", guard);
#ifdef _HWID
			if (ZGetGameInterface()) 
			{
				ZPostBanMe(WHT_INJECT);
			}
			else 
			{
				char Log[64];
				sprintf(Log, EXIT_TEXT);
				mlog("%s\n", Log);
				ExitProcess(NULL);
			}
#else
			ExitProcess(NULL);
#endif
		}
		Sleep(3000);
	}
}
void Scanner()
{ //Anti Memory editing
	//	SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_BEGIN);
	CRITICAL_SECTION m_cs;
	InitializeCriticalSection(&m_cs);
	EnterCriticalSection(&m_cs);
	unsigned long Start = (unsigned long)GetModuleHandle(NULL) + 0x1000;
	unsigned long End = Start + 0x34d3f9;
	int cnt = 0, cnt2 = 0;
	unsigned long EndAddy = 0;
	bool foundend = false;
	for (unsigned long s = Start; s <= End; s++)
	{
		unsigned char n = *((unsigned char*)s);
		if (n >= 0x00 && n <= 0xff) cnt++;
		if (n == 0x00) cnt2++; else cnt2 = 0;
		if (cnt2 >= 16 && foundend == false)
		{
			EndAddy = s - Start;//16 bytes past code end itll do
			foundend = true;
		}
	}
	PBYTE Mem = new BYTE[EndAddy];
	memcpy(Mem, (LPVOID)Start, EndAddy);
	unsigned long int TimeTest1 = 0, TimeTest2 = 0;
	LeaveCriticalSection(&m_cs);
	while (true)
	{
		EnterCriticalSection(&m_cs);
		if (memcmp(Mem, (LPVOID)Start, EndAddy) != 0)
		{
			mlog("Memory is Different\n");
			__asm
			{
				mov eax, 0x0242141
				call eax
			}
		}
		TimeTest1 = TimeTest2;
		TimeTest2 = timeGetTime();
		if (TimeTest1 != 0)
		{
			if ((TimeTest2 - TimeTest1) > 110000)
			{
				__asm
				{
					mov eax, 0x0242141
					call eax
				}
			}
		}
		LeaveCriticalSection(&m_cs);
		Sleep(24000);
	}
}