#ifndef _ZGLOBAL_H
#define _ZGLOBAL_H

//#include "RMesh.h"
#include "ZApplication.h"


// 전역적으로 사용하는 것은 이곳에다 넣도록 하자
#ifdef LOCALE_BRAZIL
#define APPLICATION_NAME		"The Duel"
#elif defined(_NAMEAPP)
#define APPLICATION_NAME		_NAMEAPP
#else
#define APPLICATION_NAME		"Gunz"
#endif

// 저장되는 폴더
#ifdef LOCALE_BRAZIL
#define GUNZ_FOLDER			"/The Duel"
#elif LOCALE_JAPAN
#define GUNZ_FOLDER			"/GUNZWEI"
#elif defined(_NAMEAPPFOLDER)
#define GUNZ_FOLDER			"/" _NAMEAPPFOLDER
#else
#define GUNZ_FOLDER			"/Gunz"
#endif

#define SCREENSHOT_FOLDER	"/Screenshots"
#define REPLAY_FOLDER		"/Replay"
#define EMBLEM_FOLDER		"/Emblem"


class MZFileSystem;
class MMessenger;

class ZApplication;
class ZGameClient;
class ZSoundEngine;
class ZGameInterface;
class ZEffectManager;
class ZScreenEffectManager;
class ZDirectInput;
class ZCombatInterface;
class ZCamera;
class ZGame;
class ZBaseQuest;
class ZQuest;
class ZSurvival;
class ZGameTypeManager;
class ZWorldManager;
class ZMessengerManager;
class ZEmblemInterface;
class ZInput;

extern ZDirectInput	g_DInput;
extern ZInput* g_pInput;

extern HANDLE g_hIntegrityThread;

bool					ZIsLaunchDevelop(void);

RMeshMgr*				ZGetNpcMeshMgr(void);
RMeshMgr*				ZGetMeshMgr(void);
RMeshMgr*				ZGetWeaponMeshMgr(void);
RAniEventMgr*			ZGetAniEventMgr(void);

ZSoundEngine*			ZGetSoundEngine(void);

ZEffectManager*			ZGetEffectManager(void);
ZScreenEffectManager*	ZGetScreenEffectManager(void);

int						ZGetSVNRevision(void);


// dll-injection으로 호출하는 핵 때문에 매크로 인라이닝
#define ZGetApplication()		ZApplication::GetInstance()
#define ZGetGameClient()		(ZApplication::GetGameInterface() ? ZApplication::GetGameInterface()->GetGameClient() : NULL)
#define ZGetGame()				(ZApplication::GetGameInterface() ? ZApplication::GetGameInterface()->GetGame() : NULL)

#define ZGetGameInterface()		ZApplication::GetGameInterface()
#define ZGetCombatInterface()	(ZApplication::GetGameInterface() ? ZApplication::GetGameInterface()->GetCombatInterface() : NULL)

#define ZGetFileSystem()		ZApplication::GetFileSystem()
#define ZGetDirectInput()		(&g_DInput)

#define ZGetQuest()				((ZBaseQuest*)((ZApplication::GetGameInterface()) ? ZApplication::GetGameInterface()->GetQuest() : NULL))
#define ZGetQuestExactly()		((ZQuest*)((ZApplication::GetGameInterface()) ? ZApplication::GetGameInterface()->GetQuestExactly() : NULL))
#define ZGetSurvivalExactly()	((ZSurvival*)((ZApplication::GetGameInterface()) ? ZApplication::GetGameInterface()->GetSurvivalExactly() : NULL))

#define ZGetGameTypeManager()	((ZApplication::GetGameInterface()) ? ZApplication::GetGameInterface()->GetGameTypeManager() : NULL)

#define ZGetInput()				(g_pInput)
#define ZGetCamera()			(ZApplication::GetGameInterface() ? ZApplication::GetGameInterface()->GetCamera() : NULL)

#define ZGetWorldManager()		ZApplication::GetInstance()->GetWorldManager()
#define ZGetWorld()				(ZGetWorldManager()->GetCurrent())

inline ZEmblemInterface*	ZGetEmblemInterface() { return ZApplication::GetInstance()->GetEmblemInterface(); }
inline ZOptionInterface*	ZGetOptionInterface(void) { return ZApplication::GetInstance()->GetOptionInterface(); }


#define ZIsActionKeyPressed(_ActionID)	(ZGetInput()->IsActionKeyPressed(_ActionID))

// Returns true if the key is pressed down this frame.
#define ZIsActionKeyDown(_ActionID)	(ZGetInput()->IsActionKeyDown(_ActionID))
// Returns true if the key is not pressed down this frame.
#define ZIsActionKeyUp(_ActionID)	(!ZGetInput()->IsActionKeyDown(_ActionID))

// Returns true if the key was pressed down last frame.
#define ZWasActionKeyDownLast(_ActionID)	(ZGetInput()->WasActionKeyDownLast(_ActionID))
// Returns true if the key was not pressed down last frame.
#define ZWasActionKeyUpLast(_ActionID)	(!ZGetInput()->WasActionKeyDownLast(_ActionID))

// Returns true if the action key was just pressed down this frame, i.e. it was up last frame and
// down this frame.
#define ZWasActionKeyPressed(_ActionID) (ZWasActionKeyUpLast(_ActionID) && ZIsActionKeyDown(_ActionID))
// Returns true if the action key was just released this frame, i.e. it was down last frame and
// up this frame.
#define ZWasActionKeyReleased(_ActionID) (ZWasActionKeyDownLast(_ActionID) && ZIsActionKeyUp(_ActionID))
#define PROTECT_DEBUG_REGISTER(b) if(GetTickCount() >0)if(GetTickCount() >0)if(GetTickCount() >0)if(b)
//jintriple3 디버그 레지스터 해킹 방어 위한 비교 숫자.
#define FOR_DEBUG_REGISTER 1000


//static BOOL IsWow64()
//{
//	BOOL bIsWow64 = FALSE;
//
//	IsWow64Process is not available on all supported versions of Windows.
//	Use GetModuleHandle to get a handle to the DLL that contains the function
//	and GetProcAddress to get a pointer to the function if available.
//
//	typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
//	LPFN_ISWOW64PROCESS fnIsWow64Process;
//	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "IsWow64Process");
//
//	if (NULL != fnIsWow64Process)
//	{
//		if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64))
//		{
//			handle error
//			bIsWow64 = false;
//		}
//	}
//	return bIsWow64;
//}

#ifdef _TDD
	DWORD ZTDDTimeGetTime();
	extern DWORD g_nPseudoTimeForTDD;
	extern bool g_bUsePseudoTimeForTDD;
#endif

inline DWORD ZTimeGetTime()
{
#ifdef _TDD
	return ZTDDTimeGetTime();
#else
	return timeGetTime();
#endif
}

#endif

#ifdef _HITSCOUNT
extern int nShots, nHits;
#endif
extern int PositionXYZ;

extern HANDLE AntiNoob;