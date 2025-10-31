// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#ifdef _DEBUG
	#define _SECURE_SCL 1
	#define _ITERATOR_DEBUG_LEVEL 2
#else
	#define _SECURE_SCL 0
	#define _ITERATOR_DEBUG_LEVEL 0
#endif

#ifndef _STDAFX_H
#define _STDAFX_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _WIN32_WINNT 0x0501
#include <sdkddkver.h>

// 여기다 include
#include <afxdb.h>
#include <afxtempl.h>
#include <afxdtctl.h>

#include <Winsock2.h>
#include <mswsock.h>
#include <crtdbg.h>
#include <windows.h>
#include <stdlib.h>

/*	// for debug publish
#define _HAS_ITERATOR_DEBUGGING 0
#define _SECURE_SCL 0 

#undef _ASSERT
#define _ASSERT(expr) ((void)0)
#undef ASSERT
#define ASSERT(f)          ((void)0)
*/

// stl
#include <string.h>
#include <map>
#include <list>
#include <vector>
#include <algorithm>
#include <queue>
#include <set>

using namespace std;

#ifndef _PUBLISH

//	#define _CLASSIC					// 클래식 모드용 디파인
#endif

#define _QUEST_ITEM		// 퀘스트 아이템 개발용. by 추교성.
#define _MONSTER_BIBLE	// 몬스터 도관 개발요. by 추교성. 릴리즈때문에 잠시 보류.
#define _BLOCK_HACKER	// 해킹관련 정보를 DB에 남기는 작업.


#include "MLocaleDefine.h"
#include "MDebug.h"
#include "MMatchDebug.h"
#include "MXml.h"
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"

#include "MUID.h"
#include "MMatchGlobal.h"
#include "MMatchUtil.h"
#include "MSharedCommandTable.h"
#include "MCommand.h"
#include "MCommandParameter.h"
#include "MCommandCommunicator.h"
#include "MErrorTable.h"
#include "MServer.h"
#include "MMatchServer.h"
#include "MMatchClient.h"
#include "MObject.h"
#include "MMatchItem.h"
#include "MMatchObjCache.h"
#include "MMatchStage.h"
#include "MMatchObject.h"
#include "MMatchChannel.h"
//#include "plugin_definitions.h"
//#include "ts3_functions.h"

#include "MMatchDebug.h"

#endif