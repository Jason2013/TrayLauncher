
#include "GDIWavePic.h"

CGDIWavePic::CGDIWavePic(HDC hdc, HBITMAP hBitmap, RECT *rc)
: CWavePic(rc->right - rc->left, rc->bottom - rc->top),
m_hMemDC(CreateCompatibleDC(hdc)), m_hBitmap(NULL), m_hOldBitmap(NULL),
m_pSrcColors(new unsigned int[Width()*Height()]), m_pvBits(NULL)
{
	int x = 0,y = 0;
	if (rc) {
		x = rc->left;
		y = rc->top;
	}

	BITMAPINFO bmi = {{0},{{0}}};

	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = Width();
	bmi.bmiHeader.biHeight = -Height();
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;		 // four 8-bit components
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight * 4;
	m_hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, static_cast<void **>(static_cast<void *>(&m_pvBits)), NULL, 0x0);

	m_hOldBitmap = (HBITMAP)SelectObject(m_hMemDC, m_hBitmap);


	MemDC_H hdcTemp(CreateCompatibleDC(hdc));
	HBITMAP hOld = (HBITMAP)SelectObject(hdcTemp, hBitmap);
	BitBlt(m_hMemDC	,0,0,Width(),Height(),hdcTemp,x,y,SRCCOPY);
	SelectObject(hdcTemp,hOld);

	int w = Width();
	int h = Height();

	COLORREF clr;
	ColourType * const pClr = m_pSrcColors.Get();
	for (int index = -1, j = 0; j < h; ++j) {
		for (int i = 0; i < w; ++i) {
			clr = GetPixel(m_hMemDC,i,j);
			pClr[++index] = ( GetRValue(clr) << 16 ) | ( GetGValue(clr) << 8 ) | GetBValue(clr);
		}
	}
}


CGDIWavePic::~CGDIWavePic(void)
{
	SelectObject(m_hMemDC,m_hOldBitmap);
	m_pvBits  = NULL;
}

//int CGDIWavePic::DrawWave(void *dst)
//{
//	return DrawWave((HDC)dst);
//}


//! 显视当前的波形到指定的 ＤＣ　的指定的位置， 并生成下一张图
int CGDIWavePic::DrawWave(HDC hdc, int x, int y, bool bDrawWithOutPrepare)
{
	if (hdc)
		BitBlt(hdc,x, y, Width(), Height(), m_hMemDC, 0, 0, SRCCOPY);
	if (bDrawWithOutPrepare || IsFlat())
		return 0;
	return PrePareWave();
}


//! 缩放显示当前的波形图， 并生成下一张图
int CGDIWavePic::DrawWave(HDC hdc, int dstLeft, int dstTop, int dstWidth, int dstHeight, bool bDrawWithOutPrepare)
{
	if (hdc) {
		StretchBlt(hdc, dstLeft, dstTop, dstWidth, dstHeight,
			m_hMemDC, 0,0,Width(),Height(),SRCCOPY);
	}
	if (bDrawWithOutPrepare || IsFlat())
		return 0;
	return PrePareWave();
}


//! 并生成下一张图
int CGDIWavePic::PrePareWave(void)
{
	NextWave();

	int cx = Width() + 2;

	int xOrg = 0;
	int yOrg = 0;
	int index = cx;
	int RowStart = 0;
	const int ciDarkerPower = 3;
	const unsigned int cDarkerValue = (1 << (8 - ciDarkerPower)) - 1;
	const unsigned int cDarkerFilter = cDarkerValue | (cDarkerValue << 8) | (cDarkerValue << 16);

	for(int j = 0; j < Height(); ++j) {
		for(int i = 0; i < Width(); ++i) {

			xOrg = i + ( (Depth(i + index + 2) - Depth(i + index)) >> 3 );
			yOrg = j + ( (Depth(i + index + 1 + cx) - Depth(i + index + 1 - cx)) >> 3 );

			if (xOrg >= 0 && xOrg < Width() && yOrg >= 0 && yOrg < Height()) {
				m_pvBits[i + RowStart] = m_pSrcColors[xOrg + yOrg * Width()];
			}
			else {
				m_pvBits[i + RowStart] -= (cDarkerFilter & (m_pvBits[i + RowStart] >> ciDarkerPower) );
				//m_pvBits[i + RowStart] &= 0x00e0e0e0;
			}
		}
		index += cx;
		RowStart += Width();
	}
	return 0;
}
