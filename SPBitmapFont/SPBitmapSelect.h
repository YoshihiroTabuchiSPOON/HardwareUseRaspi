#ifndef _SPBITMAPSELECT_H_
#define _SPBITMAPSELECT_H_

#include "SPBitmapFont.h"

namespace SPBitmapFont
{
class CSPBitmapSelect
{
public:
	CSPBitmapSelect();
	virtual ~CSPBitmapSelect();

	inline CSPBitmapElement * GetFirstBitmap()
	{
		return this->m_pFirstBitmap;
	}

	bool GetFonts( CSPBitmapFont & bitmapFont
					,const wchar_t * pcwszString, int iDefaultWidth, int iDefaultHeight );

protected:
	CSPBitmapElement * m_pFirstBitmap;
	CSPBitmapElement * m_pLastBitmap;

	void ReleaseAllElement();
	void AddElement( CSPBitmapElement * pElement );
	CSPBitmapElement * CreateDefaultChar(int iDefaultWidth, int iDefaultHeight);
};
}

#endif
