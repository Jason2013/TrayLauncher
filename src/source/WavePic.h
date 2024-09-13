#ifndef LCH_WAVE_PIC_H
#define LCH_WAVE_PIC_H

#include <auto_handle.h>
using wukong::Arr;

class CWavePic
{
public:
	typedef int TDepth;
	CWavePic(int w, int h);
	~CWavePic(void);
	int Drop(int x, int y, TDepth depth, int size = 0);
	void SetDrawPos(int x, int y){m_x = x; m_y = y; };
//	virtual int DrawWave(void *dst) = 0;
	bool IsFlat(void);
	void Flatten();
protected:
	//! 图象绘制的位置
	int PosX() const {return m_x;}; 
	int PosY() const {return m_y;};
	//! 图象的尺寸 （象素）
	int Width() const {return m_cx;};
	int Height() const {return m_cy;};

	void NextWave();
	const TDepth Depth(int index) const {return m_pSrfcPri[index];}
private :
	int m_cx;
	int m_cy;
	int m_x;
	int m_y;

	Arr<TDepth> m_pSrfcPri;
	Arr<TDepth> m_pSrfcSec;
	bool m_bFlat;

	CWavePic(const CWavePic &);
	CWavePic & operator = (const CWavePic &);
};

#endif // LCH_WAVE_PIC_H
