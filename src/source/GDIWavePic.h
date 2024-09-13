#ifndef LCH_GDI_WAVE_PIC_H
#define LCH_GDI_WAVE_PIC_H

#include "WavePic.h"
#include <auto_pointers.h>

class CGDIWavePic : public CWavePic
{
public:
	CGDIWavePic(HDC hdc, HBITMAP hBitmap, RECT *rc);
	CGDIWavePic(const TCHAR * pszFileName, RECT *rc);
	~CGDIWavePic(void);
	//int DrawWave(void *dst);
	int DrawWave(HDC hdc, bool bDrawWithOutPrepare = false) {
		return DrawWave(hdc, PosX(), PosY(), bDrawWithOutPrepare);
	}
	int DrawWave(HDC hdc, int x, int y, bool bDrawWithOutPrepare = false);
	int DrawWave(HDC hdc,
			  int dstLeft, int dstTop, int dstWidth, int dstHeight, bool bDrawWithOutPrepare = false);

private:
	int PrePareWave(void);

	MemDC_Handle m_hMemDC;
	gdi_ptr<HBITMAP> m_hBitmap;
	HBITMAP m_hOldBitmap;//save old
	typedef unsigned int ColourType;
	Arr<ColourType> m_pSrcColors;
	ColourType * m_pvBits;

	//Disable 
	CGDIWavePic(const CGDIWavePic &);
	CGDIWavePic & operator = (const CGDIWavePic &);
};

#endif //LCH_GDI_WAVE_PIC_H
