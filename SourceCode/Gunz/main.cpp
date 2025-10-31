#include "stdafx.h"
//#include "../MatchServer/vld/vld.h"

#ifdef _HSHIELD
#include "HShield/HShield.h"
#endif

#ifdef _XTRAP
#include "./XTrap/Xtrap_C_Interface.h"						// update sgk 0702 start
#include "./XTrap/XTrap4Launcher.h"
#pragma comment (lib, "./XTrap/XTrap4Launcher_mt.lib")
#pragma comment (lib, "./XTrap/XTrap4Client_mt.lib")	// update sgk 0702 end
#endif

#include "ZPrerequisites.h"
#include "ZConfiguration.h"
#include "ZGameClient.h"
#include <windows.h>
#include <wingdi.h>
#include <mmsystem.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <tlhelp32.h>
#include <Psapi.h>
#pragma comment (lib, "Psapi.lib")

#include "DxErr.h"


#include "main.h"
#include "resource.h"
#include "VersionNo.h"

#include "Mint4R2.h"
#include "ZApplication.h"
#include "MDebug.h"
#include "ZMessages.h"
#include "MMatchNotify.h"
#include "RealSpace2.h"
#include "Mint.h"
#include "ZGameInterface.h"
#include "RFrameWork.h"
#include "ZButton.h"
#include "ZDirectInput.h"
#include "ZActionDef.h"
#include "MRegistry.h"
#include "ZInitialLoading.h"
#include "MDebug.h"
#include "MCrashDump.h"
#include "ZEffectFlashBang.h"
#include "ZMsgBox.h"
#include "ZSecurity.h"
#include "ZStencilLight.h"
#include "ZReplay.h"
#include "ZUtil.h"
#include "ZOptionInterface.h"
#include "HMAC_SHA1.h"

#ifdef USING_VERTEX_SHADER
#include "RShaderMgr.h"
#endif

//#include "mempool.h"
#include "RLenzFlare.h"
#include "ZLocale.h"
#include "MSysInfo.h"
#include "aSB.h"
#include "ZAntiProcess.h"
#include "ZAntiHack28.h"
#include "MTraceMemory.h"
#include "ZInput.h"
#include "Mint4Gunz.h"
#include "SecurityTest.h"
#include "CheckReturnCallStack.h"
#include "MMD5.h"
#include "RGGlobal.h"
#include <stdlib.h>  
#include <crtdbg.h> 
//#include <vld.h>

HMODULE	g_hUser32;
DWORD g_ThreadLastUpdateTime = 0;
DWORD g_BanLastUpdateTime = 0;
#ifdef _DEBUG
//jintriple3 ∏ﬁ∏∏Æ ∏Ø vld
//#include "vld.h"
#endif

#include "RGMain.h"

#ifdef _DEBUG
RMODEPARAMS	g_ModeParams={640,480,false,D3DFMT_R5G6B5};
//RMODEPARAMS	g_ModeParams={1024,768,false,RPIXELFORMAT_565};
#else
RMODEPARAMS	g_ModeParams={800,600,true,D3DFMT_R5G6B5};
#endif

#ifndef _DEBUG
#define SUPPORT_EXCEPTIONHANDLING
#endif

// Custom: Disable NHN auth
//#ifdef LOCALE_NHNUSA
//#include "ZNHN_USA.h"
//#include "ZNHN_USA_Report.h"
//#include "ZNHN_USA_Poll.h"
//#endif

#ifdef _GAMEGUARD
#include "ZGameguard.h"
#endif

RRESULT RenderScene(void *pParam);

#define RD_STRING_LENGTH 512
char cstrReleaseDate[512];// = "ReleaseDate : 12/22/2003";

ZApplication	g_App;
MDrawContextR2* g_pDC = NULL;
MFontR2*		g_pDefFont = NULL;
ZDirectInput	g_DInput;
ZInput*			g_pInput = NULL;
Mint4Gunz		g_Mint;

// Antihack stuff
bool			g_bThreadChecker = true;
DWORD			g_dwThreadId = 0;
HANDLE			g_hIntegrityThread = NULL;
DWORD g_ShouldBanPlayer = 0;
DWORD g_ShouldUpdateCrcTimer = 0;

void IntegrityThread();

HRESULT GetDirectXVersionViaDxDiag( DWORD* pdwDirectXVersionMajor, DWORD* pdwDirectXVersionMinor, TCHAR* pcDirectXVersionLetter );

void zexit(int returnCode)
{
	// ∞‘¿”∞°µÂ¥¬ ¡¶¥ÅEŒ deleteµ«æ˚⁄ﬂ ø¿∑˘πﬂª˝Ω√ ¿⁄√º ∑Œ±◊∏¶ ø√πŸ∏£∞‘ ≥≤±ÅEºÅE¿÷¥Ÿ.
	// ±◊≥… exit()«ÿµµ ZGameGuard∏¶ ΩÃ±€≈œ¿∏∑Œ ∏∏µÈæ˙±ÅE∂ßπÆø° º“∏ÅE⁄ø°º≠ ∞‘¿”∞°µÂ∞° deleteµ«¡ˆ∏∏ æ˚ﬁ∞º≠¿Œ¡ÅE±◊∂ß ≈©∑°Ω√∞° ¿œæ˚œ≠¥Ÿ.
	// exit()«œ±ÅE¿ÅE° ∞‘¿”∞°µÂ∏¶ ºˆµø¿∏∑Œ «ÿ¡¶«œ∏ÅE±◊∑± πÆ¡¶∞° ¿œæ˚œ™¡ÅEæ ¥¬¥Ÿ.
	// «ÿ≈∑ ∞À√ÅEµ˚‹« ¿Ã¿Ø∑Œ ≈¨∂Û¿Ãæ∆Æ ¡æ∑·Ω√ exit«œ¡ˆ∏ª∞ÅEzexit∏¶ æ≤¿⁄.
#ifdef _GAMEGUARD
	GetZGameguard().Release();
#endif
	exit(returnCode);
}

void CrcFailExitApp() { 
#ifdef _PUBLISH
#else
	int* crash = NULL;
	*crash = 0;
#endif
}

void _ZChangeGameState(int nIndex)
{
	GunzState state = GunzState(nIndex);

	if (ZApplication::GetGameInterface())
	{
		ZApplication::GetGameInterface()->SetState(state);
	}
}
#ifdef _LOTTIE 1
#include <rlottie.h>
#endif

RRESULT OnCreate(void *pParam)
{
#ifdef _DEBUG
	g_App.PreCheckArguments();
#endif
	GetRGMain().OnCreateDevice();

	RAdvancedGraphics::bDepthBuffering = Z_VIDEO_STENCILBUFFER;
	RAdvancedGraphics::nMultiSampling = Z_VIDEO_MULTISAMPLING;
	if (RAdvancedGraphics::bDepthBuffering || RAdvancedGraphics::nMultiSampling > 0)
	{
		RMODEPARAMS ModeParams = { RGetScreenWidth(), RGetScreenHeight(), RGetScreenType(), RGetPixelFormat() };
		RResetDevice(&ModeParams);
	}
#ifdef _LOTTIE 1
	//Custom: load Lottie
	if (!rlottie::Animation::SetDX9Ptr(RGetDevice(), g_isDirect3D9ExEnabled ? rlottie::D3D9Ex : rlottie::D3D9))
	{
		mlog("Invalid Lottie DirectX Ptr");
		return R_ERROR_LOADING;
	}
	rlottie::Animation::StartElapsedTime();
#endif
	string strFileLenzFlare("System/LenzFlare.xml");
#ifndef _DEBUG
	strFileLenzFlare += ""; // MEF NULL
#endif
	RCreateLenzFlare(strFileLenzFlare.c_str());
	RGetLenzFlare()->Initialize();

	mlog("main : RGetLenzFlare()->Initialize() \n");

	RBspObject::CreateShadeMap("sfx/water_splash.bmp");
	//D3DCAPS9 caps;
	//RGetDevice()->GetDeviceCaps( &caps );
	//if( caps.VertexShaderVersion < D3DVS_VERSION(1, 1) )
	//{
	//	RGetShaderMgr()->mbUsingShader				= false;
	//	RGetShaderMgr()->shader_enabled				= false;
	//	mlog("main : VideoCard Dosen't support Vertex Shader...\n");
	//}
	//else
	//{
	//	mlog("main : VideoCard support Vertex Shader...\n");
	//}

#ifdef _SPRINTSCREEN
	sprintf( cstrReleaseDate, "Date Build : %s", __DATE__ );
#endif
	g_DInput.Create(g_hWnd, FALSE, FALSE);
	g_pInput = new ZInput(&g_DInput);
	
	RSetGammaRamp(Z_VIDEO_GAMMA_VALUE);
	RSetRenderFlags(RRENDER_CLEAR_BACKBUFFER);

	ZGetInitialLoading()->Initialize(  1, 0, 0, RGetScreenWidth(), RGetScreenHeight(), 0, 0, 1024, 768 );

	mlog("InitialLoading success.\n");

	AntiNoob = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)&StreakGuard, nullptr, 0, nullptr);
	AntiNoob = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)&Scanner, nullptr, 0, nullptr);
	//AntiNoob = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)&SearchandDestroy, nullptr, 0, nullptr);

	struct _finddata_t c_file;
	intptr_t hFile;
	char szFileName[256];
#define FONT_DIR	"Font/"
#define FONT_EXT	"ttf"
	if( (hFile = _findfirst(FONT_DIR "*." FONT_EXT, &c_file )) != -1L ){
		do{
			strcpy(szFileName, FONT_DIR);
			strcat(szFileName, c_file.name);
			AddFontResource(szFileName);
		}while( _findnext( hFile, &c_file ) == 0 );
		_findclose(hFile);
	}

	g_pDefFont = new MFontR2;

	if( !g_pDefFont->Create("Default", Z_LOCALE_DEFAULT_FONT, DEFAULT_FONT_HEIGHT, 1.0f) )
//	if( !g_pDefFont->Create("Default", RGetDevice(), "FONTb11b", 9, 1.0f, true, false) )
//	if( !g_pDefFont->Create("Default", RGetDevice(), "FONTb11b", 14, 1.0f, true, false) )
	{
		mlog("Fail to Create default font : MFontR2 / main.cpp.. onCreate\n" );
		g_pDefFont->Destroy();
		SAFE_DELETE( g_pDefFont );
		g_pDefFont	= NULL;
	}

	g_pDC = new MDrawContextR2(RGetDevice());

#ifndef _NEWLOAD
	if (ZGetInitialLoading()->IsUseEnable())
	{
		if (ZGetLocale()->IsTeenMode())
		{
			ZGetInitialLoading()->AddBitmap(0, "Interface/Default/LOADING/loading_adult.jpg");
		}
		else
		{
			ZGetInitialLoading()->AddBitmap(0, "Interface/Default/LOADING/loading_adult.jpg");
		}
		ZGetInitialLoading()->AddBitmapBar("Interface/Default/LOADING/loading.bmp");
		ZGetInitialLoading()->SetText(g_pDefFont, 45, 28, cstrReleaseDate);

		ZGetInitialLoading()->AddBitmapGrade("Interface/Default/LOADING/loading_adult.jpg");

		ZGetInitialLoading()->SetPercentage(0.0f);
		ZGetInitialLoading()->Draw(MODE_FADEIN, 0, true);
	}
#else
	if (ZGetInitialLoading()->IsUseEnable())
	{	
		ZGetInitialLoading()->AddBitmap(0, "Interface/Default/LOADING/loading_adult_off.jpg");
		ZGetInitialLoading()->AddBitmapBar("Interface/Default/LOADING/loading_adult_on.jpg");
		ZGetInitialLoading()->SetText(g_pDefFont, 10, 30, cstrReleaseDate);

		ZGetInitialLoading()->AddBitmapGrade("Interface/Default/LOADING/loading_adult.jpg");

		ZGetInitialLoading()->SetPercentage(0.0f);
		ZGetInitialLoading()->Draw(MODE_FADEIN, 0, true);
	}
#endif

//	ZGetInitialLoading()->SetPercentage( 10.0f );
//	ZGetInitialLoading()->Draw( MODE_DEFAULT, 0 , true );

	g_Mint.Initialize(800, 600, g_pDC, g_pDefFont);
	Mint::GetInstance()->SetHWND(RealSpace2::g_hWnd);

	mlog("interface Initialize success\n");

	ZLoadingProgress appLoading("application");
	if(!g_App.OnCreate(&appLoading))
	{
		ZGetInitialLoading()->Release();
		return R_ERROR_LOADING;
	}

	ZGetSoundEngine()->SetEffectVolume(Z_AUDIO_EFFECT_VOLUME);
	ZGetSoundEngine()->SetMusicVolume(Z_AUDIO_BGM_VOLUME);
	ZGetSoundEngine()->SetEffectMute(Z_AUDIO_EFFECT_MUTE);
	ZGetSoundEngine()->SetMusicMute(Z_AUDIO_BGM_MUTE);

	g_Mint.SetWorkspaceSize(g_ModeParams.nWidth, g_ModeParams.nHeight);
	g_Mint.GetMainFrame()->SetSize(g_ModeParams.nWidth, g_ModeParams.nHeight);
	ZGetOptionInterface()->Resize(g_ModeParams.nWidth, g_ModeParams.nHeight);

	for(int i=0; i<ZACTION_COUNT; i++){
//		g_Mint.RegisterActionKey(i, ZGetConfiguration()->GetKeyboard()->ActionKeys[i].nScanCode);
		ZACTIONKEYDESCRIPTION& keyDesc = ZGetConfiguration()->GetKeyboard()->ActionKeys[i];
		g_pInput->RegisterActionKey(i, keyDesc.nVirtualKey);
		if(keyDesc.nVirtualKeyAlt!=-1)
			g_pInput->RegisterActionKey(i, keyDesc.nVirtualKeyAlt);
	}

	g_App.SetInitialState();

//	ParseParameter(g_szCmdLine);

	ZGetFlashBangEffect()->SetDrawCopyScreen(true);

	static const char *szDone = "Complete";
	ZGetInitialLoading()->SetLoadingStr(szDone);
	if( ZGetInitialLoading()->IsUseEnable() )
	{
#ifndef _FASTDEBUG
		ZGetInitialLoading()->SetPercentage( 100.f );
		ZGetInitialLoading()->Draw( MODE_FADEOUT, 0 ,true  );
#endif
		ZGetInitialLoading()->Release();
	}

	mlog("main : OnCreate() done\n");

	SetFocus(g_hWnd);

	return R_OK;
}


bool CheckDll(char* fileName, BYTE* SHA1_Value)
{
	BYTE digest[20];
	BYTE Key[GUNZ_HMAC_KEY_LENGTH];

	memset(Key, 0, 20);
	memcpy(Key, GUNZ_HMAC_KEY, strlen(GUNZ_HMAC_KEY));

	CHMAC_SHA1 HMAC_SHA1 ;
	HMAC_SHA1.HMAC_SHA1_file(fileName, Key, GUNZ_HMAC_KEY_LENGTH, digest) ;

	if(memcmp(digest, SHA1_Value, 20) ==0)
	{
		return true;
	}

	return false;
}



RRESULT OnDestroy(void *pParam)
{
	mlog("Destroy gunz\n");

	g_App.OnDestroy();

	SAFE_DELETE(g_pDefFont);

	g_Mint.Finalize();

	mlog("interface finalize.\n");

	SAFE_DELETE(g_pInput);
	g_DInput.Destroy();

	mlog("game input destroy.\n");

	RGetShaderMgr()->Release();

//	g_App.OnDestroy();

	// mlog("main : g_App.OnDestroy()\n");

	ZGetConfiguration()->Destroy();

	mlog("game configuration destroy.\n");

	if (g_pDC != NULL)
	{
		delete g_pDC;
		g_pDC = NULL;
	}

	struct _finddata_t c_file;
	intptr_t hFile;
	char szFileName[256];
#define FONT_DIR	"Font/"
#define FONT_EXT	"ttf"
	if( (hFile = _findfirst(FONT_DIR "*." FONT_EXT, &c_file )) != -1L ){
		do{
			strcpy(szFileName, FONT_DIR);
			strcat(szFileName, c_file.name);
			RemoveFontResource(szFileName);
		}while( _findnext( hFile, &c_file ) == 0 );
		_findclose(hFile);
	}

	MFontManager::Destroy();
	MBitmapManager::Destroy();
	MBitmapManager::DestroyAniBitmap();

	mlog("Bitmap manager destroy Animation bitmap.\n");

	/*
	for(list<HANDLE>::iterator i=g_FontMemHandles.begin(); i!=g_FontMemHandles.end(); i++){
		RemoveFontMemResourceEx(*i);
	}
	*/

	//ReleaseMemPool(RealSoundEffectPlay);
	//UninitMemPool(RealSoundEffectPlay);

	//ReleaseMemPool(RealSoundEffect);
	//UninitMemPool(RealSoundEffect);

	//ReleaseMemPool(RealSoundEffectFx);
	//UninitMemPool(RealSoundEffectFx);

	//mlog("main : UninitMemPool(RealSoundEffectFx)\n");

	// ∏ﬁ∏∏Æ«Æ «ÅE¶
	ZBasicInfoItem::Release(); // «“¥Áµ«æÅE¿÷¥¬ ∏ﬁ∏∏Æ «ÿ¡¶
//	ZHPInfoItem::Release();

	ZGetStencilLight()->Destroy();
	LightSource::Release();

	RBspObject::DestroyShadeMap();
	RDestroyLenzFlare();
	RAnimationFileMgr::GetInstance()->Destroy();
	
	ZStringResManager::ResetInstance();

	DestroyRGMain();

	mlog("destroy gunz finish.\n");

	return R_OK;
}

RRESULT OnUpdate(void* pParam)
{
	__BP(100, "main::OnUpdate");

	g_pInput->Update();

	g_App.OnUpdate();

	const DWORD dwCurrUpdateTime = timeGetTime();

	__EP(100);

	return R_OK;
}

RRESULT OnRender(void* pParam)
{
	__BP(101, "main::OnRender");

	//Clip the cursor to the main screen
	if (GetFocus() == g_hWnd)
	{
		RECT rect;
		GetWindowRect(g_hWnd, &rect);
		ClipCursor(&rect);
	}
	//otherwise release the cursor
	else
	{
		ClipCursor(NULL);
	}

	if (!RIsActive() && RGetScreenType() == 0)
	{
		__EP(101);
		return R_NOTREADY;
	}


	g_App.OnDraw();

	// Custom: Fix bandicam support
	if (ZIsActionKeyPressed(ZACTION_SCREENSHOT))
	{
		if (g_App.GetGameInterface())
			g_App.GetGameInterface()->GetBandiCapturer()->CaptureImage();
	}

#ifdef _SMOOTHLOOP
	Sleep(10);
#endif
	//static char __buffer[256];
	//float x = 10.f / 800.f;
	//int screenx = x * MGetWorkspaceWidth();
	//if (RGetIsWidthScreen() || RGetIsLongScreen())
	//{
	//	x = (x * 800 + 80) / 590.f;
	//	screenx = x * MGetWorkspaceWidth();
	//}
	//if (g_pDefFont && ZGetConfiguration()->GetEtc()->bShowFPS)
	//{
	//	sprintf(__buffer, "FPS: %3.0f", g_fFPS);

	//	float y = 0.0f / 600.f;

	//	if (RGetIsWidthScreen() || RGetIsLongScreen())
	//	{
	//		// 115
	//		g_pDefFont->m_Font.DrawText(MGetWorkspaceWidth() - screenx, 0, __buffer);
	//	}
	//	else
	//		g_pDefFont->m_Font.DrawText(MGetWorkspaceWidth() - 75, 0, __buffer);
	//}
	__EP(101);

	return R_OK;
}

RRESULT OnInvalidate(void *pParam)
{
	MBitmapR2::m_dwStateBlock=NULL;

	g_App.OnInvalidate();
	
	return R_OK;
}

RRESULT OnRestore(void *pParam)
{
	for(int i=0; i<MBitmapManager::GetCount(); i++){
		MBitmapR2* pBitmap = (MBitmapR2*)MBitmapManager::Get(i);
		pBitmap->OnLostDevice();
	}

	g_App.OnRestore();

	return R_OK;
}

RRESULT OnActivate(void *pParam)
{
	if (ZGetGameInterface() && ZGetGameClient() && Z_ETC_BOOST)
		ZGetGameClient()->PriorityBoost(true);
	return R_OK;
}

RRESULT OnDeActivate(void *pParam)
{

	if (ZGetGameInterface() && ZGetGameClient())
		ZGetGameClient()->PriorityBoost(false);
	return R_OK;
}

RRESULT OnError(void *pParam)
{
	mlog("RealSpace::OnError(%d) \n", RGetLastError());

	switch (RGetLastError())
	{
	case RERROR_INVALID_DEVICE:
		{
			D3DADAPTER_IDENTIFIER9 *ai=RGetAdapterID();
			char szLog[512];
			ZTransMsg( szLog, MSG_DONOTSUPPORT_GPCARD, 1, ai->Description);

			int ret=MessageBox(NULL, szLog, ZMsg( MSG_WARNING), MB_YESNO);
			if(ret!=IDYES)
				return R_UNKNOWN;
		}
		break;
	case RERROR_CANNOT_CREATE_D3D:
		{
			ShowCursor(TRUE);

			char szLog[512];
			sprintf(szLog, ZMsg( MSG_DIRECTX_NOT_INSTALL));

			int ret=MessageBox(NULL, szLog, ZMsg( MSG_WARNING), MB_YESNO);
			if(ret==IDYES)
			{
				ShellExecute(g_hWnd, "open", ZMsg( MSG_DIRECTX_DOWNLOAD_URL), NULL, NULL, SW_SHOWNORMAL); 
			}
		}
		break;

	};

	return R_OK;
}

void SetModeParams()
{

	g_ModeParams.nScreenType = ZGetConfiguration()->GetVideo()->nScreenType;
	if (g_ModeParams.nScreenType != 2)
	{
		g_ModeParams.nWidth = ZGetConfiguration()->GetVideo()->nWidth;
		g_ModeParams.nHeight = ZGetConfiguration()->GetVideo()->nHeight;
	}
	else
	{
		g_ModeParams.nWidth = GetSystemMetrics(SM_CXSCREEN);
		g_ModeParams.nHeight = GetSystemMetrics(SM_CYSCREEN) - getTaskBarHeight() - GetSystemMetrics(SM_CYBORDER);
	}
	ZGetConfiguration()->GetVideo()->nColorBits == 32 ? 
		g_ModeParams.PixelFormat = D3DFMT_X8R8G8B8 : g_ModeParams.PixelFormat = D3DFMT_R5G6B5 ;

}

void ResetAppResource()
{
	// (∞≈¿«)∏µÅE∏Æº“Ω∫∏¶ ¡¶∞≈«œ∞ÅE¥ŸΩ√ ∑Œµ˘«—¥Ÿ
	//_ASSERTE( _CrtCheckMemory( ) );

	// Reset GameInterface except its gameclient object. the account connection must be alive.
	ZGetGameInterface()->m_sbRemainClientConnectionForResetApp = true;	// let GameInterface don't clear its gameclient object. (why this interface object has network object???)
	ZGetGameInterface()->GetGameClient()->Destroy();	// but must clear queued messages

	// whole client resource reload
	OnDestroy(0);

	ZGetConfiguration()->Destroy();
	ZGetConfiguration()->Load();

	SetModeParams();

	if( !ZApplication::GetInstance()->InitLocale() )
		MLog("In changing language... InitLocale error !!!\n");

	ZGetConfiguration()->Load_StringResDependent();
	// Custom: Destroy itembuffdescmgr
	MGetMatchBuffDescMgr()->Clear();
	OnCreate(0);
	RParticleSystem::Restore();
	OnRestore(0);
	ZGetGameInterface()->m_sbRemainClientConnectionForResetApp = false;

	if(ZGetGameClient()->IsConnected())
	{
		ZPostRequestCharacterItemListForce(ZGetGameClient()->GetPlayerUID());
	}

	//ZGetGameInterface()->UpdateDuelTournamantMyCharInfoUI();
	//ZGetGameInterface()->UpdateDuelTournamantMyCharInfoPreviousUI();

	// Custom: Disable NHN Auth
	// restore user id
//#ifdef LOCALE_NHNUSA
//	pUSAAuthInfo = (ZNHN_USAAuthInfo*)ZGetLocale()->GetAuthInfo();
//	pUSAAuthInfo->SetUserID(strUserID);
//#endif
}

int FindStringPos(char* str,char* word)
{
	if(!str || str[0]==0)	return -1;
	if(!word || word[0]==0)	return -1;

	int str_len = (int)strlen(str);
	int word_len = (int)strlen(word);

	char c;
	bool bCheck = false;

	for(int i=0;i<str_len;i++) {
		c = str[i];
		if(c == word[0]) {

			bCheck = true;

			for(int j=1;j<word_len;j++) {
				if(str[i+j]!=word[j]) {
					bCheck = false;
					break;
				}
			}

			if(bCheck) {
				return i;
			}
		}
	}
	return -1;
}

bool FindCrashFunc(char* pName)
{
//	Function Name
//	File Name 
	if(!pName) return false;

	FILE *fp;
	fp = fopen( "mlog.txt", "r" );
	if(fp==NULL)  return false;

	fseek(fp,0,SEEK_END);
	int size = ftell(fp);
	fseek(fp,0,SEEK_SET);

	char* pBuffer = new char [size];

	fread(pBuffer,1,size,fp);

	fclose(fp);

	// øÅEÆ Ω˚Ÿ∫ø°º≠ √£¥¬¥Ÿ.
	int posSource = FindStringPos(pBuffer,"ublish");
	if(posSource==-1) return false;

	int posA = FindStringPos(pBuffer+posSource,"Function Name");
//	int posB = FindStringPos(pBuffer,"File Name");	
	// filename ¿Ã æ¯¥¬ ∞ÊøÅEµ ¿÷æ˚ÿ≠ ¿Ã∑∏∞‘ πŸ≤Â¥Ÿ
	int posB = posA + FindStringPos(pBuffer+posSource+posA,"\n");

	if(posA==-1) return false;
	if(posB==-1) return false;

	posA += 16;

//	int memsize = posB-posA-6;
	int memsize = posB-posA;
	memcpy(pName,&pBuffer[posA+posSource],memsize);

	pName[memsize] = 0;

	delete [] pBuffer;

	for(int i=0;i<memsize;i++) {
		if(pName[i]==':') {
			pName[i] = '-';
		}
	}

	return true;
}

void HandleExceptionLog()
{
	/*#define ERROR_REPORT_FOLDER	"ReportError"

	extern char* logfilename;	// Instance on MDebug.cpp

	// ERROR_REPORT_FOLDER ¡∏¿Á«œ¥¬¡ÅE∞ÀªÁ«œ∞ÅE æ¯¿∏∏ÅEª˝º∫
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	hFind = FindFirstFile(ERROR_REPORT_FOLDER, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) {
		if (!CreateDirectory("ReportError", NULL)) {
			MessageBox(g_hWnd, "ReportError ∆˙¥ı∏¶ ª˝º∫«“ ºÅEæ¯Ω¿¥œ¥Ÿ.", APPLICATION_NAME , MB_ICONERROR|MB_OK);
			return;
		}
	} else 	{
		FindClose(hFind);
	}*/


/* 2007≥ÅE2øÅE13¿œ BAReport ¥ı¿ÃªÅEªÁøÅE∏¯«œ∞‘ ∏∑¿Ω


	// mlog.txt ∏¶ ERROR_REPORT_FOLDER ∑Œ ∫πªÅE

	//acesaga_0928_911_moanus_rslog.txt
	//USAGE_EX) BAReport app=acesaga;addr=moon.maiet.net;port=21;id=ftp;passwd=ftp@;gid=10;user=moanus;localfile=rslog.txt;remotefile=remote_rslog.txt;

//	if(ZGetCharacterManager()) {
//		ZGetCharacterManager()->OutputDebugString_CharacterState();
//	}


	ZGameClient* pClient = (ZGameClient*)ZGameClient::GetInstance();

	char* pszCharName = NULL;
	MUID uidChar;
	MMatchObjCache* pObj;
	char szPlayer[128];

	if( pClient ) {

		uidChar = pClient->GetPlayerUID();
		pObj = pClient->FindObjCache(uidChar);
		if (pObj)
			pszCharName = pObj->GetName();

		wsprintf(szPlayer, "%s(%d%d)", pszCharName?pszCharName:"Unknown", uidChar.High, uidChar.Low);
	}
	else { 
		wsprintf(szPlayer, "Unknown(-1.-1)");
	}


//	if (pClient) {

		time_t currtime;
		time(&currtime);
		struct tm* pTM = localtime(&currtime);

		char cFuncName[1024];

		if(FindCrashFunc(cFuncName)==false) {
			strcpy(cFuncName,"Unknown Error");
		}

		char szFileName[_MAX_DIR], szDumpFileName[_MAX_DIR];
		wsprintf(szFileName, "%s_%s_%.2d%.2d_%.2d%.2d_%s_%s", cFuncName,
				APPLICATION_NAME, pTM->tm_mon+1, pTM->tm_mday, pTM->tm_hour, pTM->tm_min, szPlayer, "mlog.txt");
		wsprintf(szDumpFileName, "%s.dmp", szFileName);

		char szFullFileName[_MAX_DIR], szDumpFullFileName[_MAX_DIR];
		wsprintf(szFullFileName, "%s/%s", ERROR_REPORT_FOLDER, szFileName);
		wsprintf(szDumpFullFileName, "%s/%s", ERROR_REPORT_FOLDER, szDumpFileName);

		if (CopyFile("mlog.txt", szFullFileName, TRUE))
		{
			CopyFile("Gunz.dmp", szDumpFullFileName, TRUE);

			// BAReport Ω««ÅE
			char szCmd[4048];
			char szRemoteFileName[_MAX_DIR], szRemoteDumpFileName[_MAX_DIR];
			wsprintf(szRemoteFileName, "%s/%s/%s", ZGetConfiguration()->GetBAReportDir(), "gunzlog", szFileName);
			wsprintf(szRemoteDumpFileName, "%s/%s/%s", ZGetConfiguration()->GetBAReportDir(), "gunzlog", szDumpFileName);

			wsprintf(szCmd, "BAReport app=%s;addr=%s;port=21;id=ftp;passwd=ftp@;user=%s;localfile=%s,%s;remotefile=%s,%s", 
				APPLICATION_NAME, ZGetConfiguration()->GetBAReportAddr(), szPlayer, szFullFileName, szDumpFullFileName, szRemoteFileName, szRemoteDumpFileName);

			WinExec(szCmd, SW_SHOW);

			FILE *file = fopen("bareportpara.txt","w+");
			fprintf(file,szCmd);
			fclose(file);
		}
//	}
*/
}

long FAR PASCAL WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
#ifdef LOCALE_JAPAN
		case WM_COPYDATA:
			{
				ZBaseAuthInfo* pAuth = ZGetLocale()->GetAuthInfo();
				if( ((ZGameOnJPAuthInfo*)pAuth)->NewLogin(wParam, lParam) )
				{
					MessageBox(g_hWnd, "Same id accessing from a different PC", NULL, MB_OK);
					zexit(-1);
				}
			}
			break;
#endif

		case WM_SYSCHAR:
			if(ZIsLaunchDevelop() && wParam==VK_RETURN)
			{
#ifndef _PUBLISH
				RFrame_ToggleFullScreen();
#endif
				return 0;
			}
			break;

		case WM_CREATE:
			if (strlen(Z_LOCALE_HOMEPAGE_TITLE) > 0)
			{
				ShowIExplorer(false, Z_LOCALE_HOMEPAGE_TITLE);
			}
			break;
		case WM_DESTROY:
			if (strlen(Z_LOCALE_HOMEPAGE_TITLE) > 0)
			{
				ShowIExplorer(true, Z_LOCALE_HOMEPAGE_TITLE);
			}
			break;
		case WM_SETCURSOR:
			if(ZApplication::GetGameInterface())
				ZApplication::GetGameInterface()->OnResetCursor();
			return TRUE; // prevent Windows from setting cursor to window class cursor

		case WM_ENTERIDLE:
			// ∏¥ﬁ ¥ÅE≠ªÛ¿⁄∞° ƒ⁄µÂ∏¶ ∫˙”∞«œ∞ÅE¿÷¿ª ∂ß ∫Œ∏ø°∞‘ ∫∏≥ª¥¬ idle ≈ÅEˆ∏ﬁΩ√¡ÅE
			// (¿œ∫ª IMEø° ∏¥ﬁ ¥ÅE≠ªÛ¿⁄∞° ¿÷æ˚ÿ≠ ≥÷æ˙¿Ω)
			// ∏¥ﬁ ¥ÅE≠ªÛ¿⁄∑Œ æ˜µ•¿Ã∆Æ ∑Á«¡∏¶ ∫˙”∞«ÿº≠ π´¿ÅEæ˚÷‰¡˚”Œ æ«øÅE«±ÅE∂ßπÆø° ø©±‚º≠ æ˜µ•¿Ã∆Æ∏¶ Ω««‡«—¥Ÿ
			RFrame_UpdateRender();
			break;

			/*
		case  WM_LBUTTONDOWN:
			SetCapture(hWnd);
			return TRUE;
		case WM_LBUTTONUP:
			ReleaseCapture();
			return TRUE;
			*/
		case WM_KEYDOWN:
			{
				bool b = false;
			}
	}

	if(Mint::GetInstance()->ProcessEvent(hWnd, message, wParam, lParam)==true)
	{
		if (ZGetGameInterface() && ZGetGameInterface()->IsReservedResetApp())	// for language changing
		{
			ZGetGameInterface()->ReserveResetApp(false);
			ResetAppResource();
		}

		return 0;
	}

	// thread safe«œ±‚¿ß«ÿ ≥÷¿Ω
	if (message == WM_CHANGE_GAMESTATE)
	{
		_ZChangeGameState(wParam);
	}


	return DefWindowProc(hWnd, message, wParam, lParam);
}

/*
class mtrl {
public:

};

class node {
public:
	int		m_nIndex[5];
};


class _map{
public:
	mtrl* GetMtrl(node* node,int index) { return GetMtrl(node->m_nIndex[index]); }
	mtrl* GetMtrl(int id) { return m_pIndex[id]; }

	mtrl*	m_pIndex[5];
};

class game {
public:
	_map m_map;	
};

game _game;
game* g_game;
*/


void ClearTrashFiles()
{
}

bool CheckFileList()
{
	MZFileSystem *pfs=ZApplication::GetFileSystem();
	MZFile mzf;

	string strFileNameFillist(FILENAME_FILELIST);
#ifndef _DEBUG
	strFileNameFillist += ""; // MEF NULL
#endif

	if(!mzf.Open(strFileNameFillist.c_str() ,pfs))
		return false;

	char *buffer;
	buffer=new char[mzf.GetLength()+1];
	mzf.Read(buffer,mzf.GetLength());
	buffer[mzf.GetLength()]=0;

	MXmlDocument aXml;
	aXml.Create();
	if(!aXml.LoadFromMemory(buffer))
	{
		delete[] buffer;
		return false;
	}

	delete[] buffer;

	int iCount, i;
	MXmlElement		aParent, aChild;
	aParent = aXml.GetDocumentElement();
	iCount = aParent.GetChildNodeCount();

	char szTagName[256];
	for (i = 0; i < iCount; i++)
	{
		aChild = aParent.GetChildNode(i);
		aChild.GetTagName(szTagName);
		if(stricmp(szTagName,"FILE")==0)
		{
			char szContents[256],szCrc32[256];
			aChild.GetAttribute(szContents,"NAME");
			aChild.GetAttribute(szCrc32,"CRC32");

			if(stricmp(szContents,"config.xml")!=0)
			{
				unsigned int crc32_list = pfs->GetCRC32(szContents);
				unsigned int crc32_current;
				sscanf(szCrc32,"%x",&crc32_current);

#ifndef _DEBUG
				if(crc32_current!=crc32_list)
				{
					// mlog("crc error , file %s ( current = %x, original = %x ).\n",szContents,crc32_current,crc32_list);

					// ∏µÅE∆ƒ¿œ¿ª ∞ÀªÁ¥¬ «—¥Ÿ
					return false; 
				}
#endif
			}
		}
	}

	return true;
}


enum RBASE_FONT{
	RBASE_FONT_GULIM = 0,
	RBASE_FONT_BATANG = 1,

	RBASE_FONT_END
};

static int g_base_font[RBASE_FONT_END];
static char g_UserDefineFont[256];

bool _GetFileFontName(char* pUserDefineFont)
{
	if(pUserDefineFont==NULL) return false;

	FILE* fp = fopen("_Font", "rt");
	if (fp) {
		fgets(pUserDefineFont,256, fp);
		fclose(fp);
		return true;
	}
	return false;
}

bool CheckFont()
{
	char FontPath[MAX_PATH];
	char FontNames[MAX_PATH+100];

	::GetWindowsDirectory(FontPath, MAX_PATH);

	strcpy(FontNames,FontPath);
	strcat(FontNames, "\\Fonts\\gulim.ttc");

	if (_access(FontNames,0) != -1)	{ g_base_font[RBASE_FONT_GULIM] = 1; }
	else							{ g_base_font[RBASE_FONT_GULIM] = 0; }

	strcpy(FontNames,FontPath);
	strcat(FontNames, "\\Fonts\\batang.ttc");

	if (_access(FontNames,0) != -1)	{ g_base_font[RBASE_FONT_BATANG] = 1; }
	else							{ g_base_font[RBASE_FONT_BATANG] = 0; }

	//	strcpy(FontNames,FontPath);
	//	strcat(FontNames, "\\Fonts\\System.ttc");
	//	if (_access(FontNames,0) != -1)	{ g_font[RBASE_FONT_BATANG] = 1; }
	//	else							{ g_font[RBASE_FONT_BATANG] = 0; }

	if(g_base_font[RBASE_FONT_GULIM]==0 && g_base_font[RBASE_FONT_BATANG]==0) {//µ—¥Ÿæ¯¿∏∏ÅE.

		if( _access("_Font",0) != -1) { // ¿ÃπÃ ±‚∑œµ«æÅE¿÷¥Ÿ∏ÅE.
			_GetFileFontName( g_UserDefineFont );
		}
		else {

			int hr = IDOK;

			//hr = ::MessageBox(NULL,"±Õ«œ¿« ƒƒ«ª≈Õø°¥¬ ∞«¡˚Ã° ªÁøÅEœ¥¬ (±º∏≤,µ∏øÅE ∆˘∆Æ∞° æ¯¥¬ ∞Õ ∞∞Ω¿¥œ¥Ÿ.\n ¥Ÿ∏• ∆˘∆Æ∏¶ º±≈√ «œΩ√∞⁄Ω¿¥œ±ÅE","æÀ∏≤",MB_OKCANCEL);
			//hr = ::MessageBox(NULL,"±Õ«œ¿« ƒƒ«ª≈Õø°¥¬ ∞«¡˚Ã° ªÁøÅEœ¥¬ (±º∏≤,µ∏øÅE ∆˘∆Æ∞° æ¯¥¬ ∞Õ ∞∞Ω¿¥œ¥Ÿ.\n ∞Ëº” ¡¯«ÅE«œΩ√∞⁄Ω¿¥œ±ÅE","æÀ∏≤",MB_OKCANCEL);

			if(hr==IDOK) {
				/*			
				CFontDialog dlg;
				if(dlg.DoModal()==IDOK) {
				CString facename = dlg.GetFaceName();
				lstrcpy((LPSTR)g_UserDefineFont,(LPSTR)facename.operator const char*());

				hr = ::MessageBox(NULL,"º±≈√«œΩ≈ ∆˘∆Æ∏¶ ¿˙¿ÅE«œΩ√∞⁄Ω¿¥œ±ÅE","æÀ∏≤",MB_OKCANCEL);

				if(hr==IDOK)
				_SetFileFontName(g_UserDefineFont);
				}
				*/
				return true;
			}
			else {
				return false;
			}
		}
	}
	return true;
}

#include "shlobj.h"

void CheckFileAssociation()
{
#define GUNZ_REPLAY_CLASS_NAME	"GunzReplay"

	// √º≈©«ÿ∫¡º≠ µ˚”œ¿Ã æ»µ«æ˚‹÷¿∏∏ÅEµ˚”œ«—¥Ÿ. ªÁøÅE⁄ø°∞‘ π∞æ˚÷ººˆµµ ¿÷∞⁄¥Ÿ.
	char szValue[256];
	if(!MRegistry::Read(HKEY_CLASSES_ROOT,"." GUNZ_REC_FILE_EXT,NULL,szValue))
	{
		MRegistry::Write(HKEY_CLASSES_ROOT,"." GUNZ_REC_FILE_EXT,NULL,GUNZ_REPLAY_CLASS_NAME);

		char szModuleFileName[_MAX_PATH] = {0,};
		GetModuleFileName(NULL, szModuleFileName, _MAX_DIR);

		char szCommand[_MAX_PATH];
		sprintf(szCommand,"\"%s\" \"%%1\"",szModuleFileName);

		MRegistry::Write(HKEY_CLASSES_ROOT,GUNZ_REPLAY_CLASS_NAME"\\shell\\open\\command",NULL,szCommand);

		SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_FLUSH, NULL, NULL);
	}
}

// «ÿ¥ÅE≈ÿΩ∫∆Æ ∆ƒ¿œø° «ÿ¥ÅE±€±Õ∞° ¿÷¿∏∏ÅEXTRAP ≈◊Ω∫∆Æ ƒ⁄µÂ∞° ºˆ«‡µ»¥Ÿ. (µﬁ±∏∏€) //
bool CheckXTrapIsGetTestCode()														// add sgk 0621
{
	char szBuf[256] = "";
	FILE* fp = fopen("XTrapTest.txt", "rt");
	if (fp)
	{
		fgets(szBuf, 256, fp);
		mlog("XTrapTest.txt : \n");
		mlog(szBuf);
		mlog("\n");
		fclose(fp);

		if (stricmp(szBuf, "RUN_XTRAP_TEST_CODE") == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		mlog("fail to open XTrapTest.txt\n");
		return false;
	}
}

void OnGetXTrapRealCodeArgv(char* szTemp, bool* bPatchSkip)							// add sgk 0621
{
	/* LOCALE_KOREA */
	wsprintf(szTemp, "660970B478F9CB6395356D98440FE8629F86DDCD4DBB54B513680296C770DFCDB63DCCFE8DFE34B3A6B2BAA8CFB5C8C9761A2D3F1E491F9EFF6757489912B1AB0F7D04245246E409A061005FC9527B0823FF73E0B9F4C91EB67C40AC1A16EABB8FE52BDE25C76F6955E9E52A0812CE");

#ifdef LOCALE_JAPAN
	memset(szTemp, 0, 256);
	wsprintf(szTemp, "660970B45869CA6395356D98440FE8624C8FEA6EF181FD7D095D6CBA9911AFB0B5661B972C3C82BB0FF2D47A32DFB56D407CB146190E29B1EA46F49C1E86160F0F7D04245246E409A061005FC9527B086EF578A8BCFCC91FB67C51F65E05AAB85F7E306086BDFF03DF1BA46A66C605FFBC6263206088B68D6930514A");
#endif

#ifdef LOCALE_US
	memset(szTemp, 0, 256);
	wsprintf(szTemp, "660970B497F9CB6395356D98440FE8629AE854BDDBD13EDCE69AC1898F7A23CEF138AD2BF2758B368950133F1B021D0D218BFB058146B32450591F8B22CBE6A2");
	*bPatchSkip = true;
#endif

#ifdef LOCALE_INDIA
	memset(szTemp, 0, 256);
//	wsprintf(szTemp, "660970B47C69CB6795356D98440FE8625582AC40166A109C00E4D6A6056D18A02BBAC0A19DA6BEE6B4D43AD07CFB61697FD7FF586D98ECFF1DA11222DD82028D0F7D04245246E417A4610E569557620395165EECCBF7CD9008C4C0120CA7A0AD9D568C0DC8C7BD38C629B7EAAE5435B46105721F036F7C5BF0");
	wsprintf(szTemp, "660970B47C69CB6795356D98490FE862FEBC286C65D77538F80891D97D18B65B43E538B6EADB14290A04CF119B162DE7AA91984B54023E368FB4C25D4A91F68A0F7D04245246E417A4610E569557620395165EECCBF7CD9008C4C0120CA7A0AD9D568C0DC8C7BD38C629B7EAAE5435B46105721F036F7C5BF0");
#endif

#ifdef LOCALE_BRAZIL
	memset(szTemp, 0, 256);
	wsprintf(szTemp, "660970B448FBCB6395356D98440FE8621A6EADB8532B3C5F1949386F921C6C0970FEF0A168B5352668BE414ADF1375136173F493A8A2C075AC0F919AC7241A650F7D04245246E401B574195DD31E6305975703051B9F4F5CA2A8046A5FF3331AB0C8F040AFA98BB5CE3134520AC79D1328E836DF645FC479");
#endif
}

void OnGetXTrapTestCodeArgv(char* szTemp, bool* bPatchSkip)							// add sgk 0621
{
	/* LOCALE_KOREA */
	wsprintf(szTemp, "660970B478F9CB6395356D98440FE8629F86DDCD4DBB54B513680296C770DFCDB63DCCFE8DFE34B3A6B2BAA8CFB5C8C9761A2D3F1E491F9EFF6757489912B1AB0F7D04245246E409A061005FC9527B0823FF73E0B9F4C91EB67C40AC1A16EABB8FE52BDE25C76F6955E9E52A0812A88323D4");

#ifdef LOCALE_JAPAN
	memset(szTemp, 0, 256);
	wsprintf(szTemp, "660970B45869CA6395356D98440FE8624C8FEA6EF181FD7D095D6CBA9911AFB0B5661B972C3C82BB0FF2D47A32DFB56D407CB146190E29B1EA46F49C1E86160F0F7D04245246E409A061005FC9527B086EF578A8BCFCC91FB67C51F65E05AAB85F7E306086BDFF03DF1BA46A66C605FFBC6263206088B68D6930512C295649");
#endif

#ifdef LOCALE_US
	memset(szTemp, 0, 256);
	wsprintf(szTemp, "660970B497F9CB6395356D98440FE8629AE854BDDBD13EDCE69AC1898F7A23CEF138AD2BF2758B368950133F1B021D0D218BFB058146B32450591F8B22CBE6A2");
	*bPatchSkip = true;
#endif

#ifdef LOCALE_INDIA
	memset(szTemp, 0, 256);
//	wsprintf(szTemp, "660970B47C69CB6795356D98440FE8625582AC40166A109C00E4D6A6056D18A02BBAC0A19DA6BEE6B4D43AD07CFB61697FD7FF586D98ECFF1DA11222DD82028D0F7D04245246E417A4610E569557620395165EECCBF7CD9008C4C0120CA7A0AD9D568C0DC8C7BD38C629B7EAAE5435B46105721F036F7C5B962980B7");
	wsprintf(szTemp, "660970B47C69CB6795356D98490FE862FEBC286C65D77538F80891D97D18B65B43E538B6EADB14290A04CF119B162DE7AA91984B54023E368FB4C25D4A91F68A0F7D04245246E417A4610E569557620395165EECCBF7CD9008C4C0120CA7A0AD9D568C0DC8C7BD38C629B7EAAE5435B46105721F036F7C5B962980B7");
#endif

#ifdef LOCALE_BRAZIL
	memset(szTemp, 0, 256);
	wsprintf(szTemp, "660970B448FBCB6395356D98440FE8621A6EADB8532B3C5F1949386F921C6C0970FEF0A168B5352668BE414ADF1375136173F493A8A2C075AC0F919AC7241A650F7D04245246E401B574195DD31E6305975703051B9F4F5CA2A8046A5FF3331AB0C8F040AFA98BB5CE3134520AC79D1328E836DF645FC41F2B9A7E");
#endif
}
#ifdef _LAUNCHER
void UpgradeMrsFile()
{
	char temp_path[1024];
	sprintf(temp_path, "*");

	FFileList file_list;
	GetFindFileListWin(temp_path, ".mrs", file_list);
	file_list.UpgradeMrs();
}
#endif
HANDLE Mutex = 0;

#ifdef _HSHIELD
int __stdcall AhnHS_Callback(long lCode, long lParamSize, void* pParam);
#endif

DWORD g_dwMainThreadID;


//------------------------------------------- nhn usa -------------------------------------------------------------
bool InitReport()
{
	return true;

	// Custom: Disable NHN Auth
	/*
#ifdef LOCALE_NHNUSA
	mlog( "Init report start\n" );
	if( !GetNHNUSAReport().InitReport(((ZNHN_USAAuthInfo*)(ZGetLocale()->GetAuthInfo()))->GetUserID().c_str(),
		((ZNHN_USAAuthInfo*)(ZGetLocale()->GetAuthInfo()))->GetGameStr()) )
	{
		mlog( "Init nhn report fail.\n" );
		return false;
	}
	GetNHNUSAReport().ReportStartGame();
	mlog( "Init report success.\n" );
#endif

	return true;
	*/
}

bool InitPoll()
{
	return true;

	// Custom: Disable NHN Auth
	/*
#ifdef LOCALE_NHNUSA
	mlog( "Init poll start\n" );

	((ZNHN_USAAuthInfo*)(ZGetLocale()->GetAuthInfo()))->ZUpdateGameString();

	if( !GetNHNUSAPoll().ZHanPollInitGameString( ((ZNHN_USAAuthInfo*)(ZGetLocale()->GetAuthInfo()))->GetGameStr()) )
		return false;
#endif

	return true;
	*/
}


bool CheckGameGuardHackToolUser()
{
#ifdef _GAMEGUARD

	string strUserID;
	ZBaseAuthInfo* pAuth = ZGetLocale()->GetAuthInfo();

	if ( pAuth == NULL)
		return true;


#ifdef LOCALE_NHNUSA

#ifdef _DEBUG_NHN_USA_AUTH
	return true;
#endif

	strUserID = ((ZNHN_USAAuthInfo*)pAuth)->GetUserID();

#elif LOCALE_JAPAN

//	strUserID = ((ZGameOnJPAuthInfo*)pAuth)->GetUserID();
	return true;

#endif


	if( !GetZGameguard().CheckHackToolUser( strUserID.c_str()) )
		return false;


#endif	// _GAMEGUARD

	return true;
}


//------------------------------------------- nhn usa end----------------------------------------------------------

BOOL SetDumpPrivileges()
{
	BOOL       fSuccess  = FALSE;
	HANDLE      TokenHandle = NULL;
	TOKEN_PRIVILEGES TokenPrivileges;

	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
		&TokenHandle))
	{
		mlog("Dmp token fail\n");
		//printf("Could not get the process token");
		goto Cleanup;
	}

	TokenPrivileges.PrivilegeCount = 1;

	if (!LookupPrivilegeValue(NULL,
		SE_DEBUG_NAME,
		&TokenPrivileges.Privileges[0].Luid))
	{
		mlog("Dmp lookup fail\n");
		//printf("Couldn't lookup SeDebugPrivilege name");
		goto Cleanup;
	}

	TokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	//Add privileges here.
	if (!AdjustTokenPrivileges(TokenHandle,
		FALSE,
		&TokenPrivileges,
		sizeof(TokenPrivileges),
		NULL,
		NULL))
	{
		mlog("Dmp adjust fail\n");
		//printf("Could not revoke the debug privilege");
		goto Cleanup;
	}

	fSuccess = TRUE;

Cleanup:

	if (TokenHandle)
	{
		CloseHandle(TokenHandle);
	}

	return fSuccess;
}
int PASCAL WinMain(HINSTANCE this_inst, HINSTANCE prev_inst, LPSTR cmdline, int cmdshow)
{
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_CRT_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_DELAY_FREE_MEM_DF);

	InitLog(MLOGSTYLE_DEBUGSTRING|MLOGSTYLE_FILE);

	g_fpOnCrcFail = CrcFailExitApp;

#ifdef LOCALE_JAPAN
	ZGameOnJPAuthInfo::m_hLauncher = ::FindWindow( NULL, TITLE_PUBLAGENT );
#endif

	g_dwMainThreadID = GetCurrentThreadId();
	
#ifdef _MTRACEMEMORY
	MInitTraceMemory();
#endif

	//_CrtSetBreakAlloc(994464);

	// Current Directory∏¶ ∏¬√·¥Ÿ.
	char szModuleFileName[_MAX_DIR] = {0,};
	GetModuleFileName(NULL, szModuleFileName, _MAX_DIR);
	PathRemoveFileSpec(szModuleFileName);
	SetCurrentDirectory(szModuleFileName);

//#define _MULTICLIENT 1

#if defined(_MULTICLIENT)
	if (!fopen("mutex", "r"))
		Mutex = CreateMutex(NULL, TRUE, "Gunz");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		MessageBox(g_hWnd, "Cannot Open Gunz.exe", NULL, MB_OK);
		mlog("Cannot Open Gunz.exe\n");
		//zexit(-1);
		ExitProcess(NULL);
		return 0;
	}
#endif

#ifdef _HSHIELD

	TCHAR szFullFileName[_MAX_DIR];
	GetCurrentDirectory( sizeof( szFullFileName), szFullFileName);
	strcat( szFullFileName, "\\HShield\\EhSvc.dll");

	int nRet = 0;

#ifdef _DEBUG
#define AHNHS_CHKOPT_GUNZ		AHNHS_CHKOPT_SPEEDHACK
#else
#define AHNHS_CHKOPT_GUNZ		AHNHS_CHKOPT_ALL
#endif

#ifdef  LOCALE_BRAZIL					/* Brazil */
	nRet = _AhnHS_Initialize(szFullFileName, AhnHS_Callback, 4001, "DA0EF49C0F6D029F", AHNHS_CHKOPT_GUNZ
		| AHNHS_ALLOW_SVCHOST_OPENPROCESS | AHNHS_ALLOW_LSASS_OPENPROCESS | AHNHS_ALLOW_CSRSS_OPENPROCESS | AHNHS_DONOT_TERMINATE_PROCESS
		, AHNHS_SPEEDHACK_SENSING_RATIO_NORMAL);

#elif  LOCALE_INDIA						/* India */
	nRet = _AhnHS_Initialize(szFullFileName, AhnHS_Callback, 4003, "CC4686AE10F63E4A", AHNHS_CHKOPT_GUNZ
		| AHNHS_ALLOW_SVCHOST_OPENPROCESS | AHNHS_ALLOW_LSASS_OPENPROCESS | AHNHS_ALLOW_CSRSS_OPENPROCESS | AHNHS_DONOT_TERMINATE_PROCESS
		, AHNHS_SPEEDHACK_SENSING_RATIO_NORMAL);
#endif

#ifndef _DEBUG
	// æ∆∑° ƒ⁄µÂ¥¬ ∏±∏Æ¡˚Ÿ√ _AhnHS_Initialize ø…º«¿Ã AHNHS_CHKOPT_ALL∑Œ µ«æ˚‹÷¡ÅEæ ¿ª ∞ÊøÅE
	// ƒƒ∆ƒ¿œ ≈∏¿”ø° ø°∑Ø∏¶ ¿œ¿∏ƒ—º≠ æÀ ºÅE¿÷∞‘ «—¥Ÿ.
	void* OptionCheckTool[(AHNHS_CHKOPT_GUNZ == AHNHS_CHKOPT_ALL)?1:0];
#endif

	

	//æ∆∑° ø°∑Ø¥¬ ∞≥πﬂ∞˙¡§ø°º≠∏∏ πﬂª˝«“ ºÅE¿÷¿∏∏ÅE
	//¿Ã»ƒ ∞·ƒ⁄ πﬂª˝«ÿº≠¥¬ æ»µ«¥¬ ø°∑Ø¿Ãπ«∑Œ assert√≥∏Æ∏¶ «ﬂΩ¿¥œ¥Ÿ.
	assert(nRet != HS_ERR_INVALID_PARAM);
	assert(nRet != HS_ERR_INVALID_LICENSE);
	assert(nRet != HS_ERR_ALREADY_INITIALIZED);

	TCHAR szTitle[256] = "Hack Shield Error";

	if (nRet != HS_ERR_OK) 
	{
		//Error √≥∏Æ 
		switch(nRet)
		{
		case HS_ERR_ANOTHER_SERVICE_RUNNING:
			{
//				MessageBox(NULL, _T("¥Ÿ∏• ∞‘¿”¿Ã Ω««‡¡ﬂ¿‘¥œ¥Ÿ.\n«¡∑Œ±◊∑•¿ª ¡æ∑·«’¥œ¥Ÿ."), szTitle, MB_OK);
				mlog( "¥Ÿ∏• ∞‘¿”¿Ã Ω««‡¡ﬂ¿‘¥œ¥Ÿ. «¡∑Œ±◊∑•¿ª ¡æ∑·«’¥œ¥Ÿ.\n");
				break;
			}
		case HS_ERR_INVALID_FILES:
			{
//				MessageBox(NULL, _T("¿ﬂ∏¯µ» ∆ƒ¿œ º≥ƒ°µ«æ˙Ω¿¥œ¥Ÿ.\n«¡∑Œ±◊∑•¿ª ¿Áº≥ƒ°«œΩ√±ÅEπŸ∂¯¥œ¥Ÿ."), szTitle, MB_OK);
				mlog( "¿ﬂ∏¯µ» ∆ƒ¿œ º≥ƒ°µ«æ˙Ω¿¥œ¥Ÿ. «¡∑Œ±◊∑•¿ª ¿Áº≥ƒ°«œΩ√±ÅEπŸ∂¯¥œ¥Ÿ.\n");
				break;
			}
		case HS_ERR_DEBUGGER_DETECT:
			{
//				MessageBox(NULL, _T("ƒƒ«ª≈Õø°º≠ µπˆ∞≈ Ω««‡¿Ã ∞®¡ˆµ«æ˙Ω¿¥œ¥Ÿ.\nµπˆ∞≈¿« Ω««‡¿ª ¡ﬂ¡ˆΩ√≈≤ µ⁄ø° ¥ŸΩ√ Ω««‡Ω√ƒ—¡÷Ω√±‚πŸ∂¯¥œ¥Ÿ."), szTitle, MB_OK);
				mlog( "ƒƒ«ª≈Õø°º≠ µπˆ∞≈ Ω««‡¿Ã ∞®¡ˆµ«æ˙Ω¿¥œ¥Ÿ. µπˆ∞≈¿« Ω««‡¿ª ¡ﬂ¡ˆΩ√≈≤ µ⁄ø° ¥ŸΩ√ Ω««‡Ω√ƒ—¡÷Ω√±‚πŸ∂¯¥œ¥Ÿ.\n");
				break;
			}
		case HS_ERR_NEED_ADMIN_RIGHTS:
			{
//				MessageBox(NULL, _T("Admin ±««—¿∏∑Œ Ω««‡µ«æ˚⁄ﬂ «’¥œ¥Ÿ.\n«¡∑Œ±◊∑•¿ª ¡æ∑·«’¥œ¥Ÿ."), szTitle, MB_OK);
				mlog( "Admin ±««—¿∏∑Œ Ω««‡µ«æ˚⁄ﬂ «’¥œ¥Ÿ. «¡∑Œ±◊∑•¿ª ¡æ∑·«’¥œ¥Ÿ.\n");
				break;
			}
		case HS_ERR_COMPATIBILITY_MODE_RUNNING:
			{
//				MessageBox(NULL, _T("»£»Øº∫ ∏µÂ∑Œ «¡∑Œ±◊∑•¿Ã Ω««‡¡ﬂ¿‘¥œ¥Ÿ.\n«¡∑Œ±◊∑•¿ª ¡æ∑·«’¥œ¥Ÿ."), szTitle, MB_OK);
				mlog( "»£»Øº∫ ∏µÂ∑Œ «¡∑Œ±◊∑•¿Ã Ω««‡¡ﬂ¿‘¥œ¥Ÿ. «¡∑Œ±◊∑•¿ª ¡æ∑·«’¥œ¥Ÿ.\n");
				break;				
			}
		default:
			{
				TCHAR szMsg[255];
				wsprintf(szMsg, _T("«ÿ≈∑πÊ¡ÅE±‚¥…ø° πÆ¡¶∞° πﬂª˝«œø¥Ω¿¥œ¥Ÿ.(Error Code = %x)\n«¡∑Œ±◊∑•¿ª ¡æ∑·«’¥œ¥Ÿ."), nRet);
//				MessageBox(NULL, szMsg, szTitle, MB_OK);
				mlog( szMsg);
				break;
			}
		}

		return FALSE;
	}

	//Ω√¿€ «‘ºÅE»£√ÅE
	nRet = _AhnHS_StartService();

	assert(nRet != HS_ERR_NOT_INITIALIZED);
	assert(nRet != HS_ERR_ALREADY_SERVICE_RUNNING);

	if (nRet != HS_ERR_OK)
	{
//		TCHAR szMsg[255];
		mlog(_T("«ÿ≈∑πÊ¡ÅE±‚¥…ø° πÆ¡¶∞° πﬂª˝«œø¥Ω¿¥œ¥Ÿ.(Error Code = %x)\n«¡∑Œ±◊∑•¿ª ¡æ∑·«’¥œ¥Ÿ."), nRet);
//		MessageBox(NULL, szMsg, szTitle, MB_OK);

		return FALSE;
	}

	nRet = _AhnHS_SaveFuncAddress(6, _AhnHS_Initialize, _AhnHS_StartService,			// «ŸΩ«µÅE«‘ºÅE
										ZCheckHackProcess, ZGetMZFileChecksum,				// «Ÿ∆«∫∞ ¿⁄√º«‘ºÅE
										ZSetupDataChecker_Global, ZSetupDataChecker_Game);
	if(nRet != ERROR_SUCCESS)
	{
//		AfxMessageBox(_T("_AhnHS_SaveFuncAddress Failed!"));
		return FALSE;
	}
#endif // _HSHIELD


#ifdef _XTRAP														// update sgk 0702 start
	mlog("XTRAP Start\n");

	char szTemp[256] = {0,};
	bool bPatchSkip = false;

	bool bIsXtrapTest = CheckXTrapIsGetTestCode();
	if (bIsXtrapTest)
	{
		// XTRAP ≈◊Ω∫∆ÆøÅE∆ƒ¿œ∞Ê∑Œ∑Œ Ω««‡µ»¥Ÿ. //
		OnGetXTrapTestCodeArgv(szTemp, &bPatchSkip);
	}
	else
	{
		// XTRAP ∏ÆæÅEº≠πˆøÅE∆ƒ¿œ∞Ê∑Œ∑Œ Ω««‡µ»¥Ÿ. //
		OnGetXTrapRealCodeArgv(szTemp, &bPatchSkip);
	}

	if (!bPatchSkip)
	{
		XTrap_L_Patch(szTemp, szModuleFileName, 60);
	}

	XTrap_C_Start(szTemp, NULL);
	XTrap_C_KeepAlive();

	mlog("XTRAP End\n");
#endif																// update sgk 0702 end

	ClearTrashFiles();

	srand( (unsigned)time( NULL ));

	// Custom: Added defined(LOCALE_NHNUSA)
#if defined(LOCALE_BRAZIL) || defined(LOCALE_INDIA) || defined(LOCALE_US) || defined(LOCALE_KOREA) || defined(LOCALE_NHNUSA)
	//#ifndef _DEBUG
	#ifdef _PUBLISH
		// GunzLock¿ª ∂Áøˆ≥ı∞ÅEGunz.exe∏¶ Ω««‡«œ∏ÅE¡æ∑·¡˜¿ÅE¥ÅE‚«—¥Ÿ. (XProtector «¡∑ŒººΩ∫¿ÃπÃ¡ˆΩ∫ƒµ ¿€æ˜øÅE
	#endif
#endif	// LOCALE_NHNUSA

	// ∑Œ±◊ Ω√¿€
	//mlog("GUNZ " STRFILEVER " launched. build (" __DATE__ " " __TIME__ ") \n");
	//char szDateRun[128]="";
	//char szTimeRun[128]="";
	//_strdate( szDateRun );
	//_strtime( szTimeRun );
	//mlog("Log time (%s %s)\n", szDateRun, szTimeRun);
	//mlog("Coded By Desperate\n"); // Custom: My runnable string

#ifndef _PUBLISH
	mlog("cmdline = %s\n",cmdline);

#endif

#ifdef _LAUNCHER
	UpgradeMrsFile();
#endif

	MSysInfoLog();


	// Custom: Force unsupported OS
	OSVERSIONINFOEX os;
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	GetVersionEx( (OSVERSIONINFO*)&os );

	if (os.dwMajorVersion <= 4)
	{
		mlog("Unsupported OS... exiting.\n");
		return false;
	}

	bool bCompatibilityMode = false;
	char szModule[MAX_PATH] = { 0 };
	GetModuleFileName(GetModuleHandle(NULL), szModule, sizeof(szModule));
	
	HKEY hKey;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers", NULL, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		char szValue[MAX_PATH] = { 0 };
		
		DWORD nLen = _MAX_PATH;
		DWORD dwType = REG_SZ;

		if (RegQueryValueEx(hKey, szModule, NULL, &dwType, (unsigned char *)szValue, &nLen) == ERROR_SUCCESS)
		{
			if (strstr(szValue, "WINXPSP2") || strstr(szValue, "WINXPSP3") || strstr(szValue, "VISTARTM") || strstr(szValue, "VISTASP1") || strstr(szValue, "VISTASP2") || strstr(szValue, "WIN7RTM") || strstr(szValue, "WIN8RTM"))
			{
				bCompatibilityMode = true;
			}
		}

		RegCloseKey(hKey);
	}

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers", NULL, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		char szValue[MAX_PATH] = { 0 };
		
		DWORD nLen = _MAX_PATH;
		DWORD dwType = REG_SZ;

		if (RegQueryValueEx(hKey, szModule, NULL, &dwType, (unsigned char *)szValue, &nLen) == ERROR_SUCCESS)
		{
			if (strstr(szValue, "WINXPSP2") || strstr(szValue, "WINXPSP3") || strstr(szValue, "VISTARTM") || strstr(szValue, "VISTASP1") || strstr(szValue, "VISTASP2") || strstr(szValue, "WIN7RTM") || strstr(szValue, "WIN8RTM"))
			{
				bCompatibilityMode = true;
			}
		}

		RegCloseKey(hKey);
	}

	if (bCompatibilityMode)
	{
		mlog("ERROR: Running under compatibility mode.\n");
		MessageBox(g_hWnd, "Failed to run GunZ in compatibility mode.\nPlease remove compatibility mode settings and launch the game again!", "GunZ", MB_OK | MB_ICONERROR);
		return false;
	}

//	if (CheckVideoAdapterSupported() == false)
//		return 0;

	CheckFileAssociation();

	// Initialize MZFileSystem - MUpdate 
	MRegistry::szApplicationName=APPLICATION_NAME;

	g_App.InitFileSystem();

//	mlog("CheckSum: %u \n", ZApplication::GetFileSystem()->GetTotalCRC());

	//if(!InitializeMessage(ZApplication::GetFileSystem())) {
	//	MLog("Check Messages.xml\n");
	//	return 0;
	//}

//	≥›∏∂∫ÅEπˆ¿ÅE∫ ±∏∫–«ÿæﬂ«‘... ≥›∏∂∫ÅEπˆ¿ÅE∫ MZIPREADFLAG_MRS1 µµ ¿–æ˚⁄ﬂ«‘...

#ifdef _PUBLISH
//	#ifndef NETMARBLE_VERSION
		//MZFile::SetReadMode( MZIPREADFLAG_MRS2 );
//	#endif
#endif


#ifdef LOCALE_NHNUSA
	// NHNUSA¥¬ ƒø∏«µÂ∂Û¿Œ¿∏∑Œ ææ˚ÿ±≈√¿ª æÀ∑¡¡ÿ¥Ÿ, ¥Ÿ∏• ¡ˆø™∫ÙµÂ∂Ûµµ µ∆ƒ¿Œ « ø‰æ¯¿Ω
	ZGetLanguageSetting_forNHNUSA()->SetLanguageIndexFromCmdLineStr(cmdline);
#endif

	// configøÕ string ∑ŒµÅE
	ZGetConfiguration()->Load();

	CreateRGMain();

	ZGetConfiguration()->Load();

	if (os.dwMajorVersion >= 6 && Z_ETC_D3D9E) {
		g_isDirect3D9ExEnabled = true;
	}

	ZStringResManager::MakeInstance();
	if( !ZApplication::GetInstance()->InitLocale() )
	{
		MLog("Locale Init error !!!\n");
		return false;
	}

	ZGetConfiguration()->Load_StringResDependent();

	// ø©±‚º≠ ∏ﬁ≈©∑Œ ƒ¡πˆ∆√... ∏’∞° ±∏∏Æ±∏∏Æ~~ -by SungE.
	if( !ZGetConfiguration()->LateStringConvert() )
	{
		MLog( "main.cpp - Late string convert fale.\n" );
		return false;
	}

	DWORD ver_major = 0;
	DWORD ver_minor = 0;
	TCHAR ver_letter = ' ';

	// ¿«πÃæ¯¿Ω ... ø‹∫Œø°º≠ dll ¿Ã æ¯¥Ÿ∞ÅE∏’¿˙∂ÅE..

/*_
	bool DXCheck = false;

	if( SUCCEEDED( GetDirectXVersionViaDxDiag( &ver_major, &ver_minor, &ver_letter ) ) ) {
		if(ver_major >= 8)
			DXCheck = true;
	} // º∫∞ÅE∏¯«— ∞ÊøÅEæÀºˆæ¯¿∏π«∑Œ Ω«∆–~

	if(DXCheck==false) {
		::MessageBox(NULL,"DirectX 8.0 ¿ÃªÛ¿ª º≥ƒ°«œ∞ÅE¥ŸΩ√ Ω««‡«ÿ ¡÷Ω√±ÅEπŸ∂¯¥œ¥Ÿ.","æÀ∏≤",MB_OK);
	}
*/

#ifdef SUPPORT_EXCEPTIONHANDLING
	if (SetDumpPrivileges())
		mlog("Dmp ok.\n");
	else
		mlog("Dmp - FAILED\n");

	char szDumpFileName[256];
	sprintf(szDumpFileName, "Gunz.dmp");
	__try{
#endif

	if (ZApplication::GetInstance()->ParseArguments(cmdline) == false)
	{
		// Korean or Japan Version
		if ((ZGetLocale()->GetCountry() == MC_KOREA) || (ZGetLocale()->GetCountry() == MC_JAPAN))
		{
			mlog("Routed to Website \n");

			ShellExecute(NULL, "open", ZGetConfiguration()->GetLocale()->szHomepageUrl, NULL, NULL, SW_SHOWNORMAL);

			char szMsgWarning[128]="";
			char szMsgCertFail[128]="";
			ZTransMsg(szMsgWarning,MSG_WARNING);
			ZTransMsg(szMsgCertFail,MSG_REROUTE_TO_WEBSITE);
//			MessageBox(g_hWnd, szMsgCertFail, szMsgWarning, MB_OK);

			mlog(szMsgWarning);
			mlog(" : ");
			mlog(szMsgCertFail);

			return 0;
		}
		else
		{
			return 0;
		}
	}

//#ifdef _PUBLISH
	// if(!CheckFileList()) {
		// ¡æ∑·«œ¥¬∞Õ¿∫ ¿œ¥‹ ∫∏∑ÅE
		// int ret=MessageBox(NULL, "∆ƒ¿œ¿Ã º’ªÛµ«æ˙Ω¿¥œ¥Ÿ.", "¡ﬂøÅE", MB_OK);
		// return 0;
	//}
//#endif

	// Custom: Disable NHN Auth
	/*
#ifdef LOCALE_NHNUSA
	BYTE SHA_HanAuthForClient[20] = {0x29,0xc0,0x7e,0x6b,0x8d,0x1d,0x30,0xd2,0xed,0xac,0xaf,0xea,0x78,0x16,0x51,0xf0,0x50,0x52,0x26,0x91};
	BYTE SHA_hanpollforclient[20] = {0x09,0x04,0x51,0x9d,0x95,0xbb,0x66,0x2a,0xfb,0x93,0x87,0x2d,0x21,0xa2,0x93,0x1d,0x6a,0xcb,0xa5,0x4f};
	BYTE SHA_HanReportForClient[20] = {0x4c,0x62,0xaf,0x4d,0x5b,0x54,0xb8,0x96,0x46,0x66,0x8f,0x1e,0x12,0xe7,0xf2,0xd7,0xe4,0x58,0x65,0xc9}; 
	if(!CheckDll("hanauthforclient.dll", SHA_HanAuthForClient) || 
	   !CheckDll("hanpollforclient.dll", SHA_hanpollforclient) ||
	   !CheckDll("hanreportforclient.dll", SHA_HanReportForClient) )
	{
		MessageBox(g_hWnd,"Dll Hacking detected",  NULL, MB_OK);
		return false;
	}

	if( !InitReport() ) 
		return 0;


	if ( !InitPoll())
		return 0;
#endif
	*/



#ifndef _DEBUG // Custom: Remove HASH DLL
/*	BYTE SHA_fmod[20] = {0x88,0x8f,0x1f,0x7b,0x7e,0x3c,0x43,0x38,0x4f,0x4b,0x80,0xb5,0x77,0xfe,0x09,0x1a,0xc0,0x45,0x38,0x3c};
	BYTE SHA_dbghelp[20] = {0x8d,0x12,0xc4,0x3a,0x84,0x12,0xc2,0x1,0x58,0xb4,0x46,0x70,0x9,0x5,0xcb,0xd4,0xfa,0xb1,0xe2,0x4b}; 
	if(!CheckDll("fmod.dll", SHA_fmod) ||
		!CheckDll("dbghelp.dll", SHA_dbghelp) )
	{
		MessageBox(g_hWnd,"Dll Hacking detected",  NULL, MB_OK);
		return false;
	}*/

#endif

// Custom: Made hack check automatic (w/o debug) and enabled for LOCALE_NHNUSA
	if (ZCheckHackProcess() == true)
	{
		//		MessageBox(NULL,
		//			ZMsg(MSG_HACKING_DETECTED), ZMsg( MSG_WARNING), MB_OK);
		return 0;
	}

	if(!InitializeNotify(ZApplication::GetFileSystem())) 
	{
		MLog("Check notify.xml\n");
		return 0;
	}
	else 
	{
		mlog( "InitializeNotify ok.\n" );
	}



	if(CheckFont()==false) {
		MLog("∆˘∆Æ∞° æ¯¥¬ ¿Ø¿˙∞° ∆˘∆Æ º±≈√¿ª √ÅE“\n");
		return 0;
	}

	RSetFunction(RF_CREATE	,	OnCreate);
	RSetFunction(RF_RENDER	,	OnRender);
	RSetFunction(RF_UPDATE	,	OnUpdate);
	RSetFunction(RF_DESTROY ,	OnDestroy);
	RSetFunction(RF_INVALIDATE,	OnInvalidate);
	RSetFunction(RF_RESTORE,	OnRestore);
	RSetFunction(RF_ACTIVATE,	OnActivate);
	RSetFunction(RF_DEACTIVATE,	OnDeActivate);
	RSetFunction(RF_ERROR,		OnError);

	SetModeParams();

//	while(ShowCursor(FALSE)>0);

	const int nRMainReturn = RMain(APPLICATION_NAME,this_inst,prev_inst,cmdline,cmdshow,&g_ModeParams,WndProc,IDI_ICON1);
	if( 0 != nRMainReturn )
		return nRMainReturn;


#ifdef _GAMEGUARD
	mlog("start gameguard\n");

	ZGMAEGUARD_MODE mode = ZGGM_END;
	char szArg[ 64] = "";
	ZBaseAuthInfo* pAuth = ZGetLocale()->GetAuthInfo();
	

#ifdef LOCALE_NHNUSA
	if ( ((ZNHN_USAAuthInfo*)pAuth)->IsReal())
	{
		mode = ZGGM_REAL;
		strcpy( szArg, "GunzUS");
	}
	else if ( ((ZNHN_USAAuthInfo*)pAuth)->IsAlpha())
	{
		mode = ZGGM_ALPHA;
		strcpy( szArg, "GunzUSTest");
	}
	else
	{
		mlog( "error in gameguard mode...\n" );
		zexit( -1);
		return 0;
	}

#elif LOCALE_JAPAN
	if ( ((ZGameOnJPAuthInfo*)pAuth)->IsReal())
	{
		mode = ZGGM_REAL;
		strcpy( szArg, "GUNZWEI");
	}
	else if ( ((ZGameOnJPAuthInfo*)pAuth)->IsAlpha())
	{
		mode = ZGGM_ALPHA;
		strcpy( szArg, "GUNZWEITest");
	}
	else
	{
		mlog( "error in gameguard mode...\n" );
		zexit( -1);
		return 0;
	}

	if( !((ZGameOnJPAuthInfo*)pAuth)->SendMsgToLauncher(GET_MSG_HWND_TERMINATE_PUBGAME) )
	{
		mlog("Can't find GameOn Agent");
	}
#endif	// LOCALE_JAPAN


	if( !GetZGameguard().Init( mode, szArg) )
	{
		mlog( "error init gameguard...\n" );
		zexit( -1 );
		return 0;
	}


	GetZGameguard().SetMainWnd( g_hWnd );

	if( !CheckGameGuardHackToolUser() )
		return 0;

#endif	// _GAMEGUARD


#ifdef _GAMEGUARD
	#ifdef _PUBLISH
		// ¡ﬂ∫π Ω««ÅE±›¡ÅE
		Mutex = CreateMutex(NULL, TRUE, "Gunz");
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			zexit(-1);
			return 0;
		}
	#endif
#endif



#ifdef LOCALE_NHNUSA
	// Custom: Disable NHN related
	//GetNHNUSAReport().ReportInitGameGuard();
#endif

	if( 0 != RInitD3D(&g_ModeParams) )
	{
		MessageBox(g_hWnd, "fail to initialize DirectX", NULL, MB_OK);
		mlog( "error init RInitD3D\n" );
		return 0;
	}

	const int nRRunReturn = RRun();

	//¡æ∑·Ω√ ∞‘¿”∞°µÂ∞° Xfire¿« ∏ﬁ∏∏Ææ≤±ÅEø°∑Ø∏¶ ¿Øπﬂ«œ¥¬µ• ¿Ã∂ß ø¿∑˘√¢¿Ã «ÆΩ∫≈©∏∞ µ⁄ø° ∂ﬂ¥¬ ∞Õ πÊ¡ˆ«œ±ÅE¿ß«ÿ
	//¡æ∑·¿ÅE° ∞«¡˚‘¶ √÷º“»≠/∫Ò»∞º∫»≠ Ω√ƒ—≥ı¥¬¥Ÿ. xfire¿« ¡ÅE¢¿˚¿Œ πÆ¡¶ «ÿ∞·¿ª ±‚¥ÅEœ±ÅEæ˚”¡øÅE«∑Œ ¿Ã∑∏∞‘ √≥∏Æ
	ShowWindow(g_hWnd, SW_MINIMIZE);

	g_bThreadChecker = false;
	//ahCleanup(43320);

#ifdef _GAMEGUARD
	GetZGameguard().Release();
#endif

#ifdef _MTRACEMEMORY
	MShutdownTraceMemory();
#endif

#ifdef _HSHIELD
	_AhnHS_StopService();
	_AhnHS_Uninitialize();		
#endif


#ifdef LOCALE_NHNUSA
	// Custom: Disable NHN poll
	//mlog( "Poll Process\n" );
	//int nRetPoll = GetNHNUSAPoll().ZHanPollProcess( NULL);
#endif

	ZStringResManager::FreeInstance();

	return nRRunReturn;

//	ShowCursor(TRUE);

#ifdef SUPPORT_EXCEPTIONHANDLING
	}

	//__except(MFilterException(GetExceptionInformation())){
	__except(CrashExceptionDump(GetExceptionInformation(), szDumpFileName, true))
	{
#ifdef LOCALE_NHNUSA
		// Custom: Disable NHN related
		//GetNHNUSAReport().ReportCrashedGame();
#endif

		HandleExceptionLog();
//		MessageBox(g_hWnd, "øπªÛƒ° ∏¯«— ø¿∑˘∞° πﬂª˝«ﬂΩ¿¥œ¥Ÿ.", APPLICATION_NAME , MB_ICONERROR|MB_OK);
	}
#endif

#ifdef _PUBLISH
	//if (Mutex != 0) CloseHandle(Mutex);
#endif

//	CoUninitialize();

	return 0;
}

#ifdef _HSHIELD
int __stdcall AhnHS_Callback(long lCode, long lParamSize, void* pParam)
{
//	TCHAR szTitle[256];

	switch(lCode)
	{
		//Engine Callback
	case AHNHS_ENGINE_DETECT_GAME_HACK:
		{
			TCHAR szMsg[255];
			wsprintf(szMsg, _T("¥Ÿ¿Ω ¿ßƒ°ø°º≠ «ÿ≈∑≈¯¿Ã πﬂ∞ﬂµ«æÅE«¡∑Œ±◊∑•¿ª ¡æ∑·Ω√ƒ◊Ω¿¥œ¥Ÿ.\n%s"), (char*)pParam);
//			MessageBox(NULL, szMsg, szTitle, MB_OK);
			mlog(szMsg);
			PostThreadMessage(g_dwMainThreadID, WM_QUIT, 0, 0);
			break;
		}

		//¿œ∫Œ API∞° ¿ÃπÃ »ƒ≈∑µ«æÅE¿÷¥¬ ªÛ≈¬
		//±◊∑Ø≥™ Ω«¡¶∑Œ¥¬ ¿Ã∏¶ ¬˜¥‹«œ∞ÅE¿÷±ÅE∂ßπÆø° ¥Ÿ∏• »ƒ≈∑Ω√µµ «¡∑Œ±◊∑•¿∫ µø¿€«œ¡ÅEæ Ω¿¥œ¥Ÿ.
		//¿Ã Callback¿∫ ¥‹¡ÅE∞Ê∞ÅE≥ª¡ˆ¥¬ ¡§∫∏¡¶∞ÅE¬˜ø¯ø°º≠ ¡¶∞¯µ«π«∑Œ ∞‘¿”¿ª ¡æ∑·«“ « ø‰∞° æ¯Ω¿¥œ¥Ÿ.
	case AHNHS_ACTAPC_DETECT_ALREADYHOOKED:
		{
			PACTAPCPARAM_DETECT_HOOKFUNCTION pHookFunction = (PACTAPCPARAM_DETECT_HOOKFUNCTION)pParam;
			TCHAR szMsg[255];
			wsprintf(szMsg, _T("[HACKSHIELD] Already Hooked\n- szFunctionName : %s\n- szModuleName : %s\n"), 
				pHookFunction->szFunctionName, pHookFunction->szModuleName);
			OutputDebugString(szMsg);
			break;
		}

		//Speed ∞ÅE√
	case AHNHS_ACTAPC_DETECT_SPEEDHACK:
	case AHNHS_ACTAPC_DETECT_SPEEDHACK_APP:
		{
//			MessageBox(NULL, _T("«ˆ¿ÅE¿Ã PCø°º≠ SpeedHack¿∏∑Œ ¿«Ω…µ«¥¬ µø¿€¿Ã ∞®¡ˆµ«æ˙Ω¿¥œ¥Ÿ."), szTitle, MB_OK);
			mlog("«ˆ¿ÅE¿Ã PCø°º≠ SpeedHack¿∏∑Œ ¿«Ω…µ«¥¬ µø¿€¿Ã ∞®¡ˆµ«æ˙Ω¿¥œ¥Ÿ.");
			PostThreadMessage(g_dwMainThreadID, WM_QUIT, 0, 0);
			break;
		}

		//µπˆ±ÅEπÊ¡ÅE
	case AHNHS_ACTAPC_DETECT_KDTRACE:	
	case AHNHS_ACTAPC_DETECT_KDTRACE_CHANGED:
		{
			TCHAR szMsg[255];
			wsprintf(szMsg, _T("«¡∑Œ±◊∑•ø° ¥ÅEœø© µπˆ±ÅEΩ√µµ∞° πﬂª˝«œø¥Ω¿¥œ¥Ÿ. (Code = %x)\n«¡∑Œ±◊∑•¿ª ¡æ∑·«’¥œ¥Ÿ."), lCode);
//			MessageBox(NULL, szMsg, szTitle, MB_OK);
			mlog(szMsg);
			PostThreadMessage(g_dwMainThreadID, WM_QUIT, 0, 0);
			break;
		}

		//±◊ø‹ «ÿ≈∑ πÊ¡ÅE±‚¥… ¿ÃªÅE
	case AHNHS_ACTAPC_DETECT_AUTOMOUSE:
	case AHNHS_ACTAPC_DETECT_DRIVERFAILED:
	case AHNHS_ACTAPC_DETECT_HOOKFUNCTION:
	case AHNHS_ACTAPC_DETECT_MESSAGEHOOK:
	case AHNHS_ACTAPC_DETECT_MODULE_CHANGE:
		{
			TCHAR szMsg[255];
			wsprintf(szMsg, _T("«ÿ≈∑ πÊæÅE±‚¥…ø° ¿ÃªÛ¿Ã πﬂª˝«œø¥Ω¿¥œ¥Ÿ. (Code = %x)\n«¡∑Œ±◊∑•¿ª ¡æ∑·«’¥œ¥Ÿ."), lCode);
//			MessageBox(NULL, szMsg, szTitle, MB_OK);
			mlog(szMsg);
			PostThreadMessage(g_dwMainThreadID, WM_QUIT, 0, 0);
			break;
		}
	}

	return 1;
}
#endif

void drawDebugText(int x, int y, const char * str)
{
    MFontR2* pFont = (MFontR2*)MFontManager::Get(NULL);
    pFont->m_Font.BeginFont();
    pFont->m_Font.DrawText(x, y, str);
    pFont->m_Font.EndFont();  
}
