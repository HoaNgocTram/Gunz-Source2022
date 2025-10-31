#pragma once
//#include "vld.h"
#include <sdkddkver.h>
#include <afxdb.h>
#include <afxtempl.h>
#include <afxdtctl.h>
#include <winsock2.h>
#include <mswsock.h>
#include <crtdbg.h>
#include <windows.h>
#include <mmsystem.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <math.h>
#include <cstddef>
#include <comutil.h>
#include <stdio.h>
#include <intrin.h>
#include <IPHlpApi.h>
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"
#pragma comment(lib, "Iphlpapi.lib")
#include "../CSCommon/MFeatureDefine.h"

#include "detours.h"
#pragma comment(lib, "detours.lib")

#define _WIN32_WINNT 0x0501
#define _AFX_NO_MFC_CONTROLS_IN_DIALOGS
#define _CRTDBG_MAP_ALLOC
#define WIN32_LEAN_AND_MEAN	

#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define NO_WARN_MBCS_MFC_DEPRECATION

/////////////////////
//  Toggle system  //
//  By Desperate  //
/////////////////////

#define _MAPSNOWATHERS 1

// Custom: Anti Hack
#define	DETOURS_X86 1
#define	DETOURS_32BIT 1

//////////CHATSYSTEM//////
#define _NEW_CHAT 1
#define _CHATBACKGROUND 1
/////////////////////////

#define _SYSINTERNEW 1
#define _MACROTIME 1
#define _FIRSTBLOOD 1
//#define _NEWLOAD 1
#define _HITSCOUNT 1
#define _SPRINTSCREEN 1
#define _HIDETHINGLOADING 1
#define _PAINTMODE 1
#define _FSM_ALLOWUNKNOWN 1
#define _SCREENSHOT_FILTER_CHAT 1
#define _CHARACTERLOBBY 1
#define _LAUNCHER 1
#define _FONTSIZE 1
#define _NAMEAPP "Gunz"
#define _NAMEAPPFOLDER "Gunz The Duel"
#define _NAMEAPPFOLDER_BANDICAM "\\Gunz The Duel"
#define _LOGINPING 1
#define _DRAWHPAP 1
#define _MELEESHOT 1
//#define _MAIET_LOGO 1
#define _LOGINORI 1
#define _FIXGUNMASIVE 1
#define _CHARGEMASSIVESTYLE 1
#define _LADDERDUEL 1
#define _FONTNEW 1
#define _CAMERADISTANCE 1
#define _OGGTOMP3 1
#define _TRAILCOLOR 1
#define _BULLETBAR 1
#define _MACOLOR 1
#define _CROSS_SIZE_BOX 1
#define _BARNPC 1
#define _NPCBAR 1
#define _FREELOOK 1
#define _EXTFILEMRS_E  ".mrs"
#if defined(LOCALE_NHNUSA)
#endif
#if defined(_DEBUG) || defined(_RELEASE) || defined(LOCALE_KOREA) || defined(LOCALE_NHNUSA) || defined(LOCALE_JAPAN) || defined(LOCALE_BRAZIL)

#endif

// stl
#include <list>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <sstream>

// DirectX Includes
#include "d3d9.h"
#include "d3dx9math.h"

#include "fmod.h"

// cml
#include "MXml.h"
#include "MUtil.h"
#include "MDebug.h"
#include "MRTTI.h"
#include "MUID.h"
#include "MemPool.h"

// xor head
#include <xorstr.h>

// mint
#include "Mint.h"
#include "MWidget.h"
#include "MBitmap.h"
#include "MButton.h"
#include "MListBox.h"
#include "MTextArea.h"
#include "MTabCtrl.h"
#include "MComboBox.h"
#include "MFrame.h"
#include "MPopupMenu.h"

// realspace2
#include "rtypes.h"
#include "RNameSpace.h"
#include "XBspObject.h"
#include "RTypes.h"
#include "RealSpace2.h"
#include "RBspObject.h"
#include "RMeshMgr.h"
#include "RVisualMeshMgr.h"
#include "RMaterialList.h"
#include "RAnimationMgr.h"
#include "Mint4R2.h"

// cscommon
#include "MObject.h"
#include "MMatchObject.h"
#include "MMatchStage.h"
#include "MMatchItem.h"
#include "MMatchMap.h"
#include "MSafeUDP.h"
#include "MMatchClient.h"
#include "MGameClient.h"
#include "MMatchTransDataType.h"
#include "MErrorTable.h"
#include "Config.h"
#include "MSharedCommandTable.h"
#include "MObjectTypes.h"
#include "MMatchBuff.h"

// gunz global
#include "ZApplication.h"
#include "ZGlobal.h"
#include "ZMessages.h"
#include "ZStringResManager.h"
#include "ZGameInterface.h"
#include "ZCombatInterface.h"
#include "ZGame.h"
#include "ZGameClient.h"
#include "ZObject.h"
#include "ZIDLResource.h"
#include "ZInterfaceListener.h"
#include "ZColorTable.h"
#include "ZMyInfo.h"
#include "ZMyItemList.h"
#include "ZNetRepository.h"
#include "ZItem.h"
#include "ZItemDesc.h"
#include "ZPost.h"
#include "ZSoundEngine.h"
#include "ZSoundFMod.h"
#include "ZCamera.h"
#include "ZCharacter.h"
#include "ZActor.h"
#include "ThemidaSDK.h"
#include "RGGlobal.h"
#include "targetver.h"
