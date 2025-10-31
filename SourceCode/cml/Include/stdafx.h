// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#ifndef _STDAFX_H
#define _STDAFX_H

#define _WIN32_WINNT 0x0501
#include <sdkddkver.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _EXTFILEMRS  "mrs"
#define _EXTFILEMRS_E  ".mrs"
#define _EXTFILEMRS_ZIP  ".zip"

// ¿©±â´Ù include
#include <stdio.h>
#include <list>
#include <map>
#include <string>
#include <winsock2.h>
#include <windows.h>
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"
#include "targetver.h"

#endif