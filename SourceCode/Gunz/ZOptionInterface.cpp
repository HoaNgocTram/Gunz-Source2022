#include "StdAfx.h"
#include "ZOptionInterface.h"
#include "MSlider.h"
#include "ZConfiguration.h"
#include "ZActionKey.h"
#include "FileInfo.h"
#include "ZCanvas.h"
#include "ZInput.h"
#include "ZRoomListBox.h"
#include "ZShopEquipListbox.h"
#include "ZShopEquipInterface.h"
#include "RFrameWork.h"

#include "RGMain.h"
#include "MDrawContext.h"


#include "ZCamera.h"


#define DEFAULT_SLIDER_MAX			10000

#define DEFAULT_GAMMA_SLIDER_MIN	50
#define DEFAULT_GAMMA_SLIDER_MAX	800

#define LISTBOX_CELL_ARRANGE(pList, feldsize, flsize)   { pList->GetField(1)->nTabSize += (int)( (float)feldsize * flsize) - (int)( (float)feldsize);  }  // ±◊∏≤ƒ≠ ≈©±‚¥¬ ∞˙›§, ±◊∫Œ∫–∏∏≈≠ 2π¯ø≠ø° ¥ı«‘

static	map< int, D3DDISPLAYMODE> gDisplayMode;
#define DEFAULT_REFRESHRATE			0

template< class F, class S>
class value_equals
{
private:
	S second;
public:
	value_equals(const S& s) : second(s) {}
	bool operator() (pair<const F, S> elem)
	{ return elem.second == second; }
};

bool operator == ( D3DDISPLAYMODE lhs, D3DDISPLAYMODE rhs )
{
	return( lhs.Width == rhs.Width && lhs.Height == rhs.Height && lhs.Format == rhs.Format );
}

// Custom: Support 16:9 aspect ratios
// Custom: Deprecate 640x480
static int widths[]={ 640,800,1024,1280,1280,1366,1600,1280,1440, 1600, 1650, 1920, 1920, 2560};
static int heights[]={ 480,600,768,720,960,768,1200,800,900, 900, 1050, 1080, 1200, 1600};

//static int widths[]={ 640,800,1024,1280,1600,1280,1440, 1650, 1920, 2560};
//static int heights[]={ 480,600,768,960,1200,800,900, 1050, 1200, 1600};


ZOptionInterface::ZOptionInterface(void)
{
	mbTimer = false;

//	mOldScreenWidth = 0;
//	mOldScreenHeight = 0;
	mnOldBpp = D3DFMT_A8R8G8B8;
	mTimerTime = 0;

	mOldScreenWidth = 800;
	mOldScreenHeight = 600;

	// Custom: Window mode widget
	//mbOldScreenWindowed = RIsFullScreen();
}

ZOptionInterface::~ZOptionInterface(void)
{
	gDisplayMode.clear();
}
static int i = 0;
void ZOptionInterface::InitInterfaceOption(void)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	mlog("start InitInterface option\n");
	/*
	// Mouse Sensitivity Min/Max (Z_MOUSE_SENSITIVITY_MIN ~ Z_MOUSE_SENSITIVITY_MAX)
	BEGIN_WIDGETLIST("MouseSensitivitySlider", pResource, MSlider*, pWidget);
	pWidget->SetMinMax(Z_MOUSE_SENSITIVITY_MIN, Z_MOUSE_SENSITIVITY_MAX);
	pWidget->SetValue(Z_MOUSE_SENSITIVITY);
	END_WIDGETLIST();
	*/

	MSlider* pWidget = (MSlider*)pResource->FindWidget("MouseSensitivitySlider");
	if (pWidget)
	{
		pWidget->SetMinMax(MOUSE_SENSITIVITY_MIN, MOUSE_SENSITIVITY_MAX);
		pWidget->SetValue(ZGetConfiguration()->GetMouseSensitivityInInt());
	}


	MComboBox* pComboBox = (MComboBox*)pResource->FindWidget("DashColor");
	if (pComboBox)
	{
		pComboBox->SetSelIndex(Z_ETC_DASH);
	}
	/*MSlider* pWidget = (MSlider*)pResource->FindWidget("MouseSensitivitySlider");
	if(pWidget)
	{
		pWidget->SetMinMax(MOUSE_SENSITIVITY_MIN, MOUSE_SENSITIVITY_MAX);
		pWidget->SetValue( ZGetConfiguration()->GetMouseSensitivityInInt());
	}*/
	pWidget = (MSlider*)pResource->FindWidget("JoystickSensitivitySlider");
	if(pWidget)
	{
		pWidget->SetMinMax(0, DEFAULT_SLIDER_MAX);
		pWidget->SetValue(ZGetConfiguration()->GetJoystick()->fSensitivity * DEFAULT_SLIDER_MAX);
	}

	pWidget = (MSlider*)pResource->FindWidget("BGMVolumeSlider");
	if(pWidget)
	{
		pWidget->SetMinMax(0, DEFAULT_SLIDER_MAX);
		pWidget->SetValue(Z_AUDIO_BGM_VOLUME*DEFAULT_SLIDER_MAX);
	}
	pWidget = (MSlider*)pResource->FindWidget("EffectVolumeSlider");
	if(pWidget)
	{
		pWidget->SetMinMax(0, DEFAULT_SLIDER_MAX);
		pWidget->SetValue(Z_AUDIO_EFFECT_VOLUME*DEFAULT_SLIDER_MAX);
	}
	pWidget = (MSlider*)pResource->FindWidget("VideoGamma");
	if(pWidget)
	{
		pWidget->SetMinMax(DEFAULT_GAMMA_SLIDER_MIN, DEFAULT_GAMMA_SLIDER_MAX);
		pWidget->SetValue(Z_VIDEO_GAMMA_VALUE*DEFAULT_GAMMA_SLIDER_MAX);
		pWidget->SetValue(Z_VIDEO_GAMMA_VALUE);
	}

	// Action Key
	for(int i=0; i<ZACTION_COUNT; i++){
		char szItemName[256];
		sprintf(szItemName, "%sActionKey", ZGetConfiguration()->GetKeyboard()->ActionKeys[i].szName);

		BEGIN_WIDGETLIST(szItemName, pResource, ZActionKey*, pWidget);
		pWidget->SetActionKey(ZGetConfiguration()->GetKeyboard()->ActionKeys[i].nVirtualKey);
		pWidget->SetActionKey(ZGetConfiguration()->GetKeyboard()->ActionKeys[i].nVirtualKeyAlt);
		END_WIDGETLIST();
	}

	//	ComboBox
	{
		MComboBox *pWidget = (MComboBox*)pResource->FindWidget("ScreenResolution");
		if(pWidget)
		{
			pWidget->RemoveAll();
			gDisplayMode.clear();

			int dmIndex = 0;
			char szBuf[256];

			D3DDISPLAYMODE ddm;

			D3DFORMAT format[2] = {
				D3DFMT_X8R8G8B8,
					D3DFMT_R5G6B5
			};

			for( int i=0;i<2;i++) 
			{
				int nDM = RGetAdapterModeCount( format[i] );

				mlog("Number of Display mode : %d\n", nDM );

				for( int idm = 0 ; idm < nDM; ++idm )
				{
					if( REnumAdapterMode( D3DADAPTER_DEFAULT,format[i], idm,  &ddm ))
					{
						// Custom: Deprecate 640x480
						if( ddm.Width < 640 || ddm.Height < 480 )
							continue;

						//if( ddm.Width < 640 || ddm.Height < 480 )
						//	continue;

						// Custom: Support 16:9 aspect ratios
						//if( ((float)ddm.Height / (float)ddm.Width  != 0.75f) &&  
						//	((float)ddm.Height / (float)ddm.Width  != 0.625f) &&
						//	((float)ddm.Height / (float)ddm.Width  != 0.5625f))
						//	continue;

						//if( ((float)ddm.Height / (float)ddm.Width  != 0.75f) &&  
						//	((float)ddm.Height / (float)ddm.Width  != 0.625f) )
						//	continue;

						ddm.RefreshRate = DEFAULT_REFRESHRATE;

						if( ddm.Format == D3DFMT_X8R8G8B8 || ddm.Format == D3DFMT_R5G6B5 )
						{
							map<int, D3DDISPLAYMODE>::iterator iter_ = find_if( gDisplayMode.begin(), gDisplayMode.end(), value_equals<int, D3DDISPLAYMODE>(ddm));
							if( iter_ == gDisplayMode.end() )
							{
								gDisplayMode.insert( map<int, D3DDISPLAYMODE>::value_type( dmIndex++, ddm ) );
								sprintf( szBuf, "%d x %d %dbpp", ddm.Width, ddm.Height, ddm.Format==D3DFMT_X8R8G8B8?32:16 );
								pWidget->Add(szBuf);
							}
						}
					}
				}
			}

			// ∏∏æÅEµ˚”œµ» «ÿªÛµµ∞° «œ≥™µµ æ¯¿ª∞ÊøÅE∞≠¡¶∑Œ µ˚”œ
			if( gDisplayMode.size() == 0 )
			{
				for( int i = 0 ; i < 10; ++i )
				{
					ddm.Width	= widths[i/2];
					ddm.Height	= heights[i/2];
					ddm.RefreshRate = DEFAULT_REFRESHRATE;
					ddm.Format	= ( ( i%2 == 1) ? D3DFMT_X8R8G8B8 : D3DFMT_R5G6B5 );

					int bpp = (i%2 == 1)? 32 : 16;
					gDisplayMode.insert( map<int, D3DDISPLAYMODE>::value_type(i,ddm) );
					sprintf( szBuf, "%dx%d  %d bpp", ddm.Width, ddm.Height, bpp );
					pWidget->Add( szBuf );
				}
			}
			ddm.Width = RGetScreenWidth();
			ddm.Height = RGetScreenHeight();
			ddm.RefreshRate = DEFAULT_REFRESHRATE;
			ddm.Format	= RGetPixelFormat();
			map< int, D3DDISPLAYMODE>::iterator iter = find_if( gDisplayMode.begin(), gDisplayMode.end(), value_equals<int, D3DDISPLAYMODE>(ddm));

			// Custom: Iterator crash fix, in case resolution isn't supported
			if( iter != gDisplayMode.end() )
				pWidget->SetSelIndex( iter->first );
			else
				pWidget->SetSelIndex( 0 );
		}

		pWidget = (MComboBox*)pResource->FindWidget("CharTexLevel");
		if(pWidget)	{
			pWidget->SetSelIndex(ZGetConfiguration()->GetVideo()->nCharTexLevel);
		}

		pWidget = (MComboBox*)pResource->FindWidget("MapTexLevel");
		if(pWidget)	{
			pWidget->SetSelIndex(ZGetConfiguration()->GetVideo()->nMapTexLevel);
		}

		pWidget = (MComboBox*)pResource->FindWidget("EffectLevel");
		if(pWidget)	{
			pWidget->SetSelIndex(ZGetConfiguration()->GetVideo()->nEffectLevel);
		}

		pWidget = (MComboBox*)pResource->FindWidget("TextureFormat");
		if(pWidget)	{
			pWidget->SetSelIndex(ZGetConfiguration()->GetVideo()->nTextureFormat);
		}

		// µøøµªÅEƒ∏√ƒ 20081017... by kam
		pWidget = (MComboBox*)pResource->FindWidget("MovingPictureResolution");
		if(pWidget)	{
			pWidget->SetSelIndex(ZGetConfiguration()->GetMovingPicture()->iResolution); // µøøµªÅEƒ∏√ƒ «ÿªÛµµ ºº∆√
		}
		// µøøµªÅEƒ∏√ƒ 20081028... by kam
		pWidget = (MComboBox*)pResource->FindWidget("MovingPictureFileSize");
		if(pWidget)	{
			pWidget->SetSelIndex(ZGetConfiguration()->GetMovingPicture()->iFileSize); // µøøµªÅEƒ∏√ƒ ∆ƒ¿œ≈©±ÅEøÅE?¶«?
		}

		// ææÅEº±≈√
		pWidget = (MComboBox*)pResource->FindWidget("LanguageSelectComboBox");
		if(pWidget)	{
			pWidget->RemoveAll();
			size_t size = ZGetConfiguration()->GetLocale()->vecSelectableLanguage.size();

			for (unsigned int i=0; i<size; ++i) {
				pWidget->Add( ZGetConfiguration()->GetLocale()->vecSelectableLanguage[i].strLanguageName.c_str());
			}

			pWidget->SetSelIndex( ZGetConfiguration()->GetSelectedLanguageIndex());

			GunzState state = ZApplication::GetGameInterface()->GetState();
			if (state == GUNZ_GAME || state == GUNZ_STAGE)
				pWidget->Enable(false);
			else
				pWidget->Enable(true);
		}
	}

	//	Button
	{
		MButton* pWidget = (MButton*)pResource->FindWidget("Reflection");
		if( pWidget )
		{
			pWidget->SetCheck(ZGetConfiguration()->GetVideo()->bReflection);
		}

		pWidget = (MButton*)pResource->FindWidget("LightMap");
		if( pWidget )
		{
			//if(ZGetConfiguration()->GetVideo()->bTerrible && !RIsHardwareTNL() )
			{
				pWidget->SetCheck(ZGetConfiguration()->GetVideo()->bLightMap);

				if(ZGetGame()) {
					ZGetGame()->GetWorld()->GetBsp()->LightMapOnOff(ZGetConfiguration()->GetVideo()->bLightMap);
				}
				else {
					RBspObject::SetDrawLightMap(ZGetConfiguration()->GetVideo()->bLightMap);
				}
			}
			//else
			//{
			//	pWidget->SetCheck( true );
			//	pWidget->Enable( false );
			//	MLabel* label = (MLabel*)pResource->FindWidget("Lightmap Label");
			//	if(label) label->SetTextColor( MCOLOR( 64, 64, 64 ));
			//}
		}

		pWidget = (MButton*)pResource->FindWidget("DynamicLight");
		if( pWidget )
		{
			pWidget->SetCheck(ZGetConfiguration()->GetVideo()->bDynamicLight);
		}

		pWidget = (MButton*)pResource->FindWidget("Shader");
		if( pWidget )
		{
			//if( !RShaderMgr::shader_enabled )
			if(!RIsSupportVS())
			{
				pWidget->SetCheck(false);
				pWidget->Enable(false);
			}
			else
			{
				pWidget->SetCheck(ZGetConfiguration()->GetVideo()->bShader);
			}
		}

		// Custom: Windowed mode widget
		MComboBox* pNewWidget = (MComboBox*)pResource->FindWidget("WindowedMode");
		if( pNewWidget )
		{
			pNewWidget->SetSelIndex(ZGetConfiguration()->GetVideo()->nScreenType );
		}

		pWidget = (MButton*)pResource->FindWidget("Archetype");
		if( pWidget )
		{
			pWidget->SetCheck( ZGetConfiguration()->GetVideo()->bArchetype );
		}

		pWidget = (MButton*)pResource->FindWidget("WhiteMap");
		if( pWidget )
		{
			pWidget->SetCheck( ZGetConfiguration()->GetVideo()->bWhitemap );
			SetWhiteMap(ZGetConfiguration()->GetVideo()->bWhitemap);
		}

		// Custom: 24bit depth buffer option
		pWidget = (MButton*)pResource->FindWidget("Stencil");
		if (pWidget)
		{
			pWidget->SetCheck(Z_VIDEO_STENCILBUFFER);
		}

		pWidget = (MButton*)pResource->FindWidget("ShotEffects");
		if (pWidget)
		{
			pWidget->SetCheck(ZGetConfiguration()->GetVideo()->bShotEffects);
		}

		pWidget = (MButton*)pResource->FindWidget("BulletMarks");
		if (pWidget)
		{
			pWidget->SetCheck(Z_VIDEO_BULLET_MARKS);
		}
		pWidget = (MButton*)pResource->FindWidget("SparkShot");
		if (pWidget)
		{
			pWidget->SetCheck(Z_ETC_SPARK);
		}
		pWidget = (MButton*)pResource->FindWidget("HitMaker");
		if (pWidget)
		{
			pWidget->SetCheck(Z_ECT_HITMAKER);
		}
		pWidget = (MButton*)pResource->FindWidget("KillCam");
		if (pWidget)
		{
			pWidget->SetCheck(Z_ECT_KILLCAM);
		}
		pWidget = (MButton*)pResource->FindWidget("Blood");
		if (pWidget)
		{
			pWidget->SetCheck(Z_ECT_BLOOD);
		}
#ifdef _BULLETBAR
		pWidget = (MButton*)pResource->FindWidget("BulletBar");
		if (pWidget)
		{
			pWidget->SetCheck(Z_ECT_BULLETBAR);
		}
#endif
#ifdef _NPCBAR
		pWidget = (MButton*)pResource->FindWidget("NPCBar");
		if (pWidget)
		{
			pWidget->SetCheck(Z_ECT_NPCBAR);
		}
#endif

		pWidget = (MButton*)pResource->FindWidget("NewRenderer");
		if (pWidget)
		{
			pWidget->SetCheck(ZGetConfiguration()->GetEtc()->bNewRenderer);
		}

		pWidget = (MButton*)pResource->FindWidget("D3D9EX");
		if (pWidget) {
			bool isEnabledD3D9Ex = pWidget->GetCheck();
			if (Z_ETC_D3D9E != isEnabledD3D9Ex) {
				ZApplication::GetGameInterface()->ShowMessage("Please restart the client for the changes to take effect.");
				Z_ETC_D3D9E = pWidget->GetCheck();
			}
		}

		pWidget = (MButton*)pResource->FindWidget("NoFog");
		if (pWidget)
		{
			pWidget->SetCheck(Z_ECT_NOFOG);
		}
		pWidget = (MButton*)pResource->FindWidget("ScoreNew");
		if (pWidget)
		{
			pWidget->SetCheck(Z_ECT_SCORE);
		}
		pWidget = (MButton*)pResource->FindWidget("ExtraInfo");
		if (pWidget)
		{
			pWidget->SetCheck(Z_ECT_EXTRA_SHOW);
		}
		pWidget = (MButton*)pResource->FindWidget("NewBar");
		if (pWidget)
		{
			pWidget->SetCheck(Z_ETC_NEWBAR);
		}
		pWidget = (MButton*)pResource->FindWidget("Emoticon");
		if (pWidget)
		{
			pWidget->SetCheck(Z_ETC_EMOTE);
		}
		pWidget = (MButton*)pResource->FindWidget("RespawnEffect");
		if (pWidget)
		{
			pWidget->SetCheck(Z_ECT_RT);
		}

		pWidget = (MButton*)pResource->FindWidget("NoSmoke");
		if (pWidget)
		{
			pWidget->SetCheck(Z_ECT_NOSMOKE);
		}
		pWidget = (MButton*)pResource->FindWidget("3DShotGun");
		if (pWidget)
		{
			pWidget->SetCheck(Z_ECT_3DSHOTGUN);
		}


		pWidget = (MButton*)pResource->FindWidget("KillStreak");
		if (pWidget)
		{
			pWidget->SetCheck(Z_ECT_KILL);
		}

		pWidget = (MButton*)pResource->FindWidget("LowPoly");
		if (pWidget)
		{
			pWidget->SetCheck(Z_ECT_LOWPOLY);
		}

		pWidget = (MButton*)pResource->FindWidget("NewChatEnable");
		if (pWidget)
		{
			pWidget->SetCheck(Z_ETC_CHATNEW);
		}

		pWidget = (MButton*)pResource->FindWidget("FirstPerson");
		if (pWidget)
		{
			pWidget->SetCheck(Z_ETC_PERSON);
		}
		pWidget = (MButton*)pResource->FindWidget("PingColors");
		if (pWidget)
		{
			pWidget->SetCheck(Z_ETC_COLORS);
		}
		pWidget = (MButton*)pResource->FindWidget("CountryFlags");
		if (pWidget)
		{
			pWidget->SetCheck(Z_ETC_FLAG);
		}

		pWidget = (MButton*)pResource->FindWidget("PostureChar");
		if (pWidget)
		{
			pWidget->SetCheck(Z_ETC_POSTURECHAR);
		}

		pWidget = (MButton*)pResource->FindWidget("LatencyGraphic");
		if (pWidget)
		{
			pWidget->SetCheck(Z_ETC_LATENCY);
		}

#ifdef _CHATBACKGROUND
		pWidget = (MButton*)pResource->FindWidget("ChatBackGroundNormal");
		if (pWidget)
		{
			pWidget->SetCheck(Z_ETC_CHATBACKGROUND);
		}
#endif
#ifdef _VOICE_CHAT
		pWidget = (MButton*)pResource->FindWidget("VoiceChatToggle");
		if (pWidget)
		{
			pWidget->SetCheck(Z_ECT_VOICE);
		}
#endif
		// Custom: Rewrote SwordTrail ComboBox
		MComboBox* pComboBox = (MComboBox*)pResource->FindWidget("SwordTrail");
		if( pComboBox )
		{
			pComboBox->SetSelIndex( ZGetConfiguration()->GetEtc()->nSwordTrail );
			RSetTrackSize(ZGetConfiguration()->GetEtc()->nSwordTrail);
		}
		pComboBox = (MComboBox*)pResource->FindWidget("MultiSampling");
		if (pComboBox)
		{
			pComboBox->SetSelIndex(Z_VIDEO_MULTISAMPLING);
		}
		pWidget = (MButton*)pResource->FindWidget("WeaponRotation");
		if( pWidget )
		{
			pWidget->SetCheck( ZGetConfiguration()->GetEtc()->bWeaponRotation );
		}

		pWidget = (MButton*)pResource->FindWidget("FrameLimit_Display");
		if( pWidget )
		{
			pWidget->SetCheck( ZGetConfiguration()->GetEtc()->bShowFPS );
		}

		pWidget = (MButton*)pResource->FindWidget("HPAPScoreboard");
		if (pWidget)
		{
			pWidget->SetCheck(ZGetConfiguration()->GetEtc()->bHPAPScoreboard);
		}
		pWidget = (MButton*)pResource->FindWidget("ChatFilter");
		if (pWidget)
		{
			pWidget->SetCheck(ZGetConfiguration()->GetEtc()->bChatFilter);
		}

		pWidget = (MButton*)pResource->FindWidget("BGMMute");
		if(pWidget)
		{
			pWidget->SetCheck( !ZGetConfiguration()->GetAudio()->bBGMMute );
		}

		pWidget = (MButton*)pResource->FindWidget("EffectMute");
		if(pWidget)
		{
			pWidget->SetCheck( !ZGetConfiguration()->GetAudio()->bEffectMute );
		}

		pWidget	= (MButton*)pResource->FindWidget("Effect3D");
		if(pWidget)
		{
			pWidget->SetCheck(ZGetConfiguration()->GetAudio()->b3DSound);
		}

		pWidget = (MButton*)pResource->FindWidget("8BitSound");
		if(pWidget)
		{
			pWidget->SetCheck(Z_AUDIO_8BITSOUND);
			pWidget = (MButton*)pResource->FindWidget("16BitSound");
			if(pWidget) pWidget->SetCheck(!Z_AUDIO_8BITSOUND);
		}
		pWidget = (MButton*)pResource->FindWidget("InverseSound");
		if(pWidget)
		{
			pWidget->SetCheck(Z_AUDIO_INVERSE);
		}
		pWidget = (MButton*)pResource->FindWidget("HWMixing");
		if(pWidget)
		{
			pWidget->SetCheck(Z_AUDIO_HWMIXING);
		}
		pWidget = (MButton*)pResource->FindWidget("HitSound");
		if(pWidget)
		{
			pWidget->SetCheck(Z_AUDIO_HITSOUND);
		}
		pWidget = (MButton*)pResource->FindWidget("NarrationSound");
		if(pWidget)
		{
			pWidget->SetCheck(Z_AUDIO_NARRATIONSOUND);
		}
		pWidget = (MButton*)pResource->FindWidget("InvertMouse");
		if(pWidget)
		{
			pWidget->SetCheck(Z_MOUSE_INVERT);
		}
		pWidget = (MButton*)pResource->FindWidget("InvertJoystick");
		if(pWidget)
		{
			pWidget->SetCheck(Z_JOYSTICK_INVERT);
		}
		pWidget = (MButton*)pResource->FindWidget("HPAPScoreboard");
		/*if (pWidget)
		{
			pWidget->SetCheck(Z_ETC_HPAP_SCOREBOARD);
		}*/
		{
			//MComboBox* pWidget = (MComboBox*)pResource->FindWidget("ChatBackgroundColorOption");
			//if (pWidget)
			//{
			//	pWidget->SetSelIndex(Z_CHAT_BACKGROUND_COLOR - 1);
			//}
		}
	}

	// Etc
	{	
		MEdit* pEdit = (MEdit*)pResource->FindWidget("NetworkPort1");
		if (pEdit)
		{
			char szBuf[64];
			sprintf(szBuf, "%d", Z_ETC_NETWORKPORT1);
			pEdit->SetText(szBuf);
		}

		pEdit = (MEdit*)pResource->FindWidget("NetworkPort2");
		if (pEdit)
		{
			char szBuf[64];
			sprintf(szBuf, "%d", Z_ETC_NETWORKPORT2);
			pEdit->SetText(szBuf);
		}

		pEdit = (MEdit*)pResource->FindWidget("MouseSensitivityEdit");
		if (pEdit)
		{
			char szBuf[1024];
			sprintf(szBuf, "%d", ZGetConfiguration()->GetMouseSensitivityInInt());
			pEdit->SetText(szBuf);
			pEdit->SetMaxLength(16);
		}

		MButton* pBtnBoost = (MButton*)pResource->FindWidget("BoostOption");
		if (pBtnBoost)
		{
			pBtnBoost->SetCheck(Z_ETC_BOOST);
		}

		MButton* pBtnNormalChat = (MButton*)pResource->FindWidget("NormalChatOption");
		if (pBtnNormalChat)
		{
			pBtnNormalChat->SetCheck( Z_ETC_REJECT_NORMALCHAT);
		}

		MButton* pBtnTeamChat = (MButton*)pResource->FindWidget("TeamChatOption");
		if (pBtnTeamChat)
		{
			pBtnTeamChat->SetCheck(Z_ETC_REJECT_TEAMCHAT);
		}

		MButton* pBtnClanChat = (MButton*)pResource->FindWidget("ClanChatOption");
		if (pBtnClanChat)
		{
			pBtnClanChat->SetCheck(Z_ETC_REJECT_CLANCHAT);
		}

		MButton* pBtnWhisper = (MButton*)pResource->FindWidget("WhisperOption");
		if (pBtnWhisper)
		{
			pBtnWhisper->SetCheck(Z_ETC_REJECT_WHISPER);
		}

		MButton* pBtnInvite = (MButton*)pResource->FindWidget("InviteOption");
		if (pBtnInvite)
		{
			pBtnInvite->SetCheck(Z_ETC_REJECT_INVITE);
		}

		MComboBox *pComboBox = (MComboBox*)pResource->FindWidget("CrossHairComboBox");
		if(pComboBox)
		{
			pComboBox->RemoveAll();

			// ±‚∫ª ≈©∑ŒΩ∫ ?EÅE«œµÂƒ⁄µ˘¿∏∑Œ ¿‘∑¬

			for (int i = 0; i < ZCSP_CUSTOM; i++)
			{
				char szText[256];
				sprintf(szText, "%s %d", ZMsg(MSG_WORD_TYPE), i+1);
				pComboBox->Add(szText);
			}
			char szCustomFile[256];
			sprintf(szCustomFile, "%s%s%s", PATH_CUSTOM_CROSSHAIR, FN_CROSSHAIR_HEADER, FN_CROSSHAIR_TAILER);
			if (IsExist(szCustomFile)) pComboBox->Add("Custom");

			if (Z_ETC_CROSSHAIR >= pComboBox->GetCount())	// ªÁøÅE⁄¡ˆ¡§¿Ãø¥¥¬µ?ªÁøÅE⁄¡ˆ¡§¿?æ¯æ˚›≥¿ª ∞ÊøÅE
			{
				Z_ETC_CROSSHAIR = 0;
			}
			pComboBox->SetSelIndex(Z_ETC_CROSSHAIR);			
		}

		ZCanvas* pCrossHairPreview = (ZCanvas*)pResource->FindWidget("CrossHairPreviewCanvas");
		if (pCrossHairPreview)
		{
			pCrossHairPreview->SetOnDrawCallback(ZCrossHair::OnDrawOptionCrossHairPreview);
		}
#ifdef _CAMERADISTANCE
		pComboBox = (MComboBox*)pResource->FindWidget("CameraDistance");
		if (pComboBox)
		{
			pComboBox->SetSelIndex(Z_ETC_CAMERA);
			ZGetCamera()->RSetCameraDistance(Z_ETC_CAMERA);
		}
#endif
#ifdef _CHATBACKGROUND	
		pEdit = (MEdit*)pResource->FindWidget("ChatBackGroundOpacity");
		if (pEdit)
		{
			if (Z_ETC_CHATOPACITY < 1 || Z_ETC_CHATOPACITY > 100)
				Z_ETC_CHATOPACITY = 0;

			char szBuf[64];
			sprintf(szBuf, "%d", Z_ETC_CHATOPACITY);
			pEdit->SetText(szBuf);
		}
#endif

#ifdef _CROSS_SIZE_BOX
		pEdit = (MEdit*)pResource->FindWidget("CrossSize");
		if (pEdit)
		{
			if (Z_ETC_CROSS_SIZE < 50.0f || Z_ETC_CROSS_SIZE > 250.0f)
				Z_ETC_CROSS_SIZE = 50.0f;

			char szBuf[64];
			sprintf(szBuf, "%d", Z_ETC_CROSS_SIZE);
			pEdit->SetText(szBuf);
		}
#endif

		// Custom: FrameLimit_PerSecond is now an editable box
		pEdit = (MEdit*)pResource->FindWidget("FrameLimit_PerSecond");
		if (pEdit)
		{
			if (Z_ETC_FRAMELIMIT_PERSECOND < 60 || Z_ETC_FRAMELIMIT_PERSECOND > 1000)
				Z_ETC_FRAMELIMIT_PERSECOND = 0;

			char szBuf[64];
			sprintf(szBuf, "%d", Z_ETC_FRAMELIMIT_PERSECOND);
			pEdit->SetText(szBuf);
			RSetFrameLimitPerSeceond(Z_ETC_FRAMELIMIT_PERSECOND);

			//SetWidgetToolTipText("FrameLimit_PerSecond", "Acceptable values: 0 or 60~1000. Recommended: 250", MAM_RIGHT | MAM_TOP);
		}
	}

	//Macro
	{
		static char stemp_str[ZCONFIG_MACRO_MAX][80] = {
			"MacroF1",
			"MacroF2",
			"MacroF3",
			"MacroF4",
			"MacroF5",
			"MacroF6",
			"MacroF7",
			"MacroF8",
		};

		ZCONFIG_MACRO* pMacro = ZGetConfiguration()->GetMacro();

		if(pMacro) {
			MEdit* pEdit = NULL;
			for(int i=0;i<ZCONFIG_MACRO_MAX;i++) {

				pEdit = (MEdit*) pResource->FindWidget(stemp_str[i]);

				if (pEdit) {
					pEdit->SetText( pMacro->GetString(i) );
				}
			}
		}
	}

	{
		auto&& Cfg = *ZGetConfiguration();
		std::pair<const char*, bool> Buttons[] = {

			{ "ChatFontBoldOption",       Cfg.GetChat()->BoldFont },
			{ "ChatTimeStampOption",      Cfg.GetChat()->TimeStamp },
			{ "ChatEnableCursorOption",   Cfg.GetChat()->EnableCursor },
			{ "", false },
		};

		for (auto&& ButtonInfo : Buttons)
		{
			auto ButtonWidget = ZFindWidgetAs<MButton>(ButtonInfo.first);
			if (ButtonWidget)
				ButtonWidget->SetCheck(ButtonInfo.second);
		}


		char StringFontSize[64];

		if(Cfg.GetChat()->FontSize >= 20)
			sprintf_safe(StringFontSize, "%d", 20);
		else if(Cfg.GetChat()->FontSize <= 10)
			sprintf_safe(StringFontSize, "%d", 10);
		else
			sprintf_safe(StringFontSize, "%d", Cfg.GetChat()->FontSize);

		char StringBackgroundColor[64];
		sprintf_safe(StringBackgroundColor, "%08X", Cfg.GetChat()->BackgroundColor);

#ifdef _TRAILCOLOR	                                  
		char StringTrailColor[64];
		sprintf_safe(StringTrailColor, "%08X", Cfg.GetEtc()->bTrailColors);
#endif

		std::pair<const char*, const char*> Edits[] = {

			{ "ChatFontOption",            Cfg.GetChat()->Font.c_str() },
			{ "ChatFontSizeOption",         StringFontSize },
			{ "ChatBackgroundColorOption",	StringBackgroundColor},
#ifdef _TRAILCOLOR
			{ "TrailColor",	                StringTrailColor},
#endif
			{ "", false },
		};

		for (auto&& EditInfo : Edits)
		{
			auto EditWidget = ZFindWidgetAs<MEdit>(EditInfo.first);
			if (EditWidget)
				EditWidget->SetText(EditInfo.second);
		}
	}
	mlog("end of InitInterface option ok\n");
}

bool ZOptionInterface::SaveInterfaceOption(void)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MDrawContext* pDC = Mint::GetInstance()->GetDrawContext();

	int nDashColor = 0;
	MComboBox* pComboBox = (MComboBox*)pResource->FindWidget("DashColor");
	if (pComboBox)
	{
		nDashColor = pComboBox->GetSelIndex();
		if (Z_ETC_DASH != nDashColor)
		{
			Z_ETC_DASH = nDashColor;
		}
	}
	{ // ΩΩ∂Û¿Ã¥ÅE
		MSlider* pWidget = (MSlider*)pResource->FindWidget("MouseSensitivitySlider");
		Z_MOUSE_SENSITIVITY = (float) ((MSlider*)pWidget)->GetValue() / (float)MOUSE_SENSITIVITY_MAX;

		pWidget = (MSlider*)pResource->FindWidget("JoystickSensitivitySlider");
		Z_JOYSTICK_SENSITIVITY = (float) ((MSlider*)pWidget)->GetValue() / (float)DEFAULT_SLIDER_MAX;

		pWidget = (MSlider*)pResource->FindWidget("BGMVolumeSlider");
		if(pWidget)
		{
			Z_AUDIO_BGM_VOLUME = (float) ((MSlider*)pWidget)->GetValue() / (float)DEFAULT_SLIDER_MAX;
			ZGetSoundEngine()->SetMusicVolume(Z_AUDIO_BGM_VOLUME) ;
		}
		pWidget = (MSlider*)pResource->FindWidget("EffectVolumeSlider");
		if(pWidget)
		{
			Z_AUDIO_EFFECT_VOLUME = (float) ((MSlider*)pWidget)->GetValue() / (float)DEFAULT_SLIDER_MAX;

			ZGetSoundEngine()->SetEffectVolume(Z_AUDIO_EFFECT_VOLUME);
		}

	}

	int i=0;

	//for(i=0; i<ZACTION_COUNT; i++) {
	//	ZGetInput()->UnregisterActionKey(i);
	//}

	// ∏µŒ ≈¨∏Ææ˚‰ƒ ¿Áµ˚”œ
	ZGetInput()->ClearActionKey();

	for(i=0; i<ZACTION_COUNT; i++){
		char szItemName[256];
		sprintf(szItemName, "%sActionKey", ZGetConfiguration()->GetKeyboard()->ActionKeys[i].szName);
		ZActionKey* pWidget = (ZActionKey*)pResource->FindWidget(szItemName);
		if(pWidget==NULL) continue;
		int nKey = 0;
		pWidget->GetActionKey(&nKey);
		//		Mint::GetInstance()->UnregisterActionKey(i);
		//		Mint::GetInstance()->RegisterActionKey(i, nKey);	// ≈∞ µ˚”œ

//		ZGetInput()->UnregisterActionKey(i);
		ZGetInput()->RegisterActionKey(i,nKey);

		ZVIRTUALKEY altKey;
		pWidget->GetActionAltKey(&altKey);
		if(altKey!=-1)
			ZGetInput()->RegisterActionKey(i,altKey);

		// ZConfiguration¿∏∑Œ ø…º« ¿˙¿ÅE
		ZGetConfiguration()->GetKeyboard()->ActionKeys[i].nVirtualKey = nKey;
		ZGetConfiguration()->GetKeyboard()->ActionKeys[i].nVirtualKeyAlt = altKey;
	}

	// ªÅE≈∞∏ »ƒ ∏µÅE≈∞ªÛ≈¬∏¶ release∑Œ º¬∆√
	ZGetInput()->OffActionKeys();

	/*
	int nCnt[ZACTION_COUNT];

	for(i=0; i<ZACTION_COUNT; i++)
	{
		nCnt[i] = ZGetConfiguration()->GetKeyboard()->ActionKeys[i].nVirtualKey;
	}
	*/

	{
		Z_VIDEO_WIDTH = RGetScreenWidth();
		Z_VIDEO_HEIGHT	= RGetScreenHeight();
		Z_VIDEO_FULLSCREEN	= RGetScreenType();
		Z_VIDEO_BPP	= RGetPixelFormat()==D3DFMT_X8R8G8B8 ? 32:16 ;

		MComboBox*	pWidget = (MComboBox*)pResource->FindWidget("CharTexLevel");

		int TexLevel = 0;
		DWORD flag = 0;
		int EffectLevel = 0;
		int nTextureFormat = 0;

		if(pWidget)	{

			TexLevel = pWidget->GetSelIndex();

			if( ZGetConfiguration()->GetVideo()->bTerrible ){
				ZGetConfiguration()->GetVideo()->nCharTexLevel = TexLevel;
				if( TexLevel == 2 )
					SetObjectTextureLevel(TexLevel+2);
				else
					SetObjectTextureLevel(TexLevel);

				flag |= RTextureType_Object;
			}
			else if( ZGetConfiguration()->GetVideo()->nCharTexLevel != TexLevel ) {
				ZGetConfiguration()->GetVideo()->nCharTexLevel = TexLevel;
				SetObjectTextureLevel(TexLevel);
				flag |= RTextureType_Object;
			}
		}

		pWidget = (MComboBox*)pResource->FindWidget("MapTexLevel");

		if(pWidget)	{

			TexLevel = pWidget->GetSelIndex();

			if( ZGetConfiguration()->GetVideo()->bTerrible ){
				ZGetConfiguration()->GetVideo()->nCharTexLevel = TexLevel;
				if( TexLevel == 2 )
					SetObjectTextureLevel(TexLevel+2);
				else
					SetObjectTextureLevel(TexLevel);

				flag |= RTextureType_Object;
			}
			if( ZGetConfiguration()->GetVideo()->nMapTexLevel != TexLevel ) {
				ZGetConfiguration()->GetVideo()->nMapTexLevel = TexLevel;

				SetMapTextureLevel(TexLevel);
				flag |= RTextureType_Map;
			}
		}
		// Custom: Archetype brought to scope
		{
			MButton* pWidget2 = (MButton*)pResource->FindWidget("Archetype");
			if( pWidget2 )
			{
				bool bTickedBefore = Z_VIDEO_ARCHETYPE;
				Z_VIDEO_ARCHETYPE = pWidget2->GetCheck();

				if (/*!bTickedBefore &&*/ Z_VIDEO_ARCHETYPE)
					SetMapTextureLevel(8);
				else
					SetMapTextureLevel(ZGetConfiguration()->GetVideo()->nMapTexLevel);

				if (bTickedBefore != Z_VIDEO_ARCHETYPE)
				{
					if (!(flag & RTextureType_Map))
						flag |= RTextureType_Map;
				}
			}
		}

		// Custom: Whitemaps
		{
			MButton* pWidget2 = (MButton*)pResource->FindWidget("WhiteMap");
			if( pWidget2 )
			{
				Z_VIDEO_WHITEMAP = pWidget2->GetCheck();
				if (Z_VIDEO_WHITEMAP)
					SetWhiteMap(true);
				else
					SetWhiteMap(false);
			}
		}

		pWidget = (MComboBox*)pResource->FindWidget("EffectLevel");

		if(pWidget)	{

			EffectLevel = pWidget->GetSelIndex();

			if( ZGetConfiguration()->GetVideo()->nEffectLevel != EffectLevel ) {
				ZGetConfiguration()->GetVideo()->nEffectLevel = EffectLevel;
				SetEffectLevel(EffectLevel);
				//				flag |= RTextureType_Map;
			}
		}

		pWidget = (MComboBox*)pResource->FindWidget("TextureFormat");

		if(pWidget)	{

			nTextureFormat = pWidget->GetSelIndex();

			if( ZGetConfiguration()->GetVideo()->nTextureFormat != nTextureFormat ) {
				ZGetConfiguration()->GetVideo()->nTextureFormat = nTextureFormat;
				SetTextureFormat(nTextureFormat);
				flag = RTextureType_All;
			}
		}
		if(flag) 
		{
			RChangeBaseTextureLevel(flag);
		}
	}
	{ // µøøµªÅEƒ∏√ƒ 20081017... by kam
		MComboBox*	pWidget = (MComboBox*)pResource->FindWidget("MovingPictureResolution");
		int iResolution = 0;
		if(pWidget)	{
			iResolution = pWidget->GetSelIndex();
			ZGetConfiguration()->GetMovingPicture()->iResolution = iResolution;	// configurationø° ºº∆√«—¥Ÿ.
			SetBandiCaptureConfig(iResolution);									// π›µƒ∏√≥ µøøµªÅE«ÿªÛµµø° ¿˚øÅE—¥Ÿ
		}
		pWidget = (MComboBox*)pResource->FindWidget("MovingPictureFileSize");
		int iFileSize = 0;
		if(pWidget)	{
			iFileSize = pWidget->GetSelIndex();
			ZGetConfiguration()->GetMovingPicture()->iFileSize = iFileSize;	// configurationø° ºº∆√«—¥Ÿ.
			SetBandiCaptureFileSize(iFileSize);								// π›µƒ∏√≥ µøøµªÅE∆ƒ¿œ≈©±ÅEºº∆√ø° ¿˚øÅE—¥Ÿ
		}
	}
	{	// ææ˚ÿ±≈√
#ifdef _MULTILANGUAGE
		MComboBox*	pWidget = (MComboBox*)pResource->FindWidget("LanguageSelectComboBox");
		if(pWidget)	{
			ZGetConfiguration()->SetSelectedLanguageIndex( pWidget->GetSelIndex());
		}
#endif //_MULTILANGUAGE
	}
	{
		MButton* pWidget = (MButton*)pResource->FindWidget("Reflection");
		if(pWidget)
		{
			Z_VIDEO_REFLECTION = pWidget->GetCheck();
		}

		pWidget = (MButton*)pResource->FindWidget("LightMap");
		if(pWidget)
		{
			Z_VIDEO_LIGHTMAP = pWidget->GetCheck();

			if(ZGetGame()) {
				ZGetGame()->GetWorld()->GetBsp()->LightMapOnOff(Z_VIDEO_LIGHTMAP);
			}
			else {
				RBspObject::SetDrawLightMap(Z_VIDEO_LIGHTMAP);
			}
		}

		pWidget = (MButton*)pResource->FindWidget("DynamicLight");
		if(pWidget)
		{
			Z_VIDEO_DYNAMICLIGHT = pWidget->GetCheck();
		}

		pWidget = (MButton*)pResource->FindWidget("Shader");
		if(pWidget)
		{
			Z_VIDEO_SHADER = pWidget->GetCheck();

			if( Z_VIDEO_SHADER )
			{
				RGetShaderMgr()->SetEnable();
			}
			else
			{
				RGetShaderMgr()->SetDisable();
			}
			//*/
		}

		// Custom: Windowed mode widget
		bool bResetDevice = false;
		MComboBox* pNewWidget = (MComboBox*)pResource->FindWidget("WindowedMode");
		if (pNewWidget)
		{
			int nScreenType = pNewWidget->GetSelIndex();
			if (nScreenType != Z_VIDEO_FULLSCREEN)
			{
				Z_VIDEO_FULLSCREEN = nScreenType;
				bResetDevice = true;
			}
		}

		MComboBox* pComboBox = (MComboBox*)pResource->FindWidget("SwordTrail");
		if( pComboBox )
		{
			ZGetConfiguration()->GetEtc()->nSwordTrail = pComboBox->GetSelIndex();
			RSetTrackSize(ZGetConfiguration()->GetEtc()->nSwordTrail);
		}
		pWidget = (MButton*)pResource->FindWidget("WeaponRotation");
		if( pWidget )
		{
			ZGetConfiguration()->GetEtc()->bWeaponRotation = pWidget->GetCheck();
		}


		// Custom: 24bit depth buffer
		pWidget = (MButton*)pResource->FindWidget("Stencil");
		if (pWidget)
		{
			bool bStencil = pWidget->GetCheck();
			if (bStencil != Z_VIDEO_STENCILBUFFER)
			{
				Z_VIDEO_STENCILBUFFER = pWidget->GetCheck();
				bResetDevice = true;
			}
		}

		pWidget = (MButton*)pResource->FindWidget("ShotEffects");
		if (pWidget)
		{
			bool bOldVal = pWidget->GetCheck();
			if (bOldVal != ZGetConfiguration()->GetVideo()->bShotEffects)
			{
				ZGetConfiguration()->GetVideo()->bShotEffects = pWidget->GetCheck();
			}
		}

		pWidget = (MButton*)pResource->FindWidget("BulletMarks");
		if (pWidget)
		{
			bool bBulletMarks = pWidget->GetCheck();
			if (bBulletMarks != Z_VIDEO_BULLET_MARKS)
			{
				Z_VIDEO_BULLET_MARKS = pWidget->GetCheck();
			}
		}

		pWidget = (MButton*)pResource->FindWidget("SparkShot");
		if (pWidget)
		{
			bool bSparkShot = pWidget->GetCheck();
			if (bSparkShot != Z_ETC_SPARK)
			{
				Z_ETC_SPARK = pWidget->GetCheck();
			}
		}
		pWidget = (MButton*)pResource->FindWidget("NewBar");
		if (pWidget)
		{
			bool bNewBar = pWidget->GetCheck();
			if (bNewBar != Z_ETC_NEWBAR)
			{
				Z_ETC_NEWBAR = pWidget->GetCheck();
			}
		}
		pWidget = (MButton*)pResource->FindWidget("Emoticon");
		if (pWidget)
		{
			bool bEmote = pWidget->GetCheck();
			if (bEmote != Z_ETC_EMOTE)
			{
				Z_ETC_EMOTE = pWidget->GetCheck();
			}
		}
		pWidget = (MButton*)pResource->FindWidget("HitMaker");
		if (pWidget)
		{
			bool bHitMaker = pWidget->GetCheck();
			if (bHitMaker != Z_ECT_HITMAKER)
			{
				Z_ECT_HITMAKER = pWidget->GetCheck();
			}
		}
		pWidget = (MButton*)pResource->FindWidget("KillCam");
		if (pWidget)
		{
			bool bKillCam = pWidget->GetCheck();
			if (bKillCam != Z_ECT_KILLCAM)
			{
				Z_ECT_KILLCAM = pWidget->GetCheck();
			}
		}
		pWidget = (MButton*)pResource->FindWidget("Blood");
		if (pWidget)
		{
			bool bBlood = pWidget->GetCheck();
			if (bBlood != Z_ECT_BLOOD)
			{
				Z_ECT_BLOOD = pWidget->GetCheck();
			}
		}
#ifdef _BULLETBAR
		pWidget = (MButton*)pResource->FindWidget("BulletBar");
		if (pWidget)
		{
			bool bBulletBar = pWidget->GetCheck();
			if (bBulletBar != Z_ECT_BULLETBAR)
			{
				Z_ECT_BULLETBAR = pWidget->GetCheck();
			}
		}
#endif
#ifdef _NPCBAR
		pWidget = (MButton*)pResource->FindWidget("NPCBar");
		if (pWidget)
		{
			bool bNPCbar = pWidget->GetCheck();
			if (bNPCbar != Z_ECT_NPCBAR)
			{
				Z_ECT_NPCBAR = pWidget->GetCheck();
			}
		}
#endif

		pWidget = (MButton*)pResource->FindWidget("NewRenderer");
		if (pWidget)
		{
			ZGetConfiguration()->GetEtc()->bNewRenderer = pWidget->GetCheck();
		}

		pWidget = (MButton*)pResource->FindWidget("D3D9EX");
		if (pWidget) {
			OSVERSIONINFOEX os;
			os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
			GetVersionEx((OSVERSIONINFO*)&os);
			if (os.dwMajorVersion < 6)
				pWidget->Enable(false);

			pWidget->SetCheck(ZGetConfiguration()->GetEtc()->enableD3D9Ex);
		}

		pWidget = (MButton*)pResource->FindWidget("NoFog");
		if (pWidget)
		{
			bool bNoFog = pWidget->GetCheck();
			if (bNoFog != Z_ECT_NOFOG)
			{
				Z_ECT_NOFOG = pWidget->GetCheck();
			}
		}
		pWidget = (MButton*)pResource->FindWidget("ScoreNew");
		if (pWidget)
		{
			bool bScore = pWidget->GetCheck();
			if (bScore != Z_ECT_SCORE)
			{
				Z_ECT_SCORE = pWidget->GetCheck();
			}
		}
		pWidget = (MButton*)pResource->FindWidget("ExtraInfo");
		if (pWidget)
		{
			bool bExtraInfo = pWidget->GetCheck();
			if (bExtraInfo != Z_ECT_EXTRA_SHOW)
			{
				Z_ECT_EXTRA_SHOW = pWidget->GetCheck();
			}
		}
		pWidget = (MButton*)pResource->FindWidget("RespawnEffect");
		if (pWidget)
		{
			bool bRT = pWidget->GetCheck();
			if (bRT != Z_ECT_RT)
			{
				Z_ECT_RT = pWidget->GetCheck();
			}
		}

		pWidget = (MButton*)pResource->FindWidget("NoSmoke");
		if (pWidget)
		{
			bool bNoSmoke = pWidget->GetCheck();
			if (bNoSmoke != Z_ECT_NOSMOKE)
			{
				Z_ECT_NOSMOKE = pWidget->GetCheck();
			}
		}
		pWidget = (MButton*)pResource->FindWidget("3DShotGun");
		if (pWidget)
		{
			bool b3dShotgun = pWidget->GetCheck();
			if (b3dShotgun != Z_ECT_3DSHOTGUN)
			{
				Z_ECT_3DSHOTGUN = pWidget->GetCheck();
			}
		}
		pWidget = (MButton*)pResource->FindWidget("FirstPerson");
		if (pWidget)
		{
			bool bFirstPerson = pWidget->GetCheck();
			if (bFirstPerson != Z_ETC_PERSON)
			{
				Z_ETC_PERSON = pWidget->GetCheck();
			}
		}

		pWidget = (MButton*)pResource->FindWidget("PingColors");
		if (pWidget)
		{
			bool bPingColors = pWidget->GetCheck();
			if (bPingColors != Z_ETC_COLORS)
			{
				Z_ETC_COLORS = pWidget->GetCheck();
			}
		}
		pWidget = (MButton*)pResource->FindWidget("NewChatEnable");
		if (pWidget)
		{
			bool bNewChat = pWidget->GetCheck();
			if (bNewChat != Z_ETC_CHATNEW)
			{
				Z_ETC_CHATNEW = pWidget->GetCheck();
			}
		}
#ifdef _CHATBACKGROUND
		pWidget = (MButton*)pResource->FindWidget("ChatBackGroundNormal");
		if (pWidget)
		{
			bool bChatBackground = pWidget->GetCheck();
			if (bChatBackground != Z_ETC_CHATBACKGROUND)
			{
				Z_ETC_CHATBACKGROUND = pWidget->GetCheck();
			}
		}
#endif

		pWidget = (MButton*)pResource->FindWidget("KillStreak");
		if (pWidget)
		{
			bool bKill = pWidget->GetCheck();
			if (bKill != Z_ECT_KILL)
			{
				Z_ECT_KILL = pWidget->GetCheck();
			}
		}

		pWidget = (MButton*)pResource->FindWidget("LowPoly");
		if (pWidget)
		{
			bool bLowPoly = pWidget->GetCheck();
			if (bLowPoly != Z_ECT_LOWPOLY)
			{
				Z_ECT_LOWPOLY = pWidget->GetCheck();
			}
		}

#ifdef _VOICE_CHAT
		pWidget = (MButton*)pResource->FindWidget("VoiceChatToggle");
		if (pWidget)
		{
			bool bVoiceChat = pWidget->GetCheck();
			if (bVoiceChat != Z_ECT_VOICE)
			{
				Z_ECT_VOICE = pWidget->GetCheck();
			}
		}
#endif

		pWidget = (MButton*)pResource->FindWidget("CountryFlags");
		if (pWidget)
		{
			bool bCountryFlags = pWidget->GetCheck();
			if (bCountryFlags != Z_ETC_FLAG)
			{
				Z_ETC_FLAG = pWidget->GetCheck();
			}
		}

		pWidget = (MButton*)pResource->FindWidget("LatencyGraphic");
		if (pWidget)
		{
			bool bLatencyGraphic = pWidget->GetCheck();
			if (bLatencyGraphic != Z_ETC_LATENCY)
			{
				Z_ETC_LATENCY = pWidget->GetCheck();
			}
		}

		pWidget = (MButton*)pResource->FindWidget("PostureChar");
		if (pWidget)
		{
			bool bPosture = pWidget->GetCheck();
			if (bPosture != Z_ETC_POSTURECHAR)
			{
				Z_ETC_POSTURECHAR = pWidget->GetCheck();
			}
		}

		int nSamplingType = 0;
		pComboBox = (MComboBox*)pResource->FindWidget("MultiSampling");
		if (pComboBox)
		{
			nSamplingType = pComboBox->GetSelIndex();
			if (Z_VIDEO_MULTISAMPLING != nSamplingType)
			{
				Z_VIDEO_MULTISAMPLING = pComboBox->GetSelIndex();
				bResetDevice = true;
			}
		}

		//{
		//	pComboBox = (MComboBox*)pResource->FindWidget("ChatBackgroundColorOption");
		//	if (pWidget)
		//	{
		//		Z_CHAT_BACKGROUND_COLOR = pComboBox->GetSelIndex() + 1;
		//	}

		//	GetRGMain().GetChat().SetBackgroundColor();
		//}
		
		if (bResetDevice)
		{
			RMODEPARAMS Params = { RGetScreenWidth(), RGetScreenHeight(), Z_VIDEO_FULLSCREEN, RGetPixelFormat() };
			RAdvancedGraphics::bDepthBuffering = Z_VIDEO_STENCILBUFFER;
			RAdvancedGraphics::nMultiSampling = Z_VIDEO_MULTISAMPLING;
			RResetDevice(&Params);

		}

		pWidget = (MButton*)pResource->FindWidget("FrameLimit_Display");
		if( pWidget )
		{
			ZGetConfiguration()->GetEtc()->bShowFPS = pWidget->GetCheck();
		}
		pWidget = (MButton*)pResource->FindWidget("HPAPScoreboard");
		if (pWidget)
		{
			ZGetConfiguration()->GetEtc()->bHPAPScoreboard = pWidget->GetCheck();
		}
		pWidget = (MButton*)pResource->FindWidget("ChatFilter");
		if (pWidget)
		{
			ZGetConfiguration()->GetEtc()->bChatFilter = pWidget->GetCheck();
		}
		// Custom: End widgets

		pWidget	= (MButton*)pResource->FindWidget("BGMMute");
		if( pWidget )
		{
			Z_AUDIO_BGM_MUTE	= !(pWidget->GetCheck());

			ZGetSoundEngine()->SetMusicMute( Z_AUDIO_BGM_MUTE );
		}
		pWidget	= (MButton*)pResource->FindWidget("EffectMute");
		if(pWidget)
		{
			Z_AUDIO_EFFECT_MUTE = !(pWidget->GetCheck());
			ZGetSoundEngine()->SetEffectMute( Z_AUDIO_EFFECT_MUTE );

		}
		// Custom: 3D shotgun sounds
		pWidget	= (MButton*)pResource->FindWidget("Effect3D");
		if(pWidget)
		{
			Z_AUDIO_3D_SOUND = pWidget->GetCheck();		
		
		}
		pWidget = (MButton*)pResource->FindWidget("8BitSound");
		if(pWidget)
		{
			Z_AUDIO_8BITSOUND = pWidget->GetCheck();
#ifdef _BIRDSOUND

#else
			ZGetSoundEngine()->SetSamplingBits(Z_AUDIO_8BITSOUND);
#endif
		}
		pWidget = (MButton*)pResource->FindWidget("InverseSound");
		if(pWidget)
		{
			Z_AUDIO_INVERSE = pWidget->GetCheck();
#ifdef _BIRDSOUND

#else
			ZGetSoundEngine()->SetInverseSound( Z_AUDIO_INVERSE );
#endif
		}
		pWidget = (MButton*)pResource->FindWidget("HWMixing");
		if(pWidget)
		{
			Z_AUDIO_HWMIXING = pWidget->GetCheck();
#ifdef _BIRDSOUND

#else
			ZGetSoundEngine()->Reset(g_hWnd, Z_AUDIO_HWMIXING);
#endif
		}
		pWidget = (MButton*)pResource->FindWidget("HitSound");
		if(pWidget)
		{
			Z_AUDIO_HITSOUND = pWidget->GetCheck();
		}
		pWidget = (MButton*)pResource->FindWidget("NarrationSound");
		if(pWidget)
		{
			Z_AUDIO_NARRATIONSOUND = pWidget->GetCheck();
		}
		pWidget = (MButton*)pResource->FindWidget("InvertMouse");
		if(pWidget)
		{
			Z_MOUSE_INVERT = pWidget->GetCheck();
		}
		pWidget = (MButton*)pResource->FindWidget("InvertJoystick");
		if(pWidget)
		{
			Z_JOYSTICK_INVERT = pWidget->GetCheck();
		}
		/*pWidget = (MButton*)pResource->FindWidget("HPAPScoreboard");
		if (pWidget)
		{
			pWidget->SetCheck(Z_ETC_HPAP_SCOREBOARD);
		}*/
		//MComboBox* MCombobox = (MComboBox*)pResource->FindWidget("ChatBackgroundColorOption");
		//if (MCombobox)
		//{
		//	Z_CHAT_BACKGROUND_COLOR = MCombobox->GetSelIndex() + 1;
		//}

		//GetRGMain().GetChat().SetBackgroundColor();
	}
	{	// Etc
		MEdit* pEdit = (MEdit*)pResource->FindWidget("NetworkPort1");
		if (pEdit)
		{
			int nPreviousPort = Z_ETC_NETWORKPORT1;
			Z_ETC_NETWORKPORT1 = atoi(pEdit->GetText());
		}

		pEdit = (MEdit*)pResource->FindWidget("NetworkPort2");
		if (pEdit)
		{
			int nPreviousPort = Z_ETC_NETWORKPORT2;
			Z_ETC_NETWORKPORT2 = atoi(pEdit->GetText());
		}

		MButton* pBoost = (MButton*)pResource->FindWidget("BoostOption");
		if(pBoost)
		{
			if (Z_ETC_BOOST != pBoost->GetCheck()) {
				Z_ETC_BOOST = pBoost->GetCheck();
				if (Z_ETC_BOOST)
					ZGetGameClient()->PriorityBoost(true);
				else
					ZGetGameClient()->PriorityBoost(false);
			}
		}

		MButton* pNormalChat = (MButton*)pResource->FindWidget("NormalChatOption");
		if(pNormalChat)
		{
			if (Z_ETC_REJECT_NORMALCHAT != pNormalChat->GetCheck()) {
				Z_ETC_REJECT_NORMALCHAT = pNormalChat->GetCheck();
				if (Z_ETC_REJECT_NORMALCHAT)
					ZGetGameClient()->SetRejectNormalChat(true);
				else
					ZGetGameClient()->SetRejectNormalChat(false);
			}
		}

		MButton* pTeamChat = (MButton*)pResource->FindWidget("TeamChatOption");
		if(pTeamChat)
		{
			if (Z_ETC_REJECT_TEAMCHAT != pTeamChat->GetCheck()) {
				Z_ETC_REJECT_TEAMCHAT = pTeamChat->GetCheck();
				if (Z_ETC_REJECT_TEAMCHAT)
					ZGetGameClient()->SetRejectTeamChat(true);
				else
					ZGetGameClient()->SetRejectTeamChat(false);
			}
		}

		MButton* pClanChat = (MButton*)pResource->FindWidget("ClanChatOption");
		if(pClanChat)
		{
			if (Z_ETC_REJECT_CLANCHAT != pClanChat->GetCheck()) {
				Z_ETC_REJECT_CLANCHAT = pClanChat->GetCheck();
				if (Z_ETC_REJECT_CLANCHAT)
					ZGetGameClient()->SetRejectClanChat(true);
				else
					ZGetGameClient()->SetRejectClanChat(false);
			}
		}

		MButton* pWhisper = (MButton*)pResource->FindWidget("WhisperOption");
		if(pWhisper)
		{
			if (Z_ETC_REJECT_WHISPER != pWhisper->GetCheck()) {
				Z_ETC_REJECT_WHISPER = pWhisper->GetCheck();
				if (Z_ETC_REJECT_WHISPER)
					ZGetGameClient()->SetRejectWhisper(true);
				else
					ZGetGameClient()->SetRejectWhisper(false);
				ZPostUserOption();
			}
		}

		MButton* pInvite = (MButton*)pResource->FindWidget("InviteOption");
		if(pInvite)
		{
			if (Z_ETC_REJECT_INVITE != pInvite->GetCheck()) {
				Z_ETC_REJECT_INVITE = pInvite->GetCheck();
				if (Z_ETC_REJECT_INVITE)
					ZGetGameClient()->SetRejectInvite(true);
				else
					ZGetGameClient()->SetRejectInvite(false);
				ZPostUserOption();
			}
		}

		MComboBox* pComboBox = (MComboBox*)pResource->FindWidget("CrossHairComboBox");
		if (pComboBox)
		{
			Z_ETC_CROSSHAIR = pComboBox->GetSelIndex();
		}
#ifdef _CAMERADISTANCE
		pComboBox = (MComboBox*)pResource->FindWidget("CameraDistance");
		if (pComboBox)
		{
			Z_ETC_CAMERA = pComboBox->GetSelIndex();
			ZGetCamera()->RSetCameraDistance(Z_ETC_CAMERA);
			ZGetCamera()->UpdateDist();
		}
#endif
#ifdef _CHATBACKGROUND
		pEdit = (MEdit*)pResource->FindWidget("ChatBackGroundOpacity");
		if (pEdit)
		{
			int nOpacity = atoi(pEdit->GetText());

			if (nOpacity < 1 || nOpacity > 100)
				nOpacity = 0;

			Z_ETC_CHATOPACITY = atoi(pEdit->GetText());
		}
#endif

#ifdef _CROSS_SIZE_BOX
		pEdit = (MEdit*)pResource->FindWidget("CrossSize");
		if (pEdit)
		{
			float nCross_Size = atoi(pEdit->GetText());

			if (nCross_Size < 50.0f || nCross_Size > 250.0f)
				nCross_Size = 50.0f;

			Z_ETC_CROSS_SIZE = atoi(pEdit->GetText());
		}
#endif

		// Custom: FrameLimit_PerSecond is now an editable box
		pEdit = (MEdit*)pResource->FindWidget("FrameLimit_PerSecond");
		if (pEdit)
		{
			int nFrameLimit = atoi(pEdit->GetText());

			if (nFrameLimit < 60 || nFrameLimit > 500)
				nFrameLimit = 0;

			Z_ETC_FRAMELIMIT_PERSECOND = atoi(pEdit->GetText());
			RSetFrameLimitPerSeceond(Z_ETC_FRAMELIMIT_PERSECOND);
		}
		//pComboBox = (MComboBox*)pResource->FindWidget("WaterQuality");
		//if (pComboBox)
		//{
		//	int i = pComboBox->GetSelIndex();
		//	ZGetGame()->GetWorld()->GetWaters()->OnInvalidate();
		//	if (Z_ETC_WATERQUALITY != i)
		//	{
		//		Z_ETC_WATERQUALITY = i;
		//	}
		//	RSetWaterQuality(i);
		//	ZGetGame()->GetWorld()->GetWaters()->OnRestore();
		//}

		/*
		pComboBox = (MComboBox*)pResource->FindWidget("FrameLimit_PerSecond");
		if (pComboBox)
		{
			Z_ETC_FRAMELIMIT_PERSECOND = pComboBox->GetSelIndex();
			RSetFrameLimitPerSeceond(Z_ETC_FRAMELIMIT_PERSECOND);
		}
		*/
	}

	{
		auto&& Cfg = *ZGetConfiguration();

		const auto OldFont = Cfg.GetChat()->Font;
		const auto OldBoldFont = Cfg.GetChat()->BoldFont;
		const auto OldFontSize = Cfg.GetChat()->FontSize;


		std::pair<const char*, bool*> Buttons[] = {
			{ "ChatFontBoldOption",      &Cfg.GetChat()->BoldFont },
			{ "ChatTimeStampOption",     &Cfg.GetChat()->TimeStamp },
			{ "ChatEnableCursorOption",  &Cfg.GetChat()->EnableCursor },
			{"",false},
		};

		for (auto&& ButtonInfo : Buttons)
		{
			auto ButtonWidget = ZFindWidgetAs<MButton>(ButtonInfo.first);
			if (ButtonWidget)
				*ButtonInfo.second = ButtonWidget->GetCheck();
		}

		using SetEditFunction = void(*)(const char*);
		std::pair<const char*, SetEditFunction> Edits[] = {
			{ "ChatFontOption", [](const char* Value) {
				ZGetConfiguration()->GetChat()->Font = Value; } },
			{ "ChatFontSizeOption", [](const char* Value) {
				ZGetConfiguration()->GetChat()->FontSize = StringToInt<int>(Value).value_or(15); } },
			{ "ChatBackgroundColorOption", [](const char* Value) {
				ZGetConfiguration()->GetChat()->BackgroundColor =
					StringToInt<u32, 16>(Value).value_or(0x80000000); } },
			{ "",false },
#ifdef _TRAILCOLOR
			{ "TrailColor", [](const char* Value) {
				ZGetConfiguration()->GetEtc()->bTrailColors =
					StringToInt<u32, 16>(Value).value_or(0x80000000); } },
			{ "",false },
#endif
		};

		for (auto&& EditInfo : Edits)
		{
			auto EditWidget = ZFindWidgetAs<MEdit>(EditInfo.first);
			if (EditWidget)
				EditInfo.second(EditWidget->GetText());
		}

		const auto& CurFont = Cfg.GetChat()->Font;
		const auto CurBoldFont = Cfg.GetChat()->BoldFont;
		auto CurFontSize = Cfg.GetChat()->FontSize;
		if (!iequals(OldFont, CurFont) || OldBoldFont != CurBoldFont)
		{
			GetRGMain().GetChat().SetFont(CurFont, CurBoldFont);
		}
		if (OldFontSize != CurFontSize)
		{
			if (CurFontSize >= 20)
				CurFontSize = 20;
			else if (CurFontSize <= 10)
				CurFontSize = 10;

			GetRGMain().GetChat().SetFontSize(CurFontSize);
		}
		GetRGMain().GetChat().SetBackgroundColor(Cfg.GetChat()->BackgroundColor);
	}
	{
		// Macro

		static char stemp_str[ZCONFIG_MACRO_MAX][80] = {
			"MacroF1",
			"MacroF2",
			"MacroF3",
			"MacroF4",
			"MacroF5",
			"MacroF6",
			"MacroF7",
			"MacroF8",
		};

		ZCONFIG_MACRO* pMacro = ZGetConfiguration()->GetMacro();

		if(pMacro) {

			MEdit* pEdit = NULL;

			for(int i=0;i<ZCONFIG_MACRO_MAX;i++) {

				pEdit = (MEdit*) pResource->FindWidget(stemp_str[i]);

				if (pEdit) {

					pMacro->SetString(i, (char*)pEdit->GetText());
				}
			}
		}

	}

	// ∞®∏∂∞™ ¿˙¿ÅE
	MSlider* pSlider = (MSlider*)pResource->FindWidget("VideoGamma");
	if (pSlider != NULL) 
	{
		Z_VIDEO_GAMMA_VALUE = pSlider->GetValue();
	}

	ZGetConfiguration()->Save( Z_LOCALE_XML_HEADER);

	return true;
}



void ZOptionInterface::ShowResizeConfirmDialog( bool Resized )
{
	if( Resized )
	{
		ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
		MWidget* pWidget = pResource->FindWidget("ViewConfirm");
		if(pWidget!= 0)
			pWidget->Show( true, true );
	}
	else
	{
		ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
		MWidget* pWidget = pResource->FindWidget("ResizeConfirm");
		if(pWidget!= 0)
			pWidget->Show( true, true );
	}
}

bool ZOptionInterface::SetTimer( bool b, float time /* = 0.f  */ )
{
	static DWORD DeadTime = 0;

	if( !b )
	{
		mbTimer = b;
		return false;
	}

	if( !mbTimer )
	{
		mTimerTime	= timeGetTime();
		mbTimer		= true;
		DeadTime		= time*1000;
	}

	if(( timeGetTime() - mTimerTime ) > DeadTime )
	{
		DeadTime = 0;
		mbTimer	= false;
		return true;
	}
	else
	{
		char szBuf[128];
		sprintf(szBuf, "%d", min(max( (10 - (int)(( timeGetTime() - mTimerTime ) * 0.001)),0),10));

		char szText[ 128];
		ZTransMsg( szText, MSG_BACKTOTHEPREV, 1, szBuf);

		ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
		MLabel* Countdown = (MLabel*)pResource->FindWidget( "ViewConfirm_CountDown" );
		if ( Countdown)
			Countdown->SetText( szText);
	}
	return false;
}

void ZOptionInterface::ShowNetworkPortConfirmDialog()
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MWidget* pWidget = pResource->FindWidget("NetworkPortConfirm");
	if(pWidget!= 0) pWidget->Show( true, true );
}

bool ZOptionInterface::IsDiffNetworkPort()
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	int nCurrPort = ntohs( ZGetGameClient()->GetSafeUDP()->GetLocalPort() );

	int nNewPort1, nNewPort2;

	MEdit* pEdit1 = (MEdit*)pResource->FindWidget("NetworkPort1");
	if ( pEdit1)
		nNewPort1 = atoi( pEdit1->GetText());
	else
		return false;


	MEdit* pEdit2 = (MEdit*)pResource->FindWidget("NetworkPort2");
	if ( pEdit2)
		nNewPort2 = atoi( pEdit2->GetText());
	else
		return false;


	if ( nNewPort1 > nNewPort2)
	{
		char szStr[ 25];
		itoa( Z_ETC_NETWORKPORT1, szStr, 10);
		pEdit1->SetText( szStr);
		itoa( Z_ETC_NETWORKPORT2, szStr, 10);
		pEdit2->SetText( szStr);

		return false;
	}


	if ( ( nNewPort1 != Z_ETC_NETWORKPORT1) || ( nNewPort2 != Z_ETC_NETWORKPORT2))
		return true;

	return false;
}

// Custom: Windowed mode widget
void ZOptionInterface::ShowWindowConfirmDialog()
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MWidget* pWidget = pResource->FindWidget("WindowConfirm");

	if(pWidget!= 0) pWidget->Show( true, true );
}

bool ZOptionInterface::IsDiffWindow()
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	int nScreenType = 0;

	MComboBox* pComboBox = (MComboBox*)pResource->FindWidget("WindowedMode");
	if (pComboBox)
		nScreenType = pComboBox->GetSelIndex();
	else
		return false;

	if (nScreenType != Z_VIDEO_FULLSCREEN)
	{
		Z_VIDEO_FULLSCREEN = nScreenType;
		return true;
		ZGetGameInterface()->ReserveResetApp(true);
	}

	return false;
}

void ZOptionInterface::OptimizationVideoOption()
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MButton* pButton = 0;
	MComboBox* pCombo = 0;
	MLabel* pLabel = 0;

	ZGetConfiguration()->SetForceOptimization( true );

	if(!RIsHardwareTNL())
	{	
		pCombo = (MComboBox*)pResource->FindWidget("CharTexLevel");
		if(pCombo!=0) pCombo->SetSelIndex(2); // ≥™ª›
		pCombo = (MComboBox*)pResource->FindWidget("MapTexLevel");
		if(pCombo!=0) pCombo->SetSelIndex(2); // ≥™ª›
		pCombo = (MComboBox*)pResource->FindWidget("EffectLevel");
		if(pCombo!=0) pCombo->SetSelIndex(2); // ≥™ª›
		pCombo = (MComboBox*)pResource->FindWidget("TextureFormat");
		if(pCombo!=0) pCombo->SetSelIndex(0); // 16 bit

		pButton = (MButton*)pResource->FindWidget("Reflection");
		if(pButton!=0) pButton->SetCheck(false);
		pButton = (MButton*)pResource->FindWidget("LightMap");
		if(pButton!=0) pButton->SetCheck(false);		
		pButton = (MButton*)pResource->FindWidget("DynamicLight");
		if(pButton!=0) pButton->SetCheck(false);

		pCombo	= (MComboBox*)pResource->FindWidget("ScreenResolution");
		if( pCombo != 0)
		{
			D3DDISPLAYMODE ddm;
			// Custom: Deprecate 640x480
			ddm.Width = 800;//640;
			ddm.Height = 600;//480;
			ddm.Format = D3DFMT_R5G6B5;
			ddm.RefreshRate = DEFAULT_REFRESHRATE;
			map<int, D3DDISPLAYMODE>::iterator iter_ = find_if( gDisplayMode.begin(), gDisplayMode.end(), value_equals<int, D3DDISPLAYMODE>(ddm));
			if( iter_ != gDisplayMode.end() )
			{
				int n = iter_->first;
				pCombo->SetSelIndex( n );
			}
		}

		ZGetConfiguration()->GetVideo()->bTerrible = true;
		return;
	}	

	ZGetConfiguration()->GetVideo()->bTerrible = false;

	int nVMem = RGetApproxVMem() /1024 /1024;
	if( nVMem < 32 )
	{		
		pCombo = (MComboBox*)pResource->FindWidget("CharTexLevel");
		if(pCombo!=0) pCombo->SetSelIndex(2); // ≥™ª›
		pCombo = (MComboBox*)pResource->FindWidget("MapTexLevel");
		if(pCombo!=0) pCombo->SetSelIndex(2); // ≥™ª›
		pCombo = (MComboBox*)pResource->FindWidget("EffectLevel");
		if(pCombo!=0) pCombo->SetSelIndex(2); // ≥™ª›
		pCombo = (MComboBox*)pResource->FindWidget("TextureFormat");
		if(pCombo!=0) pCombo->SetSelIndex(0); // 16 bit

		pButton = (MButton*)pResource->FindWidget("Reflection");
		if(pButton!=0) pButton->SetCheck(false);
		pButton = (MButton*)pResource->FindWidget("DynamicLight");
		if(pButton!=0) pButton->SetCheck(false);		
	}
	else if( nVMem < 64 )
	{
		pCombo = (MComboBox*)pResource->FindWidget("CharTexLevel");
		if(pCombo!=0) pCombo->SetSelIndex(1); // ∫∏≈ÅE
		pCombo = (MComboBox*)pResource->FindWidget("MapTexLevel");
		if(pCombo!=0) pCombo->SetSelIndex(2); // ≥™ª›
		pCombo = (MComboBox*)pResource->FindWidget("EffectLevel");
		if(pCombo!=0) pCombo->SetSelIndex(1); // ∫∏≈ÅE
		pCombo = (MComboBox*)pResource->FindWidget("TextureFormat");
		if(pCombo!=0) pCombo->SetSelIndex(0); // 16 bit

		pButton = (MButton*)pResource->FindWidget("Reflection");
		if(pButton!=0) pButton->SetCheck(false);
		pButton = (MButton*)pResource->FindWidget("DynamicLight");
		if(pButton!=0) pButton->SetCheck(true);
	}
	else // nVMem > 64
	{
		pCombo = (MComboBox*)pResource->FindWidget("CharTexLevel");
		if(pCombo!=0) pCombo->SetSelIndex(1); // ∫∏≈ÅE
		pCombo = (MComboBox*)pResource->FindWidget("MapTexLevel");
		if(pCombo!=0) pCombo->SetSelIndex(1); // ∫∏≈ÅE
		pCombo = (MComboBox*)pResource->FindWidget("EffectLevel");
		if(pCombo!=0) pCombo->SetSelIndex(0); // ¡¡¿Ω
		pCombo = (MComboBox*)pResource->FindWidget("TextureFormat");
		if(pCombo!=0) pCombo->SetSelIndex(1); // 32 bit

		pButton = (MButton*)pResource->FindWidget("Reflection");
		if(pButton!=0) pButton->SetCheck(true);		
		pButton = (MButton*)pResource->FindWidget("DynamicLight");
		if(pButton!=0) pButton->SetCheck(true);
	}

	//pLabel = (MLabel*)pResource->FindWidget("Lightmap Label");
	//if(pLabel!=0) pLabel->SetTextColor(MCOLOR(64,64,64));
	pButton = (MButton*)pResource->FindWidget("LightMap");
	if(pButton!=0) {
		pButton->SetCheck(true);		
		//pButton->Enable(false);
	}

	if(RIsSupportVS())
	{
		pButton = (MButton*)pResource->FindWidget("Shader");
		if(pButton!=0) pButton->SetCheck(true);
	}
	else
	{		
		pButton = (MButton*)pResource->FindWidget("Shader");
		if(pButton!=0) pButton->SetCheck(false);
	}
}

bool ZOptionInterface::ResizeWidgetRecursive( MWidget* pWidget/*, int w, int h*/)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	//MWidget* pWidget = pResource->FindWidget(szName);
	if(pWidget==NULL) return false;
	int n = pWidget->GetChildCount();
	for( int i = 0; i < n; ++i)
	{
		MWidget* pChildWidget = pWidget->GetChild(i);
		ResizeWidgetRecursive( pChildWidget/*, w, h */);
	}

	// idlø°º≠ ¿–æ˚ÿ≠ ¿˚¿˝«— ∞™¿ª ∞°¡ˆ∞ÅE¿÷¥¬ ¿ß¡¨¿Ã∂Û∏ÅE±◊∞™¿∏∑Œ ∏ÆªÁ¿Ã¡˚„—¥Ÿ
	if(pWidget->GetIDLRect().w>0 && pWidget->GetIDLRect().h>0)	
	{
		// idl ø°º≠¥¬ 800x600 ±‚¡ÿ¿∏∑Œ ±‚º˙µ«æ˚‹÷¥Ÿ
		const float tempWidth = ((float)RGetScreenWidth()) / 800;
		const float tempHeight = ((float)RGetScreenHeight()) / 600;

		MRECT r=pWidget->GetIDLRect();
		r.x*=tempWidth;
		r.w*=tempWidth;
		r.y*=tempHeight;
		r.h*=tempHeight;
		pWidget->SetBounds(r);
	}
	else 
	{
		const float tempWidth = ((float)RGetScreenWidth()) / mOldScreenWidth;
		const float tempHeight = ((float)RGetScreenHeight()) / mOldScreenHeight;
		MPOINT p = pWidget->GetPosition();
		p.Scale( tempWidth, tempHeight );
		pWidget->SetPosition( p );
		MRECT r=pWidget->GetRect();
		pWidget->SetSize( r.w*tempWidth, r.h*tempHeight );
	}
	return true;
}

void ZOptionInterface::AdjustMultipliedWidgetsManually()
{
	// ±‚¡∏«‘ºÅE-> bool ZOptionInterface::ResizeWidget(const char* szName, int w, int h) ø°º≠ ∞°¡Æø¬ ƒ⁄µÂ¥Ÿ
	// «œµÂƒ⁄µ˘µÈ¿Ã¥Ÿ......

	int w = RGetScreenWidth();
	int h = RGetScreenHeight();

	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MWidget* pWidget;
	
	pWidget = pResource->FindWidget("Login");
	if (pWidget) {
		// Resize frame
		pWidget->SetSize( w, h);

		// Resize background image
		pWidget = pResource->FindWidget( "Login_BackgrdImg");
		if ( pWidget)
			pWidget->SetSize( w, h);

		// Reposition login frame
		pWidget = pResource->FindWidget( "LoginFrame");
		if ( pWidget)
		{
			MRECT rect;
			rect = pWidget->GetRect();

			//jintriple3 «ÿªÛµµø° µ˚∏• º≠πÅEº±≈√ √¢ ¿ßƒ° ¡ﬂæ”ø°º≠ æ‡∞£ æ∆∑°∑Œ...
			/*	rect.x = (w / 2) - (rect.w / 2) + 5;
			rect.y = h - rect.h - 10;
			*/
			rect.x = (w / 2) - (rect.w / 2) + 5;
			rect.y = (int)((float)h * 0.555f);
			if( rect.h + rect.y > h )
				rect.y = h - rect.h - 10;
			pWidget->SetBounds( rect);
		}

		// REposition connecting message
		pWidget = pResource->FindWidget( "Login_ConnectingMsg");
		if ( pWidget)
		{
			
			/*MRECT rect;
			rect = pWidget->GetRect();
			rect.x = 0;
			rect.y = (int)(h * 0.66f);
			rect.w = w;
			pWidget->SetBounds( rect);
			*/
		}
	}

	/*pWidget = pResource->FindWidget("Shop");
	if (pWidget) {
		//_AdjustMultipliedItemListManually("AllEquipmentList", "Shop_ListLabel");
		//_AdjustMultipliedItemListManually("MyAllEquipmentList", "Shop_ListLabel");
		//_AdjustMultipliedItemListManually("CashEquipmentList", "Shop_ListLabel");

		ZGetGameInterface()->GetShopEquipInterface()->SelectEquipmentFrameList( "Shop", false);
	}

	pWidget = pResource->FindWidget("Equipment");
	if (pWidget) {
		//_AdjustMultipliedItemListManually("EquipmentList", "Equip_ListLabel1");
		//_AdjustMultipliedItemListManually("AccountItemList", "Equip_ListLabel2");

		ZGetGameInterface()->GetShopEquipInterface()->SelectEquipmentFrameList( "Equip", true);
	}*/
	ZGetGameInterface()->GetShopEquipInterface()->SelectEquipmentFrameList( NULL, true);

/*
	pWidget = pResource->FindWidget("Replay_FileList");
	if (pWidget) {
		enum { LF_FILENAME, LF_VERSION, LF_MAX };
		MListBox* pReplayListBox = (MListBox*)pWidget;
		MRECT rc = pReplayListBox->GetClientRect();
		if (pReplayListBox->GetFieldCount() == LF_MAX) {
			pReplayListBox->GetField(LF_FILENAME)->nTabSize = rc.0.75f *
		} else {
			ASSERT(0);
		}
	}*/

	pWidget = pResource->FindWidget("Stage");
	if (pWidget) {
		ZApplication::GetStageInterface()->GetSacrificeItemBoxPos();
		ZApplication::GetStageInterface()->SetRelayMapBoxPos(0);
	}

	pWidget = pResource->FindWidget("Lobby");
	if (pWidget) {
		ZRoomListBox* pRoomList;

		pRoomList = (ZRoomListBox*)pResource->FindWidget( "Lobby_StageList" );
		if( pRoomList != 0 )
		{
			const float tempWidth = ((float)RGetScreenWidth()) / mOldScreenWidth;
			const float tempHeight = ((float)RGetScreenHeight()) / mOldScreenHeight;
			pRoomList->Resize( tempWidth, tempHeight );
		}
	}

	pWidget = pResource->FindWidget("CharCreation");
	if (pWidget) {
		MRECT rect;
		rect = pWidget->GetRect();
		rect.x = 50 * ( RGetScreenWidth() / 800.0f);
		rect.y = (int)((RGetScreenHeight() - rect.h) / 2.0f);
		pWidget->SetBounds( rect);
	}

	pWidget = pResource->FindWidget("CombatDTInfo");
	if (pWidget) {

	}
/*
	else if(stricmp( szName, "CombatDTInfo")==0)
		ResizeWidgetRecursive( pWidget, w, h );
	else if(stricmp( szName, "CombatDT_CharacterInfo")==0)
		ResizeWidgetRecursive( pWidget, w, h );
	*/

	pWidget = pResource->FindWidget("BuyItemDetailFrame_Thumbnail");
	if (pWidget) {	// ¿Ã ΩÊ≥◊¿œ ¿ÃπÃ¡ˆ¥¬ øÕ¿ÃµÅE«ÿªÛµµø°º≠µµ ¡§ªÁ∞¢?E∏∑?∫∏¿Ãµµ∑œ ¡∂¡§
		MRECT rc = pWidget->GetRect();
		rc.h = rc.w;
		pWidget->SetBounds(rc);
	}
}

extern MFontR2* g_pDefFont;
void ZOptionInterface::ResizeDefaultFont(int newScreenHeight)
{
	float fontResizeRatio = newScreenHeight / 600.f;

#ifdef _FONTSIZE
	if (!RGetIsHDScreen() && RGetWidthScreen() != 0.8f)
		fontResizeRatio *= 0.90f;
	else if (!RGetIsHDScreen() && RGetWidthScreen() == 0.8f)
		fontResizeRatio *= 0.80f;
#else
	return;
#endif // 

	g_pDefFont->Destroy();

	int newFontHeight = (int)(DEFAULT_FONT_HEIGHT * fontResizeRatio + 0.5f);	//UI¿€º∫Ω√ ±‚¡ÿ«ÿªÛµµ∞° 800*600¿Ãπ«∑Œ 600ø° ¥Î«ÿ ∞ËªÍ
	if (newFontHeight < FONT_MINIMUM_HEIGHT)
		newFontHeight = FONT_MINIMUM_HEIGHT;	// 640*480ø°º≠ ≥ π´ π∂∞≥¡ˆ¡ˆ æ ∞‘

	if (!g_pDefFont->Create("Default", Z_LOCALE_DEFAULT_FONT, newFontHeight, 1.0f))
	{
		mlog("Fail to Recreate default font : MFontR2 / screen resize\n");
		g_pDefFont->Destroy();
		SAFE_DELETE(g_pDefFont);
	}

	// UIø°º≠ ∑Œµ˘µ» ∆˘∆ÆµÈ ≈©±‚ ∫Ø∞Ê
	MFontManager::Resize(fontResizeRatio, FONT_MINIMUM_HEIGHT); //ø¯∑° ≈©±‚(800*600±‚¡ÿUI)ø° ¥Î«ÿº≠ ∏ÓπË¿≤∑Œ ∫Ø»≠«ÿæﬂ«œ¥¬¡ˆ∏¶ æÀ∑¡¡‹
}

void ZOptionInterface::Resize(int w, int h)
{
	ResizeDefaultFont(h);
	// «ÿªÛµµ ∫Ø∞ÊΩ√ ¿ß¡¨µÈ¿« ≈©±‚∏¶ πË¿≤«ÿ¡ÿ¥Ÿ
	if (mOldScreenHeight == 0) mOldScreenHeight = 600;
	if (mOldScreenWidth == 0) mOldScreenWidth = 800;

	ZGetGameInterface()->MultiplySize(w/800.f, h/600.f, w/float(mOldScreenWidth), h/float(mOldScreenHeight));

	AdjustMultipliedWidgetsManually();	// πË¿≤¿˚øÅEœ∞˙œ™º?¡˜¡¢ ∏≈∏∏¡Æ¡‡æﬂ «œ¥¬ ∫Œ∫–µÅE

	ZGetGameInterface()->SetSize(w, h);

	if (ZGetCombatInterface()) ZGetCombatInterface()->Resize(w, h);

	GetRGMain().Resize(w, h);
	// Custom: If ZOptionInterface is resized (anytime res is changed), update camera distance automatically.
	/*if (ZGetGameInterface() && !ZGetGameInterface()->GetGameClient()->GetMatchStageSetting()->IsModifierUsed(MMOD_FPS))
	{
		ZCamera* pCamera = ZGetGameInterface()->GetCamera();
		if (pCamera)
			pCamera->AdjustDist();
	}*/
}

void ZOptionInterface::GetOldScreenResolution()
{
	RMODEPARAMS ModeParams;
	ModeParams.nWidth	= mOldScreenWidth;
	ModeParams.nHeight	= mOldScreenHeight;
	ModeParams.nScreenType	= ZGetConfiguration()->GetVideo()->nScreenType;
	ModeParams.PixelFormat	= mnOldBpp;

	//mOldScreenWidth = RGetScreenWidth();
	//mOldScreenHeight = RGetScreenHeight();
	//mnOldBpp = RGetPixelFormat();

	RResetDevice( &ModeParams );
	Mint::GetInstance()->SetWorkspaceSize(ModeParams.nWidth, ModeParams.nHeight);
	Mint::GetInstance()->GetMainFrame()->SetSize(ModeParams.nWidth, ModeParams.nHeight);
	Resize(ModeParams.nWidth, ModeParams.nHeight);

	D3DDISPLAYMODE ddm;
	ddm.Width = ModeParams.nWidth;
	ddm.Height = ModeParams.nHeight;
	ddm.Format = ModeParams.PixelFormat;
	ddm.RefreshRate = DEFAULT_REFRESHRATE;

	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MComboBox *pWidget = (MComboBox*)pResource->FindWidget( "ScreenResolution" );

	map<int, D3DDISPLAYMODE>::iterator  iter = find_if( gDisplayMode.begin(), gDisplayMode.end(), value_equals<int, D3DDISPLAYMODE>(ddm));
	if( iter != gDisplayMode.end() )
		pWidget->SetSelIndex( iter->first );
}

bool ZOptionInterface::IsDiffScreenResolution()
{
	if (ZGetConfiguration()->GetVideo()->nScreenType == 2)
		return false;//the resolution never changes
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	MComboBox *pWidget = (MComboBox*)pResource->FindWidget("ScreenResolution");
	if(pWidget)
	{
		int nSel = pWidget->GetSelIndex();
		D3DDISPLAYMODE ddm = (gDisplayMode.find( nSel ))->second;

		// Custom: Window mode
		if( ddm.Width == RGetScreenWidth() && ddm.Height == RGetScreenHeight() && ddm.Format == RGetPixelFormat() /*&& mbOldScreenWindowed == RIsFullScreen()*/ )
			return false;
#ifdef _DEBUG
		mlog( "%d/%d , %d/%d, %d/%d\n", ddm.Width, RGetScreenWidth(), ddm.Height, RGetScreenHeight(), ddm.Format == D3DFMT_X8R8G8B8 ? 32 : 16, RGetPixelFormat() == D3DFMT_X8R8G8B8 ? 32 : 16 );
#endif
	}
	return true;
}

bool ZOptionInterface::TestScreenResolution()
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	MComboBox *pWidget = (MComboBox*)pResource->FindWidget("ScreenResolution");
	if(pWidget)
	{
		RMODEPARAMS	ModeParams;

		map<int, D3DDISPLAYMODE>::iterator iter = gDisplayMode.find( pWidget->GetSelIndex() );
		if( iter == gDisplayMode.end() )
		{
			mlog("º±≈√«— «ÿªÛµµ∞° ¡∏¿Á«œ¡ÅEæ æ∆º≠ «ÿªÛµµ ∫Ø∞Êø° Ω«∆–«œø¥Ω¿¥œ¥Ÿ..\n" );
			return false;
		}

		D3DDISPLAYMODE ddm = iter->second;

		mOldScreenWidth	= RGetScreenWidth();
		mOldScreenHeight	= RGetScreenHeight();
		mnOldBpp				= RGetPixelFormat();

		ModeParams.nWidth = ddm.Width;
		ModeParams.nHeight = ddm.Height;
		ModeParams.nScreenType = ZGetConfiguration()->GetVideo()->nScreenType;
		ModeParams.PixelFormat = ddm.Format;

		RResetDevice(&ModeParams);

		Mint::GetInstance()->SetWorkspaceSize(ModeParams.nWidth, ModeParams.nHeight);
		Mint::GetInstance()->GetMainFrame()->SetSize(ModeParams.nWidth, ModeParams.nHeight);
		Resize(ModeParams.nWidth, ModeParams.nHeight);
	}
	return true;
}

void ZOptionInterface::Update()
{
	if( mbTimer )
	{
		if( SetTimer(true) )
		{
			GetOldScreenResolution();
			ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
			MWidget* pWidget = pResource->FindWidget("ViewConfirm");
			if(pWidget!= 0) pWidget->Show( false );
		}
	}
}

void ZOptionInterface::OnActionKeySet(ZActionKey* pActionKey, ZVIRTUALKEY key)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	for(int i=0; i<ZACTION_COUNT; i++){
		char szItemName[256];
		sprintf(szItemName, "%sActionKey", ZGetConfiguration()->GetKeyboard()->ActionKeys[i].szName);
		ZActionKey* pWidget = (ZActionKey*)pResource->FindWidget(szItemName);
		if(pWidget==NULL) continue;
		if(pWidget==pActionKey) continue;

		if(pWidget->DeleteActionKey(key))
			pWidget->UpdateText();
	}
}








///////////////////// ¿Ã«œ interface listener
BEGIN_IMPLEMENT_LISTENER(ZGetOptionFrameButtonListener, MBTN_CLK_MSG)
	// ø…º« «¡∑π¿” ∫∏ø©¡÷±ÅE
	ZGetOptionInterface()->InitInterfaceOption();
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MWidget* pWidget = pResource->FindWidget("Option");
	pWidget->Show(true, true);

#ifdef LOCALE_NHNUSA
	pWidget = pResource->FindWidget( "Option_Textarea01");
	if ( pWidget)		pWidget->Show( false);
	MButton* pButton = (MButton*)pResource->FindWidget( "BoostOption");
	if ( pButton)
	{
		pButton->SetCheck( false);
		pButton->Show( false);
	}
#endif

#ifndef _MULTILANGUAGE	// ¥Ÿ¡ﬂææ˚›ˆøÅEµ∆ƒ¿Œ¿ª ≤∞¿ª∂© ææ˚ÿ±≈√ ƒﬁ∫∏π⁄Ω∫∏¶ º˚∞‹πˆ∏∞¥Ÿ
	pWidget = (MLabel*)pResource->FindWidget( "LanguageSelectLabel");
	if ( pWidget) {
		pWidget->Show( false);
	}
	MComboBox* pCBType = (MComboBox*)pResource->FindWidget( "LanguageSelectComboBox");
	if ( pCBType) {
		pCBType->Show( false);
	}
#endif

END_IMPLEMENT_LISTENER()


BEGIN_IMPLEMENT_LISTENER(ZGetSaveOptionButtonListener, MBTN_CLK_MSG)
	// Save & Close
	/*
	ZApplication::GetGameInterface()->SaveInterfaceOption();
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MWidget* pWidget = pResource->FindWidget("Option");
	if(pWidget!=NULL) pWidget->Show(false);
	//*/

	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	if( pWidget->m_bEventAcceleratorCall ) {

		MTabCtrl* pTab = (MTabCtrl*)pResource->FindWidget("OptionTabControl");

		if(pTab) {//≈∞∫∏µÅEø…º«¿∫ ≈∞¿‘∑¬¿ª ∏∑æ∆¡ÿ¥Ÿ.
			if(pTab->GetSelIndex()==3)//¿Ã∏ß¿ª √£æ∆ ¡∂ªÅE
				return true;
		}
	}

	if( ZGetOptionInterface()->IsDiffNetworkPort() )
	{
		static bool bRestartAsk = false;
		if (bRestartAsk == false) {
			ZGetOptionInterface()->ShowNetworkPortConfirmDialog();
			bRestartAsk = true;
			return true;
		}
	}

	//if (ZGetOptionInterface()->IsDiffWindow())
	//{
	//	static bool bRestartAsk = false;
	//	if (bRestartAsk == false) {
	//		ZGetOptionInterface()->ShowWindowConfirmDialog();
	//		bRestartAsk = true;
	//		return true;
	//	}
	//}


	if( ZGetOptionInterface()->IsDiffScreenResolution() )
	{
		ZGetOptionInterface()->ShowResizeConfirmDialog( false );
	}
	else
	{
		bool bLanguageChanged = false;
		MComboBox* pLanguageComboBox = (MComboBox*)pResource->FindWidget("LanguageSelectComboBox");
		if (pLanguageComboBox) {
			if (ZGetConfiguration()->GetSelectedLanguageIndex() != pLanguageComboBox->GetSelIndex())
				bLanguageChanged = true;
		}

		ZGetOptionInterface()->SaveInterfaceOption();
		ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
		MWidget* pWidget = pResource->FindWidget("Option");
		if(pWidget!=NULL) pWidget->Show(false);

		if(bLanguageChanged)	// ¿ÁΩ√¿€ »Æ¿Œ ∏ﬁºº¡ÅEπ⁄Ω∫
		{
			ZApplication::GetGameInterface()->ShowConfirmMessage(
				ZMsg(MSG_CONFIRM_RESTART_CHANGE_LANGUAGE), ZGetLanguageChangeConfirmListenter());
		}
	}

	if (ZApplication::GetGameInterface()->GetState() == GUNZ_GAME)
	{
		if (ZGetCombatInterface())
		{
			ZGetCombatInterface()->GetCrossHair()->ChangeFromOption();
		}
	}

END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetCancelOptionButtonListener, MBTN_CLK_MSG)
//	ZApplication::GetGameInterface()->SaveInterfaceOption();
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();

	// TODO: ¿Ã∞‘ « ø‰«—∞° ? ≈◊Ω∫∆Æ ø‰∏¡
	if( pWidget->m_bEventAcceleratorCall ) {

		MTabCtrl* pTab = (MTabCtrl*)pResource->FindWidget("OptionTabControl");

		if(pTab) {//≈∞∫∏µÅEø…º«¿∫ ≈∞¿‘∑¬¿ª ∏∑æ∆¡ÿ¥Ÿ.
			if(pTab->GetSelIndex()==3)//¿Ã∏ß¿ª √£æ∆ ¡∂ªÅE
				return true;
		}
	}

	MWidget* pWidget = pResource->FindWidget("Option");
	
	if(pWidget!=NULL) pWidget->Show(false);

	// ø¯∑° ∞®∏∂∞™¿∏∑Œ µπ∏Æ±ÅE
	MSlider* pSlider = (MSlider*)pResource->FindWidget("VideoGamma");
	if (pSlider != NULL) 
	{
		if (pSlider->GetValue() != Z_VIDEO_GAMMA_VALUE)
		{
			RSetGammaRamp(Z_VIDEO_GAMMA_VALUE);
		}
	}

	// Close Only
//	ZApplication::GetGameInterface()->SetState(GUNZ_PREVIOUS);
END_IMPLEMENT_LISTENER()


///////////////////////////////////////////////////
/// control

BEGIN_IMPLEMENT_LISTENER( ZGetLoadDefaultKeySettingListener, MBTN_CLK_MSG)
	ZGetConfiguration()->LoadDefaultKeySetting( );
	for(int i=0; i<ZACTION_COUNT; i++)
	{
		char szItemName[256];
		sprintf(szItemName, "%sActionKey", ZGetConfiguration()->GetKeyboard()->ActionKeys[i].szName);
		ZActionKey* pWidget = (ZActionKey*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget(szItemName);
		if(pWidget==NULL) continue;
		//unsigned int nKey = 0;
		//pWidget->GetActionKey(&nKey);
		//Mint::GetInstance()->UnregisterActionKey(i);
		//Mint::GetInstance()->RegisterActionKey(i, nKey);	// ≈∞ µ˚”œ
		//m_Keyboard.ActionKeys[i].nScanCode = nKey;	// ø…º« ¿˙¿ÅE
		pWidget->ClearActionKey();
		pWidget->SetActionKey(ZGetConfiguration()->GetKeyboard()->ActionKeys[i].nVirtualKey);
		pWidget->SetActionKey(ZGetConfiguration()->GetKeyboard()->ActionKeys[i].nVirtualKeyAlt);
	}
END_IMPLEMENT_LISTENER()


//////////////////////////////////////////////////
/// video

BEGIN_IMPLEMENT_LISTENER(ZGetOptionGammaSliderChangeListener, MLIST_VALUE_CHANGED)
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MSlider* pSlider = (MSlider*)pResource->FindWidget("VideoGamma");
	if (pSlider != NULL) 
	{
		unsigned short nGamma = (unsigned short)pSlider->GetValue();
		if (nGamma < 50) nGamma = 50;
		RSetGammaRamp(nGamma);
	}
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER( ZGetRequestResizeListener, MBTN_CLK_MSG )
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MWidget* pWidget = pResource->FindWidget("ResizeConfirm");
	if(pWidget!= 0) pWidget->Show( false );
	//«ÿªÛµµ ∫Ø∞Ê»ƒ
	ZGetOptionInterface()->TestScreenResolution();
	ZGetOptionInterface()->SetTimer( true, 10 );
	ZGetOptionInterface()->ShowResizeConfirmDialog( true );
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER( ZGetViewConfirmCancelListener, MBTN_CLK_MSG )
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MWidget* pWidget = pResource->FindWidget("ViewConfirm");
	if(pWidget!= 0) pWidget->Show( false );
	// «ÿªÛµµ ø¯∑°¥ÅE?∫Ø∞ÅE
 	ZGetOptionInterface()->SetTimer( false );
	ZGetOptionInterface()->GetOldScreenResolution();
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER( ZGetViewConfrimAcceptListener, MBTN_CLK_MSG )

	ZGetOptionInterface()->SetTimer( false );

	ZGetOptionInterface()->SaveInterfaceOption();

	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MWidget* pWidget = pResource->FindWidget("ViewConfirm");
	if(pWidget!= 0) pWidget->Show( false );
		
	pWidget = pResource->FindWidget("Option");
	if(pWidget!=NULL) pWidget->Show(false);
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetCancelResizeConfirmListener, MBTN_CLK_MSG)
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MWidget* pWidget = pResource->FindWidget("ResizeConfirm");
	if(pWidget!=NULL) pWidget->Show(false);
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER( ZSetOptimizationListener, MBTN_CLK_MSG )
	ZGetOptionInterface()->OptimizationVideoOption();
END_IMPLEMENT_LISTENER()

// Custom: Windowed mode widget
BEGIN_IMPLEMENT_LISTENER(ZGetWindowChangeRestartListener, MBTN_CLK_MSG)
ZGetOptionInterface()->SaveInterfaceOption();
ZChangeGameState(GUNZ_SHUTDOWN);
ZApplication* pApp = ZApplication::GetInstance();
pApp->Exit();
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER(ZGetWindowChangeCancelListener, MBTN_CLK_MSG)
ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
MWidget* pWidget = pResource->FindWidget("WindowConfirm");
if (pWidget != 0) pWidget->Show(false);
END_IMPLEMENT_LISTENER()

//////////////////////////////////////////////////
/// sound

BEGIN_IMPLEMENT_LISTENER( ZGet8BitSoundListener, MBTN_CLK_MSG )
	MButton* pWidget = (MButton*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("8BitSound");
	if(pWidget)
	{
		pWidget->SetCheck(true);
		pWidget = (MButton*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("16BitSound");
		if(pWidget) pWidget->SetCheck(false);
	}
END_IMPLEMENT_LISTENER()
BEGIN_IMPLEMENT_LISTENER( ZGet16BitSoundListener, MBTN_CLK_MSG )
	MButton* pWidget = (MButton*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("16BitSound");
	if(pWidget)
	{
		pWidget->SetCheck(true);
		pWidget = (MButton*)ZApplication::GetGameInterface()->GetIDLResource()->FindWidget("8BitSound");
		if(pWidget) pWidget->SetCheck(false);
	}
END_IMPLEMENT_LISTENER()


/////////////////////////////////////////////////
//// network
BEGIN_IMPLEMENT_LISTENER( ZGetNetworkPortChangeRestartListener, MBTN_CLK_MSG )
	ZGetOptionInterface()->SaveInterfaceOption();
	ZChangeGameState(GUNZ_SHUTDOWN);
	ZApplication* pApp = ZApplication::GetInstance();
	pApp->Exit();
END_IMPLEMENT_LISTENER()

BEGIN_IMPLEMENT_LISTENER( ZGetNetworkPortChangeCancelListener, MBTN_CLK_MSG )
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MWidget* pWidget = pResource->FindWidget("NetworkPortConfirm");
	if(pWidget!= 0) pWidget->Show(false);
END_IMPLEMENT_LISTENER()

/////////////////////////////////////////////////
//// mouse
static void SetMouseSensitivitySlider(int i)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MSlider* pSlider = (MSlider*)pResource->FindWidget("MouseSensitivitySlider");
	if (pSlider) 
	{
		pSlider->SetValue(i);
	}
}
static void SetMouseSensitivityEdit(int i)
{
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MEdit* pEdit = (MEdit*)pResource->FindWidget("MouseSensitivityEdit");
	if (pEdit)
	{
		char sz[1024] = "";
		sprintf(sz, "%d", i);
		pEdit->SetText(sz);
	}
}

BEGIN_IMPLEMENT_LISTENER( ZGetMouseSensitivitySliderListener, MLIST_VALUE_CHANGED)
	ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
	MSlider* pSlider = (MSlider*)pResource->FindWidget("MouseSensitivitySlider");
	if (pSlider != NULL) 
	{
		int i = ZGetConfiguration()->ValidateMouseSensitivityInInt( pSlider->GetValue());
		SetMouseSensitivityEdit(i);
	}
END_IMPLEMENT_LISTENER()

// ¿Ã ø°µ∆Æπ⁄Ω∫ø°º≠ ∏ﬁΩ√¡ÅEµŒ∞≥∏¶ √≥∏Æ«ÿæﬂ«œ±ÅE∂ßπÆø° BEGIN_IMPLEMENT_LISTENER/END_IMPLEMENT_LISTENER ∏≈≈©∑Œ∏¶ æ»ΩË¿Ω
MListener* ZGetMouseSensitivityEditListener(void){
	class ListenerClass : public MListener
	{
	public:
		virtual bool OnCommand(MWidget* pWidget, const char* szMessage){
			ZIDLResource* pResource = ZApplication::GetGameInterface()->GetIDLResource();
			MEdit* pEdit= (MEdit*)pResource->FindWidget("MouseSensitivityEdit");
			{
				int i = atoi( pEdit->GetText());
				int v = ZGetConfiguration()->ValidateMouseSensitivityInInt(i);

				if(MWidget::IsMsg(szMessage, MEDIT_CHAR_MSG)==true)
				{
					SetMouseSensitivitySlider(v);
					return true;
				}
				else if(MWidget::IsMsg(szMessage, MEDIT_KILL_FOCUS)==true)
				{
					SetMouseSensitivitySlider(v);
					SetMouseSensitivityEdit(v);
					return true;
				}
			}
			return false;
		}
	};
	static ListenerClass	Listener;
	return &Listener;
}
