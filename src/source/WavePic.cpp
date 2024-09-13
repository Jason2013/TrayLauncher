
#include "WavePic.h"

CWavePic::CWavePic(int w, int h):m_cx(w), m_cy(h), m_x(0), m_y(0),
m_pSrfcPri(new TDepth[(m_cx+2)*(m_cy+2)]), m_pSrfcSec(new TDepth[(m_cx+2)*(m_cy+2)]), m_bFlat(true)
{
	Flatten();
}


CWavePic::~CWavePic(void)
{
}

//////////////////////// common \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\//

int CWavePic::Drop(int x, int y, TDepth depth, int size/* = 0 */)
{
	if (size <= 1) {
		x -= m_x;
		y -= m_y;
		TDepth *pPoint = &m_pSrfcPri[(y + 1)*(m_cx+2) + x + 1];
		if (depth != 0 && x >= 0 && x < m_cx && y >= 0 && y < m_cy) {
			*pPoint = -depth;
			m_bFlat = false;
		}
	}
	else {
		x -= size/2;
		y -= size/2;
		for (int i = 0; i < size; ++i) {
			for (int j = 0; j < size; ++j) {
				Drop(x + i, y + j, depth, 1);
			}
		}
	}
	return depth;
}


void CWavePic::Flatten(void)
{
	memset(m_pSrfcPri.Get(), 0, (m_cx+2)*(m_cy+2)*sizeof(TDepth));
	memset(m_pSrfcSec.Get(), 0, (m_cx+2)*(m_cy+2)*sizeof(TDepth));
	m_bFlat = true;
}


bool CWavePic::IsFlat(void)
{
	if (m_bFlat)
		return true;
	int n = (m_cx+2)*(m_cy+2-1);
	int i=m_cx+2;
	while(i<n && !m_pSrfcPri[i]) ++i;
	return m_bFlat = (i >= n);
}

//! 波动形状计算，变化每个点的深度
void CWavePic::NextWave()
{
	int cx = m_cx + 2;
	const TDepth * const pPri = m_pSrfcPri.Get();
	TDepth * const pSec = m_pSrfcSec.Get();
	for (int j = 0, index = 0; j < m_cy; ++j) {
		index += cx;
		for (int i = index + 1; i < index + 1 + m_cx; ++i) {
			// 扩散
			pSec[i] = ( ( (pPri[i-cx] + pPri[i+cx] + pPri[i-1] + pPri[i+1])*13 +
						  (pPri[i-cx-1] + pPri[i+cx+1] + pPri[i+cx-1] + pPri[i-cx+1])*3
						) >> 5) - pSec[i];
			// 衰减2^n分之1
			pSec[i] -= (pSec[i] >> 5);
		}
	}
	m_pSrfcSec.Swap(m_pSrfcPri);
}

