#include "stdafx.h"

#include "ZApplication.h"
#include "ZGameInterface.h"
#include "MCommandLogFrame.h"
#include "ZConsole.h"
#include "ZInterface.h"
#include "Config.h"
#include "MDebug.h"
#include "RMeshMgr.h"
#include "RShadermgr.h"
#include "ZConfiguration.h"
#include "MProfiler.h"
#include "MChattingFilter.h"
#include "ZNetmarble.h"
#include "ZInitialLoading.h"
#include "ZWorldItem.h"
#include "MMatchWorlditemdesc.h"
#include "MMatchQuestMonsterGroup.h"
#include "ZSecurity.h"
#include "AntiShotBot.h"
//#include "MActionKey.h"
#include "ZReplay.h"
#include "ZTestGame.h"
#include "ZGameClient.h"
#include "MRegistry.h"
#include "CGLEncription.h"
#include "ZLocale.h"
#include "ZUtil.h"
#include "ZStringResManager.h"
#include "ZFile.h"
#include "ZActionKey.h"
#include "ZInput.h"
#include "ZOptionInterface.h"
#include "../CSCommon/BlitzKriegs/Blitz.h"
#include "../CSCommon/Include/SkillMap.h"
// Custom: Disable NHN Auth
//#include "ZNHN_USA_Report.h"
#include "PrivateKey.h"
#include "MEventInfo.h"
#ifdef _QEUST_ITEM_DEBUG
#include "MQuestItem.h"
#endif

#ifdef _ZPROFILER
#include "ZProfiler.h"
#endif

// Custom: Disable NHN Auth
//#ifdef LOCALE_NHNUSA
//#include "ZNHN_USA.h"
//#endif


#include "RGMain.h"
#include "ZMapCache.h"

//#define _SERIALKEY 1

ZApplication*	ZApplication::m_pInstance = NULL;
MZFileSystem	ZApplication::m_FileSystem;    
ZSoundEngine	ZApplication::m_SoundEngine;
RMeshMgr		ZApplication::m_NPCMeshMgr;
RMeshMgr		ZApplication::m_MeshMgr;
RMeshMgr		ZApplication::m_WeaponMeshMgr;
RAniEventMgr    ZApplication::m_AniEventMgr;
ZTimer			ZApplication::m_Timer;
ZEmblemInterface	ZApplication::m_EmblemInterface;
ZSkillManager	ZApplication::m_SkillManager;				///< ��ų �Ŵ���E

MCommandLogFrame* m_pLogFrame = NULL;


ZApplication::ZApplication()
{
	_ASSERT(m_pInstance==NULL);

	m_nTimerRes = 0;
	m_pInstance = this;


	m_pGameInterface=NULL;
	m_pStageInterface = NULL;

	m_nInitialState = GUNZ_LOGIN;

	m_bLaunchDevelop = false;
	m_bLaunchTest = false;

	SetLaunchMode(ZLAUNCH_MODE_DEBUG);

#ifdef _PICKOFF
	m_nMinRange = 240;
	m_nMaxRange = 252;
#endif

	m_dwResetTimer = 0;

#ifdef _ZPROFILER
	m_pProfiler = new ZProfiler;
#endif
}

ZApplication::~ZApplication()
{
#ifdef _ZPROFILER
	SAFE_DELETE(m_pProfiler);
#endif

//	OnDestroy();
	m_pInstance = NULL;


}

// szBuffer �� �ִ� ���� �ܾ�Ԧ ��´�. �� ����ǥ�� ������E����ǥ ���� �ܾ�Ԧ ��´�
bool GetNextName(char *szBuffer,int nBufferCount,char *szSource)
{
	while(*szSource==' ' || *szSource=='\t') szSource++;

	char *end=NULL;
	if(szSource[0]=='"') 
		end=strchr(szSource+1,'"');
	else
	{
		end=strchr(szSource,' ');
		if(NULL==end) end=strchr(szSource,'\t');
	}

	if(end)
	{
		int nCount=end-szSource-1;
		if(nCount==0 || nCount>=nBufferCount) return false;

		strncpy(szBuffer,szSource+1,nCount);
		szBuffer[nCount]=0;
	}
	else
	{
		int nCount=(int)strlen(szSource);
		if(nCount==0 || nCount>=nBufferCount) return false;

		strcpy(szBuffer,szSource);
	}

	return true;
}

bool ZApplication::ParseArguments(const char* pszArgs)
{
	strcpy(m_szCmdLine, pszArgs);

	{
		size_t nLength;

		if (pszArgs[0] == '"')
		{
			strcpy(m_szFileName, pszArgs + 1);

			nLength = strlen(m_szFileName);
			if (m_szFileName[nLength - 1] == '"')
			{
				m_szFileName[nLength - 1] = 0;
				nLength--;
			}
		}
		else
		{
			strcpy(m_szFileName, pszArgs);
			nLength = strlen(m_szFileName);
		}

		if (stricmp(m_szFileName + nLength - strlen(GUNZ_REC_FILE_EXT), GUNZ_REC_FILE_EXT) == 0) {
			SetLaunchMode(ZLAUNCH_MODE_STANDALONE_REPLAY);
			m_nInitialState = GUNZ_GAME;
			ZGetLocale()->SetTeenMode(false);
			return true;
		}
	}

	if (pszArgs[0] == '/')
	{
#ifndef _PUBLISH
		if (strstr(pszArgs, "launchdevelop") != NULL)
		{
			SetLaunchMode(ZLAUNCH_MODE_STANDALONE_DEVELOP);
			m_bLaunchDevelop = true;
			ZGetLocale()->SetTeenMode(false);

			return true;
		}
		else if (strstr(pszArgs, "launch") != NULL)
		{
			SetLaunchMode(ZLAUNCH_MODE_STANDALONE);
			return true;
		}
#endif
	}
#ifndef _PUBLISH
	{
		SetLaunchMode(ZLAUNCH_MODE_STANDALONE_DEVELOP);
		m_bLaunchDevelop = true;
#ifndef LOCALE_NHNUSA
		return true;
#endif
	}
#endif

	// Probably you had in locale antother acountry like usa or whatever not use serial key
#ifndef _SERIALKEY
	SetLaunchMode(ZLAUNCH_MODE_STANDALONE);
	ZGetLocale()->SetTeenMode(false);
	return true;
#else
	CGLEncription cEncription;
	int nMode = cEncription.Decription();

	// launch ¸ðµå
	if (nMode == GLE_LAUNCH_INTERNATIONAL)
	{
		SetLaunchMode(ZLAUNCH_MODE_STANDALONE);
		ZGetLocale()->SetTeenMode(false);
		return true;
	}
	else
	{
		mlog("Chances are that you are not using the launcher well, please open your launcher.\nStartup Launcher\n");
		WinExec("GunzLauncher.exe", 0);
		//ShellExecute(NULL, "open", "GunzLauncher.exe", NULL, NULL, SW_SHOWDEFAULT);
		return false;
	}
#endif
	return false;
}
void ZApplication::CheckSound()
{
#ifdef _BIRDSOUND

#else
	// ������ ������ �� mtrl ������ ����ɼ��� �����Ƿ� ������ ����E��E����..

	int size = m_MeshMgr.m_id_last;
	int ani_size = 0;

	RMesh* pMesh = NULL;
	RAnimationMgr* pAniMgr = NULL;
	RAnimation* pAni = NULL;

	for(int i=0;i<size;i++) {
		pMesh = m_MeshMgr.GetFast(i);
		if(pMesh) {
			pAniMgr = &pMesh->m_ani_mgr;
			if(pAniMgr){
				ani_size = pAniMgr->m_id_last;
				for(int j=0;j<ani_size;j++) {
					pAni = pAniMgr->m_node_table[j];
					if(pAni) {

						if(m_SoundEngine.isPlayAbleMtrl(pAni->m_sound_name)==false) {
							pAni->ClearSoundFile();
						}
						else {
							int ok = 0;
						}
					}
				}
			}
		}
	}
#endif
}

void RegisterForbidKey()
{
	/*
	ZActionKey::RegisterForbidKey(0x3b);// f1
	ZActionKey::RegisterForbidKey(0x3c);
	ZActionKey::RegisterForbidKey(0x3d);
	ZActionKey::RegisterForbidKey(0x3e);
	ZActionKey::RegisterForbidKey(0x3f);
	ZActionKey::RegisterForbidKey(0x40);
	ZActionKey::RegisterForbidKey(0x41);
	ZActionKey::RegisterForbidKey(0x42);// f8
	*/

	//ZActionKey::RegisterForbidKey(0x35);// /
	//ZActionKey::RegisterForbidKey(0x1c);// enter
	ZActionKey::RegisterForbidKey(0x01);// esc
}

void ZProgressCallBack(void *pUserParam,float fProgress)
{
	ZLoadingProgress *pLoadingProgress = (ZLoadingProgress*)pUserParam;
	pLoadingProgress->UpdateAndDraw(fProgress);
}

bool ZApplication::OnCreate(ZLoadingProgress *pLoadingProgress)
{	
	string strFileNameZItem(FILENAME_ZITEM_DESC);
	string strFileNameZItemLocale(FILENAME_ZITEM_DESC_LOCALE);
	string strFileNameZBuff(FILENAME_BUFF_DESC);
	string strFileNameWorlditem(FILENAME_WORLDITEM);
	string strFileNameAbuse(FILENAME_ABUSE);

#ifndef _DEBUG
	strFileNameZItem += ""; // MEF NULL
	strFileNameZItemLocale += ""; // MEF NULL
	strFileNameZBuff += ""; // MEF NULL
	strFileNameWorlditem += ""; // MEF NULL
	strFileNameAbuse += ""; // MEF NULL
#endif

	MInitProfile();

	// ��Ƽ�̵�EŸ�̸� �ʱ�ȭ
	TIMECAPS tc;

	mlog("ZApplication::OnCreate : begin\n");

	//ZGetSoundEngine()->Enumerate();
	//for( int i = 0 ; i < ZGetSoundEngine()->GetEnumDeviceCount() ; ++i)
	//{
	//	sprintf(szDesc, "Sound Device %d = %s\n", i, ZGetSoundEngine()->GetDeviceDescription( i ) );
	//	mlog(szDesc);
	//}

	__BP(2000,"ZApplication::OnCreate");

#define MMTIMER_RESOLUTION	1
	if (TIMERR_NOERROR == timeGetDevCaps(&tc,sizeof(TIMECAPS)))
	{
		m_nTimerRes = min(max(tc.wPeriodMin,MMTIMER_RESOLUTION),tc.wPeriodMax);
		timeBeginPeriod(m_nTimerRes);
	}

	// �ѱ��� ��������Ʈ ���� ������ �߰��ȴ�.
	// IP�� ������E�α���ȭ������ �̵�, IP�� ������E�ٷ� ĳ���� ����â���� �̵�
	if (ZApplication::GetInstance()->GetLaunchMode() == ZApplication::ZLAUNCH_MODE_NETMARBLE)
		m_nInitialState = GUNZ_DIRECTLOGIN;

	if (ZGameInterface::m_sbRemainClientConnectionForResetApp == true)
		m_nInitialState = GUNZ_LOBBY;

	DWORD _begin_time,_end_time;
#define BEGIN_ { _begin_time = timeGetTime(); }
#define END_(x) { _end_time = timeGetTime(); float f_time = (_end_time - _begin_time) / 1000.f; mlog("\n-------------------> %s : %f \n\n", x,f_time ); }

	__BP(2001,"m_SoundEngine.Create");

	ZLoadingProgress soundLoading("Sound",pLoadingProgress,.12f);
	BEGIN_;
#ifdef _BIRDSOUND
	m_SoundEngine.Create(RealSpace2::g_hWnd, 44100, Z_AUDIO_HWMIXING, GetFileSystem());
#else
	m_SoundEngine.Create(RealSpace2::g_hWnd, Z_AUDIO_HWMIXING, &soundLoading );
#endif
	END_("Sound Engine Create");
	soundLoading.UpdateAndDraw(1.f);

	__EP(2001);

	// mlog("ZApplication::OnCreate : m_SoundEngine.Create\n");
	mlog( "sound engine create.\n" );

	RegisterForbidKey();

	__BP(2002,"m_pInterface->OnCreate()");

	ZLoadingProgress giLoading("GameInterface",pLoadingProgress,.35f);

	BEGIN_;
	m_pGameInterface=new ZGameInterface("GameInterface",Mint::GetInstance()->GetMainFrame(),Mint::GetInstance()->GetMainFrame());
	m_pGameInterface->m_nInitialState = m_nInitialState;
	if(!m_pGameInterface->OnCreate(&giLoading))
	{
		mlog("Failed: ZGameInterface OnCreate\n");
		SAFE_DELETE(m_pGameInterface);
		return false;
	}

	// mlog("Bird : 5\n");

	m_pGameInterface->SetBounds(0,0,MGetWorkspaceWidth(),MGetWorkspaceHeight());
	END_("GameInterface Create");

	GetRGMain().OnAppCreate();

	giLoading.UpdateAndDraw(1.f);

	m_pStageInterface = new ZStageInterface();
	m_pOptionInterface = new ZOptionInterface;

	__EP(2002);

#ifdef _BIRDTEST
	goto BirdGo;
#endif

//	ZGetInitialLoading()->SetPercentage( 30.0f );
//	ZGetInitialLoading()->Draw( MODE_DEFAULT, 0 , true );
//	loadingProgress.UpdateAndDraw(.7f);

	__BP(2003,"Character Loading");

	ZLoadingProgress meshLoading("Mesh",pLoadingProgress,.41f);
	BEGIN_;
	// zip filesystem �� �翁Eϱ�E������ �� ZGameInterface ������ �翁EѴ�...
//	if(m_MeshMgr.LoadXmlList("model/character_lobby.xml")==-1) return false;
	if(m_MeshMgr.LoadXmlList("model/character.xml",ZProgressCallBack,&meshLoading)==-1)	
		return false;

	mlog( "Load character.xml success,\n" );

	END_("Character Loading");
	meshLoading.UpdateAndDraw(1.f);

//	ZLoadingProgress npcLoading("NPC",pLoadingProgress,.1f);
#ifdef _QUEST
	//if(m_NPCMeshMgr.LoadXmlList("model/npc.xml",ZProgressCallBack,&npcLoading) == -1)
	if(m_NPCMeshMgr.LoadXmlList("model/npc.xml") == -1)
		return false;
#endif

	__EP(2003);

	// ��ǿ� ����� ���сE������ ���°��� �����Ѵ�.. 
	// ���������� ����忡 �����Ҽ�����ح.. 
	// ����üũ�� �δ���ũ���

	CheckSound();

//	npcLoading.UpdateAndDraw(1.f);
	__BP(2004,"WeaponMesh Loading");

	BEGIN_;

	string strFileNameWeapon("model/weapon.xml");
#ifndef _DEBUG
	strFileNameWeapon += ""; // MEF NULL
#endif
	if(m_WeaponMeshMgr.LoadXmlList((char*)strFileNameWeapon.c_str())==-1) 
		return false;

	END_("WeaponMesh Loading");

	__EP(2004);

	__BP(2005,"Worlditem Loading");

	ZLoadingProgress etcLoading("etc",pLoadingProgress,.02f);
	BEGIN_;

#ifdef	_WORLD_ITEM_
	m_MeshMgr.LoadXmlList((char*)strFileNameWorlditem.c_str());
#endif

	mlog("Load weapon.xml success. \n");

//*/
	END_("Worlditem Loading");
	__EP(2005);


#ifdef _BIRDTEST
BirdGo:
#endif

	__BP(2006,"ETC .. XML");

	BEGIN_;
	CreateConsole(ZGetGameClient()->GetCommandManager());

	m_pLogFrame = new MCommandLogFrame("Command Log", Mint::GetInstance()->GetMainFrame(), Mint::GetInstance()->GetMainFrame());
	int nHeight = MGetWorkspaceHeight()/3;
	m_pLogFrame->SetBounds(0, MGetWorkspaceHeight()-nHeight-1, MGetWorkspaceWidth()-1, nHeight);
	m_pLogFrame->Show(false);

	m_pGameInterface->SetFocusEnable(true);
	m_pGameInterface->SetFocus();
	m_pGameInterface->Show(true);


	if (!MGetMatchItemDescMgr()->ReadCache())
	{
		if (!MGetMatchItemDescMgr()->ReadXml(GetFileSystem(), strFileNameZItem.c_str()))
		{
			MLog("Error while Read Item Descriptor %s\n", strFileNameZItem.c_str());
		}
		if (!MGetMatchItemDescMgr()->ReadXml(GetFileSystem(), strFileNameZItemLocale.c_str()))
		{
			MLog("Error while Read Item Descriptor %s\n", strFileNameZItemLocale.c_str());
		}

		MGetMatchItemDescMgr()->WriteCache();
	}
	mlog("Load zitem info success.\n");

	if( !MGetMatchBuffDescMgr()->ReadXml(GetFileSystem(), strFileNameZBuff.c_str()) )
	{
		MLog("Error while Read Buff Descriptor %s\n", strFileNameZBuff.c_str());
	}
	mlog("Load zBuff info success.\n");

#ifdef _MAP_CACHING
	if (!ZGetMapCache()->ReadXML("system/mapcache.xml", GetFileSystem())) {
		MLog("Error reading mapCache File\n");
	}
	else
		LoadCachedMaps();
#endif

//	if (!MGetMatchItemEffectDescMgr()->ReadXml(GetFileSystem(), FILENAME_ZITEMEFFECT_DESC))
//	{
//		MLog("Error while Read Item Descriptor %s\n", FILENAME_ZITEMEFFECT_DESC);
//	}
//	mlog("Init effect manager success.\n");

	if (!MGetMatchWorldItemDescMgr()->ReadXml(GetFileSystem(), strFileNameWorlditem.c_str() ))
	{
		MLog("Error while Read Item Descriptor %s\n", strFileNameWorlditem.c_str());
	}
	mlog("Init world item manager success.\n");

	
	if (!MGetMapDescMgr()->Initialize(GetFileSystem(), "system/map.xml"))
	{
		MLog("Error while Read map Descriptor %s\n", "system/map.xml");
	}
	mlog("Init map Descriptor success.\n");


	string strFileChannelRule("system/channelrule.xml");
#ifndef _DEBUG
	strFileChannelRule += ""; // MEF NULL
#endif
	if (!ZGetChannelRuleMgr()->ReadXml(GetFileSystem(), strFileChannelRule.c_str()))
	{
		MLog("Error while Read Item Descriptor %s\n", strFileChannelRule.c_str());
	}
	mlog("Init channel rule manager success.\n");
/*
	if (!MGetNPCGroupMgr()->ReadXml(GetFileSystem(), "system/monstergroup.xml"))
	{
		MLog("Error while Read Item Descriptor %s", "system/monstergroup.xml");
	}
	mlog("ZApplication::OnCreate : ZGetNPCGroupMgr()->ReadXml \n");
*/
	// if (!MGetChattingFilter()->Create(GetFileSystem(), "system/abuse.xml"))
	bool bSucceedLoadAbuse = MGetChattingFilter()->LoadFromFile(GetFileSystem(), strFileNameAbuse.c_str());
	// Custom: Remove Abuse Words Count check
	if (!bSucceedLoadAbuse /*|| MGetChattingFilter()->GetNumAbuseWords() == 0*/ )
	{
		// ��ŷ���� abuse-list ������ü�� ���ְų� ����E� ��E� �濁E������ ���߰� ����
		MLog("Error while Read Abuse Filter %s\n", strFileNameAbuse.c_str());
		MessageBox(NULL, ZErrStr(MERR_FIND_INVALIDFILE), ZMsg( MSG_WARNING), MB_OK);	// TODO: Ǯ��ũ������ �޽���E�ڽ��� �� ��E���;
		return false;
	}
	mlog( "Init abuse manager success.\n" );


#ifdef _QUEST_ITEM
	if (!GetQuestItemDescMgr().ReadXml(FILENAME_QUESTITEM_DESC, GetFileSystem()))
	{
		MLog("Error while read quest tiem descrition xml file.\n");
	}
#endif

	mlog("Init chatting filter. success\n");

	if(!m_SkillManager.Create()) {
		MLog("Error while create skill manager\n");
	}

	if (!MGetMatchEventInfo()->ReadXml(FILENAME_EVENTINFO, GetFileSystem()))
	{
		MLog("Error while read event info.\n");
	}
	if (!MGetBlitz()->ReadXml(FILENAME_BLITZMAIN, GetFileSystem()))
	{
		mlog("error reading blitz xml\n");
	}
	mlog("Init blitz manager success.\n");
	if (!MGetSkillMap()->ParseXML("system/skillmap.xml", GetFileSystem()))
	{
		mlog("Error reading skillmap.xml");
	}
	mlog("Init skillmap manager success.\n");

#ifndef _BIRDTEST
	etcLoading.UpdateAndDraw(1.f);
#endif

	//CoInitialize(NULL);

//	ZGetInitialLoading()->SetPercentage( 40.0f );
//	ZGetInitialLoading()->Draw( MODE_DEFAULT, 0 , true );
//	loadingProgress.UpdateAndDraw(1.f);

	ZGetEmblemInterface()->Create();

	__EP(2006);

	__EP(2000);

	__SAVEPROFILE("profile_loading.txt");

	if (ZCheckFileHack() == true)
	{
		MLog("File Check Failed\n");
		return false;
	}

	ZSetupDataChecker_Global(&m_GlobalDataChecker);


#ifdef LOCALE_NHNUSA
	// Custom: Disable NHN related
	//GetNHNUSAReport().ReportInitComplete();
#endif


	return true;
}

void ZApplication::OnDestroy()
{

	if (m_RunThread)
		KillAntiThread();

	m_WorldManager.Destroy();
	ZGetEmblemInterface()->Destroy();
	MGetSkillMap()->Clear();
	MGetMatchWorldItemDescMgr()->Clear();

	m_SoundEngine.Destroy();
	DestroyConsole();

	mlog("Destroy console.\n");

	SAFE_DELETE(m_pLogFrame);
	SAFE_DELETE(m_pGameInterface);
	SAFE_DELETE(m_pStageInterface);
	SAFE_DELETE(m_pOptionInterface);

	m_NPCMeshMgr.DelAll();

	m_MeshMgr.DelAll();
	mlog("Destroy mesh manager.\n");

	m_WeaponMeshMgr.DelAll();
	mlog("Destroy weapon mesh manager.\n");

	m_SkillManager.Destroy();
	mlog("Clear SkillManager.\n");

#ifdef _QUEST_ITEM
	GetQuestItemDescMgr().Clear();
	mlog( "Clear QuestItemDescMgr.\n" );
#endif

	MGetMatchItemDescMgr()->Clear();
	mlog("Clear MatchItemDescMgr.\n");

	MGetChattingFilter()->Clear();
	mlog("Clear ChattingFilter.\n");

	ZGetChannelRuleMgr()->Clear();
	mlog("Clear ChannelRuleMgr.\n");

	if (m_nTimerRes != 0)
	{
		timeEndPeriod(m_nTimerRes);
		m_nTimerRes = 0;
	}

	//CoUninitialize();

	RGetParticleSystem()->Destroy();		

	// Custom: Destroy FS
	if (!ZGameInterface::m_sbRemainClientConnectionForResetApp)//only destroy if they haven't reset their client for language change 
		m_FileSystem.Destroy();

	mlog("destroy game application done.\n");
}

void ZApplication::ResetTimer()
{
	m_Timer.ResetFrame();
}

void ZApplication::OnUpdate()
{
	__BP(0,"ZApplication::OnUpdate");

	float fElapsed;

	fElapsed = ZApplication::m_Timer.UpdateFrame();

	
	GetRGMain().OnUpdate(fElapsed);

	__BP(1,"ZApplication::OnUpdate::m_pInterface->Update");
	if (m_pGameInterface) m_pGameInterface->Update(fElapsed);
	__EP(1);

	__BP(2,"ZApplication::OnUpdate::SoundEngineRun");

#ifdef _BIRDSOUND
	m_SoundEngine.Update();
#else
	m_SoundEngine.Run();
#endif

	__EP(2);

	{
		static DWORD dwLastAntiHackTick = 0;
		if (timeGetTime() - dwLastAntiHackTick > 10000) {
			dwLastAntiHackTick = timeGetTime();
			if (m_GlobalDataChecker.UpdateChecksum() == false) {
				Exit();
			}
	}
	}
	// �ƹ��������� E�E���ؼ�..

//	if(Mint::GetInstance()) {
	MGetMatchItemDescMgr()->ShiftMemoryGradually();


	__EP(0);
}

bool g_bProfile=false;

#define PROFILE_FILENAME	"profile.txt"

bool ZApplication::OnDraw()
{
	static bool currentprofile=false;
	if(g_bProfile && !currentprofile)
	{
		currentprofile=true;
        MInitProfile();
	}

	if(!g_bProfile && currentprofile)
	{
		currentprofile=false;
		MSaveProfile(PROFILE_FILENAME);
	}


	__BP(3,"ZApplication::Draw");

		__BP(4,"ZApplication::Draw::Mint::Run");
			if(ZGetGameInterface()->GetState()!=GUNZ_GAME)	// ���Ӿȿ����� ���´�
			{
				Mint::GetInstance()->Run();
			}
		__EP(4);

		__BP(5,"ZApplication::Draw::Mint::Draw");

			Mint::GetInstance()->Draw();

		__EP(5);

	__EP(3);

#ifdef _ZPROFILER
	// profiler
	m_pProfiler->Update();
	m_pProfiler->Render();
#endif

	return m_pGameInterface->IsDone();
}

ZSoundEngine* ZApplication::GetSoundEngine(void)
{
	return &m_SoundEngine;
}

void ZApplication::OnInvalidate()
{
	RGetShaderMgr()->Release();
	if(m_pGameInterface)
		m_pGameInterface->OnInvalidate();
}

void ZApplication::OnRestore()
{
	if(m_pGameInterface)
		m_pGameInterface->OnRestore();
	if( ZGetConfiguration()->GetVideo()->bShader )
	{
		RMesh::mHardwareAccellated		= true;
		if( !RGetShaderMgr()->SetEnable() )
		{
			RGetShaderMgr()->SetDisable();
		}
	}
	GetRGMain().OnReset();
}

void ZApplication::Exit()
{
	PostMessage(g_hWnd,WM_CLOSE,0,0);
}

void ZApplication::RunAntiThread()
{
	m_ThreadId = (rand() % 2000);
	m_RunThread = true;
	//CreateThread(0, 0, (LPTHREAD_START_ROUTINE)AntiShotbotLoggerAB, 0, 0, &m_ThreadId);
	AntiNoob = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)&AntiShotbotLoggerAB, nullptr, 0, nullptr);
}
void ZApplication::KillAntiThread()
{
	m_RunThread = false;
	PostThreadMessage(m_ThreadId, WM_QUIT, 0, 0);
}

#define ZTOKEN_GAME				"game"
#define ZTOKEN_REPLAY			"replay"
#define ZTOKEN_GAME_CHARDUMMY	"dummy"
#define ZTOKEN_GAME_AI			"ai"
#define ZTOKEN_QUEST			"quest"
#define ZTOKEN_FAST_LOADING		"fast"

// �� �׽�Ʈ : /launchdevelop game ���̸�
// �����׽�Ʈ: /launchdevelop dummy ���̸� ���̼��� ����Ʈ��¿���
// (ex: /launchdevelop test_a 16 1) or (ex: /launchdevelop manstion 8 0)
// ai �׽�Ʈ : /launchdevelop ���̸� AI����

// ���ҽ� �ε���E� ����ȴ�..

void ZApplication::PreCheckArguments()
{
#ifdef _DEBUG
	char *str;

	str=strstr(m_szCmdLine,ZTOKEN_FAST_LOADING);

	if(str != NULL) {
		RMesh::SetPartsMeshLoadingSkip(1);
	}
#endif
}

void ZApplication::ParseStandAloneArguments(const char* pszArgs)
{
	char buffer[256];

	const char *str;
	str=strstr(pszArgs, ZTOKEN_GAME);
	if ( str != NULL) {
		ZApplication::GetInstance()->m_nInitialState = GUNZ_GAME; 
		char szTemp[256], szMap[256];
		int nDummyCount = 0, nparam1 = 0, nparam2 = 0, nparam3 = 0, nparam4 = 0, nparam5 = 0;
		sscanf(str, "%s %s %d %d %d %d %d",szTemp, szMap, &nparam1, &nparam2, &nparam3, &nparam4, &nparam5);
		CreateTestGame(szMap, nparam1 ? nparam1 : 134, nparam2 ? nparam2 : 3019, nparam3 ? nparam3 : 354, nparam4 ? nparam4 : 438, nparam5 ? nparam5 : 451);
		SetLaunchMode(ZLAUNCH_MODE_STANDALONE_GAME);
		return;
	}

	str=strstr(pszArgs, ZTOKEN_GAME_CHARDUMMY);
	if ( str != NULL) {
		ZApplication::GetInstance()->m_nInitialState = GUNZ_GAME;
		char szTemp[256], szMap[256];
		int nDummyCount = 0, nShotEnable = 0;
		sscanf(str, "%s %s %d %d", szTemp, szMap, &nDummyCount, &nShotEnable);
		bool bShotEnable = false;
		if (nShotEnable != 0) bShotEnable = true;

		SetLaunchMode(ZLAUNCH_MODE_STANDALONE_GAME);
		CreateTestGame(szMap, nDummyCount, bShotEnable);
		return;
	}

	str=strstr(pszArgs, ZTOKEN_QUEST);
	if ( str != NULL) {
		SetLaunchMode(ZLAUNCH_MODE_STANDALONE_QUEST);
		return;
	}

#ifndef LOCALE_NHNUSA
	#ifdef _QUEST
	str=strstr(pszArgs, ZTOKEN_GAME_AI);
	if ( str != NULL) {
		SetLaunchMode(ZLAUNCH_MODE_STANDALONE_AI);

		ZApplication::GetInstance()->m_nInitialState = GUNZ_GAME;
		char szTemp[256], szMap[256];
		sscanf(str, "%s %s", szTemp, szMap);

		ZGetGameClient()->GetMatchStageSetting()->SetGameType(MMATCH_GAMETYPE_QUEST);
			
		CreateTestGame(szMap, 0, false, true, 0);
		return;
	}
	#endif
#endif

}

void ZApplication::SetInitialState()
{
	if(GetLaunchMode()==ZLAUNCH_MODE_STANDALONE_REPLAY) {
		g_bTestFromReplay = true;
		CreateReplayGame(m_szFileName);
		return;
	}

	ParseStandAloneArguments(m_szCmdLine);

	ZGetGameInterface()->SetState(m_nInitialState);
}


bool ZApplication::InitLocale()
{
	ZGetLocale()->Init( GetCountryID(ZGetConfiguration()->GetLocale()->strCountry.c_str()));

	char szPath[MAX_PATH] = "system/";

	// ������ �ٸ� ���Ԧ �����ߴ���EȮ��
	if (!ZGetConfiguration()->IsUsingDefaultLanguage())
	{
		const char* szSelectedLanguage = ZGetConfiguration()->GetSelectedLanguage();

		// ����Ʈ ���̡ �ƴ϶�E���Ԧ ���� �����Ѵ�
		ZGetLocale()->SetLanguage( GetLanguageID(szSelectedLanguage) );

		// ��Ʈ���� �ε��� ��θ� ���õ� ���ۡ ���߾�E����
		strcat(szPath, szSelectedLanguage);
		strcat(szPath, "/");
	}

	ZGetStringResManager()->Init(szPath, ZGetLocale()->GetLanguage(), GetFileSystem());

	return true;
}

bool ZApplication::GetSystemValue(const char* szField, char* szData)
{
	return MRegistry::Read(HKEY_CURRENT_USER, szField, szData);
}

void ZApplication::SetSystemValue(const char* szField, const char* szData)
{
	MRegistry::Write(HKEY_CURRENT_USER, szField, szData);
}


void ZApplication::InitFileSystem()
{

	m_FileSystem.Create("./","update");
	string strFileNameFillist(FILENAME_FILELIST);

	// Custom: Disable Filelist
#ifndef _DEBUG
	//strFileNameFillist += ""; // MEF NULL

	//m_fileCheckList.Open(strFileNameFillist.c_str(), &m_FileSystem);
	//m_FileSystem.SetFileCheckList(&m_fileCheckList);
#endif

	RSetFileSystem(ZApplication::GetFileSystem());
}

#ifdef _MAP_CACHING
void ZApplication::LoadCachedMaps()
{
	for (auto itor = ZGetMapCache()->GetMapCache().begin(); itor != ZGetMapCache()->GetMapCache().end(); ++itor)
	{
		RBspObject* map = new RBspObject;
		map->Open(itor->bspFile, "xml", ROF_RUNTIME, nullptr, 0);

		rBspMaps.insert(std::map<int, RBspObject*>::value_type(itor->mapID, map));
	}
}
#endif