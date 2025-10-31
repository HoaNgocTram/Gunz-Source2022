#include "stdafx.h"
#include "CDDB.h"

CDDB::CDDB()
{
	m_pParentDC = NULL;

	m_nWidth = 0;
	m_nHeight = 0;
	m_nColor = RGB(255, 255, 255);
	m_nTransColor = RGB(255, 0, 255);
	m_CoordinateMode = CM_RELATIVE;
}

CDDB::~CDDB()
{
	m_DC.DeleteDC();
	m_pParentDC = NULL;

	m_nWidth = 0;
	m_nHeight = 0;
	m_nColor = RGB(255, 255, 255);
	m_nTransColor = RGB(255, 0, 255);
	m_CoordinateMode = CM_RELATIVE;
}

bool CDDB::CreateDDB(CDC* pDC, int width, int height, COLORREF color)
{
	if (!m_DC.CreateCompatibleDC(pDC))
		return false;

	m_pParentDC = pDC;

	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(pDC, width, height);
	m_DC.SelectObject(&bitmap);

	m_nWidth = width;
	m_nHeight = height;
	m_DC.SetBkMode(TRANSPARENT);

	m_DC.FillSolidRect(0, 0, width, height, color);
	m_nColor = color;

	return true;
}

bool CDDB::CreateDDB(CDC* pDC, UINT nIDResource)
{
	if (!m_DC.CreateCompatibleDC(pDC))
		return false;

	m_pParentDC = pDC;

	CBitmap bitmap;
	if (!bitmap.LoadBitmap(nIDResource))
		return false;

	m_DC.SelectObject(&bitmap);

	BITMAP bmpInfo;
	bitmap.GetBitmap(&bmpInfo);
	m_nWidth = bmpInfo.bmWidth;
	m_nHeight = bmpInfo.bmHeight;
	m_DC.SetBkMode(TRANSPARENT);

	return true;
}

bool CDDB::CreateDDB(CDC* pDC, LPCTSTR szBmpFileName)
{
	return false;
}

bool CDDB::DeleteDDB()
{
	if (m_DC.DeleteDC() == TRUE)
	{
		m_DC.DeleteDC();
		m_pParentDC = NULL;

		m_nWidth = 0;
		m_nHeight = 0;
		m_nColor = RGB(255, 255, 255);
		m_nTransColor = RGB(255, 0, 255);
		m_CoordinateMode = CM_RELATIVE;

		return true;
	}

	return  false;
}

void CDDB::Rendering(CDC* pDC)
{
	if (m_pParentDC)
	{
		pDC->BitBlt(0, 0, m_nWidth, m_nHeight, &m_DC, 0, 0, SRCCOPY);
	}
}

void CDDB::Rendering(CDC* pDC, int x, int y, int xSrc, int ySrc, int dxSrc, int dySrc)
{
	if (m_CoordinateMode != CM_RELATIVE)
		GetRelativeCoordinate(&xSrc, &ySrc, &dxSrc, &dySrc);

	if (m_pParentDC)
	{
		pDC->BitBlt(x, y, dxSrc, dySrc, &m_DC, xSrc, ySrc, SRCCOPY);
	}
}

bool CDDB::PutBitmap(int x, int y, CDC *pDC, int xSrc, int ySrc, int dxSrc, int dySrc, DWORD dwRop, int alpha)
{
	return PutBitmap(x, y, pDC, xSrc, ySrc, dxSrc, dySrc, dxSrc, dySrc, dwRop, alpha);
}

bool CDDB::PutBitmap(int x, int y, CDC *pDC, int xSrc, int ySrc, int dxSrc, int dySrc, int width, int height, DWORD dwRop, int alpha)
{
	if (m_CoordinateMode != CM_RELATIVE)
		GetRelativeCoordinate(&xSrc, &ySrc, &dxSrc, &dySrc);

	if (alpha < 255)
	{
		BLENDFUNCTION blendFunc = { AC_SRC_OVER, 0, alpha, 0 };

		AlphaBlend(m_DC, x, y, width, height, pDC->m_hDC, xSrc, ySrc, dxSrc, dySrc, blendFunc);
	}

	else if (dwRop == TRANSPARENT)
	{
		TransparentBlt(m_DC, x, y, width, height, pDC->m_hDC, xSrc, ySrc, dxSrc, dySrc, m_nTransColor);
	}

	else
	{
		m_DC.StretchBlt(x, y, width, height, pDC, xSrc, ySrc, dxSrc, dySrc, dwRop);
	}

	return true;
}

bool CDDB::PutBitmap(int x, int y, UINT nIDResource, int xSrc, int ySrc, int dxSrc, int dySrc, DWORD dwRop, int alpha)
{
	return PutBitmap(x, y, nIDResource, xSrc, ySrc, dxSrc, dySrc, dxSrc, dySrc, dwRop, alpha);
}

bool CDDB::PutBitmap(int x, int y, UINT nIDResource, int xSrc, int ySrc, int dxSrc, int dySrc, int width, int height, DWORD dwRop, int alpha)
{
	CDC NewDC;
	NewDC.CreateCompatibleDC(&m_DC);

	CBitmap NewBmp;
	if (!NewBmp.LoadBitmap(nIDResource))
		return false;

	CBitmap *pOldBmp = (CBitmap *)NewDC.SelectObject(&NewBmp);
	BITMAP bmpInfo;
	NewBmp.GetBitmap(&bmpInfo);

	bool bRetVal = PutBitmap(x, y, &NewDC, xSrc, ySrc, dxSrc, dySrc, width, height, dwRop, alpha);

	NewDC.SelectObject(pOldBmp);

	return bRetVal;
}

void CDDB::FillColor(COLORREF color)
{
	m_DC.FillSolidRect(0, 0, m_nWidth, m_nHeight, color);
}

bool CDDB::FillTiledBitmap(int x, int y, int dx, int dy, CDC *pDC, int xSrc, int ySrc, int dxSrc, int dySrc)
{
	if (m_CoordinateMode != CM_ABSOLUTE)
		GetAbsoluteCoordinate(&x, &y, &dx, &dy);

	if (m_CoordinateMode != CM_RELATIVE)
		GetRelativeCoordinate(&xSrc, &ySrc, &dxSrc, &dySrc);

	int px, py;
	int width, height;
	for (py = y; py < dy; py += dySrc)
	{
		height = dy - py;
		if (height > dySrc)
			height = dySrc;

		for (px = x; px < dx; px += dxSrc)
		{
			width = dx - px;
			if (width > dxSrc)
				width = dxSrc;

			m_DC.BitBlt(px, py, width, height, pDC, xSrc, ySrc, SRCCOPY);
		}
	}

	return true;
}

bool CDDB::FillTiledBitmap(int x, int y, int dx, int dy, UINT nIDResource, int xSrc, int ySrc, int dxSrc, int dySrc)
{
	CDC NewDC;
	NewDC.CreateCompatibleDC(&m_DC);

	CBitmap NewBmp;
	if (!NewBmp.LoadBitmap(nIDResource))
		return false;

	CBitmap *pOldBmp = (CBitmap *)NewDC.SelectObject(&NewBmp);
	BITMAP bmpInfo;
	NewBmp.GetBitmap(&bmpInfo);

	BOOL bRetVal = FillTiledBitmap(x, y, dx, dy, &NewDC, xSrc, ySrc, dxSrc, dySrc);

	NewDC.SelectObject(pOldBmp);

	return ((bRetVal > 0) ? true : false);
}

void CDDB::PutPixel(int x, int y, COLORREF color)
{
	if (color != NOCOLOR)
		m_nColor = color;

	m_DC.SetPixel(x, y, m_nColor);
}

COLORREF CDDB::GetPixel(int x, int y)
{
	return m_DC.GetPixel(x, y);
}

void CDDB::PutLine(int x, int y, int dx, int dy, COLORREF color, int width)
{
	if (color != NOCOLOR)
		m_nColor = color;

	if (m_CoordinateMode != CM_ABSOLUTE)
		GetAbsoluteCoordinate(&x, &y, &dx, &dy);

	CPen pen;
	pen.CreatePen(PS_SOLID, width, m_nColor);
	CPen *pOldPen = (CPen *)m_DC.SelectObject(&pen);

	m_DC.MoveTo(x, y);
	m_DC.LineTo(dx, dy);
	m_DC.SelectObject(pOldPen);
}

void CDDB::PutBox(int x, int y, int dx, int dy, COLORREF color, int width)
{
	if (color != NOCOLOR)
		m_nColor = color;

	if (m_CoordinateMode != CM_ABSOLUTE)
		GetAbsoluteCoordinate(&x, &y, &dx, &dy);

	CPen pen;
	pen.CreatePen(PS_SOLID, width, m_nColor);
	CPen *pOldPen = (CPen *)m_DC.SelectObject(&pen);

	m_DC.MoveTo(x, y);
	m_DC.LineTo(dx - 1, y);
	m_DC.LineTo(dx - 1, dy - 1);
	m_DC.LineTo(x, dy - 1);
	m_DC.LineTo(x, y - 1);
	m_DC.SelectObject(pOldPen);
}

void CDDB::PutFillBox(int x, int y, int dx, int dy, COLORREF color)
{
	if (color != NOCOLOR)
		m_nColor = color;

	if (m_CoordinateMode != CM_RELATIVE)
		GetRelativeCoordinate(&x, &y, &dx, &dy);

	m_DC.FillSolidRect(x, y, dx, dy, m_nColor);
}

void CDDB::PutRoundBox(int x, int y, int dx, int dy, int radius, COLORREF color, int width)
{
	if (color != NOCOLOR)
		m_nColor = color;

	if (m_CoordinateMode != CM_ABSOLUTE)
		GetAbsoluteCoordinate(&x, &y, &dx, &dy);

	CPen pen;
	pen.CreatePen(PS_SOLID, width, m_nColor);
	CPen *pOldPen = (CPen *)m_DC.SelectObject(&pen);

	m_DC.MoveTo(x + radius, y);
	m_DC.AngleArc(x + radius, y + radius, radius, 90, 90);
	m_DC.MoveTo(dx - 1, y + radius);
	m_DC.AngleArc(dx - radius - 1, y + radius, radius, 0, 90);
	m_DC.MoveTo(x, dy - radius - 1);
	m_DC.AngleArc(x + radius, dy - radius - 1, radius, 180, 90);
	m_DC.MoveTo(dx - radius - 1, dy - 1);
	m_DC.AngleArc(dx - radius - 1, dy - radius - 1, radius, 270, 90);

	m_DC.MoveTo(x + radius, y);
	m_DC.LineTo(dx - radius + 1, y);
	m_DC.MoveTo(x + radius, dy - 1);
	m_DC.LineTo(dx - radius + 1, dy - 1);
	m_DC.MoveTo(x, y + radius);
	m_DC.LineTo(x, dy - radius + 1);
	m_DC.MoveTo(dx - 1, y + radius);
	m_DC.LineTo(dx - 1, dy - radius + 1);

	m_DC.SelectObject(pOldPen);
}

void CDDB::PutFillRoundBox(int x, int y, int dx, int dy, int radius, COLORREF color)
{
	if (color != NOCOLOR)
		m_nColor = color;

	if (m_CoordinateMode != CM_ABSOLUTE)
		GetAbsoluteCoordinate(&x, &y, &dx, &dy);

	CPen pen;
	pen.CreatePen(PS_SOLID, 1, m_nColor);
	CPen *pOldPen = (CPen *)m_DC.SelectObject(&pen);

	CBrush brush;
	brush.CreateSolidBrush(m_nColor);
	CBrush *pOldBrush = (CBrush *)m_DC.SelectObject(&brush);

	m_DC.RoundRect(x, y, dx, dy, radius, radius);

	m_DC.SelectObject(pOldPen);
	m_DC.SelectObject(&pOldBrush);
}

void CDDB::PutCircle(int x, int y, int radius, COLORREF color, int width)
{
	if (color != NOCOLOR)
		m_nColor = color;

	CPen pen;
	pen.CreatePen(PS_SOLID, width, m_nColor);
	CPen *pOldPen = (CPen *)m_DC.SelectObject(&pen);

	m_DC.MoveTo(x + radius, y);
	m_DC.AngleArc(x, y, radius, 0, 360);

	m_DC.SelectObject(pOldPen);
}

void CDDB::PutFillCircle(int x, int y, int radius, COLORREF color)
{
	if (color != NOCOLOR)
		m_nColor = color;

	CPen pen;
	pen.CreatePen(PS_SOLID, 1, m_nColor);
	CPen *pOldPen = (CPen *)m_DC.SelectObject(&pen);

	CBrush brush;
	brush.CreateSolidBrush(m_nColor);
	CBrush *pOldBrush = (CBrush *)m_DC.SelectObject(&brush);

	m_DC.Ellipse(x - radius, y - radius, x + radius, y + radius);

	m_DC.SelectObject(pOldPen);
	m_DC.SelectObject(pOldBrush);
}

void CDDB::PutFillEllips(int x, int y, int rx, int ry, COLORREF color)
{
	if (color != NOCOLOR)
		m_nColor = color;

	CPen pen;
	pen.CreatePen(PS_SOLID, 1, m_nColor);
	CPen *pOldPen = (CPen *)m_DC.SelectObject(&pen);

	CBrush brush;
	brush.CreateSolidBrush(m_nColor);
	CBrush *pOldBrush = (CBrush *)m_DC.SelectObject(&brush);

	m_DC.Ellipse(x - rx, y - ry, x + rx, y + ry);

	m_DC.SelectObject(pOldPen);
	m_DC.SelectObject(pOldBrush);
}

void CDDB::PutText(int x, int y, int nSize, LPCTSTR lpszFont, LPCTSTR lpszMsg, COLORREF color, UINT nStyle)
{
	if (color != NOCOLOR)
		m_nColor = color;

	UINT nBold = (nStyle & TS_BOLD) ? FW_BOLD : FW_NORMAL;
	BYTE bItalic = (nStyle & TS_ITALIC) ? true : false;
	BYTE bUnderLine = (nStyle & TS_UDLINE) ? true : false;

	CFont font;
	font.CreateFont(nSize, 0, 0, 0, nBold, bItalic, bUnderLine, 0, ANSI_CHARSET,
		OUT_CHARACTER_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS, lpszFont);

	CFont *pOldFont = (CFont *)m_DC.SelectObject(&font);

	m_DC.SetTextColor(m_nColor);
	m_DC.TextOut(x, y, lpszMsg);
	m_DC.SelectObject(pOldFont);
}

void CDDB::PutText(int x, int y, int dx, int dy, int nSize, LPCTSTR lpszFont, LPCTSTR lpszMsg, COLORREF color, UINT nStyle, UINT nAlign)
{
	if (color != NOCOLOR)
		m_nColor = color;

	if (m_CoordinateMode != CM_ABSOLUTE)
		GetAbsoluteCoordinate(&x, &y, &dx, &dy);

	UINT nBold = (nStyle & TS_BOLD) ? FW_BOLD : FW_NORMAL;
	BYTE bItalic = (nStyle & TS_ITALIC) ? true : false;
	BYTE bUnderLine = (nStyle & TS_UDLINE) ? true : false;

	CFont font;
	font.CreateFont(nSize, 0, 0, 0, nBold, bItalic, bUnderLine, 0, ANSI_CHARSET,
		OUT_CHARACTER_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS, lpszFont);

	CFont *pOldFont = (CFont *)m_DC.SelectObject(&font);

	CRect rect;
	rect.top = y;
	rect.left = x;
	rect.right = dx;
	rect.bottom = dy;

	m_DC.SetTextColor(m_nColor);
	m_DC.DrawText(lpszMsg, (LPRECT)rect, nAlign);
	m_DC.SelectObject(pOldFont);
}

void CDDB::GetRelativeCoordinate(int *x1, int *y1, int *x2, int *y2)
{
	*x2 = *x2 - *x1;
	*y2 = *y2 - *y1;
}

void CDDB::GetAbsoluteCoordinate(int *x1, int *y1, int *width, int *height)
{
	*width = *x1 + *width;
	*height = *y1 + *height;
}