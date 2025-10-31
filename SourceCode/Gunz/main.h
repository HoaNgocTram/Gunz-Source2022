#ifndef _MAIN_H
#define _MAIN_H

//#pragma once
#include "../CSCommon/MFeatureDefine.h"
#pragma warning(disable:4099)

#ifdef _DEBUG
#pragma comment(lib,"cmld.lib")
#pragma comment(lib,"mint2d.lib")
#pragma comment(lib,"RealSoundd.lib")
#pragma comment(lib,"RealSpace2d.lib")
#pragma comment(lib,"CSCommond.lib")
#pragma comment(lib,"SafeUDPd.lib")

#else
#pragma comment(lib,"cml.lib")
#pragma comment(lib,"mint2.lib")
#pragma comment(lib,"RealSound.lib")
#pragma comment(lib,"RealSpace2.lib")
#pragma comment(lib,"CSCommon.lib")
#pragma comment(lib,"SafeUDP.lib")
#pragma comment(lib, "dxerr.lib")

#ifdef _VOICE_CHAT
#pragma comment(lib,"portaudio.lib")
#pragma comment(lib,"celt.lib")
#pragma comment(lib,"opus.lib")
#pragma comment(lib,"silk_common.lib")
#pragma comment(lib,"silk_fixed.lib")
#pragma comment(lib,"silk_float.lib")
#endif

#endif


#pragma comment(lib,"winmm.lib")




#endif