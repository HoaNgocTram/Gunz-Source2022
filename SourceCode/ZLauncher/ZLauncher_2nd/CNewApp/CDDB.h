#ifndef _INCLUDED_CDDB_
#define _INCLUDED_CDDB_

#define NOCOLOR			0xFFFFFFFF

enum TextStyle
{
	TS_BOLD = 0x0001,
	TS_ITALIC = 0x0002,
	TS_UDLINE = 0x0004,
};

enum CoordinateMode
{
	CM_ABSOLUTE = 0,
	CM_RELATIVE = 1,
};

class CDDB
{
protected:
	CDC				m_DC;
	CDC*			m_pParentDC;

	int				m_nWidth;
	int				m_nHeight;
	COLORREF		m_nColor;
	COLORREF		m_nTransColor;

	CoordinateMode	m_CoordinateMode;

public:
	CDDB();
	~CDDB();

	CDC* GetDC() { return &m_DC; }
	CDC* GetParentWnd() { return m_pParentDC; }

	int GetWidth() { return m_nWidth; }
	int GetHeight() { return m_nHeight; }

	void SetColor(COLORREF color) { m_nColor = color; }
	COLORREF GetColor() { return m_nColor; }

	void SetTransColor(COLORREF color) { m_nTransColor = color; }
	COLORREF GetTransColor() { return m_nTransColor; }

	void SetCoordinateMode(CoordinateMode cm) { m_CoordinateMode = cm; }
	CoordinateMode GetCoordinateMode() { return m_CoordinateMode; }

	bool CreateDDB(CDC* pDC, int width, int height, COLORREF color = 0);
	bool CreateDDB(CDC* pDC, UINT nIDResource);
	bool CreateDDB(CDC* pDC, LPCTSTR szBmpFileName);
	bool DeleteDDB();

	void Rendering(CDC* pDC);
	void Rendering(CDC* pDC, int x, int y, int xSrc, int ySrc, int dxSrc, int dySrc);

	bool PutBitmap(int x, int y, CDC *DC, int xSrc, int ySrc, int dxSrc, int dySrc, DWORD dwRop = SRCCOPY, int alpha = 255);
	bool PutBitmap(int x, int y, CDC *DC, int xSrc, int ySrc, int dxSrc, int dySrc, int width, int height, DWORD dwRop = SRCCOPY, int alpha = 255);
	bool PutBitmap(int x, int y, UINT nIDResource, int xSrc, int ySrc, int dxSrc, int dySrc, DWORD dwRop = SRCCOPY, int alpha = 255);
	bool PutBitmap(int x, int y, UINT nIDResource, int xSrc, int ySrc, int dxSrc, int dySrc, int width, int height, DWORD dwRop = SRCCOPY, int alpha = 255);

	void FillColor(COLORREF color);
	bool FillTiledBitmap(int x, int y, int dx, int dy, CDC *pDC, int xSrc, int ySrc, int dxSrc, int dySrc);
	bool FillTiledBitmap(int x, int y, int dx, int dy, UINT nIDResource, int xSrc, int ySrc, int dxSrc, int dySrc);

	void PutPixel(int x, int y, COLORREF color = NOCOLOR);
	COLORREF GetPixel(int x, int y);
	void PutLine(int x, int y, int dx, int dy, COLORREF color = NOCOLOR, int width = 1);
	void PutBox(int x, int y, int dx, int dy, COLORREF color = NOCOLOR, int width = 1);
	void PutFillBox(int x, int y, int dx, int dy, COLORREF color = NOCOLOR);
	void PutRoundBox(int x, int y, int dx, int dy, int radius, COLORREF color = NOCOLOR, int width = 1);
	void PutFillRoundBox(int x, int y, int dx, int dy, int radius, COLORREF color = NOCOLOR);
	void PutCircle(int x, int y, int radius, COLORREF color = NOCOLOR, int width = 1);
	void PutFillCircle(int x, int y, int radius, COLORREF color = NOCOLOR);
	void PutFillEllips(int x, int y, int rx, int ry, COLORREF color = NOCOLOR);
	void PutText(int x, int y, int nSize, LPCTSTR lpszFont, LPCTSTR lpszMsg, COLORREF color = NOCOLOR, UINT nStyle = 0);
	void PutText(int x, int y, int dx, int dy, int nSize, LPCTSTR lpszFont, LPCTSTR lpszMsg, COLORREF color = NOCOLOR, UINT nStyle = 0, UINT nAlign = DT_LEFT);

	void GetRelativeCoordinate(int *x1, int *y1, int *x2, int *y2);
	void GetAbsoluteCoordinate(int *x1, int *y1, int *width, int *height);
};

#endif
