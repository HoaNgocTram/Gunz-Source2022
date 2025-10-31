#include "StdAfx.h"
#include "AntiShotBot.h"
#define LLKHF_LOWER_IL_INJECTED 0x00000002
#define LLMHF_LOWER_IL_INJECTED 0x00000002

HHOOK MouseHook;
HHOOK KeyboardHook;
static HHOOK s_hkMouseHookMacro = 0;
std::list<MTD_KeyParams*> keyList;

template<typename T>
void pop_front(std::vector<T>& vec)
{
	assert(!vec.empty());
	vec.erase(vec.begin());
}

LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
	{
		if (wParam == WM_RBUTTONDOWN || wParam == WM_LBUTTONDOWN || wParam == WM_MBUTTONDOWN || wParam == WM_XBUTTONDOWN)
		{
			MSLLHOOKSTRUCT* info = (MSLLHOOKSTRUCT*)lParam;

			if ((info->flags & LLMHF_LOWER_IL_INJECTED) == LLMHF_LOWER_IL_INJECTED)
			{
				ExitProcess(NULL);
				return 1;
			}
		}
	}
	return CallNextHookEx(MouseHook, nCode, wParam, lParam);
}

LRESULT CALLBACK MouseHookMacro(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION) {
		if (wParam == WM_RBUTTONDOWN || wParam == WM_LBUTTONDOWN || wParam == WM_MBUTTONDOWN || wParam == WM_XBUTTONDOWN) {
			auto pHookData = (MSLLHOOKSTRUCT*)lParam;
			if ((pHookData->flags & LLMHF_INJECTED) == LLMHF_INJECTED) {
				ExitProcess(NULL);
				return 1;
			}
			if ((pHookData->flags & LLMHF_INJECTED) == LLMHF_INJECTED) {
				ExitProcess(NULL);
				return 1;
			}
		}
	}
	return CallNextHookEx(s_hkMouseHookMacro, nCode, wParam, lParam);
}

LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
	{
		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
		{
			KBDLLHOOKSTRUCT* info = (KBDLLHOOKSTRUCT*)lParam;
			if (info->vkCode <= 0x00 || info->vkCode >= 0xE0 ||
				info->vkCode == 0xE9 || /*info->vkCode == 0xA4 || info->vkCode == 0xA5 || */
				info->vkCode == 0x5B || info->vkCode == 0x5C || info->vkCode == 0x07 || info->vkCode == 0x08 ||
				info->vkCode == 0x09 || info->vkCode == 0x0D || (info->vkCode >= 0xA6 && info->vkCode <= 0xB9) ||
				info->vkCode == 0x14 || info->vkCode == 0x91 || info->vkCode == 0x1B || info->vkCode == 0xE7 ||
				info->vkCode == 0x0C || (info->vkCode >= 0xC1 && info->vkCode <= 0xDA) ||
				(info->vkCode >= VK_KANA && info->vkCode <= 0x1A) || (info->vkCode >= 0x1C && info->vkCode <= 0x1F))
			{
				return CallNextHookEx(KeyboardHook, nCode, wParam, lParam);
			}

			if (keyList.size() == 5)
			{
				MTD_KeyParams* frontParam = keyList.front();
				delete frontParam;
				keyList.pop_front();
			}

			MTD_KeyParams* params = new MTD_KeyParams();
			params->vkCode = info->vkCode;
			params->scanCode = info->scanCode;
			params->flags = info->flags;
			params->time = info->time;
			params->extraInfo = info->dwExtraInfo;
			keyList.push_back(params);

			if ((info->flags & LLKHF_INJECTED) == LLKHF_INJECTED && !((info->flags & LLKHF_EXTENDED) == LLKHF_EXTENDED) && info->dwExtraInfo == 0)
			{
				ExitProcess(NULL);
				return 1;
			}
		}
	}
	return CallNextHookEx(KeyboardHook, nCode, wParam, lParam);
}


void AntiShotbotLoggerAB()
{
	HINSTANCE hInstance = GetModuleHandle(NULL);
	MouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, hInstance, NULL);
	KeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, hInstance, NULL);

	int sigsearch = 0;
	MSG message;
	while (GetMessage(&message, NULL, sigsearch, sigsearch) && (ZGetApplication()->m_RunThread)) 
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	UnhookWindowsHookEx(KeyboardHook);
	UnhookWindowsHookEx(MouseHook);
}