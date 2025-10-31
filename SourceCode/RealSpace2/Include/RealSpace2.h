#ifndef __REALSPACE2_H
#define __REALSPACE2_H

#include "RTypes.h"
#include "RError.h"
#include "MZFileSystem.h"

#pragma comment(lib,"d3dx9.lib")

#include "RNameSpace.h"
_NAMESPACE_REALSPACE2_BEGIN

class RParticleSystem;

class RSaveAsJpegThread
{
public:
	RSaveAsJpegThread(int x, int y, BYTE* data, int datasize, const char* fileName, int fileNameSize)
	{
		m_x = x;
		m_y = y;
		m_data = new BYTE[datasize];
		m_fileName = new char[fileNameSize];
		memcpy(m_data, data, datasize);
		strcpy(m_fileName, fileName);
	}
	~RSaveAsJpegThread()
	{
		m_x = 0;
		m_y = 0;
		SAFE_DELETE_ARRAY(m_data);
		SAFE_DELETE_ARRAY(m_fileName);
	}
	int m_x;
	int m_y;
	BYTE* m_data;
	char* m_fileName;
};

// ¹Ù±ù¿¡¼­ ÂE¶ÇÒ¸¸ÇÑ °ÍµE
bool	RIsActive();
bool	RIsQuery();
void	RSetQuery(bool b);//Å×½ºÆ®¿E.
int 	RGetScreenType();
bool	RIsHardwareTNL();
bool	RIsSupportVS();
bool	RIsAvailUserClipPlane();
bool	RIsTrilinear();
int		RGetApproxVMem();
int		RGetScreenWidth();
int		RGetScreenHeight();
int     RGetIsWidthScreen();
float	RGetWidthScreen();
int		RGetIsLongScreen(); // Custom: MAIET update
//float	RGetAspectRatio(); // Custom: 16:9 support
int		RGetPicmip();
RPIXELFORMAT RGetPixelFormat();
D3DADAPTER_IDENTIFIER9*	RGetAdapterID();
void SetClearColor(DWORD c);
int		RGetVidioMemory();
void	RSetWBuffer(bool bEnable);
bool	RIsStencilBuffer();
//void	RFlashWindow(); // Custom: Flash Window
int		RGetIsHDScreen();
#ifdef _TRAILCOLOR
void	RSetTrailColor(int nTrailColor = 0);
#endif
extern int getTaskBarHeight();

int RGetAdapterModeCount( D3DFORMAT Format,UINT Adapter = D3DADAPTER_DEFAULT );
bool REnumAdapterMode( UINT Adapter, D3DFORMAT Format ,UINT Mode, D3DDISPLAYMODE* pMode );


// ÆÄÆ¼Å¬ ½Ã½ºÅÛ
RParticleSystem *RGetParticleSystem();


extern int g_nFrameCount,g_nLastFrameCount;
extern float g_fFPS;
extern int g_nFrameLimitValue;
extern HWND	g_hWnd;
extern MZFileSystem *g_pFileSystem;
extern bool g_isDirect3D9ExEnabled;
extern DWORD g_dwLastTime;
#ifdef _TRAILCOLOR
extern DWORD g_dwColorValue[2];
#endif
// ÃÊ±âÈ­ & Á¾·E
bool RInitDisplay(HWND hWnd,const RMODEPARAMS *params);
bool RCloseDisplay();
void RSetFileSystem(MZFileSystem *pFileSystem);
void RAdjustWindow(const RMODEPARAMS *pModeParams);		// À©µµ¿EÅ©±â¸¦ ¸ÂÃçÁÜ

// µð¹ÙÀÌ½º & ºäÆ÷Æ® 
LPDIRECT3DDEVICE9	RGetDevice();

// ¸ðµåÀE¯ & ÇÃ¸®ÇÎ°EÃ
void RResetDevice(const RMODEPARAMS *params);
RRESULT RIsReadyToRender();
bool RFlip();
bool REndScene();
bool RBeginScene();

// RSÀÇ ·»´õ¿Í °EÃµÈ ¿É¼ÇµE- ´EÃß°¡ÇÒ °Í ÀÖÀ¸¸EÃß°¡ÇÏµµ·Ï ÇÏÀÚ.
enum RRENDER_FLAGS
{
	RRENDER_CLEAR_BACKBUFFER		= 0x000001,
};

void RSetRenderFlags(unsigned long nFlags);
unsigned long RGetRenderFlags();

//Fog
void RSetFog(bool bFog, float fNear = 0, float fFar=0, DWORD dwColor=0xFFFFFFFF );
bool RGetFog();
float RGetFogNear();
float RGetFogFar();
DWORD RGetFogColor();

// Ä«¸Þ¶E°EÃ	( RCamera.cpp )
extern rvector RCameraPosition,RCameraDirection,RCameraUp;
extern rmatrix RView,RProjection,RViewProjection,RViewport,RViewProjectionViewport;

void RSetCamera(const rvector &from, const rvector &at, const rvector &up);
void RUpdateCamera();		// rsetcamera ¸¦ »ç¿EÏ°Å³ª Á÷Á¢ °úà£ÈÄ update¸¦ ÇØÁà¾ßÇÕ´Ï´Ù.

void RSetProjection(float fFov,float fNearZ,float fFarZ);
void RSetProjection(float fFov,float fAspect,float fNearZ,float fFarZ);

inline rplane *RGetViewFrustum();
void RSetViewport(int x1,int y1,int x2,int y2);
D3DVIEWPORT9		*RGetViewport();

void RResetTransform();		// ¹Û¿¡¼­ ¾µ ÀÏÀº ¾ø´Â Æò¼Ç.. µð¹ÙÀÌ½º ·Î½ºÆ® µÇ¾úÀ»¶§ ºÒ¸².

// ÅøÀ» À§ÇØ¼­ ³EE
extern D3DPRESENT_PARAMETERS g_d3dpp;

///////////////////////// µµ¿òÀ» ÁÖ´Â Æã¼ÇµE

// È­¸éÁÂÇ¥°Ex,y ¸¦ 3Â÷¿øÀÇ lineÀ¸·Î º¯È¯
bool RGetScreenLine(int sx,int sy,rvector *pos,rvector *dir);

// È­¸éÁÂÇ¥°èÀÇ x,y ¿Í ÀÓÀÇÀÇ Æò¸é°úÀÇ ±³Á¡À» ±¸ÇÑ´Ù.
rvector RGetIntersection(int x,int y,rplane &plane);
bool RGetIntersection( rvector& a, rvector& b, rplane &plane, rvector* pIntersection );

// 3d ÁÂÇ¥¸¦ È­¸éÁÂÇ¥·Î transform ÇÑ´Ù.
rvector RGetTransformCoord(rvector &coord);

// ±×¸®´Â°Í°E°EÃµÈ Æã¼Ç..
void RDrawLine(rvector &v1,rvector &v2,DWORD dwColor);
void RDrawCylinder(rvector origin,float fRadius,float fHeight,int nSegment);
void RDrawCorn(rvector center,rvector pole,float fRadius,int nSegment);
void RDrawSphere(rvector origin,float fRadius,int nSegment);
void RDrawAxis(rvector origin,float fSize);
void RDrawCircle(rvector origin,float fRadius, int nSegment);
void RDrawArc(rvector origin, float fRadius, float fAngle1, float fAngle2, int nSegment, DWORD color);

// bmp¸¦ ÀúÀåÇÑ´Ù data ´Â argb °¢°¢ 1byte·Î µÇ¾ûÜÖ´Ù.
bool RSaveAsBmp(int x,int y,void *data,const char *szFilename);

// ½ºÅ©¸°¼¦À» ÀúÀåÇÑ´Ù data ´Â argb °¢°¢ 1byte·Î µÇ¾ûÜÖ´Ù.
bool RScreenShot(int x, int y, BYTE* data, int dataSize, const char *szFilename);

LPDIRECT3DSURFACE9 RCreateImageSurface(const char *filename);

// °¨¸¶°ª ¼³Á¤
void RSetGammaRamp(unsigned short nGammaValue = 255);
// 1ÃÊ´EÇÁ·¹ÀÓ Á¦ÇÑ ¼³Á¤
// 1ÃÊ´ç ÇÁ·¹ÀÓ Á¦ÇÑ ¼³Á¤
void RSetFrameLimitPerSeceond(int nFrameLimit = 0);
//void RSetFrameLimitPerSeceond(unsigned short nFrameLimit = 0);

// Custom: swordtrack size
extern int g_nTrackSize;
//extern int g_nWaterQuality;
void RSetTrackSize(int nTrackSize = 0);
//void RSetWaterQuality(int nWaterQuality = 0);
// wbuffer enable
void RSetWBuffer(bool bEnable);

// winmain ¾ûÙÃ½ºÆ®.

enum RFUNCTIONTYPE {
	RF_CREATE  =0,
	RF_DESTROY,
	RF_RENDER,
	RF_UPDATE,
	RF_INVALIDATE,
	RF_RESTORE,
	RF_ACTIVATE,
	RF_DEACTIVATE,
	RF_ERROR,

	RF_ENDOFRFUNCTIONTYPE		// ÀÌ°Ç ½ÇÁ¦ ¾²´Â Æã¼ÇÀÌ ¾Æ´Ï°E. ¼ýÀÚ¸¦ ¼¼±âÀ§ÇÑ°ÍÀÓ.
};

typedef RRESULT (*RFFUNCTION)(void *Params);

//void RSetFunction(RFUNCTIONTYPE ft,RFFUNCTION pfunc);
//int RMain(const char *AppName, HINSTANCE this_inst, HINSTANCE prev_inst, LPSTR cmdline, int cmdshow, RMODEPARAMS *pModeParams, WNDPROC winproc=NULL, WORD nIconResID=NULL );
//
//int RRun();

//////Do Not Use This Method --> Danger...
//void FixedResolutionRenderStart();
//void FixedResolutionRenderEnd();
//void FixedResolutionRenderInvalidate();
////// Use This Method
//void FixedResolutionRenderFlip();
//bool IsFixedResolution();
//void SetFixedResolution( bool b );


////// inline functions
extern rplane RViewFrustum[6];
inline rplane *RGetViewFrustum()
{
	return RViewFrustum;
}

extern void ComputeZPlane(rplane* plane, float z, int sign);

_NAMESPACE_REALSPACE2_END

namespace RAdvancedGraphics
{
	extern bool bDepthBuffering;
	extern int  nMultiSampling;
};

#endif