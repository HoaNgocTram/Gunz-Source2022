#include "stdafx.h"
#include "RealSpace2.h"
#include "MDebug.h"
#include "RParticleSystem.h"
#include "RFrameWork.h"
#include "RBaseTexture.h"
//#include "d3dxerr.h"
#include "RMeshUtil.h"
#include "RFont.h"
#include "dxerr.h"
#ifdef _HSHIELD
#include "../../Gunz/HShield/HShield.h"
#endif

inline double RRound(double number)
{
    return number < 0.0 ? ceil(number - 0.5) : floor(number + 0.5);
}

bool g_bUsable = false;

namespace RAdvancedGraphics
{
	bool bDepthBuffering = false;
	int  nMultiSampling = 0;
};

_NAMESPACE_REALSPACE2_BEGIN

#pragma comment ( lib, "d3d9.lib" )

extern RFFUNCTION g_pFunctions[RF_ENDOFRFUNCTIONTYPE];

bool g_bHardwareTNL,g_bSupportVS,g_bAvailUserClipPlane;
int g_nScreenWidth,g_nScreenHeight,g_nPicmip,g_nScreenType;
RPIXELFORMAT		g_PixelFormat;
LPDIRECT3D9EX			g_pD3D = NULL;
LPDIRECT3DDEVICE9EX		g_pd3dDevice = NULL;
D3DADAPTER_IDENTIFIER9	g_DeviceID;
D3DPRESENT_PARAMETERS	g_d3dpp; 
D3DCAPS9				g_d3dcaps;
HWND					g_hWnd;
MZFileSystem			*g_pFileSystem=NULL;
RParticleSystem			g_ParticleSystem;
HMODULE					g_hD3DLibrary=NULL;
bool g_TakingScreenie = false;
bool g_bStencilBuffer;

// FIXED RESOLUTION ��E� ...
//LPDIRECT3DTEXTURE9 g_lpTexture = 0;
//LPDIRECT3DSURFACE9 g_lpSurface = 0;
//LPDIRECT3DSURFACE9 g_lpStencil = 0;
//LPDIRECT3DSURFACE9 g_lpSurfaceBackup = 0;
//LPDIRECT3DSURFACE9 g_lpStencilBackup = 0;
//bool			g_bFixedResolution = false;

//Fog
float g_fFogNear;
float g_fFogFar;
DWORD g_dwFogColor;
bool g_bFog = false;

int	g_nVidioMemory = 0;
double currentTime = timeGetTime();
int g_nFrameCount=0,g_nLastFrameCount=0;
float g_fFPS=0;
int g_nFrameLimitValue=0;
DWORD g_dwLastTime=timeGetTime();
DWORD g_dwLastFPSTime=timeGetTime();

bool g_isDirect3D9ExEnabled = false;

int g_nTrackSize = 0;
bool		g_bTrilinear=true;
const bool	bTripleBuffer=false;
bool		g_bQuery = false;

D3DMULTISAMPLE_TYPE	g_MultiSample = D3DMULTISAMPLE_NONE;
void RSetQuery(bool b)		{ g_bQuery = b; }
bool RIsQuery()				{ return g_bQuery; }
int  RGetScreenType()		{ return g_nScreenType; }
bool RIsHardwareTNL()		{ return g_bHardwareTNL; }
bool RIsSupportVS()			{ return g_bSupportVS;}
bool RIsTrilinear()			{ return g_bTrilinear; }
int	 RGetApproxVMem()		{ if(g_pd3dDevice==0) return 0; return g_pd3dDevice->GetAvailableTextureMem()*0.5f;}
bool RIsAvailUserClipPlane() { return g_bAvailUserClipPlane; }
int  RGetScreenWidth()		{ return g_nScreenWidth; }
int  RGetScreenHeight()		{ return g_nScreenHeight; }
int RGetIsHDScreen() { return (float(g_nScreenHeight) / float(g_nScreenWidth) == 0.5625f) ? 1 : 0; }

// Custom: RGetAspectRatio for 4:3 (0.75), 16:9 (0.5625), 16:10 (0.625) support
//float RGetAspectRatio()		{ return float(g_nScreenHeight)/float(g_nScreenWidth); }

// Custom: MAIET added a new function (RGetIsLongScreen) - I CREATED THE NAME KK
int RGetIsLongScreen()		{ return (float(g_nScreenHeight)/float(g_nScreenWidth) == 0.5625f) ? 1 : 0; }

int RGetIsWidthScreen()		{ return (float(g_nScreenHeight)/float(g_nScreenWidth) == 0.625f) ? 1 : 0; }
float RGetWidthScreen()		{ return float(g_nScreenHeight)/float(g_nScreenWidth); }

int RGetPicmip()			{ return g_nPicmip; }
RPIXELFORMAT RGetPixelFormat() { return g_PixelFormat; }
LPDIRECT3DDEVICE9	RGetDevice() { return g_pd3dDevice; }
bool RIsStencilBuffer()		{ return g_bStencilBuffer; }
bool CheckVideoAdapterSupported();

unsigned long g_rsnRenderFlags = RRENDER_CLEAR_BACKBUFFER;
void RSetRenderFlags(unsigned long nFlags)
{
	g_rsnRenderFlags = nFlags;
}
unsigned long RGetRenderFlags() { return g_rsnRenderFlags; }

int RGetVidioMemory() {
	return g_nVidioMemory;
}

HRESULT RError(int nErrCode)
{
	RSetError(nErrCode);
	if(g_pFunctions[RF_ERROR])
		return g_pFunctions[RF_ERROR](NULL);

	return R_OK;
}

bool CreateDirect3D9()
{
	if (!g_pD3D)
	{
		if (g_isDirect3D9ExEnabled)
		{
			HRESULT result = Direct3DCreate9Ex(D3D_SDK_VERSION, &g_pD3D);
		}
		else
		{
			(LPDIRECT3D9&)g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
		}
	}

	g_pD3D->GetAdapterIdentifier(0, 0, &g_DeviceID);

	return true;
}

D3DADAPTER_IDENTIFIER9*	RGetAdapterID() 
{
	if (!g_pD3D) _ASSERT(0);
	return &g_DeviceID; 
}

BOOL IsCompressedTextureFormatOk( D3DFORMAT TextureFormat ) 
{
	HRESULT hr = g_pD3D->CheckDeviceFormat( D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		g_PixelFormat ,
		0,
		D3DRTYPE_TEXTURE,
		TextureFormat);

	return SUCCEEDED( hr );
}

void RSetFileSystem(MZFileSystem *pFileSystem) { g_pFileSystem=pFileSystem; }

RParticleSystem *RGetParticleSystem() { return &g_ParticleSystem; }

// �ʱ�ȭ & ����E

/*
#include <ddraw.h>

//m_MaxAllocMem = m_pDevice->GetAvailableTextureMem();

BOOL GetVidMemory(DWORD& dwTotal,DWORD& dwFree) {

	LPDIRECTDRAW7 lpDD = NULL;
	DDSCAPS2      ddsCaps2; 
	HRESULT       hr; 

	hr = DirectDrawCreateEx(NULL, (VOID**)&lpDD, IID_IDirectDraw7, NULL );

	if (FAILED(hr))
		return FALSE; 

	ZeroMemory(&ddsCaps2, sizeof(ddsCaps2));

	ddsCaps2.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM; 

	hr = lpDD->GetAvailableVidMem(&ddsCaps2, &dwTotal, &dwFree); 

	if (FAILED(hr))
		return FALSE;

	if(lpDD)
		lpDD->Release();

	return TRUE;
}
*/
static void InitDevice()
{
	g_pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE  , D3DCULL_NONE );
/*
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER , D3DTEXF_LINEAR);
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER , D3DTEXF_LINEAR);
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER , g_bTrilinear ? D3DTEXF_LINEAR : D3DTEXF_NONE );
	g_pd3dDevice->SetTextureStageState( 1, D3DTSS_MAGFILTER , D3DTEXF_LINEAR);
	g_pd3dDevice->SetTextureStageState( 1, D3DTSS_MINFILTER , D3DTEXF_LINEAR);
	g_pd3dDevice->SetTextureStageState( 1, D3DTSS_MIPFILTER , g_bTrilinear ? D3DTEXF_LINEAR : D3DTEXF_NONE );
*/
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER , g_bTrilinear ? D3DTEXF_LINEAR : D3DTEXF_NONE );

	g_pd3dDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetSamplerState( 1, D3DSAMP_MIPFILTER , g_bTrilinear ? D3DTEXF_LINEAR : D3DTEXF_NONE );

//	g_pd3dDevice->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x00000000);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_NOTEQUAL );

	RSetWBuffer(true);

//	g_pd3dDevice->SetRenderState(D3DRS_ZFUNC ,D3DCMP_LESSEQUAL);

	float fMaxBias = -1.0f;
//	g_pd3dDevice->SetTextureStageState( 0 ,D3DTSS_MIPMAPLODBIAS , *(unsigned long*)&fMaxBias);
//	g_pd3dDevice->SetTextureStageState( 1 ,D3DTSS_MIPMAPLODBIAS , *(unsigned long*)&fMaxBias);
	g_pd3dDevice->SetSamplerState( 0 ,D3DSAMP_MIPMAPLODBIAS , *(unsigned long*)&fMaxBias);
	g_pd3dDevice->SetSamplerState( 1 ,D3DSAMP_MIPMAPLODBIAS , *(unsigned long*)&fMaxBias);
/*
	if(g_MultiSample)
    	g_pd3dDevice->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS , TRUE );
	else 
		g_pd3dDevice->SetRenderState( D3DRS_MULTISAMPLEANTIALIAS , FALSE );
*/
	g_nVidioMemory = g_pd3dDevice->GetAvailableTextureMem()/2;

	mlog("Video memory %f \n",g_nVidioMemory / float(1024*1024) );

	if( D3DERR_NOTAVAILABLE == g_pd3dDevice->CreateQuery( D3DQUERYTYPE_OCCLUSION, NULL ) )
		g_bQuery = false;
	else
		g_bQuery = true;

	RBeginScene();

/*
	DWORD v1,v2;

	if(GetVidMemory(v1,v2)) {
		int k=0;
	}
*/
}

int getTaskBarHeight()
{
	RECT rect;
	HWND taskBar = FindWindow("Shell_traywnd", NULL);
	if (taskBar && GetWindowRect(taskBar, &rect)) {
		return rect.bottom - rect.top;
	}
	return NULL;
}

void CheckMipFilter()
{
	LPDIRECT3DDEVICE9 pd3dDevice=RGetDevice();

	pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	pd3dDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pd3dDevice->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

	DWORD dwNumPasses;
	HRESULT hr=pd3dDevice->ValidateDevice( &dwNumPasses );
	if(hr==D3DERR_UNSUPPORTEDTEXTUREFILTER ) {
		g_bTrilinear = false;
	}
}

D3DDISPLAYMODE g_d3ddm;

bool RInitDisplay(HWND hWnd, const RMODEPARAMS *params)
{
	if (CreateDirect3D9() == false)
	{
		RError(RERROR_CANNOT_CREATE_D3D);
		return false;
	}

#ifdef _HSHIELD
	char szSysDir[MAX_PATH] = { 0, };
	GetSystemDirectory(szSysDir, MAX_PATH);

	int nRet = _AhnHS_CheckAPIHooked("d3d9.dll", "Direct3DCreate9", szSysDir);
	if (nRet == HS_ERR_API_IS_HOOKED)
	{
		mlog("d3d9.dll Hooking detected. (Error code = %x)\n", nRet);
		// Direct3D ����� ��ŷ �Ǵ� ���ۿ� ���� �翁Eǰ�E�����Ƿ� ������ �����ϸ�E�˴ϴ�. 
		return false;
	}
#endif 

	if (CheckVideoAdapterSupported()==false)
	{
		if (RError(RERROR_INVALID_DEVICE) != R_OK) return false;
	}

	D3DCAPS9 d3dcaps;
	g_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT , D3DDEVTYPE_HAL , &d3dcaps );
	g_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT , D3DDEVTYPE_HAL , &g_d3dcaps );
	
	g_bHardwareTNL = (d3dcaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ) != 0;

//	g_bTrilinear = (d3dcaps)
//	g_bHardwareTNL = false;

	g_bAvailUserClipPlane = (d3dcaps.MaxUserClipPlanes > 0 )? true : false;
	if(d3dcaps.RasterCaps & D3DPRASTERCAPS_WFOG) mlog("WFog Enabled Device.\n");

	g_bSupportVS = ( d3dcaps.VertexShaderVersion >= D3DVS_VERSION(1, 1));
	
	if(!g_bSupportVS) mlog("Vertex Shader isn't supported\n");
	else
	{
		if( d3dcaps.MaxVertexShaderConst < 60 )
		{
			mlog("Too small Constant Number to use Hardware Skinning so Disable Vertex Shader\n");
			g_bSupportVS = false;
		}
	}

	// get screen information
	HRESULT hr;
	hr=g_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &g_d3ddm );
	
	switch (params->nScreenType)
	{
	case 0:
		g_nScreenType = 0;
		break;
	case 1:
		g_nScreenType = 1;
		break;
	case 2:
		g_nScreenType = 2;
		break;
	default:
		break;
	}

	if (params->nScreenType != 2)
	{
		g_nScreenWidth = params->nWidth;
		g_nScreenHeight = params->nHeight;
	}
	else
	{
		g_nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		g_nScreenHeight = GetSystemMetrics(SM_CYSCREEN) - getTaskBarHeight() - GetSystemMetrics(SM_CYBORDER);
	}
	g_PixelFormat=(params->nScreenType == 0 ? params->PixelFormat	 : g_d3ddm.Format);
	
	ZeroMemory( &g_d3dpp, sizeof(g_d3dpp) );
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	if (g_nScreenType != 2)
	{
		g_d3dpp.BackBufferWidth = params->nWidth;
		g_d3dpp.BackBufferHeight = params->nHeight;
	}
	else
	{
		g_d3dpp.BackBufferWidth = params->nWidth;
		g_d3dpp.BackBufferHeight = GetSystemMetrics(SM_CYSCREEN) - getTaskBarHeight() - GetSystemMetrics(SM_CYBORDER);
	}
	g_d3dpp.BackBufferCount = bTripleBuffer ? 2 : 1;						// or 2 ??
	if (params->nScreenType >= 1)
		g_d3dpp.Windowed = true;
	else
		g_d3dpp.Windowed = false;
	g_d3dpp.BackBufferFormat = g_PixelFormat;
	g_d3dpp.EnableAutoDepthStencil = TRUE;

	switch (RAdvancedGraphics::nMultiSampling)
	{
	case 0:
		g_MultiSample = D3DMULTISAMPLE_NONE; break;
	case 1:
		if (SUCCEEDED(g_pD3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_PixelFormat, params->nScreenType > 0 ? true : false, D3DMULTISAMPLE_2_SAMPLES, NULL))) {
			g_MultiSample = D3DMULTISAMPLE_2_SAMPLES;
		}
		else{
			g_MultiSample = D3DMULTISAMPLE_NONE;
		}break;
	case 2:
		if (SUCCEEDED(g_pD3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_PixelFormat, params->nScreenType > 0 ? true : false, D3DMULTISAMPLE_4_SAMPLES, NULL))) {
			g_MultiSample = D3DMULTISAMPLE_4_SAMPLES;
		}
		else{
			g_MultiSample = D3DMULTISAMPLE_NONE;
		}break;
	case 3:
		if (SUCCEEDED(g_pD3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_PixelFormat, params->nScreenType > 0 ? true : false, D3DMULTISAMPLE_8_SAMPLES, NULL))){
			g_MultiSample = D3DMULTISAMPLE_8_SAMPLES;
		}
		else{
			g_MultiSample = D3DMULTISAMPLE_NONE;
		}break;
	default:
		g_MultiSample = D3DMULTISAMPLE_NONE;break;
	}


	g_d3dpp.MultiSampleType =  g_MultiSample;

	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	
	g_d3dpp.Flags=NULL;//D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	// Custom: Disabled vsync?
	g_d3dpp.PresentationInterval=
		D3DPRESENT_INTERVAL_IMMEDIATE; //params->bFullScreen ? D3DPRESENT_INTERVAL_IMMEDIATE : D3DPRESENT_INTERVAL_DEFAULT;

	DWORD BehaviorFlags=D3DCREATE_FPU_PRESERVE | 
		(g_bHardwareTNL ? D3DCREATE_HARDWARE_VERTEXPROCESSING : D3DCREATE_SOFTWARE_VERTEXPROCESSING);
	//BehaviorFlags=D3DCREATE_FPU_PRESERVE | D3DCREATE_SOFTWARE_VERTEXPROCESSING;

#ifdef _MT
//	mlog("multithread.\n");

	// �� �÷��״� device �� ���� �����忡�� ���� �翁EѴ�. �����ս� ���ϰ� �ִ�.
//	BehaviorFlags|=D3DCREATE_MULTITHREADED;
#endif


#ifdef _DEBUG
#	define _NVPERFHUD
#endif

#ifndef _NVPERFHUD
	if (g_isDirect3D9ExEnabled) {
		D3DDISPLAYMODEEX displayMode;
		displayMode.Size = sizeof(D3DDISPLAYMODEEX);
		displayMode.Width = RGetScreenWidth();
		displayMode.Height = RGetScreenHeight();
		displayMode.Format = g_d3ddm.Format;
		displayMode.RefreshRate = 0;
		displayMode.ScanLineOrdering = D3DSCANLINEORDERING_PROGRESSIVE;
		HRESULT result = g_pD3D->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, BehaviorFlags, &g_d3dpp, RGetScreenType() != 0 ? &displayMode : nullptr, &g_pd3dDevice);
		if (FAILED(result)) {
			SAFE_RELEASE(g_pD3D);
			mlog("can't create device\n");
			return false;
		}
	}
	else {
		HRESULT result = (LPDIRECT3D9(g_pD3D)->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, BehaviorFlags, &g_d3dpp, (IDirect3DDevice9**)&g_pd3dDevice));
		if (FAILED(result)) {
			SAFE_RELEASE(g_pD3D);
			mlog("can't create device\n");
			return false;
		}
	}
#else
	//UINT AdapterToUse = D3DADAPTER_DEFAULT;
	//D3DDEVTYPE DeviceType = D3DDEVTYPE_HAL;

	//for( UINT Adapter = 0; Adapter < g_pD3D->GetAdapterCount(); Adapter++ ) 
	//{    
	//	D3DADAPTER_IDENTIFIER9 Identifier;    
	//	HRESULT Res = g_pD3D->GetAdapterIdentifier( Adapter, 0, &Identifier );    
	//	if( strstr( Identifier.Description, "PerfHUD" ) != 0 )    
	//	{        
	//		AdapterToUse = Adapter;        
	//		DeviceType = D3DDEVTYPE_REF;        
	//		break;    
	//	}
	//}
	//if( FAILED( g_pD3D->CreateDevice( AdapterToUse, DeviceType, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice ) ) )
	//{    
	//	mlog("can't create device\n");
	//	return false;
	//}

	if (g_isDirect3D9ExEnabled) {
		D3DDISPLAYMODEEX displayMode;
		displayMode.Size = sizeof(D3DDISPLAYMODEEX);
		displayMode.Width = RGetScreenWidth();
		displayMode.Height = RGetScreenHeight();
		displayMode.Format = g_d3ddm.Format;
		displayMode.RefreshRate = 0;
		displayMode.ScanLineOrdering = D3DSCANLINEORDERING_PROGRESSIVE;
		HRESULT result = g_pD3D->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, BehaviorFlags, &g_d3dpp, RGetScreenType() != 0 ? &displayMode : nullptr, &g_pd3dDevice);
		if (FAILED(result)) {
			SAFE_RELEASE(g_pD3D);
			mlog("can't create device\n");
			return false;
		}
	}
	else {
		HRESULT result = (LPDIRECT3D9(g_pD3D)->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, BehaviorFlags, &g_d3dpp, (IDirect3DDevice9**)&g_pd3dDevice));
		if (FAILED(result)) {
			SAFE_RELEASE(g_pD3D);
			mlog("can't create device\n");
			return false;
		}
	}

#endif	// #ifndef _NVPERFHUD
	mlog("device created.\n");

	g_bUsable = true;

	/*
	if( FAILED( g_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &g_d3ddm ) ) )
		return false;

	hr=g_pd3dDevice->GetDisplayMode( &g_d3ddm );
	g_PixelFormat=g_d3ddm.Format;
	*/

	g_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, FALSE);
	RSetViewport(0,0,g_nScreenWidth,g_nScreenHeight);
	g_pd3dDevice->Clear(0,NULL,D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER ,0x000000,1.0f ,0);

	CheckMipFilter();
	InitDevice();
	RFrame_Restore();
	RBaseTexture_Create();
	if(!RFontCreate())
	{
		RCloseDisplay();
		mlog("can't create font\n");
		return false;
	}

	g_hWnd=hWnd;

	return true;
}

bool RCloseDisplay()
{
	RFontDestroy();
	RBaseTexture_Destory();
	RFrame_Invalidate();
	RBaseTexture_Invalidate();

	if(g_pd3dDevice)
	{
		g_pd3dDevice->EndScene();
		SAFE_RELEASE(g_pd3dDevice);
	}

	SAFE_RELEASE(g_pD3D);
	FreeLibrary(g_hD3DLibrary);
	g_hD3DLibrary = NULL;

	return true;
}

void RAdjustWindow(const RMODEPARAMS *pModeParams)
{
	if((GetWindowLong( g_hWnd, GWL_STYLE) & WS_CHILD) !=0)
		return;

	if (pModeParams->nScreenType == 0)
		SetWindowLong(g_hWnd, GWL_STYLE, WS_POPUP | WS_SYSMENU);
	else if (pModeParams->nScreenType == 1)
		SetWindowLong(g_hWnd, GWL_STYLE, WS_POPUP | WS_CAPTION | WS_SYSMENU);
	else
		SetWindowLong(g_hWnd, GWL_STYLE, WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX);

	RECT rt = { 0,0,pModeParams->nWidth,pModeParams->nHeight };
	if (pModeParams->nScreenType >= 1)
	{
		AdjustWindowRect(&rt, GetWindowLong(g_hWnd,GWL_STYLE), FALSE);
		SetWindowPos(g_hWnd, HWND_NOTOPMOST, 0, 0, rt.right - rt.left, rt.bottom - rt.top,SWP_SHOWWINDOW);
	}

	// Custom: Set the window icon (icon always glitches when alt-tab after loading mesh)
	char szFileName[_MAX_DIR];
	GetModuleFileName(NULL, szFileName, _MAX_DIR);
	SendMessage( g_hWnd, WM_SETICON, ICON_BIG, (LPARAM)ExtractIcon(0, szFileName, 0) );
}

// �����E� & �ø��ΰ�E�

//void ResetFont();

void RResetDevice(const RMODEPARAMS *params)
{
	mlog("Reset Device \n");

	RFrame_Invalidate();
	RBaseTexture_Invalidate();

	switch (params->nScreenType)
	{
	case 0:
		g_nScreenType = 0;
		break;
	case 1:
		g_nScreenType = 1;
		break;
	case 2:
		g_nScreenType = 2;
		break;
	default:
		break;
	}
	if (params->nScreenType != 2)
	{
		g_nScreenWidth = params->nWidth;
		g_nScreenHeight = params->nHeight;
	}
	else
	{
		g_nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
		g_nScreenHeight = GetSystemMetrics(SM_CYSCREEN) - getTaskBarHeight();
	}
	g_PixelFormat = (g_nScreenType == 0 ? params->PixelFormat : g_d3ddm.Format);	// ������E���� ���� ��������

	if (params->nScreenType >= 1)
		g_d3dpp.Windowed = true;
	else
		g_d3dpp.Windowed = false;
	if (g_nScreenType != 2)
	{
		g_d3dpp.BackBufferWidth = g_nScreenWidth;
		g_d3dpp.BackBufferHeight = g_nScreenHeight;
	}
	else
	{
		g_d3dpp.BackBufferWidth = g_nScreenWidth;
		g_d3dpp.BackBufferHeight = GetSystemMetrics(SM_CYSCREEN) - getTaskBarHeight() - GetSystemMetrics(SM_CYBORDER);
	}
	g_d3dpp.BackBufferFormat = g_PixelFormat;

	switch (RAdvancedGraphics::nMultiSampling)
	{
	case 0:
		g_MultiSample = D3DMULTISAMPLE_NONE; break;
	case 1:
		if (SUCCEEDED(g_pD3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_PixelFormat, g_nScreenType > 0 ? true : false, D3DMULTISAMPLE_2_SAMPLES, NULL))) {
			g_MultiSample = D3DMULTISAMPLE_2_SAMPLES;
		}
		else{
			g_MultiSample = D3DMULTISAMPLE_NONE;
		}break;
	case 2:
		if (SUCCEEDED(g_pD3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_PixelFormat, g_nScreenType > 0 ? true : false, D3DMULTISAMPLE_4_SAMPLES, NULL))) {
			g_MultiSample = D3DMULTISAMPLE_4_SAMPLES;
		}
		else{
			g_MultiSample = D3DMULTISAMPLE_NONE;
		}break;
	case 3:
		if (SUCCEEDED(g_pD3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_PixelFormat, g_nScreenType > 0 ? true : false, D3DMULTISAMPLE_8_SAMPLES, NULL))){
			g_MultiSample = D3DMULTISAMPLE_8_SAMPLES;
		}
		else{
			g_MultiSample = D3DMULTISAMPLE_NONE;
		}break;
	default:
		g_MultiSample = D3DMULTISAMPLE_NONE; break;
	}


	if (RAdvancedGraphics::bDepthBuffering)
	{
		g_bStencilBuffer = true;
		if (FAILED(g_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24S8)) ||
			FAILED(g_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_R5G6B5, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24S8)))
		{
			g_bStencilBuffer = false;
		}
		else if (FAILED(g_pD3D->CheckDepthStencilMatch(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_PixelFormat, g_PixelFormat, D3DFMT_D24S8)))
		{
			g_bStencilBuffer = false;
		}
		if (g_bStencilBuffer)
		{
			mlog("24bit depth buffering enabled\n");
			g_d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
		}
		else
			g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	}
	else
	{
		g_bStencilBuffer = false;
		g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	}
	g_d3dpp.MultiSampleType =  g_MultiSample;

	HRESULT hr=g_pd3dDevice->Reset(&g_d3dpp);
	
	if (g_isDirect3D9ExEnabled) {
		D3DDISPLAYMODEEX displayMode;
		displayMode.Size = sizeof(D3DDISPLAYMODEEX);
		displayMode.Width = RGetScreenWidth();
		displayMode.Height = RGetScreenHeight();
		displayMode.Format = g_d3ddm.Format;
		displayMode.RefreshRate = 0;
		displayMode.ScanLineOrdering = D3DSCANLINEORDERING_PROGRESSIVE;
		hr = g_pd3dDevice->ResetEx(&g_d3dpp, RGetScreenType() != 0 ? &displayMode : nullptr);
	}
	else {
		hr = (LPDIRECT3DDEVICE9(g_pd3dDevice)->Reset(&g_d3dpp));
	}
	
	_ASSERT(hr==D3D_OK);
	if( hr != D3D_OK ) {
		int *a=0;
		*a = 1;	// CRASH #1
	}

	InitDevice();

	RAdjustWindow(params);

	if (params->nScreenType != 2)
		ShowWindow(g_hWnd, SW_SHOW);
	else
		ShowWindow(g_hWnd, SW_SHOWMAXIMIZED);
	UpdateWindow(g_hWnd);

	RSetViewport(0,0,g_nScreenWidth,g_nScreenHeight);

	RUpdateCamera();

	RResetTransform();

	RBaseTexture_Restore();
	RFrame_Restore();
}

RRESULT RIsReadyToRender()
{
	if (!g_pd3dDevice) return R_NOTREADY;
	HRESULT hr;

	if (g_isDirect3D9ExEnabled)
	{
		hr = g_pd3dDevice->CheckDeviceState(g_hWnd);
		if (hr == S_PRESENT_MODE_CHANGED)
		{
			D3DDISPLAYMODEEX displayMode;
			displayMode.Size = sizeof(D3DDISPLAYMODEEX);
			displayMode.Width = RGetScreenWidth();
			displayMode.Height = RGetScreenHeight();
			displayMode.Format = g_d3ddm.Format;
			displayMode.RefreshRate = 0;
			displayMode.ScanLineOrdering = D3DSCANLINEORDERING_PROGRESSIVE;
			hr = g_pd3dDevice->ResetEx(&g_d3dpp, RGetScreenType() != 0 ? &displayMode : nullptr);
			return R_NOTREADY;
		}

		if (hr == D3DERR_DEVICEHUNG)
		{
			//TODO: restore textures
			RFrame_Invalidate();
			RBaseTexture_Invalidate();
			D3DDISPLAYMODEEX displayMode;
			displayMode.Size = sizeof(D3DDISPLAYMODEEX);
			displayMode.Width = RGetScreenWidth();
			displayMode.Height = RGetScreenHeight();
			displayMode.Format = g_d3ddm.Format;
			displayMode.RefreshRate = 0;
			displayMode.ScanLineOrdering = D3DSCANLINEORDERING_PROGRESSIVE;
			hr = g_pd3dDevice->ResetEx(&g_d3dpp, RGetScreenType() != 0 ? &displayMode : nullptr);
			RBaseTexture_Restore();
			RFrame_Restore();
			return R_NOTREADY;
		}
		return R_OK;
	}

	else
	{
		if (FAILED(hr = g_pd3dDevice->TestCooperativeLevel()))
		{
			// If the device was lost, do not render until we get it back
			if (D3DERR_DEVICELOST == hr)
				return R_NOTREADY;

			// Check if the device needs to be resized.
			if (D3DERR_DEVICENOTRESET == hr)
				return R_RESTORED;
		}
	}
	return R_OK;
}

#define FPS_INTERVAL	1000

static DWORD g_clear_color = 0x00000000;

void SetClearColor(DWORD c) {
	g_clear_color = c;
}

//static bool g_rsbRenderStarted = false;


bool REndScene()
{
//	if (g_rsbRenderStarted == false) return false;
	
	g_pd3dDevice->EndScene();
//	g_rsbRenderStarted = false;
	
	return true;
}

bool RBeginScene()
{
//	if (g_rsbRenderStarted == true) return false;
	
	g_pd3dDevice->BeginScene();
//	g_rsbRenderStarted = true;
	
	return true;
}
// Custom: Corrección de FPS para plataformas Windows 8 y 10.
bool WinFPSFix() {
	DWORD Version = GetVersion();
	DWORD Major = (DWORD)(LOBYTE(LOWORD(Version)));
	DWORD Minor = (DWORD)(HIBYTE(LOWORD(Version)));

	return (Major > 6) || ((Minor == 6) && (Minor >= 2));
}
bool RFlip()
{
	//	if (g_rsbRenderStarted == true) 
	REndScene();
	if (g_isDirect3D9ExEnabled)
	{
		HRESULT hr = g_pd3dDevice->PresentEx(NULL, NULL, NULL, NULL, D3DPRESENT_INTERVAL_IMMEDIATE);
		if (hr != D3D_OK)
		{
			return false;
		}
	}
	else
	{
		HRESULT hr = ((LPDIRECT3DDEVICE9)g_pd3dDevice)->Present(NULL, NULL, NULL, NULL);
	}

	// Custom: Replaced && to &, it's a bitwise flag. Stupid MAIET.
	// k, did not touch it now
	if (g_rsnRenderFlags & RRENDER_CLEAR_BACKBUFFER)
	{
		if (g_bStencilBuffer)
			g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, g_clear_color, 1.0f, 0L);
		else
			g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, g_clear_color, 1.0f, 0L);
	}
	else
		g_pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, g_clear_color, 1.0f, 0);

	RBeginScene();

	{
		g_nFrameCount++;

		double fFrameLimit = 0;
		if (g_nFrameLimitValue > 0)
		{
			fFrameLimit = 1000.0 / g_nFrameLimitValue;
		}

		double newTime = timeGetTime();
		double frameTime = newTime - currentTime;
		double waitTime = fFrameLimit - frameTime;

		if (frameTime > 100.0)
		{
			//due to some machines disliking the timegettime being added whit a float rather than a double, i made it a double
			currentTime = timeGetTime() + fFrameLimit;
		}

		while (waitTime > 0.0)
		{
			double deltaTime = min(waitTime, fFrameLimit);
			Sleep(deltaTime);
			waitTime -= deltaTime;
		}
		currentTime += fFrameLimit;


		double currentTimeFPS = timeGetTime();
		if (g_dwLastFPSTime + FPS_INTERVAL < currentTimeFPS)
		{
			g_fFPS = (g_nFrameCount - g_nLastFrameCount) * (float)FPS_INTERVAL / ((float)(currentTimeFPS - g_dwLastFPSTime) * (FPS_INTERVAL / 1000.f));
			g_dwLastFPSTime = currentTimeFPS;
			g_nLastFrameCount = g_nFrameCount;
		}
	}

	return true;

}

void RDrawLine(rvector &v1,rvector &v2,DWORD dwColor)
{
/*
	RGetDevice()->SetTexture(0,NULL);
	RGetDevice()->SetRenderState( D3DRS_LIGHTING, FALSE );
	RGetDevice()->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE );
*/
	struct LVERTEX {
		float x, y, z;		// world position
		DWORD color;
	} ;

	LVERTEX ver[2]={{v1.x,v1.y,v1.z,dwColor},{v2.x,v2.y,v2.z,dwColor}};

	HRESULT hr=RGetDevice()->DrawPrimitiveUP(D3DPT_LINELIST,1,ver,sizeof(LVERTEX));
}

rvector RGetTransformCoord(rvector &coord)
{
	rvector ret;
	D3DXVec3TransformCoord(&ret,&coord,&RViewProjectionViewport);
	return ret;
}

// �޸𸮸� ���� �Ҵ��ϹǷ� �� ����E��������Ѵ�.
bool SaveMemoryBmp(int x,int y,void *data,void **retmemory,int *nsize)
{
	unsigned char *memory=NULL,*dest=NULL;

	if(!data) return false;

	int nBytesPerLine=(x*3+3)/4*4;		// 4 byte align
	int nMemoryNeed=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+nBytesPerLine*y;
	memory=new unsigned char[nMemoryNeed];
	if(!memory) return false;

	*nsize=nMemoryNeed;
	*retmemory=memory;


	dest=memory;
	BITMAPFILEHEADER *pbmfh=(BITMAPFILEHEADER*)dest;
	dest+=sizeof(BITMAPFILEHEADER);
	BITMAPINFOHEADER *pbmih=(BITMAPINFOHEADER*)dest;
	dest+=sizeof(BITMAPINFOHEADER);

	// SET FILE HEADER : 14 byte
	pbmfh->bfType = 0x4D42;
	pbmfh->bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + x*y*3;
	pbmfh->bfReserved1 = 0;
	pbmfh->bfReserved2 = 0;
	pbmfh->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	// SET INFO HEADER : 40 byte
	ZeroMemory( pbmih, sizeof(BITMAPINFOHEADER) );
	pbmih->biSize = sizeof(BITMAPINFOHEADER);
	pbmih->biWidth = x;
	pbmih->biHeight = y;
	pbmih->biPlanes = 1;
	pbmih->biBitCount = 24;
	pbmih->biCompression = BI_RGB;
	pbmih->biSizeImage = 0;
	pbmih->biXPelsPerMeter = 0;
	pbmih->biYPelsPerMeter = 0;
	pbmih->biClrUsed = 0;
	pbmih->biClrImportant = 0;

	int i,j;

	//	unsigned char *p=(unsigned char*)data+(y-1)*(x*3);
	DWORD *p=(DWORD*)data+(y-1)*x;

	for(i=y-1;i>=0;i--)
	{
		for(j=0;j<x;j++)
		{
			*dest++=*((BYTE*)p+j*4+0);
			*dest++=*((BYTE*)p+j*4+1);
			*dest++=*((BYTE*)p+j*4+2);
		}
		if( x*3 % 4 != 0)
		{
			unsigned char zero[]={0,0,0,0};
			memcpy(dest,zero,4 - x*3 % 4);
			dest+=4 - x*3 % 4;
		}
		p-=x;
	}

	return true;
}


#ifdef _USE_GDIPLUS
	#include "unknwn.h"
	#include "gdiplus.h"
	using namespace Gdiplus;

	int GetCodecClsid(const WCHAR* format, CLSID* pClsid)
	{
		UINT  num = 0;          // number of image encoders
		UINT  size = 0;         // size of the image encoder array in bytes

		ImageCodecInfo* pImageCodecInfo = NULL;

		GetImageEncodersSize(&num, &size);
		if(size == 0)
			return -1;  // Failure

		pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
		if(pImageCodecInfo == NULL)
			return -1;  // Failure

		GetImageEncoders(num, size, pImageCodecInfo);

		for(UINT j = 0; j < num; ++j)
		{
			if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
			{
				*pClsid = pImageCodecInfo[j].Clsid;
				free(pImageCodecInfo);
				return j;  // Success
			}    
		} // for

		free(pImageCodecInfo);
		return -1;  // Failure
	} // GetCodecClsid

	// data �� ARGB 32bit ����
	void RSaveAsJpeg(RealSpace2::RSaveAsJpegThread* pJpgThread)
	{
		// Setting up RAW Data
		BitmapData bitmapData;
		bitmapData.Width = pJpgThread->m_x;
		bitmapData.Height = pJpgThread->m_y;
		bitmapData.Stride = 4*bitmapData.Width;
		bitmapData.PixelFormat = PixelFormat32bppARGB; 
		bitmapData.Scan0 = pJpgThread->m_data;
		bitmapData.Reserved = NULL;

		// Write to Bitmap
		Rect rect(0, 0, pJpgThread->m_x, pJpgThread->m_y);
		Bitmap bitmap(pJpgThread->m_x, pJpgThread->m_y, 4*bitmapData.Width, PixelFormat32bppARGB, pJpgThread->m_data);
		bitmap.LockBits(&rect, ImageLockModeWrite|ImageLockModeUserInputBuf, PixelFormat32bppARGB, &bitmapData);
		bitmap.UnlockBits(&bitmapData);
		
		// Make WFileName
		WCHAR wstrName[256];
		int nNameLen = strlen(pJpgThread->m_fileName)+1;
		MultiByteToWideChar(CP_ACP, 0, pJpgThread->m_fileName, -1, wstrName, nNameLen-1);
		wstrName[nNameLen-1] = 0;

		// Save Bitmap
		CLSID  Clsid;
		int ret = GetCodecClsid(L"image/jpeg", &Clsid);
		bitmap.Save(wstrName, &Clsid, NULL);
		SAFE_DELETE(pJpgThread);
		g_TakingScreenie = false;
	}
#endif	// _USE_GDIPLUS

// data �� ARGB 32bit ����
bool RSaveAsBmp(int x,int y,void *data,const char *szFilename)
{
	void *memory;
	int nSize;

	if(!SaveMemoryBmp(x,y,data,&memory,&nSize))
		return false;

	FILE *file;
	file=fopen(szFilename,"wb+");
	if(!file) return false;

	fwrite( memory, nSize, 1, file );
	fclose( file );

	delete memory;

	return true;
}

// data �� ARGB 32bit ����
bool RScreenShot(int x,int y, BYTE* data, int dataSize, const char *szFilename)
{
	if (!g_TakingScreenie)
	{
		bool g_TakingScreenie = true;
		char szFullFileName[_MAX_DIR];
		sprintf(szFullFileName, "%s.jpg", szFilename);
		RSaveAsJpegThread* pJpegThread = new RSaveAsJpegThread(x, y, data, dataSize, szFullFileName, strlen(szFullFileName) + 1);
		std::thread t(RSaveAsJpeg, std::ref(pJpegThread));
		t.detach();
		return true;
	}
	return false;
}

bool RGetScreenLine(int sx,int sy,rvector *pos,rvector *dir)
{
	rvector scrpoint=rvector((float)sx,(float)sy,0.1f);

	rmatrix inv;
	float det;
	if(D3DXMatrixInverse(&inv,&det,&RViewProjectionViewport)==NULL)
		return false;

	rvector worldpoint;
	D3DXVec3TransformCoord(&worldpoint,&scrpoint,&inv);

	*pos=RCameraPosition;
	*dir=worldpoint-RCameraPosition;
	D3DXVec3Normalize(dir,dir);

	return true;
}

rvector RGetIntersection(int x,int y,rplane &plane)
{
	rvector scrpoint=rvector((float)x,(float)y,0.1f);	

	rmatrix inv;	
	float det;
	D3DXMatrixInverse(&inv,&det,&RViewProjectionViewport);

	rvector worldpoint;
	D3DXVec3TransformCoord(&worldpoint,&scrpoint,&inv);

	rvector ret;
	D3DXPlaneIntersectLine(&ret,&plane,&worldpoint,&RCameraPosition);

	return ret;
}

bool RGetIntersection( rvector& a, rvector& b, rplane &plane, rvector* pIntersection )
{
	rvector d = b - a;
	
	float t = -( plane.a * d.x + plane.b * d.y + plane.c * d.z + plane.d ) 
		/ ( plane.a * a.x + plane.b * a.y + plane.c * a.z );

	if( t < 0.0f || t > 1.0f )
	{
		return false;
	}
	else
	{
		*pIntersection = a + t*d;
	}
	return true;
}

LPDIRECT3DSURFACE9 RCreateImageSurface(const char *filename)
{
	char *buffer;
	MZFile mzf;

	if(g_pFileSystem)
	{
		if(!mzf.Open(filename,g_pFileSystem)) {
			if(!mzf.Open(filename))
				return false;
		}
	}else
	{
		if(!mzf.Open(filename))
			return false;
	}

	buffer=new char[mzf.GetLength()];
	mzf.Read(buffer,mzf.GetLength());

	LPDIRECT3DSURFACE9 pSurface=NULL;
	D3DXIMAGE_INFO info;

//	RGetDevice()->CreateImageSurface(1,1,D3DFMT_A8R8G8B8,&pSurface);
	RGetDevice()->CreateOffscreenPlainSurface(1,1,D3DFMT_A8R8G8B8,D3DPOOL_SCRATCH,&pSurface,NULL);
	D3DXLoadSurfaceFromFileInMemory(pSurface,NULL,NULL,buffer,mzf.GetLength(),NULL,D3DX_FILTER_NONE,0,&info);
	pSurface->Release();

	HRESULT hr;
	
//	hr = RGetDevice()->CreateImageSurface(info.Width,info.Height,info.Format,&pSurface);
	hr = RGetDevice()->CreateOffscreenPlainSurface(info.Width,info.Height,info.Format,D3DPOOL_SCRATCH,&pSurface,NULL);
	_ASSERT(hr==D3D_OK);
	hr = D3DXLoadSurfaceFromFileInMemory(pSurface,NULL,NULL,buffer,mzf.GetLength(),NULL,D3DX_FILTER_NONE,0,&info);
	_ASSERT(hr==D3D_OK);

	delete[] buffer;
	mzf.Close();

	return pSurface;
}

// ������ ���� - �⺻�� = 255
void RSetGammaRamp(unsigned short nGammaValue)
{
	D3DCAPS9 caps; 
	RGetDevice()->GetDeviceCaps(&caps);
	if (!(caps.Caps2 & D3DCAPS2_FULLSCREENGAMMA)) return;

	D3DGAMMARAMP gramp;

	RGetDevice()->GetGammaRamp(0,&gramp);

	for (int i = 0; i < 256; i++)
	{
		gramp.red[i] = ((i * nGammaValue) > 65535 ? 65535: (i * nGammaValue));
		gramp.green[i] = ((i * nGammaValue) > 65535 ? 65535: (i * nGammaValue));
		gramp.blue[i] = ((i * nGammaValue) > 65535 ? 65535: (i * nGammaValue));
	}
	RGetDevice()->SetGammaRamp(0,D3DSGR_CALIBRATE, &gramp);
}
//editable textbox info taken in
void RSetFrameLimitPerSeceond(int nFrameLimit)
{ // 1�ʴ� ������ ���� (���� ����: ������, 60fps, 120fps, 240fps)

	if( nFrameLimit < 60 || nFrameLimit > 500 )
		g_nFrameLimitValue = 0;
	else
		g_nFrameLimitValue = nFrameLimit;
}
/*
void RSetFrameLimitPerSeceond(unsigned short nFrameLimit)
{ // 1�ʴ� ������ ���� (���� ����: ������, 60fps, 120fps, 240fps)
	switch( nFrameLimit )
	{
	case 0:	{	g_nFrameLimitValue = 0;		}	break;
	case 1:	{	g_nFrameLimitValue = 60;	}	break;
	case 2:	{	g_nFrameLimitValue = 120;	}	break;
	case 3:	{	g_nFrameLimitValue = 240;	}	break;
	default:{	g_nFrameLimitValue = 0;		}	break;
	}
}
*/
// Custom: TrackSize ComboBox
void RSetTrackSize(int nTrackSize)
{
	switch (nTrackSize)
	{
	case 0:g_nTrackSize = 1; break;
	case 1: g_nTrackSize = 4; break;
	case 2: g_nTrackSize = 7; break;
	case 3: g_nTrackSize = 9; break;
	case 4: g_nTrackSize = 11; break;
	default: g_nTrackSize = 1; break;
	}
}

//void RSetWaterQuality(int nWaterQuality)
//{
//	switch (nWaterQuality)
//	{
//	case 0:
//		g_nWaterQuality = 256; break;
//	case 1:
//		g_nWaterQuality = 512; break;
//	case 2:
//		g_nWaterQuality = 1024; break;
//	case 3:
//		g_nWaterQuality = 2048; break;
//	default:
//		g_nWaterQuality = 512; break;
//	}
//}

void RDrawCylinder(rvector origin,float fRadius,float fHeight,int nSegment)
{
	RGetDevice()->SetTexture(0,NULL);
	RGetDevice()->SetTexture(1,NULL);
	RGetDevice()->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE );
	for(int i=0;i<nSegment;i++)
	{
		float fAngle=i*2*pi/(float)nSegment;
		float fAngle2=(i+1)*2*pi/(float)nSegment;

		rvector a=fRadius*rvector(cos(fAngle),sin(fAngle),0)+origin;
		rvector b=fRadius*rvector(cos(fAngle2),sin(fAngle2),0)+origin;

		RDrawLine(a+rvector(0,0,fHeight),b+rvector(0,0,fHeight),0xffff0000);
		RDrawLine(a-rvector(0,0,fHeight),b-rvector(0,0,fHeight),0xffff0000);

		RDrawLine(a+rvector(0,0,fHeight),a-rvector(0,0,fHeight),0xffff0000);
	}
}

void RDrawCorn(rvector center,rvector pole,float fRadius,int nSegment)
{
	RGetDevice()->SetTexture(0,NULL);
	RGetDevice()->SetTexture(1,NULL);
	RGetDevice()->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE );

	rvector dir=pole-center;
	Normalize(dir);
	
	rvector up= DotProduct(dir,rvector(0,0,1)) > DotProduct(dir,rvector(0,1,0)) ?
		rvector(0,1,0) : rvector(0,0,1);

	rvector x,y;
	CrossProduct(&x,up,dir);
	Normalize(x);
	CrossProduct(&y,x,dir);
	Normalize(y);

	for(int i=0;i<nSegment;i++)
	{
		float fAngle=i*2*pi/(float)nSegment;
		float fAngle2=(i+1)*2*pi/(float)nSegment;

		rvector a=fRadius*(x*cos(fAngle)+y*sin(fAngle))+center;
		rvector b=fRadius*(x*cos(fAngle2)+y*sin(fAngle2))+center;

		RDrawLine(a,pole,0xffff0000);	// ����E
		RDrawLine(a,b,0xffff0000);	// �ظ�E
	}
}

void RDrawSphere(rvector origin,float fRadius,int nSegment)
{
	RGetDevice()->SetTexture(0,NULL);
	RGetDevice()->SetTexture(1,NULL);
	RGetDevice()->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE );

	for(int i=0;i<nSegment;i++)
	{
		float fAngleZ=i*2*pi/(float)nSegment;
		float fAngleZ2=(i+1)*2*pi/(float)nSegment;
		for(int j=0;j<nSegment;j++)
		{
			float fAngle=j*2*pi/(float)nSegment;
			float fAngle2=(j+1)*2*pi/(float)nSegment;

			rvector a=fRadius*rvector(cos(fAngle)*cos(fAngleZ),sin(fAngle)*cos(fAngleZ),sin(fAngleZ))+origin;
			rvector b=fRadius*rvector(cos(fAngle2)*cos(fAngleZ),sin(fAngle2)*cos(fAngleZ),sin(fAngleZ))+origin;

			RDrawLine(a,b,0xffff0000);

			b=fRadius*rvector(cos(fAngle)*cos(fAngleZ2),sin(fAngle)*cos(fAngleZ2),sin(fAngleZ2))+origin;

			RDrawLine(a,b,0xffff0000);
		}
	}
}

// ���߿� ȭ��E��� �׷��ָ�E���ڴ� -_-;
void RDrawAxis(rvector origin,float fSize)
{
	RGetDevice()->SetTexture(0,NULL);
	RGetDevice()->SetTexture(1,NULL);
	RGetDevice()->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE );

	RDrawLine(origin,origin+rvector(fSize,0,0),0xffff0000);
	RDrawLine(origin,origin+rvector(0,fSize,0),0xff00ff00);
	RDrawLine(origin,origin+rvector(0,0,fSize),0xff0000ff);
}

void RDrawCircle(rvector origin,float fRadius, int nSegment)
{
	RGetDevice()->SetTexture(0,NULL);
	RGetDevice()->SetTexture(1,NULL);
	RGetDevice()->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE );
	for(int i=0;i<nSegment;i++)
	{
		float fAngle=i*2*pi/(float)nSegment;
		float fAngle2=(i+1)*2*pi/(float)nSegment;

		rvector a=fRadius*rvector(cos(fAngle),sin(fAngle),0)+origin;
		rvector b=fRadius*rvector(cos(fAngle2),sin(fAngle2),0)+origin;

		RDrawLine(a,b,0xffff0000);
	}
}

void RDrawArc(rvector origin, float fRadius, float fAngle1, float fAngle2, int nSegment, DWORD color)
{
	float fAngle = fAngle2 - fAngle1;
	for(int i=0;i<nSegment;i++)
	{
		float fAngleA = fAngle1 + (i * fAngle /(float)nSegment);
		float fAngleB = fAngle1 + ( (i+1) * fAngle /(float)nSegment );

		rvector a=fRadius*rvector(cos(fAngleA),sin(fAngleA),0)+origin;
		rvector b=fRadius*rvector(cos(fAngleB),sin(fAngleB),0)+origin;

		RDrawLine(a,b,color);
	}

}

void RSetWBuffer(bool bEnable)
{
	if (bEnable) {
		if (g_d3dcaps.RasterCaps & D3DPRASTERCAPS_WBUFFER) {
			RGetDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_USEW);
		}
		else {
			RGetDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
		}
	}
	else {
		RGetDevice()->SetRenderState(D3DRS_ZENABLE, FALSE);
	}
}

//////////////////////////////////////////////////////////////////////////
//		FIXED RESOLUTION 
//////////////////////////////////////////////////////////////////////////
//void FixedResolutionRenderStart()
//{
//	if( g_lpTexture == 0 )
//	{
//		D3DDISPLAYMODE mode;
//		RGetDevice()->GetDisplayMode( &mode );		
//
//		if(FAILED( D3DXCreateTexture( RGetDevice(), 1024, 1024, 1, D3DUSAGE_RENDERTARGET, mode.Format, D3DPOOL_DEFAULT, &g_lpTexture))
//			&& FAILED( D3DXCreateTexture( RGetDevice(), 1024, 1024, 1, D3DUSAGE_DYNAMIC|D3DUSAGE_RENDERTARGET, mode.Format, D3DPOOL_DEFAULT, &g_lpTexture)))
//		{
//			mlog("Failed to Create Fixed Resolution Texture...\n");
//		}
//		else
//		{
//			g_lpTexture->GetSurfaceLevel( 0, &g_lpSurface );
//			D3DSURFACE_DESC desc;
//			g_lpSurface->GetDesc( &desc );
//			RGetDevice()->CreateDepthStencilSurface( desc.Width, desc.Height, D3DFMT_D16, D3DMULTISAMPLE_NONE , &g_lpStencil );
//		}
//	}
//	RGetDevice()->GetRenderTarget( &g_lpSurfaceBackup );
//	RGetDevice()->GetDepthStencilSurface( &g_lpStencilBackup );
//	RGetDevice()->SetRenderTarget( g_lpSurface, g_lpStencil );
//	RGetDevice()->Clear(0,NULL,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,0x00000000,1.0f ,0);
//}
//void FixedResolutionRenderEnd()
//{
//	RGetDevice()->SetRenderTarget( g_lpSurfaceBackup, g_lpStencilBackup );
//	SAFE_RELEASE( g_lpSurfaceBackup );
//	SAFE_RELEASE( g_lpStencilBackup );
//}
//void FixedResolutionRenderInvalidate()
//{
//	SAFE_RELEASE( g_lpSurface);
//	SAFE_RELEASE( g_lpStencil);
//	SAFE_RELEASE( g_lpTexture);
//}
//void FixedResolutionRenderFlip()
//{
//	FixedResolutionRenderEnd();
//#define CORRECTION 0.01
//	static RTLVertex g_DummyPlane[4] = 
//	{
//		{D3DXVECTOR4( 0.0f - CORRECTION, 0.0f - CORRECTION, 1.0f, 1.0f ),0xFFFFFFFF,0.0f, 0.0f},
//		{D3DXVECTOR4( 800 + CORRECTION, 0.0f - CORRECTION, 1.0f, 1.0f ),0xFFFFFFFF,0.78125f, 0.0f},
//		{D3DXVECTOR4( 800 + CORRECTION, 600 + CORRECTION, 1.0f, 1.0f ),0xFFFFFFFF,0.78125f, 0.5859375f},
//		{D3DXVECTOR4( 0.0f - CORRECTION, 600 + CORRECTION, 1.0f, 1.0f ),0xFFFFFFFF,0.0f, 0.5859375f}
//	};
//
//	g_DummyPlane[0].p = D3DXVECTOR4(0.0f+CORRECTION, 0.0f - CORRECTION, 0.0f, 1.0f );
//	g_DummyPlane[1].p = D3DXVECTOR4(RGetScreenWidth()+CORRECTION, 0.0f - CORRECTION, 0.0f, 1.0f );
//	g_DummyPlane[2].p = D3DXVECTOR4(RGetScreenWidth()+CORRECTION, RGetScreenHeight() - CORRECTION, 0.0f, 1.0f );
//	g_DummyPlane[3].p = D3DXVECTOR4(0.0f+CORRECTION, RGetScreenHeight() - CORRECTION, 0.0f, 1.0f );
//
//	RGetDevice()->SetTexture( 0, g_lpTexture );
//	RGetDevice()->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
//	RGetDevice()->SetTextureStageState( 0,  D3DTSS_COLORARG1, D3DTA_TEXTURE );
//	RGetDevice()->SetFVF( RTLVertexType );
//
//	RGetDevice()->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, g_DummyPlane, sizeof(RTLVertex) );
//
//	RFlip(true);	
//	FixedResolutionRenderStart();
//}
//bool IsFixedResolution() 
//{
//	return g_bFixedResolution;
//}
//void SetFixedResolution( bool b )
//{
//	g_bFixedResolution = b;
//	if( b )
//	{
//		FixedResolutionRenderStart();
//	}
//}

int RGetAdapterModeCount( D3DFORMAT Format , UINT Adapter /* = D3DADAPTER_DEFAULT  */ )
{
	return g_pD3D->GetAdapterModeCount( Adapter,Format );
}

bool REnumAdapterMode( UINT Adapter, D3DFORMAT Format ,UINT Mode, D3DDISPLAYMODE* pMode )
{
	if( pMode == 0 )
		return false;
	g_pD3D->EnumAdapterModes( Adapter, Format, Mode, pMode );
	return true;
}

void RSetFog( bool bFog, float fNear /* = 0 */, float fFar/* =0 */, DWORD dwColor/* =0xFFFFFFFF  */)
{
	g_bFog = bFog;
	g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, g_bFog );
	if(bFog)
	{
		g_fFogNear = fNear;
		g_fFogFar = fFar;
		g_dwFogColor = dwColor;
		g_pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, dwColor);
		g_pd3dDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
		g_pd3dDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD *)(&g_fFogNear));
		g_pd3dDevice->SetRenderState(D3DRS_FOGEND, *(DWORD *)(&g_fFogFar));	
	}
}

bool RGetFog() { return g_bFog;}
float RGetFogNear(){ return g_fFogNear;}
float RGetFogFar(){ return g_fFogFar;}
DWORD RGetFogColor(){ return g_dwFogColor;}


bool CheckVideoAdapterSupported()
{
	bool bSupported=true;

	D3DADAPTER_IDENTIFIER9 *ai=RGetAdapterID();
	if (ai==NULL) return false;	


	if (ai->VendorId==0x8086)
	{
		if(ai->DeviceId==0x7125)	// 82810E
			bSupported=false;

		// intel 810 , 815  �ø���E
		//if(ai->DeviceId==0x7800 || ai->DeviceId==0x7121 || ai->DeviceId==0x7123 || ai->DeviceId==0x7125 || ai->DeviceId==0x1132)
		//	bSupported=false;
	}
/*
	if(ai->VendorId==0x1039)	// SiS Ĩ��
	{
		bSupported=false;
	}
	if(ai->VendorId==0x5333) {	// Savage
		if(ai->DeviceId==0x8a22)
			bSupported=false;
	}
	if(ai->VendorId==0x10de) {	// NVidia ī�嵁E
		if(ai->DeviceId==0x2c || ai->DeviceId==0x2d)
			bSupported=false;
	}
*/
	if(ai->VendorId==0x121a) {	// 3dfx ī�嵁E
		bSupported=false;
	}


	return bSupported;
}

// Custom: Flash Window
//void RFlashWindow()
//{
//	if (RIsActive())
//		return;
//
//	FLASHWINFO info;
//	info.cbSize = sizeof(info);
//	info.hwnd = g_hWnd;
//	info.dwFlags = FLASHW_ALL;
//	info.dwTimeout = 0;
//	info.uCount = 3;
//	
//	FlashWindowEx(&info);
//}
#ifdef _TRAILCOLOR
void RSetTrailColor(int nTrailColor)
{
	switch (nTrailColor)
	{
	case 0:
		g_dwColorValue[0] = 0x4fffffff;
		g_dwColorValue[1] = 0x0fffffff;
		break;
	case 1:
		g_dwColorValue[0] = 0x4f57dd4b;
		g_dwColorValue[1] = 0x0f57dd4b;
		break;
	case 2:
		g_dwColorValue[0] = 0x4fff3232;
		g_dwColorValue[1] = 0x0fff3232;
		break;
	case 3:
		g_dwColorValue[0] = 0x4fFF69B4;
		g_dwColorValue[1]=  0x0fFF69B4;
		break;
	default:
		g_dwColorValue[0] = 0x4fffffff;
		g_dwColorValue[1] = 0x0fffffff;
		break;
	}
}
#endif


_NAMESPACE_REALSPACE2_END
